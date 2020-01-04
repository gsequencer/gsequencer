/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <unistd.h>
#include <errno.h>

#include <ags/i18n.h>

void ags_returnable_thread_class_init(AgsReturnableThreadClass *returnable_thread);
void ags_returnable_thread_init(AgsReturnableThread *returnable_thread);
void ags_returnable_thread_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_returnable_thread_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
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
  PROP_SAFE_DATA,
};

enum{
  SAFE_RUN,
  LAST_SIGNAL,
};

static gpointer ags_returnable_thread_parent_class = NULL;
static guint returnable_thread_signals[LAST_SIGNAL];

GType
ags_returnable_thread_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_returnable_thread = 0;

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

    ags_type_returnable_thread = g_type_register_static(AGS_TYPE_THREAD,
							"AgsReturnableThread",
							&ags_returnable_thread_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_returnable_thread);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
ags_returnable_thread_init(AgsReturnableThread *returnable_thread)
{
  AgsThread *thread;

  thread = AGS_THREAD(returnable_thread);

  ags_thread_set_flags(thread, AGS_THREAD_UNREF_ON_EXIT);

  g_object_set(thread,
	       "frequency", AGS_RETURNABLE_THREAD_DEFAULT_JIFFIE,
	       NULL);

  g_atomic_int_set(&(returnable_thread->flags),
		   AGS_RETURNABLE_THREAD_RUN_ONCE);

  returnable_thread->thread_pool = NULL;
  
  g_rec_mutex_init(&(returnable_thread->reset_mutex));
  
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
  AgsThreadPool *thread_pool;
  AgsReturnableThread *returnable_thread;
  
  GRecMutex *thread_mutex;
  
  //  g_message("reset:0");
  
  /* retrieve some variables */
  returnable_thread = AGS_RETURNABLE_THREAD(thread);
  
  /* safe run */
  if((AGS_RETURNABLE_THREAD_IN_USE & (g_atomic_int_get(&(returnable_thread->flags)))) != 0){
    ags_returnable_thread_safe_run(returnable_thread);

    if((AGS_RETURNABLE_THREAD_RUN_ONCE & (g_atomic_int_get(&(returnable_thread->flags)))) != 0){
      g_atomic_int_and(&(returnable_thread->flags),
		       (~AGS_RETURNABLE_THREAD_IN_USE));

      /* return to thread pool */
      thread_mutex = AGS_THREAD_GET_OBJ_MUTEX(thread);

      g_rec_mutex_lock(thread_mutex);

      thread_pool = returnable_thread->thread_pool;
      
      g_rec_mutex_unlock(thread_mutex);

      /* give returnable thread back to thread pool */
      g_atomic_pointer_set(&(returnable_thread->safe_data),
			   NULL);
      ags_returnable_thread_disconnect_safe_run(returnable_thread);

      g_atomic_pointer_set(&(thread_pool->returnable_thread),
			   g_list_prepend(g_atomic_pointer_get(&(thread_pool->returnable_thread)),
					  returnable_thread));      
    }
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
 * ags_returnable_thread_test_flags:
 * @returnable_thread: the #AgsReturnableThread
 * @flags: the flags
 *
 * Test @flags to be set on @returnable_thread.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_returnable_thread_test_flags(AgsReturnableThread *returnable_thread, guint flags)
{
  gboolean retval;  

  if(!AGS_IS_RETURNABLE_THREAD(returnable_thread)){
    return(FALSE);
  }
  
  retval = ((flags & (g_atomic_int_get(&(returnable_thread->flags)))) != 0) ? TRUE: FALSE;
    
  return(retval);
}

/**
 * ags_returnable_thread_set_flags:
 * @returnable_thread: the #AgsReturnableThread
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 3.0.0
 */
void
ags_returnable_thread_set_flags(AgsReturnableThread *returnable_thread, guint flags)
{
  if(!AGS_IS_RETURNABLE_THREAD(returnable_thread)){
    return;
  }

  g_atomic_int_or(&(returnable_thread->flags), flags);
}

/**
 * ags_returnable_thread_unset_flags:
 * @returnable_thread: the #AgsReturnableThread
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 3.0.0
 */
void
ags_returnable_thread_unset_flags(AgsReturnableThread *returnable_thread, guint flags)
{
  if(!AGS_IS_RETURNABLE_THREAD(returnable_thread)){
    return;
  }

  g_atomic_int_and(&(returnable_thread->flags), (~flags));
}

/**
 * ags_returnable_thread_connect_safe_run:
 * @returnable_thread: the thread to connect
 * @callback: the callback
 *
 * Connects @callback to @thread.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Create a new instance of #AgsReturnableThread.
 *
 * Returns: the new #AgsReturnableThread
 *
 * Since: 3.0.0
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
