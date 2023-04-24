/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_EXPORT_THREAD_H__
#define __AGS_EXPORT_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_audio_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_EXPORT_THREAD                (ags_export_thread_get_type())
#define AGS_EXPORT_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EXPORT_THREAD, AgsExportThread))
#define AGS_EXPORT_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_EXPORT_THREAD, AgsExportThreadClass))
#define AGS_IS_EXPORT_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EXPORT_THREAD))
#define AGS_IS_EXPORT_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EXPORT_THREAD))
#define AGS_EXPORT_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_EXPORT_THREAD, AgsExportThreadClass))

#define AGS_EXPORT_THREAD_DEFAULT_JIFFIE (ceil(AGS_SOUNDCARD_DEFAULT_SAMPLERATE / AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE) + AGS_SOUNDCARD_DEFAULT_OVERCLOCK) // same as soundcard thread

typedef struct _AgsExportThread AgsExportThread;
typedef struct _AgsExportThreadClass AgsExportThreadClass;

/**
 * AgsExportThreadFlags:
 * @AGS_EXPORT_THREAD_LIVE_PERFORMANCE: do live export
 * 
 * Enum values to control the behavior or indicate internal state of #AgsExportThread by
 * enable/disable as flags.
 */
typedef enum{
  AGS_EXPORT_THREAD_LIVE_PERFORMANCE       = 1,
  AGS_EXPORT_THREAD_IS_EXPORTING           = 1 <<  1,
}AgsExportThreadFlags;

struct _AgsExportThread
{
  AgsThread thread;

  AgsExportThreadFlags flags;

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

/* flags */
gboolean ags_export_thread_test_flags(AgsExportThread *export_thread, AgsExportThreadFlags flags);
void ags_export_thread_set_flags(AgsExportThread *export_thread, AgsExportThreadFlags flags);
void ags_export_thread_unset_flags(AgsExportThread *export_thread, AgsExportThreadFlags flags);

AgsExportThread* ags_export_thread_find_soundcard(AgsExportThread *export_thread,
						  GObject *soundcard);

AgsExportThread* ags_export_thread_new(GObject *soundcard, AgsAudioFile *audio_file);

G_END_DECLS

#endif /*__AGS_EXPORT_THREAD_H__*/
