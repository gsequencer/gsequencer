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

#ifndef __AGS_AUDIO_THREAD_H__
#define __AGS_AUDIO_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_AUDIO_THREAD                (ags_audio_thread_get_type())
#define AGS_AUDIO_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_THREAD, AgsAudioThread))
#define AGS_AUDIO_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_THREAD, AgsAudioThreadClass))
#define AGS_IS_AUDIO_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_THREAD))
#define AGS_IS_AUDIO_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_THREAD))
#define AGS_AUDIO_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_THREAD, AgsAudioThreadClass))

#define AGS_AUDIO_THREAD_DEFAULT_JIFFIE (48.0)

typedef struct _AgsAudioThread AgsAudioThread;
typedef struct _AgsAudioThreadClass AgsAudioThreadClass;

typedef enum{
  AGS_AUDIO_THREAD_WAITING    = 1,
  AGS_AUDIO_THREAD_WAKEUP     = 1 <<  1,
  AGS_AUDIO_THREAD_DONE       = 1 <<  2,
  AGS_AUDIO_THREAD_NOTIFY     = 1 <<  3,
}AgsAudioThreadFlags;

struct _AgsAudioThread
{
  AgsThread thread;

  volatile guint flags;

  pthread_mutexattr_t wakeup_attr;
  pthread_mutex_t *wakeup_mutex;
  pthread_cond_t *wakeup_cond;

  pthread_mutexattr_t done_attr;
  pthread_mutex_t *done_mutex;
  pthread_cond_t *done_cond;

  GObject *audio;
};

struct _AgsAudioThreadClass
{
  AgsThreadClass thread;
};

GType ags_audio_thread_get_type();

AgsAudioThread* ags_audio_thread_new(GObject *devout,
				     GObject *audio);

#endif /*__AGS_AUDIO_THREAD_H__*/
