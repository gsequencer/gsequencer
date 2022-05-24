/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_ipatch_sample_class_init(AgsIpatchSampleClass *ipatch_sample);
void ags_ipatch_sample_connectable_interface_init(AgsConnectableInterface *connectable);
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

AgsUUID* ags_ipatch_sample_get_uuid(AgsConnectable *connectable);
gboolean ags_ipatch_sample_has_resource(AgsConnectable *connectable);
gboolean ags_ipatch_sample_is_ready(AgsConnectable *connectable);
void ags_ipatch_sample_add_to_registry(AgsConnectable *connectable);
void ags_ipatch_sample_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_ipatch_sample_list_resource(AgsConnectable *connectable);
xmlNode* ags_ipatch_sample_xml_compose(AgsConnectable *connectable);
void ags_ipatch_sample_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_ipatch_sample_is_connected(AgsConnectable *connectable);
void ags_ipatch_sample_connect(AgsConnectable *connectable);
void ags_ipatch_sample_disconnect(AgsConnectable *connectable);

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
			     void *dbuffer, guint daudio_channels,
			     guint audio_channel,
			     guint frame_count, guint format);
void ags_ipatch_sample_write(AgsSoundResource *sound_resource,
			     void *sbuffer, guint saudio_channels,
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

GType
ags_ipatch_sample_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ipatch_sample = 0;

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
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_ipatch_sample,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ipatch_sample);
  }

  return g_define_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
ags_ipatch_sample_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_ipatch_sample_get_uuid;
  connectable->has_resource = ags_ipatch_sample_has_resource;
  connectable->is_ready = ags_ipatch_sample_is_ready;

  connectable->add_to_registry = ags_ipatch_sample_add_to_registry;
  connectable->remove_from_registry = ags_ipatch_sample_remove_from_registry;

  connectable->list_resource = ags_ipatch_sample_list_resource;
  connectable->xml_compose = ags_ipatch_sample_xml_compose;
  connectable->xml_parse = ags_ipatch_sample_xml_parse;

  connectable->is_connected = ags_ipatch_sample_is_connected;
  
  connectable->connect = ags_ipatch_sample_connect;
  connectable->disconnect = ags_ipatch_sample_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_ipatch_sample_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
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

  ipatch_sample->flags = 0;
  ipatch_sample->connectable_flags = 0;

  /* add audio file mutex */
  g_rec_mutex_init(&(ipatch_sample->obj_mutex));

  /* uuid */
  ipatch_sample->uuid = ags_uuid_alloc();
  ags_uuid_generate(ipatch_sample->uuid);

  config = ags_config_get_instance();

  ipatch_sample->audio_channels = 1;
  ipatch_sample->audio_channel_written = (gint64 *) malloc(ipatch_sample->audio_channels * sizeof(gint64));

  ipatch_sample->audio_channel_written[0] = -1;

  ipatch_sample->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  ipatch_sample->format = AGS_SOUNDCARD_DOUBLE;

  ipatch_sample->offset = 0;
  ipatch_sample->buffer_offset = 0;

  ipatch_sample->full_buffer = NULL;
  ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->audio_channels * ipatch_sample->buffer_size,
					   ipatch_sample->format);

  ipatch_sample->pointer = NULL;
  ipatch_sample->current = NULL;
  ipatch_sample->length = 0;

  ipatch_sample->sample = NULL;
}

void
ags_ipatch_sample_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsIpatchSample *ipatch_sample;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(gobject);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  switch(prop_id){
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(ipatch_sample_mutex);

      if(buffer_size == ipatch_sample->buffer_size){
	g_rec_mutex_unlock(ipatch_sample_mutex);

	return;	
      }
      
      ags_stream_free(ipatch_sample->buffer);

      ipatch_sample->buffer_size = buffer_size;
      ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->buffer_size,
					       ipatch_sample->format);

      g_rec_mutex_unlock(ipatch_sample_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(ipatch_sample_mutex);

      if(format == ipatch_sample->format){
	g_rec_mutex_unlock(ipatch_sample_mutex);

	return;	
      }

      ags_stream_free(ipatch_sample->buffer);

      ipatch_sample->format = format;
      ipatch_sample->buffer = ags_stream_alloc(ipatch_sample->buffer_size,
					       ipatch_sample->format);

      g_rec_mutex_unlock(ipatch_sample_mutex);
    }
    break;
  case PROP_SAMPLE:
    {
      IpatchContainer *sample;

      sample = g_value_get_object(value);

      g_rec_mutex_lock(ipatch_sample_mutex);

      if(ipatch_sample->sample == sample){
	g_rec_mutex_unlock(ipatch_sample_mutex);
      
	return;
      }

      if(ipatch_sample->sample != NULL){
	g_object_unref(ipatch_sample->sample);
      }

      if(sample != NULL){
	g_object_ref(sample);
      }
      
      ipatch_sample->sample = sample;

      g_rec_mutex_unlock(ipatch_sample_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_ipatch_sample_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsIpatchSample *ipatch_sample;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = (AgsIpatchSample *) gobject;

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);
  
  switch(prop_id){
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(ipatch_sample_mutex);

      g_value_set_uint(value, ipatch_sample->buffer_size);

      g_rec_mutex_unlock(ipatch_sample_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(ipatch_sample_mutex);

      g_value_set_uint(value, ipatch_sample->format);

      g_rec_mutex_unlock(ipatch_sample_mutex);
    }
    break;
  case PROP_SAMPLE:
    {
      g_rec_mutex_lock(ipatch_sample_mutex);

      g_value_set_object(value, ipatch_sample->sample);

      g_rec_mutex_unlock(ipatch_sample_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
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

    ipatch_sample->sample = NULL;
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_ipatch_sample_parent_class)->finalize(gobject);
}

AgsUUID*
ags_ipatch_sample_get_uuid(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;
  
  AgsUUID *ptr;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  /* get UUID */
  g_rec_mutex_lock(ipatch_sample_mutex);

  ptr = ipatch_sample->uuid;

  g_rec_mutex_unlock(ipatch_sample_mutex);
  
  return(ptr);
}

gboolean
ags_ipatch_sample_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_ipatch_sample_is_ready(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;
  
  gboolean is_ready;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  /* check is ready */
  g_rec_mutex_lock(ipatch_sample_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (ipatch_sample->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(ipatch_sample_mutex);
  
  return(is_ready);
}

void
ags_ipatch_sample_add_to_registry(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  GRecMutex *ipatch_sample_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  g_rec_mutex_lock(ipatch_sample_mutex);

  ipatch_sample->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(ipatch_sample_mutex);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) ipatch_sample);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_ipatch_sample_remove_from_registry(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;

  GRecMutex *ipatch_sample_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  g_rec_mutex_lock(ipatch_sample_mutex);

  ipatch_sample->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(ipatch_sample_mutex);

  //TODO:JK: implement me
}

xmlNode*
ags_ipatch_sample_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_ipatch_sample_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_ipatch_sample_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_ipatch_sample_is_connected(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;
  
  gboolean is_connected;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  /* check is connected */
  g_rec_mutex_lock(ipatch_sample_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (ipatch_sample->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(ipatch_sample_mutex);
  
  return(is_connected);
}

void
ags_ipatch_sample_connect(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;

  GRecMutex *ipatch_sample_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  g_rec_mutex_lock(ipatch_sample_mutex);

  ipatch_sample->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(ipatch_sample_mutex);
}

void
ags_ipatch_sample_disconnect(AgsConnectable *connectable)
{
  AgsIpatchSample *ipatch_sample;

  GRecMutex *ipatch_sample_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ipatch_sample = AGS_IPATCH_SAMPLE(connectable);

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  g_rec_mutex_lock(ipatch_sample_mutex);

  ipatch_sample->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(ipatch_sample_mutex);
}

gboolean
ags_ipatch_sample_info(AgsSoundResource *sound_resource,
		       guint *frame_count,
		       guint *loop_start, guint *loop_end)
{
  AgsIpatchSample *ipatch_sample;

  guint sample_frame_count;
  guint sample_loop_start, sample_loop_end;
  
  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  sample_frame_count = 0;

  sample_loop_start = 0;
  sample_loop_end = 0;
  
  g_object_get(ipatch_sample->sample,
	       "sample-size", &sample_frame_count,
	       "loop-start", &sample_loop_start,
	       "loop-end", &sample_loop_end,
	       NULL);

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
  
  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  if(channels > IPATCH_SAMPLE_MAX_CHANNELS){
    g_critical("max channels exceeded");

    return;
  }
  
  g_rec_mutex_lock(ipatch_sample_mutex);

  //NOTE:JK: this won't work
#if 0
  g_object_get(ipatch_sample->sample,
	       "sample-format", &sample_format,
	       NULL);
  sample_format &= (~IPATCH_SAMPLE_CHANNEL_MASK);
  sample_format |= (IPATCH_SAMPLE_CHANNEL_MASK & ((channels - 1) << IPATCH_SAMPLE_CHANNEL_SHIFT));
#endif
  
  g_object_set(ipatch_sample->sample,
	       "sample-rate", samplerate,
//	       "sample-format", sample_format,
	       NULL);
  
  g_rec_mutex_unlock(ipatch_sample_mutex);

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

  guint current_buffer_size;
  guint current_format;

  gint sample_format;
  guint sample_channels;
  guint sample_samplerate;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  current_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  current_format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(ipatch_sample,
	       "buffer-size", &current_buffer_size,
	       "format", &current_format,
	       NULL);
  
  sample_format = 0;
  sample_samplerate = 0;
  
  g_rec_mutex_lock(ipatch_sample_mutex);

  g_object_get(ipatch_sample->sample,
	       "sample-format", &sample_format,
	       "sample-rate", &sample_samplerate,
	       NULL);

  g_rec_mutex_unlock(ipatch_sample_mutex);

  if(channels != NULL){
    channels[0] = IPATCH_SAMPLE_FORMAT_GET_CHANNEL_COUNT(sample_format);
  }
  
  if(samplerate != NULL){
    samplerate[0] = sample_samplerate;
  }

  if(buffer_size != NULL){
    buffer_size[0] = current_buffer_size;
  }

  if(format != NULL){
    format[0] = current_format;
  }
}

guint
ags_ipatch_sample_read(AgsSoundResource *sound_resource,
		       void *dbuffer, guint daudio_channels,
		       guint audio_channel,
		       guint frame_count, guint format)
{
  AgsIpatchSample *ipatch_sample;

  guint total_frame_count;
  guint copy_mode;
  
  GError *error;
  
  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  g_rec_mutex_lock(ipatch_sample_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(ipatch_sample->format));

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);
  
  if(ipatch_sample->offset >= total_frame_count){
    g_rec_mutex_unlock(ipatch_sample_mutex);
    
    return(0);
  }

  if(ipatch_sample->offset + frame_count >= total_frame_count){
    frame_count = total_frame_count - ipatch_sample->offset;
  }

  error = NULL;

  switch(ipatch_sample->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ipatch_sample_read_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				   ipatch_sample->offset,
				   frame_count,
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
				   frame_count,
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
				   frame_count,
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
				   frame_count,
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
				   frame_count,
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
				   frame_count,
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

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
  }
  
  ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, 0,
					      ipatch_sample->buffer, 1, 0,
					      frame_count, copy_mode);

  ipatch_sample->offset += frame_count;

  g_rec_mutex_unlock(ipatch_sample_mutex);

  return(frame_count);
}

void
ags_ipatch_sample_write(AgsSoundResource *sound_resource,
			void *sbuffer, guint saudio_channels,
			guint audio_channel,
			guint frame_count, guint format)
{
  AgsIpatchSample *ipatch_sample;

  guint copy_mode;

  GError *error;

  GRecMutex *ipatch_sample_mutex;

  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  g_rec_mutex_lock(ipatch_sample_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(ipatch_sample->format),
						  ags_audio_buffer_util_format_from_soundcard(format));

  ags_audio_buffer_util_copy_buffer_to_buffer(ipatch_sample->buffer, 1, audio_channel,
					      sbuffer, saudio_channels, 0,
					      frame_count, copy_mode);

  error = NULL;
  
  switch(ipatch_sample->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      ipatch_sample_write_transform(IPATCH_SAMPLE(ipatch_sample->sample),
				    ipatch_sample->offset,
				    frame_count,
				    ipatch_sample->buffer,
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
				    ipatch_sample->buffer,
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
				    ipatch_sample->buffer,
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
				    ipatch_sample->buffer,
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
				    ipatch_sample->buffer,
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
				    ipatch_sample->buffer,
				    IPATCH_SAMPLE_DOUBLE | IPATCH_SAMPLE_MONO,
				    IPATCH_SAMPLE_MAP_CHANNEL(audio_channel, 0), 
				    &error);
    }
    break;
  }

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
  }
  
  ipatch_sample->offset += frame_count;

  g_rec_mutex_unlock(ipatch_sample_mutex);
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

  GRecMutex *ipatch_sample_mutex;
  
  ipatch_sample = AGS_IPATCH_SAMPLE(sound_resource);

  /* get ipatch sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);

  g_rec_mutex_lock(ipatch_sample_mutex);

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

  g_rec_mutex_unlock(ipatch_sample_mutex);
}

/**
 * ags_ipatch_sample_test_flags:
 * @ipatch_sample: the #AgsIpatchSample
 * @flags: the flags
 *
 * Test @flags to be set on @ipatch_sample.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_ipatch_sample_test_flags(AgsIpatchSample *ipatch_sample, guint flags)
{
  gboolean retval;  
  
  GRecMutex *ipatch_sample_mutex;

  if(!AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return(FALSE);
  }

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  /* test */
  g_rec_mutex_lock(ipatch_sample_mutex);

  retval = (flags & (ipatch_sample->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(ipatch_sample_mutex);

  return(retval);
}

/**
 * ags_ipatch_sample_set_flags:
 * @ipatch_sample: the #AgsIpatchSample
 * @flags: see #AgsIpatchSampleFlags-enum
 *
 * Enable a feature of @ipatch_sample.
 *
 * Since: 3.0.0
 */
void
ags_ipatch_sample_set_flags(AgsIpatchSample *ipatch_sample, guint flags)
{
  GRecMutex *ipatch_sample_mutex;

  if(!AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(ipatch_sample_mutex);

  ipatch_sample->flags |= flags;
  
  g_rec_mutex_unlock(ipatch_sample_mutex);
}
    
/**
 * ags_ipatch_sample_unset_flags:
 * @ipatch_sample: the #AgsIpatchSample
 * @flags: see #AgsIpatchSampleFlags-enum
 *
 * Disable a feature of @ipatch_sample.
 *
 * Since: 3.0.0
 */
void
ags_ipatch_sample_unset_flags(AgsIpatchSample *ipatch_sample, guint flags)
{  
  GRecMutex *ipatch_sample_mutex;

  if(!AGS_IS_IPATCH_SAMPLE(ipatch_sample)){
    return;
  }

  /* get ipatch_sample mutex */
  ipatch_sample_mutex = AGS_IPATCH_SAMPLE_GET_OBJ_MUTEX(ipatch_sample);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(ipatch_sample_mutex);

  ipatch_sample->flags &= (~flags);
  
  g_rec_mutex_unlock(ipatch_sample_mutex);
}

/**
 * ags_ipatch_sample_new:
 *
 * Creates a new instance of #AgsIpatchSample.
 *
 * Returns: the new #AgsIpatchSample.
 *
 * Since: 3.0.0
 */
AgsIpatchSample*
ags_ipatch_sample_new()
{
  AgsIpatchSample *ipatch_sample;

  ipatch_sample = (AgsIpatchSample *) g_object_new(AGS_TYPE_IPATCH_SAMPLE,
						   NULL);

  return(ipatch_sample);
}
