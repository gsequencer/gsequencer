/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_EXPORT_THREAD_H__
#define __AGS_EXPORT_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/audio/file/ags_audio_file.h>

#define AGS_TYPE_EXPORT_THREAD                (ags_export_thread_get_type())
#define AGS_EXPORT_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_THREAD, AgsExportThread))
#define AGS_EXPORT_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_EXPORT_THREAD, AgsExportThreadClass))
#define AGS_IS_EXPORT_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EXPORT_THREAD))
#define AGS_IS_EXPORT_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EXPORT_THREAD))
#define AGS_EXPORT_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_EXPORT_THREAD, AgsExportThreadClass))

#define AGS_EXPORT_THREAD_DEFAULT_JIFFIE (48.0) // same as devout thread

typedef struct _AgsExportThread AgsExportThread;
typedef struct _AgsExportThreadClass AgsExportThreadClass;

typedef enum{
  AGS_EXPORT_THREAD_LIVE_PERFORMANCE       = 1,
};

struct _AgsExportThread
{
  AgsThread thread;

  guint flags;

  guint tic;
  guint counter;

  GObject *soundcard;
  AgsAudioFile *audio_file;
};

struct _AgsExportThreadClass
{
  AgsThreadClass thread;
};

GType ags_export_thread_get_type();

AgsExportThread* ags_export_thread_new(GObject *soundcard, AgsAudioFile *audio_file);

#endif /*__AGS_EXPORT_THREAD_H__*/
