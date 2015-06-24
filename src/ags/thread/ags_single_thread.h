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

#ifndef __AGS_SINGLE_THREAD_H__
#define __AGS_SINGLE_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_devout_thread.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_gui_thread.h>

#include <unistd.h>

#define AGS_TYPE_SINGLE_THREAD                (ags_single_thread_get_type())
#define AGS_SINGLE_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SINGLE_THREAD, AgsSingleThread))
#define AGS_SINGLE_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SINGLE_THREAD, AgsSingleThreadClass))
#define AGS_IS_SINGLE_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SINGLE_THREAD))
#define AGS_IS_SINGLE_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SINGLE_THREAD))
#define AGS_SINGLE_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SINGLE_THREAD, AgsSingleThreadClass))

#define AGS_SINGLE_THREAD_DEFAULT_GUI_JIFFIE (30)

typedef struct _AgsSingleThread AgsSingleThread;
typedef struct _AgsSingleThreadClass AgsSingleThreadClass;

struct _AgsSingleThread
{
  AgsThread thread;

  AgsAudioLoop *audio_loop;
  AgsDevoutThread *devout_thread;
  AgsTaskThread *task_thread;
  AgsGuiThread *gui_thread;
};

struct _AgsSingleThreadClass
{
  AgsThreadClass thread;
};

GType ags_single_thread_get_type();

AgsSingleThread* ags_single_thread_new();

#endif /*__AGS_SINGLE_THREAD_H__*/

