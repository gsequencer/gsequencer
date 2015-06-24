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

#include <ags/audio/ags_task.h>

#define AGS_TYPE_TASK_THREAD                (ags_task_thread_get_type())
#define AGS_TASK_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TASK_THREAD, AgsTaskThread))
#define AGS_TASK_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TASK_THREAD, AgsTaskThread))
#define AGS_IS_TASK_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TASK_THREAD))
#define AGS_IS_TASK_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TASK_THREAD))
#define AGS_TASK_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TASK_THREAD, AgsTaskThreadClass))

#define AGS_TASK_THREAD_DEFAULT_JIFFIE (1000)

typedef struct _AgsTaskThread AgsTaskThread;
typedef struct _AgsTaskThreadClass AgsTaskThreadClass;
typedef struct _AgsTaskThreadAppend AgsTaskThreadAppend;

struct _AgsTaskThread
{
  AgsThread thread;

  guint flags;

  GMutex mutex;
  GCond cond;

  pthread_mutex_t *read_mutex;
  pthread_mutex_t *launch_mutex;

  volatile guint queued;
  volatile guint pending;

  volatile GList *exec;
  volatile GList *queue;

  AgsThreadPool *thread_pool;
};

struct _AgsTaskThreadClass
{
  AgsThreadClass thread;
};

struct _AgsTaskThreadAppend
{
  volatile AgsTaskThread *task_thread;
  volatile gpointer data;
};

GType ags_task_thread_get_type();

void ags_task_thread_append_task(AgsTaskThread *task_thread, AgsTask *task);
void ags_task_thread_append_tasks(AgsTaskThread *task_thread, GList *list);

AgsTaskThread* ags_task_thread_new(GObject *devout);

#endif /*__AGS_TASK_THREAD_H__*/
