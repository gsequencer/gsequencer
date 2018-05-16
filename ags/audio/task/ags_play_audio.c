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
void ags_play_audio_connectable_interface_init(AgsConnectableInterface *connectable);
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
  PROP_AUDIO_LOOP,
  PROP_AUDIO,
  PROP_SOUND_SCOPE,
};

GType
ags_play_audio_get_type()
{
  static GType ags_type_play_audio = 0;

  if(!ags_type_play_audio){
    static const GTypeInfo ags_play_audio_info = {
      sizeof (AgsPlayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudio),
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
   * AgsPlayAudio:audio-loop:
   *
   * The assigned #AgsAudioLoop
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio-loop",
				   i18n_pspec("audio loop of play audio"),
				   i18n_pspec("The audio loop of play audio task"),
				   AGS_TYPE_AUDIO_LOOP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_LOOP,
				  param_spec);

  /**
   * AgsPlayAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 1.0.0
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
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("sound-scope",
				     i18n_pspec("sound scope"),
				     i18n_pspec("The sound scope"),
				     0,
				     AGS_SOUND_SCOPE_LAST,
				     0,
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
  play_audio->audio_loop = NULL;
  play_audio->audio = NULL;

  play_audio->sound_scope = 0;
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
  case PROP_AUDIO_LOOP:
    {
      AgsAudioLoop *audio_loop;

      audio_loop = (AgsAudioLoop *) g_value_get_object(value);

      if(play_audio->audio_loop == (GObject *) audio_loop){
	return;
      }

      if(play_audio->audio_loop != NULL){
	g_object_unref(play_audio->audio_loop);
      }

      if(audio_loop != NULL){
	g_object_ref(audio_loop);
      }

      play_audio->audio_loop = (GObject *) audio_loop;
    }
    break;
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
      guint sound_scope;

      sound_scope = g_value_get_uint(value);

      play_audio->sound_scope = sound_scope;
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
  case PROP_AUDIO_LOOP:
    {
      g_value_set_object(value, play_audio->audio_loop);
    }
    break;
  case PROP_AUDIO:
    {
      g_value_set_object(value, play_audio->audio);
    }
    break;
  case PROP_SOUND_SCOPE:
    {
      g_value_set_uint(value, play_audio->sound_scope);
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

  if(play_audio->audio_loop != NULL){
    g_object_unref(play_audio->audio_loop);

    play_audio->audio_loop = NULL;
  }

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

  if(play_audio->audio_loop != NULL){
    g_object_unref(play_audio->audio_loop);
  }

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
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  
  AgsPlayAudio *play_audio;

  AgsAudioLoop *audio_loop;
  AgsAudioThread *audio_thread;
  AgsChannelThread *channel_thread;
  
  AgsConfig *config;

  GList *start_queue;
      
  gchar *str0, *str1;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *audio_loop_mutex;
  pthread_mutex_t *audio_thread_mutex;
  pthread_mutex_t *channel_thread_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  config = ags_config_get_instance();

  play_audio = AGS_PLAY_AUDIO(task);
  
  audio = AGS_AUDIO(play_audio->audio);
  
  audio_loop = AGS_AUDIO_LOOP(play_audio->audio_loop);  

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio loop mutex */
  pthread_mutex_lock(application_mutex);

  audio_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) audio_loop);
  
  pthread_mutex_unlock(application_mutex);

  /* play to AgsAudioLoop */
  ags_audio_loop_add_audio(audio_loop,
			   (GObject *) audio);

  /* read config */  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model");
  
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope");

  /* check config */
  if(!g_ascii_strncasecmp(str0,
			  "super-threaded",
			  15)){
    if(!g_ascii_strncasecmp(str1,
			    "audio",
			    6) ||
       !g_ascii_strncasecmp(str1,
			    "channel",
			    8)){
      /* get some fields */
      pthread_mutex_lock(audio_mutex);
	
      output = audio->output;
	
      playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

      audio_thread = playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER];
	
      pthread_mutex_unlock(audio_mutex);

      /* parent mutex */
      pthread_mutex_lock(application_mutex);

      audio_thread_mutex = ags_mutex_manager_lookup(mutex_manager,
						    (GObject *) audio_thread);
  
      pthread_mutex_unlock(application_mutex);
      
      /* super threaded setup - channel */
      if(!g_ascii_strncasecmp(str1,
			      "channel",
			      8)){
	/* sequencer */
	start_queue = NULL;
	
	while(output != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(application_mutex);

	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) output);
  
	  pthread_mutex_unlock(application_mutex);

	  if(play_audio->do_sequencer){
	    /* get some fields */
	    pthread_mutex_lock(channel_mutex);

	    playback = AGS_PLAYBACK(output->playback);
	    channel_thread = AGS_CHANNEL_THREAD(playback->channel_thread[AGS_PLAYBACK_SCOPE_SEQUENCER]);

	    pthread_mutex_unlock(channel_mutex);

	    /* start queue */
	    //FIXME:JK: check running
	    g_atomic_int_or(&(channel_thread->flags),
			    (AGS_CHANNEL_THREAD_WAIT |
			     AGS_CHANNEL_THREAD_DONE |
			     AGS_CHANNEL_THREAD_WAIT_SYNC |
			     AGS_CHANNEL_THREAD_DONE_SYNC));
	    start_queue = g_list_prepend(start_queue,
					 channel_thread);

	    /* add if needed */
	    if(g_atomic_pointer_get(&(AGS_THREAD(channel_thread)->parent)) == NULL){
	      ags_thread_add_child_extended(audio_thread,
					    channel_thread,
					    TRUE, TRUE);
	      ags_connectable_connect(AGS_CONNECTABLE(channel_thread));
	    }
	  }

	  /* iterate */
	  pthread_mutex_lock(channel_mutex);
	  
	  output = output->next;

	  pthread_mutex_unlock(channel_mutex);
	}

	/* start queue */
	start_queue = g_list_reverse(start_queue);

	pthread_mutex_lock(audio_thread_mutex);

	if(start_queue != NULL){
	  if(g_atomic_pointer_get(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue)) != NULL){
	    g_atomic_pointer_set(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue),
				 g_list_concat(start_queue,
					       g_atomic_pointer_get(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue))));
	  }else{
	    g_atomic_pointer_set(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->start_queue),
				 start_queue);
	  }
	}

	pthread_mutex_unlock(audio_thread_mutex);

	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	
	output = audio->output;
	
	audio_thread = playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION];
	
	pthread_mutex_unlock(audio_mutex);

	/* parent mutex */
	pthread_mutex_lock(application_mutex);

	audio_thread_mutex = ags_mutex_manager_lookup(mutex_manager,
						      (GObject *) audio_thread);
  
	pthread_mutex_unlock(application_mutex);

	/* notation */
	start_queue = NULL;
	
	while(output != NULL){
	  /* get channel mutex */
	  pthread_mutex_lock(application_mutex);

	  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						   (GObject *) output);
  
	  pthread_mutex_unlock(application_mutex);

	  if(play_audio->do_notation){
	    /* get some fields */
	    pthread_mutex_lock(channel_mutex);

	    playback = AGS_PLAYBACK(output->playback);
	    channel_thread = AGS_CHANNEL_THREAD(playback->channel_thread[AGS_PLAYBACK_SCOPE_NOTATION]);

	    pthread_mutex_unlock(channel_mutex);

	    /* start queue */
	    //FIXME:JK: check running
	    g_atomic_int_or(&(channel_thread->flags),
			    (AGS_CHANNEL_THREAD_WAIT |
			     AGS_CHANNEL_THREAD_DONE |
			     AGS_CHANNEL_THREAD_WAIT_SYNC |
			     AGS_CHANNEL_THREAD_DONE_SYNC));
	    start_queue = g_list_prepend(start_queue,
					 channel_thread);

	    /* add if needed */
	    if(g_atomic_pointer_get(&(AGS_THREAD(channel_thread)->parent)) == NULL){
	      ags_thread_add_child_extended(audio_thread,
					    channel_thread,
					    TRUE, TRUE);
	      ags_connectable_connect(AGS_CONNECTABLE(channel_thread));
	    }
	  }
	  
	  /* iterate */
	  pthread_mutex_lock(channel_mutex);
	  
	  output = output->next;

	  pthread_mutex_unlock(channel_mutex);
	}
	
	/* start queue */
	start_queue = g_list_reverse(start_queue);

	pthread_mutex_lock(audio_thread_mutex);

	if(start_queue != NULL){
	  if(g_atomic_pointer_get(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]->start_queue)) != NULL){
	    g_atomic_pointer_set(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]->start_queue),
				 g_list_concat(start_queue,
					       g_atomic_pointer_get(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]->start_queue))));
	  }else{
	    g_atomic_pointer_set(&(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]->start_queue),
				 start_queue);
	  }
	}

	pthread_mutex_unlock(audio_thread_mutex);
      }
      
      /* super threaded setup - audio */
      start_queue = NULL;
      
      if(play_audio->do_sequencer){
	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	
	audio_thread = playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER];
	
	pthread_mutex_unlock(audio_mutex);

	/* start queue */
	g_atomic_int_or(&(audio_thread->flags),
			(AGS_AUDIO_THREAD_WAIT |
			 AGS_AUDIO_THREAD_DONE |
			 AGS_AUDIO_THREAD_WAIT_SYNC |
			 AGS_AUDIO_THREAD_DONE_SYNC));
	start_queue = g_list_prepend(start_queue,
				     audio_thread);

	/* add if needed */
	if(g_atomic_pointer_get(&(AGS_THREAD(audio_thread)->parent)) == NULL){
	  ags_thread_add_child_extended((AgsThread *) audio_loop,
					audio_thread,
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(audio_thread));
	}
      }

      if(play_audio->do_notation){
	/* get some fields */
	pthread_mutex_lock(audio_mutex);
	
	audio_thread = playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION];
	
	pthread_mutex_unlock(audio_mutex);

	/* start queue */
	g_atomic_int_or(&(audio_thread->flags),
			(AGS_AUDIO_THREAD_WAIT |
			 AGS_AUDIO_THREAD_DONE |
			 AGS_AUDIO_THREAD_WAIT_SYNC |
			 AGS_AUDIO_THREAD_DONE_SYNC));
	start_queue = g_list_prepend(start_queue,
				     audio_thread);

	/* add if needed */
	if(g_atomic_pointer_get(&(AGS_THREAD(audio_thread)->parent)) == NULL){
	  ags_thread_add_child_extended((AgsThread *) audio_loop,
					audio_thread,
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(audio_thread));
	}
      }
      
      /* start queue */
      start_queue = g_list_reverse(start_queue);

      pthread_mutex_lock(audio_loop_mutex);

      if(start_queue != NULL){
	if(g_atomic_pointer_get(&(AGS_THREAD(audio_loop)->start_queue)) != NULL){
	  g_atomic_pointer_set(&(AGS_THREAD(audio_loop)->start_queue),
			       g_list_concat(start_queue,
					     g_atomic_pointer_get(&(AGS_THREAD(audio_loop)->start_queue))));
	}else{
	  g_atomic_pointer_set(&(AGS_THREAD(audio_loop)->start_queue),
			       start_queue);
	}
      }

      pthread_mutex_unlock(audio_loop_mutex);
    }
  }

  g_free(str0);
  g_free(str1);
  
  /* add to server registry */
  //  server = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(audio_loop->application_context));

  //  if(server != NULL && (AGS_SERVER_RUNNING & (server->flags)) != 0){
  //    ags_connectable_add_to_registry(AGS_CONNECTABLE(play_audio->audio));
  //  }
}

/**
 * ags_play_audio_new:
 * @audio_loop: the #AgsAudioLoop
 * @audio: the #AgsAudio to play
 * @sound_scope: sound scope
 *
 * Creates a new instance of #AgsPlayAudio.
 *
 * Returns: the new #AgsPlayAudio.
 *
 * Since: 2.0.0
 */
AgsPlayAudio*
ags_play_audio_new(GObject *audio_loop,
		   GObject *audio,
		   guint sound_scope)
{
  AgsPlayAudio *play_audio;

  play_audio = (AgsPlayAudio *) g_object_new(AGS_TYPE_PLAY_AUDIO,
					     "audio-loop", audio_loop,
					     "audio", audio,
					     "sound-scope", sound_scope,
					     NULL);
  
  return(play_audio);
}
