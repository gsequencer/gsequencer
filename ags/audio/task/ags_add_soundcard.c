/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/task/ags_add_soundcard.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_sound_provider.h>

void ags_add_soundcard_class_init(AgsAddSoundcardClass *add_soundcard);
void ags_add_soundcard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_add_soundcard_init(AgsAddSoundcard *add_soundcard);
void ags_add_soundcard_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_add_soundcard_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_add_soundcard_connect(AgsConnectable *connectable);
void ags_add_soundcard_disconnect(AgsConnectable *connectable);
void ags_add_soundcard_finalize(GObject *gobject);

void ags_add_soundcard_launch(AgsTask *task);

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_SOUNDCARD,
};

/**
 * SECTION:ags_add_soundcard
 * @short_description: add soundcard object to context
 * @title: AgsAddSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_add_soundcard.h
 *
 * The #AgsAddSoundcard task adds #AgsSoundcard to context.
 */

static gpointer ags_add_soundcard_parent_class = NULL;
static AgsConnectableInterface *ags_add_soundcard_parent_connectable_interface;

GType
ags_add_soundcard_get_type()
{
  static GType ags_type_add_soundcard = 0;

  if(!ags_type_add_soundcard){
    static const GTypeInfo ags_add_soundcard_info = {
      sizeof (AgsAddSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_add_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAddSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_add_soundcard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_add_soundcard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_add_soundcard = g_type_register_static(AGS_TYPE_TASK,
						    "AgsAddSoundcard\0",
						    &ags_add_soundcard_info,
						    0);

    g_type_add_interface_static(ags_type_add_soundcard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_add_soundcard);
}

void
ags_add_soundcard_class_init(AgsAddSoundcardClass *add_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_add_soundcard_parent_class = g_type_class_peek_parent(add_soundcard);

  /* gobject */
  gobject = (GObjectClass *) add_soundcard;

  gobject->set_property = ags_add_soundcard_set_property;
  gobject->get_property = ags_add_soundcard_get_property;

  gobject->finalize = ags_add_soundcard_finalize;

  /**
   * AgsAddSoundcard:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("application-context\0",
				   "application context of add soundcard\0",
				   "The application context of add soundcard task\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsAddSoundcard:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("soundcard\0",
				   "soundcard of add soundcard\0",
				   "The soundcard of add soundcard task\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) add_soundcard;

  task->launch = ags_add_soundcard_launch;
}

void
ags_add_soundcard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_add_soundcard_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_add_soundcard_connect;
  connectable->disconnect = ags_add_soundcard_disconnect;
}

void
ags_add_soundcard_init(AgsAddSoundcard *add_soundcard)
{
  add_soundcard->application_context = NULL;
  
  add_soundcard->soundcard = NULL;
}

void
ags_add_soundcard_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = AGS_ADD_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(add_soundcard->application_context == (GObject *) application_context){
	return;
      }

      if(add_soundcard->application_context != NULL){
	g_object_unref(add_soundcard->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      add_soundcard->application_context = (GObject *) application_context;
    }
    break;
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(add_soundcard->soundcard == (GObject *) soundcard){
	return;
      }

      if(add_soundcard->soundcard != NULL){
	g_object_unref(add_soundcard->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      add_soundcard->soundcard = (GObject *) soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_soundcard_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = AGS_ADD_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, add_soundcard->application_context);
    }
    break;
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, add_soundcard->soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_add_soundcard_connect(AgsConnectable *connectable)
{
  ags_add_soundcard_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_add_soundcard_disconnect(AgsConnectable *connectable)
{
  ags_add_soundcard_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_add_soundcard_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_add_soundcard_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_add_soundcard_launch(AgsTask *task)
{
  AgsAddSoundcard *add_soundcard;
  
  add_soundcard = AGS_ADD_SOUNDCARD(task);

  ags_sound_provider_set_soundcard(AGS_SOUND_PROVIDER(add_soundcard->application_context),
				   g_list_append(ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(add_soundcard->application_context)),
						 add_soundcard->soundcard));
}

/**
 * ags_add_soundcard_new:
 * @application_context: the #AgsApplicationContext
 * @soundcard: the #AgsSoundcard to add
 *
 * Creates an #AgsAddSoundcard.
 *
 * Returns: an new #AgsAddSoundcard.
 *
 * Since: 0.7.65
 */
AgsAddSoundcard*
ags_add_soundcard_new(AgsApplicationContext *application_context,
		      GObject *soundcard)
{
  AgsAddSoundcard *add_soundcard;

  add_soundcard = (AgsAddSoundcard *) g_object_new(AGS_TYPE_ADD_SOUNDCARD,
						   NULL);

  add_soundcard->application_context = application_context;

  add_soundcard->soundcard = soundcard;

  return(add_soundcard);
}
