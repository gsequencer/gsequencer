/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_GSTREAMER_FILE_AUDIO_SINK_H__
#define __AGS_GSTREAMER_FILE_AUDIO_SINK_H__

#include <glib.h>
#include <glib-object.h>

#include <gst/gst.h>
#include <gst/audio/gstaudiosink.h>

#include <ags/libags.h>

#include <ags/audio/file/ags_gstreamer_file.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK                (ags_gstreamer_file_audio_sink_get_type())
#define AGS_GSTREAMER_FILE_AUDIO_SINK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK, AgsGstreamerFileAudioSink))
#define AGS_GSTREAMER_FILE_AUDIO_SINK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK, AgsGstreamerFileAudioSinkClass))
#define AGS_IS_GSTREAMER_FILE_AUDIO_SINK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK))
#define AGS_IS_GSTREAMER_FILE_AUDIO_SINK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK))
#define AGS_GSTREAMER_FILE_AUDIO_SINK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK, AgsGstreamerFileAudioSinkClass))

#define AGS_GSTREAMER_FILE_AUDIO_SINK_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerFileAudioSink *) obj)->obj_mutex))

typedef struct _AgsGstreamerFileAudioSink AgsGstreamerFileAudioSink;
typedef struct _AgsGstreamerFileAudioSinkClass AgsGstreamerFileAudioSinkClass;

/**
 * AgsGstreamerFileAudioSinkStatusFlags:
 * @AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DONE: sync done write data
 * @AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_WAIT: sync wait write data
 * @AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DIRTY: dirty write data
 *
 * Enum values to control the behavior or indicate internal state of #AgsGstreamerFileAudioSink by
 * enable/disable as flags.
 */
typedef enum{
  AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DONE     = 1,
  AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_WAIT     = 1 << 1,
  AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DIRTY    = 1 << 2,
}AgsGstreamerFileAudioSinkStatusFlags;

struct _AgsGstreamerFileAudioSink
{
  GstAudioSink audio_sink;

  guint status_flags;

  GRecMutex obj_mutex;

  AgsGstreamerFile *gstreamer_file;

  GMutex wakeup_mutex;
  GCond wakeup_cond;
  
  guint audio_channels;

  guint buffer_size;
  guint format;
  
  gdouble *buffer;

  gint ring_buffer_samplerate;
  gint ring_buffer_channels;
};

struct _AgsGstreamerFileAudioSinkClass
{
  GstAudioSinkClass audio_sink;
};

GType ags_gstreamer_file_audio_sink_get_type();

gboolean ags_gstreamer_file_audio_sink_test_status_flags(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink, guint status_flags);
void ags_gstreamer_file_audio_sink_set_status_flags(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink, guint status_flags);
void ags_gstreamer_file_audio_sink_unset_status_flags(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink, guint status_flags);

AgsGstreamerFileAudioSink* ags_gstreamer_file_audio_sink_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_FILE_AUDIO_SINK_H__*/
