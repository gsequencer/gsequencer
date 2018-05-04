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
			    guint frame_count, gint whence);

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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_ipatch_sample_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ipatch_sample = g_type_register_static(G_TYPE_OBJECT,
						    "AgsIpatchSample",
						    &ags_ipatch_sample_info,
						    0);

    g_type_add_interface_static(ags_type_ipatch_sample,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
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

  gobject->dispose = ags_ipatch_sample_dispose;
  gobject->finalize = ags_ipatch_sample_finalize;

  /* properties */
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
  ipatch_sample->format = ags_soundcard_helper_config_get_format(config);

  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->buffer_size,
					   ipatch_sample->format);
}

void
ags_ipatch_sample_dispose(GObject *gobject)
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = AGS_IPATCH_SAMPLE(gobject);

  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_sample_parent_class)->dispose(gobject);
}

void
ags_ipatch_sample_finalize(GObject *gobject)
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = AGS_IPATCH_SAMPLE(gobject);

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
  //TODO:JK: implement me
}

void
ags_ipatch_sample_get_presets(AgsSoundResource *sound_resource,
			      guint *channels,
			      guint *samplerate,
			      guint *buffer_size,
			      guint *format)
{
  AgsIpatchSample *ipatch_sample;

  guint sample_format;
  guint sample_channels;
  guint sample_samplerate;

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
  //TODO:JK: implement me
}

void
ags_ipatch_sample_write(AgsSoundResource *sound_resource,
			void *sbuffer,
			guint audio_channel,
			guint frame_count, guint format)
{
  //TODO:JK: implement me
}

void
ags_ipatch_sample_flush(AgsSoundResource *sound_resource)
{
  //TODO:JK: implement me
}

void
ags_ipatch_sample_seek(AgsSoundResource *sound_resource,
		       guint frame_count, gint whence)
{
  //TODO:JK: implement me
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
