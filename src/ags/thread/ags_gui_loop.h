/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2016 Joël Krähemann
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

#ifndef __AGS_GUI_LOOP_H__
#define __AGS_GUI_LOOP_H__

#include <math.h>

#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_GUI_LOOP                (ags_gui_loop_get_type())
#define AGS_GUI_LOOP(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GUI_LOOP, AgsGuiLoop))
#define AGS_GUI_LOOP_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GUI_LOOP, AgsGuiLoopClass))
#define AGS_IS_GUI_LOOP(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GUI_LOOP))
#define AGS_IS_GUI_LOOP_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GUI_LOOP))
#define AGS_GUI_LOOP_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GUI_LOOP, AgsGuiLoopClass))

#define AGS_GUI_LOOP_DEFAULT_JIFFIE (60.0)

typedef struct _AgsGuiLoop AgsGuiLoop;
typedef struct _AgsGuiLoopClass AgsGuiLoopClass;

struct _AgsGuiLoop
{
  AgsThread thread;

  guint flags;

  volatile guint tic;
  volatile guint last_sync;

  GCond cond;
  GMutex mutex;

  GObject *ags_main;

  GObject *async_queue;
};

struct _AgsGuiLoopClass
{
  AgsThreadClass thread;
};

GType ags_gui_loop_get_type();

AgsGuiLoop* ags_gui_loop_new(GObject *devout, GObject *ags_main);

#endif /*__AGS_GUI_LOOP_H__*/
