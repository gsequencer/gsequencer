/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/file/ags_audio_toolbox.h>

#include <ags/audio/ags_synth_util.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <string.h>

#include <ags/i18n.h>

void ags_audio_toolbox_class_init(AgsAudioToolboxClass *audio_toolbox);
void ags_audio_toolbox_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_toolbox_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_audio_toolbox_init(AgsAudioToolbox *audio_toolbox);
void ags_audio_toolbox_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_audio_toolbox_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_audio_toolbox_dispose(GObject *gobject);
void ags_audio_toolbox_finalize(GObject *gobject);

AgsUUID* ags_audio_toolbox_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_toolbox_has_resource(AgsConnectable *connectable);
gboolean ags_audio_toolbox_is_ready(AgsConnectable *connectable);
void ags_audio_toolbox_add_to_registry(AgsConnectable *connectable);
void ags_audio_toolbox_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_toolbox_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_toolbox_xml_compose(AgsConnectable *connectable);
void ags_audio_toolbox_xml_parse(AgsConnectable *connectable,
				 xmlNode *node);
gboolean ags_audio_toolbox_is_connected(AgsConnectable *connectable);
void ags_audio_toolbox_connect(AgsConnectable *connectable);
void ags_audio_toolbox_disconnect(AgsConnectable *connectable);

gboolean ags_audio_toolbox_open(AgsSoundResource *sound_resource,
				gchar *filename);
gboolean ags_audio_toolbox_rw_open(AgsSoundResource *sound_resource,
				   gchar *filename,
				   guint audio_channels, guint samplerate,
				   gboolean create);
gboolean ags_audio_toolbox_info(AgsSoundResource *sound_resource,
				guint *frame_count,
				guint *loop_start, guint *loop_end);
void ags_audio_toolbox_set_presets(AgsSoundResource *sound_resource,
				   guint channels,
				   guint samplerate,
				   guint buffer_size,
				   guint format);
void ags_audio_toolbox_get_presets(AgsSoundResource *sound_resource,
				   guint *channels,
				   guint *samplerate,
				   guint *buffer_size,
				   guint *format);
guint ags_audio_toolbox_read(AgsSoundResource *sound_resource,
			     void *dbuffer, guint daudio_channels,
			     guint audio_channel,
			     guint frame_count, guint format);
void ags_audio_toolbox_write(AgsSoundResource *sound_resource,
			     void *sbuffer, guint saudio_channels,
			     guint audio_channel,
			     guint frame_count, guint format);
void ags_audio_toolbox_flush(AgsSoundResource *sound_resource);
void ags_audio_toolbox_seek(AgsSoundResource *sound_resource,
			    gint64 frame_count, gint whence);
void ags_audio_toolbox_close(AgsSoundResource *sound_resource);

/**
 * SECTION:ags_audio_toolbox
 * @short_description: Libaudio_toolbox wrapper
 * @title: AgsAudioToolbox
 * @section_id:
 * @include: ags/audio/file/ags_audio_toolbox.h
 *
 * #AgsAudioToolbox is the base object to ineract with libaudio_toolbox.
 */

enum{
  PROP_0,
  PROP_AUDIO_CHANNELS,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_FILE,
};

static gpointer ags_audio_toolbox_parent_class = NULL;
static AgsSoundResourceInterface *ags_audio_toolbox_parent_sound_resource_interface;

GType
ags_audio_toolbox_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_toolbox = 0;

    static const GTypeInfo ags_audio_toolbox_info = {
      sizeof (AgsAudioToolboxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_toolbox_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioToolbox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_toolbox_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_toolbox_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_audio_toolbox_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_toolbox = g_type_register_static(G_TYPE_OBJECT,
						    "AgsAudioToolbox",
						    &ags_audio_toolbox_info,
						    0);

    g_type_add_interface_static(ags_type_audio_toolbox,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_toolbox,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_toolbox);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_toolbox_class_init(AgsAudioToolboxClass *audio_toolbox)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_audio_toolbox_parent_class = g_type_class_peek_parent(audio_toolbox);

  gobject = (GObjectClass *) audio_toolbox;

  gobject->set_property = ags_audio_toolbox_set_property;
  gobject->get_property = ags_audio_toolbox_get_property;

  gobject->dispose = ags_audio_toolbox_dispose;
  gobject->finalize = ags_audio_toolbox_finalize;

  /* properties */
  /**
   * AgsAudioToolbox:audio-channels:
   *
   * The audio channels to be used.
   * 
   * Since: 3.12.0
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
   * AgsAudioToolbox:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 3.12.0
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
   * AgsAudioToolbox:samplerate:
   *
   * The samplerate to be used.
   * 
   * Since: 3.12.0
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
   * AgsAudioToolbox:format:
   *
   * The format to be used.
   * 
   * Since: 3.12.0
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
   * AgsAudioToolbox:file:
   *
   * The assigned output #AUDIO_TOOLBOX-struct.
   * 
   * Since: 3.12.0
   */
  param_spec = g_param_spec_pointer("file",
				    i18n_pspec("assigned file"),
				    i18n_pspec("The assigned file"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);
}

void
ags_audio_toolbox_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_toolbox_get_uuid;
  connectable->has_resource = ags_audio_toolbox_has_resource;
  connectable->is_ready = ags_audio_toolbox_is_ready;

  connectable->add_to_registry = ags_audio_toolbox_add_to_registry;
  connectable->remove_from_registry = ags_audio_toolbox_remove_from_registry;

  connectable->list_resource = ags_audio_toolbox_list_resource;
  connectable->xml_compose = ags_audio_toolbox_xml_compose;
  connectable->xml_parse = ags_audio_toolbox_xml_parse;

  connectable->is_connected = ags_audio_toolbox_is_connected;
  
  connectable->connect = ags_audio_toolbox_connect;
  connectable->disconnect = ags_audio_toolbox_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_toolbox_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  ags_audio_toolbox_parent_sound_resource_interface = g_type_interface_peek_parent(sound_resource);

  sound_resource->open = ags_audio_toolbox_open;
  sound_resource->rw_open = ags_audio_toolbox_rw_open;

  sound_resource->load = NULL;

  sound_resource->info = ags_audio_toolbox_info;

  sound_resource->set_presets = ags_audio_toolbox_set_presets;
  sound_resource->get_presets = ags_audio_toolbox_get_presets;
  
  sound_resource->read = ags_audio_toolbox_read;

  sound_resource->write = ags_audio_toolbox_write;
  sound_resource->flush = ags_audio_toolbox_flush;
  
  sound_resource->seek = ags_audio_toolbox_seek;

  sound_resource->close = ags_audio_toolbox_close;
}

void
ags_audio_toolbox_init(AgsAudioToolbox *audio_toolbox)
{
  AgsConfig *config;

  audio_toolbox->flags = AGS_AUDIO_TOOLBOX_FILL_CACHE;

  /* add audio file mutex */
  g_rec_mutex_init(&(audio_toolbox->obj_mutex));  

  /* uuid */
  audio_toolbox->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_toolbox->uuid);

  config = ags_config_get_instance();

  audio_toolbox->audio_channels = 1;
  audio_toolbox->audio_channel_written = (gint64 *) malloc(audio_toolbox->audio_channels * sizeof(gint64));

  audio_toolbox->audio_channel_written[0] = -1;
  
  audio_toolbox->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio_toolbox->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio_toolbox->format = AGS_SOUNDCARD_DOUBLE;

  audio_toolbox->offset = 0;
  audio_toolbox->buffer_offset = 0;

  audio_toolbox->full_buffer = NULL;
  audio_toolbox->buffer = ags_stream_alloc(audio_toolbox->audio_channels * audio_toolbox->buffer_size,
					   audio_toolbox->format);

  audio_toolbox->pointer = NULL;
  audio_toolbox->current = NULL;
  audio_toolbox->length = 0;

  audio_toolbox->audio_file = NULL;
  audio_toolbox->audio_buffer_list = NULL;
  audio_toolbox->stream = NULL;
}

void
ags_audio_toolbox_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsAudioToolbox *audio_toolbox;

  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(gobject);

  /* get audio file mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
    {
      guint audio_channels;
      guint i;
      
      audio_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_toolbox_mutex);

      if(audio_channels == audio_toolbox->audio_channels){
	g_rec_mutex_unlock(audio_toolbox_mutex);

	return;	
      }
      
      ags_stream_free(audio_toolbox->buffer);

      if(audio_channels > 0){
	audio_toolbox->audio_channel_written = (gint64 *) realloc(audio_toolbox->audio_channel_written,
								  audio_channels * sizeof(gint64));
	
	for(i = audio_toolbox->audio_channels; i < audio_channels; i++){
	  audio_toolbox->audio_channel_written[i] = -1;
	}
      }else{
	free(audio_toolbox->audio_channel_written);
      }

      audio_toolbox->audio_channels = audio_channels;
      
      audio_toolbox->buffer = ags_stream_alloc(audio_toolbox->audio_channels * audio_toolbox->buffer_size,
					       audio_toolbox->format);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(audio_toolbox_mutex);

      if(samplerate == audio_toolbox->samplerate){
	g_rec_mutex_unlock(audio_toolbox_mutex);
      
	return;	
      }

      audio_toolbox->samplerate = samplerate;

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(audio_toolbox_mutex);

      if(buffer_size == audio_toolbox->buffer_size){
	g_rec_mutex_unlock(audio_toolbox_mutex);
	
	return;	
      }
      
      ags_stream_free(audio_toolbox->buffer);

      audio_toolbox->buffer_size = buffer_size;
      audio_toolbox->buffer = ags_stream_alloc(audio_toolbox->audio_channels * audio_toolbox->buffer_size,
					       audio_toolbox->format);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(audio_toolbox_mutex);

      if(format == audio_toolbox->format){
	g_rec_mutex_unlock(audio_toolbox_mutex);
      
	return;	
      }

      ags_stream_free(audio_toolbox->buffer);

      audio_toolbox->format = format;
      audio_toolbox->buffer = ags_stream_alloc(audio_toolbox->audio_channels * audio_toolbox->buffer_size,
					       audio_toolbox->format);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_FILE:
    {
      ExtAudioFileRef audio_file;

      audio_file = g_value_get_pointer(value);

      g_rec_mutex_lock(audio_toolbox_mutex);

      if(audio_toolbox->audio_file == audio_file){
	g_rec_mutex_unlock(audio_toolbox_mutex);
	
	return;
      }
      
      audio_toolbox->audio_file = audio_file;

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_audio_toolbox_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsAudioToolbox *audio_toolbox;

  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(gobject);

  /* get audio file mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);
  
  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
    {
      g_rec_mutex_lock(audio_toolbox_mutex);

      g_value_set_uint(value, audio_toolbox->audio_channels);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(audio_toolbox_mutex);

      g_value_set_uint(value, audio_toolbox->samplerate);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(audio_toolbox_mutex);

      g_value_set_uint(value, audio_toolbox->buffer_size);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(audio_toolbox_mutex);

      g_value_set_uint(value, audio_toolbox->format);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  case PROP_FILE:
    {
      g_rec_mutex_lock(audio_toolbox_mutex);

      g_value_set_pointer(value, audio_toolbox->audio_file);

      g_rec_mutex_unlock(audio_toolbox_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_audio_toolbox_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_audio_toolbox_parent_class)->dispose(gobject);
}

void
ags_audio_toolbox_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_audio_toolbox_parent_class)->finalize(gobject);
}


AgsUUID*
ags_audio_toolbox_get_uuid(AgsConnectable *connectable)
{
  AgsAudioToolbox *audio_toolbox;
  
  AgsUUID *ptr;

  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(connectable);

  /* get audio file mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  /* get UUID */
  g_rec_mutex_lock(audio_toolbox_mutex);

  ptr = audio_toolbox->uuid;

  g_rec_mutex_unlock(audio_toolbox_mutex);
  
  return(ptr);
}

gboolean
ags_audio_toolbox_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_audio_toolbox_is_ready(AgsConnectable *connectable)
{
  AgsAudioToolbox *audio_toolbox;
  
  gboolean is_ready;

  audio_toolbox = AGS_AUDIO_TOOLBOX(connectable);

  /* check is ready */
  is_ready = ags_audio_toolbox_test_flags(audio_toolbox, AGS_AUDIO_TOOLBOX_ADDED_TO_REGISTRY);

  return(is_ready);
}

void
ags_audio_toolbox_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioToolbox *audio_toolbox;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  audio_toolbox = AGS_AUDIO_TOOLBOX(connectable);

  ags_audio_toolbox_set_flags(audio_toolbox, AGS_AUDIO_TOOLBOX_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) audio_toolbox);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_audio_toolbox_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_audio_toolbox_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_toolbox_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_toolbox_xml_parse(AgsConnectable *connectable,
			    xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_audio_toolbox_is_connected(AgsConnectable *connectable)
{
  AgsAudioToolbox *audio_toolbox;
  
  gboolean is_connected;

  audio_toolbox = AGS_AUDIO_TOOLBOX(connectable);

  /* check is connected */
  is_connected = ags_audio_toolbox_test_flags(audio_toolbox, AGS_AUDIO_TOOLBOX_CONNECTED);

  return(is_connected);
}

void
ags_audio_toolbox_connect(AgsConnectable *connectable)
{
  AgsAudioToolbox *audio_toolbox;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_toolbox = AGS_AUDIO_TOOLBOX(connectable);
  
  ags_audio_toolbox_set_flags(audio_toolbox, AGS_AUDIO_TOOLBOX_CONNECTED);
}

void
ags_audio_toolbox_disconnect(AgsConnectable *connectable)
{
  AgsAudioToolbox *audio_toolbox;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_toolbox = AGS_AUDIO_TOOLBOX(connectable);

  ags_audio_toolbox_unset_flags(audio_toolbox, AGS_AUDIO_TOOLBOX_CONNECTED);
}

/**
 * ags_audio_toolbox_test_flags:
 * @audio_toolbox: the #AgsAudioToolbox
 * @flags: the flags
 *
 * Test @flags to be set on @audio_toolbox.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.12.0
 */
gboolean
ags_audio_toolbox_test_flags(AgsAudioToolbox *audio_toolbox, guint flags)
{
  gboolean retval;  
  
  GRecMutex *audio_toolbox_mutex;

  if(!AGS_IS_AUDIO_TOOLBOX(audio_toolbox)){
    return(FALSE);
  }

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  /* test */
  g_rec_mutex_lock(audio_toolbox_mutex);

  retval = (flags & (audio_toolbox->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_toolbox_mutex);

  return(retval);
}

/**
 * ags_audio_toolbox_set_flags:
 * @audio_toolbox: the #AgsAudioToolbox
 * @flags: see #AgsAudioToolboxFlags-enum
 *
 * Enable a feature of @audio_toolbox.
 *
 * Since: 3.12.0
 */
void
ags_audio_toolbox_set_flags(AgsAudioToolbox *audio_toolbox, guint flags)
{
  GRecMutex *audio_toolbox_mutex;

  if(!AGS_IS_AUDIO_TOOLBOX(audio_toolbox)){
    return;
  }

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_toolbox_mutex);

  audio_toolbox->flags |= flags;
  
  g_rec_mutex_unlock(audio_toolbox_mutex);
}
    
/**
 * ags_audio_toolbox_unset_flags:
 * @audio_toolbox: the #AgsAudioToolbox
 * @flags: see #AgsAudioToolboxFlags-enum
 *
 * Disable a feature of @audio_toolbox.
 *
 * Since: 3.12.0
 */
void
ags_audio_toolbox_unset_flags(AgsAudioToolbox *audio_toolbox, guint flags)
{  
  GRecMutex *audio_toolbox_mutex;

  if(!AGS_IS_AUDIO_TOOLBOX(audio_toolbox)){
    return;
  }

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_toolbox_mutex);

  audio_toolbox->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_toolbox_mutex);
}

gboolean
ags_audio_toolbox_open(AgsSoundResource *sound_resource,
		       gchar *filename)
{
  AgsAudioToolbox *audio_toolbox;

  AudioStreamBasicDescription *stream, *client_stream;
  
  guint format;
  guint samplerate;
  guint audio_channels;
  guint32 prop_size;
  OSStatus retval;
  
  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  /* info */
  NSString *audio_file_path = [[NSString stringWithUTF8String:filename]
			       stringByExpandingTildeInPath];

  NSURL *audio_url = [NSURL fileURLWithPath:audio_file_path];
  
  g_rec_mutex_lock(audio_toolbox_mutex);

  retval = !noErr;
  
  if(filename != NULL){
    retval = ExtAudioFileOpenURL(audio_url, &(audio_toolbox->audio_file));
  }
  
  if(retval != noErr){
    g_rec_mutex_unlock(audio_toolbox_mutex);
    
    return(FALSE);
  }

  stream = (AudioStreamBasicDescription *) g_malloc(sizeof(AudioStreamBasicDescription));

  memset(stream, 0, sizeof(AudioStreamBasicDescription));

  prop_size = sizeof(AudioStreamBasicDescription);

  retval = ExtAudioFileGetProperty(audio_toolbox->audio_file,
				   kExtAudioFileProperty_FileDataFormat,
				   &prop_size, stream);

  audio_toolbox->stream = stream;

  audio_channels = stream->mChannelsPerFrame;
  samplerate = stream->mSampleRate;

  //TODO:JK: improve format
  client_stream = (AudioStreamBasicDescription *) g_malloc(sizeof(AudioStreamBasicDescription));
  
  memcpy(client_stream, stream, sizeof(AudioStreamBasicDescription));

  client_stream->mSampleRate = samplerate;
  client_stream->mFormatID = kAudioFormatLinearPCM;
  client_stream->mFormatFlags = kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
  client_stream->mBitsPerChannel = 16;
  client_stream->mChannelsPerFrame = audio_channels;
  client_stream->mBytesPerFrame = client_stream->mChannelsPerFrame * 2;
  client_stream->mFramesPerPacket = 1;
  client_stream->mBytesPerPacket = client_stream->mFramesPerPacket * client_stream->mBytesPerFrame;
  
  retval = ExtAudioFileSetProperty(audio_toolbox->audio_file,
				   kExtAudioFileProperty_ClientDataFormat,
				   prop_size, client_stream);

  audio_toolbox->audio_buffer_list = (AudioBufferList *) g_malloc(sizeof(AudioBufferList));

  format = AGS_SOUNDCARD_SIGNED_16_BIT;

  audio_toolbox->audio_buffer_list->mBuffers[0].mDataByteSize = audio_channels * audio_toolbox->buffer_size * sizeof(gint16);
  
  audio_toolbox->audio_buffer_list->mNumberBuffers = 1;
  audio_toolbox->audio_buffer_list->mBuffers[0].mNumberChannels = audio_channels;
  audio_toolbox->audio_buffer_list->mBuffers[0].mData = ags_stream_alloc(audio_channels * audio_toolbox->buffer_size,
									 format);
  
  g_rec_mutex_unlock(audio_toolbox_mutex);
  
  g_object_set(audio_toolbox,
	       "samplerate", samplerate,
	       "audio-channels", audio_channels,
	       "format", format,
	       NULL);
  
  return(TRUE);
}

gboolean
ags_audio_toolbox_rw_open(AgsSoundResource *sound_resource,
			  gchar *filename,
			  guint audio_channels, guint samplerate,
			  gboolean create)
{
  AgsAudioToolbox *audio_toolbox;
  
  guint major_format;
  OSStatus retval;
  gboolean success;
    
  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  /* info */
  if(!create &&
     !g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
    return(FALSE);
  }
  
  NSString *audio_file_path = [[NSString stringWithUTF8String:filename]
			       stringByExpandingTildeInPath];

  NSURL *audio_url = [NSURL fileURLWithPath:audio_file_path];

  g_rec_mutex_lock(audio_toolbox_mutex);

  audio_toolbox->stream = (AudioStreamBasicDescription *) g_malloc(sizeof(AudioStreamBasicDescription));
  memset(audio_toolbox->stream, 0, sizeof(AudioStreamBasicDescription));

  audio_toolbox->stream->mSampleRate = samplerate;
  audio_toolbox->stream->mFormatID = kAudioFormatLinearPCM;
  audio_toolbox->stream->mFormatFlags = 0;
  audio_toolbox->stream->mBytesPerPacket = 1;
  audio_toolbox->stream->mFramesPerPacket = 1;
  audio_toolbox->stream->mBytesPerFrame = 1;
  audio_toolbox->stream->mChannelsPerFrame = audio_channels;
  audio_toolbox->stream->mBitsPerChannel = 8;
  
  g_rec_mutex_unlock(audio_toolbox_mutex);
  
  if(g_str_has_suffix(filename, ".wav")){
    g_rec_mutex_lock(audio_toolbox_mutex);
    
    retval = ExtAudioFileCreateWithURL((CFURLRef) audio_url,
				       kAudioFileWAVEType,
				       &(audio_toolbox->stream),
				       NULL,
				       kAudioFileFlags_EraseFile,
				       &(audio_toolbox->audio_file));
    
    g_rec_mutex_unlock(audio_toolbox_mutex);

    g_object_set(audio_toolbox,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".aif") ||
	   g_str_has_suffix(filename, ".aiff") ||
	   g_str_has_suffix(filename, ".aifc")){
    g_rec_mutex_lock(audio_toolbox_mutex);

    retval = ExtAudioFileCreateWithURL((CFURLRef) audio_url,
				       kAudioFileAIFFType,
				       &(audio_toolbox->stream),
				       NULL,
				       kAudioFileFlags_EraseFile,
				       &(audio_toolbox->audio_file));

    g_rec_mutex_unlock(audio_toolbox_mutex);

    g_object_set(audio_toolbox,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else{
    g_rec_mutex_lock(audio_toolbox_mutex);
    
    retval = ExtAudioFileCreateWithURL((CFURLRef) audio_url,
				       kAudioFileWAVEType,
				       &(audio_toolbox->stream),
				       NULL,
				       kAudioFileFlags_EraseFile,
				       &(audio_toolbox->audio_file));
    
    g_rec_mutex_unlock(audio_toolbox_mutex);

    g_object_set(audio_toolbox,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }
  
  g_rec_mutex_lock(audio_toolbox_mutex);
    
  success = (audio_toolbox->audio_file != NULL) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_toolbox_mutex);

  g_object_set(audio_toolbox,
	       "samplerate", samplerate,
	       "audio-channels", audio_channels,
	       NULL);

  return(success);
}

gboolean
ags_audio_toolbox_info(AgsSoundResource *sound_resource,
		       guint *frame_count,
		       guint *loop_start, guint *loop_end)
{
  AgsAudioToolbox *audio_toolbox;

  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  if(loop_start != NULL){
    loop_start[0] = 0;
  }

  if(loop_end != NULL){
    loop_end[0] = 0;
  }
  
  g_rec_mutex_lock(audio_toolbox_mutex);

  if(audio_toolbox->stream == NULL){
    g_rec_mutex_unlock(audio_toolbox_mutex);
    
    if(frame_count != NULL){
      frame_count[0] = 0;
    }

    return(FALSE);
  }

  if(frame_count != NULL){
    if(audio_toolbox->stream != NULL){
      gint64 file_length_frames;
      guint32 prop_size;
      
      prop_size = sizeof(gint64);
      ExtAudioFileGetProperty(audio_toolbox->audio_file,
			      kExtAudioFileProperty_FileLengthFrames,
			      &prop_size, &file_length_frames);
      
      frame_count[0] = file_length_frames;
    }else{
      frame_count[0] = 0;
    }
  }

  g_rec_mutex_unlock(audio_toolbox_mutex);

  return(TRUE);
}

void
ags_audio_toolbox_set_presets(AgsSoundResource *sound_resource,
			      guint channels,
			      guint samplerate,
			      guint buffer_size,
			      guint format)
{
  AgsAudioToolbox *audio_toolbox;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  g_object_set(audio_toolbox,
	       "audio-channels", channels,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_audio_toolbox_get_presets(AgsSoundResource *sound_resource,
			      guint *channels,
			      guint *samplerate,
			      guint *buffer_size,
			      guint *format)
{
  AgsAudioToolbox *audio_toolbox;
   
  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  g_rec_mutex_lock(audio_toolbox_mutex);

  if(audio_toolbox->stream == NULL){
    g_rec_mutex_unlock(audio_toolbox_mutex);

    if(channels != NULL){
      *channels = 0;
    }

    if(samplerate != NULL){
      *samplerate = 0;
    }
    
    if(buffer_size != NULL){
      *buffer_size = 0;
    }

    if(format != NULL){
      *format = 0;
    }

    return;
  }
  
  if(channels != NULL){
    *channels = audio_toolbox->audio_channels;
  }

  if(samplerate != NULL){
    *samplerate = audio_toolbox->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = audio_toolbox->buffer_size;
  }
  
  if(format != NULL){
    *format = audio_toolbox->format;
  }

  g_rec_mutex_unlock(audio_toolbox_mutex);
}

guint
ags_audio_toolbox_read(AgsSoundResource *sound_resource,
		       void *dbuffer, guint daudio_channels,
		       guint audio_channel,
		       guint frame_count, guint format)
{
  AgsAudioToolbox *audio_toolbox;

  guint32 multi_frames;
  guint total_frame_count;
  guint read_count;
  guint copy_mode;
  gboolean use_cache;
  guint i;

  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  total_frame_count = 0;

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);
  
  g_rec_mutex_lock(audio_toolbox_mutex);

  if(audio_toolbox->offset >= total_frame_count){
    g_rec_mutex_unlock(audio_toolbox_mutex);
    
    return(0);
  }

  if(audio_toolbox->offset + frame_count >= total_frame_count){
    if(total_frame_count > audio_toolbox->offset){
      frame_count = total_frame_count - audio_toolbox->offset;
    }else{
      g_rec_mutex_unlock(audio_toolbox_mutex);
    
      return(0);
    }
  }

#if 0
  use_cache = FALSE;

  if(audio_toolbox->buffer_offset == audio_toolbox->offset &&
     frame_count <= audio_toolbox->buffer_size){
    use_cache = TRUE;
  }
#endif
  
  audio_toolbox->buffer_offset = audio_toolbox->offset;
  
  read_count = audio_toolbox->buffer_size;

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(audio_toolbox->format));
      
  for(i = 0; i < frame_count && audio_toolbox->offset + i < total_frame_count; ){
    OSStatus retval;
    
    if(audio_toolbox->offset + read_count > total_frame_count){
      read_count = total_frame_count - audio_toolbox->offset;
    }

    if(i + read_count > frame_count){
      read_count = frame_count - i;
    }
    
    multi_frames = read_count * audio_toolbox->audio_channels;

    retval = -1;

    ags_audio_buffer_util_clear_buffer(audio_toolbox->audio_buffer_list->mBuffers[0].mData, 1,
				       audio_toolbox->audio_channels * audio_toolbox->buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
    
    //    if(!use_cache){
    //      g_message("read %d %d", audio_toolbox->offset, audio_toolbox->buffer_size);
      
    switch(audio_toolbox->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	//TODO:JK: implement me
	retval = 0;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	retval = ExtAudioFileRead(audio_toolbox->audio_file, &multi_frames, audio_toolbox->audio_buffer_list);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	//TODO:JK: implement me
	retval = 0;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	//TODO:JK: implement me
	retval = 0;
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	retval = ExtAudioFileRead(audio_toolbox->audio_file, &multi_frames, audio_toolbox->audio_buffer_list);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	retval = ExtAudioFileRead(audio_toolbox->audio_file, &multi_frames, audio_toolbox->audio_buffer_list);
      }
      break;
    }

    audio_toolbox->offset += read_count;
      
    if(retval == -1){
      g_warning("read failed");
    }

    //    }
    
    ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, (i * daudio_channels),
						audio_toolbox->audio_buffer_list->mBuffers[0].mData, audio_toolbox->audio_channels, audio_channel,
						read_count, copy_mode);
    //    g_message("[%d] %d", audio_channel, ags_synth_util_get_xcross_count_s16(dbuffer, read_count));
  
    // g_message("xcross %d", ags_synth_util_get_xcross_count_s16(audio_toolbox->audio_buffer_list->mBuffers[0].mData, read_count));
    
    i += read_count;
  }

  g_rec_mutex_unlock(audio_toolbox_mutex);
  
  return(frame_count);
}

void
ags_audio_toolbox_write(AgsSoundResource *sound_resource,
			void *sbuffer, guint saudio_channels,
			guint audio_channel,
			guint frame_count, guint format)
{
  AgsAudioToolbox *audio_toolbox;

  guint copy_mode;
  guint32 multi_frames;
  guint i;
  gboolean do_write;
  
  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);
  
  g_rec_mutex_lock(audio_toolbox_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_toolbox->format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  ags_audio_buffer_util_copy_buffer_to_buffer(audio_toolbox->buffer, audio_toolbox->audio_channels, audio_channel,
					      sbuffer, saudio_channels, audio_channel,
					      frame_count, copy_mode);

  audio_toolbox->audio_channel_written[audio_channel] = frame_count;
  do_write = TRUE;

  for(i = 0; i < audio_toolbox->audio_channels; i++){
    if(audio_toolbox->audio_channel_written[i] == -1){
      do_write = FALSE;
      
      break;
    }
  }

  if(do_write){
    multi_frames = frame_count * audio_toolbox->audio_channels;

    switch(audio_toolbox->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	audio_toolbox->audio_buffer_list->mBuffers[0].mDataByteSize = multi_frames * sizeof(gint16);
	audio_toolbox->audio_buffer_list->mBuffers[0].mData = sbuffer;

	ExtAudioFileWrite(audio_toolbox->audio_file, multi_frames, audio_toolbox->audio_buffer_list);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	audio_toolbox->audio_buffer_list->mBuffers[0].mDataByteSize = multi_frames * sizeof(gfloat);
	audio_toolbox->audio_buffer_list->mBuffers[0].mData = sbuffer;

	ExtAudioFileWrite(audio_toolbox->audio_file, multi_frames, audio_toolbox->audio_buffer_list);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	audio_toolbox->audio_buffer_list->mBuffers[0].mDataByteSize = multi_frames * sizeof(gdouble);
	audio_toolbox->audio_buffer_list->mBuffers[0].mData = sbuffer;

	ExtAudioFileWrite(audio_toolbox->audio_file, multi_frames, audio_toolbox->audio_buffer_list);
      }
      break;
    }
    
    for(i = 0; i < audio_toolbox->audio_channels; i++){
      audio_toolbox->audio_channel_written[i] = -1;
    }

    if(audio_toolbox->format == AGS_SOUNDCARD_DOUBLE){
      ags_audio_buffer_util_clear_double(audio_toolbox->buffer, audio_toolbox->audio_channels,
					 frame_count);
    }else if(audio_toolbox->format == AGS_SOUNDCARD_FLOAT){
      ags_audio_buffer_util_clear_float(audio_toolbox->buffer, audio_toolbox->audio_channels,
					frame_count);
    }else{
      ags_audio_buffer_util_clear_buffer(audio_toolbox->buffer, audio_toolbox->audio_channels,
					 frame_count, ags_audio_buffer_util_format_from_soundcard(audio_toolbox->format));
    }
    
    audio_toolbox->offset += frame_count;
  }
  
  g_rec_mutex_unlock(audio_toolbox_mutex);
}

void
ags_audio_toolbox_flush(AgsSoundResource *sound_resource)
{
  AgsAudioToolbox *audio_toolbox;
  
  GRecMutex *audio_toolbox_mutex;
   
  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  g_rec_mutex_lock(audio_toolbox_mutex);

  if(audio_toolbox->audio_file == NULL){
    g_rec_mutex_unlock(audio_toolbox_mutex);

    return;
  }
  
  g_message("flush not implemented");

  g_rec_mutex_unlock(audio_toolbox_mutex);
}

void
ags_audio_toolbox_seek(AgsSoundResource *sound_resource,
		       gint64 frame_count, gint whence)
{
  AgsAudioToolbox *audio_toolbox;

  guint total_frame_count;
  OSStatus retval;
  
  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);

  g_rec_mutex_lock(audio_toolbox_mutex);

  if(whence == G_SEEK_CUR){
    if(frame_count >= 0){
      if(audio_toolbox->offset + frame_count < total_frame_count){
	audio_toolbox->offset += total_frame_count;
      }else{
	audio_toolbox->offset = total_frame_count;
      }
    }else{
      if(audio_toolbox->offset + frame_count >= 0){
	audio_toolbox->offset += total_frame_count;
      }else{
	audio_toolbox->offset = 0;
      }
    } 
  }else if(whence == G_SEEK_SET){
    if(frame_count >= 0){
      if(frame_count < total_frame_count){
	audio_toolbox->offset = frame_count;
      }else{
	audio_toolbox->offset = total_frame_count;
      }
    }else{
      audio_toolbox->offset = 0;
    }
  }else if(whence == G_SEEK_END){
    if(frame_count > 0){
      audio_toolbox->offset = total_frame_count;
    }else{
      if(total_frame_count + frame_count > 0){
	audio_toolbox->offset = total_frame_count + total_frame_count;
      }else{
	audio_toolbox->offset = 0;
      }
    }
  }

  retval = ExtAudioFileSeek(audio_toolbox->audio_file, audio_toolbox->offset);

  g_rec_mutex_unlock(audio_toolbox_mutex);

  if(retval == -1){
    g_warning("seek failed");
  }
}

void
ags_audio_toolbox_close(AgsSoundResource *sound_resource)
{
  AgsAudioToolbox *audio_toolbox;
   
  GRecMutex *audio_toolbox_mutex;

  audio_toolbox = AGS_AUDIO_TOOLBOX(sound_resource);

  /* get audio_toolbox mutex */
  audio_toolbox_mutex = AGS_AUDIO_TOOLBOX_GET_OBJ_MUTEX(audio_toolbox);

  if(audio_toolbox->audio_file == NULL){
    return;
  }

  ExtAudioFileDispose(audio_toolbox->audio_file);

  g_rec_mutex_lock(audio_toolbox_mutex);

  audio_toolbox->audio_file = NULL;

  g_rec_mutex_unlock(audio_toolbox_mutex);
}

/**
 * ags_audio_toolbox_check_suffix:
 * @filename: the filename
 * 
 * Check suffix.
 * 
 * Returns: %TRUE if suffix supported, else %FALSE
 * 
 * Since: 3.12.0
 */
gboolean
ags_audio_toolbox_check_suffix(gchar *filename)
{
  if(g_str_has_suffix(filename, ".aac") ||
     g_str_has_suffix(filename, ".adts") ||
     g_str_has_suffix(filename, ".ac3") ||
     g_str_has_suffix(filename, ".aif") ||
     g_str_has_suffix(filename, ".aiff") ||
     g_str_has_suffix(filename, ".caf") ||
     g_str_has_suffix(filename, ".mp3") ||
     g_str_has_suffix(filename, ".mp4") ||
     g_str_has_suffix(filename, ".m4a") ||
     g_str_has_suffix(filename, ".snd") ||
     g_str_has_suffix(filename, ".au") ||
     g_str_has_suffix(filename, ".sd2") ||
     g_str_has_suffix(filename, ".wav") ||
     g_str_has_suffix(filename, ".flac")){
    return(TRUE);
  }

  return(FALSE);
}

/**
 * ags_audio_toolbox_new:
 *
 * Creates a new instance of #AgsAudioToolbox.
 *
 * Returns: the new #AgsAudioToolbox.
 *
 * Since: 3.12.0
 */
AgsAudioToolbox*
ags_audio_toolbox_new()
{
  AgsAudioToolbox *audio_toolbox;

  audio_toolbox = (AgsAudioToolbox *) g_object_new(AGS_TYPE_AUDIO_TOOLBOX,
						   NULL);

  return(audio_toolbox);
}
