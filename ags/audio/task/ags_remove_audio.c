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

#include <ags/audio/task/ags_remove_audio.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/i18n.h>

void ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio);
void ags_remove_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_audio_init(AgsRemoveAudio *remove_audio);
void ags_remove_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_remove_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_remove_audio_connect(AgsConnectable *connectable);
void ags_remove_audio_disconnect(AgsConnectable *connectable);
void ags_remove_audio_finalize(GObject *gobject);

void ags_remove_audio_launch(AgsTask *task);

/**
 * SECTION:ags_remove_audio
 * @short_description: remove audio of soundcard task
 * @title: AgsRemoveAudio
 * @section_id:
 * @include: ags/audio/task/ags_remove_audio.h
 *
 * The #AgsRemoveAudio task removes #AgsAudio of #AgsSoundcard.
 */

static gpointer ags_remove_audio_parent_class = NULL;
static AgsConnectableInterface *ags_remove_audio_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SOUNDCARD,
};

GType
ags_remove_audio_get_type()
{
  static GType ags_type_remove_audio = 0;

  if(!ags_type_remove_audio){
    static const GTypeInfo ags_remove_audio_info = {
      sizeof (AgsRemoveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsRemoveAudio",
						   &ags_remove_audio_info,
						   0);
    
    g_type_add_interface_static(ags_type_remove_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_audio);
}

void
ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;

  ags_remove_audio_parent_class = g_type_class_peek_parent(remove_audio);

  /* gobject */
  gobject = (GObjectClass *) remove_audio;

  gobject->set_property = ags_remove_audio_set_property;
  gobject->get_property = ags_remove_audio_get_property;

  gobject->finalize = ags_remove_audio_finalize;
  
  /* properties */
  /**
   * AgsRemoveAudio:soundcard:
   *
   * The assigned #AgsSoundcard
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("soundcard",
				   i18n_pspec("soundcard of remove audio"),
				   i18n_pspec("The soundcard of remove audio task"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUNDCARD,
				  param_spec);

  /**
   * AgsRemoveAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of remove audio"),
				   i18n_pspec("The audio of remove audio task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) remove_audio;

  task->launch = ags_remove_audio_launch;
}

void
ags_remove_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_audio_connect;
  connectable->disconnect = ags_remove_audio_disconnect;
}

void
ags_remove_audio_init(AgsRemoveAudio *remove_audio)
{
  remove_audio->soundcard = NULL;
  remove_audio->audio = NULL;
}

void
ags_remove_audio_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = AGS_REMOVE_AUDIO(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      GObject *soundcard;

      soundcard = (GObject *) g_value_get_object(value);

      if(remove_audio->soundcard == (GObject *) soundcard){
	return;
      }

      if(remove_audio->soundcard != NULL){
	g_object_unref(remove_audio->soundcard);
      }

      if(soundcard != NULL){
	g_object_ref(soundcard);
      }

      remove_audio->soundcard = (GObject *) soundcard;
    }
    break;
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(remove_audio->audio == (GObject *) audio){
	return;
      }

      if(remove_audio->audio != NULL){
	g_object_unref(remove_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      remove_audio->audio = (GObject *) audio;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_audio_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = AGS_REMOVE_AUDIO(gobject);

  switch(prop_id){
  case PROP_SOUNDCARD:
    {
      g_value_set_object(value, remove_audio->soundcard);
    }
    break;
  case PROP_AUDIO:
    {
      g_value_set_object(value, remove_audio->audio);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_remove_audio_connect(AgsConnectable *connectable)
{
  ags_remove_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_audio_disconnect(AgsConnectable *connectable)
{
  ags_remove_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_audio_launch(AgsTask *task)
{
  AgsRemoveAudio *remove_audio;
  AgsChannel *current;

  GList *list;
  
  remove_audio = AGS_REMOVE_AUDIO(task);

  current = remove_audio->audio->output;

  while(current != NULL){
    ags_channel_set_link(current,
			 NULL,
			 NULL);
    
    current = current->next;
  }

  current = remove_audio->audio->input;

  while(current != NULL){
    ags_channel_set_link(current,
			 NULL,
			 NULL);
    
    current = current->next;
  }
  
  /* remove audio */
  list = ags_soundcard_get_audio(AGS_SOUNDCARD(remove_audio->soundcard));
  list = g_list_remove(list,
		       remove_audio->audio);
  ags_soundcard_set_audio(AGS_SOUNDCARD(remove_audio->soundcard),
			  list);

  g_object_run_dispose(remove_audio->audio);
  g_object_unref(remove_audio->audio);
}

/**
 * ags_remove_audio_new:
 * @soundcard: the #GObject implementing #AgsSoundcard
 * @audio: the #AgsAudio to remove
 *
 * Creates an #AgsRemoveAudio.
 *
 * Returns: an new #AgsRemoveAudio.
 *
 * Since: 0.4
 */
AgsRemoveAudio*
ags_remove_audio_new(GObject *soundcard,
		     AgsAudio *audio)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = (AgsRemoveAudio *) g_object_new(AGS_TYPE_REMOVE_AUDIO,
						 NULL);

  remove_audio->soundcard = soundcard;
  remove_audio->audio = audio;

  return(remove_audio);
}
