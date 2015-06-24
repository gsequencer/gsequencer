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

#ifndef __AGS_TIMESTAMP_THREAD_H__
#define __AGS_TIMESTAMP_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_TIMESTAMP_THREAD                (ags_timestamp_thread_get_type())
#define AGS_TIMESTAMP_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TIMESTAMP_THREAD, AgsTimestampThread))
#define AGS_TIMESTAMP_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TIMESTAMP_THREAD, AgsTimestampThread))
#define AGS_IS_TIMESTAMP_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TIMESTAMP_THREAD))
#define AGS_IS_TIMESTAMP_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TIMESTAMP_THREAD))
#define AGS_TIMESTAMP_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TIMESTAMP_THREAD, AgsTimestampThreadClass))

#define AGS_MICROSECONDS_PER_SECOND (1000000.0)
#define AGS_TIMESTAMP_THREAD_DEFAULT_BPM (120.0)
#define AGS_TIMESTAMP_THREAD_DEFAULT_SAMPLERATE (44100.0)
#define AGS_TIMESTAMP_THREAD_DEFAULT_BUFFER_SIZE (256.0)
#define AGS_TIMESTAMP_THREAD_DEFAULT_JIFFIE (AGS_MICROSECONDS_PER_SECOND * \
					     60.0 / AGS_TIMESTAMP_THREAD_DEFAULT_BPM)
#define AGS_TIMESTAMP_THREAD_DEFAULT_TIC (AGS_TIMESTAMP_THREAD_DEFAULT_JIFFIE / \
					  AGS_TIMESTAMP_THREAD_DEFAULT_SAMPLERATE / AGS_TIMESTAMP_THREAD_DEFAULT_BUFFER_SIZE)

typedef struct _AgsTimestampThread AgsTimestampThread;
typedef struct _AgsTimestampThreadClass AgsTimestampThreadClass;

struct _AgsTimestampThread
{
  AgsThread thread;

  GObject *current_timestamp;
  GObject *current_latency;

  GObject *timestamp;
};

struct _AgsTimestampThreadClass
{
  AgsThreadClass thread;
};

GType ags_timestamp_thread_get_type();

AgsTimestampThread* ags_timestamp_thread_new();

#endif /*__AGS_TIMESTAMP_THREAD_H__*/

