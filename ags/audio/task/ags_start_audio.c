/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/task/ags_start_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_start_audio_class_init(AgsStartAudioClass *start_audio);
void ags_start_audio_init(AgsStartAudio *start_audio);
void ags_start_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_start_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_start_audio_dispose(GObject *gobject);
void ags_start_audio_finalize(GObject *gobject);

void ags_start_audio_launch(AgsTask *task);

/**
 * SECTION:ags_start_audio
 * @short_description: start audio object to audio loop
 * @title: AgsStartAudio
 * @section_id:
 * @include: ags/audio/task/ags_start_audio.h
 *
 * The #AgsStartAudio task starts #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_start_audio_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SOUND_SCOPE,
};

GType
ags_start_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_start_audio = 0;

    static const GTypeInfo ags_start_audio_info = {
      sizeof(AgsStartAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_start_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsStartAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_start_audio_init,
    };

    ags_type_start_audio = g_type_register_static(AGS_TYPE_TASK,
						  "AgsStartAudio",
						  &ags_start_audio_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_start_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_start_audio_class_init(AgsStartAudioClass *start_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_start_audio_parent_class = g_type_class_peek_parent(start_audio);

  /* gobject */
  gobject = (GObjectClass *) start_audio;

  gobject->set_property = ags_start_audio_set_property;
  gobject->get_property = ags_start_audio_get_property;

  gobject->dispose = ags_start_audio_dispose;
  gobject->finalize = ags_start_audio_finalize;

  /* properties */
  /**
   * AgsStartAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of start audio"),
				   i18n_pspec("The audio of start audio task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsStartAudio:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_int("sound-scope",
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
  task = (AgsTaskClass *) start_audio;

  task->launch = ags_start_audio_launch;
}

void
ags_start_audio_init(AgsStartAudio *start_audio)
{
  start_audio->audio = NULL;

  start_audio->sound_scope = -1;
}

void
ags_start_audio_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsStartAudio *start_audio;

  start_audio = AGS_START_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(start_audio->audio == audio){
	return;
      }

      if(start_audio->audio != NULL){
	g_object_unref(start_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      start_audio->audio = audio;
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      start_audio->sound_scope = g_value_get_int(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_audio_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsStartAudio *start_audio;

  start_audio = AGS_START_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, start_audio->audio);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_int(value, start_audio->sound_scope);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_start_audio_dispose(GObject *gobject)
{
  AgsStartAudio *start_audio;

  start_audio = AGS_START_AUDIO(gobject);

  if(start_audio->audio != NULL){
    g_object_unref(start_audio->audio);

    start_audio->audio = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_start_audio_parent_class)->dispose(gobject);
}

void
ags_start_audio_finalize(GObject *gobject)
{
  AgsStartAudio *start_audio;

  start_audio = AGS_START_AUDIO(gobject);

  if(start_audio->audio != NULL){
    g_object_unref(start_audio->audio);
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_start_audio_parent_class)->finalize(gobject);
}

void
ags_start_audio_launch(AgsTask *task)
{
  AgsAudio *audio;
  
  AgsStartAudio *start_audio;
    
  GList *recall_id;

  gint sound_scope;

  start_audio = AGS_START_AUDIO(task);

  g_object_get(start_audio,
	       "audio", &audio,
	       "sound-scope", &sound_scope,
	       NULL);
  
  recall_id = ags_audio_start(audio,
			      sound_scope);

  g_object_unref(audio);
  
  g_list_free_full(recall_id,
		   g_object_unref);
}

/**
 * ags_start_audio_new:
 * @audio: the #AgsAudio to start
 * @sound_scope: the #AgsSoundScope-enum or -1 for all
 *
 * Create a new instance of #AgsStartAudio.
 *
 * Returns: the new #AgsStartAudio.
 *
 * Since: 3.0.0
 */
AgsStartAudio*
ags_start_audio_new(AgsAudio *audio,
		    gint sound_scope)
{
  AgsStartAudio *start_audio;

  start_audio = (AgsStartAudio *) g_object_new(AGS_TYPE_START_AUDIO,
					       "audio", audio,
					       "sound-scope", sound_scope,
					       NULL);
  
  return(start_audio);
}
