/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/thread/ags_autosave_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

void ags_autosave_thread_class_init(AgsAutosaveThreadClass *autosave_thread);
void ags_autosave_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_autosave_thread_init(AgsAutosaveThread *autosave_thread);
void ags_autosave_thread_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_autosave_thread_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_autosave_thread_connect(AgsConnectable *connectable);
void ags_autosave_thread_disconnect(AgsConnectable *connectable);
void ags_autosave_thread_finalize(GObject *gobject);

void ags_autosave_thread_start(AgsThread *thread);
void ags_autosave_thread_run(AgsThread *thread);
void ags_autosave_thread_stop(AgsThread *thread);

enum{
  PROP_0,
  PROP_AGS_MAIN,
};

static gpointer ags_autosave_thread_parent_class = NULL;
static AgsConnectableInterface *ags_autosave_thread_parent_connectable_interface;

GType
ags_autosave_thread_get_type()
{
  static GType ags_type_autosave_thread = 0;

  if(!ags_type_autosave_thread){
    static const GTypeInfo ags_autosave_thread_info = {
      sizeof (AgsAutosaveThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_autosave_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutosaveThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_autosave_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_autosave_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_autosave_thread = g_type_register_static(AGS_TYPE_THREAD,
						      "AgsAutosaveThread\0",
						      &ags_autosave_thread_info,
						      0);
    
    g_type_add_interface_static(ags_type_autosave_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_autosave_thread);
}

void
ags_autosave_thread_class_init(AgsAutosaveThreadClass *autosave_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;

  ags_autosave_thread_parent_class = g_type_class_peek_parent(autosave_thread);

  /* GObject */
  gobject = (GObjectClass *) autosave_thread;

  gobject->get_property = ags_autosave_thread_get_property;
  gobject->set_property = ags_autosave_thread_set_property;

  gobject->finalize = ags_autosave_thread_finalize;

  /* properties */
  param_spec = g_param_spec_object("ags_main\0",
				   "ags_main to check against\0",
				   "The ags_main to check against serialization.\0",
				   AGS_TYPE_MAIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AGS_MAIN,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) autosave_thread;

  thread->start = ags_autosave_thread_start;
  thread->run = ags_autosave_thread_run;
  thread->stop = ags_autosave_thread_stop;
}

void
ags_autosave_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_autosave_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_autosave_thread_connect;
  connectable->disconnect = ags_autosave_thread_disconnect;
}

void
ags_autosave_thread_init(AgsAutosaveThread *autosave_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(autosave_thread);

  autosave_thread->ags_main = NULL;

  autosave_thread->delay = AGS_AUTOSAVE_THREAD_DEFAULT_DELAY;
  autosave_thread->counter = 0;
}

void
ags_autosave_thread_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_AGS_MAIN:
    {
      AgsMain *ags_main;

      ags_main = g_value_get_object(value);

      if(autosave_thread->ags_main == ags_main){
	return;
      }

      if(autosave_thread->ags_main != NULL){
	g_object_unref(autosave_thread->ags_main);
      }

      if(ags_main != NULL){
	g_object_ref(ags_main);
      }

      autosave_thread->ags_main = ags_main;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_autosave_thread_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(gobject);

  switch(prop_id){
  case PROP_AGS_MAIN:
    {
      g_value_set_object(value, autosave_thread->ags_main);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_autosave_thread_connect(AgsConnectable *connectable)
{
  ags_autosave_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_autosave_thread_disconnect(AgsConnectable *connectable)
{
  ags_autosave_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_autosave_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_autosave_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_autosave_thread_start(AgsThread *thread)
{
  //TODO:JK: implement me

  AGS_THREAD_CLASS(ags_autosave_thread_parent_class)->start(thread);
}

void
ags_autosave_thread_run(AgsThread *thread)
{
  static const struct timespec delay = {
    0,
    NSEC_PER_SEC / 2,
  };

  AgsAutosaveThread *autosave_thread;

  autosave_thread = AGS_AUTOSAVE_THREAD(thread);
  
  if(autosave_thread->counter != autosave_thread->delay){
    autosave_thread->counter += 1;
  }else{
    AgsFile *file;

    autosave_thread->counter = 0;
    
    file = ags_file_new();
    g_object_set(G_OBJECT(file),
		 "ags-main\0", autosave_thread->ags_main,
		 "filename\0", g_strdup(AGS_AUTOSAVE_THREAD_DEFAULT_FILENAME),
		 NULL);
    ags_file_write(file);
    g_object_unref(file);
  }

  nanosleep(&delay, NULL);
}

void
ags_autosave_thread_stop(AgsThread *thread)
{
  //TODO:JK: implement me
}

AgsAutosaveThread*
ags_autosave_thread_new(GObject *devout, AgsMain *ags_main)
{
  AgsAutosaveThread *autosave_thread;

  autosave_thread = (AgsAutosaveThread *) g_object_new(AGS_TYPE_AUTOSAVE_THREAD,
						       "devout\0", devout,
						       "ags-main\0", ags_main,
						       NULL);

  return(autosave_thread);
}
