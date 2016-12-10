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

#ifndef __AGS_SOUNDCARD_THREAD_H__
#define __AGS_SOUNDCARD_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_soundcard.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_SOUNDCARD_THREAD                (ags_soundcard_thread_get_type())
#define AGS_SOUNDCARD_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUNDCARD_THREAD, AgsSoundcardThread))
#define AGS_SOUNDCARD_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SOUNDCARD_THREAD, AgsSoundcardThreadClass))
#define AGS_IS_SOUNDCARD_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOUNDCARD_THREAD))
#define AGS_IS_SOUNDCARD_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOUNDCARD_THREAD))
#define AGS_SOUNDCARD_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SOUNDCARD_THREAD, AgsSoundcardThreadClass))

#define AGS_SOUNDCARD_THREAD_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK)

typedef struct _AgsSoundcardThread AgsSoundcardThread;
typedef struct _AgsSoundcardThreadClass AgsSoundcardThreadClass;

struct _AgsSoundcardThread
{
  AgsThread thread;

  time_t time_val;

  GObject *soundcard;
  
  AgsThread *timestamp_thread;

  GError *error;
};

struct _AgsSoundcardThreadClass
{
  AgsThreadClass thread;
};

GType ags_soundcard_thread_get_type();

AgsSoundcardThread* ags_soundcard_thread_find_soundcard(AgsSoundcardThread *soundcard_thread,
							GObject *soundcard);

AgsSoundcardThread* ags_soundcard_thread_new(GObject *soundcard);

#endif /*__AGS_SOUNDCARD_THREAD_H__*/
