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

#include <ags/audio/file/ags_gstreamer_file_audio_sink.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/i18n.h>

void ags_gstreamer_file_audio_sink_class_init(AgsGstreamerFileAudioSinkClass *gstreamer_file_audio_sink);
void ags_gstreamer_file_audio_sink_init(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink);
void ags_gstreamer_file_audio_sink_dispose(GObject *gobject);
void ags_gstreamer_file_audio_sink_finalize(GObject *gobject);

gboolean ags_gstreamer_file_audio_sink_open(GstAudioSink *sink);
gboolean ags_gstreamer_file_audio_sink_prepare(GstAudioSink *sink, GstAudioRingBufferSpec *spec);
gboolean ags_gstreamer_file_audio_sink_unprepare(GstAudioSink *sink);
gboolean ags_gstreamer_file_audio_sink_close(GstAudioSink *sink);
gint ags_gstreamer_file_audio_sink_write(GstAudioSink *sink, gpointer data, guint length);
guint ags_gstreamer_file_audio_sink_delay(GstAudioSink *sink);
void ags_gstreamer_file_audio_sink_reset(GstAudioSink *sink);

/**
 * SECTION:ags_gstreamer_file_audio_sink
 * @short_description: gstreamer audio sink
 * @title: AgsGstreamerFileAudioSink
 * @section_id:
 * @include: ags/audio/audio_sink/ags_gstreamer_file_audio_sink.h
 *
 * #AgsGstreamerFileAudioSink is the base object to ineract with libgstreamer-1.0 audio sink.
 */

static gpointer ags_gstreamer_file_audio_sink_parent_class = NULL;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static GstStaticPadTemplate ags_gstreamer_file_audio_sink_src_template = GST_STATIC_PAD_TEMPLATE ("src",
												  GST_PAD_SRC,
												  GST_PAD_ALWAYS,
												  GST_STATIC_CAPS ("audio/x-raw, "
														   "format = (string) F64LE, "
														   "layout = (string) { interleaved }, "
														   "rate = " GST_AUDIO_RATE_RANGE ", "
														   "channels = " GST_AUDIO_CHANNELS_RANGE));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
static GstStaticPadTemplate ags_gstreamer_file_audio_sink_src_template = GST_STATIC_PAD_TEMPLATE ("src",
												  GST_PAD_SRC,
												  GST_PAD_ALWAYS,
												  GST_STATIC_CAPS ("audio/x-raw, "
														   "format = (string) F64BE, "
														   "layout = (string) { interleaved }, "
														   "rate = " GST_AUDIO_RATE_RANGE ", "
														   "channels = " GST_AUDIO_CHANNELS_RANGE));
#else
static GstStaticPadTemplate ags_gstreamer_file_audio_sink_src_template = GST_STATIC_PAD_TEMPLATE ("src",
												  GST_PAD_SRC,
												  GST_PAD_ALWAYS,
												  GST_STATIC_CAPS ("audio/x-raw, "
														   "format = (string) F64LE, "
														   "layout = (string) { interleaved }, "
														   "rate = " GST_AUDIO_RATE_RANGE ", "
														   "channels = " GST_AUDIO_CHANNELS_RANGE));
#endif

GType
ags_gstreamer_file_audio_sink_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_file_audio_sink = 0;

    static const GTypeInfo ags_gstreamer_file_audio_sink_info = {
      sizeof (AgsGstreamerFileAudioSinkClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_file_audio_sink_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerFileAudioSink),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_file_audio_sink_init,
    };

    ags_type_gstreamer_file_audio_sink = g_type_register_static(GST_TYPE_AUDIO_SINK,
								"AgsGstreamerFileAudioSink",
								&ags_gstreamer_file_audio_sink_info,
								0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_file_audio_sink);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_file_audio_sink_class_init(AgsGstreamerFileAudioSinkClass *gstreamer_file_audio_sink)
{
  GObjectClass *gobject;
  GstElementClass *element;
  GstAudioSinkClass *audio_sink;
  
  ags_gstreamer_file_audio_sink_parent_class = g_type_class_peek_parent(gstreamer_file_audio_sink);

  gobject = (GObjectClass *) gstreamer_file_audio_sink;

  gobject->dispose = ags_gstreamer_file_audio_sink_dispose;
  gobject->finalize = ags_gstreamer_file_audio_sink_finalize;

  /* GstElementClass */
  element = (GstElementClass *) gstreamer_file_audio_sink;
  
  gst_element_class_add_static_pad_template(element,
					    &ags_gstreamer_file_audio_sink_src_template);

  /* GstAudioSinkClass */
  audio_sink = (GstAudioSinkClass *) gstreamer_file_audio_sink;
  
  audio_sink->open = ags_gstreamer_file_audio_sink_open;
  audio_sink->prepare = ags_gstreamer_file_audio_sink_prepare;
  audio_sink->unprepare = ags_gstreamer_file_audio_sink_unprepare;
  audio_sink->close = ags_gstreamer_file_audio_sink_close;
  audio_sink->write = ags_gstreamer_file_audio_sink_write;
  audio_sink->delay = ags_gstreamer_file_audio_sink_delay;
  audio_sink->reset = ags_gstreamer_file_audio_sink_reset;
}

void
ags_gstreamer_file_audio_sink_init(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink)
{
  AgsConfig *config;

  guint blocksize;

  gstreamer_file_audio_sink->status_flags = 0;
  
  /* add gstreamer file audio sink mutex */
  g_rec_mutex_init(&(gstreamer_file_audio_sink->obj_mutex));  

  g_mutex_init(&(gstreamer_file_audio_sink->wakeup_mutex));
  
  g_cond_init(&(gstreamer_file_audio_sink->wakeup_cond));

  config = ags_config_get_instance();

  gstreamer_file_audio_sink->gstreamer_file = NULL;

  gstreamer_file_audio_sink->audio_channels = 1;

  gstreamer_file_audio_sink->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  gstreamer_file_audio_sink->format = AGS_SOUNDCARD_DOUBLE;

  gstreamer_file_audio_sink->buffer = ags_stream_alloc(gstreamer_file_audio_sink->audio_channels * gstreamer_file_audio_sink->buffer_size,
						       gstreamer_file_audio_sink->format);

  blocksize = gstreamer_file_audio_sink->audio_channels * gstreamer_file_audio_sink->buffer_size * sizeof(gdouble);
  
  g_object_set(gstreamer_file_audio_sink,
	       "blocksize", blocksize,
	       NULL);
}

void
ags_gstreamer_file_audio_sink_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_sink_parent_class)->dispose(gobject);
}

void
ags_gstreamer_file_audio_sink_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_sink_parent_class)->finalize(gobject);
}

gboolean
ags_gstreamer_file_audio_sink_open(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_sink_prepare(GstAudioSink *sink, GstAudioRingBufferSpec *spec)
{
  AgsGstreamerFileAudioSink *gstreamer_file_audio_sink;

  guint blocksize;
  
  GRecMutex *gstreamer_file_audio_sink_mutex;
  
  gstreamer_file_audio_sink = (AgsGstreamerFileAudioSink *) sink;

  gstreamer_file_audio_sink_mutex = AGS_GSTREAMER_FILE_AUDIO_SINK_GET_OBJ_MUTEX(gstreamer_file_audio_sink);

  g_rec_mutex_lock(gstreamer_file_audio_sink_mutex);
  
  gstreamer_file_audio_sink->ring_buffer_samplerate = spec->info.rate;
  gstreamer_file_audio_sink->ring_buffer_channels = spec->info.channels;

  blocksize = gstreamer_file_audio_sink->audio_channels * gstreamer_file_audio_sink->buffer_size * sizeof(gdouble);
  
  if(gstreamer_file_audio_sink->ring_buffer_channels != gstreamer_file_audio_sink->audio_channels){
    gstreamer_file_audio_sink->audio_channels = gstreamer_file_audio_sink->ring_buffer_channels;

    ags_stream_free(gstreamer_file_audio_sink->buffer);
    
    gstreamer_file_audio_sink->buffer = ags_stream_alloc(gstreamer_file_audio_sink->audio_channels * gstreamer_file_audio_sink->buffer_size,
							 gstreamer_file_audio_sink->format);

    blocksize = gstreamer_file_audio_sink->audio_channels * gstreamer_file_audio_sink->buffer_size * sizeof(gdouble);
  }
  
  g_rec_mutex_unlock(gstreamer_file_audio_sink_mutex);

  g_object_set(gstreamer_file_audio_sink,
	       "blocksize", blocksize,
	       NULL);

  ags_gstreamer_file_audio_sink_unset_status_flags(gstreamer_file_audio_sink, AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DIRTY);

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_sink_unprepare(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_sink_close(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(TRUE);
}

gint
ags_gstreamer_file_audio_sink_write(GstAudioSink *sink, gpointer data, guint length)
{
  AgsGstreamerFileAudioSink *gstreamer_file_audio_sink;

  guint multi_frame_count;
  guint available_multi_frame_count;
  
  GRecMutex *gstreamer_file_audio_sink_mutex;
  
  gstreamer_file_audio_sink = (AgsGstreamerFileAudioSink *) sink;

  gstreamer_file_audio_sink_mutex = AGS_GSTREAMER_FILE_AUDIO_SINK_GET_OBJ_MUTEX(gstreamer_file_audio_sink);

  /* sync */
  g_mutex_lock(&(gstreamer_file_audio_sink->wakeup_mutex));

  if(ags_gstreamer_file_audio_sink_test_status_flags(gstreamer_file_audio_sink, AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_WAIT)){
    ags_gstreamer_file_audio_sink_unset_status_flags(gstreamer_file_audio_sink, AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DONE);
    
    while(ags_gstreamer_file_audio_sink_test_status_flags(gstreamer_file_audio_sink, AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_WAIT) &&
	  !ags_gstreamer_file_audio_sink_test_status_flags(gstreamer_file_audio_sink, AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DONE)){
      g_cond_wait(&(gstreamer_file_audio_sink->wakeup_cond),
		  &(gstreamer_file_audio_sink->wakeup_mutex));
    }
  }
  
  ags_gstreamer_file_audio_sink_set_status_flags(gstreamer_file_audio_sink, (AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_WAIT |
									     AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DONE));
  
  g_mutex_unlock(&(gstreamer_file_audio_sink->wakeup_mutex));

  /* copy */
  g_rec_mutex_lock(gstreamer_file_audio_sink_mutex);

  multi_frame_count = gstreamer_file_audio_sink->audio_channels * gstreamer_file_audio_sink->buffer_size;

  available_multi_frame_count = length / sizeof(gdouble);
  
  ags_audio_buffer_util_clear_double(gstreamer_file_audio_sink->buffer, 1, multi_frame_count);
  
  ags_audio_buffer_util_copy_double_to_double(gstreamer_file_audio_sink->buffer, 1,
					      data, 1,
					      available_multi_frame_count);

  g_rec_mutex_unlock(gstreamer_file_audio_sink_mutex);

  ags_gstreamer_file_audio_sink_set_status_flags(gstreamer_file_audio_sink, AGS_GSTREAMER_FILE_AUDIO_SINK_STATUS_DIRTY);

  return((gint) length);
}

guint
ags_gstreamer_file_audio_sink_delay(GstAudioSink *sink)
{
  //TODO:JK: implement me

  return(0);
}

void
ags_gstreamer_file_audio_sink_reset(GstAudioSink *sink)
{
  //TODO:JK: implement me
}

/**
 * ags_gstreamer_file_audio_sink_test_status_flags:
 * @gstreamer_file_audio_sink: the #AgsGstreamerFileAudioSink
 * @status_flags: status flags
 * 
 * Test @status_flags of @gstreamer_file_audio_sink.
 * 
 * Returns: %TRUE if status flags set, otherwise %FALSE
 * 
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_file_audio_sink_test_status_flags(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink, guint status_flags)
{
  gboolean retval;

  GRecMutex *gstreamer_file_audio_sink_mutex;

  if(!AGS_IS_GSTREAMER_FILE_AUDIO_SINK(gstreamer_file_audio_sink)){
    return(FALSE);
  }
  
  gstreamer_file_audio_sink_mutex = AGS_GSTREAMER_FILE_AUDIO_SINK_GET_OBJ_MUTEX(gstreamer_file_audio_sink);

  g_rec_mutex_lock(gstreamer_file_audio_sink_mutex);

  retval = (status_flags & (gstreamer_file_audio_sink->status_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_file_audio_sink_mutex);

  return(retval);
}

/**
 * ags_gstreamer_file_audio_sink_set_status_flags:
 * @gstreamer_file_audio_sink: the #AgsGstreamerFileAudioSink
 * @status_flags: status flags
 * 
 * Set @status_flags of @gstreamer_file_audio_sink.
 * 
 * Since: 3.6.0
 */
void
ags_gstreamer_file_audio_sink_set_status_flags(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink, guint status_flags)
{
  GRecMutex *gstreamer_file_audio_sink_mutex;

  if(!AGS_IS_GSTREAMER_FILE_AUDIO_SINK(gstreamer_file_audio_sink)){
    return;
  }
  
  gstreamer_file_audio_sink_mutex = AGS_GSTREAMER_FILE_AUDIO_SINK_GET_OBJ_MUTEX(gstreamer_file_audio_sink);

  g_rec_mutex_lock(gstreamer_file_audio_sink_mutex);

  gstreamer_file_audio_sink->status_flags |= status_flags;
  
  g_rec_mutex_unlock(gstreamer_file_audio_sink_mutex);
}

/**
 * ags_gstreamer_file_audio_sink_unset_status_flags:
 * @gstreamer_file_audio_sink: the #AgsGstreamerFileAudioSink
 * @status_flags: status flags
 * 
 * Unset @status_flags of @gstreamer_file_audio_sink.
 * 
 * Since: 3.6.0
 */
void
ags_gstreamer_file_audio_sink_unset_status_flags(AgsGstreamerFileAudioSink *gstreamer_file_audio_sink, guint status_flags)
{
  GRecMutex *gstreamer_file_audio_sink_mutex;

  if(!AGS_IS_GSTREAMER_FILE_AUDIO_SINK(gstreamer_file_audio_sink)){
    return;
  }
  
  gstreamer_file_audio_sink_mutex = AGS_GSTREAMER_FILE_AUDIO_SINK_GET_OBJ_MUTEX(gstreamer_file_audio_sink);

  g_rec_mutex_lock(gstreamer_file_audio_sink_mutex);

  gstreamer_file_audio_sink->status_flags &= (~status_flags);
  
  g_rec_mutex_unlock(gstreamer_file_audio_sink_mutex);
}

/**
 * ags_gstreamer_file_audio_sink_new:
 *
 * Creates a new instance of #AgsGstreamerFileAudioSink.
 *
 * Returns: the new #AgsGstreamerFileAudioSink.
 *
 * Since: 3.6.0
 */
AgsGstreamerFileAudioSink*
ags_gstreamer_file_audio_sink_new()
{
  AgsGstreamerFileAudioSink *gstreamer_file_audio_sink;

  gstreamer_file_audio_sink = (AgsGstreamerFileAudioSink *) g_object_new(AGS_TYPE_GSTREAMER_FILE_AUDIO_SINK,
									 NULL);

  return(gstreamer_file_audio_sink);
}
