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

#include <ags/audio/task/ags_cancel_audio.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/audio/thread/ags_audio_thread.h>

#include <ags/i18n.h>

void ags_cancel_audio_class_init(AgsCancelAudioClass *cancel_audio);
void ags_cancel_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cancel_audio_init(AgsCancelAudio *cancel_audio);
void ags_cancel_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_cancel_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_cancel_audio_connect(AgsConnectable *connectable);
void ags_cancel_audio_disconnect(AgsConnectable *connectable);
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
static AgsConnectableInterface *ags_cancel_audio_parent_connectable_interface;

enum{
  PROP_0,
  PROP_AUDIO,
  PROP_DO_PLAYBACK,
  PROP_DO_SEQUENCER,
  PROP_DO_NOTATION,
};

GType
ags_cancel_audio_get_type()
{
  static GType ags_type_cancel_audio = 0;

  if(!ags_type_cancel_audio){
    static const GTypeInfo ags_cancel_audio_info = {
      sizeof (AgsCancelAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCancelAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cancel_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_cancel_audio = g_type_register_static(AGS_TYPE_TASK,
						   "AgsCancelAudio",
						   &ags_cancel_audio_info,
						   0);
    
    g_type_add_interface_static(ags_type_cancel_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_cancel_audio);
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
   * Since: 1.0.0
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
   * AgsCancelAudio:do-playback:
   *
   * The effects do-playback.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("do-playback",
				     i18n_pspec("do playback"),
				     i18n_pspec("Do playback of audio"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_PLAYBACK,
				  param_spec);

  /**
   * AgsCancelAudio:do-sequencer:
   *
   * The effects do-sequencer.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("do-sequencer",
				     i18n_pspec("do sequencer"),
				     i18n_pspec("Do sequencer of audio"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_SEQUENCER,
				  param_spec);

  /**
   * AgsCancelAudio:do-notation:
   *
   * The effects do-notation.
   * 
   * Since: 1.0.0
   */
  param_spec =  g_param_spec_boolean("do-notation",
				     i18n_pspec("do notation"),
				     i18n_pspec("Do notation of audio"),
				     FALSE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_NOTATION,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) cancel_audio;

  task->launch = ags_cancel_audio_launch;
}

void
ags_cancel_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_cancel_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_cancel_audio_connect;
  connectable->disconnect = ags_cancel_audio_disconnect;
}

void
ags_cancel_audio_init(AgsCancelAudio *cancel_audio)
{
  cancel_audio->audio = NULL;

  cancel_audio->do_playback = FALSE;
  cancel_audio->do_sequencer = FALSE;
  cancel_audio->do_notation = FALSE;
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

      if(cancel_audio->audio == (GObject *) audio){
	return;
      }

      if(cancel_audio->audio != NULL){
	g_object_unref(cancel_audio->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);
      }

      cancel_audio->audio = (GObject *) audio;
    }
    break;
  case PROP_DO_PLAYBACK:
    {
      guint do_playback;

      do_playback = g_value_get_boolean(value);

      cancel_audio->do_playback = do_playback;
    }
    break;
  case PROP_DO_SEQUENCER:
    {
      guint do_sequencer;

      do_sequencer = g_value_get_boolean(value);

      cancel_audio->do_sequencer = do_sequencer;
    }
    break;
  case PROP_DO_NOTATION:
    {
      guint do_notation;

      do_notation = g_value_get_boolean(value);

      cancel_audio->do_notation = do_notation;
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
  case PROP_DO_PLAYBACK:
    {
      g_value_set_boolean(value, cancel_audio->do_playback);
    }
    break;
  case PROP_DO_SEQUENCER:
    {
      g_value_set_boolean(value, cancel_audio->do_sequencer);
    }
    break;
  case PROP_DO_NOTATION:
    {
      g_value_set_boolean(value, cancel_audio->do_notation);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_audio_connect(AgsConnectable *connectable)
{
  ags_cancel_audio_parent_connectable_interface->connect(connectable);


  /* empty */
}

void
ags_cancel_audio_disconnect(AgsConnectable *connectable)
{
  ags_cancel_audio_parent_connectable_interface->disconnect(connectable);

  /* empty */
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
  AgsPlaybackDomain *playback_domain;
  AgsPlayback *playback;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecallID *recall_id;

  AgsCancelAudio *cancel_audio;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  cancel_audio = AGS_CANCEL_AUDIO(task);

  audio = cancel_audio->audio;

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(audio_mutex);
  
  playback_domain = AGS_PLAYBACK_DOMAIN(audio->playback_domain);

  pthread_mutex_unlock(audio_mutex);
  
  /* cancel playback */
  if(cancel_audio->do_playback){
    g_atomic_int_and(&(playback_domain->flags),
		     (~AGS_PLAYBACK_DOMAIN_PLAYBACK));
    /* get some fields */
    pthread_mutex_lock(audio_mutex);
    
    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);
    
    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      playback = AGS_PLAYBACK(channel->playback);
      recall_id = playback->recall_id[AGS_PLAYBACK_SCOPE_PLAYBACK];
      
      pthread_mutex_unlock(channel_mutex);

      if(recall_id == NULL){
	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);

	continue;
      }

      /* cancel */
      g_object_ref(recall_id);
      ags_channel_tillrecycling_cancel(channel,
				       recall_id);

      /* set recall id to NULL and iterate */
      pthread_mutex_lock(channel_mutex);

      playback->recall_id[AGS_PLAYBACK_SCOPE_PLAYBACK] = NULL;

      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }

    if((AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO & (g_atomic_int_get(&(playback_domain->flags)))) != 0){
      AgsAudioThread *audio_thread;

      /* get some fields */
      pthread_mutex_lock(audio_mutex);
      
      audio_thread = (AgsAudioThread *) playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_PLAYBACK];

      pthread_mutex_unlock(audio_mutex);
      
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(audio_thread)->flags)))) != 0){
	ags_thread_stop(audio_thread);

	/* ensure synced */
	pthread_mutex_lock(audio_thread->wakeup_mutex);

	g_atomic_int_and(&(audio_thread->flags),
			 (~AGS_AUDIO_THREAD_WAIT));

	if((AGS_AUDIO_THREAD_DONE & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
	  pthread_cond_signal(audio_thread->wakeup_cond);
	}
      
	pthread_mutex_unlock(audio_thread->wakeup_mutex);
      }
    }
  }
  
  /* cancel sequencer */
  if(cancel_audio->do_sequencer){
    g_atomic_int_and(&(playback_domain->flags),
		     (~AGS_PLAYBACK_DOMAIN_SEQUENCER));
    /* get some fields */
    pthread_mutex_lock(audio_mutex);
    
    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      playback = AGS_PLAYBACK(channel->playback);
      recall_id = playback->recall_id[AGS_PLAYBACK_SCOPE_SEQUENCER];
      
      pthread_mutex_unlock(channel_mutex);

      if(recall_id == NULL){
	pthread_mutex_lock(channel_mutex);

	channel = channel->next;

	pthread_mutex_unlock(channel_mutex);
	
	continue;
      }

      g_object_ref(recall_id);
      ags_channel_tillrecycling_cancel(channel,
				       recall_id);

      /* set recall id to NULL and iterate */
      pthread_mutex_lock(channel_mutex);

      playback->recall_id[AGS_PLAYBACK_SCOPE_SEQUENCER] = NULL;

      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }

    if((AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO & (g_atomic_int_get(&(playback_domain->flags)))) != 0){
      AgsAudioThread *audio_thread;

      audio_thread = (AgsAudioThread *) playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_SEQUENCER];
      
      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(audio_thread)->flags)))) != 0){
	ags_thread_stop(audio_thread);

	/* ensure synced */
	pthread_mutex_lock(audio_thread->wakeup_mutex);

	g_atomic_int_and(&(audio_thread->flags),
			 (~AGS_AUDIO_THREAD_WAIT));

	if((AGS_AUDIO_THREAD_DONE & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
	  pthread_cond_signal(audio_thread->wakeup_cond);
	}
      
	pthread_mutex_unlock(audio_thread->wakeup_mutex);
      }
    }
  }
  
  /* cancel notation */
  if(cancel_audio->do_notation){
    g_atomic_int_and(&(playback_domain->flags),
		     (~AGS_PLAYBACK_DOMAIN_NOTATION));
    /* get some fields */
    pthread_mutex_lock(audio_mutex);
    
    channel = audio->output;

    pthread_mutex_unlock(audio_mutex);

    while(channel != NULL){
      /* get channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) channel);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(channel_mutex);

      playback = AGS_PLAYBACK(channel->playback);
      recall_id = playback->recall_id[AGS_PLAYBACK_SCOPE_NOTATION];
      
      pthread_mutex_unlock(channel_mutex);

      if(recall_id == NULL){
	channel = channel->next;
	
	continue;
      }

      g_object_ref(recall_id);
      ags_channel_tillrecycling_cancel(channel,
				       recall_id);

      /* set recall id to NULL and iterate */
      pthread_mutex_lock(channel_mutex);
      
      playback->recall_id[AGS_PLAYBACK_SCOPE_NOTATION] = NULL;
      
      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
    }

    if((AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO & (g_atomic_int_get(&(playback_domain->flags)))) != 0){
      AgsAudioThread *audio_thread;

      audio_thread = (AgsAudioThread *) playback_domain->audio_thread[AGS_PLAYBACK_DOMAIN_SCOPE_NOTATION];

      if((AGS_THREAD_RUNNING & (g_atomic_int_get(&(AGS_THREAD(audio_thread)->flags)))) != 0){
	ags_thread_stop(audio_thread);

	/* ensure synced */
	pthread_mutex_lock(audio_thread->wakeup_mutex);

	g_atomic_int_and(&(audio_thread->flags),
			 (~AGS_AUDIO_THREAD_WAIT));

	if((AGS_AUDIO_THREAD_DONE & (g_atomic_int_get(&(audio_thread->flags)))) == 0){
	  pthread_cond_signal(audio_thread->wakeup_cond);
	}
      
	pthread_mutex_unlock(audio_thread->wakeup_mutex);
      }
    }
  }
}

/**
 * ags_cancel_audio_new:
 * @audio: the #AgsAudio to cancel
 * @do_playback: if %TRUE playback is canceld
 * @do_sequencer: if %TRUE sequencer is canceld
 * @do_notation: if %TRUE notation is canceld
 *
 * Creates an #AgsCancelAudio.
 *
 * Returns: an new #AgsCancelAudio.
 *
 * Since: 1.0.0
 */
AgsCancelAudio*
ags_cancel_audio_new(AgsAudio *audio,
		     gboolean do_playback, gboolean do_sequencer, gboolean do_notation)
{
  AgsCancelAudio *cancel_audio;

  cancel_audio = (AgsCancelAudio *) g_object_new(AGS_TYPE_CANCEL_AUDIO,
						 "audio", audio,
						 "do-playback", do_playback,
						 "do-sequencer", do_sequencer,
						 "do-notation", do_notation,
						 NULL);


  return(cancel_audio);
}
