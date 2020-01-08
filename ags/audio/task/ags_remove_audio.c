/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/i18n.h>

void ags_remove_audio_class_init(AgsRemoveAudioClass *remove_audio);
void ags_remove_audio_init(AgsRemoveAudio *remove_audio);
void ags_remove_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_remove_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_remove_audio_dispose(GObject *gobject);
void ags_remove_audio_finalize(GObject *gobject);

void ags_remove_audio_launch(AgsTask *task);

/**
 * SECTION:ags_remove_audio
 * @short_description: remove audio of application context
 * @title: AgsRemoveAudio
 * @section_id:
 * @include: ags/audio/task/ags_remove_audio.h
 *
 * The #AgsRemoveAudio task removes #AgsAudio of #AgsApplicationContext.
 */

static gpointer ags_remove_audio_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
};

GType
ags_remove_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_remove_audio = 0;

    static const GTypeInfo ags_remove_audio_info = {
      sizeof(AgsRemoveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsRemoveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_audio_init,
    };

    ags_type_remove_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsRemoveAudio",
						   &ags_remove_audio_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_remove_audio);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_remove_audio_dispose;
  gobject->finalize = ags_remove_audio_finalize;
  
  /* properties */
  /**
   * AgsRemoveAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
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
ags_remove_audio_init(AgsRemoveAudio *remove_audio)
{
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
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(remove_audio->audio == audio){
	return;
      }

      if(remove_audio->audio != NULL){
	g_object_unref(remove_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      remove_audio->audio = audio;
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
ags_remove_audio_dispose(GObject *gobject)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = AGS_REMOVE_AUDIO(gobject);

  if(remove_audio->audio != NULL){
    g_object_unref(remove_audio->audio);

    remove_audio->audio = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_remove_audio_parent_class)->dispose(gobject);
}

void
ags_remove_audio_finalize(GObject *gobject)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = AGS_REMOVE_AUDIO(gobject);

  if(remove_audio->audio != NULL){
    g_object_unref(remove_audio->audio);
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_remove_audio_parent_class)->finalize(gobject);
}

void
ags_remove_audio_launch(AgsTask *task)
{
  AgsRemoveAudio *remove_audio;

  AgsApplicationContext *application_context;
  
  GList *start_list, *list;
  
  remove_audio = AGS_REMOVE_AUDIO(task);

  application_context = ags_application_context_get_instance();
  
  if(!AGS_IS_SOUND_PROVIDER(application_context) ||
     !AGS_IS_AUDIO(remove_audio->audio)){
    return;
  }

  /* remove audio */
  list =
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  if(g_list_find(list,
		 remove_audio->audio) != NULL){
    /* remove to sound provider */
    list = g_list_remove(list,
			 remove_audio->audio);
    ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
				 list);

    /* AgsAudio */
    ags_connectable_disconnect(AGS_CONNECTABLE(remove_audio->audio));

    g_object_unref(remove_audio->audio);
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_remove_audio_new:
 * @audio: the #AgsAudio to remove
 *
 * Create a new instance of #AgsRemoveAudio.
 *
 * Returns: the new #AgsRemoveAudio
 *
 * Since: 3.0.0
 */
AgsRemoveAudio*
ags_remove_audio_new(AgsAudio *audio)
{
  AgsRemoveAudio *remove_audio;

  remove_audio = (AgsRemoveAudio *) g_object_new(AGS_TYPE_REMOVE_AUDIO,
						 "audio", audio,
						 NULL);

  return(remove_audio);
}
