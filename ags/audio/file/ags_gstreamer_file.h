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

#ifndef __AGS_GSTREAMER_FILE_H__
#define __AGS_GSTREAMER_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <gst/gst.h>

#include <gst/pbutils/pbutils.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_FILE                (ags_gstreamer_file_get_type())
#define AGS_GSTREAMER_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_FILE, AgsGstreamerFile))
#define AGS_GSTREAMER_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_GSTREAMER_FILE, AgsGstreamerFileClass))
#define AGS_IS_GSTREAMER_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_GSTREAMER_FILE))
#define AGS_IS_GSTREAMER_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_GSTREAMER_FILE))
#define AGS_GSTREAMER_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_GSTREAMER_FILE, AgsGstreamerFileClass))

#define AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerFile *) obj)->obj_mutex))

#define AGS_GSTREAMER_FILE_DEFAULT_DISCOVERER_TIMEOUT (3 * GST_SECOND)
  
typedef struct _AgsGstreamerFile AgsGstreamerFile;
typedef struct _AgsGstreamerFileClass AgsGstreamerFileClass;

/**
 * AgsGstreamerFileFlags:
 * @AGS_GSTREAMER_FILE_ADDED_TO_REGISTRY: the gstreamer_file was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_GSTREAMER_FILE_CONNECTED: indicates the gstreamer_file was connected by calling #AgsConnectable::connect()
 * 
 * Enum values to control the behavior or indicate internal state of #AgsGstreamerFile by
 * enable/disable as flags.
 */
typedef enum{
  AGS_GSTREAMER_FILE_ADDED_TO_REGISTRY    = 1,
  AGS_GSTREAMER_FILE_CONNECTED            = 1 <<  1,
}AgsGstreamerFileFlags;

struct _AgsGstreamerFile
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  gchar *filename;
  
  guint audio_channels;
  gint64 *audio_channel_written;

  guint samplerate;
  guint buffer_size;
  guint format;

  guint64 offset;
  guint64 buffer_offset;

  void *full_buffer;
  void *buffer;

  guchar *pointer;
  guchar *current;
  gsize length;

  GstElement *read_pipeline;
  gboolean read_pipeline_running;
  
  GstElement *write_pipeline;
  gboolean write_pipeline_running;

  GstElement *playbin;
  GstElement *video_sink;
  GstElement *audio_sink;
  GstElement *text_sink;

  GstEncodingProfile *encoding_profile;
  
  GstElement *rw_video_app_src;
  GstElement *rw_audio_app_src;
  GstElement *rw_text_app_src;
  GstElement *rw_audio_tee;
  GstElement *rw_video_file_sink_queue;
  GstElement *rw_audio_file_sink_queue;
  GstElement *rw_text_file_sink_queue;
  GstElement *rw_audio_convert;
  GstElement *rw_audio_resample;
  GstElement *rw_audio_mixer;
  GstElement *rw_file_encoder;
  GstElement *rw_file_sink;
  GstElement *rw_audio_app_sink_queue;
  GstElement *rw_audio_app_sink;

  guint prev_frame_count;
  
  GstSample *last_sample;

  guint source_id;
  
  GList *rw_buffer;
  GstBuffer *current_buffer;
};

struct _AgsGstreamerFileClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_file_get_type();

gboolean ags_gstreamer_file_test_flags(AgsGstreamerFile *gstreamer_file, guint flags);
void ags_gstreamer_file_set_flags(AgsGstreamerFile *gstreamer_file, guint flags);
void ags_gstreamer_file_unset_flags(AgsGstreamerFile *gstreamer_file, guint flags);

gboolean ags_gstreamer_file_check_suffix(gchar *filename);

GstEncodingProfile* ags_gstreamer_file_create_mp3_encoding_profile(AgsGstreamerFile *gstreamer_file);
GstEncodingProfile* ags_gstreamer_file_create_aac_encoding_profile(AgsGstreamerFile *gstreamer_file);
GstEncodingProfile* ags_gstreamer_file_create_mp4_encoding_profile(AgsGstreamerFile *gstreamer_file);
GstEncodingProfile* ags_gstreamer_file_create_mkv_encoding_profile(AgsGstreamerFile *gstreamer_file);
GstEncodingProfile* ags_gstreamer_file_create_webm_encoding_profile(AgsGstreamerFile *gstreamer_file);
GstEncodingProfile* ags_gstreamer_file_create_mpeg_encoding_profile(AgsGstreamerFile *gstreamer_file);

gboolean ags_gstreamer_file_detect_encoding_profile(AgsGstreamerFile *gstreamer_file);

AgsGstreamerFile* ags_gstreamer_file_new();

G_END_DECLS

#endif /*__AGS_GSTREAMER_FILE_H__*/
