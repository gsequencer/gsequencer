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

#include <ags/audio/file/ags_gstreamer_file_audio_src.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/i18n.h>

void ags_gstreamer_file_audio_src_class_init(AgsGstreamerFileAudioSrcClass *gstreamer_file_audio_src);
void ags_gstreamer_file_audio_src_init(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src);
void ags_gstreamer_file_audio_src_dispose(GObject *gobject);
void ags_gstreamer_file_audio_src_finalize(GObject *gobject);

gboolean ags_gstreamer_file_audio_src_set_caps(GstBaseSrc *src,
					       GstCaps *caps);

gboolean ags_gstreamer_file_audio_src_open(GstAudioSrc *src);
gboolean ags_gstreamer_file_audio_src_prepare(GstAudioSrc *src, GstAudioRingBufferSpec *spec);
gboolean ags_gstreamer_file_audio_src_unprepare(GstAudioSrc *src);
gboolean ags_gstreamer_file_audio_src_close(GstAudioSrc *src);
guint ags_gstreamer_file_audio_src_read(GstAudioSrc *src, gpointer data, guint length,
					GstClockTime *timestamp);
guint ags_gstreamer_file_audio_src_delay(GstAudioSrc *src);
void ags_gstreamer_file_audio_src_reset(GstAudioSrc *src);

/**
 * SECTION:ags_gstreamer_file_audio_src
 * @short_description: gstreamer audio src
 * @title: AgsGstreamerFileAudioSrc
 * @section_id:
 * @include: ags/audio/file/ags_gstreamer_file_audio_src.h
 *
 * #AgsGstreamerFileAudioSrc is the base object to ineract with libgstreamer-1.0 audio src.
 */

static gpointer ags_gstreamer_file_audio_src_parent_class = NULL;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
GstStaticPadTemplate ags_gstreamer_file_audio_src_src_template = GST_STATIC_PAD_TEMPLATE ("src",
											  GST_PAD_SRC,
											  GST_PAD_ALWAYS,
											  GST_STATIC_CAPS ("audio/x-raw, "
													   "format = (string) { F64LE }, "
													   "layout = (string) { interleaved }, "
													   "rate = " GST_AUDIO_RATE_RANGE ", "
													   "channels = " GST_AUDIO_CHANNELS_RANGE));
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
GstStaticPadTemplate ags_gstreamer_file_audio_src_src_template = GST_STATIC_PAD_TEMPLATE ("src",
											  GST_PAD_SRC,
											  GST_PAD_ALWAYS,
											  GST_STATIC_CAPS ("audio/x-raw, "
													   "format = (string) { F64BE }, "
													   "layout = (string) { interleaved }, "
													   "rate = " GST_AUDIO_RATE_RANGE ", "
													   "channels = " GST_AUDIO_CHANNELS_RANGE));
#else
GstStaticPadTemplate ags_gstreamer_file_audio_src_src_template = GST_STATIC_PAD_TEMPLATE ("src",
											  GST_PAD_SRC,
											  GST_PAD_ALWAYS,
											  GST_STATIC_CAPS ("audio/x-raw, "
													   "format = (string) { F64LE }, "
													   "layout = (string) { interleaved }, "
													   "rate = " GST_AUDIO_RATE_RANGE ", "
													   "channels = " GST_AUDIO_CHANNELS_RANGE));
#endif

GType
ags_gstreamer_file_audio_src_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_file_audio_src = 0;

    static const GTypeInfo ags_gstreamer_file_audio_src_info = {
      sizeof (AgsGstreamerFileAudioSrcClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_file_audio_src_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerFileAudioSrc),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_file_audio_src_init,
    };

    ags_type_gstreamer_file_audio_src = g_type_register_static(GST_TYPE_AUDIO_SRC,
							       "AgsGstreamerFileAudioSrc",
							       &ags_gstreamer_file_audio_src_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_file_audio_src);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_file_audio_src_class_init(AgsGstreamerFileAudioSrcClass *gstreamer_file_audio_src)
{
  GObjectClass *gobject;
  GstElementClass *element;
  GstBaseSrcClass *base_src;
  GstAudioSrcClass *audio_src;

  ags_gstreamer_file_audio_src_parent_class = g_type_class_peek_parent(gstreamer_file_audio_src);

  gobject = (GObjectClass *) gstreamer_file_audio_src;

  gobject->dispose = ags_gstreamer_file_audio_src_dispose;
  gobject->finalize = ags_gstreamer_file_audio_src_finalize;

  /* GstElementClass */
  element = (GstElementClass *) gstreamer_file_audio_src;

  gst_element_class_add_static_pad_template(element,
					    &ags_gstreamer_file_audio_src_src_template);

  /* GstBaseSrcClass */
  base_src = (GstAudioSrcClass *) gstreamer_file_audio_src;

  base_src->set_caps = ags_gstreamer_file_audio_src_set_caps;
  
  /* GstAudioSrcClass */
  audio_src = (GstAudioSrcClass *) gstreamer_file_audio_src;
  
  audio_src->open = ags_gstreamer_file_audio_src_open;
  audio_src->prepare = ags_gstreamer_file_audio_src_prepare;
  audio_src->unprepare = ags_gstreamer_file_audio_src_unprepare;
  audio_src->close = ags_gstreamer_file_audio_src_close;
  audio_src->read = ags_gstreamer_file_audio_src_read;
  audio_src->delay = ags_gstreamer_file_audio_src_delay;
  audio_src->reset = ags_gstreamer_file_audio_src_reset;
}

void
ags_gstreamer_file_audio_src_init(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src)
{
  AgsConfig *config;

  guint blocksize;

  gstreamer_file_audio_src->status_flags = 0;
  
  /* add gstreamer file audio src mutex */
  g_rec_mutex_init(&(gstreamer_file_audio_src->obj_mutex));  

  g_mutex_init(&(gstreamer_file_audio_src->wakeup_mutex));
  
  g_cond_init(&(gstreamer_file_audio_src->wakeup_cond));

  config = ags_config_get_instance();

  gstreamer_file_audio_src->gstreamer_file = NULL;

  gstreamer_file_audio_src->audio_channels = 1;

  gstreamer_file_audio_src->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  gstreamer_file_audio_src->format = AGS_SOUNDCARD_DOUBLE;

  gstreamer_file_audio_src->buffer = ags_stream_alloc(gstreamer_file_audio_src->audio_channels * gstreamer_file_audio_src->buffer_size,
						      gstreamer_file_audio_src->format);

  blocksize = gstreamer_file_audio_src->audio_channels * gstreamer_file_audio_src->buffer_size * sizeof(gdouble);
  
  g_object_set(gstreamer_file_audio_src,
	       "blocksize", blocksize,
	       NULL);
}

void
ags_gstreamer_file_audio_src_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_src_parent_class)->dispose(gobject);
}

void
ags_gstreamer_file_audio_src_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_audio_src_parent_class)->finalize(gobject);
}

gboolean
ags_gstreamer_file_audio_src_set_caps(GstBaseSrc *src,
				      GstCaps *caps)
{
  AgsGstreamerFileAudioSrc *gstreamer_file_audio_src;

  gboolean success;
  
  GRecMutex *gstreamer_file_audio_src_mutex;
  
  gstreamer_file_audio_src = (AgsGstreamerFileAudioSrc *) src;

  gstreamer_file_audio_src_mutex = AGS_GSTREAMER_FILE_AUDIO_SRC_GET_OBJ_MUTEX(gstreamer_file_audio_src);

  success = FALSE;

  g_rec_mutex_lock(gstreamer_file_audio_src_mutex);

  if(gst_audio_info_from_caps(&(gstreamer_file_audio_src->info), caps)){
    success = TRUE;
  }
  
  g_rec_mutex_unlock(gstreamer_file_audio_src_mutex);

  return(success);
}

gboolean
ags_gstreamer_file_audio_src_open(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_src_prepare(GstAudioSrc *src, GstAudioRingBufferSpec *spec)
{
  AgsGstreamerFileAudioSrc *gstreamer_file_audio_src;

  guint blocksize;
  
  GRecMutex *gstreamer_file_audio_src_mutex;
  
  gstreamer_file_audio_src = (AgsGstreamerFileAudioSrc *) src;

  gstreamer_file_audio_src_mutex = AGS_GSTREAMER_FILE_AUDIO_SRC_GET_OBJ_MUTEX(gstreamer_file_audio_src);

  g_rec_mutex_lock(gstreamer_file_audio_src_mutex);
  
  gstreamer_file_audio_src->ring_buffer_samplerate = spec->info.rate;
  gstreamer_file_audio_src->ring_buffer_channels = spec->info.channels;

  blocksize = gstreamer_file_audio_src->audio_channels * gstreamer_file_audio_src->buffer_size * sizeof(gdouble);
  
  if(gstreamer_file_audio_src->ring_buffer_channels != gstreamer_file_audio_src->audio_channels){
    gstreamer_file_audio_src->audio_channels = gstreamer_file_audio_src->ring_buffer_channels;

    ags_stream_free(gstreamer_file_audio_src->buffer);
    
    gstreamer_file_audio_src->buffer = ags_stream_alloc(gstreamer_file_audio_src->audio_channels * gstreamer_file_audio_src->buffer_size,
							gstreamer_file_audio_src->format);

    blocksize = gstreamer_file_audio_src->audio_channels * gstreamer_file_audio_src->buffer_size * sizeof(gdouble);
  }
  
  g_rec_mutex_unlock(gstreamer_file_audio_src_mutex);

  g_object_set(gstreamer_file_audio_src,
	       "blocksize", blocksize,
	       NULL);

  ags_gstreamer_file_audio_src_unset_status_flags(gstreamer_file_audio_src, AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_CLEAN);

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_src_unprepare(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(TRUE);
}

gboolean
ags_gstreamer_file_audio_src_close(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(TRUE);
}

guint
ags_gstreamer_file_audio_src_read(GstAudioSrc *src, gpointer data, guint length,
				  GstClockTime *timestamp)
{
  AgsGstreamerFileAudioSrc *gstreamer_file_audio_src;

  guint multi_frame_count;
  guint available_multi_frame_count;
  
  GRecMutex *gstreamer_file_audio_src_mutex;
  
  gstreamer_file_audio_src = (AgsGstreamerFileAudioSrc *) src;

  gstreamer_file_audio_src_mutex = AGS_GSTREAMER_FILE_AUDIO_SRC_GET_OBJ_MUTEX(gstreamer_file_audio_src);

  /* sync */
  g_mutex_lock(&(gstreamer_file_audio_src->wakeup_mutex));

  if(ags_gstreamer_file_audio_src_test_status_flags(gstreamer_file_audio_src, AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_WAIT)){
    ags_gstreamer_file_audio_src_unset_status_flags(gstreamer_file_audio_src, AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_DONE);
    
    while(ags_gstreamer_file_audio_src_test_status_flags(gstreamer_file_audio_src, AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_WAIT) &&
	  !ags_gstreamer_file_audio_src_test_status_flags(gstreamer_file_audio_src, AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_DONE)){
      g_cond_wait(&(gstreamer_file_audio_src->wakeup_cond),
		  &(gstreamer_file_audio_src->wakeup_mutex));
    }
  }
  
  ags_gstreamer_file_audio_src_set_status_flags(gstreamer_file_audio_src, (AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_WAIT |
									   AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_DONE));
  
  g_mutex_unlock(&(gstreamer_file_audio_src->wakeup_mutex));

  /* copy */
  g_rec_mutex_lock(gstreamer_file_audio_src_mutex);

  multi_frame_count = gstreamer_file_audio_src->audio_channels * gstreamer_file_audio_src->buffer_size;

  available_multi_frame_count = length / sizeof(gdouble);
    
  ags_audio_buffer_util_copy_double_to_double(data, 1,
					      gstreamer_file_audio_src->buffer, 1,
					      available_multi_frame_count);

  ags_audio_buffer_util_clear_double(gstreamer_file_audio_src->buffer, 1, multi_frame_count);

  g_rec_mutex_unlock(gstreamer_file_audio_src_mutex);

  ags_gstreamer_file_audio_src_set_status_flags(gstreamer_file_audio_src, AGS_GSTREAMER_FILE_AUDIO_SRC_STATUS_CLEAN);

  return((gint) length);
}

guint
ags_gstreamer_file_audio_src_delay(GstAudioSrc *src)
{
  //TODO:JK: implement me

  return(0);
}

void
ags_gstreamer_file_audio_src_reset(GstAudioSrc *src)
{
  //TODO:JK: implement me
}

/**
 * ags_gstreamer_file_audio_src_test_status_flags:
 * @gstreamer_file_audio_src: the #AgsGstreamerFileAudioSrc
 * @status_flags: status flags
 * 
 * Test @status_flags of @gstreamer_file_audio_src.
 * 
 * Returns: %TRUE if status flags set, otherwise %FALSE
 * 
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_file_audio_src_test_status_flags(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src, guint status_flags)
{
  gboolean retval;

  GRecMutex *gstreamer_file_audio_src_mutex;

  if(!AGS_IS_GSTREAMER_FILE_AUDIO_SRC(gstreamer_file_audio_src)){
    return(FALSE);
  }
  
  gstreamer_file_audio_src_mutex = AGS_GSTREAMER_FILE_AUDIO_SRC_GET_OBJ_MUTEX(gstreamer_file_audio_src);

  g_rec_mutex_lock(gstreamer_file_audio_src_mutex);

  retval = (status_flags & (gstreamer_file_audio_src->status_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_file_audio_src_mutex);

  return(retval);
}

/**
 * ags_gstreamer_file_audio_src_set_status_flags:
 * @gstreamer_file_audio_src: the #AgsGstreamerFileAudioSrc
 * @status_flags: status flags
 * 
 * Set @status_flags of @gstreamer_file_audio_src.
 * 
 * Since: 3.6.0
 */
void
ags_gstreamer_file_audio_src_set_status_flags(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src, guint status_flags)
{
  GRecMutex *gstreamer_file_audio_src_mutex;

  if(!AGS_IS_GSTREAMER_FILE_AUDIO_SRC(gstreamer_file_audio_src)){
    return;
  }
  
  gstreamer_file_audio_src_mutex = AGS_GSTREAMER_FILE_AUDIO_SRC_GET_OBJ_MUTEX(gstreamer_file_audio_src);

  g_rec_mutex_lock(gstreamer_file_audio_src_mutex);

  gstreamer_file_audio_src->status_flags |= status_flags;
  
  g_rec_mutex_unlock(gstreamer_file_audio_src_mutex);
}

/**
 * ags_gstreamer_file_audio_src_unset_status_flags:
 * @gstreamer_file_audio_src: the #AgsGstreamerFileAudioSrc
 * @status_flags: status flags
 * 
 * Unset @status_flags of @gstreamer_file_audio_src.
 * 
 * Since: 3.6.0
 */
void
ags_gstreamer_file_audio_src_unset_status_flags(AgsGstreamerFileAudioSrc *gstreamer_file_audio_src, guint status_flags)
{
  GRecMutex *gstreamer_file_audio_src_mutex;

  if(!AGS_IS_GSTREAMER_FILE_AUDIO_SRC(gstreamer_file_audio_src)){
    return;
  }
  
  gstreamer_file_audio_src_mutex = AGS_GSTREAMER_FILE_AUDIO_SRC_GET_OBJ_MUTEX(gstreamer_file_audio_src);

  g_rec_mutex_lock(gstreamer_file_audio_src_mutex);

  gstreamer_file_audio_src->status_flags &= (~status_flags);
  
  g_rec_mutex_unlock(gstreamer_file_audio_src_mutex);
}

/**
 * ags_gstreamer_file_audio_src_new:
 *
 * Creates a new instance of #AgsGstreamerFileAudioSrc.
 *
 * Returns: the new #AgsGstreamerFileAudioSrc.
 *
 * Since: 3.6.0
 */
AgsGstreamerFileAudioSrc*
ags_gstreamer_file_audio_src_new()
{
  AgsGstreamerFileAudioSrc *gstreamer_file_audio_src;

  gstreamer_file_audio_src = (AgsGstreamerFileAudioSrc *) g_object_new(AGS_TYPE_GSTREAMER_FILE_AUDIO_SRC,
								       NULL);

  return(gstreamer_file_audio_src);
}
