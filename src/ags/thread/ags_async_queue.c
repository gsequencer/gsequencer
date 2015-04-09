/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/thread/ags_async_queue.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

void ags_async_queue_class_init(AgsAsyncQueueClass *async_queue);
void ags_async_queue_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_async_queue_init(AgsAsyncQueue *async_queue);
void ags_async_queue_connect(AgsConnectable *connectable);
void ags_async_queue_disconnect(AgsConnectable *connectable);
void ags_async_queue_finalize(GObject *gobject);

void ags_async_queue_real_interrupt(AgsAsyncQueue *async_queue);
void ags_async_queue_real_push_context(AgsAsyncQueue *async_queue,
				       AgsContext *context);
void ags_async_queue_real_pop_context(AgsAsyncQueue *async_queue,
				      AgsContext *context);

void ags_async_queue_run_callback(AgsThread *thread,
				  AgsAsyncQueue *async_queue);

enum{
  INTERRUPT,
  PUSH_CONTEXT,
  POP_CONTEXT,
  LAST_SIGNAL,
};

static gpointer ags_async_queue_parent_class = NULL;
static guint async_queue_signals[LAST_SIGNAL];

GType
ags_async_queue_get_type()
{
  static GType ags_type_async_queue = 0;

  if(!ags_type_async_queue){
    const GTypeInfo ags_async_queue_info = {
      sizeof (AgsAsyncQueueClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_async_queue_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAsyncQueue),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_async_queue_init,
    };

    ags_type_async_queue = g_type_register_static(G_TYPE_OBJECT,
						  "AgsAsyncQueue\0",
						  &ags_async_queue_info,
						  0);
  }
  
  return(ags_type_async_queue);
}

void
ags_async_queue_class_init(AgsAsyncQueueClass *async_queue)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_async_queue_parent_class = g_type_class_peek_parent(async_queue);

  /* AgsAsyncQueue */
  async_queue->interrupt = ags_async_queue_real_interrupt;
  async_queue->push_context = ags_async_queue_real_push_context;
  async_queue->pop_context = ags_async_queue_real_pop_context;

  /* signals */
  async_queue_signals[INTERRUPT] =
    g_signal_new("interrupt\0",
		 G_TYPE_FROM_CLASS(async_queue),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAsyncQueueClass, interrupt),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  async_queue_signals[PUSH_CONTEXT] =
    g_signal_new("push_context\0",
		 G_TYPE_FROM_CLASS(async_queue),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAsyncQueueClass, push_context),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  async_queue_signals[POP_CONTEXT] =
    g_signal_new("pop_context\0",
		 G_TYPE_FROM_CLASS(async_queue),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsAsyncQueueClass, pop_context),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);
}

void
ags_async_queue_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_async_queue_connect;
  connectable->disconnect = ags_async_queue_disconnect;
}

void
ags_async_queue_init(AgsAsyncQueue *async_queue)
{
  struct sigevent sev;

  /* deprecated
  async_queue->flags = AGS_ASYNC_QUEUE_POSIX_THREADS;

  async_queue->output_sum = AGS_DEVOUT_DEFAULT_BUFFER_SIZE;
  async_queue->systemrate = NSEC_PER_SEC / AGS_ASYNC_QUEUE_DEFAULT_SYSTEM_JIFFIE;
  //TODO:JK: fix aproximation
  async_queue->interval = async_queue->output_sum * NSEC_PER_SEC / async_queue->systemrate / 2;

  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = AGS_ASYNC_QUEUE_SIGNAL_HIGH;
  sev.sigev_value.sival_ptr = &(async_queue->timerid);

  //  timer_create(AGS_ASYNC_QUEUE_CLOCK_ID, &sev, &(async_queue->timerid));

  async_queue->stack = g_queue_new();
  async_queue->timer = g_hash_table_new(g_str_hash, g_str_equal);

  pthread_mutex_init(&(async_queue->lock.mutex), NULL);

  async_queue->context = NULL;
  async_queue->unref_context = NULL;
  */
}

void
ags_async_queue_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_async_queue_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_async_queue_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

AgsTimer*
ags_timer_alloc(time_t tv_sec, long tv_nsec)
{
  AgsTimer *timer;

  timer = (AgsTimer *) malloc(sizeof(AgsTimer));
  
  timer->run_delay.tv_sec = tv_sec;
  timer->run_delay.tv_nsec = tv_nsec;
  
  timer->record_history = FALSE;
  timer->history = NULL;

  return(timer);
}

AgsContext*
ags_context_alloc(GQueue *stack, GHashTable *timer)
{
  AgsContext *context;

  context = (AgsContext *) malloc(sizeof(AgsContext));

  context->stack = stack;
  context->timer = timer;

  return(context);
}

guint
ags_async_queue_next_interval(AgsAsyncQueue *async_queue)
{
  return(async_queue->output_sum * async_queue->systemrate / NSEC_PER_SEC);
}

AgsContext*
ags_async_queue_find_context(AgsAsyncQueue *async_queue,
			     AgsStackable *stackable)
{
  //TODO:JK: implement me
}

void
ags_async_queue_add(AgsAsyncQueue *async_queue, AgsStackable *stackable)
{
  AgsTimer *timer;
  static time_t tv_sec = 0;
  static long tv_nsec = 0;
  static long delay = 4000;
  static const long max_delay = 800000;
  static gboolean odd = FALSE;

  if((AGS_ASYNC_QUEUE_LINUX_THREADS & (async_queue->flags)) != 0){
    //TODO:JK: uncomment me
    //    while(atomic_read(&(async_queue->lock.monitor)) == 1){
    //      ags_async_queue_idle(async_queue);
    //    }

    //    atomic_set(&(async_queue->lock.monitor),
    //	       1);
  }else if((AGS_ASYNC_QUEUE_POSIX_THREADS & (async_queue->flags)) != 0){
    pthread_mutex_lock(&(async_queue->lock.mutex));
  }

  timer = ags_timer_alloc(tv_sec,
			  tv_nsec);

  if(!odd){
    tv_nsec += delay;

    if(tv_nsec >= max_delay){
      tv_nsec = max_delay / 2;
      odd = TRUE;
    }
  }else{
    tv_nsec -= delay;

    if(2 * delay > tv_nsec - delay){
      odd = FALSE;
    }
  }

  g_queue_push_tail(async_queue->stack,
		    stackable);
  g_hash_table_insert(async_queue->timer, stackable, timer);

  //  g_signal_connect(G_OBJECT(stackable), "run\0",
  //		   G_CALLBACK(ags_async_queue_run_callback), async_queue);

  if((AGS_ASYNC_QUEUE_LINUX_THREADS & (async_queue->flags)) != 0){
    //TODO:JK: uncomment me
    //    atomic_set(&(async_queue->lock.monitor),
    //	       0);
  }else if((AGS_ASYNC_QUEUE_POSIX_THREADS & (async_queue->flags)) != 0){
    pthread_mutex_unlock(&(async_queue->lock.mutex));
  }
}

gboolean
ags_async_queue_remove(AgsAsyncQueue *async_queue, AgsStackable *stackable)
{
  AgsTimer *timer;

  timer = g_hash_table_lookup(async_queue->timer,
			      stackable);
  g_hash_table_remove(async_queue->timer,
		      stackable);
  free(timer);
}

void
ags_async_queue_idle(AgsAsyncQueue *async_queue)
{
  /* software interrupt - caused by exceeding multiplexing by time */
  ags_async_queue_interrupt(async_queue);
}

void
ags_async_queue_run_callback(AgsThread *thread,
			     AgsAsyncQueue *async_queue)
{
  AgsTimer *timer;
  gboolean interrupt_first;
  struct timespec delay = {
    0,
    NSEC_PER_SEC / thread->freq / 2,
  };

  interrupt_first = ((AGS_ASYNC_QUEUE_STOP_BIT_0 & (async_queue->flags)) != 0) ? TRUE: FALSE;

  timer = g_hash_table_lookup(async_queue->timer,
			      AGS_STACKABLE(thread));

  //  nanosleep(&(timer->run_delay), NULL);
  //  ags_async_queue_interrupt(async_queue);

  //  nanosleep(&(delay), NULL);

  while((interrupt_first &&
	 (AGS_ASYNC_QUEUE_STOP_BIT_0 & (async_queue->flags)) != 0) ||
	(!interrupt_first &&
	 AGS_ASYNC_QUEUE_STOP_BIT_1 & (async_queue->flags)) != 0){
  }
}

void
ags_async_queue_real_interrupt(AgsAsyncQueue *async_queue)
{
  //  pthread_yield();
}

void
ags_async_queue_interrupt(AgsAsyncQueue *async_queue)
{
  g_return_if_fail(AGS_IS_ASYNC_QUEUE(async_queue));

  g_object_ref(G_OBJECT(async_queue));
  g_signal_emit(G_OBJECT(async_queue),
		async_queue_signals[INTERRUPT], 0);
  g_object_unref(G_OBJECT(async_queue));
}

void
ags_async_queue_real_push_context(AgsAsyncQueue *async_queue,
				  AgsContext *context)
{
  async_queue->stack = context->stack;
  async_queue->timer = context->timer;
}

void
ags_async_queue_push_context(AgsAsyncQueue *async_queue,
			     AgsContext *context)
{
  g_return_if_fail(AGS_IS_ASYNC_QUEUE(async_queue));

  g_object_ref(G_OBJECT(async_queue));
  g_signal_emit(G_OBJECT(async_queue),
		async_queue_signals[PUSH_CONTEXT], 0,
		context);
  g_object_unref(G_OBJECT(async_queue));
}

void
ags_async_queue_real_pop_context(AgsAsyncQueue *async_queue,
				 AgsContext *context)
{
  async_queue->stack = NULL;
  async_queue->timer = NULL;
}

void
ags_async_queue_pop_context(AgsAsyncQueue *async_queue,
			    AgsContext *context)
{
  g_return_if_fail(AGS_IS_ASYNC_QUEUE(async_queue));

  g_object_ref(G_OBJECT(async_queue));
  g_signal_emit(G_OBJECT(async_queue),
		async_queue_signals[POP_CONTEXT], 0,
		context);
  g_object_unref(G_OBJECT(async_queue));
}

void*
ags_async_queue_worker(void *ptr)
{
  AgsAsyncQueue *async_queue;
  struct timespec delay = {
    0,
    async_queue->output_sum * NSEC_PER_SEC / async_queue->systemrate / 2,
  };


  while((AGS_ASYNC_QUEUE_WORKER_RUNNING & (async_queue->flags)) != 0){
    if((AGS_ASYNC_QUEUE_STOP_BIT_0 & (async_queue->flags)) != 0){
      async_queue->flags &= (~AGS_ASYNC_QUEUE_STOP_BIT_0);
      async_queue->flags |= (AGS_ASYNC_QUEUE_STOP_BIT_1);
    }else{
      async_queue->flags &= (~AGS_ASYNC_QUEUE_STOP_BIT_1);
      async_queue->flags |= (AGS_ASYNC_QUEUE_STOP_BIT_0);
    }

    nanosleep(&delay, NULL);
  }

  return(NULL);
}

/**
 * ags_async_queue_clean:
 */
void
ags_async_queue_clean(AgsAsyncQueue *async_queue)
{
  GList *list;

  list = async_queue->unref_context;

  while(list != NULL){
    g_object_unref(list->data);

    list = list->next;
  }

  g_list_free(async_queue->unref_context);
  async_queue->unref_context = NULL;
}

/**
 * ags_async_queue_new:
 */
AgsAsyncQueue*
ags_async_queue_new()
{
  AgsAsyncQueue *async_queue;

  async_queue = (AgsAsyncQueue *) g_object_new(AGS_TYPE_ASYNC_QUEUE,
					       NULL);

  return(async_queue);
}
