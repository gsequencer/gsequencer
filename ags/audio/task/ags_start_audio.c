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
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

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
   * Since: 2.0.0
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
   * Since: 2.0.0
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
  AgsChannel *output;
  AgsRecycling *recycling;
  AgsPlaybackDomain *playback_domain;
  AgsRecallID *audio_recall_id, *channel_recall_id;
  AgsRecyclingContext *recycling_context;
  
  AgsStartAudio *start_audio;

  AgsAudioLoop *audio_loop;
  AgsAudioThread *audio_thread;
  
  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gint sound_scope;
  static const guint staging_flags = (AGS_SOUND_STAGING_CHECK_RT_DATA |
				      AGS_SOUND_STAGING_RUN_INIT_PRE |
				      AGS_SOUND_STAGING_RUN_INIT_INTER |
				      AGS_SOUND_STAGING_RUN_INIT_POST);

  start_audio = AGS_START_AUDIO(task);
  
  audio = start_audio->audio;

  if(!AGS_IS_AUDIO(audio)){
    return;
  }

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  g_object_get(playback_domain,
	       "output-playback", &list_start,
	       NULL);
  
  sound_scope = start_audio->sound_scope;

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* add audio to AgsAudioLoop */
  ags_audio_loop_add_audio(audio_loop,
			   (GObject *) audio);

  /* flag to playback audio */
  audio_loop->flags |= AGS_AUDIO_LOOP_PLAY_AUDIO;

  if(sound_scope >= 0){
    list = list_start;

    while(list != NULL){
      /* get some fields */
      g_object_get(list->data,
		   "channel", &output,
		   NULL);
      g_object_unref(output);
      
      g_object_get(output,
		   "first-recycling", &recycling,
		   NULL);
      g_object_unref(recycling);

      /* create recall id and recycling context */
      audio_recall_id = ags_recall_id_new();
      ags_recall_id_set_sound_scope(audio_recall_id, sound_scope);
      ags_audio_add_recall_id(audio,
			      (GObject *) audio_recall_id);

      channel_recall_id = ags_recall_id_new();
      ags_recall_id_set_sound_scope(channel_recall_id, sound_scope);
      ags_channel_add_recall_id(output,
				channel_recall_id);
      
      recycling_context = ags_recycling_context_new(1);
      ags_recycling_context_replace(recycling_context,
				    recycling,
				    0);

      g_object_set(recycling_context,
		   "recall-id", audio_recall_id,
		   NULL);
      
      g_object_set(audio_recall_id,
		   "recycling-context", recycling_context,
		   NULL);

      g_object_set(channel_recall_id,
		   "recycling-context", recycling_context,
		   NULL);
      
      ags_playback_set_recall_id(list->data,
				 audio_recall_id,
				 sound_scope);

      /* add to start queue */
      ags_thread_add_start_queue((AgsThread *) audio_loop,
				 ags_playback_get_channel_thread(list->data,
								 sound_scope));
      
      /* iterate */
      list = list->next;
    }
    
    /* play init */
    ags_audio_recursive_run_stage(audio,
				  sound_scope, staging_flags);

    /* add to start queue */
    audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									   sound_scope);
        
    ags_thread_add_start_queue((AgsThread *) audio_loop,
			       (AgsThread *) audio_thread);
  }else{
    gint i;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){      
      list = list_start;

      while(list != NULL){	
	/* get some fields */
	g_object_get(list->data,
		     "channel", &output,
		     NULL);
	g_object_unref(output);

	g_object_get(output,
		     "first-recycling", &recycling,
		     NULL);
	g_object_unref(recycling);
	
	/* create recall id and recycling context */
	audio_recall_id = ags_recall_id_new();
	ags_recall_id_set_sound_scope(audio_recall_id, sound_scope);
	ags_audio_add_recall_id(audio,
				(GObject *) audio_recall_id);

	channel_recall_id = ags_recall_id_new();
	ags_recall_id_set_sound_scope(channel_recall_id, sound_scope);
	ags_channel_add_recall_id(output,
				  channel_recall_id);

	recycling_context = ags_recycling_context_new(1);
	ags_recycling_context_replace(recycling_context,
				      recycling,
				      0);

	g_object_set(recycling_context,
		     "recall-id", audio_recall_id,
		     NULL);
      
	g_object_set(audio_recall_id,
		     "recycling-context", recycling_context,
		     NULL);

	g_object_set(channel_recall_id,
		     "recycling-context", recycling_context,
		     NULL);
	
	ags_playback_set_recall_id(list->data,
				   audio_recall_id,
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
      audio_thread = (AgsAudioThread *) ags_playback_domain_get_audio_thread(playback_domain,
									     i);
            
      ags_thread_add_start_queue((AgsThread *) audio_loop,
				 (AgsThread *) audio_thread);
    }
  }

  g_object_unref(playback_domain);
  
  g_list_free_full(list_start,
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
 * Since: 2.0.0
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
