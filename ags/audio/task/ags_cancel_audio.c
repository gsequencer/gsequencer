/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/task/ags_cancel_audio.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_cancel_audio_class_init(AgsCancelAudioClass *cancel_audio);
void ags_cancel_audio_init(AgsCancelAudio *cancel_audio);
void ags_cancel_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_cancel_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_cancel_audio_dispose(GObject *gobject);
void ags_cancel_audio_finalize(GObject *gobject);

void ags_cancel_audio_launch(AgsTask *task);

/**
 * SECTION:ags_cancel_audio
 * @short_description: cancel audio task
 * @title: AgsCancelAudio
 * @section_id:
 * @include: ags/audio/task/ags_cancel_audio.h
 *
 * The #AgsCancelAudio task cancels #AgsAudio playback.
 */

static gpointer ags_cancel_audio_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SOUND_SCOPE,
};

GType
ags_cancel_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_cancel_audio = 0;

    static const GTypeInfo ags_cancel_audio_info = {
      sizeof(AgsCancelAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCancelAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_audio_init,
    };

    ags_type_cancel_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsCancelAudio",
						   &ags_cancel_audio_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_cancel_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_cancel_audio_class_init(AgsCancelAudioClass *cancel_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_cancel_audio_parent_class = g_type_class_peek_parent(cancel_audio);

  /* gobject */
  gobject = (GObjectClass *) cancel_audio;

  gobject->set_property = ags_cancel_audio_set_property;
  gobject->get_property = ags_cancel_audio_get_property;

  gobject->dispose = ags_cancel_audio_dispose;
  gobject->finalize = ags_cancel_audio_finalize;

  /* properties */
  /**
   * AgsCancelAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of cancel audio"),
				   i18n_pspec("The audio of cancel audio task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsCancelAudio:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("sound-scope",
				i18n_pspec("sound scope"),
				i18n_pspec("The sound scope"),
				 -1,
				AGS_SOUND_SCOPE_LAST,
				-1,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUND_SCOPE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) cancel_audio;

  task->launch = ags_cancel_audio_launch;
}

void
ags_cancel_audio_init(AgsCancelAudio *cancel_audio)
{
  cancel_audio->audio = NULL;

  cancel_audio->sound_scope = -1;
}

void
ags_cancel_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = AGS_CANCEL_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(cancel_audio->audio == audio){
	return;
      }

      if(cancel_audio->audio != NULL){
	g_object_unref(cancel_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      cancel_audio->audio = audio;
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      cancel_audio->sound_scope = g_value_get_int(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = AGS_CANCEL_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, cancel_audio->audio);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_int(value, cancel_audio->sound_scope);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_audio_dispose(GObject *gobject)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = AGS_CANCEL_AUDIO(gobject);

  if(cancel_audio->audio != NULL){
    g_object_unref(cancel_audio->audio);
    
    cancel_audio->audio = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_cancel_audio_parent_class)->dispose(gobject);
}

void
ags_cancel_audio_finalize(GObject *gobject)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = AGS_CANCEL_AUDIO(gobject);

  if(cancel_audio->audio != NULL){
    g_object_unref(cancel_audio->audio);    
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_cancel_audio_parent_class)->finalize(gobject);
}

void
ags_cancel_audio_launch(AgsTask *task)
{
  AgsAudio *audio;

  AgsCancelAudio *cancel_audio;
  
  GList *start_recall_id;
  
  gint sound_scope;
  
  cancel_audio = AGS_CANCEL_AUDIO(task);

  g_return_if_fail(AGS_IS_AUDIO(cancel_audio->audio));

  g_object_get(cancel_audio,
	       "audio", &audio,
	       "sound-scope", &sound_scope,
	       NULL);

  start_recall_id = ags_audio_check_scope(audio, sound_scope);
  
  ags_audio_stop(audio,
		 start_recall_id, sound_scope);
  
  g_object_unref(audio);

  g_list_free_full(start_recall_id,
		   g_object_unref);
} 
 

/**
 * ags_cancel_audio_new:
 * @audio: the #AgsAudio to cancel
 * @sound_scope: the #AgsSoundScope-enum or -1 for all
 *
 * Create a new instance of #AgsCancelAudio.
 *
 * Returns: the new #AgsCancelAudio.
 *
 * Since: 3.0.0
 */
AgsCancelAudio*
ags_cancel_audio_new(AgsAudio *audio,
		     gint sound_scope)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = (AgsCancelAudio *) g_object_new(AGS_TYPE_CANCEL_AUDIO,
						 "audio", audio,
						 "sound-scope", sound_scope,
						 NULL);


  return(cancel_audio);
}
