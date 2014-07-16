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

void ags_async_queue_class_init(AgsAsyncQueueClass *async_queue);
void ags_async_queue_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_async_queue_init(AgsAsyncQueue *async_queue);
void ags_async_queue_connect(AgsConnectable *connectable);
void ags_async_queue_disconnect(AgsConnectable *connectable);
void ags_async_queue_finalize(GObject *gobject);

void ags_async_queue_run_callback(AgsThread *thread,
				  AgsAsyncQueue *async_queue);

static gpointer ags_async_queue_parent_class = NULL;

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

  ags_async_queue_parent_class = g_type_class_peek_parent(async_queue);
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
  async_queue->stack = g_queue_new();
  async_queue->timer = g_hash_table_new(g_str_hash, g_str_equal);
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

void
ags_asnyc_queue_add(AgsAsyncQueue *async_queue, AgsStackable *stackable)
{
  AgsTimer *timer;
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static time_t tv_sec = 0;
  static long tv_nsec = 0;
  static long delay = 4000;
  static const long max_delay = 800000;
  static gboolean odd = FALSE;

  pthread_mutex_lock(&mutex);
  
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

  g_queue_push(async_queue->stack,
	       stackable);
  g_hash_table_insert(async_queue->timer, stackable, timer);

  pthread_mutex_unlock(&mutex);
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
ags_async_queue_initerrupt(AgsAsyncQueue *async_queue)
{
  //TODO:JK: implement me
}

void
ags_async_queue_run_callback(AgsThread *thread,
			     AgsAsyncQueue *async_queue)
{
  AgsTimer *timer;

  timer = g_hash_table_lookup(async_queue->timer,
			      AGS_STACKABLE(thread));

  nanosleep(&(timer->run_delay), NULL);
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
