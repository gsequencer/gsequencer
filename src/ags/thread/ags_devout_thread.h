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

#ifndef __AGS_DEVOUT_THREAD_H__
#define __AGS_DEVOUT_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_DEVOUT_THREAD                (ags_devout_thread_get_type())
#define AGS_DEVOUT_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DEVOUT_THREAD, AgsDevoutThread))
#define AGS_DEVOUT_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DEVOUT_THREAD, AgsDevoutThreadClass))
#define AGS_IS_DEVOUT_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DEVOUT_THREAD))
#define AGS_IS_DEVOUT_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DEVOUT_THREAD))
#define AGS_DEVOUT_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DEVOUT_THREAD, AgsDevoutThreadClass))

#define AGS_DEVOUT_THREAD_DEFAULT_JIFFIE (100)

typedef struct _AgsDevoutThread AgsDevoutThread;
typedef struct _AgsDevoutThreadClass AgsDevoutThreadClass;

struct _AgsDevoutThread
{
  AgsThread thread;

  time_t time_val;

  AgsThread *timestamp_thread;

  GError *error;
};

struct _AgsDevoutThreadClass
{
  AgsThreadClass thread;
};

GType ags_devout_thread_get_type();

AgsDevoutThread* ags_devout_thread_new(GObject *devout);

#endif /*__AGS_DEVOUT_THREAD_H__*/
