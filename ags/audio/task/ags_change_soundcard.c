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

#include <ags/audio/task/ags_change_soundcard.h>

#include <ags/object/ags_connection_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_connection.h>

void ags_change_soundcard_class_init(AgsChangeSoundcardClass *change_soundcard);
void ags_change_soundcard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_change_soundcard_init(AgsChangeSoundcard *change_soundcard);
void ags_change_soundcard_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_change_soundcard_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_change_soundcard_connect(AgsConnectable *connectable);
void ags_change_soundcard_disconnect(AgsConnectable *connectable);
void ags_change_soundcard_finalize(GObject *gobject);

void ags_change_soundcard_launch(AgsTask *task);

/**
 * SECTION:ags_change_soundcard
 * @short_description: change soundcard
 * @title: AgsChangeSoundcard
 * @section_id:
 * @include: ags/audio/task/ags_change_soundcard.h
 *
 * The #AgsChangeSoundcard task change the specified soundcard.
 */

static gpointer ags_change_soundcard_parent_class = NULL;
static AgsConnectableInterface *ags_change_soundcard_parent_connectable_interface;

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_NEW_SOUNDCARD,
  PROP_OLD_SOUNDCARD,
};

GType
ags_change_soundcard_get_type()
{
  static GType ags_type_change_soundcard = 0;

  if(!ags_type_change_soundcard){
    static const GTypeInfo ags_change_soundcard_info = {
      sizeof (AgsChangeSoundcardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_change_soundcard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChangeSoundcard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_change_soundcard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_change_soundcard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_change_soundcard = g_type_register_static(AGS_TYPE_TASK,
						       "AgsChangeSoundcard",
						       &ags_change_soundcard_info,
						       0);

    g_type_add_interface_static(ags_type_change_soundcard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_change_soundcard);
}

void
ags_change_soundcard_class_init(AgsChangeSoundcardClass *change_soundcard)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_change_soundcard_parent_class = g_type_class_peek_parent(change_soundcard);

  /* GObjectClass */
  gobject = (GObjectClass *) change_soundcard;

  gobject->set_property = ags_change_soundcard_set_property;
  gobject->get_property = ags_change_soundcard_get_property;

  gobject->finalize = ags_change_soundcard_finalize;

  /* properties */
  /**
   * AgsChangeSoundcard:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("application-context",
				   "application context of change soundcard",
				   "The application context of change soundcard task",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsChangeSoundcard:new-soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("new-soundcard",
				   "new soundcard of change soundcard",
				   "The new soundcard of change soundcard task",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NEW_SOUNDCARD,
				  param_spec);

  /**
   * AgsChangeSoundcard:old-soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("old-soundcard",
				   "old soundcard of change soundcard",
				   "The old soundcard of change soundcard task",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OLD_SOUNDCARD,
				  param_spec);
  
  /* AgsTaskClass */
  task = (AgsTaskClass *) change_soundcard;

  task->launch = ags_change_soundcard_launch;
}

void
ags_change_soundcard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_change_soundcard_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_change_soundcard_connect;
  connectable->disconnect = ags_change_soundcard_disconnect;
}

void
ags_change_soundcard_init(AgsChangeSoundcard *change_soundcard)
{
  change_soundcard->application_context = NULL;
  
  change_soundcard->new_soundcard = NULL;
  change_soundcard->old_soundcard = NULL;
}

void
ags_change_soundcard_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsChangeSoundcard *change_soundcard;

  change_soundcard = AGS_CHANGE_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(change_soundcard->application_context == (GObject *) application_context){
	return;
      }

      if(change_soundcard->application_context != NULL){
	g_object_unref(change_soundcard->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      change_soundcard->application_context = (GObject *) application_context;
    }
    break;
  case PROP_NEW_SOUNDCARD:
    {
      GObject *new_soundcard;

      new_soundcard = (GObject *) g_value_get_object(value);

      if(change_soundcard->new_soundcard == (GObject *) new_soundcard){
	return;
      }

      if(change_soundcard->new_soundcard != NULL){
	g_object_unref(change_soundcard->new_soundcard);
      }

      if(new_soundcard != NULL){
	g_object_ref(new_soundcard);
      }

      change_soundcard->new_soundcard = (GObject *) new_soundcard;
    }
    break;
  case PROP_OLD_SOUNDCARD:
    {
      GObject *old_soundcard;

      old_soundcard = (GObject *) g_value_get_object(value);

      if(change_soundcard->old_soundcard == (GObject *) old_soundcard){
	return;
      }

      if(change_soundcard->old_soundcard != NULL){
	g_object_unref(change_soundcard->old_soundcard);
      }

      if(old_soundcard != NULL){
	g_object_ref(old_soundcard);
      }

      change_soundcard->old_soundcard = (GObject *) old_soundcard;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_change_soundcard_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsChangeSoundcard *change_soundcard;

  change_soundcard = AGS_CHANGE_SOUNDCARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, change_soundcard->application_context);
    }
    break;
  case PROP_NEW_SOUNDCARD:
    {
      g_value_set_object(value, change_soundcard->new_soundcard);
    }
    break;
  case PROP_OLD_SOUNDCARD:
    {
      g_value_set_object(value, change_soundcard->old_soundcard);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_change_soundcard_connect(AgsConnectable *connectable)
{
  ags_change_soundcard_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_change_soundcard_disconnect(AgsConnectable *connectable)
{
  ags_change_soundcard_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_change_soundcard_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_change_soundcard_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_change_soundcard_launch(AgsTask *task)
{
  AgsChangeSoundcard *change_soundcard;

  AgsAudioConnection *audio_connection;
  
  AgsConnectionManager *connection_manager;
  
  GObject *new_soundcard, *old_soundcard;

  GType soundcard_type;

  GParameter *parameter;
  GList *list, *old_audio, *new_audio;
  
  change_soundcard = AGS_CHANGE_SOUNDCARD(task);

  connection_manager = ags_connection_manager_get_instance();

  new_soundcard = change_soundcard->new_soundcard;
  old_soundcard = change_soundcard->old_soundcard;
  
  list = ags_connection_manager_get_connection(connection_manager);
  soundcard_type = G_OBJECT_TYPE(old_soundcard);

  /* move audio */
  new_audio = ags_soundcard_get_audio(AGS_SOUNDCARD(new_soundcard));
  old_audio = ags_soundcard_get_audio(AGS_SOUNDCARD(old_soundcard));
  
  if(old_audio != NULL){
    if(new_audio != NULL){
      ags_soundcard_set_audio(AGS_SOUNDCARD(new_soundcard),
			      g_list_concat(old_audio,
					    new_audio));
    }else{
      ags_soundcard_set_audio(AGS_SOUNDCARD(new_soundcard),
			      old_audio);
    }
  }
  
  ags_soundcard_set_audio(AGS_SOUNDCARD(old_soundcard),
			  NULL);
  
  /* reset soundcard */
  parameter = (GParameter *) g_new0(GParameter,
				    1);

  parameter[0].name = "soundcard";
  
  g_value_init(&(parameter[0].value),
	       G_TYPE_OBJECT);
  g_value_set_object(&(parameter[0].value),
		     new_soundcard);
  
  while(old_audio != NULL){
    ags_audio_recursive_set_property(old_audio->data,
				     parameter, 1);

    old_audio = old_audio->next;
  }

  /* reset audio connection */
  while((list = ags_connection_find_type_and_data_object_type(list,
							      AGS_TYPE_AUDIO_CONNECTION,
							      soundcard_type)) != NULL){
    GObject *data_object;

    audio_connection = AGS_AUDIO_CONNECTION(list->data);
    
    g_object_get(G_OBJECT(list->data),
		 "data-object", &data_object,
		 NULL);
	    
    if(AGS_IS_SOUNDCARD(data_object)){
      g_object_set(audio_connection,
		   "data-object", new_soundcard,
		   NULL);
    }

    list = list->next;
  }
}

/**
 * ags_change_soundcard_new:
 * @application_context: the #AgsApplicationContext
 * @new_soundcard: the #AgsSoundcard to set
 * @old_soundcard: the #AgsSoundcard to unset
 *
 * Creates an #AgsChangeSoundcard.
 *
 * Returns: an new #AgsChangeSoundcard.
 *
 * Since: 0.7.65
 */
AgsChangeSoundcard*
ags_change_soundcard_new(AgsApplicationContext *application_context,
			 GObject *new_soundcard,
			 GObject *old_soundcard)
{
  AgsChangeSoundcard *change_soundcard;

  change_soundcard = (AgsChangeSoundcard *) g_object_new(AGS_TYPE_CHANGE_SOUNDCARD,
							 NULL);

  change_soundcard->application_context = application_context;
  
  change_soundcard->new_soundcard = new_soundcard;
  change_soundcard->old_soundcard = old_soundcard;
  
  return(change_soundcard);
}
