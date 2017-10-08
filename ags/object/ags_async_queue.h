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

#ifndef __AGS_ASYNC_QUEUE_H__
#define __AGS_ASYNC_QUEUE_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#define AGS_TYPE_ASYNC_QUEUE                    (ags_async_queue_get_type())
#define AGS_ASYNC_QUEUE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueue))
#define AGS_ASYNC_QUEUE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueueInterface))
#define AGS_IS_ASYNC_QUEUE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ASYNC_QUEUE))
#define AGS_IS_ASYNC_QUEUE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_ASYNC_QUEUE))
#define AGS_ASYNC_QUEUE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueueInterface))

typedef struct _AgsAsyncQueue AgsAsyncQueue;
typedef struct _AgsAsyncQueueInterface AgsAsyncQueueInterface;

struct _AgsAsyncQueueInterface
{
  GTypeInterface ginterface;
  
  void (*increment_wait_ref)(AgsAsyncQueue *async_queue);
  guint (*get_wait_ref)(AgsAsyncQueue *async_queue);

  void (*set_run_mutex)(AgsAsyncQueue *async_queue, pthread_mutex_t *run_mutex);
  pthread_mutex_t* (*get_run_mutex)(AgsAsyncQueue *async_queue);

  void (*set_run_cond)(AgsAsyncQueue *async_queue, pthread_cond_t *run_cond);
  pthread_cond_t* (*get_run_cond)(AgsAsyncQueue *async_queue);

  void (*set_run)(AgsAsyncQueue *async_queue, gboolean is_run);
  gboolean (*is_run)(AgsAsyncQueue *async_queue);
};

GType ags_async_queue_get_type();

void ags_async_queue_increment_wait_ref(AgsAsyncQueue *async_queue);
guint ags_async_queue_get_wait_ref(AgsAsyncQueue *async_queue);

void ags_async_queue_set_run_mutex(AgsAsyncQueue *async_queue, pthread_mutex_t *run_mutex);
pthread_mutex_t* ags_async_queue_get_run_mutex(AgsAsyncQueue *async_queue);

void ags_async_queue_set_run_cond(AgsAsyncQueue *async_queue, pthread_cond_t *run_cond);
pthread_cond_t* ags_async_queue_get_run_cond(AgsAsyncQueue *async_queue);

void ags_async_queue_set_run(AgsAsyncQueue *async_queue, gboolean is_run);
gboolean ags_async_queue_is_run(AgsAsyncQueue *async_queue);

#endif /*__AGS_ASYNC_QUEUE_H__*/
