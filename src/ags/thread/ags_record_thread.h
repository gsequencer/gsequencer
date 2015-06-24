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

#ifndef __AGS_RECORD_THREAD_H__
#define __AGS_RECORD_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/server/ags_registry.h>

#define AGS_TYPE_RECORD_THREAD                (ags_record_thread_get_type())
#define AGS_RECORD_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECORD_THREAD, AgsRecordThread))
#define AGS_RECORD_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RECORD_THREAD, AgsRecordThreadClass))
#define AGS_IS_RECORD_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECORD_THREAD))
#define AGS_IS_RECORD_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECORD_THREAD))
#define AGS_RECORD_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RECORD_THREAD, AgsRecordThreadClass))

#define AGS_RECORD_THREAD_DEFAULT_DELAY (128)

typedef struct _AgsRecordThread AgsRecordThread;
typedef struct _AgsRecordThreadClass AgsRecordThreadClass;

struct _AgsRecordThread
{
  AgsThread thread;

  AgsRegistry *registry;

  guint delay;
  guint counter;
};

struct _AgsRecordThreadClass
{
  AgsThreadClass thread;
};

GType ags_record_thread_get_type();

AgsRecordThread* ags_record_thread_new(GObject *devout, AgsRegistry *registry);

#endif /*__AGS_RECORD_THREAD_H__*/
