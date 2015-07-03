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

#include <ags/thread/ags_channel_thread.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/thread/ags_timestamp_thread.h>

#include <ags/audio/ags_channel.h>

void ags_channel_thread_class_init(AgsChannelThreadClass *channel_thread);
void ags_channel_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_thread_init(AgsChannelThread *channel_thread);
void ags_channel_thread_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_channel_thread_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_channel_thread_connect(AgsConnectable *connectable);
void ags_channel_thread_disconnect(AgsConnectable *connectable);
void ags_channel_thread_finalize(GObject *gobject);

void ags_channel_thread_start(AgsThread *thread);
void ags_channel_thread_run(AgsThread *thread);
void ags_channel_thread_stop(AgsThread *thread);

/**
 * SECTION:ags_channel_thread
 * @short_description: channel thread
 * @title: AgsChannelThread
 * @section_id:
 * @include: ags/thread/ags_channel_thread.h
 *
 * The #AgsChannelThread acts as channel output thread to soundcard.
 */

static gpointer ags_channel_thread_parent_class = NULL;
static AgsConnectableInterface *ags_channel_thread_parent_connectable_interface;

enum{
  PROP_0,
  PROP_CHANNEL,
};

GType
ags_channel_thread_get_type()
{
  static GType ags_type_channel_thread = 0;

  if(!ags_type_channel_thread){
    static const GTypeInfo ags_channel_thread_info = {
      sizeof (AgsChannelThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_thread = g_type_register_static(AGS_TYPE_THREAD,
						     "AgsChannelThread\0",
						     &ags_channel_thread_info,
						     0);
    
    g_type_add_interface_static(ags_type_channel_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_channel_thread);
}

void
ags_channel_thread_class_init(AgsChannelThreadClass *channel_thread)
{
  GObjectClass *gobject;
  AgsThreadClass *thread;
  GParamSpec *param_spec;
  
  ags_channel_thread_parent_class = g_type_class_peek_parent(channel_thread);

  /* GObject */
  gobject = (GObjectClass *) channel_thread;
    
  gobject->set_property = ags_channel_thread_set_property;
  gobject->get_property = ags_channel_thread_get_property;

  gobject->finalize = ags_channel_thread_finalize;

  /* properties */
  /**
   * AgsChannelThread:channel:
   *
   * The assigned #AgsChannel.
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_object("channel\0",
				   "channel assigned to\0",
				   "The AgsChannel it is assigned to.\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);

  /* AgsThread */
  thread = (AgsThreadClass *) channel_thread;

  thread->start = ags_channel_thread_start;
  thread->run = ags_channel_thread_run;
  thread->stop = ags_channel_thread_stop;
}

void
ags_channel_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_channel_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_channel_thread_connect;
  connectable->disconnect = ags_channel_thread_disconnect;
}

void
ags_channel_thread_init(AgsChannelThread *channel_thread)
{
  g_atomic_int_set(&(channel_thread->flags),
		   0);

  /* start */
  pthread_mutexattr_init(&(channel_thread->wakeup_attr));
  pthread_mutexattr_settype(&(channel_thread->wakeup_attr),
			    PTHREAD_MUTEX_RECURSIVE);

  channel_thread->wakeup_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(channel_thread->wakeup_mutex,
		     &(channel_thread->wakeup_attr));
  
  channel_thread->wakeup_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(channel_thread->wakeup_cond, NULL);

  /* sync */
  pthread_mutexattr_init(&(channel_thread->done_attr));
  pthread_mutexattr_settype(&(channel_thread->done_attr),
			    PTHREAD_MUTEX_RECURSIVE);

  channel_thread->done_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(channel_thread->done_mutex,
		     &(channel_thread->done_attr));
  
  channel_thread->done_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(channel_thread->done_cond, NULL);
}

void
ags_channel_thread_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsChannelThread *channel_thread;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      if(channel_thread->channel != NULL){
	g_object_unref(G_OBJECT(channel_thread->channel));
      }

      if(channel != NULL){
	g_object_ref(G_OBJECT(channel));
      }

      channel_thread->channel = G_OBJECT(channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_thread_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsChannelThread *channel_thread;

  channel_thread = AGS_CHANNEL_THREAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value, G_OBJECT(channel_thread->channel));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_channel_thread_connect(AgsConnectable *connectable)
{
  ags_channel_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_channel_thread_disconnect(AgsConnectable *connectable)
{
  ags_channel_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_channel_thread_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_channel_thread_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_channel_thread_start(AgsThread *thread)
{
  //TODO:JK: implement me
  
  AGS_THREAD_CLASS(ags_channel_thread_parent_class)->start(thread);
}

void
ags_channel_thread_run(AgsThread *thread)
{
  //TODO:JK: implement me
}

void
ags_channel_thread_stop(AgsThread *thread)
{
  //TODO:JK: implement me
  
  AGS_THREAD_CLASS(ags_channel_thread_parent_class)->stop(thread);
}

/**
 * ags_channel_thread_new:
 * @devout: the #AgsDevout
 * @channel: the #AgsChannel
 *
 * Create a new #AgsChannelThread.
 *
 * Returns: the new #AgsChannelThread
 *
 * Since: 0.4.2
 */
AgsChannelThread*
ags_channel_thread_new(GObject *devout,
		       GObject *channel)
{
  AgsChannelThread *channel_thread;

  channel_thread = (AgsChannelThread *) g_object_new(AGS_TYPE_CHANNEL_THREAD,
						     "devout\0", devout,
						     "channel\0", channel,
						     NULL);


  return(channel_thread);
}
