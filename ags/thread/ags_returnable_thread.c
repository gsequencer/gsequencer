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

#include <ags/thread/ags_returnable_thread.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_task_thread.h>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <unistd.h>
#include <errno.h>

#include <ags/i18n.h>

void ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread);
void ags_returnable_thread_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_returnable_thread_init(AgsReturnableThread *returnable_thread);
void ags_returnable_thread_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_returnable_thread_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_returnable_thread_connect(AgsConnectable *connectable);
void ags_returnable_thread_disconnect(AgsConnectable *connectable);
void ags_returnable_thread_dispose(GObject *gobject);
void ags_returnable_thread_finalize(GObject *gobject);

void ags_returnable_thread_start(AgsThread *thread);
void ags_returnable_thread_run(AgsThread *thread);
void ags_returnable_thread_stop(AgsThread *thread);
void ags_returnable_thread_resume(AgsThread *thread);

/**
 * SECTION:ags_returnable_thread
 * @short_description: returnable thread
 * @title: AgsReturnableThread
 * @section_id:
 * @include: ags/thread/ags_returnable_thread.h
 *
 * The #AgsReturnableThread acts as thread. It should return after a short
 * while because of limited thread pool.
 */

enum{
  PROP_0,
  PROP_THREAD_POOL,
};

enum{
  SAFE_RUN,
  LAST_SIGNAL,
};

static gpointer ags_returnable_thread_parent_class = NULL;
static AgsConnectableInterface *ags_returnable_thread_parent_connectable_interface;
static guint returnable_thread_signals[LAST_SIGNAL];

GType
ags_returnable_thread_get_type()
{
  static GType ags_type_returnable_thread = 0;

  if(!ags_type_returnable_thread){
    static const GTypeInfo ags_returnable_thread_info = {
      sizeof (AgsReturnableThreadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_returnable_thread_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsReturnableThread),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_returnable_thread_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_returnable_thread_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_returnable_thread = g_type_register_static(AGS_TYPE_THREAD,
							"AgsReturnableThread",
							&ags_returnable_thread_info,
							0);

    g_type_add_interface_static(ags_type_returnable_thread,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_returnable_thread);
}

void
ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread)
{
  AgsThreadClass *thread;

  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_returnable_thread_parent_class = g_type_class_peek_parent(returnable_thread);

  /* GObjectClass */
  gobject = (GObjectClass *) returnable_thread;

  gobject->set_property = ags_returnable_thread_set_property;
  gobject->get_property = ags_returnable_thread_get_property;

  gobject->dispose = ags_returnable_thread_dispose;
  gobject->finalize = ags_returnable_thread_finalize;

  /* properties */
  /**
   * AgsReturnableThread:thread-pool:
   *
   * The assigned #AgsThreadPool providing default settings.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("thread-pool",
				   i18n_pspec("assigned thread pool"),
				   i18n_pspec("The thread pool it is assigned with"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_THREAD_POOL,
				  param_spec);

  /* AgsThreadClass */
  thread = (AgsThreadClass *) returnable_thread;

  thread->start = ags_returnable_thread_start;
  thread->run = ags_returnable_thread_run;
  thread->stop = ags_returnable_thread_stop;
  thread->resume = ags_returnable_thread_resume;

  /* AgsReturnableThreadClass */
  returnable_thread->safe_run = NULL;

  /* signals */
  /**
   * AgsReturnableThread::safe-run:
   * @returnable_thread: the #AgsReturnableThread
   *
   * The ::safe-run is invoked durin AgsThread::run as
   * a context safe wrapper.
   *
   * Since: 0.7.122
   */
  returnable_thread_signals[SAFE_RUN] =
    g_signal_new("safe-run",
		 G_TYPE_FROM_CLASS (returnable_thread),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsReturnableThreadClass, safe_run),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

}

void
ags_returnable_thread_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_returnable_thread_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_returnable_thread_connect;
  connectable->disconnect = ags_returnable_thread_disconnect;
}

void
ags_returnable_thread_init(AgsReturnableThread *returnable_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(returnable_thread);

  g_atomic_int_or(&(thread->flags),
  		  AGS_THREAD_UNREF_ON_EXIT);

  thread->freq = AGS_RETURNABLE_THREAD_DEFAULT_JIFFIE;

  g_atomic_int_set(&(returnable_thread->flags),
		   AGS_RETURNABLE_THREAD_RUN_ONCE);

  returnable_thread->thread_pool = NULL;
  
  returnable_thread->reset_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(returnable_thread->reset_mutex, NULL);
  
  g_atomic_pointer_set(&(returnable_thread->safe_data),
		       NULL);

  returnable_thread->handler = 0;
}

void
ags_returnable_thread_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = AGS_RETURNABLE_THREAD(gobject);

  switch(prop_id){
  case PROP_THREAD_POOL:
    {
      GObject *thread_pool;

      thread_pool = g_value_get_object(value);

      if(returnable_thread->thread_pool == thread_pool)
	return;

      if(returnable_thread->thread_pool != NULL){
	g_object_unref(returnable_thread->thread_pool);
      }
      
      if(thread_pool != NULL){
	g_object_ref(thread_pool);
      }
      
      returnable_thread->thread_pool = thread_pool;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_returnable_thread_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = AGS_RETURNABLE_THREAD(gobject);

  switch(prop_id){
  case PROP_THREAD_POOL:
    {
      g_value_set_object(value, returnable_thread->thread_pool);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_returnable_thread_connect(AgsConnectable *connectable)
{
  ags_returnable_thread_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_returnable_thread_disconnect(AgsConnectable *connectable)
{
  ags_returnable_thread_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_returnable_thread_dispose(GObject *gobject)
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = (AgsReturnableThread *) gobject;

  /* thread pool */
  if(returnable_thread->thread_pool != NULL){
    g_object_unref(returnable_thread->thread_pool);

    returnable_thread->thread_pool = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_returnable_thread_parent_class)->dispose(gobject);
}

void
ags_returnable_thread_finalize(GObject *gobject)
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = (AgsReturnableThread *) gobject;

  /* thread pool */
  if(returnable_thread->thread_pool != NULL){
    g_object_unref(returnable_thread->thread_pool);
  }

  /* reset mutex */
  pthread_mutex_destroy(AGS_RETURNABLE_THREAD(gobject)->reset_mutex);

  free(AGS_RETURNABLE_THREAD(gobject)->reset_mutex);
  
  /* call parent */
  G_OBJECT_CLASS(ags_returnable_thread_parent_class)->finalize(gobject);
}

void
ags_returnable_thread_start(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_returnable_thread_parent_class)->start(thread);
}

void
ags_returnable_thread_run(AgsThread *thread)
{
  AgsReturnableThread *returnable_thread;
  AgsTaskThread *task_thread;
  GList *tmplist;

  gboolean unref_thread;
  //  g_message("reset:0");
  
  /* retrieve some variables */
  returnable_thread = AGS_RETURNABLE_THREAD(thread);
  
  unref_thread = FALSE;
  
  /* safe run */
  if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) != 0){

    ags_returnable_thread_safe_run(returnable_thread);

    if((AGS_RETURNABLE_THREAD_RUN_ONCE & (g_atomic_int_get(&(returnable_thread->flags)))) != 0){
      g_atomic_int_and(&(returnable_thread->flags),
		       (~AGS_RETURNABLE_THREAD_IN_USE));
      ags_thread_stop(thread);
      
      unref_thread = TRUE;
    }

    task_thread = (AgsTaskThread *) ags_thread_find_type(ags_thread_get_toplevel(AGS_THREAD_POOL(returnable_thread->thread_pool)->parent),
							 AGS_TYPE_TASK_THREAD);
    ags_task_thread_clear_cache(task_thread);
  }

  if(unref_thread){
    //    g_object_unref(thread);
  }
}

void
ags_returnable_thread_safe_run(AgsReturnableThread *returnable_thread)
{
  guint returnable_thread_signal;

  returnable_thread_signal = returnable_thread_signals[SAFE_RUN];

  g_return_if_fail(AGS_IS_RETURNABLE_THREAD(returnable_thread));

  g_object_ref(returnable_thread);
  g_signal_emit(returnable_thread,
		returnable_thread_signal, 0);
  g_object_unref(returnable_thread);
}

void
ags_returnable_thread_stop(AgsThread *thread)
{
  AGS_THREAD_CLASS(ags_returnable_thread_parent_class)->stop(thread);
}

void
ags_returnable_thread_resume(AgsThread *thread)
{
  /* empty */
}

/**
 * ags_returnable_thread_connect_safe_run:
 * @returnable_thread: the thread to connect
 * @callback: the callback
 *
 * Connects @callback to @thread.
 *
 * Since: 0.4
 */
void
ags_returnable_thread_connect_safe_run(AgsReturnableThread *returnable_thread, AgsReturnableThreadCallback callback)
{
  if(returnable_thread->handler > 0){
    return;
  }

  returnable_thread->handler = g_signal_connect(G_OBJECT(returnable_thread), "safe-run",
						G_CALLBACK(callback), returnable_thread);
}

/**
 * ags_returnable_thread_disconnect_safe_run:
 * @returnable_thread: the thread to disconnect
 *
 * Disconnects callback of @thread.
 *
 * Since: 0.4
 */
void
ags_returnable_thread_disconnect_safe_run(AgsReturnableThread *returnable_thread)
{
  if(returnable_thread->handler == 0){
    return;
  }
  
  g_signal_handler_disconnect(G_OBJECT(returnable_thread),
			      returnable_thread->handler);

  returnable_thread->handler = 0;
}

/**
 * ags_returnable_thread_new:
 * @thread_pool: the #AgsThreadPool
 *
 * Create a new #AgsReturnableThread.
 *
 * Returns: the new #AgsReturnableThread
 *
 * Since: 0.4
 */
AgsReturnableThread*
ags_returnable_thread_new(GObject *thread_pool)
{
  AgsReturnableThread *returnable_thread;

  returnable_thread = (AgsReturnableThread *) g_object_new(AGS_TYPE_RETURNABLE_THREAD,
							   "thread-pool", thread_pool,
							   NULL);
  
  return(returnable_thread);
}
