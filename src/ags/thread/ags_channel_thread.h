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

#ifndef __AGS_CHANNEL_THREAD_H__
#define __AGS_CHANNEL_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_CHANNEL_THREAD                (ags_channel_thread_get_type())
#define AGS_CHANNEL_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL_THREAD, AgsChannelThread))
#define AGS_CHANNEL_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_CHANNEL_THREAD, AgsChannelThreadClass))
#define AGS_IS_CHANNEL_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CHANNEL_THREAD))
#define AGS_IS_CHANNEL_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CHANNEL_THREAD))
#define AGS_CHANNEL_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_CHANNEL_THREAD, AgsChannelThreadClass))

#define AGS_CHANNEL_THREAD_DEFAULT_JIFFIE (48.0)

typedef struct _AgsChannelThread AgsChannelThread;
typedef struct _AgsChannelThreadClass AgsChannelThreadClass;

typedef enum{
  AGS_CHANNEL_THREAD_WAITING    = 1,
  AGS_CHANNEL_THREAD_WAKEUP     = 1 <<  1,
}AgsChannelThreadFlags;

struct _AgsChannelThread
{
  AgsThread thread;

  volatile guint flags;
  
  pthread_mutexattr_t wakeup_attr;
  pthread_mutex_t *wakeup_mutex;
  pthread_cond_t *wakeup_cond;

  GObject *channel;
};

struct _AgsChannelThreadClass
{
  AgsThreadClass thread;
};

GType ags_channel_thread_get_type();

AgsChannelThread* ags_channel_thread_new(GObject *devout,
					 GObject *channel);

#endif /*__AGS_CHANNEL_THREAD_H__*/
