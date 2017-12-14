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

#include <ags/audio/task/ags_cancel_channel.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>

#include <ags/i18n.h>

void ags_cancel_channel_class_init(AgsCancelChannelClass *cancel_channel);
void ags_cancel_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cancel_channel_init(AgsCancelChannel *cancel_channel);
void ags_cancel_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_cancel_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_cancel_channel_connect(AgsConnectable *connectable);
void ags_cancel_channel_disconnect(AgsConnectable *connectable);
void ags_cancel_channel_dispose(GObject *gobject);
void ags_cancel_channel_finalize(GObject *gobject);

void ags_cancel_channel_launch(AgsTask *task);

/**
 * SECTION:ags_cancel_channel
 * @short_description: cancel channel task
 * @title: AgsCancelChannel
 * @section_id:
 * @include: ags/audio/task/ags_cancel_channel.h
 *
 * The #AgsCancelChannel task cancels #AgsChannel playback.
 */

static gpointer ags_cancel_channel_parent_class = NULL;
static AgsConnectableInterface *ags_cancel_channel_parent_connectable_interface;

enum{
  PROP_0,
  PROP_CHANNEL,
  PROP_RECALL_ID,
  PROP_PLAYBACK,
};

GType
ags_cancel_channel_get_type()
{
  static GType ags_type_cancel_channel = 0;

  if(!ags_type_cancel_channel){
    static const GTypeInfo ags_cancel_channel_info = {
      sizeof (AgsCancelChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCancelChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cancel_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_cancel_channel = g_type_register_static(AGS_TYPE_TASK,
						     "AgsCancelChannel",
						     &ags_cancel_channel_info,
						     0);
    
    g_type_add_interface_static(ags_type_cancel_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_cancel_channel);
}

void
ags_cancel_channel_class_init(AgsCancelChannelClass *cancel_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;
  GParamSpec *param_spec;
  
  ags_cancel_channel_parent_class = g_type_class_peek_parent(cancel_channel);

  /* gobject */
  gobject = (GObjectClass *) cancel_channel;

  gobject->set_property = ags_cancel_channel_set_property;
  gobject->get_property = ags_cancel_channel_get_property;

  gobject->dispose = ags_cancel_channel_dispose;
  gobject->finalize = ags_cancel_channel_finalize;

  /* properties */
  /**
   * AgsCancelChannel:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel of cancel channel"),
				   i18n_pspec("The channel of cancel channel task"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /**
   * AgsCancelChannel:recall-id:
   *
   * The assigned #AgsRecallID
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("recall-id",
				   i18n_pspec("recall id of cancel channel"),
				   i18n_pspec("The recall id of cancel channel task"),
				   AGS_TYPE_RECALL_ID,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECALL_ID,
				  param_spec);

  /**
   * AgsCancelChannel:playback:
   *
   * The assigned #AgsPlayback
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("playback",
				   i18n_pspec("playback of cancel channel"),
				   i18n_pspec("The playback of cancel channel task"),
				   AGS_TYPE_PLAYBACK,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) cancel_channel;

  task->launch = ags_cancel_channel_launch;
}

void
ags_cancel_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_cancel_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_cancel_channel_connect;
  connectable->disconnect = ags_cancel_channel_disconnect;
}

void
ags_cancel_channel_init(AgsCancelChannel *cancel_channel)
{
  cancel_channel->channel = NULL;
  cancel_channel->recall_id = NULL;

  cancel_channel->playback = NULL;
}

void
ags_cancel_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(cancel_channel->channel == (GObject *) channel){
	return;
      }

      if(cancel_channel->channel != NULL){
	g_object_unref(cancel_channel->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      cancel_channel->channel = (GObject *) channel;
    }
    break;
  case PROP_RECALL_ID:
    {
      AgsRecallID *recall_id;

      recall_id = (AgsRecallID *) g_value_get_object(value);

      if(cancel_channel->recall_id == (GObject *) recall_id){
	return;
      }

      if(cancel_channel->recall_id != NULL){
	g_object_unref(cancel_channel->recall_id);
      }

      if(recall_id != NULL){
	g_object_ref(recall_id);
      }

      cancel_channel->recall_id = (GObject *) recall_id;
    }
    break;
  case PROP_PLAYBACK:
    {
      AgsPlayback *playback;

      playback = (AgsPlayback *) g_value_get_object(value);

      if(cancel_channel->playback == (GObject *) playback){
	return;
      }

      if(cancel_channel->playback != NULL){
	g_object_unref(cancel_channel->playback);
      }

      if(playback != NULL){
	g_object_ref(playback);
      }

      cancel_channel->playback = (GObject *) playback;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, cancel_channel->channel);
    }
    break;
  case PROP_RECALL_ID:
    {
      g_value_set_object(value, cancel_channel->recall_id);
    }
    break;
  case PROP_PLAYBACK:
    {
      g_value_set_object(value, cancel_channel->playback);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_cancel_channel_connect(AgsConnectable *connectable)
{
  ags_cancel_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_cancel_channel_disconnect(AgsConnectable *connectable)
{
  ags_cancel_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_cancel_channel_dispose(GObject *gobject)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  if(cancel_channel->channel != NULL){
    g_object_unref(cancel_channel->channel);

    cancel_channel->channel = NULL;
  }

  if(cancel_channel->recall_id != NULL){
    g_object_unref(cancel_channel->recall_id);

    cancel_channel->recall_id = NULL;
  }

  if(cancel_channel->playback != NULL){
    g_object_unref(cancel_channel->playback);

    cancel_channel->playback = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_cancel_channel_parent_class)->dispose(gobject);
}

void
ags_cancel_channel_finalize(GObject *gobject)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = AGS_CANCEL_CHANNEL(gobject);

  if(cancel_channel->channel != NULL){
    g_object_unref(cancel_channel->channel);
  }

  if(cancel_channel->recall_id != NULL){
    g_object_unref(cancel_channel->recall_id);
  }

  if(cancel_channel->playback != NULL){
    g_object_unref(cancel_channel->playback);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_cancel_channel_parent_class)->finalize(gobject);
}

void
ags_cancel_channel_launch(AgsTask *task)
{
  AgsPlayback *playback;
  AgsChannel *channel;
  AgsRecallID *recall_id;
  
  AgsCancelChannel *cancel_channel;

  AgsChannelThread *channel_thread;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  cancel_channel = AGS_CANCEL_CHANNEL(task);

  channel = cancel_channel->channel;

  /* get channel mutex */
  pthread_mutex_lock(application_mutex);

  channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel);
  
  pthread_mutex_unlock(application_mutex);

  /* get some fields */
  pthread_mutex_lock(channel_mutex);

  playback = AGS_PLAYBACK(channel->playback);

  recall_id = playback->recall_id[AGS_PLAYBACK_SCOPE_PLAYBACK];

  channel_thread = playback->channel_thread[AGS_PLAYBACK_SCOPE_PLAYBACK];
    
  pthread_mutex_unlock(channel_mutex);
  
  /* cancel playback */
  if(cancel_channel->recall_id != NULL &&
     cancel_channel->recall_id == recall_id){
    g_atomic_int_and(&(playback->flags),
		     (~AGS_PLAYBACK_PLAYBACK));

    g_object_ref(recall_id);
    ags_channel_tillrecycling_cancel(channel,
				     recall_id);
    
    pthread_mutex_lock(channel_mutex);
    
    playback->recall_id[AGS_PLAYBACK_SCOPE_PLAYBACK] = NULL;

    pthread_mutex_unlock(channel_mutex);

    if((AGS_PLAYBACK_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(playback->flags)))) != 0){
      ags_thread_stop(channel_thread);
    }
  }

  /* cancel sequencer */
  pthread_mutex_lock(channel_mutex);

  recall_id = playback->recall_id[AGS_PLAYBACK_SCOPE_SEQUENCER];

  channel_thread = playback->channel_thread[AGS_PLAYBACK_SCOPE_SEQUENCER];
    
  pthread_mutex_unlock(channel_mutex);

  if(cancel_channel->recall_id != NULL &&
     cancel_channel->recall_id == recall_id){
    g_atomic_int_and(&(playback->flags),
		     (~AGS_PLAYBACK_SEQUENCER));

    g_object_ref(recall_id);
    ags_channel_tillrecycling_cancel(channel,
				     recall_id);

    pthread_mutex_lock(channel_mutex);
    
    playback->recall_id[AGS_PLAYBACK_SCOPE_SEQUENCER] = NULL;

    pthread_mutex_unlock(channel_mutex);

    if((AGS_PLAYBACK_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(playback->flags)))) != 0){
      ags_thread_stop(channel_thread);
    }
  }

  /* cancel notation */
  pthread_mutex_lock(channel_mutex);

  recall_id = playback->recall_id[AGS_PLAYBACK_SCOPE_NOTATION];

  channel_thread = playback->channel_thread[AGS_PLAYBACK_SCOPE_NOTATION];
    
  pthread_mutex_unlock(channel_mutex);

  if(cancel_channel->recall_id != NULL &&
     cancel_channel->recall_id == recall_id){
    g_atomic_int_and(&(playback->flags),
		     (~AGS_PLAYBACK_NOTATION));

    g_object_ref(recall_id);
    ags_channel_tillrecycling_cancel(channel,
				     recall_id);

    pthread_mutex_lock(channel_mutex);
    
    playback->recall_id[AGS_PLAYBACK_SCOPE_NOTATION] = NULL;

    pthread_mutex_unlock(channel_mutex);

    if((AGS_PLAYBACK_SUPER_THREADED_CHANNEL & (g_atomic_int_get(&(playback->flags)))) != 0){
      ags_thread_stop(channel_thread);
    }
  }

  ags_channel_done(channel,
		   cancel_channel->recall_id);		   
}

/**
 * ags_cancel_channel_new:
 * @channel: the #AgsChannel to cancel
 * @recall_id: the #AgsRecallID to cancel
 * @playback: the #AgsPlayback
 *
 * Creates an #AgsCancelChannel.
 *
 * Returns: an new #AgsCancelChannel.
 *
 * Since: 1.0.0
 */
AgsCancelChannel*
ags_cancel_channel_new(AgsChannel *channel, AgsRecallID *recall_id,
		       GObject *playback)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = (AgsCancelChannel *) g_object_new(AGS_TYPE_CANCEL_CHANNEL,
						     "channel", channel,
						     "recall-id", recall_id,
						     "playback", playback,
						     NULL);

  return(cancel_channel);
}
