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

#include <ags/audio/file/ags_gstreamer_file.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_gstreamer_file_audio_src.h>
#include <ags/audio/file/ags_gstreamer_file_audio_sink.h>

#include <string.h>

#include <ags/i18n.h>

void ags_gstreamer_file_class_init(AgsGstreamerFileClass *gstreamer_file);
void ags_gstreamer_file_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gstreamer_file_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_gstreamer_file_init(AgsGstreamerFile *gstreamer_file);
void ags_gstreamer_file_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_gstreamer_file_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_gstreamer_file_dispose(GObject *gobject);
void ags_gstreamer_file_finalize(GObject *gobject);

AgsUUID* ags_gstreamer_file_get_uuid(AgsConnectable *connectable);
gboolean ags_gstreamer_file_has_resource(AgsConnectable *connectable);
gboolean ags_gstreamer_file_is_ready(AgsConnectable *connectable);
void ags_gstreamer_file_add_to_registry(AgsConnectable *connectable);
void ags_gstreamer_file_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_gstreamer_file_list_resource(AgsConnectable *connectable);
xmlNode* ags_gstreamer_file_xml_compose(AgsConnectable *connectable);
void ags_gstreamer_file_xml_parse(AgsConnectable *connectable,
				  xmlNode *node);
gboolean ags_gstreamer_file_is_connected(AgsConnectable *connectable);
void ags_gstreamer_file_connect(AgsConnectable *connectable);
void ags_gstreamer_file_disconnect(AgsConnectable *connectable);

gboolean ags_gstreamer_file_open(AgsSoundResource *sound_resource,
				 gchar *filename);
gboolean ags_gstreamer_file_rw_open(AgsSoundResource *sound_resource,
				    gchar *filename,
				    guint audio_channels, guint samplerate,
				    gboolean create);
gboolean ags_gstreamer_file_info(AgsSoundResource *sound_resource,
				 guint *frame_count,
				 guint *loop_start, guint *loop_end);
void ags_gstreamer_file_set_presets(AgsSoundResource *sound_resource,
				    guint channels,
				    guint samplerate,
				    guint buffer_size,
				    guint format);
void ags_gstreamer_file_get_presets(AgsSoundResource *sound_resource,
				    guint *channels,
				    guint *samplerate,
				    guint *buffer_size,
				    guint *format);
guint ags_gstreamer_file_read(AgsSoundResource *sound_resource,
			      void *dbuffer, guint daudio_channels,
			      guint audio_channel,
			      guint frame_count, guint format);
void ags_gstreamer_file_write(AgsSoundResource *sound_resource,
			      void *sbuffer, guint saudio_channels,
			      guint audio_channel,
			      guint frame_count, guint format);
void ags_gstreamer_file_flush(AgsSoundResource *sound_resource);
void ags_gstreamer_file_seek(AgsSoundResource *sound_resource,
			     gint64 frame_count, gint whence);
void ags_gstreamer_file_close(AgsSoundResource *sound_resource);

/**
 * SECTION:ags_gstreamer_file
 * @short_description: gstreamer file
 * @title: AgsGstreamerFile
 * @section_id:
 * @include: ags/audio/file/ags_gstreamer_file.h
 *
 * #AgsGstreamerFile is the base object to ineract with libgstreamer-1.0 file support.
 */

enum{
  PROP_0,
  PROP_AUDIO_CHANNELS,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
};

static gpointer ags_gstreamer_file_parent_class = NULL;
static AgsSoundResourceInterface *ags_gstreamer_file_parent_sound_resource_interface;

GType
ags_gstreamer_file_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_file = 0;

    static const GTypeInfo ags_gstreamer_file_info = {
      sizeof (AgsGstreamerFileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_file_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerFile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_file_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_file_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_file_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gstreamer_file = g_type_register_static(G_TYPE_OBJECT,
						     "AgsGstreamerFile",
						     &ags_gstreamer_file_info,
						     0);

    g_type_add_interface_static(ags_type_gstreamer_file,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_gstreamer_file,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_file);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_file_class_init(AgsGstreamerFileClass *gstreamer_file)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_gstreamer_file_parent_class = g_type_class_peek_parent(gstreamer_file);

  gobject = (GObjectClass *) gstreamer_file;

  gobject->set_property = ags_gstreamer_file_set_property;
  gobject->get_property = ags_gstreamer_file_get_property;

  gobject->dispose = ags_gstreamer_file_dispose;
  gobject->finalize = ags_gstreamer_file_finalize;

  /* properties */
  /**
   * AgsGstreamerFile:audio-channels:
   *
   * The audio channels to be used.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_uint("audio-channels",
				 i18n_pspec("using audio channels"),
				 i18n_pspec("The audio channels to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsGstreamerFile:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 3.6.0
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
   * AgsGstreamerFile:format:
   *
   * The format to be used.
   * 
   * Since: 3.6.0
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
}


void
ags_gstreamer_file_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_gstreamer_file_get_uuid;
  connectable->has_resource = ags_gstreamer_file_has_resource;
  connectable->is_ready = ags_gstreamer_file_is_ready;

  connectable->add_to_registry = ags_gstreamer_file_add_to_registry;
  connectable->remove_from_registry = ags_gstreamer_file_remove_from_registry;

  connectable->list_resource = ags_gstreamer_file_list_resource;
  connectable->xml_compose = ags_gstreamer_file_xml_compose;
  connectable->xml_parse = ags_gstreamer_file_xml_parse;

  connectable->is_connected = ags_gstreamer_file_is_connected;
  
  connectable->connect = ags_gstreamer_file_connect;
  connectable->disconnect = ags_gstreamer_file_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_gstreamer_file_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  ags_gstreamer_file_parent_sound_resource_interface = g_type_interface_peek_parent(sound_resource);

  sound_resource->open = ags_gstreamer_file_open;
  sound_resource->rw_open = ags_gstreamer_file_rw_open;

  sound_resource->load = NULL;

  sound_resource->info = ags_gstreamer_file_info;

  sound_resource->set_presets = ags_gstreamer_file_set_presets;
  sound_resource->get_presets = ags_gstreamer_file_get_presets;
  
  sound_resource->read = ags_gstreamer_file_read;

  sound_resource->write = ags_gstreamer_file_write;
  sound_resource->flush = ags_gstreamer_file_flush;
  
  sound_resource->seek = ags_gstreamer_file_seek;

  sound_resource->close = ags_gstreamer_file_close;
}

void
ags_gstreamer_file_init(AgsGstreamerFile *gstreamer_file)
{
  AgsConfig *config;

  gstreamer_file->flags = 0;

  /* add gstreamer file mutex */
  g_rec_mutex_init(&(gstreamer_file->obj_mutex));  

  /* uuid */
  gstreamer_file->uuid = ags_uuid_alloc();
  ags_uuid_generate(gstreamer_file->uuid);

  config = ags_config_get_instance();

  gstreamer_file->audio_channels = 1;
  gstreamer_file->audio_channel_written = (gint64 *) malloc(gstreamer_file->audio_channels * sizeof(gint64));

  gstreamer_file->audio_channel_written[0] = -1;
  
  gstreamer_file->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  gstreamer_file->format = AGS_SOUNDCARD_SIGNED_16_BIT;

  gstreamer_file->offset = 0;
  gstreamer_file->buffer_offset = 0;

  gstreamer_file->full_buffer = NULL;
  gstreamer_file->buffer = ags_stream_alloc(gstreamer_file->audio_channels * gstreamer_file->buffer_size,
					    gstreamer_file->format);

  gstreamer_file->pointer = NULL;
  gstreamer_file->current = NULL;
  gstreamer_file->length = 0;

  gstreamer_file->read_pipeline = NULL;
  gstreamer_file->read_pipeline_running = FALSE;

  gstreamer_file->write_pipeline = NULL;
  gstreamer_file->write_pipeline_running = FALSE;
  
  gstreamer_file->file_source = NULL;
  gstreamer_file->file_decoder = NULL;
  gstreamer_file->data_sink_convert = NULL;
  gstreamer_file->data_sink = NULL;
  
  gstreamer_file->data_source = NULL;
  gstreamer_file->data_source_convert = NULL;
  gstreamer_file->file_encoder = NULL;
  gstreamer_file->file_sink = NULL;
}

void
ags_gstreamer_file_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsGstreamerFile *gstreamer_file;

  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(gobject);

  /* get audio file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
  {
    guint audio_channels;
    guint i;
      
    audio_channels = g_value_get_uint(value);

    g_rec_mutex_lock(gstreamer_file_mutex);

    if(audio_channels == gstreamer_file->audio_channels){
      g_rec_mutex_unlock(gstreamer_file_mutex);

      return;	
    }
      
    ags_stream_free(gstreamer_file->buffer);

    if(audio_channels > 0){
      gstreamer_file->audio_channel_written = (gint64 *) realloc(gstreamer_file->audio_channel_written,
								 audio_channels * sizeof(gint64));
	
      for(i = gstreamer_file->audio_channels; i < audio_channels; i++){
	gstreamer_file->audio_channel_written[i] = -1;
      }
    }else{
      free(gstreamer_file->audio_channel_written);
    }

    gstreamer_file->audio_channels = audio_channels;
      
    gstreamer_file->buffer = ags_stream_alloc(gstreamer_file->audio_channels * gstreamer_file->buffer_size,
					      gstreamer_file->format);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(gstreamer_file_mutex);

    if(buffer_size == gstreamer_file->buffer_size){
      g_rec_mutex_unlock(gstreamer_file_mutex);
	
      return;	
    }
      
    ags_stream_free(gstreamer_file->buffer);

    gstreamer_file->buffer_size = buffer_size;
    gstreamer_file->buffer = ags_stream_alloc(gstreamer_file->audio_channels * gstreamer_file->buffer_size,
					      gstreamer_file->format);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(gstreamer_file_mutex);

    if(format == gstreamer_file->format){
      g_rec_mutex_unlock(gstreamer_file_mutex);
      
      return;	
    }

    ags_stream_free(gstreamer_file->buffer);

    gstreamer_file->format = format;
    gstreamer_file->buffer = ags_stream_alloc(gstreamer_file->audio_channels * gstreamer_file->buffer_size,
					      gstreamer_file->format);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_gstreamer_file_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsGstreamerFile *gstreamer_file;

  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(gobject);

  /* get audio file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);
  
  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
  {
    g_rec_mutex_lock(gstreamer_file_mutex);

    g_value_set_uint(value, gstreamer_file->audio_channels);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(gstreamer_file_mutex);

    g_value_set_uint(value, gstreamer_file->buffer_size);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(gstreamer_file_mutex);

    g_value_set_uint(value, gstreamer_file->format);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_gstreamer_file_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_parent_class)->dispose(gobject);
}

void
ags_gstreamer_file_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_file_parent_class)->finalize(gobject);
}

AgsUUID*
ags_gstreamer_file_get_uuid(AgsConnectable *connectable)
{
  AgsGstreamerFile *gstreamer_file;
  
  AgsUUID *ptr;

  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(connectable);

  /* get audio file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* get UUID */
  g_rec_mutex_lock(gstreamer_file_mutex);

  ptr = gstreamer_file->uuid;

  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  return(ptr);
}

gboolean
ags_gstreamer_file_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_gstreamer_file_is_ready(AgsConnectable *connectable)
{
  AgsGstreamerFile *gstreamer_file;
  
  gboolean is_ready;

  gstreamer_file = AGS_GSTREAMER_FILE(connectable);

  /* check is ready */
  is_ready = ags_gstreamer_file_test_flags(gstreamer_file, AGS_GSTREAMER_FILE_ADDED_TO_REGISTRY);

  return(is_ready);
}

void
ags_gstreamer_file_add_to_registry(AgsConnectable *connectable)
{
  AgsGstreamerFile *gstreamer_file;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  gstreamer_file = AGS_GSTREAMER_FILE(connectable);

  ags_gstreamer_file_set_flags(gstreamer_file, AGS_GSTREAMER_FILE_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) gstreamer_file);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_gstreamer_file_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_gstreamer_file_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_gstreamer_file_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_gstreamer_file_xml_parse(AgsConnectable *connectable,
			     xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_gstreamer_file_is_connected(AgsConnectable *connectable)
{
  AgsGstreamerFile *gstreamer_file;
  
  gboolean is_connected;

  gstreamer_file = AGS_GSTREAMER_FILE(connectable);

  /* check is connected */
  is_connected = ags_gstreamer_file_test_flags(gstreamer_file, AGS_GSTREAMER_FILE_CONNECTED);

  return(is_connected);
}

void
ags_gstreamer_file_connect(AgsConnectable *connectable)
{
  AgsGstreamerFile *gstreamer_file;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_file = AGS_GSTREAMER_FILE(connectable);
  
  ags_gstreamer_file_set_flags(gstreamer_file, AGS_GSTREAMER_FILE_CONNECTED);
}

void
ags_gstreamer_file_disconnect(AgsConnectable *connectable)
{
  AgsGstreamerFile *gstreamer_file;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_file = AGS_GSTREAMER_FILE(connectable);

  ags_gstreamer_file_unset_flags(gstreamer_file, AGS_GSTREAMER_FILE_CONNECTED);
}

/**
 * ags_gstreamer_file_test_flags:
 * @gstreamer_file: the #AgsGstreamerFile
 * @flags: the flags
 *
 * Test @flags to be set on @gstreamer_file.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_file_test_flags(AgsGstreamerFile *gstreamer_file, guint flags)
{
  gboolean retval;  
  
  GRecMutex *gstreamer_file_mutex;

  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(FALSE);
  }

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* test */
  g_rec_mutex_lock(gstreamer_file_mutex);

  retval = (flags & (gstreamer_file->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  return(retval);
}

/**
 * ags_gstreamer_file_set_flags:
 * @gstreamer_file: the #AgsGstreamerFile
 * @flags: see #AgsGstreamerFileFlags-enum
 *
 * Enable a feature of @gstreamer_file.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_file_set_flags(AgsGstreamerFile *gstreamer_file, guint flags)
{
  GRecMutex *gstreamer_file_mutex;

  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return;
  }

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->flags |= flags;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
}
    
/**
 * ags_gstreamer_file_unset_flags:
 * @gstreamer_file: the #AgsGstreamerFile
 * @flags: see #AgsGstreamerFileFlags-enum
 *
 * Disable a feature of @gstreamer_file.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_file_unset_flags(AgsGstreamerFile *gstreamer_file, guint flags)
{  
  GRecMutex *gstreamer_file_mutex;

  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return;
  }

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->flags &= (~flags);
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
}

gboolean
ags_gstreamer_file_open(AgsSoundResource *sound_resource,
			gchar *filename)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *read_pipeline;
  GstElement *file_source;
  GstElement *file_decoder;
  GstElement *data_sink_convert;
  GstElement *data_sink;

  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  if(gstreamer_file->read_pipeline != NULL){
    g_rec_mutex_unlock(gstreamer_file_mutex);

    return(FALSE);
  }
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* read file */
  read_pipeline = gst_pipeline_new("AGS ro-pipeline");
  
  file_source = gst_element_factory_make("filesrc", "AGS file source");

  g_object_set(file_source,
	       "location", filename,
	       NULL);
  
  file_decoder = gst_element_factory_make("decodebin", "AGS file decoder");

  data_sink_convert = gst_element_factory_make("audioconvert", "AGS data sink convert");

  data_sink = (GstElement *) ags_gstreamer_file_audio_sink_new();
  gst_element_set_name(data_sink, "AGS data sink");

  gst_bin_add_many(GST_BIN(read_pipeline),
		   file_source,
		   file_decoder,
		   data_sink_convert,
		   data_sink,
		   NULL);

  gst_element_link(file_source, file_decoder);
  gst_element_link(file_decoder, data_sink_convert);
  gst_element_link(data_sink_convert, data_sink);

  /* apply */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->read_pipeline = read_pipeline;

  gstreamer_file->file_source = file_source;
  gstreamer_file->file_decoder = file_decoder;
  gstreamer_file->data_sink_convert = data_sink_convert;
  gstreamer_file->data_sink = data_sink;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  return(TRUE);
}

gboolean
ags_gstreamer_file_rw_open(AgsSoundResource *sound_resource,
			   gchar *filename,
			   guint audio_channels, guint samplerate,
			   gboolean create)
{
  AgsGstreamerFile *gstreamer_file;
  
  GstElement *read_pipeline;
  GstElement *write_pipeline;
  GstElement *data_source_convert;
  GstElement *data_source;
  GstElement *file_encoder;
  GstElement *file_sink;
  GstElement *file_source;
  GstElement *file_decoder;
  GstElement *data_sink_convert;
  GstElement *data_sink;

  gboolean success;
    
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  if(gstreamer_file->read_pipeline != NULL ||
     gstreamer_file->write_pipeline != NULL){  
    g_rec_mutex_unlock(gstreamer_file_mutex);

    return(FALSE);
  }

  if(!create &&
     !g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
    g_rec_mutex_unlock(gstreamer_file_mutex);

    return(FALSE);
  }  
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  success = TRUE;
  
  read_pipeline = gst_pipeline_new("AGS rw-pipeline (read)");
  write_pipeline = gst_pipeline_new("AGS rw-pipeline (write)");

  /* read file */
  file_source = gst_element_factory_make("filesrc", "AGS file source");
  g_object_set(file_source,
	       "location", filename,
	       NULL);
  
  file_decoder = gst_element_factory_make("decodebin", "AGS file decoder");

  data_sink_convert = gst_element_factory_make("audioconvert", "AGS data sink convert");

  data_sink = (GstElement *) ags_gstreamer_file_audio_sink_new();
  gst_element_set_name(data_sink, "AGS data sink");

  gst_bin_add_many(GST_BIN(read_pipeline),
		   file_source,
		   file_decoder,
		   data_sink_convert,
		   data_sink,
		   NULL);

  gst_element_link(file_source, file_decoder);
  gst_element_link(file_decoder, data_sink_convert);
  gst_element_link(data_sink_convert, data_sink);

  /* write file */
  data_source = (GstElement *) ags_gstreamer_file_audio_src_new();
  gst_element_set_name(data_source, "AGS data source");
  
  data_source_convert = gst_element_factory_make("audioconvert", "AGS data source convert");

  file_encoder = gst_element_factory_make("encodebin", "AGS file encoder");

  file_sink = gst_element_factory_make("filesink", "AGS file sink");
  g_object_set(file_sink,
	       "location", filename,
	       NULL);

  gst_bin_add_many(GST_BIN(write_pipeline),
		   data_source,
		   data_source_convert,
		   file_encoder,
		   file_sink,
		   NULL);

  gst_element_link(data_source, data_source_convert);
  gst_element_link(data_source_convert, file_encoder);
  gst_element_link(file_encoder, file_sink);

  /* apply */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->read_pipeline = read_pipeline;
  gstreamer_file->write_pipeline = write_pipeline;

  gstreamer_file->file_source = file_source;
  gstreamer_file->file_decoder = file_decoder;
  gstreamer_file->data_sink_convert = data_sink_convert;
  gstreamer_file->data_sink = data_sink;

  gstreamer_file->data_source = data_source;
  gstreamer_file->data_source_convert = data_source_convert;
  gstreamer_file->file_encoder = file_encoder;
  gstreamer_file->file_sink = file_sink;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  return(success);
}

gboolean
ags_gstreamer_file_info(AgsSoundResource *sound_resource,
			guint *frame_count,
			guint *loop_start, guint *loop_end)
{
  AgsGstreamerFile *gstreamer_file;
  GstElement *pipeline;
  
  gint frames;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  if(loop_start != NULL){
    loop_start[0] = 0;
  }

  if(loop_end != NULL){
    loop_end[0] = 0;
  }

  /* get pipeline */
  g_rec_mutex_lock(gstreamer_file_mutex);

  pipeline = gstreamer_file->read_pipeline;

  if(pipeline != NULL){
    g_object_ref(pipeline);
  }
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* query */
  frames = 0;
  
  gst_element_query_duration(pipeline, GST_FORMAT_DEFAULT, &frames);

  if(pipeline != NULL){
    g_object_unref(pipeline);
  }

  if(frame_count != NULL){
    frame_count[0] = frames;
  }
  
  return(TRUE);
}

void
ags_gstreamer_file_set_presets(AgsSoundResource *sound_resource,
			       guint channels,
			       guint samplerate,
			       guint buffer_size,
			       guint format)
{
  AgsGstreamerFile *gstreamer_file;
  GstElement *read_pipeline, *write_pipeline;
  GstPad *src_pad, *sink_pad;
  GstCaps *caps;
  GstStructure *s;

  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* get pipeline */
  g_rec_mutex_lock(gstreamer_file_mutex);

  read_pipeline = gstreamer_file->read_pipeline;

  if(read_pipeline != NULL){
    g_object_ref(read_pipeline);
  }

  write_pipeline = gstreamer_file->write_pipeline;

  if(write_pipeline != NULL){
    g_object_ref(write_pipeline);
  }
  
  src_pad = GST_BASE_SRC_PAD(read_pipeline);
  sink_pad = GST_BASE_SINK_PAD(write_pipeline);
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  g_object_set(gstreamer_file,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);

  /* get caps */
  caps = gst_pad_get_allowed_caps(src_pad);

  s = gst_caps_get_structure(caps, 0);
  
  gst_structure_set(s,
		    "channels", channels,
		    "rate", samplerate,
		    NULL);

  if(caps != NULL){
    gst_caps_unref(caps);
  }
  
  caps = gst_pad_get_allowed_caps(sink_pad);

  s = gst_caps_get_structure(caps, 0);
  
  gst_structure_set(s,
		    "channels", channels,
		    "rate", samplerate,
		    NULL);

  if(caps != NULL){
    gst_caps_unref(caps);
  }
  
  if(read_pipeline != NULL){
    g_object_unref(read_pipeline);
  }

  if(write_pipeline != NULL){
    g_object_unref(write_pipeline);
  }
}

void
ags_gstreamer_file_get_presets(AgsSoundResource *sound_resource,
			       guint *channels,
			       guint *samplerate,
			       guint *buffer_size,
			       guint *format)
{
  AgsGstreamerFile *gstreamer_file;   
  GstElement *pipeline;
  GstPad *src_pad;
  GstCaps *caps;
  GstStructure *s;

  gint current_samplerate;
  gint current_channels;
  guint current_buffer_size;
  guint current_format;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* get pipeline */
  g_rec_mutex_lock(gstreamer_file_mutex);

  pipeline = gstreamer_file->read_pipeline;

  if(pipeline != NULL){
    g_object_ref(pipeline);
  }
  
  src_pad = GST_BASE_SRC_PAD(pipeline);

  current_buffer_size = gstreamer_file->buffer_size;
  current_format = gstreamer_file->format;

  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* get caps */
  caps = gst_pad_get_allowed_caps(src_pad);

  s = gst_caps_get_structure(caps, 0);
  
  current_channels = 0;
  current_samplerate = 0;

  gst_structure_get(s,
		    "channels", &current_channels,
		    "rate", &current_samplerate,
		    NULL);  

  if(caps != NULL){
    gst_caps_unref(caps);
  }

  if(pipeline != NULL){
    g_object_unref(pipeline);
  }

  if(channels != NULL){
    channels[0] = current_channels;
  }

  if(samplerate != NULL){
    samplerate[0] = current_samplerate;
  }

  if(buffer_size != NULL){
    buffer_size[0] = current_buffer_size;
  }

  if(format != NULL){
    format[0] = current_format;
  }
}

guint
ags_gstreamer_file_read(AgsSoundResource *sound_resource,
			void *dbuffer, guint daudio_channels,
			guint audio_channel,
			guint frame_count, guint format)
{
  AgsGstreamerFile *gstreamer_file;
  GstElement *pipeline;

  gboolean is_running;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);
  
  g_rec_mutex_lock(gstreamer_file_mutex);

  pipeline = gstreamer_file->read_pipeline;

  if(pipeline != NULL){
    g_object_ref(pipeline);
  }

  is_running = gstreamer_file->read_pipeline_running;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  if(!is_running){
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
  }
  
  //TODO:JK: implement me

  if(pipeline != NULL){
    g_object_unref(pipeline);
  }
  
  return(frame_count);
}

void
ags_gstreamer_file_write(AgsSoundResource *sound_resource,
			 void *sbuffer, guint saudio_channels,
			 guint audio_channel,
			 guint frame_count, guint format)
{
  AgsGstreamerFile *gstreamer_file;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  //TODO:JK: implement me
}

void
ags_gstreamer_file_flush(AgsSoundResource *sound_resource)
{
  AgsGstreamerFile *gstreamer_file;
  
  GRecMutex *gstreamer_file_mutex;
   
  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  //TODO:JK: implement me
}

void
ags_gstreamer_file_seek(AgsSoundResource *sound_resource,
			gint64 frame_count, gint whence)
{
  AgsGstreamerFile *gstreamer_file;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  //TODO:JK: implement me
}

void
ags_gstreamer_file_close(AgsSoundResource *sound_resource)
{
  AgsGstreamerFile *gstreamer_file;
   
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  //TODO:JK: implement me
}

/**
 * ags_gstreamer_file_new:
 *
 * Creates a new instance of #AgsGstreamerFile.
 *
 * Returns: the new #AgsGstreamerFile.
 *
 * Since: 3.6.0
 */
AgsGstreamerFile*
ags_gstreamer_file_new()
{
  AgsGstreamerFile *gstreamer_file;

  gstreamer_file = (AgsGstreamerFile *) g_object_new(AGS_TYPE_GSTREAMER_FILE,
						     NULL);

  return(gstreamer_file);
}
