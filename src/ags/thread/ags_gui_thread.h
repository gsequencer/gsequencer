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

#ifndef __AGS_GUI_THREAD_H__
#define __AGS_GUI_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <unistd.h>

#define AGS_TYPE_GUI_THREAD                (ags_gui_thread_get_type())
#define AGS_GUI_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GUI_THREAD, AgsGuiThread))
#define AGS_GUI_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GUI_THREAD, AgsGuiThreadClass))
#define AGS_IS_GUI_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GUI_THREAD))
#define AGS_IS_GUI_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GUI_THREAD))
#define AGS_GUI_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GUI_THREAD, AgsGuiThreadClass))

#define AGS_GUI_THREAD_DEFAULT_JIFFIE (250)

typedef struct _AgsGuiThread AgsGuiThread;
typedef struct _AgsGuiThreadClass AgsGuiThreadClass;

struct _AgsGuiThread
{
  AgsThread thread;

  GMutex mutex;
  GCond cond;

  GList *task_completion;
};

struct _AgsGuiThreadClass
{
  AgsThreadClass thread;
};

GType ags_gui_thread_get_type();

AgsGuiThread* ags_gui_thread_new();

#endif /*__AGS_GUI_THREAD_H__*/
