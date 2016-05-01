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
						   "AgsAppendAudio\0",
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

  ags_append_audio_parent_class = g_type_class_peek_parent(append_audio);

  /* gobject */
  gobject = (GObjectClass *) append_audio;

  gobject->finalize = ags_append_audio_finalize;

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
  AgsSoundcardThread *soundcard_thread;
  
  AgsServer *server;

  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  GList *start_queue;
      
  gchar *str0, *str1;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *soundcard_thread_mutex;

  append_audio = AGS_APPEND_AUDIO(task);
  
  audio_loop = AGS_AUDIO_LOOP(append_audio->audio_loop);
  
  audio = append_audio->audio;

  soundcard_thread = ags_thread_find_type(audio_loop,
					  AGS_TYPE_SOUNDCARD_THREAD);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  soundcard_thread_mutex = ags_mutex_manager_lookup(mutex_manager,
						    soundcard_thread);
  
  pthread_mutex_unlock(application_mutex);

  /* append to AgsAudioLoop */
  ags_audio_loop_add_audio(audio_loop,
			   audio);

  /**/
  config = ags_config_get_instance();
  
  str0 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "model\0");
  
  str1 = ags_config_get_value(config,
			      AGS_CONFIG_THREAD,
			      "super-threaded-scope\0");

  start_queue = NULL;
      
  if(!g_ascii_strncasecmp(str0,
			  "super-threaded\0",
			  15)){
    if(!g_ascii_strncasecmp(str1,
			    "audio\0",
			    6) ||
       !g_ascii_strncasecmp(str1,
			    "channel\0",
			    8)){
      /* super threaded setup - audio */
      if(append_audio->do_sequencer){
	start_queue = g_list_prepend(start_queue,
				     AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]);

	if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]->parent == NULL){
	  ags_thread_add_child_extended(soundcard_thread,
					AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1]));
	}
      }

      if(append_audio->do_notation){
	start_queue = g_list_prepend(start_queue,
				     AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]);

	if(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]->parent == NULL){
	  ags_thread_add_child_extended(soundcard_thread,
					AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2],
					TRUE, TRUE);
	  ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[2]));
	}
      }
	
      /* super threaed setup - channel */
      if(!g_ascii_strncasecmp(str1,
			      "channel\0",
			      8)){
	AgsChannel *output;

	output = audio->output;
      
	while(output != NULL){
	  if(append_audio->do_sequencer){
	    start_queue = g_list_prepend(start_queue,
					 AGS_PLAYBACK(output->playback)->channel_thread[1]);

	    if(AGS_PLAYBACK(output->playback)->channel_thread[1]->parent == NULL){
	      ags_thread_add_child_extended(AGS_PLAYBACK_DOMAIN(audio->playback_domain)->audio_thread[1],
					    AGS_PLAYBACK(output->playback)->channel_thread[1],
					    TRUE, TRUE);
	      ags_connectable_connect(AGS_CONNECTABLE(AGS_PLAYBACK(output->playback)->channel_thread[1]));
	    }
	  }

	  if(append_audio->do_notation){
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
      }
    }
  }

  free(str0);
  free(str1);

  /* start queue */
  pthread_mutex_lock(soundcard_thread_mutex);

  if(start_queue != NULL){
    if(g_atomic_pointer_get(&(AGS_THREAD(soundcard_thread)->start_queue)) != NULL){
      g_atomic_pointer_set(&(AGS_THREAD(soundcard_thread)->start_queue),
			   g_list_concat(start_queue,
					 g_atomic_pointer_get(&(AGS_THREAD(soundcard_thread)->start_queue))));
    }else{
      g_atomic_pointer_set(&(AGS_THREAD(soundcard_thread)->start_queue),
			   start_queue);
    }
  }

  pthread_mutex_unlock(soundcard_thread_mutex);
  
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
