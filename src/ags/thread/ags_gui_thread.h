/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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

typedef enum{
  AGS_GUI_THREAD_START  = 1,
  AGS_GUI_THREAD_STOP   = 1 << 1,
};

struct _AgsGuiThread
{
  AgsThread thread;

  volatile guint flags;

  pthread_mutex_t sync_mutex;
  pthread_cond_t start;
  pthread_cond_t stop;

  GMutex mutex;
  GCond cond;
  GThread *gui_thread;
};

struct _AgsGuiThreadClass
{
  AgsThreadClass thread;
};

GType ags_gui_thread_get_type();

AgsGuiThread* ags_gui_thread_new();

#endif /*__AGS_GUI_THREAD_H__*/
