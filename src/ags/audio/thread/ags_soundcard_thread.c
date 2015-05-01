/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_timestamp_thread.h>

void ags_soundcard_thread_class_init(AgsSoundcardThreadClass *soundcard_thread);
void ags_soundcard_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_soundcard_thread_init(AgsSoundcardThread *soundcard_thread);
void ags_soundcard_thread_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_soundcard_thread_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_soundcard_thread_connect(AgsConnectable *connectable);
void ags_soundcard_thread_disconnect(AgsConnectable *connectable);
void ags_soundcard_thread_finalize(GObject *gobject);

void ags_soundcard_thread_start(AgsThread *thread);
void ags_soundcard_thread_run(AgsThread *thread);
void ags_soundcard_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_soundcard_thread
 * @short_description: soundcard thread
 * @title: AgsSoundcardThread
 * @section_id:
 * @include: ags/thread/ags_soundcard_thread.h
 *
 * The #AgsSoundcardThread acts as audio output thread to soundcard.
 */

static gpointer ags_soundcard_thread_parent_class = NULL;
static AgsConnectableInterface *ags_soundcard_thread_parent_connectable_interface;

enum{
  PROP_0,
  PROP_SOUNDCARD,
};

GType
ags_soundcard_thread_get_type()
{
  static GType ags_type_soundcard_thread = 0;

  if(!ags_type_soundcard_thread){
    static const GTypeInfo ags_soundcard_thread_info = {
      sizeof (AgsSoundcardThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_soundcard_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSoundcardThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_soundcard_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_soundcard_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_soundcard_thread = g_type_register_static(AGS_TYPE_THREAD,
						       "AgsSoundcardThread\0",
						       &ags_soundcard_thread_info,
						       0);
    
    g_type_add_interface_static(ags_type_soundcard_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_soundcard_thread);
}

void
ags_soundcard_thread_class_init(AgsSoundcardThreadClass *soundcard_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;
  
  ags_soundcard_thread_parent_class = g_type_class_peek_parent(soundcard_thread);

  /* GObject */
  gobject = (GObjectClass *) soundcard_thread;

  gobject->set_property = ags_soundcard_thread_set_property;
  gobject->get_property = ags_soundcard_thread_get_property;

  gobject->finalize = ags_soundcard_thread_finalize;

  /**
   * AgsThread:soundcard:
   *
   * The assigned #AgsSoundcard.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard assigned to\0",
				   "The AgsSoundcard it is assigned to.\0",
				   G_TYPE_OBJECT,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) soundcard_thread;

  thread->start = ags_soundcard_thread_start;
  thread->run = ags_soundcard_thread_run;
  thread->stop = ags_soundcard_thread_stop;
}

void
ags_soundcard_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_soundcard_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_soundcard_thread_connect;
  connectable->disconnect = ags_soundcard_thread_disconnect;
}

void
ags_soundcard_thread_init(AgsSoundcardThread *soundcard_thread)
{
  AgsThread *thread;
  
  thread = AGS_THREAD(soundcard_thread);
  
  thread->freq = AGS_SOUNDCARD_THREAD_DEFAULT_JIFFIE;

  soundcard_thread->soundcard = NULL;

  soundcard_thread->timestamp_thread = ags_timestamp_thread_new();
  ags_thread_add_child(thread, soundcard_thread->timestamp_thread);

  soundcard_thread->error = NULL;
}

void
ags_soundcard_thread_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      AgsSoundcard *soundcard;

      soundcard = (AgsSoundcard *) g_value_get_object(value);

      if(soundcard_thread->soundcard != NULL){
	g_object_unref(G_OBJECT(soundcard_thread->soundcard));
      }

      if(soundcard != NULL){
	g_object_ref(G_OBJECT(soundcard));
      }

      soundcard_thread->soundcard = G_OBJECT(soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_soundcard_thread_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, G_OBJECT(soundcard_thread->soundcard));
    }
    break;
  }
}

void
ags_soundcard_thread_connect(AgsConnectable *connectable)
{
  ags_soundcard_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_soundcard_thread_disconnect(AgsConnectable *connectable)
{
  ags_soundcard_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_soundcard_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_soundcard_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_soundcard_thread_start(AgsThread *thread)
{
  AgsSoundcard *soundcard;
  AgsSoundcardThread *soundcard_thread;
  static gboolean initialized = FALSE;
  GError *error;

  soundcard_thread = AGS_SOUNDCARD_THREAD(thread);

  soundcard = AGS_SOUNDCARD(soundcard_thread->soundcard);

  /*  */
  soundcard_thread->error = NULL;

  if(ags_soundcard_get_buffer(soundcard) == NULL){
    ags_soundcard_play_init(soundcard,
			    &(soundcard_thread->error));
      
#ifdef AGS_DEBUG
    g_message("ags_devout_alsa_play\0");
#endif
  }

  if((AGS_THREAD_SINGLE_LOOP & (g_atomic_int_get(&(thread->flags)))) == 0){
    AGS_THREAD_CLASS(ags_soundcard_thread_parent_class)->start(thread);
  }
}

void
ags_soundcard_thread_run(AgsThread *thread)
{
  AgsSoundcard *soundcard;
  AgsSoundcardThread *soundcard_thread;
  long delay;
  GError *error;

  soundcard_thread = AGS_SOUNDCARD_THREAD(thread);

  soundcard = AGS_SOUNDCARD(soundcard_thread->soundcard);

  error = NULL;
  ags_soundcard_play(soundcard,
		     &error);

  if(error != NULL){
    //TODO:JK: implement me
  }
}

void
ags_soundcard_thread_stop(AgsThread *thread)
{
  AgsSoundcard *soundcard;
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = AGS_SOUNDCARD_THREAD(thread);

  soundcard = AGS_SOUNDCARD(soundcard_thread->soundcard);

  AGS_THREAD_CLASS(ags_soundcard_thread_parent_class)->stop(thread);
  ags_soundcard_stop(soundcard);
}

/**
 * ags_soundcard_thread_new:
 * @soundcard: the #AgsSoundcard
 *
 * Create a new #AgsSoundcardThread.
 *
 * Returns: the new #AgsSoundcardThread
 *
 * Since: 0.4
 */
AgsSoundcardThread*
ags_soundcard_thread_new(GObject *soundcard)
{
  AgsSoundcardThread *soundcard_thread;

  soundcard_thread = (AgsSoundcardThread *) g_object_new(AGS_TYPE_SOUNDCARD_THREAD,
							 "soundcard\0", soundcard,
							 NULL);


  return(soundcard_thread);
}
