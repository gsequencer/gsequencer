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

#include <ags/audio/task/ags_play_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/i18n.h>

void ags_play_audio_class_init(AgsPlayAudioClass *play_audio);
void ags_play_audio_init(AgsPlayAudio *play_audio);
void ags_play_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_play_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_play_audio_dispose(GObject *gobject);
void ags_play_audio_finalize(GObject *gobject);

void ags_play_audio_launch(AgsTask *task);

/**
 * SECTION:ags_play_audio
 * @short_description: play audio object to audio loop
 * @title: AgsPlayAudio
 * @section_id:
 * @include: ags/audio/task/ags_play_audio.h
 *
 * The #AgsPlayAudio task plays #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_play_audio_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_SOUND_SCOPE,
};

GType
ags_play_audio_get_type()
{
  static GType ags_type_play_audio = 0;

  if(!ags_type_play_audio){
    static const GTypeInfo ags_play_audio_info = {
      sizeof(AgsPlayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPlayAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_init,
    };

    ags_type_play_audio = g_type_register_static(AGS_TYPE_TASK,
						 "AgsPlayAudio",
						 &ags_play_audio_info,
						 0);
  }
  
  return(ags_type_play_audio);
}

void
ags_play_audio_class_init(AgsPlayAudioClass *play_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_play_audio_parent_class = g_type_class_peek_parent(play_audio);

  /* gobject */
  gobject = (GObjectClass *) play_audio;

  gobject->set_property = ags_play_audio_set_property;
  gobject->get_property = ags_play_audio_get_property;

  gobject->dispose = ags_play_audio_dispose;
  gobject->finalize = ags_play_audio_finalize;

  /* properties */
  /**
   * AgsPlayAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio",
				   i18n_pspec("audio of play audio"),
				   i18n_pspec("The audio of play audio task"),
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsPlayAudio:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_int("sound-scope",
				 i18n_pspec("sound scope"),
				 i18n_pspec("The sound scope"),
				 0,
				 AGS_SOUND_SCOPE_LAST,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUND_SCOPE,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) play_audio;

  task->launch = ags_play_audio_launch;
}

void
ags_play_audio_init(AgsPlayAudio *play_audio)
{
  play_audio->audio = NULL;

  play_audio->sound_scope = -1;
}

void
ags_play_audio_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsPlayAudio *play_audio;

  play_audio = AGS_PLAY_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(play_audio->audio == (GObject *) audio){
	return;
      }

      if(play_audio->audio != NULL){
	g_object_unref(play_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      play_audio->audio = (GObject *) audio;
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      play_audio->sound_scope = g_value_get_int(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_audio_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsPlayAudio *play_audio;

  play_audio = AGS_PLAY_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      g_value_set_object(value, play_audio->audio);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_int(value, play_audio->sound_scope);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_audio_dispose(GObject *gobject)
{
  AgsPlayAudio *play_audio;

  play_audio = AGS_PLAY_AUDIO(gobject);

  if(play_audio->audio != NULL){
    g_object_unref(play_audio->audio);

    play_audio->audio = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_parent_class)->dispose(gobject);
}

void
ags_play_audio_finalize(GObject *gobject)
{
  AgsPlayAudio *play_audio;

  play_audio = AGS_PLAY_AUDIO(gobject);

  if(play_audio->audio != NULL){
    g_object_unref(play_audio->audio);
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_parent_class)->finalize(gobject);
}

void
ags_play_audio_launch(AgsTask *task)
{
  AgsAudio *audio;
  AgsChannel *output;
  AgsRecycling *recycling;
  AgsPlaybackDomain *playback_domain;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  
  AgsPlayAudio *play_audio;

  AgsAudioLoop *audio_loop;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gint sound_scope;
  static const guint staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				      AGS_SOUND_STAGING_RUN_INIT_PRE |
				      AGS_SOUND_STAGING_RUN_INIT_INTER |
				      AGS_SOUND_STAGING_RUN_INIT_POST);

  play_audio = AGS_PLAY_AUDIO(task);
  
  audio = play_audio->audio;

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  g_object_get(playback_domain,
	       "output-playback", &list_start,
	       NULL);
  
  sound_scope = play_audio->sound_scope;

  application_context = ags_application_context_get_instance();

  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       NULL);

  /* add audio to AgsAudioLoop */
  ags_audio_loop_add_audio(audio_loop,
			   (GObject *) audio);

  if(sound_scope >= 0){
    list = list_start;

    while(list != NULL){
      /* get some fields */
      g_object_get(list->data,
		   "channel", &output,
		   NULL);

      g_object_get(output,
		   "first-recycling", &recycling,
		   NULL);

      /* create recall id and recycling context */
      recall_id = ags_recall_id_new();

      recycling_context = ags_recycling_context_new(1);
      ags_recycling_context_replace(recycling_context,
				    recycling,
				    0);

      g_object_set(recycling_context,
		   "recall-id", recall_id,
		   NULL);
      
      g_object_set(recall_id,
		   "recycling-context", recycling_context,
		   NULL);
      
      ags_playback_set_recall_id(list->data,
				 recall_id,
				 sound_scope);

      /* add to start queue */
      ags_thread_add_start_queue(ags_playback_domain_get_audio_thread(playback_domain,
								      sound_scope),
				 ags_playback_get_channel_thread(list->data,
								 sound_scope));
      
      /* iterate */
      list = list->next;
    }
    
    /* play init */
    ags_audio_recursive_run_stage(audio,
				  sound_scope, staging_flags);

    /* add to start queue */
    ags_thread_add_start_queue(audio_loop,
			       ags_playback_domain_get_audio_thread(playback_domain,
								    sound_scope));
  }else{
    gint i;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){      
      list = list_start;

      while(list != NULL){	
	/* get some fields */
	g_object_get(list->data,
		     "channel", &output,
		     NULL);

	g_object_get(output,
		     "first-recycling", &recycling,
		     NULL);

	/* create recall id and recycling context */
	recall_id = ags_recall_id_new();

	recycling_context = ags_recycling_context_new(1);
	ags_recycling_context_replace(recycling_context,
				      recycling,
				      0);

	g_object_set(recycling_context,
		     "recall-id", recall_id,
		     NULL);
      
	g_object_set(recall_id,
		     "recycling-context", recycling_context,
		     NULL);
	
	ags_playback_set_recall_id(list->data,
				   recall_id,
				   i);

	/* add to start queue */
	ags_thread_add_start_queue(ags_playback_domain_get_audio_thread(playback_domain,
									i),
				   ags_playback_get_channel_thread(list->data,
								   i));
	
	/* iterate */
	list = list->next;
      }

      /* play init */
      ags_audio_recursive_run_stage(audio,
				    i, staging_flags);

      /* add to start queue */
      ags_thread_add_start_queue(audio_loop,
				 ags_playback_domain_get_audio_thread(playback_domain,
								      i));
    }
  }
}

/**
 * ags_play_audio_new:
 * @audio: the #AgsAudio to play
 * @sound_scope: the #AgsSoundScope-enum or -1 for all
 *
 * Create a new instance of #AgsPlayAudio.
 *
 * Returns: the new #AgsPlayAudio.
 *
 * Since: 2.0.0
 */
AgsPlayAudio*
ags_play_audio_new(AgsAudio *audio,
		   gint sound_scope)
{
  AgsPlayAudio *play_audio;

  play_audio = (AgsPlayAudio *) g_object_new(AGS_TYPE_PLAY_AUDIO,
					     "audio", audio,
					     "sound-scope", sound_scope,
					     NULL);
  
  return(play_audio);
}
