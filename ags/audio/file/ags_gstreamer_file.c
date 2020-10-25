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

#include <gst/gst.h>
#include <gst/audio/gstaudiosink.h>
#include <gst/audio/gstaudiosrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>

#include <math.h>
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

gboolean ags_gstreamer_file_seek_data(GstElement *source, guint64 position, AgsGstreamerFile *gstreamer_file);
void* ags_gstreamer_file_rw_thread_run(void *ptr);

void ags_gstreamer_file_start_feed(GstElement *source, guint size, AgsGstreamerFile *gstreamer_file);
void ags_gstreamer_file_stop_feed(GstElement *source, AgsGstreamerFile *gstreamer_file);

void ags_gstreamer_file_error_callback(GstBus *bus, GstMessage *msg, AgsGstreamerFile *gstreamer_file);

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
  PROP_FILENAME,
  PROP_AUDIO_CHANNELS,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
};

/* playbin flags */
typedef enum{
  GST_PLAY_FLAG_VIDEO         = (1 << 0), /* We want video output */
  GST_PLAY_FLAG_AUDIO         = (1 << 1), /* We want audio output */
  GST_PLAY_FLAG_TEXT          = (1 << 2)  /* We want subtitle output */
}GstPlayFlags;

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
   * AgsGstreamerFile:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of audio file"),
				   i18n_pspec("The filename of audio file"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

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
   * AgsGstreamerFile:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("using samplerate"),
				 i18n_pspec("The samplerate to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
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

  gstreamer_file->audio_channels = AGS_GSTREAMER_FILE_DEFAULT_AUDIO_CHANNELS;
  gstreamer_file->audio_channel_written = (gint64 *) malloc(gstreamer_file->audio_channels * sizeof(gint64));

  gstreamer_file->audio_channel_written[0] = 0;
  
  gstreamer_file->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  gstreamer_file->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  gstreamer_file->format = AGS_GSTREAMER_FILE_DEFAULT_FORMAT;

  gstreamer_file->offset = 0;
  gstreamer_file->buffer_offset = 0;

  gstreamer_file->filename = NULL;
  
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
  gstreamer_file->write_pipeline_need_data = FALSE;

  gstreamer_file->playbin = NULL;
  gstreamer_file->audio_sink = NULL;
  gstreamer_file->video_sink = NULL;
  gstreamer_file->text_sink = NULL;

  gstreamer_file->rw_audio_app_src = NULL;
  gstreamer_file->rw_video_app_src = NULL;
  gstreamer_file->rw_text_app_src = NULL;
  gstreamer_file->rw_audio_tee = NULL;
  gstreamer_file->rw_video_file_sink_queue = NULL;
  gstreamer_file->rw_audio_file_sink_queue = NULL;
  gstreamer_file->rw_text_file_sink_queue = NULL;
  gstreamer_file->rw_audio_convert = NULL;
  gstreamer_file->rw_audio_resample = NULL;
  gstreamer_file->rw_audio_mixer = NULL;
  gstreamer_file->rw_file_encoder = NULL;
  gstreamer_file->rw_file_sink = NULL;
  gstreamer_file->rw_audio_app_sink_queue = NULL;
  gstreamer_file->rw_audio_app_sink = NULL;

  gstreamer_file->prev_frame_count = 0;
  
  gstreamer_file->last_sample = NULL;

  gstreamer_file->rw_thread = NULL;

  gstreamer_file->current_buffer = NULL;

  gstreamer_file->rw_buffer = NULL;
  gstreamer_file->rw_info = NULL;
  gstreamer_file->rw_current_buffer = NULL;
  gstreamer_file->rw_current_info = NULL;
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
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(gstreamer_file_mutex);

      if(gstreamer_file->filename == filename){
	g_rec_mutex_unlock(gstreamer_file_mutex);

	return;
      }
      
      if(gstreamer_file->filename != NULL){
	g_free(gstreamer_file->filename);
      }

      gstreamer_file->filename = g_strdup(filename);

      g_rec_mutex_unlock(gstreamer_file_mutex);
    }
    break;
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
	gstreamer_file->audio_channel_written[i] = 0;
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
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(gstreamer_file_mutex);

    if(samplerate == gstreamer_file->samplerate){
      g_rec_mutex_unlock(gstreamer_file_mutex);
      
      return;	
    }

    gstreamer_file->samplerate = samplerate;

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
  case PROP_FILENAME:
    {
      g_rec_mutex_lock(gstreamer_file_mutex);

      g_value_set_string(value, gstreamer_file->filename);

      g_rec_mutex_unlock(gstreamer_file_mutex);
    }
    break;
  case PROP_AUDIO_CHANNELS:
  {
    g_rec_mutex_lock(gstreamer_file_mutex);

    g_value_set_uint(value, gstreamer_file->audio_channels);

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(gstreamer_file_mutex);

    g_value_set_uint(value, gstreamer_file->samplerate);

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
  GstElement *playbin;
  GstElement *audio_sink;
  GstElement *video_sink;

  gchar *file_uri;
  gchar *caps;
  
  GstState current_state;
  GstStateChangeReturn state_change_retval;

  guint buffer_size;
  guint audio_channels;
  gint flags;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  if(gstreamer_file->read_pipeline != NULL){
    g_rec_mutex_unlock(gstreamer_file_mutex);

    return(FALSE);
  }

  audio_channels = gstreamer_file->audio_channels;
  
  buffer_size = gstreamer_file->buffer_size;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* read file */
  read_pipeline = gst_pipeline_new("AGS ro-pipeline");

  gst_element_set_state(read_pipeline,
			GST_STATE_NULL);
  
  playbin = gst_element_factory_make("playbin", "AGS playbin");
  
  audio_sink = gst_element_factory_make("appsink", "AGS audio sink");
  
  g_object_set(audio_sink,
	       "sync", FALSE,
	       NULL);
  
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = %s, rate = %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16BE }, layout = (string) { interleaved }, channels = %s, rate = %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#else
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = %s, rate = %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#endif

  g_object_set(audio_sink,
	       "caps", gst_caps_from_string(caps),
	       NULL);

  g_free(caps);

  video_sink = gst_element_factory_make("fakesink", "AGS video sink");
  g_object_set(video_sink,
	       "sync", FALSE,
	       NULL);

  file_uri = NULL;
  
  if(filename != NULL){
    if(g_path_is_absolute(filename)){
      file_uri = g_strdup_printf("file://%s",
				 filename);
    }else{
      gchar *current_dir;

      current_dir = g_get_current_dir();

      file_uri = g_strdup_printf("file://%s/%s",
				 current_dir,
				 filename);

      g_free(current_dir);
    }
  }
  
  g_object_set(playbin,
	       "uri", file_uri,
	       "audio-sink", audio_sink,
	       "video-sink", video_sink,
	       NULL);

  g_free(file_uri);

  g_object_get(playbin,
	       "flags", &flags,
	       NULL);

  flags |= GST_PLAY_FLAG_AUDIO;
  flags &= (~GST_PLAY_FLAG_VIDEO);
  flags &= (~GST_PLAY_FLAG_TEXT);
  
  g_object_set(playbin,
	       "flags", flags,
	       NULL);

  gst_bin_add_many(GST_BIN(read_pipeline),
		   playbin,
		   NULL);
  
  /* apply */  
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->read_pipeline = read_pipeline;
  gstreamer_file->read_pipeline_running = TRUE;

  gstreamer_file->playbin = playbin;
  gstreamer_file->audio_sink = audio_sink;
  gstreamer_file->video_sink = video_sink;

  gstreamer_file->filename = g_strdup(filename);
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  state_change_retval = gst_element_set_state(read_pipeline,
					      GST_STATE_PLAYING);

  if(state_change_retval == GST_STATE_CHANGE_FAILURE){
    g_critical("unable to start AGS ro-pipeline");
  }else{
    do{
      gst_element_get_state(GST_ELEMENT(read_pipeline), &current_state, NULL, 4000000);
    }while(current_state != GST_STATE_PLAYING);
  }
  
  return(TRUE);
}

gboolean
ags_gstreamer_file_seek_data(GstElement *source, guint64 position, AgsGstreamerFile *gstreamer_file)
{
  //  ags_sound_resource_seek(AGS_SOUND_RESOURCE(gstreamer_file), position, G_SEEK_SET);

  return(TRUE);
}

void*
ags_gstreamer_file_rw_thread_run(void *ptr)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *rw_audio_app_src;

  gboolean need_data;
  gboolean is_running;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(ptr);

  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  rw_audio_app_src = gstreamer_file->rw_audio_app_src;

  if(rw_audio_app_src != NULL){
    g_object_ref(rw_audio_app_src);
  }
  
  is_running = (gstreamer_file->write_pipeline_running) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  while(is_running){
    /* test write pipeline need data */
    g_rec_mutex_lock(gstreamer_file_mutex);

    need_data = gstreamer_file->write_pipeline_need_data;

    if(rw_audio_app_src == NULL){
      rw_audio_app_src = gstreamer_file->rw_audio_app_src;

      if(rw_audio_app_src != NULL){
	g_object_ref(rw_audio_app_src);
      }
    }
    
    g_rec_mutex_unlock(gstreamer_file_mutex);

    if(need_data &&
       rw_audio_app_src != NULL){
      GstMapInfo *info;

      GList *rw_buffer, *rw_info;

      gpointer data;

      GstFlowReturn flow_retval;

      g_rec_mutex_lock(gstreamer_file_mutex);

      rw_buffer = g_list_last(gstreamer_file->rw_buffer);

      data = (rw_buffer != NULL) ? rw_buffer->data: NULL;

      rw_info = g_list_last(gstreamer_file->rw_info);

      info = (rw_info != NULL) ? rw_info->data: NULL;
      
      g_rec_mutex_unlock(gstreamer_file_mutex);

      if(data != NULL &&
	 info != NULL){	
	g_signal_emit_by_name(rw_audio_app_src,
			      "push-buffer",
			      data,
			      &flow_retval);    

	gst_buffer_unmap(data,
			 info);

	g_rec_mutex_lock(gstreamer_file_mutex);
    
	gstreamer_file->rw_buffer = g_list_remove(gstreamer_file->rw_buffer,
						  data);
	gstreamer_file->rw_info = g_list_remove(gstreamer_file->rw_info,
						info);
	
	g_rec_mutex_unlock(gstreamer_file_mutex);

	gst_buffer_unref(data);
	
	g_free(info);

	if(flow_retval != GST_FLOW_OK){
	  g_warning("push buffer flow not ok");
	}
      }
    }else{
      g_usleep(AGS_GSTREAMER_FILE_DEFAULT_FLUSH_IDLE);
    }
    
    /* test write pipeline running */
    g_rec_mutex_lock(gstreamer_file_mutex);

    is_running = gstreamer_file->write_pipeline_running;

    g_rec_mutex_unlock(gstreamer_file_mutex);
  }

  if(rw_audio_app_src != NULL){
    g_object_unref(rw_audio_app_src);
  }
  
  g_thread_exit(NULL);

  return(NULL);
}

void
ags_gstreamer_file_start_feed(GstElement *source, guint size, AgsGstreamerFile *gstreamer_file)
{
  GRecMutex *gstreamer_file_mutex;
  
  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->write_pipeline_need_data = TRUE;

  g_rec_mutex_unlock(gstreamer_file_mutex);
}

void
ags_gstreamer_file_stop_feed(GstElement *source, AgsGstreamerFile *gstreamer_file)
{
  GRecMutex *gstreamer_file_mutex;
  
  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->write_pipeline_need_data = FALSE;

  g_rec_mutex_unlock(gstreamer_file_mutex);
}

void
ags_gstreamer_file_error_callback(GstBus *bus, GstMessage *msg, AgsGstreamerFile *gstreamer_file)
{
  gchar *debug_info;

  GError *error;

  /* Print error details on the screen */
  error = NULL;
  gst_message_parse_error(msg, &error, &debug_info);
  
  g_message("Error received from element %s: %s", GST_OBJECT_NAME (msg->src), error->message);
  g_message("Debugging information: %s", debug_info ? debug_info : "none");
  
  g_clear_error(&error);
  g_free(debug_info);
}

gboolean
ags_gstreamer_file_rw_open(AgsSoundResource *sound_resource,
			   gchar *filename,
			   guint audio_channels, guint samplerate,
			   gboolean create)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *write_pipeline;

  GstElement *rw_audio_app_src;
  GstElement *rw_audio_file_sink_queue;
  GstElement *rw_audio_convert;
  GstElement *rw_audio_resample;
  GstElement *rw_audio_mixer;
  GstElement *rw_file_encoder;
  GstElement *rw_file_sink;
  GstPad *rw_audio_mixer_src_pad;
  GstPad *rw_file_encoder_audio_pad;
  
  GstBus *bus;

  GThread *rw_thread;
  
  GstEncodingProfile *encoding_profile;
  
  gchar *file_uri;
  gchar *caps;
  
  GstState current_state;
  GstStateChangeReturn state_change_retval;

  gint64 channel_mask;
  guint buffer_size;
  gint flags;

  gboolean success;

  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  success = FALSE;

  g_rec_mutex_lock(gstreamer_file_mutex);

  if(gstreamer_file->write_pipeline != NULL){
    g_rec_mutex_unlock(gstreamer_file_mutex);

    return(FALSE);
  }
  
  buffer_size = gstreamer_file->buffer_size;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  g_object_set(gstreamer_file,
	       "filename", filename,
	       "audio-channels", audio_channels,
	       NULL);
  
  /* read file */
  file_uri = NULL;
  
  if(filename != NULL){
    if(g_path_is_absolute(filename)){
      file_uri = g_strdup_printf("file://%s",
				 filename);
    }else{
      gchar *current_dir;

      current_dir = g_get_current_dir();

      file_uri = g_strdup_printf("file://%s/%s",
				 current_dir,
				 filename);

      g_free(current_dir);
    }
  }

  //TODO:JK: implement me
  
  /* write file */
  write_pipeline = gst_pipeline_new("AGS rw-pipeline (write)");
  
  gst_element_set_state(write_pipeline,
  			GST_STATE_NULL);

  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->write_pipeline_running = TRUE;
  gstreamer_file->write_pipeline_need_data = FALSE;

  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  rw_thread = g_thread_new("Advanced Gtk+ Sequencer - gstreamer file rw thread",
			   ags_gstreamer_file_rw_thread_run,
			   gstreamer_file);

  rw_audio_app_src = gst_element_factory_make("appsrc", "AGS rw audio source");

  rw_audio_file_sink_queue = gst_element_factory_make("queue", "AGS rw audio file sink queue");
  
  rw_audio_convert = gst_element_factory_make("audioconvert", "AGS rw audio convert");
  rw_audio_resample = gst_element_factory_make("audioresample", "AGS rw audio resample");
  rw_audio_mixer = gst_element_factory_make("audiomixer", "AGS rw audio mixer");

  rw_file_encoder = gst_element_factory_make("encodebin", "AGS rw file encoder");
  rw_file_sink = gst_element_factory_make("filesink", "AGS rw file sink");  
    
  if(!write_pipeline ||
     !rw_audio_app_src ||
     !rw_audio_convert || !rw_audio_resample || !rw_audio_mixer ||
     !rw_audio_file_sink_queue ||
     !rw_file_encoder || !rw_file_sink){
    gstreamer_file->write_pipeline_running = FALSE;
    
    g_critical("not all elements of pipeline could be created");
  }

  /* configure audio app src */
  channel_mask = -2;

  switch(audio_channels){
  case 2:
    {
      channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT);
    }
    break;
  case 3:
    {
      channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_LFE1);
    }
    break;
  case 6:
    {
      channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER) |
	(1 << GST_AUDIO_CHANNEL_POSITION_SIDE_LEFT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_SIDE_RIGHT) |
	(1 << GST_AUDIO_CHANNEL_POSITION_LFE1);
    }
    break;
  case 8:
    {
      //TODO:JK: implement me
    }
    break;
  case 10:
    {
      //TODO:JK: implement me
    }
    break;
  }
  
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = (int) %d, channel-mask = (bitmask) %#018lx, rate = (int) %d",
			 audio_channels,
			 channel_mask,
			 samplerate);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16BE }, layout = (string) { interleaved }, channels = (int) %d, channel-mask = (bitmask) %#018lx, rate = (int) %d",
			 audio_channels,
			 channel_mask,
			 samplerate);
#else
  caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout = (string) { interleaved }, channels = (int) %d, channel-mask = (bitmask) %#018lx, rate = (int) %d",
			 audio_channels,
			 channel_mask,
			 samplerate);
#endif

  g_object_set(rw_audio_app_src,
	       "caps", gst_caps_fixate(gst_caps_from_string(caps)),
	       "format", GST_FORMAT_TIME,
	       NULL);

  g_free(caps);

  g_signal_connect(rw_audio_app_src, "seek-data",
		   G_CALLBACK(ags_gstreamer_file_seek_data), gstreamer_file);

  g_signal_connect(rw_audio_app_src, "need-data",
		   G_CALLBACK(ags_gstreamer_file_start_feed), gstreamer_file);

  g_signal_connect(rw_audio_app_src, "enough-data",
		   G_CALLBACK(ags_gstreamer_file_stop_feed), gstreamer_file);  

  /* configure encoder */
  ags_gstreamer_file_detect_encoding_profile(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  encoding_profile = gstreamer_file->encoding_profile;

  if(encoding_profile != NULL){
    g_object_ref(encoding_profile);
  }
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  g_object_set(rw_file_encoder,
	       "profile", encoding_profile,
	       NULL);

  if(encoding_profile != NULL){
    g_object_unref(encoding_profile);
  }

  /* configure file sink */
  g_object_set(rw_file_sink,
	       "location", filename,
	       NULL);

  gst_bin_add_many(GST_BIN(write_pipeline),
		   rw_audio_app_src,
		   rw_audio_convert,
		   rw_audio_resample,
		   rw_audio_mixer,
		   rw_file_encoder,
		   rw_file_sink,
		   NULL);

  gst_element_link_many(rw_audio_app_src, rw_audio_convert, rw_audio_resample, rw_audio_mixer, NULL);
  gst_element_link_many(rw_file_encoder, rw_file_sink, NULL);

  rw_audio_mixer_src_pad = gst_element_get_static_pad(rw_audio_mixer, "src");

  rw_file_encoder_audio_pad = gst_element_get_request_pad(rw_file_encoder, "audio_%u");

  gst_pad_link(rw_audio_mixer_src_pad,
	       rw_file_encoder_audio_pad);
  
  bus = gst_element_get_bus(write_pipeline);
  
  gst_bus_add_signal_watch(bus);

  g_signal_connect(G_OBJECT(bus), "message::error",
		   G_CALLBACK(ags_gstreamer_file_error_callback), gstreamer_file);
  
  gst_object_unref(bus);
  
  /* apply */
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->write_pipeline = write_pipeline;

  gstreamer_file->rw_audio_app_src = rw_audio_app_src;
  gstreamer_file->rw_audio_convert = rw_audio_convert;
  gstreamer_file->rw_audio_resample = rw_audio_resample;
  gstreamer_file->rw_audio_mixer = rw_audio_mixer;
  gstreamer_file->rw_file_encoder = rw_file_encoder;
  gstreamer_file->rw_file_sink = rw_file_sink;

  gstreamer_file->rw_thread = rw_thread;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  state_change_retval = gst_element_set_state(write_pipeline,
					      GST_STATE_PLAYING);

  if(state_change_retval == GST_STATE_CHANGE_FAILURE){
    gstreamer_file->write_pipeline_running = FALSE;
    
    g_critical("unable to start AGS rw-pipeline (write)");
  }else{
#if 0
    do{
      gst_element_get_state(GST_ELEMENT(write_pipeline), &current_state, NULL, 4000000);
    }while(current_state != GST_STATE_PLAYING);
#endif
  }
  
  return(TRUE);
}

gboolean
ags_gstreamer_file_info(AgsSoundResource *sound_resource,
			guint *frame_count,
			guint *loop_start, guint *loop_end)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *read_pipeline, *write_pipeline;
  GstElement *audio_sink, *rw_audio_app_src;
  GstPad *sink_pad, *src_pad;
  GstCaps *current_caps;

  GstAudioInfo info;

  gint64 duration;
  guint current_frame_count;

  gboolean success;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  success = FALSE;
  
  if(loop_start != NULL){
    loop_start[0] = 0;
  }

  if(loop_end != NULL){
    loop_end[0] = 0;
  }
  
  /* get read pipeline */
  g_rec_mutex_lock(gstreamer_file_mutex);

  read_pipeline = gstreamer_file->read_pipeline;
  write_pipeline = gstreamer_file->write_pipeline;

  if(read_pipeline != NULL){
    g_object_ref(read_pipeline);
  }

  if(write_pipeline != NULL){
    g_object_ref(write_pipeline);
  }
  
  audio_sink = gstreamer_file->audio_sink;

  if(audio_sink != NULL){
    g_object_ref(audio_sink);
  }
  
  rw_audio_app_src = gstreamer_file->rw_audio_app_src;

  if(rw_audio_app_src != NULL){
    g_object_ref(rw_audio_app_src);
  }
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* query */
  if(audio_sink != NULL){  
    sink_pad = gst_element_get_static_pad(audio_sink,
					  "sink");
  
    current_caps = gst_pad_get_current_caps(sink_pad);

    gst_audio_info_from_caps(&info,
			     current_caps);

    duration = 0;
  
    gst_element_query_duration(read_pipeline,
			       GST_FORMAT_TIME, &duration);

    success = TRUE;
  }else if(rw_audio_app_src != NULL){
    src_pad = gst_element_get_static_pad(rw_audio_app_src,
					 "src");
  
    current_caps = gst_pad_get_current_caps(src_pad);

    gst_audio_info_from_caps(&info,
			     current_caps);

    duration = 0;
  
    gst_element_query_duration(write_pipeline,
			       GST_FORMAT_TIME, &duration);

    success = TRUE;
  }

  if(read_pipeline != NULL){
    g_object_unref(read_pipeline);
  }

  if(write_pipeline != NULL){
    g_object_unref(write_pipeline);
  }

  if(audio_sink != NULL){
    g_object_unref(audio_sink);
  }

  if(rw_audio_app_src != NULL){
    g_object_unref(rw_audio_app_src);
  }
  
//  g_message("format %d", info.finfo->format);

  current_frame_count = 0;
  
  if(success){
    current_frame_count = GST_CLOCK_TIME_TO_FRAMES(duration, GST_AUDIO_INFO_RATE(&info));
  }
  
  if(frame_count != NULL){
    frame_count[0] = current_frame_count;
  }
  
  return(success);
}

void
ags_gstreamer_file_set_presets(AgsSoundResource *sound_resource,
			       guint channels,
			       guint samplerate,
			       guint buffer_size,
			       guint format)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *rw_audio_app_src;

  gchar *caps;

  guint audio_channels;
  gint64 channel_mask;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  rw_audio_app_src = gstreamer_file->rw_audio_app_src;

  if(rw_audio_app_src != NULL){
    g_object_ref(rw_audio_app_src);
  }
  
  gstreamer_file->samplerate = samplerate;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  g_object_set(gstreamer_file,
	       "audio-channels", channels,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);

  if(rw_audio_app_src != NULL){
    channel_mask = -2;

    switch(audio_channels){
    case 2:
      {
	channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT);
      }
      break;
    case 3:
      {
	channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_LFE1);
      }
      break;
    case 6:
      {
	channel_mask = (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_LEFT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_RIGHT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_FRONT_CENTER) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_SIDE_LEFT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_SIDE_RIGHT) |
	  (1 << GST_AUDIO_CHANNEL_POSITION_LFE1);
      }
      break;
    case 8:
      {
	//TODO:JK: implement me
      }
      break;
    case 10:
      {
	//TODO:JK: implement me
      }
      break;
    }

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout= (string) { interleaved }, channels = (int) %d, rate= (int) %d",
			   channels,
			   channel_mask,
			   samplerate);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    caps = g_strdup_printf("audio/x-raw, format = (string) { S16BE }, layout= (string) { interleaved }, channels = (int) %d, rate= (int) %d",
			   channels,
			   channel_mask,
			   samplerate);
#else
    caps = g_strdup_printf("audio/x-raw, format = (string) { S16LE }, layout= (string) { interleaved }, channels = (int) %d, rate= (int) %d",
			   channels,
			   channel_mask,
			   samplerate);
#endif

    g_object_set(rw_audio_app_src,
		 "caps", gst_caps_from_string(caps),
		 NULL);

    g_free(caps);
  }
  
  if(rw_audio_app_src != NULL){
    g_object_unref(rw_audio_app_src);
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

  GstElement *audio_sink, *rw_audio_app_src;
  GstPad *sink_pad, *src_pad;
  GstCaps *current_caps;
  
  GstAudioInfo info;

  gint current_samplerate;
  gint current_channels;
  guint current_buffer_size;
  guint current_format;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* get buffer size and format */
  g_rec_mutex_lock(gstreamer_file_mutex);

  audio_sink = gstreamer_file->audio_sink;

  if(audio_sink != NULL){
    g_object_ref(audio_sink);
  }
  
  rw_audio_app_src = gstreamer_file->rw_audio_app_src;

  if(rw_audio_app_src != NULL){
    g_object_ref(rw_audio_app_src);
  }
  
  current_channels = gstreamer_file->audio_channels;
  current_samplerate = gstreamer_file->samplerate;
  current_buffer_size = gstreamer_file->buffer_size;
  current_format = gstreamer_file->format;

  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* config */
  if(audio_sink != NULL){
    sink_pad = gst_element_get_static_pad(audio_sink,
					  "sink");
  
    current_caps = gst_pad_get_current_caps(sink_pad);

    if(current_caps != NULL){
      gst_audio_info_from_caps(&info,
			       current_caps);

      current_channels = info.channels;
      current_samplerate = info.rate;
    }
  }else if(rw_audio_app_src != NULL){
    src_pad = gst_element_get_static_pad(rw_audio_app_src,
					 "src");
  
    current_caps = gst_pad_get_current_caps(src_pad);
  
    if(current_caps != NULL){
      gst_audio_info_from_caps(&info,
			       current_caps);

      current_channels = info.channels;
      current_samplerate = info.rate;
    }
  }
  
  if(audio_sink != NULL){
    g_object_unref(audio_sink);
  }

  if(rw_audio_app_src != NULL){
    g_object_unref(rw_audio_app_src);
  }

  if(channels != NULL){
    channels[0] = (current_channels >= 0) ? current_channels: 0;
  }

  if(samplerate != NULL){
    samplerate[0] = (current_samplerate >= 0) ? current_samplerate: 0;
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

  GstElement *audio_sink;
  
  guint saudio_channels;
  guint buffer_size;
  guint source_format;
  guint64 offset;
  guint total_frame_count;
  guint read_frame_count;
  guint copy_mode;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* get source audio channels */
  saudio_channels = 1;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  source_format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  ags_gstreamer_file_get_presets(sound_resource,
				 &saudio_channels,
				 NULL,
				 &buffer_size,
				 &source_format);
  
  /* get some fields */
  g_rec_mutex_lock(gstreamer_file_mutex);

  audio_sink = gstreamer_file->audio_sink;

  offset = gstreamer_file->offset;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  ags_sound_resource_info(AGS_SOUND_RESOURCE(sound_resource),
			  &total_frame_count,
			  NULL, NULL);

  /* read */
  read_frame_count = 0;

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(source_format));

  for(read_frame_count = 0; read_frame_count < frame_count && offset + read_frame_count < total_frame_count;){
    GstSample *sample;
    GstBuffer *buffer;

    GstMapInfo info;

    guint copy_frame_count;
    guint prev_frame_count;

    g_rec_mutex_lock(gstreamer_file_mutex);

    prev_frame_count = gstreamer_file->prev_frame_count;
    
    sample = gstreamer_file->last_sample;
    
    g_rec_mutex_unlock(gstreamer_file_mutex);
    
    /* pull sample */
    if(sample != NULL){
      /* attempt #0 */
      buffer = gst_sample_get_buffer(sample);
      
      gst_buffer_map(buffer,
		     &info, GST_MAP_READ);

      if(read_frame_count + (info.size / saudio_channels / sizeof(gint16)) < frame_count){
	copy_frame_count = (info.size / saudio_channels / sizeof(gint16));
      }else{
	copy_frame_count = frame_count - read_frame_count;
      }

      if(prev_frame_count + copy_frame_count > (info.size / saudio_channels / sizeof(gint16))){
	copy_frame_count = (info.size / saudio_channels / sizeof(gint16)) - prev_frame_count;
      }
      
      ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, daudio_channels * read_frame_count,
						  info.data, saudio_channels, saudio_channels * prev_frame_count + audio_channel,
						  copy_frame_count, copy_mode);

      gst_buffer_unmap(buffer,
		       &info);
      
      read_frame_count += copy_frame_count;

      g_rec_mutex_lock(gstreamer_file_mutex);

      if(prev_frame_count + copy_frame_count >= (info.size / saudio_channels / sizeof(gint16))){
	gstreamer_file->prev_frame_count = 0;
    
	gstreamer_file->last_sample = NULL;
	
	gst_sample_unref(sample);
      }else{
	gstreamer_file->prev_frame_count += copy_frame_count;

	gstreamer_file->last_sample = sample;
      }
      
      g_rec_mutex_unlock(gstreamer_file_mutex);
    }else{
      /* attempt #0 */
      sample = gst_app_sink_pull_sample(audio_sink);

      if(sample != NULL){
	buffer = gst_sample_get_buffer(sample);
      
	gst_buffer_map(buffer,
		       &info, GST_MAP_READ);

	if(read_frame_count + (info.size / saudio_channels / sizeof(gint16)) < frame_count){
	  copy_frame_count = (info.size / saudio_channels / sizeof(gint16));
	}else{
	  copy_frame_count = frame_count - read_frame_count;
	}

	if(prev_frame_count + copy_frame_count > (info.size / saudio_channels / sizeof(gint16))){
	  copy_frame_count = (info.size / saudio_channels / sizeof(gint16)) - prev_frame_count;
	}

	ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, daudio_channels * read_frame_count,
						    info.data, saudio_channels, audio_channel,
						    copy_frame_count, copy_mode);

	gst_buffer_unmap(buffer,
			 &info);
      
	read_frame_count += copy_frame_count;
	
	g_rec_mutex_lock(gstreamer_file_mutex);

	if(copy_frame_count >= (info.size / saudio_channels / sizeof(gint16))){
	  gstreamer_file->prev_frame_count = 0;
    
	  gstreamer_file->last_sample = NULL;
	
	  gst_sample_unref(sample);
	}else{
	  gstreamer_file->prev_frame_count += copy_frame_count;

	  gstreamer_file->last_sample = sample;
	}
      
	g_rec_mutex_unlock(gstreamer_file_mutex);
      }else{
	break;
      }
    }
  }
  
#if 0
  g_message("%d -> %d + %d", total_frame_count, gstreamer_file->offset, read_frame_count);
#endif
  
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->offset += read_frame_count;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  return(read_frame_count);
}

void
ags_gstreamer_file_write(AgsSoundResource *sound_resource,
			 void *sbuffer, guint saudio_channels,
			 guint audio_channel,
			 guint frame_count, guint format)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *rw_audio_app_src;

  GList *start_rw_current_buffer, *rw_current_buffer;
  GList *start_rw_current_info, *rw_current_info;
  
  guint samplerate;
  guint64 offset;
  guint daudio_channels;
  guint buffer_size;
  guint destination_format;
  guint word_size;
  guint orig_copy_frame_count, copy_frame_count;
  guint dbuffer_offset, sbuffer_offset;
  guint copy_mode;
  guint i, i_stop, j;
  gboolean do_write;

  GRecMutex *gstreamer_file_mutex;
   
  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  /* get source audio channels */
  daudio_channels = AGS_GSTREAMER_FILE_DEFAULT_AUDIO_CHANNELS;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  destination_format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  ags_gstreamer_file_get_presets(sound_resource,
				 &daudio_channels,
				 &samplerate,
				 &buffer_size,
				 &destination_format);

  g_rec_mutex_lock(gstreamer_file_mutex);

  rw_audio_app_src = gstreamer_file->rw_audio_app_src;

  if(rw_audio_app_src != NULL){
    g_object_ref(rw_audio_app_src);
  }

  offset = gstreamer_file->offset;

  start_rw_current_buffer = g_list_copy(gstreamer_file->rw_current_buffer);
  start_rw_current_info = g_list_copy(gstreamer_file->rw_current_info);

  dbuffer_offset = gstreamer_file->audio_channel_written[audio_channel];

  g_rec_mutex_unlock(gstreamer_file_mutex);

  word_size = sizeof(gint16);
  
  switch(destination_format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gchar);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(gint64);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  }
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(destination_format),
						  ags_audio_buffer_util_format_from_soundcard(format));

  start_rw_current_buffer = g_list_reverse(start_rw_current_buffer);
  start_rw_current_info = g_list_reverse(start_rw_current_info);
  
  rw_current_buffer = start_rw_current_buffer;
  rw_current_info = start_rw_current_info;

  sbuffer_offset = 0;  

  if(frame_count * saudio_channels > AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels)){
    copy_frame_count = AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels) / daudio_channels;
    
    i_stop = floor((double) frame_count / (double) (AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels) / daudio_channels));

    if(frame_count % (AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels) / daudio_channels) != 0){
      i_stop++;
    }
  }else{
    copy_frame_count = frame_count;
    
    i_stop = 1;
  }

  orig_copy_frame_count = copy_frame_count;
  
  for(i = 0; i < i_stop; i++){  
    GstBuffer *current_buffer;
  
    GstMapInfo *info;

    guint remaining_copy_frame_count;

    remaining_copy_frame_count = 0;
    
    g_rec_mutex_lock(gstreamer_file_mutex);

    if(i != 0){
      if(gstreamer_file->audio_channel_written[audio_channel] + copy_frame_count < (i + 1) * orig_copy_frame_count){
	copy_frame_count = (gstreamer_file->audio_channel_written[audio_channel] + copy_frame_count) - i * orig_copy_frame_count;
	
	remaining_copy_frame_count = frame_count - copy_frame_count;
      }
    }
    
    g_rec_mutex_unlock(gstreamer_file_mutex);

    current_buffer = NULL;
    
    if(rw_current_buffer != NULL){
      current_buffer = rw_current_buffer->data;
    }

    info = NULL;

    if(rw_current_info != NULL){
      info = rw_current_info->data;
    }
    
    if(current_buffer == NULL){
      info = g_malloc(sizeof(GstMapInfo));
      
      current_buffer = gst_buffer_new_and_alloc(AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels) * sizeof(gint16));

      dbuffer_offset = 0;
      
      /* Set its timestamp and duration */
      g_rec_mutex_lock(gstreamer_file_mutex);

      GST_BUFFER_TIMESTAMP(current_buffer) = gst_util_uint64_scale(offset + (i * orig_copy_frame_count), GST_SECOND, samplerate);
      GST_BUFFER_DURATION(current_buffer) = gst_util_uint64_scale(orig_copy_frame_count, GST_SECOND, samplerate);

      GST_BUFFER_OFFSET(current_buffer) = offset + (i * orig_copy_frame_count);
      GST_BUFFER_OFFSET_END(current_buffer) = offset + ((i + 1) * orig_copy_frame_count);
      
      gst_buffer_map(current_buffer,
		     info, GST_MAP_WRITE);      

      gstreamer_file->rw_current_buffer = g_list_prepend(gstreamer_file->rw_current_buffer,
							 current_buffer);
      gstreamer_file->rw_current_info = g_list_prepend(gstreamer_file->rw_current_info,
						       info);
      
      g_rec_mutex_unlock(gstreamer_file_mutex);

      ags_audio_buffer_util_clear_buffer(info->data, 1,
					 AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels), ags_audio_buffer_util_format_from_soundcard(destination_format));

    }else{
      if(copy_frame_count == AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels) / daudio_channels){
	dbuffer_offset = 0;
      }else{
	if((dbuffer_offset + copy_frame_count) < sbuffer_offset + (AGS_GSTREAMER_FILE_CHUNK_FRAME_COUNT(daudio_channels) / daudio_channels)){
	  dbuffer_offset = 0;
	}
      }
    }

    if(info != NULL &&
       info->data != NULL){
      ags_audio_buffer_util_copy_buffer_to_buffer(info->data, daudio_channels, (daudio_channels * dbuffer_offset) + audio_channel,
						  sbuffer, saudio_channels, saudio_channels * sbuffer_offset,
						  copy_frame_count, copy_mode);
    }
    
    g_rec_mutex_lock(gstreamer_file_mutex);

    gstreamer_file->audio_channel_written[audio_channel] += copy_frame_count;
    
    do_write = TRUE;

    for(j = 0; j < gstreamer_file->audio_channels; j++){
      if(gstreamer_file->audio_channel_written[j] < i_stop * orig_copy_frame_count){
	do_write = FALSE;
      
	break;
      }
    }
  
    g_rec_mutex_unlock(gstreamer_file_mutex);
  
    if(do_write){
      GList *start_list, *start_info;
      
      g_rec_mutex_lock(gstreamer_file_mutex);

      start_list = g_list_reverse(gstreamer_file->rw_current_buffer);
      start_info = g_list_reverse(gstreamer_file->rw_current_info);

      gstreamer_file->rw_current_buffer = NULL;
      gstreamer_file->rw_current_info = NULL;

      if(start_list != NULL){
	if(gstreamer_file->rw_buffer == NULL){
	  gstreamer_file->rw_buffer = start_list;
	}else{
	  gstreamer_file->rw_buffer = g_list_concat(gstreamer_file->rw_buffer,
						    start_list);
	}
      }
      
      if(start_info != NULL){
	if(gstreamer_file->rw_info == NULL){
	  gstreamer_file->rw_info = start_info;
	}else{
	  gstreamer_file->rw_info = g_list_concat(gstreamer_file->rw_info,
						  start_info);
	}
      }
      
      gstreamer_file->offset += frame_count;

      for(j = 0; j < gstreamer_file->audio_channels; j++){
	gstreamer_file->audio_channel_written[j] = (sbuffer_offset + copy_frame_count) - frame_count;
      }
  
      dbuffer_offset = 0;
      
      g_rec_mutex_unlock(gstreamer_file_mutex);
    }

    /* iterate */
    if(rw_current_buffer != NULL){
      rw_current_buffer = rw_current_buffer->next;
    }

    if(rw_current_info != NULL){
      rw_current_info = rw_current_info->next;
    }
    
    dbuffer_offset += copy_frame_count;    
    sbuffer_offset += copy_frame_count;
  }

  if(rw_audio_app_src != NULL){
    g_object_unref(rw_audio_app_src);
  }

  g_list_free(rw_current_buffer);
}

void
ags_gstreamer_file_flush(AgsSoundResource *sound_resource)
{
  AgsGstreamerFile *gstreamer_file;

  gboolean has_data;
  gboolean is_running;
  
  GRecMutex *gstreamer_file_mutex;
   
  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  has_data = (gstreamer_file->rw_buffer != NULL) ? TRUE: FALSE;
  is_running = gstreamer_file->write_pipeline_running;

  g_rec_mutex_unlock(gstreamer_file_mutex);

  while(has_data && is_running){
    g_usleep(AGS_GSTREAMER_FILE_DEFAULT_FLUSH_IDLE);

    g_rec_mutex_lock(gstreamer_file_mutex);

    has_data = (gstreamer_file->rw_buffer != NULL) ? TRUE: FALSE;
    is_running = gstreamer_file->write_pipeline_running;

    g_rec_mutex_unlock(gstreamer_file_mutex);    
  }
}

void
ags_gstreamer_file_seek(AgsSoundResource *sound_resource,
			gint64 frame_count, gint whence)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *read_pipeline;
  GstElement *write_pipeline;
  GstElement *audio_sink;
  GstPad *sink_pad;
  GstCaps *current_caps;

  GstAudioInfo info;

  gint64 time_nanoseconds;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  read_pipeline = gstreamer_file->read_pipeline;

  if(read_pipeline != NULL){
    g_object_ref(read_pipeline);
  }
  
  write_pipeline = gstreamer_file->write_pipeline;

  if(write_pipeline != NULL){
    g_object_ref(write_pipeline);
  }
  
  audio_sink = gstreamer_file->audio_sink;

  if(gstreamer_file->last_sample != NULL){
    gst_sample_unref(gstreamer_file->last_sample);

    gstreamer_file->prev_frame_count = 0;
    
    gstreamer_file->last_sample = NULL;
  }

  gstreamer_file->offset = frame_count;

  //FIXME:JK: improve it
  //  gstreamer_file->rw_buffer = NULL;
  gstreamer_file->current_buffer = NULL;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* query */  
  sink_pad = gst_element_get_static_pad(audio_sink,
					"sink");
  
  current_caps = gst_pad_get_current_caps(sink_pad);

  gst_audio_info_from_caps(&info,
			   current_caps);

  time_nanoseconds = GST_FRAMES_TO_CLOCK_TIME(frame_count, GST_AUDIO_INFO_RATE(&info));
  
  gst_element_seek(read_pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
		   GST_SEEK_TYPE_SET, time_nanoseconds,
		   GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);

  if(write_pipeline != NULL){
    gst_element_seek(write_pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
		     GST_SEEK_TYPE_SET, time_nanoseconds,
		     GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
  }

  if(read_pipeline != NULL){
    g_object_unref(read_pipeline);
  }

  if(write_pipeline != NULL){
    g_object_unref(read_pipeline);
  }
}

void
ags_gstreamer_file_close(AgsSoundResource *sound_resource)
{
  AgsGstreamerFile *gstreamer_file;
   
  GstElement *read_pipeline, *write_pipeline;

  gboolean read_pipeline_running, write_pipeline_running;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);
  
  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  read_pipeline = gstreamer_file->read_pipeline;
  read_pipeline_running = gstreamer_file->read_pipeline_running;

  write_pipeline = gstreamer_file->write_pipeline;
  write_pipeline_running = gstreamer_file->write_pipeline_running;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  if(read_pipeline_running){
    gst_element_set_state(read_pipeline, GST_STATE_NULL);
  }
  
  if(write_pipeline_running){
    gst_element_set_state(write_pipeline, GST_STATE_NULL);
  }

  g_rec_mutex_lock(gstreamer_file_mutex);

  if(gstreamer_file->read_pipeline != NULL){
    g_object_unref(gstreamer_file->read_pipeline);
  }
  
  gstreamer_file->read_pipeline = NULL;
  gstreamer_file->read_pipeline_running = FALSE;

  if(gstreamer_file->write_pipeline != NULL){
    g_object_unref(gstreamer_file->write_pipeline);
  }

  gstreamer_file->write_pipeline = NULL;
  gstreamer_file->write_pipeline_running = FALSE;

  if(gstreamer_file->playbin != NULL){
    g_object_unref(gstreamer_file->playbin);
  }

  if(gstreamer_file->video_sink != NULL){
    g_object_unref(gstreamer_file->video_sink);
  }

  if(gstreamer_file->audio_sink != NULL){
    g_object_unref(gstreamer_file->audio_sink);
  }

  if(gstreamer_file->text_sink != NULL){
    g_object_unref(gstreamer_file->text_sink);
  }

  if(gstreamer_file->rw_audio_app_src != NULL){
    g_object_unref(gstreamer_file->rw_audio_app_src);
  }

  if(gstreamer_file->rw_audio_convert != NULL){
    g_object_unref(gstreamer_file->rw_audio_convert);
  }

  if(gstreamer_file->rw_audio_resample != NULL){
    g_object_unref(gstreamer_file->rw_audio_resample);
  }

  if(gstreamer_file->rw_audio_mixer != NULL){
    g_object_unref(gstreamer_file->rw_audio_mixer);
  }

  if(gstreamer_file->rw_file_encoder != NULL){
    g_object_unref(gstreamer_file->rw_file_encoder);
  }

  if(gstreamer_file->rw_file_sink != NULL){
    g_object_unref(gstreamer_file->rw_file_sink);
  }

  //FIXME:JK: improve it
  gstreamer_file->last_sample = NULL;

  gstreamer_file->rw_buffer = NULL;
  gstreamer_file->current_buffer = NULL;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
}

/**
 * ags_gstreamer_file_check_suffix:
 * @filename: the filename
 * 
 * Check suffix.
 * 
 * Returns: %TRUE if suffix supported, else %FALSE
 * 
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_file_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".mp3") ||
     g_str_has_suffix(filename, ".aac") ||
     g_str_has_suffix(filename, ".mp4") ||
     g_str_has_suffix(filename, ".mkv") ||
     g_str_has_suffix(filename, ".webm") ||
     g_str_has_suffix(filename, ".mpg") ||
     g_str_has_suffix(filename, ".mpeg") ||
     g_str_has_suffix(filename, ".wav")){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_gstreamer_file_create_wav_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create wav rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_wav_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;
  
  GstCaps *caps;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }

  caps = gst_caps_from_string("audio/x-rf64");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);
  
  caps = gst_caps_from_string("audio/x-raw");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_create_mp3_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create mp3 rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_mp3_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;
  
  GstCaps *caps;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }

  caps = gst_caps_from_string("application/x-id3");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);
  
  caps = gst_caps_from_string("audio/mpeg,mpegversion=1,layer=3");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_create_aac_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create aac rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_aac_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;

  GstCaps *caps;
    
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }  

  caps = gst_caps_from_string("audio/aac");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);
  
  caps = gst_caps_from_string("audio/x-m4a");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_create_mp4_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create mp4 rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_mp4_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;
  
  GstCaps *caps;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }  

  caps = gst_caps_from_string("video/quicktime,variant=iso");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);

#if 0
  caps = gst_caps_from_string("video/x-h264");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_video_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);
#endif
  
  caps = gst_caps_from_string("audio/mpeg,mpegversion=4");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_create_mkv_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create mkv rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_mkv_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;

  GstCaps *caps;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }
  
  caps = gst_caps_from_string("video/x-matroska");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);  
  
#if 0
  caps = gst_caps_from_string("video/x-vp8");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_video_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);
#endif
  
  caps = gst_caps_from_string("audio/x-flac");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_create_webm_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create webm rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_webm_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;

  GstCaps *caps;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }
  
  caps = gst_caps_from_string("video/x-matroska");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);
  
  caps = gst_caps_from_string("audio/x-vorbis");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_create_mpeg_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Create mpeg rw pipeline.
 * 
 * Returns: the new #GstEncodingProfile
 * 
 * Since: 3.6.0
 */
GstEncodingProfile*
ags_gstreamer_file_create_mpeg_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *container_profile;

  GstCaps *caps;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(NULL);
  }
  
  caps = gst_caps_from_string("audio/mpegts");
  container_profile = gst_encoding_container_profile_new(NULL,
							 NULL,
							 caps,
							 NULL);
  gst_caps_unref(caps);
  
  caps = gst_caps_from_string("audio/x-ac3");
  gst_encoding_container_profile_add_profile(container_profile,
					     (GstEncodingProfile*) gst_encoding_audio_profile_new(caps, NULL, NULL, 0));
  gst_caps_unref(caps);

  return(container_profile);
}

/**
 * ags_gstreamer_file_detect_encoding_profile:
 * @gstreamer_file: the #AgsGstreamerFile
 * 
 * Detect rw and create rw pipeline.
 * 
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_file_detect_encoding_profile(AgsGstreamerFile *gstreamer_file)
{
  GstEncodingProfile *encoding_profile;
  
  gchar *filename;
  gchar *file_uri;
  
  gboolean success;

  GError *error;
  
  GRecMutex *gstreamer_file_mutex;
  
  if(!AGS_IS_GSTREAMER_FILE(gstreamer_file)){
    return(FALSE);
  }

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  success = FALSE;

  filename = NULL;
  file_uri = NULL;
  
  g_object_get(gstreamer_file,
	       "filename", &filename,
	       NULL);

  if(filename != NULL){
    if(g_path_is_absolute(filename)){
      file_uri = g_strdup_printf("file://%s",
				 filename);
    }else{
      gchar *current_dir;

      current_dir = g_get_current_dir();

      file_uri = g_strdup_printf("file://%s/%s",
				 current_dir,
				 filename);

      g_free(current_dir);
    }
  }

  //NOTE:JK: default is replace the file
#if 0
  {  
    GstDiscoverer *discoverer;
    GstDiscovererInfo *info;

    error = NULL;
    discoverer = gst_discoverer_new(AGS_GSTREAMER_FILE_DEFAULT_DISCOVERER_TIMEOUT,
				    &error);

    if(error != NULL){
      g_critical("%s", error->message);
    
      g_error_free(error);
    }
  
    error = NULL;
    info = gst_discoverer_discover_uri(discoverer,
				       file_uri,
				       &error);

    if(error != NULL){
      g_message("%s", error->message);
    
      g_error_free(error);
    }
  
    encoding_profile = gst_encoding_profile_from_discoverer(info);
  }
#endif
  
  encoding_profile = NULL;
  
  if(encoding_profile == NULL){
    if(g_str_has_suffix(filename, ".mp3")){
      encoding_profile = ags_gstreamer_file_create_mp3_encoding_profile(gstreamer_file);
    }else if(g_str_has_suffix(filename, ".aac")){
      encoding_profile = ags_gstreamer_file_create_aac_encoding_profile(gstreamer_file);
    }else if(g_str_has_suffix(filename, ".mp4")){
      encoding_profile = ags_gstreamer_file_create_mp4_encoding_profile(gstreamer_file);
    }else if(g_str_has_suffix(filename, ".mkv")){
      encoding_profile = ags_gstreamer_file_create_mkv_encoding_profile(gstreamer_file);
    }else if(g_str_has_suffix(filename, ".webm")){
      encoding_profile = ags_gstreamer_file_create_webm_encoding_profile(gstreamer_file);
    }else if(g_str_has_suffix(filename, ".mpg") ||
	     g_str_has_suffix(filename, ".mpeg")){
      encoding_profile = ags_gstreamer_file_create_mpeg_encoding_profile(gstreamer_file);
    }else if(g_str_has_suffix(filename, ".wav")){
      encoding_profile = ags_gstreamer_file_create_wav_encoding_profile(gstreamer_file);
    }
  }

  
  g_rec_mutex_lock(gstreamer_file_mutex);

  gstreamer_file->encoding_profile = encoding_profile;
    
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  g_free(filename);
  g_free(file_uri);
  
  return(success);
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
