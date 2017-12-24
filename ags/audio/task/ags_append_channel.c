/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/task/ags_append_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/i18n.h>

void ags_append_channel_class_init(AgsAppendChannelClass *append_channel);
void ags_append_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_channel_init(AgsAppendChannel *append_channel);
void ags_append_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_append_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_append_channel_connect(AgsConnectable *connectable);
void ags_append_channel_disconnect(AgsConnectable *connectable);
void ags_append_channel_dispose(GObject *gobject);
void ags_append_channel_finalize(GObject *gobject);

void ags_append_channel_launch(AgsTask *task);

/**
 * SECTION:ags_append_channel
 * @short_description: append channel object to audio loop
 * @title: AgsAppendChannel
 * @section_id:
 * @include: ags/audio/task/ags_append_channel.h
 *
 * The #AgsAppendChannel task appends #AgsChannel to #AgsAudioLoop.
 */

static gpointer ags_append_channel_parent_class = NULL;
static AgsConnectableInterface *ags_append_channel_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO_LOOP,
  PROP_CHANNEL,
};

GType
ags_append_channel_get_type()
{
  static GType ags_type_append_channel = 0;

  if(!ags_type_append_channel){
    static const GTypeInfo ags_append_channel_info = {
      sizeof (AgsAppendChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsAppendChannel",
						   &ags_append_channel_info,
						   0);

    g_type_add_interface_static(ags_type_append_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_channel);
}

void
ags_append_channel_class_init(AgsAppendChannelClass *append_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_append_channel_parent_class = g_type_class_peek_parent(append_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) append_channel;

  gobject->set_property = ags_append_channel_set_property;
  gobject->get_property = ags_append_channel_get_property;

  gobject->dispose = ags_append_channel_dispose;
  gobject->finalize = ags_append_channel_finalize;

  /* properties */
  /**
   * AgsAppendChannel:audio-loop:
   *
   * The assigned #AgsAudioLoop
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("audio-loop",
				   i18n_pspec("audio loop of append channel"),
				   i18n_pspec("The audio loop of append channel task"),
				   AGS_TYPE_AUDIO_LOOP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_LOOP,
				  param_spec);

  /**
   * AgsAppendChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of append channel"),
				   i18n_pspec("The channel of append channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsTaskClass */
  task = (AgsTaskClass *) append_channel;

  task->launch = ags_append_channel_launch;
}

void
ags_append_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_channel_connect;
  connectable->disconnect = ags_append_channel_disconnect;
}

void
ags_append_channel_init(AgsAppendChannel *append_channel)
{
  append_channel->audio_loop = NULL;
  append_channel->channel = NULL;
}

void
ags_append_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsAppendChannel *append_channel;

  append_channel = AGS_APPEND_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      AgsAudioLoop *audio_loop;

      audio_loop = (AgsAudioLoop *) g_value_get_object(value);

      if(append_channel->audio_loop == (GObject *) audio_loop){
	return;
      }

      if(append_channel->audio_loop != NULL){
	g_object_unref(append_channel->audio_loop);
      }

      if(audio_loop != NULL){
	g_object_ref(audio_loop);
      }

      append_channel->audio_loop = (GObject *) audio_loop;
    }
    break;
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(append_channel->channel == (GObject *) channel){
	return;
      }

      if(append_channel->channel != NULL){
	g_object_unref(append_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      append_channel->channel = (GObject *) channel;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_append_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAppendChannel *append_channel;

  append_channel = AGS_APPEND_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      g_value_set_object(value, append_channel->audio_loop);
    }
    break;
  case PROP_CHANNEL:
    {
      g_value_set_object(value, append_channel->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_append_channel_connect(AgsConnectable *connectable)
{
  ags_append_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_channel_disconnect(AgsConnectable *connectable)
{
  ags_append_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_channel_dispose(GObject *gobject)
{
  AgsAppendChannel *append_channel;

  append_channel = AGS_APPEND_CHANNEL(gobject);

  if(append_channel->audio_loop != NULL){
    g_object_unref(append_channel->audio_loop);

    append_channel->audio_loop = NULL;
  }

  if(append_channel->channel != NULL){
    g_object_unref(append_channel->channel);

    append_channel->channel = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_append_channel_parent_class)->dispose(gobject);
}

void
ags_append_channel_finalize(GObject *gobject)
{
  AgsAppendChannel *append_channel;

  append_channel = AGS_APPEND_CHANNEL(gobject);

  if(append_channel->audio_loop != NULL){
    g_object_unref(append_channel->audio_loop);
  }

  if(append_channel->channel != NULL){
    g_object_unref(append_channel->channel);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_append_channel_parent_class)->finalize(gobject);
}

void
ags_append_channel_launch(AgsTask *task)
{
  AgsChannel *channel;
  AgsPlayback *playback;
  
  AgsAppendChannel *append_channel;

  AgsAudioLoop *audio_loop;
  AgsAudioLoop *channel_thread;

  AgsServer *server;

  AgsMutexManager *mutex_manager;
  
  AgsConfig *config;

  GList *start_queue;
  
  gchar *str0, *str1;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *channel_mutex;
  pthread_mutex_t *audio_loop_mutex;
  pthread_mutex_t *channel_thread_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  config = ags_config_get_instance();

  append_channel = AGS_APPEND_CHANNEL(task);

  channel = (AgsChannel *) append_channel->channel;

  audio_loop = AGS_AUDIO_LOOP(append_channel->audio_loop);

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

  /* append to AgsDevout */
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
  //    ags_connectable_add_to_registry(AGS_CONNECTABLE(append_channel->channel));
  //  }
}

/**
 * ags_append_channel_new:
 * @audio_loop: the #AgsAudioLoop
 * @channel: the #AgsChannel to append
 *
 * Creates an #AgsAppendChannel.
 *
 * Returns: an new #AgsAppendChannel.
 *
 * Since: 1.0.0
 */
AgsAppendChannel*
ags_append_channel_new(GObject *audio_loop,
		       GObject *channel)
{
  AgsAppendChannel *append_channel;

  append_channel = (AgsAppendChannel *) g_object_new(AGS_TYPE_APPEND_CHANNEL,
						     "audio-loop", audio_loop,
						     "channel", channel,
						     NULL);

  return(append_channel);
}
