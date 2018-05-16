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

#include <ags/audio/task/ags_play_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/i18n.h>

void ags_play_channel_class_init(AgsPlayChannelClass *play_channel);
void ags_play_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_init(AgsPlayChannel *play_channel);
void ags_play_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_dispose(GObject *gobject);
void ags_play_channel_finalize(GObject *gobject);

void ags_play_channel_launch(AgsTask *task);

/**
 * SECTION:ags_play_channel
 * @short_description: play channel object to audio loop
 * @title: AgsPlayChannel
 * @section_id:
 * @include: ags/audio/task/ags_play_channel.h
 *
 * The #AgsPlayChannel task plays #AgsChannel to #AgsAudioLoop.
 */

static gpointer ags_play_channel_parent_class = NULL;

enum{
  PROP_0,
  PROP_AUDIO_LOOP,
  PROP_CHANNEL,
};

GType
ags_play_channel_get_type()
{
  static GType ags_type_play_channel = 0;

  if(!ags_type_play_channel){
    static const GTypeInfo ags_play_channel_info = {
      sizeof (AgsPlayChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_init,
    };

    ags_type_play_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsPlayChannel",
						   &ags_play_channel_info,
						   0);
  }
  
  return(ags_type_play_channel);
}

void
ags_play_channel_class_init(AgsPlayChannelClass *play_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_play_channel_parent_class = g_type_class_peek_parent(play_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel;

  gobject->set_property = ags_play_channel_set_property;
  gobject->get_property = ags_play_channel_get_property;

  gobject->dispose = ags_play_channel_dispose;
  gobject->finalize = ags_play_channel_finalize;

  /* properties */
  /**
   * AgsPlayChannel:audio-loop:
   *
   * The assigned #AgsAudioLoop
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio-loop",
				   i18n_pspec("audio loop of play channel"),
				   i18n_pspec("The audio loop of play channel task"),
				   AGS_TYPE_AUDIO_LOOP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_LOOP,
				  param_spec);

  /**
   * AgsPlayChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of play channel"),
				   i18n_pspec("The channel of play channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) play_channel;

  task->launch = ags_play_channel_launch;
}

void
ags_play_channel_init(AgsPlayChannel *play_channel)
{
  play_channel->audio_loop = NULL;
  play_channel->channel = NULL;
}

void
ags_play_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      AgsAudioLoop *audio_loop;

      audio_loop = (AgsAudioLoop *) g_value_get_object(value);

      if(play_channel->audio_loop == (GObject *) audio_loop){
	return;
      }

      if(play_channel->audio_loop != NULL){
	g_object_unref(play_channel->audio_loop);
      }

      if(audio_loop != NULL){
	g_object_ref(audio_loop);
      }

      play_channel->audio_loop = (GObject *) audio_loop;
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(play_channel->channel == (GObject *) channel){
	return;
      }

      if(play_channel->channel != NULL){
	g_object_unref(play_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      play_channel->channel = (GObject *) channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      g_value_set_object(value, play_channel->audio_loop);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value, play_channel->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_dispose(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  if(play_channel->audio_loop != NULL){
    g_object_unref(play_channel->audio_loop);

    play_channel->audio_loop = NULL;
  }

  if(play_channel->channel != NULL){
    g_object_unref(play_channel->channel);

    play_channel->channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->dispose(gobject);
}

void
ags_play_channel_finalize(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  if(play_channel->audio_loop != NULL){
    g_object_unref(play_channel->audio_loop);
  }

  if(play_channel->channel != NULL){
    g_object_unref(play_channel->channel);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->finalize(gobject);
}

void
ags_play_channel_launch(AgsTask *task)
{
  AgsChannel *channel;
  AgsPlayback *playback;
  
  AgsPlayChannel *play_channel;

  AgsAudioLoop *audio_loop;
  AgsChannelThread *channel_thread;

  AgsConfig *config;

  GList *start_queue;
  
  gchar *str0, *str1;
  
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *audio_loop_mutex;
  pthread_mutex_t *channel_thread_mutex;

  config = ags_config_get_instance();

  play_channel = AGS_PLAY_CHANNEL(task);

  channel = (AgsChannel *) play_channel->channel;

  audio_loop = AGS_AUDIO_LOOP(play_channel->audio_loop);

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get audio loop mutex */
  pthread_mutex_lock(application_mutex);

  audio_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) audio_loop);
  
  pthread_mutex_unlock(application_mutex);

  /* play to AgsDevout */
  ags_audio_loop_add_channel(audio_loop,
			     (GObject *) channel);

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
    /* super threaded setup */
    if(!g_ascii_strncasecmp(str1,
			    "channel",
			    8)){
      start_queue = NULL;  

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      playback = AGS_PLAYBACK(channel->playback);
      channel_thread = AGS_CHANNEL_THREAD(playback->channel_thread[AGS_PLAYBACK_SCOPE_PLAYBACK]);

      pthread_mutex_unlock(channel_mutex);
      
      if((AGS_PLAYBACK_PLAYBACK & (g_atomic_int_get(&(playback->flags)))) != 0 &&
	 (AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(channel_thread)->flags)))) == 0){
	/* start queue */
	g_atomic_int_or(&(channel_thread->flags),
			(AGS_CHANNEL_THREAD_WAIT |
			 AGS_CHANNEL_THREAD_DONE |
			 AGS_CHANNEL_THREAD_WAIT_SYNC |
			 AGS_CHANNEL_THREAD_DONE_SYNC));
	start_queue = g_list_prepend(start_queue,
				     channel_thread);
	
	/* add if needed */
	if(g_atomic_pointer_get(&(AGS_THREAD(channel_thread)->parent)) == NULL){
	  ags_thread_add_child_extended((AgsThread *) audio_loop,
					channel_thread,
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(channel_thread));
	}
      }

      /* start queue */
      pthread_mutex_lock(audio_loop_mutex);
  
      if(start_queue != NULL){
	start_queue = g_list_reverse(start_queue);

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
  //    ags_connectable_add_to_registry(AGS_CONNECTABLE(play_channel->channel));
  //  }
}

/**
 * ags_play_channel_new:
 * @audio_loop: the #AgsAudioLoop
 * @channel: the #AgsChannel to play
 *
 * Creates an #AgsPlayChannel.
 *
 * Returns: an new #AgsPlayChannel.
 *
 * Since: 1.0.0
 */
AgsPlayChannel*
ags_play_channel_new(GObject *audio_loop,
		     GObject *channel)
{
  AgsPlayChannel *play_channel;

  play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						 "audio-loop", audio_loop,
						 "channel", channel,
						 NULL);

  return(play_channel);
}
