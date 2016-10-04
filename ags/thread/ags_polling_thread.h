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

#ifndef __AGS_POLLING_THREAD_H__
#define __AGS_POLLING_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <poll.h>

#define AGS_TYPE_POLLING_THREAD                (ags_polling_thread_get_type())
#define AGS_POLLING_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_POLLING_THREAD, AgsPollingThread))
#define AGS_POLLING_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_POLLING_THREAD, AgsPollingThreadClass))
#define AGS_IS_POLLING_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_POLLING_THREAD))
#define AGS_IS_POLLING_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_POLLING_THREAD))
#define AGS_POLLING_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_POLLING_THREAD, AgsPollingThreadClass))

#define AGS_POLLING_THREAD_RT_PRIORITY (95)

#define AGS_POLLING_THREAD_DEFAULT_JIFFIE (100.0) //NOTE:JK: take care of AGS_POLLING_THREAD_UNDERLOAD as exceeding AGS_THREAD_HERTZ_JIFFIE
#define AGS_POLLING_THREAD_UNDERLOAD (4500)

typedef struct _AgsPollingThread AgsPollingThread;
typedef struct _AgsPollingThreadClass AgsPollingThreadClass;

typedef enum{
  AGS_POLLING_THREAD_PERFORMANCE          = 1,
  AGS_POLLING_THREAD_OMIT                 = 1 <<  1,
}AgsPollingThreadFlags;

struct _AgsPollingThread
{
  AgsThread thread;

  guint flags;

  pthread_mutexattr_t *fd_mutexattr;
  pthread_mutex_t *fd_mutex;
  
  struct pollfd *fds;
  
  GList *poll_fd;
};

struct _AgsPollingThreadClass
{
  AgsThreadClass thread;
};

GType ags_polling_thread_get_type();

gint ags_polling_thread_fd_position(AgsPollingThread *polling_thread,
				    int fd);

void ags_polling_thread_add_poll_fd(AgsPollingThread *polling_thread,
				    GObject *gobject);
void ags_polling_thread_remove_poll_fd(AgsPollingThread *polling_thread,
				       GObject *gobject);

AgsPollingThread* ags_polling_thread_new();

#endif /*__AGS_POLLING_THREAD_H__*/
