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

#ifndef __AGS_TASK_THREAD_H__
#define __AGS_TASK_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_task.h>

#define AGS_TYPE_TASK_THREAD                (ags_task_thread_get_type())
#define AGS_TASK_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TASK_THREAD, AgsTaskThread))
#define AGS_TASK_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TASK_THREAD, AgsTaskThread))
#define AGS_IS_TASK_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TASK_THREAD))
#define AGS_IS_TASK_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TASK_THREAD))
#define AGS_TASK_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TASK_THREAD, AgsTaskThreadClass))

#ifndef AGS_TASK_THREAD_RT_PRIORITY
#define AGS_TASK_THREAD_RT_PRIORITY (95)
#endif

#define AGS_TASK_THREAD_DEFAULT_JIFFIE (AGS_THREAD_MAX_PRECISION)

typedef struct _AgsTaskThread AgsTaskThread;
typedef struct _AgsTaskThreadClass AgsTaskThreadClass;
typedef struct _AgsTaskThreadAppend AgsTaskThreadAppend;

typedef enum{
  AGS_TASK_THREAD_EXTERN_SYNC                = 1,
  AGS_TASK_THREAD_EXTERN_READY               = 1 <<  1,
  AGS_TASK_THREAD_EXTERN_AVAILABLE           = 1 <<  2,
  AGS_TASK_THREAD_SYNC_WAIT                  = 1 <<  3,
  AGS_TASK_THREAD_SYNC_DONE                  = 1 <<  4,
  AGS_TASK_THREAD_EXTERN_SYNC_WAIT           = 1 <<  5,
  AGS_TASK_THREAD_EXTERN_SYNC_DONE           = 1 <<  6,
}AgsTaskThreadFlags;

struct _AgsTaskThread
{
  AgsThread thread;

  volatile guint flags;

  volatile gboolean is_run;
  volatile guint wait_ref;  

  pthread_mutexattr_t *sync_mutexattr;
  pthread_mutex_t *sync_mutex;
  pthread_cond_t *sync_cond;

  pthread_mutexattr_t *extern_sync_mutexattr;
  pthread_mutex_t *extern_sync_mutex;
  pthread_cond_t *extern_sync_cond;
  
  pthread_mutexattr_t *run_mutexattr;
  pthread_mutex_t *run_mutex;
  pthread_cond_t *run_cond;
  
  pthread_mutexattr_t *read_mutexattr;
  pthread_mutex_t *read_mutex;

  pthread_mutexattr_t *launch_mutexattr;
  pthread_mutex_t *launch_mutex;

  volatile guint queued;
  volatile guint pending;

  volatile GList *exec;
  volatile GList *queue;

  pthread_mutexattr_t *cyclic_task_mutexattr;
  pthread_mutex_t *cyclic_task_mutex;

  volatile GList *cyclic_task;
  
  AgsThreadPool *thread_pool;
};

struct _AgsTaskThreadClass
{
  AgsThreadClass thread;

  void (*clear_cache)(AgsTaskThread *task_thread);
};

struct _AgsTaskThreadAppend
{
  volatile AgsTaskThread *task_thread;
  volatile gpointer data;
};

GType ags_task_thread_get_type();

void ags_task_thread_append_task(AgsTaskThread *task_thread,
				 AgsTask *task);
void ags_task_thread_append_tasks(AgsTaskThread *task_thread,
				  GList *list);

void ags_task_thread_append_cyclic_task(AgsTaskThread *task_thread,
					AgsTask *task);
void ags_task_thread_remove_cyclic_task(AgsTaskThread *task_thread,
					AgsTask *task);

void ags_task_thread_clear_cache(AgsTaskThread *task_thread);

AgsTaskThread* ags_task_thread_new();

#endif /*__AGS_TASK_THREAD_H__*/
