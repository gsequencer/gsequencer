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

#ifndef __AGS_WORKER_THREAD_H__
#define __AGS_WORKER_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_WORKER_THREAD                (ags_worker_thread_get_type())
#define AGS_WORKER_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WORKER_THREAD, AgsWorkerThread))
#define AGS_WORKER_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_WORKER_THREAD, AgsWorkerThreadClass))
#define AGS_IS_WORKER_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WORKER_THREAD))
#define AGS_IS_WORKER_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WORKER_THREAD))
#define AGS_WORKER_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_WORKER_THREAD, AgsWorkerThreadClass))

#define AGS_WORKER_THREAD_DEFAULT_JIFFIE (AGS_THREAD_MAX_PRECISION)

typedef struct _AgsWorkerThread AgsWorkerThread;
typedef struct _AgsWorkerThreadClass AgsWorkerThreadClass;

typedef enum{
  AGS_WORKER_THREAD_RUNNING    = 1,
  AGS_WORKER_THREAD_RUN_WAIT   = 1 <<  1,
  AGS_WORKER_THREAD_RUN_DONE   = 1 <<  2,
  AGS_WORKER_THREAD_RUN_SYNC   = 1 <<  3,
}AgsWorkerThreadFlags;

struct _AgsWorkerThread
{
  AgsThread thread;

  volatile guint flags;

  pthread_mutex_t *run_mutex;
  pthread_mutexattr_t *run_mutexattr;
  
  pthread_cond_t *run_cond;

  pthread_t *worker_thread;
  pthread_attr_t *worker_thread_attr;
};

struct _AgsWorkerThreadClass
{
  AgsThreadClass thread;

  void (*do_poll)(AgsWorkerThread *worker_thread);
};

GType ags_worker_thread_get_type();

void* ags_woker_thread_do_poll_loop(void *ptr);

void ags_worker_thread_do_poll(AgsWorkerThread *worker_thread);

AgsWorkerThread* ags_worker_thread_new();

#endif /*__AGS_WORKER_THREAD_H__*/
