/* This file is part of GSequencer.
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

#include <signal.h>
#include <time.h>

#include <linux/futex.h>
#include <sys/time.h>

#include <pthread.h>

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_stackable.h>

#define AGS_TYPE_ASYNC_QUEUE                (ags_async_queue_get_type())
#define AGS_ASYNC_QUEUE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueue))
#define AGS_ASYNC_QUEUE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueueClass))
#define AGS_IS_ASYNC_QUEUE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ASYNC_QUEUE))
#define AGS_IS_ASYNC_QUEUE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ASYNC_QUEUE))
#define AGS_ASYNC_QUEUE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueueClass))

#define AGS_ASYNC_QUEUE_DEFAULT_SYSTEM_JIFFIE (250)

#define AGS_ASYNC_QUEUE_CLOCK_ID CLOCK_REALTIME
#define AGS_ASYNC_QUEUE_SIGNAL_HIGH SIGRTMAX
#define AGS_ASYNC_QUEUE_SIGNAL_LOW SIGRTMIN

typedef struct _AgsAsyncQueue AgsAsyncQueue;
typedef struct _AgsAsyncQueueClass AgsAsyncQueueClass;
typedef struct _AgsTimer AgsTimer;
typedef struct _AgsContext AgsContext;

typedef enum{
  AGS_ASYNC_QUEUE_LINUX_THREADS     = 1,
  AGS_ASYNC_QUEUE_POSIX_THREADS     = 1 << 1,
  AGS_ASYNC_QUEUE_WORKER_RUNNING    = 1 << 2,
  AGS_ASYNC_QUEUE_INTERRUPT_OWN     = 1 << 2,
  AGS_ASYNC_QUEUE_INTERRUPT_OTHER   = 1 << 3,
  AGS_ASYNC_QUEUE_STOP_BIT_0        = 1 << 4,
  AGS_ASYNC_QUEUE_STOP_BIT_1        = 1 << 5,
}AgsAsyncQueueFlags;

struct _AgsAsyncQueue
{
  GObject object;

  guint flags;

  guint output_sum;
  guint systemrate;
  guint interval;

  timer_t timerid;

  GQueue *stack;
  GHashTable *timer;

  union{
    //    atomic_t monitor;
    pthread_mutex_t mutex;
  }lock;

  GList *context;
  GList *unref_context;
};

struct _AgsAsyncQueueClass
{
  GObjectClass object;

  void (*interrupt)(AgsAsyncQueue *async_queue);
  
  void (*push_context)(AgsAsyncQueue *async_queue,
		       AgsContext *context);
  void (*pop_context)(AgsAsyncQueue *async_queue,
		      AgsContext *context);
};

struct _AgsTimer
{
  struct timespec run_delay;

  //NOTE: not supported for now
  gboolean record_history;
  gpointer history;
};

struct _AgsContext
{
  GQueue *stack;
  GHashTable *timer;
};

GType ags_async_queue_get_type();

AgsTimer* ags_timer_alloc(time_t tv_sec, long tv_nsec);
AgsContext* ags_context_alloc(GQueue *stack, GHashTable *timer);

guint ags_async_queue_next_interval(AgsAsyncQueue *async_queue);

AgsContext* ags_async_queue_find_context(AgsAsyncQueue *async_queue,
					 AgsStackable *stackable);

void ags_async_queue_add(AgsAsyncQueue *async_queue, AgsStackable *stackable);
gboolean ags_async_queue_remove(AgsAsyncQueue *async_queue, AgsStackable *stackable);

void ags_async_queue_idle(AgsAsyncQueue *async_queue);

void ags_async_queue_interrupt(AgsAsyncQueue *async_queue);

void ags_async_queue_push_context(AgsAsyncQueue *async_queue,
				  AgsContext *context);
void ags_async_queue_pop_context(AgsAsyncQueue *async_queue,
				 AgsContext *context);

void* ags_async_queue_worker(void *ptr);

void ags_async_queue_clean(AgsAsyncQueue *async_queue);

AgsAsyncQueue* ags_async_queue_new();

#endif /*__AGS_ASYNC_QUEUE_H__*/
