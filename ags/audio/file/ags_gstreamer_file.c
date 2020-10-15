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

/* playbin flags */
typedef enum {
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
  gstreamer_file->format = AGS_SOUNDCARD_DOUBLE;

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

  gstreamer_file->playbin = NULL;
  gstreamer_file->audio_sink = NULL;
  gstreamer_file->video_sink = NULL;
  
  gstreamer_file->data_source = NULL;
  gstreamer_file->data_source_convert = NULL;
  gstreamer_file->file_encoder = NULL;
  gstreamer_file->file_sink = NULL;

  gstreamer_file->prev_frame_count = 0;
  
  gstreamer_file->last_sample = NULL;
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
  guint blocksize;
  
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

#if 0  
  blocksize = buffer_size * audio_channels * sizeof(gdouble);
#endif
  
  audio_sink = gst_element_factory_make("appsink", "AGS audio sink");
  
  g_object_set(audio_sink,
	       "sync", FALSE,
	       NULL);
  
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { F64LE }, layout= (string) { interleaved }, channels = %s, rate= %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  caps = g_strdup_printf("audio/x-raw, format = (string) { F64BE }, layout= (string) { interleaved }, channels = %s, rate= %s",
			 GST_AUDIO_CHANNELS_RANGE,
			 GST_AUDIO_RATE_RANGE);
#else
  caps = g_strdup_printf("audio/x-raw, format = (string) { F64LE }, layout= (string) { interleaved }, channels = %s, rate= %s",
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
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

#if 0
  gst_base_sink_set_blocksize(audio_sink,
			      blocksize);
#endif
  
  state_change_retval = gst_element_set_state(read_pipeline,
					      GST_STATE_PLAYING);

  if(state_change_retval == GST_STATE_CHANGE_FAILURE){
    g_critical("unable to start AGS ro-pipeline");
  }else{
    do{
      gst_element_get_state(GST_ELEMENT(read_pipeline), &current_state, NULL, 4000);
    }while(current_state != GST_STATE_PLAYING);
  }
  
  return(TRUE);
}

gboolean
ags_gstreamer_file_rw_open(AgsSoundResource *sound_resource,
			   gchar *filename,
			   guint audio_channels, guint samplerate,
			   gboolean create)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me
  
  return(success);
}

gboolean
ags_gstreamer_file_info(AgsSoundResource *sound_resource,
			guint *frame_count,
			guint *loop_start, guint *loop_end)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *read_pipeline;
  GstElement *audio_sink;
  GstPad *sink_pad;
  GstCaps *current_caps;

  GstAudioInfo info;

  gint64 duration;
  guint current_frame_count;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  if(loop_start != NULL){
    loop_start[0] = 0;
  }

  if(loop_end != NULL){
    loop_end[0] = 0;
  }
  
  /* get read pipeline */
  g_rec_mutex_lock(gstreamer_file_mutex);

  read_pipeline = gstreamer_file->read_pipeline;
  
  audio_sink = gstreamer_file->audio_sink;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);

  /* query */  
  sink_pad = gst_element_get_static_pad(audio_sink,
					"sink");
  
  current_caps = gst_pad_get_current_caps(sink_pad);

  gst_audio_info_from_caps(&info,
			   current_caps);

  duration = 0;
  
  gst_element_query_duration(read_pipeline,
			     GST_FORMAT_TIME, &duration);

//  g_message("format %d", info.finfo->format);
  
  current_frame_count = GST_CLOCK_TIME_TO_FRAMES(duration, GST_AUDIO_INFO_RATE(&info));
  
  if(frame_count != NULL){
    frame_count[0] = current_frame_count;
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

  GstElement *audio_sink;

  guint audio_channels;
  guint blocksize;
  
  GRecMutex *gstreamer_file_mutex;

  gstreamer_file = AGS_GSTREAMER_FILE(sound_resource);

  /* get gstreamer_file mutex */
  gstreamer_file_mutex = AGS_GSTREAMER_FILE_GET_OBJ_MUTEX(gstreamer_file);

  g_rec_mutex_lock(gstreamer_file_mutex);

  audio_sink = gstreamer_file->audio_sink;

  audio_channels = channels;
  
  g_rec_mutex_unlock(gstreamer_file_mutex);
  
  g_object_set(gstreamer_file,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
  
#if 0
  blocksize = buffer_size * channels * sizeof(gdouble);

  gst_base_sink_set_blocksize(audio_sink,
			      blocksize);
#endif
  
  //TODO:JK: implement me
}

void
ags_gstreamer_file_get_presets(AgsSoundResource *sound_resource,
			       guint *channels,
			       guint *samplerate,
			       guint *buffer_size,
			       guint *format)
{
  AgsGstreamerFile *gstreamer_file;

  GstElement *audio_sink;
  GstPad *sink_pad;
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
  
  current_buffer_size = gstreamer_file->buffer_size;
  current_format = gstreamer_file->format;

  g_rec_mutex_unlock(gstreamer_file_mutex);

  sink_pad = gst_element_get_static_pad(audio_sink,
					"sink");
  
  current_caps = gst_pad_get_current_caps(sink_pad);
  
  gst_audio_info_from_caps(&info,
			   current_caps);

  current_channels = info.channels;
  current_samplerate = info.rate;
  
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

  guint blocksize;
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

      if(read_frame_count + (info.size / saudio_channels / sizeof(gdouble)) < frame_count){
	copy_frame_count = (info.size / saudio_channels / sizeof(gdouble));
      }else{
	copy_frame_count = frame_count - read_frame_count;
      }

      if(prev_frame_count + copy_frame_count > (info.size / saudio_channels / sizeof(gdouble))){
	copy_frame_count = (info.size / saudio_channels / sizeof(gdouble)) - prev_frame_count;
      }
      
      ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, daudio_channels * read_frame_count,
						  info.data, saudio_channels, saudio_channels * prev_frame_count + audio_channel,
						  copy_frame_count, copy_mode);

      gst_buffer_unmap(buffer,
		       &info);
      
      read_frame_count += copy_frame_count;

      g_rec_mutex_lock(gstreamer_file_mutex);

      if(prev_frame_count + copy_frame_count >= (info.size / saudio_channels / sizeof(gdouble))){
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

	if(read_frame_count + (info.size / saudio_channels / sizeof(gdouble)) < frame_count){
	  copy_frame_count = (info.size / saudio_channels / sizeof(gdouble));
	}else{
	  copy_frame_count = frame_count - read_frame_count;
	}

	if(prev_frame_count + copy_frame_count > (info.size / saudio_channels / sizeof(gdouble))){
	  copy_frame_count = (info.size / saudio_channels / sizeof(gdouble)) - prev_frame_count;
	}

	ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, daudio_channels * read_frame_count,
						    info.data, saudio_channels, audio_channel,
						    copy_frame_count, copy_mode);

	gst_buffer_unmap(buffer,
			 &info);
      
	read_frame_count += copy_frame_count;
	
	g_rec_mutex_lock(gstreamer_file_mutex);

	if(copy_frame_count >= (info.size / saudio_channels / sizeof(gdouble))){
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

  GstElement *read_pipeline;
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

  audio_sink = gstreamer_file->audio_sink;

  if(gstreamer_file->last_sample != NULL){
    gst_sample_unref(gstreamer_file->last_sample);

    gstreamer_file->prev_frame_count = 0;
    
    gstreamer_file->last_sample = NULL;
  }

  gstreamer_file->offset = frame_count;
  
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
  if(g_str_has_suffix(filename, ".aac") ||
     g_str_has_suffix(filename, ".mp3") ||
     g_str_has_suffix(filename, ".mp4") ||
     g_str_has_suffix(filename, ".mkv")){
    return(TRUE);
  }

  return(FALSE);
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
