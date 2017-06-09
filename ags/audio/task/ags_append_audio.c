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

#include <ags/audio/task/ags_append_audio.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/server/ags_server.h>
#include <ags/server/ags_service_provider.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

void ags_append_audio_class_init(AgsAppendAudioClass *append_audio);
void ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_append_audio_init(AgsAppendAudio *append_audio);
void ags_append_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_append_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_append_audio_connect(AgsConnectable *connectable);
void ags_append_audio_disconnect(AgsConnectable *connectable);
void ags_append_audio_finalize(GObject *gobject);

void ags_append_audio_launch(AgsTask *task);

/**
 * SECTION:ags_append_audio
 * @short_description: append audio object to audio loop
 * @title: AgsAppendAudio
 * @section_id:
 * @include: ags/audio/task/ags_append_audio.h
 *
 * The #AgsAppendAudio task appends #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_append_audio_parent_class = NULL;
static AgsConnectableInterface *ags_append_audio_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO_LOOP,
  PROP_AUDIO,
  PROP_DO_PLAYBACK,
  PROP_DO_SEQUENCER,
  PROP_DO_NOTATION,
};

GType
ags_append_audio_get_type()
{
  static GType ags_type_append_audio = 0;

  if(!ags_type_append_audio){
    static const GTypeInfo ags_append_audio_info = {
      sizeof (AgsAppendAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_append_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAppendAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_append_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_append_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_append_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsAppendAudio",
						   &ags_append_audio_info,
						   0);

    g_type_add_interface_static(ags_type_append_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_append_audio);
}

void
ags_append_audio_class_init(AgsAppendAudioClass *append_audio)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_append_audio_parent_class = g_type_class_peek_parent(append_audio);

  /* gobject */
  gobject = (GObjectClass *) append_audio;

  gobject->set_property = ags_append_audio_set_property;
  gobject->get_property = ags_append_audio_get_property;

  gobject->finalize = ags_append_audio_finalize;

  /* properties */
  /**
   * AgsAppendAudio:audio-loop:
   *
   * The assigned #AgsAudioLoop
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("audio-loop",
				   "audio loop of append audio",
				   "The audio loop of append audio task",
				   AGS_TYPE_AUDIO_LOOP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_LOOP,
				  param_spec);

  /**
   * AgsAppendAudio:audio:
   *
   * The assigned #AgsAudio
   * 
   * Since: 0.7.117
   */
  param_spec = g_param_spec_object("audio",
				   "audio of append audio",
				   "The audio of append audio task",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /**
   * AgsAppendAudio:do-playback:
   *
   * The effects do-playback.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("do-playback",
				     "do playback",
				     "Do playback of audio",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_PLAYBACK,
				  param_spec);

  /**
   * AgsAppendAudio:do-sequencer:
   *
   * The effects do-sequencer.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("do-sequencer",
				     "do sequencer",
				     "Do sequencer of audio",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_SEQUENCER,
				  param_spec);

  /**
   * AgsAppendAudio:do-notation:
   *
   * The effects do-notation.
   * 
   * Since: 0.7.117
   */
  param_spec =  g_param_spec_boolean("do-notation",
				     "do notation",
				     "Do notation of audio",
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_NOTATION,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) append_audio;

  task->launch = ags_append_audio_launch;
}

void
ags_append_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_append_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_append_audio_connect;
  connectable->disconnect = ags_append_audio_disconnect;
}

void
ags_append_audio_init(AgsAppendAudio *append_audio)
{
  append_audio->audio_loop = NULL;
  append_audio->audio = NULL;

  append_audio->do_playback = FALSE;
  append_audio->do_sequencer = FALSE;
  append_audio->do_notation = FALSE;
}

void
ags_append_audio_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAppendAudio *append_audio;

  append_audio = AGS_APPEND_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      AgsAudioLoop *audio_loop;

      audio_loop = (AgsAudioLoop *) g_value_get_object(value);

      if(append_audio->audio_loop == (GObject *) audio_loop){
	return;
      }

      if(append_audio->audio_loop != NULL){
	g_object_unref(append_audio->audio_loop);
      }

      if(audio_loop != NULL){
	g_object_ref(audio_loop);
      }

      append_audio->audio_loop = (GObject *) audio_loop;
    }
    break;
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(append_audio->audio == (GObject *) audio){
	return;
      }

      if(append_audio->audio != NULL){
	g_object_unref(append_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      append_audio->audio = (GObject *) audio;
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      gboolean do_playback;

      do_playback = g_value_get_boolean(value);

      append_audio->do_playback = do_playback;
    }
    break;
  case PROP_DO_SEQUENCER:
    {
      gboolean do_sequencer;

      do_sequencer = g_value_get_boolean(value);

      append_audio->do_sequencer = do_sequencer;
    }
    break;
  case PROP_DO_NOTATION:
    {
      gboolean do_notation;

      do_notation = g_value_get_boolean(value);

      append_audio->do_notation = do_notation;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_append_audio_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAppendAudio *append_audio;

  append_audio = AGS_APPEND_AUDIO(gobject);

  switch(prop_id){
  case PROP_AUDIO_LOOP:
    {
      g_value_set_object(value, append_audio->audio_loop);
    }
    break;
  case PROP_AUDIO:
    {
      g_value_set_object(value, append_audio->audio);
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      g_value_set_boolean(value, append_audio->do_playback);
    }
    break;
  case PROP_DO_SEQUENCER:
    {
      g_value_set_boolean(value, append_audio->do_sequencer);
    }
    break;
  case PROP_DO_NOTATION:
    {
      g_value_set_boolean(value, append_audio->do_notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_append_audio_connect(AgsConnectable *connectable)
{
  ags_append_audio_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_append_audio_disconnect(AgsConnectable *connectable)
{
  ags_append_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_append_audio_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_append_audio_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_append_audio_launch(AgsTask *task)
{
  AgsAudio *audio;

  AgsAppendAudio *append_audio;

  AgsAudioLoop *audio_loop;
  
  AgsServer *server;

  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  GList *start_queue;
      
  gchar *str0, *str1;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_loop_mutex;
  pthread_mutex_t *audio_thread_mutex;
  
  append_audio = AGS_APPEND_AUDIO(task);
  
  audio_loop = AGS_AUDIO_LOOP(append_audio->audio_loop);
  
  audio = (AgsAudio *) append_audio->audio;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  audio_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) audio_loop);
  
  pthread_mutex_unlock(application_mutex);

  /* append to AgsAudioLoop */
  ags_audio_loop_add_audio(audio_loop,
			   (GObject *) audio);

  /**/
  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model");
  
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope");
      
  if(!g_ascii_strncasecmp(str0,
			  "super-threaded",
			  15)){
    if(!g_ascii_strncasecmp(str1,
			    "audio",
			    6) ||
       !g_ascii_strncasecmp(str1,
			    "channel",
			    8)){
      /* super threaded setup - channel */
      if(!g_ascii_strncasecmp(str1,
			      "channel",
			      8)){
	AgsChannel *output;

	/* parent mutex */
	pthread_mutex_lock(application_mutex);

	audio_thread_mutex = ags_mutex_manager_lookup(mutex_manager,
						      (GObject *) AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]);
  
	pthread_mutex_unlock(application_mutex);

	/* sequencer */
	output = audio->output;

	start_queue = NULL;
	
	while(output != NULL){
	  if(append_audio->do_sequencer){
	    g_atomic_int_or(&(AGS_CHANNEL_THREAD(AGS_PLAYBACK(output->playback)->channel_thread[1])->flags),
			    (AGS_CHANNEL_THREAD_WAIT |
			     AGS_CHANNEL_THREAD_DONE |
			     AGS_CHANNEL_THREAD_WAIT_SYNC |
			     AGS_CHANNEL_THREAD_DONE_SYNC));
	    start_queue = g_list_prepend(start_queue,
					 AGS_PLAYBACK(output->playback)->channel_thread[1]);

	    if(AGS_PLAYBACK(output->playback)->channel_thread[1]->parent == NULL){
	      ags_thread_add_child_extended(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
					    AGS_PLAYBACK(output->playback)->channel_thread[1],
					    TRUE, TRUE);
	      ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK(output->playback)->channel_thread[1]));
	    }
	  }
	  
	  output = output->next;
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

	/* parent mutex */
	pthread_mutex_lock(application_mutex);

	audio_thread_mutex = ags_mutex_manager_lookup(mutex_manager,
						      (GObject *) AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]);
  
	pthread_mutex_unlock(application_mutex);

	/* notation */
	output = audio->output;

	start_queue = NULL;
	
	while(output != NULL){
	  if(append_audio->do_notation){
	    g_atomic_int_or(&(AGS_CHANNEL_THREAD(AGS_PLAYBACK(output->playback)->channel_thread[2])->flags),
			    (AGS_CHANNEL_THREAD_WAIT |
			     AGS_CHANNEL_THREAD_DONE |
			     AGS_CHANNEL_THREAD_WAIT_SYNC |
			     AGS_CHANNEL_THREAD_DONE_SYNC));
	    start_queue = g_list_prepend(start_queue,
					 AGS_PLAYBACK(output->playback)->channel_thread[2]);

	    if(AGS_PLAYBACK(output->playback)->channel_thread[2]->parent == NULL){
	      ags_thread_add_child_extended(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2],
					    AGS_PLAYBACK(output->playback)->channel_thread[2],
					    TRUE, TRUE);
	      ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK(output->playback)->channel_thread[2]));
	    }
	  }
	  
	  output = output->next;
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
      
      if(append_audio->do_sequencer){
	g_atomic_int_or(&(AGS_AUDIO_THREAD(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1])->flags),
			(AGS_AUDIO_THREAD_WAIT |
			 AGS_AUDIO_THREAD_DONE |
			 AGS_AUDIO_THREAD_WAIT_SYNC |
			 AGS_AUDIO_THREAD_DONE_SYNC));
	start_queue = g_list_prepend(start_queue,
				     AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]);

	if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->parent == NULL){
	  ags_thread_add_child_extended((AgsThread *) audio_loop,
					AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]));
	}
      }

      if(append_audio->do_notation){
	g_atomic_int_or(&(AGS_AUDIO_THREAD(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2])->flags),
			(AGS_AUDIO_THREAD_WAIT |
			 AGS_AUDIO_THREAD_DONE |
			 AGS_AUDIO_THREAD_WAIT_SYNC |
			 AGS_AUDIO_THREAD_DONE_SYNC));
	start_queue = g_list_prepend(start_queue,
				     AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]);

	if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]->parent == NULL){
	  ags_thread_add_child_extended((AgsThread *) audio_loop,
					AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2],
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]));
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

  free(str0);
  free(str1);
  
  /* add to server registry */
  //  server = ags_service_provider_get_server(AGS_SERVICE_PROVIDER(audio_loop->application_context));

  //  if(server != NULL && (AGS_SERVER_RUNNING & (server->flags)) != 0){
  //    ags_connectable_add_to_registry(AGS_CONNECTABLE(append_audio->audio));
  //  }
}

/**
 * ags_append_audio_new:
 * @audio_loop: the #AgsAudioLoop
 * @audio: the #AgsAudio to append
 * @do_playback: playback scope
 * @do_sequencer: sequencer scope
 * @do_notation: notation scope
 *
 * Creates an #AgsAppendAudio.
 *
 * Returns: an new #AgsAppendAudio.
 *
 * Since: 0.4
 */
AgsAppendAudio*
ags_append_audio_new(GObject *audio_loop,
		     GObject *audio,
		     gboolean do_playback, gboolean do_sequencer, gboolean do_notation)
{
  AgsAppendAudio *append_audio;

  append_audio = (AgsAppendAudio *) g_object_new(AGS_TYPE_APPEND_AUDIO,
						 NULL);

  append_audio->audio_loop = audio_loop;
  append_audio->audio = audio;

  append_audio->do_playback = do_playback;
  append_audio->do_sequencer = do_sequencer;
  append_audio->do_notation = do_notation;
  
  return(append_audio);
}
