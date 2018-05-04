/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/file/ags_ipatch_sample.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_ipatch_sample_class_init(AgsIpatchSampleClass *ipatch_sample);
void ags_ipatch_sample_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_ipatch_sample_init(AgsIpatchSample *ipatch_sample);
void ags_ipatch_sample_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_ipatch_sample_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_ipatch_sample_dispose(GObject *gobject);
void ags_ipatch_sample_finalize(GObject *gobject);

gboolean ags_ipatch_sample_info(AgsSoundResource *sound_resource,
				guint *frame_count,
				guint *loop_start, guint *loop_end);
void ags_ipatch_sample_set_presets(AgsSoundResource *sound_resource,
				   guint channels,
				   guint samplerate,
				   guint buffer_size,
				   guint format);
void ags_ipatch_sample_get_presets(AgsSoundResource *sound_resource,
				   guint *channels,
				   guint *samplerate,
				   guint *buffer_size,
				   guint *format);
guint ags_ipatch_sample_read(AgsSoundResource *sound_resource,
			     void *dbuffer,
			     guint audio_channel,
			     guint frame_count, guint format);
void ags_ipatch_sample_write(AgsSoundResource *sound_resource,
			     void *sbuffer,
			     guint audio_channel,
			     guint frame_count, guint format);
void ags_ipatch_sample_flush(AgsSoundResource *sound_resource);
void ags_ipatch_sample_seek(AgsSoundResource *sound_resource,
			    gint64 frame_count, gint whence);

/**
 * SECTION:ags_ipatch_sample
 * @short_description: interfacing Soundfont2 related API of libinstpatch
 * @title: AgsIpatchSample
 * @section_id:
 * @include: ags/audio/file/ags_ipatch_sample.h
 *
 * #AgsIpatchSample is the base object to ineract with Soundfont2 related API.
 */

enum{
  PROP_0,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_SAMPLE,
};

static gpointer ags_ipatch_sample_parent_class = NULL;
static AgsSoundResourceInterface *ags_ipatch_sample_parent_sound_resource_interface;

GType
ags_ipatch_sample_get_type()
{
  static GType ags_type_ipatch_sample = 0;

  if(!ags_type_ipatch_sample){
    static const GTypeInfo ags_ipatch_sample_info = {
      sizeof(AgsIpatchSampleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ipatch_sample_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsIpatchSample),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ipatch_sample_init,
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sample_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_sample = g_type_register_static(G_TYPE_OBJECT,
						    "AgsIpatchSample",
						    &ags_ipatch_sample_info,
						    0);

    g_type_add_interface_static(ags_type_ipatch_sample,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);
  }
  
  return(ags_type_ipatch_sample);
}

void
ags_ipatch_sample_class_init(AgsIpatchSampleClass *ipatch_sample)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ipatch_sample_parent_class = g_type_class_peek_parent(ipatch_sample);

  gobject = (GObjectClass *) ipatch_sample;

  gobject->set_property = ags_ipatch_sample_set_property;
  gobject->get_property = ags_ipatch_sample_get_property;

  gobject->dispose = ags_ipatch_sample_dispose;
  gobject->finalize = ags_ipatch_sample_finalize;

  /* properties */
  /**
   * AgsIpatchSample:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("using buffer size"),
				 i18n_pspec("The buffer size to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsIpatchSample:format:
   *
   * The format to be used.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("using format"),
				 i18n_pspec("The format to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsIpatchSample:sample:
   *
   * The assigned output #IpatchSample.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("sample",
				   i18n_pspec("assigned sample"),
				   i18n_pspec("The assigned sample"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLE,
				  param_spec);
}

void
ags_ipatch_sample_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  ags_ipatch_sample_parent_sound_resource_interface = g_type_interface_peek_parent(sound_resource);

  sound_resource->open = NULL;
  sound_resource->rw_open = NULL;

  sound_resource->load = NULL;

  sound_resource->info = ags_ipatch_sample_info;

  sound_resource->set_presets = ags_ipatch_sample_set_presets;
  sound_resource->get_presets = ags_ipatch_sample_get_presets;
  
  sound_resource->read = ags_ipatch_sample_read;

  sound_resource->write = ags_ipatch_sample_write;
  sound_resource->flush = ags_ipatch_sample_flush;
  
  sound_resource->seek = ags_ipatch_sample_seek;

  sound_resource->close = NULL;
}

void
ags_ipatch_sample_init(AgsIpatchSample *ipatch_sample)
{
  AgsConfig *config;

  config = ags_config_get_instance();
  
  ipatch_sample->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  ipatch_sample->format = AGS_SOUNDCARD_DOUBLE;

  ipatch_sample->offset = 0;
  
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->buffer_size,
					   ipatch_sample->format);
}

void
ags_ipatch_sample_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = AGS_IPATCH_SAMPLE(gobject);

  switch(prop_id){
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == ipatch_sample->buffer_size){
	return;	
      }
      
      ags_stream_free(ipatch_sample->buffer);

      ipatch_sample->buffer_size = buffer_size;
      ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->buffer_size,
					       ipatch_sample->format);
    }
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == ipatch_sample->format){
	return;	
      }

      ags_stream_free(ipatch_sample->buffer);

      ipatch_sample->format = format;
      ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->buffer_size,
					       ipatch_sample->format);
    }
    break;
  case PROP_SAMPLE:
    {
#ifdef AGS_WITH_LIBINSTPATCH
      IpatchContainer *sample;

      sample = g_value_get_object(value);

      if(ipatch_sample->sample == sample){
	return;
      }

      if(ipatch_sample->sample != NULL){
	g_object_unref(ipatch_sample->sample);
      }

      if(sample != NULL){
	g_object_ref(sample);
      }
      
      ipatch_sample->sample = sample;
#endif
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_sample_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsIpatchSample *ipatch_sample;

  switch(prop_id){
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, ipatch_sample->buffer_size);
    }
  case PROP_FORMAT:
    {
      g_value_set_uint(value, ipatch_sample->format);
    }
    break;
  case PROP_SAMPLE:
    {
      g_value_set_object(value, ipatch_sample->sample);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ipatch_sample_dispose(GObject *gobject)
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = AGS_IPATCH_SAMPLE(gobject);

  if(ipatch_sample->sample != NULL){
    g_object_unref(ipatch_sample->sample);

    ipatch_sample->sample = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_sample_parent_class)->dispose(gobject);
}

void
ags_ipatch_sample_finalize(GObject *gobject)
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = AGS_IPATCH_SAMPLE(gobject);

  ags_stream_free(ipatch_sample->buffer);

  if(ipatch_sample->sample != NULL){
    g_object_unref(ipatch_sample->sample);
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_sample_parent_class)->finalize(gobject);
}

gboolean
ags_ipatch_sample_info(AgsSoundResource *sound_resource,
		       guint *frame_count,
		       guint *loop_start, guint *loop_end)
{
  AgsIpatchSample *ipatch_sample;

  guint sample_frame_count;
  guint sample_loop_start, sample_loop_end;
  
  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  sample_frame_count = 0;

  sample_loop_start = 0;
  sample_loop_end = 0;
  
#ifdef AGS_WITH_LIBINSTPATCH
  g_object_get(ipatch_sample->sample,
	       "sample-size", &sample_frame_count,
	       "loop-start", &sample_loop_start,
	       "loop-end", &sample_loop_end,
	       NULL);
#endif

  if(frame_count != NULL){
    *frame_count = sample_frame_count;
  }
  
  if(loop_start != NULL){
    *loop_start = sample_loop_start;
  }

  if(loop_end != NULL){
    *loop_end = sample_loop_end;
  }

  return(TRUE);
}

void
ags_ipatch_sample_set_presets(AgsSoundResource *sound_resource,
			      guint channels,
			      guint samplerate,
			      guint buffer_size,
			      guint format)
{
  AgsIpatchSample *ipatch_sample;

  gint sample_format;
  
  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  if(channels > IPATCH_SAMPLE_MAX_CHANNELS){
    g_critical("max channels exceeded");

    return;
  }
  
#ifdef AGS_WITH_LIBINSTPATCH
  g_object_get(ipatch_sample->sample,
	       "sample-format", &sample_format,
	       NULL);

  sample_format &= (~IPATCH_SAMPLE_CHANNEL_MASK);
  sample_format |= (IPATCH_SAMPLE_CHANNEL_MASK & ((channels - 1) << IPATCH_SAMPLE_CHANNEL_SHIFT));
  
  g_object_set(ipatch_sample->sample,
	       "sample-rate", samplerate,
	       "sample-format", sample_format,
	       NULL);
#endif

  g_object_set(ipatch_sample,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_ipatch_sample_get_presets(AgsSoundResource *sound_resource,
			      guint *channels,
			      guint *samplerate,
			      guint *buffer_size,
			      guint *format)
{
  AgsIpatchSample *ipatch_sample;

  gint sample_format;
  guint sample_channels;
  guint sample_samplerate;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);
  
  sample_format = 0;
  sample_samplerate = 0;
  
#ifdef AGS_WITH_LIBINSTPATCH
  g_object_get(ipatch_sample->sample,
	       "sample-format", &sample_format,
	       "sample-rate", &sample_samplerate,
	       NULL);
#endif

  if(channels != NULL){
    *channels = IPATCH_SAMPLE_FORMAT_GET_CHANNEL_COUNT(sample_format);
  }
  
  if(samplerate != NULL){
    *samplerate = sample_samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = ipatch_sample->buffer_size;
  }

  if(format != NULL){
    *format = ipatch_sample->format;
  }
}

guint
ags_ipatch_sample_read(AgsSoundResource *sound_resource,
		       void *dbuffer,
		       guint audio_channel,
		       guint frame_count, guint format)
{
  AgsIpatchSample *ipatch_sample;

  guint total_frame_count;
  guint copy_mode;
  guint read_count;
  guint i;
  
  GError *error;
  
  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

#ifdef AGS_WITH_LIBINSTPATCH
  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);
  
  if(ipatch_sample->offset >= total_frame_count){
    return(0);
  }

  if(ipatch_sample->offset + frame_count >= total_frame_count){
    frame_count = total_frame_count - ipatch_sample->offset;
  }

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(ipatch_sample->format));
  
  read_count = ipatch_sample->buffer_size;
  
  for(i = 0; i < frame_count; ){
    if(i + ipatch_sample->buffer_size > frame_count){
      read_count = frame_count - i;
    }
    
    ags_audio_buffer_util_clear_buffer(ipatch_sample->buffer, 1,
				       ipatch_sample->buffer_size, ipatch_sample->format);
  
    error = NULL;

    switch(ipatch_sample->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				     ipatch_sample->offset,
				     read_count,
				     ipatch_sample->buffer,
				     IPATCH_SAMPLE_8BIT | IPATCH_SAMPLE_MONO,
				     IPATCH_SAMPLE_MAP_CHANNEL(0, audio_channel),
				     &error);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				     ipatch_sample->offset,
				     read_count,
				     ipatch_sample->buffer,
				     IPATCH_SAMPLE_16BIT | IPATCH_SAMPLE_MONO,
				     IPATCH_SAMPLE_MAP_CHANNEL(0, audio_channel),
				     &error);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				     ipatch_sample->offset,
				     read_count,
				     ipatch_sample->buffer,
				     IPATCH_SAMPLE_24BIT | IPATCH_SAMPLE_MONO,
				     IPATCH_SAMPLE_MAP_CHANNEL(0, audio_channel),
				     &error);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				     ipatch_sample->offset,
				     read_count,
				     ipatch_sample->buffer,
				     IPATCH_SAMPLE_32BIT | IPATCH_SAMPLE_MONO,
				     IPATCH_SAMPLE_MAP_CHANNEL(0, audio_channel),
				     &error);
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				     ipatch_sample->offset,
				     read_count,
				     ipatch_sample->buffer,
				     IPATCH_SAMPLE_FLOAT | IPATCH_SAMPLE_MONO,
				     IPATCH_SAMPLE_MAP_CHANNEL(0, audio_channel),
				     &error);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				     ipatch_sample->offset,
				     read_count,
				     ipatch_sample->buffer,
				     IPATCH_SAMPLE_DOUBLE | IPATCH_SAMPLE_MONO,
				     IPATCH_SAMPLE_MAP_CHANNEL(0, audio_channel),
				     &error);
      }
      break;
    default:
      {
	g_warning("unsupported format");
      }
    }

    ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, 1, i,
						ipatch_sample->buffer, 1, 0,
						read_count, copy_mode);
    
    i += read_count;
  }

  ipatch_sample->offset += frame_count;
#else
  frame_count = 0;
#endif

  return(frame_count);
}

void
ags_ipatch_sample_write(AgsSoundResource *sound_resource,
			void *sbuffer,
			guint audio_channel,
			guint frame_count, guint format)
{
  AgsIpatchSample *ipatch_sample;

  GError *error;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  error = NULL;
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    sbuffer,
				    IPATCH_SAMPLE_8BIT | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    sbuffer,
				    IPATCH_SAMPLE_16BIT | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    sbuffer,
				    IPATCH_SAMPLE_24BIT | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    sbuffer,
				    IPATCH_SAMPLE_32BIT | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    sbuffer,
				    IPATCH_SAMPLE_FLOAT | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    sbuffer,
				    IPATCH_SAMPLE_DOUBLE | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  }
}

void
ags_ipatch_sample_flush(AgsSoundResource *sound_resource)
{
  //TODO:JK: implement me
}

void
ags_ipatch_sample_seek(AgsSoundResource *sound_resource,
		       gint64 frame_count, gint whence)
{
  AgsIpatchSample *ipatch_sample;

  guint total_frame_count;
  
  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);

  if(whence == G_SEEK_CUR){
    if(frame_count >= 0){
      if(ipatch_sample->offset + frame_count < total_frame_count){
	ipatch_sample->offset += total_frame_count;
      }else{
	ipatch_sample->offset = total_frame_count;
      }
    }else{
      if(ipatch_sample->offset + frame_count >= 0){
	ipatch_sample->offset += total_frame_count;
      }else{
	ipatch_sample->offset = 0;
      }
    } 
  }else if(whence == G_SEEK_SET){
    if(frame_count >= 0){
      if(frame_count < total_frame_count){
	ipatch_sample->offset = frame_count;
      }else{
	ipatch_sample->offset = total_frame_count;
      }
    }else{
      ipatch_sample->offset = 0;
    }
  }else if(whence == G_SEEK_END){
    if(frame_count > 0){
      ipatch_sample->offset = total_frame_count;
    }else{
      if(total_frame_count + frame_count > 0){
	ipatch_sample->offset = total_frame_count + total_frame_count;
      }else{
	ipatch_sample->offset = 0;
      }
    }
  }
}

/**
 * ags_ipatch_sample_new:
 *
 * Creates a new instance of #AgsIpatchSample.
 *
 * Returns: the new #AgsIpatchSample.
 *
 * Since: 2.0.0
 */
AgsIpatchSample*
ags_ipatch_sample_new()
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = (AgsIpatchSample *) g_object_new(AGS_TYPE_IPATCH_SAMPLE,
						   NULL);

  return(ipatch_sample);
}
