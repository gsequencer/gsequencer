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

#include <ags/audio/file/ags_sndfile.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>

#include <string.h>

#include <sndfile.h>

#include <ags/i18n.h>

void ags_sndfile_class_init(AgsSndfileClass *sndfile);
void ags_sndfile_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sndfile_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_sndfile_init(AgsSndfile *sndfile);
void ags_sndfile_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_sndfile_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_sndfile_dispose(GObject *gobject);
void ags_sndfile_finalize(GObject *gobject);

AgsUUID* ags_sndfile_get_uuid(AgsConnectable *connectable);
gboolean ags_sndfile_has_resource(AgsConnectable *connectable);
gboolean ags_sndfile_is_ready(AgsConnectable *connectable);
void ags_sndfile_add_to_registry(AgsConnectable *connectable);
void ags_sndfile_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_sndfile_list_resource(AgsConnectable *connectable);
xmlNode* ags_sndfile_xml_compose(AgsConnectable *connectable);
void ags_sndfile_xml_parse(AgsConnectable *connectable,
			  xmlNode *node);
gboolean ags_sndfile_is_connected(AgsConnectable *connectable);
void ags_sndfile_connect(AgsConnectable *connectable);
void ags_sndfile_disconnect(AgsConnectable *connectable);

gboolean ags_sndfile_open(AgsSoundResource *sound_resource,
			  gchar *filename);
gboolean ags_sndfile_rw_open(AgsSoundResource *sound_resource,
			     gchar *filename,
			     guint audio_channels, guint samplerate,
			     gboolean create);
gboolean ags_sndfile_info(AgsSoundResource *sound_resource,
			  guint *frame_count,
			  guint *loop_start, guint *loop_end);
void ags_sndfile_set_presets(AgsSoundResource *sound_resource,
			     guint channels,
			     guint samplerate,
			     guint buffer_size,
			     guint format);
void ags_sndfile_get_presets(AgsSoundResource *sound_resource,
			     guint *channels,
			     guint *samplerate,
			     guint *buffer_size,
			     guint *format);
guint ags_sndfile_read(AgsSoundResource *sound_resource,
		       void *dbuffer, guint daudio_channels,
		       guint audio_channel,
		       guint frame_count, guint format);
void ags_sndfile_write(AgsSoundResource *sound_resource,
		       void *sbuffer, guint saudio_channels,
		       guint audio_channel,
		       guint frame_count, guint format);
void ags_sndfile_flush(AgsSoundResource *sound_resource);
void ags_sndfile_seek(AgsSoundResource *sound_resource,
		      gint64 frame_count, gint whence);
void ags_sndfile_close(AgsSoundResource *sound_resource);

sf_count_t ags_sndfile_vio_get_filelen(void *user_data);
sf_count_t ags_sndfile_vio_seek(sf_count_t offset, int whence, void *user_data);
sf_count_t ags_sndfile_vio_read(void *ptr, sf_count_t count, void *user_data);
sf_count_t ags_sndfile_vio_write(const void *ptr, sf_count_t count, void *user_data);
sf_count_t ags_sndfile_vio_tell(const void *ptr, sf_count_t count, void *user_data);

/**
 * SECTION:ags_sndfile
 * @short_description: Libsndfile wrapper
 * @title: AgsSndfile
 * @section_id:
 * @include: ags/audio/file/ags_sndfile.h
 *
 * #AgsSndfile is the base object to ineract with libsndfile.
 */

enum{
  PROP_0,
  PROP_AUDIO_CHANNELS,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_FILE,
};

static gpointer ags_sndfile_parent_class = NULL;
static AgsSoundResourceInterface *ags_sndfile_parent_sound_resource_interface;

static SF_VIRTUAL_IO *ags_sndfile_virtual_io = NULL;

static pthread_mutex_t ags_sndfile_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_sndfile_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sndfile = 0;

    static const GTypeInfo ags_sndfile_info = {
      sizeof (AgsSndfileClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sndfile_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSndfile),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sndfile_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_sndfile_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sndfile = g_type_register_static(G_TYPE_OBJECT,
					      "AgsSndfile",
					      &ags_sndfile_info,
					      0);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sndfile,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sndfile);
  }

  return g_define_type_id__volatile;
}

void
ags_sndfile_class_init(AgsSndfileClass *sndfile)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_sndfile_parent_class = g_type_class_peek_parent(sndfile);

  gobject = (GObjectClass *) sndfile;

  gobject->set_property = ags_sndfile_set_property;
  gobject->get_property = ags_sndfile_get_property;

  gobject->dispose = ags_sndfile_dispose;
  gobject->finalize = ags_sndfile_finalize;

  /* sndfile callbacks */
  if(ags_sndfile_virtual_io == NULL){
    ags_sndfile_virtual_io = (SF_VIRTUAL_IO *) malloc(sizeof(SF_VIRTUAL_IO));

    ags_sndfile_virtual_io->get_filelen = ags_sndfile_vio_get_filelen;
    ags_sndfile_virtual_io->seek = ags_sndfile_vio_seek;
    ags_sndfile_virtual_io->read = ags_sndfile_vio_read;
    ags_sndfile_virtual_io->write = ags_sndfile_vio_write;
    ags_sndfile_virtual_io->tell = ags_sndfile_vio_tell;
  }

  /* properties */
  /**
   * AgsSndfile:audio-channels:
   *
   * The audio channels to be used.
   * 
   * Since: 2.0.0
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
   * AgsSndfile:buffer-size:
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
   * AgsSndfile:format:
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
   * AgsSndfile:file:
   *
   * The assigned output #SNDFILE-struct.
   * 
   * Since: 2.0.0
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
ags_sndfile_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_sndfile_get_uuid;
  connectable->has_resource = ags_sndfile_has_resource;
  connectable->is_ready = ags_sndfile_is_ready;

  connectable->add_to_registry = ags_sndfile_add_to_registry;
  connectable->remove_from_registry = ags_sndfile_remove_from_registry;

  connectable->list_resource = ags_sndfile_list_resource;
  connectable->xml_compose = ags_sndfile_xml_compose;
  connectable->xml_parse = ags_sndfile_xml_parse;

  connectable->is_connected = ags_sndfile_is_connected;
  
  connectable->connect = ags_sndfile_connect;
  connectable->disconnect = ags_sndfile_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_sndfile_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  ags_sndfile_parent_sound_resource_interface = g_type_interface_peek_parent(sound_resource);

  sound_resource->open = ags_sndfile_open;
  sound_resource->rw_open = ags_sndfile_rw_open;

  sound_resource->load = NULL;

  sound_resource->info = ags_sndfile_info;

  sound_resource->set_presets = ags_sndfile_set_presets;
  sound_resource->get_presets = ags_sndfile_get_presets;
  
  sound_resource->read = ags_sndfile_read;

  sound_resource->write = ags_sndfile_write;
  sound_resource->flush = ags_sndfile_flush;
  
  sound_resource->seek = ags_sndfile_seek;

  sound_resource->close = ags_sndfile_close;
}

void
ags_sndfile_init(AgsSndfile *sndfile)
{
  AgsConfig *config;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  sndfile->flags = AGS_SNDFILE_FILL_CACHE;

  /* add audio file mutex */
  sndfile->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  sndfile->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  sndfile->uuid = ags_uuid_alloc();
  ags_uuid_generate(sndfile->uuid);

  config = ags_config_get_instance();

  sndfile->audio_channels = 1;
  sndfile->audio_channel_written = (gint64 *) malloc(sndfile->audio_channels * sizeof(gint64));

  sndfile->audio_channel_written[0] = -1;
  
  sndfile->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  sndfile->format = AGS_SOUNDCARD_DOUBLE;

  sndfile->offset = 0;
  sndfile->buffer_offset = 0;

  sndfile->full_buffer = NULL;
  sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
				     sndfile->format);

  sndfile->pointer = NULL;
  sndfile->current = NULL;
  sndfile->length = 0;

  sndfile->info = NULL;
  sndfile->file = NULL;
}

void
ags_sndfile_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsSndfile *sndfile;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(gobject);

  /* get audio file mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
    {
      guint audio_channels;
      guint i;
      
      audio_channels = g_value_get_uint(value);

      pthread_mutex_lock(sndfile_mutex);

      if(audio_channels == sndfile->audio_channels){
	pthread_mutex_unlock(sndfile_mutex);

	return;	
      }
      
      ags_stream_free(sndfile->buffer);

      if(audio_channels > 0){
	sndfile->audio_channel_written = (guint64 *) realloc(sndfile->audio_channel_written,
							     audio_channels * sizeof(guint64));
	
	for(i = sndfile->audio_channels; i < audio_channels; i++){
	  sndfile->audio_channel_written[i] = -1;
	}
      }else{
	free(sndfile->audio_channel_written);
      }

      sndfile->audio_channels = audio_channels;
      
      sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
					 sndfile->format);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      pthread_mutex_lock(sndfile_mutex);

      if(buffer_size == sndfile->buffer_size){
	pthread_mutex_unlock(sndfile_mutex);
	
	return;	
      }
      
      ags_stream_free(sndfile->buffer);

      sndfile->buffer_size = buffer_size;
      sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
					 sndfile->format);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      pthread_mutex_lock(sndfile_mutex);

      if(format == sndfile->format){
	pthread_mutex_unlock(sndfile_mutex);
      
	return;	
      }

      ags_stream_free(sndfile->buffer);

      sndfile->format = format;
      sndfile->buffer = ags_stream_alloc(sndfile->audio_channels * sndfile->buffer_size,
					 sndfile->format);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  case PROP_FILE:
    {
      SNDFILE *file;

      file = g_value_get_pointer(value);

      pthread_mutex_lock(sndfile_mutex);

      if(sndfile->file == file){
	pthread_mutex_unlock(sndfile_mutex);
	
	return;
      }
      
      sndfile->file = file;

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sndfile_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsSndfile *sndfile;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(gobject);

  /* get audio file mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());
  
  switch(prop_id){
  case PROP_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(sndfile_mutex);

      g_value_set_uint(value, sndfile->audio_channels);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(sndfile_mutex);

      g_value_set_uint(value, sndfile->buffer_size);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(sndfile_mutex);

      g_value_set_uint(value, sndfile->format);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  case PROP_FILE:
    {
      pthread_mutex_lock(sndfile_mutex);

      g_value_set_pointer(value, sndfile->file);

      pthread_mutex_unlock(sndfile_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sndfile_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_sndfile_parent_class)->dispose(gobject);
}

void
ags_sndfile_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_sndfile_parent_class)->finalize(gobject);
}


AgsUUID*
ags_sndfile_get_uuid(AgsConnectable *connectable)
{
  AgsSndfile *sndfile;
  
  AgsUUID *ptr;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(sndfile_mutex);

  ptr = sndfile->uuid;

  pthread_mutex_unlock(sndfile_mutex);
  
  return(ptr);
}

gboolean
ags_sndfile_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_sndfile_is_ready(AgsConnectable *connectable)
{
  AgsSndfile *sndfile;
  
  gboolean is_ready;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  /* check is ready */
  pthread_mutex_lock(sndfile_mutex);
  
  is_ready = (((AGS_SNDFILE_ADDED_TO_REGISTRY & (sndfile->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(sndfile_mutex);

  return(is_ready);
}

void
ags_sndfile_add_to_registry(AgsConnectable *connectable)
{
  AgsSndfile *sndfile;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  sndfile = AGS_SNDFILE(connectable);

  ags_sndfile_set_flags(sndfile, AGS_SNDFILE_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) sndfile);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_sndfile_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_sndfile_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_sndfile_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_sndfile_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_sndfile_is_connected(AgsConnectable *connectable)
{
  AgsSndfile *sndfile;
  
  gboolean is_connected;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(connectable);

  /* get audio file mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(sndfile_mutex);

  is_connected = (((AGS_SNDFILE_CONNECTED & (sndfile->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(sndfile_mutex);

  return(is_connected);
}

void
ags_sndfile_connect(AgsConnectable *connectable)
{
  AgsSndfile *sndfile;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sndfile = AGS_SNDFILE(connectable);
  
  ags_sndfile_set_flags(sndfile, AGS_SNDFILE_CONNECTED);
}

void
ags_sndfile_disconnect(AgsConnectable *connectable)
{
  AgsSndfile *sndfile;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sndfile = AGS_SNDFILE(connectable);

  ags_sndfile_unset_flags(sndfile, AGS_SNDFILE_CONNECTED);
}

/**
 * ags_sndfile_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.36
 */
pthread_mutex_t*
ags_sndfile_get_class_mutex()
{
  return(&ags_sndfile_class_mutex);
}

/**
 * ags_sndfile_test_flags:
 * @sndfile: the #AgsSndfile
 * @flags: the flags
 *
 * Test @flags to be set on @sndfile.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.36
 */
gboolean
ags_sndfile_test_flags(AgsSndfile *sndfile, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *sndfile_mutex;

  if(!AGS_IS_SNDFILE(sndfile)){
    return(FALSE);
  }

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  /* test */
  pthread_mutex_lock(sndfile_mutex);

  retval = (flags & (sndfile->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(sndfile_mutex);

  return(retval);
}

/**
 * ags_sndfile_set_flags:
 * @sndfile: the #AgsSndfile
 * @flags: see #AgsSndfileFlags-enum
 *
 * Enable a feature of @sndfile.
 *
 * Since: 2.0.36
 */
void
ags_sndfile_set_flags(AgsSndfile *sndfile, guint flags)
{
  pthread_mutex_t *sndfile_mutex;

  if(!AGS_IS_SNDFILE(sndfile)){
    return;
  }

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(sndfile_mutex);

  sndfile->flags |= flags;
  
  pthread_mutex_unlock(sndfile_mutex);
}
    
/**
 * ags_sndfile_unset_flags:
 * @sndfile: the #AgsSndfile
 * @flags: see #AgsSndfileFlags-enum
 *
 * Disable a feature of @sndfile.
 *
 * Since: 2.0.36
 */
void
ags_sndfile_unset_flags(AgsSndfile *sndfile, guint flags)
{  
  pthread_mutex_t *sndfile_mutex;

  if(!AGS_IS_SNDFILE(sndfile)){
    return;
  }

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(sndfile_mutex);

  sndfile->flags &= (~flags);
  
  pthread_mutex_unlock(sndfile_mutex);
}

gboolean
ags_sndfile_open(AgsSoundResource *sound_resource,
		 gchar *filename)
{
  AgsSndfile *sndfile;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  /* info */
  pthread_mutex_lock(sndfile_mutex);

  if(sndfile->info != NULL){
    pthread_mutex_unlock(sndfile_mutex);
    
    return(FALSE);
  }

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  sndfile->info->format = 0;
  sndfile->info->channels = 0;
  sndfile->info->samplerate = 0;

  pthread_mutex_unlock(sndfile_mutex);
  
  if(!ags_sndfile_test_flags(sndfile, AGS_SNDFILE_VIRTUAL)){
    if(filename != NULL){
      sndfile->file = (SNDFILE *) sf_open(filename, SFM_READ, sndfile->info);
    }
  }else{
    sndfile->file = (SNDFILE *) sf_open_virtual(ags_sndfile_virtual_io, SFM_READ, sndfile->info, sndfile);
  }
  
  if(sndfile->file == NULL){
    return(FALSE);
  }

  g_object_set(sndfile,
	       "audio-channels", sndfile->info->channels,
	       NULL);
  
#ifdef AGS_DEBUG
  g_message("ags_sndfile_open(): channels %d frames %d", sndfile->info->channels, sndfile->info->frames);
#endif

  return(TRUE);
}

gboolean
ags_sndfile_rw_open(AgsSoundResource *sound_resource,
		    gchar *filename,
		    guint audio_channels, guint samplerate,
		    gboolean create)
{
  AgsSndfile *sndfile;
  
  guint major_format;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  /* info */
  pthread_mutex_lock(sndfile_mutex);

  if(sndfile->info != NULL){
    pthread_mutex_unlock(sndfile_mutex);
    
    return(FALSE);
  }

  if(!create &&
     !g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
    pthread_mutex_unlock(sndfile_mutex);

    return(FALSE);
  }  

  sndfile->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  memset(sndfile->info, 0, sizeof(SF_INFO));

  sndfile->info->samplerate = (int) samplerate;
  sndfile->info->channels = (int) audio_channels;

  pthread_mutex_unlock(sndfile_mutex);

  if(g_str_has_suffix(filename, ".wav")){
    major_format = SF_FORMAT_WAV;

    sndfile->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sndfile,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".flac")){    
    major_format = SF_FORMAT_FLAC;

    sndfile->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sndfile,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".aiff")){    
    major_format = SF_FORMAT_AIFF;

    sndfile->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sndfile,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".ogg")){
    major_format = SF_FORMAT_OGG;

    sndfile->info->format = major_format | SF_FORMAT_VORBIS;

    g_object_set(sndfile,
		 "format", AGS_SOUNDCARD_DOUBLE,
		 NULL);
  }else{
    major_format = SF_FORMAT_WAV;

    sndfile->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sndfile,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }
  
  sndfile->info->frames = 0;
  sndfile->info->seekable = 0;
  sndfile->info->sections = 0;

  if(!sf_format_check(sndfile->info)){
    g_warning("invalid format");
  }

  if((AGS_SNDFILE_VIRTUAL & (sndfile->flags)) == 0){
    if(filename != NULL){
      sndfile->file = (SNDFILE *) sf_open(filename, SFM_RDWR, sndfile->info);
    }
  }else{
    sndfile->file = (SNDFILE *) sf_open_virtual(ags_sndfile_virtual_io, SFM_RDWR, sndfile->info, sndfile);
  }

  g_object_set(sndfile,
	       "audio-channels", audio_channels,
	       NULL);

  if(sndfile->file == NULL){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_sndfile_rw_open(): channels %d frames %d", sndfile->info->channels, sndfile->info->frames);
#endif

  return(TRUE);
}

gboolean
ags_sndfile_info(AgsSoundResource *sound_resource,
		 guint *frame_count,
		 guint *loop_start, guint *loop_end)
{
  AgsSndfile *sndfile;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  if(loop_start != NULL){
    *loop_start = 0;
  }

  if(loop_end != NULL){
    *loop_end = 0;
  }
  
  if(sndfile->info == NULL){
    if(frame_count != NULL){
      *frame_count = 0;
    }

    return(FALSE);
  }
  
  pthread_mutex_lock(sndfile_mutex);

  if(frame_count != NULL){
    *frame_count = sndfile->info->frames;
  }

  pthread_mutex_unlock(sndfile_mutex);

  return(TRUE);
}

void
ags_sndfile_set_presets(AgsSoundResource *sound_resource,
			guint channels,
			guint samplerate,
			guint buffer_size,
			guint format)
{
  AgsSndfile *sndfile;

  sndfile = AGS_SNDFILE(sound_resource);

  g_object_set(sndfile,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
  
  if(sndfile->info == NULL){
    return;
  }

  sndfile->info->channels = channels;
  sndfile->info->samplerate = samplerate;
  sndfile->info->format &= (~(SF_FORMAT_PCM_S8 |
			      SF_FORMAT_PCM_16 |
			      SF_FORMAT_PCM_24 |
			      SF_FORMAT_PCM_32 |
			      SF_FORMAT_FLOAT |
			      SF_FORMAT_DOUBLE));
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_24;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      sndfile->info->format |= SF_FORMAT_PCM_32;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      sndfile->info->format |= SF_FORMAT_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      sndfile->info->format |= SF_FORMAT_DOUBLE;
    }
    break;
  }
}

void
ags_sndfile_get_presets(AgsSoundResource *sound_resource,
			guint *channels,
			guint *samplerate,
			guint *buffer_size,
			guint *format)
{
  AgsSndfile *sndfile;
   
  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  if(sndfile->info == NULL){
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
  
  pthread_mutex_lock(sndfile_mutex);

  if(channels != NULL){
    *channels = sndfile->info->channels;
  }

  if(samplerate != NULL){
    *samplerate = sndfile->info->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = sndfile->buffer_size;
  }

  if(format != NULL){
    switch(((SF_FORMAT_PCM_S8 |
	     SF_FORMAT_PCM_16 |
	     SF_FORMAT_PCM_24 |
	     SF_FORMAT_PCM_32 |
	     SF_FORMAT_FLOAT |
	     SF_FORMAT_DOUBLE) & sndfile->info->format)){
    case SF_FORMAT_PCM_S8:
      {
	*format = AGS_SOUNDCARD_SIGNED_8_BIT;
      }
      break;
    case SF_FORMAT_PCM_16:
      {
	*format = AGS_SOUNDCARD_SIGNED_16_BIT;
      }
      break;
    case SF_FORMAT_PCM_24:
      {
	*format = AGS_SOUNDCARD_SIGNED_24_BIT;
      }
      break;
    case SF_FORMAT_PCM_32:
      {
	*format = AGS_SOUNDCARD_SIGNED_32_BIT;
      }
      break;
    case SF_FORMAT_FLOAT:
      {
	*format = AGS_SOUNDCARD_FLOAT;
      }
      break;
    case SF_FORMAT_DOUBLE:
      {
	*format = AGS_SOUNDCARD_DOUBLE;
      }
      break;
    }
  }

  pthread_mutex_unlock(sndfile_mutex);
}

guint
ags_sndfile_read(AgsSoundResource *sound_resource,
		 void *dbuffer, guint daudio_channels,
		 guint audio_channel,
		 guint frame_count, guint format)
{
  AgsSndfile *sndfile;

  sf_count_t multi_frames;
  guint total_frame_count;
  guint read_count;
  guint copy_mode;
  gboolean use_cache;
  guint i;

  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);
  
  pthread_mutex_lock(sndfile_mutex);

  if(sndfile->offset >= total_frame_count){
    pthread_mutex_unlock(sndfile_mutex);
    
    return(0);
  }

  if(sndfile->offset + frame_count >= total_frame_count){
    if(total_frame_count > sndfile->offset){
      frame_count = total_frame_count - sndfile->offset;
    }else{
      pthread_mutex_unlock(sndfile_mutex);
    
      return(0);
    }
  }

#if 0
  use_cache = FALSE;

  if(sndfile->buffer_offset == sndfile->offset &&
     frame_count <= sndfile->buffer_size){
    use_cache = TRUE;
  }
#endif
  
  sndfile->buffer_offset = sndfile->offset;
  
  read_count = sndfile->buffer_size;

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(sndfile->format));
  
  pthread_mutex_unlock(sndfile_mutex);
    
  for(i = 0; i < frame_count && sndfile->offset + i < total_frame_count; ){
    sf_count_t retval;
    
    if(sndfile->offset + frame_count > total_frame_count){
      read_count = total_frame_count - sndfile->offset;
    }

    multi_frames = read_count * sndfile->info->channels;

    retval = -1;
    
    //    if(!use_cache){
    //      g_message("read %d %d", sndfile->offset, sndfile->buffer_size);
      
      switch(sndfile->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  //TODO:JK: implement me
	  retval = 0;
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  retval = sf_read_short(sndfile->file, sndfile->buffer, multi_frames);
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
	  retval = sf_read_float(sndfile->file, sndfile->buffer, multi_frames);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  retval = sf_read_double(sndfile->file, sndfile->buffer, multi_frames);
	}
	break;
      }

      sndfile->offset += read_count;
      
      if(retval == -1){
	g_warning("read failed");
      }

      if(retval != multi_frames){
	break;
      }    
      //    }

    ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, (i * daudio_channels),
						sndfile->buffer, sndfile->info->channels, audio_channel,
						read_count, copy_mode);
    //    g_message("[%d] %d", audio_channel, ags_synth_util_get_xcross_count_s16(dbuffer, read_count));
    
    i += read_count;
  }
  
  return(frame_count);
}

void
ags_sndfile_write(AgsSoundResource *sound_resource,
		  void *sbuffer, guint saudio_channels,
		  guint audio_channel,
		  guint frame_count, guint format)
{
  AgsSndfile *sndfile;

  guint copy_mode;
  sf_count_t multi_frames;
  guint i;
  gboolean do_write;
  
  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());
  
  pthread_mutex_lock(sndfile_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(sndfile->format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  ags_audio_buffer_util_copy_buffer_to_buffer(sndfile->buffer, sndfile->info->channels, audio_channel,
					      sbuffer, saudio_channels, audio_channel,
					      frame_count, copy_mode);

  sndfile->audio_channel_written[audio_channel] = frame_count;
  do_write = TRUE;

  for(i = 0; i < sndfile->audio_channels; i++){
    if(sndfile->audio_channel_written[i] == -1){
      do_write = FALSE;
      
      break;
    }
  }
  
  pthread_mutex_unlock(sndfile_mutex);

  if(do_write){
    multi_frames = frame_count * sndfile->info->channels;

    switch(sndfile->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	sf_write_short(sndfile->file, sndfile->buffer, multi_frames);
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
	sf_write_float(sndfile->file, sndfile->buffer, multi_frames);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	sf_write_double(sndfile->file, sndfile->buffer, multi_frames);
      }
      break;
    }
    
    for(i = 0; i < sndfile->audio_channels; i++){
      sndfile->audio_channel_written[i] = -1;
    }

    if(sndfile->format == AGS_SOUNDCARD_DOUBLE){
      ags_audio_buffer_util_clear_double(sndfile->buffer, sndfile->info->channels,
					 frame_count);
    }else if(sndfile->format == AGS_SOUNDCARD_FLOAT){
      ags_audio_buffer_util_clear_float(sndfile->buffer, sndfile->info->channels,
					frame_count);
    }else{
      ags_audio_buffer_util_clear_buffer(sndfile->buffer, sndfile->info->channels,
					 frame_count, ags_audio_buffer_util_format_from_soundcard(sndfile->format));
    }
    
    sndfile->offset += frame_count;
  }
}

void
ags_sndfile_flush(AgsSoundResource *sound_resource)
{
  AgsSndfile *sndfile;
   
  sndfile = AGS_SNDFILE(sound_resource);

  if(sndfile->file == NULL){
    return;
  }
  
  sf_write_sync(sndfile->file);
}

void
ags_sndfile_seek(AgsSoundResource *sound_resource,
		 gint64 frame_count, gint whence)
{
  AgsSndfile *sndfile;

  guint total_frame_count;
  sf_count_t retval;
  
  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);

  pthread_mutex_lock(sndfile_mutex);

  if(whence == G_SEEK_CUR){
    if(frame_count >= 0){
      if(sndfile->offset + frame_count < total_frame_count){
	sndfile->offset += total_frame_count;
      }else{
	sndfile->offset = total_frame_count;
      }
    }else{
      if(sndfile->offset + frame_count >= 0){
	sndfile->offset += total_frame_count;
      }else{
	sndfile->offset = 0;
      }
    } 
  }else if(whence == G_SEEK_SET){
    if(frame_count >= 0){
      if(frame_count < total_frame_count){
	sndfile->offset = frame_count;
      }else{
	sndfile->offset = total_frame_count;
      }
    }else{
      sndfile->offset = 0;
    }
  }else if(whence == G_SEEK_END){
    if(frame_count > 0){
      sndfile->offset = total_frame_count;
    }else{
      if(total_frame_count + frame_count > 0){
	sndfile->offset = total_frame_count + total_frame_count;
      }else{
	sndfile->offset = 0;
      }
    }
  }

  pthread_mutex_unlock(sndfile_mutex);

  retval = sf_seek(sndfile->file, sndfile->offset, SEEK_SET);

  if(retval == -1){
    g_warning("seek failed");
  }
}

void
ags_sndfile_close(AgsSoundResource *sound_resource)
{
  AgsSndfile *sndfile;
   
  pthread_mutex_t *sndfile_mutex;

  sndfile = AGS_SNDFILE(sound_resource);

  /* get sndfile mutex */
  pthread_mutex_lock(ags_sndfile_get_class_mutex());
  
  sndfile_mutex = sndfile->obj_mutex;
  
  pthread_mutex_unlock(ags_sndfile_get_class_mutex());

  if(sndfile->file == NULL){
    return;
  }

  sf_close(sndfile->file);

  pthread_mutex_lock(sndfile_mutex);

  if(sndfile->info != NULL){
    free(sndfile->info);
  }

  sndfile->file = NULL;
  sndfile->info = NULL;

  pthread_mutex_unlock(sndfile_mutex);
}

sf_count_t
ags_sndfile_vio_get_filelen(void *user_data)
{
  return(AGS_SNDFILE(user_data)->length);
}

sf_count_t
ags_sndfile_vio_seek(sf_count_t offset, int whence, void *user_data)
{
  switch(whence){
  case SEEK_CUR:
    AGS_SNDFILE(user_data)->current += offset;
    break;
  case SEEK_SET:
    AGS_SNDFILE(user_data)->current = &(AGS_SNDFILE(user_data)->pointer[offset]);
    break;
  case SEEK_END:
    AGS_SNDFILE(user_data)->current = &(AGS_SNDFILE(user_data)->pointer[AGS_SNDFILE(user_data)->length - offset]);
    break;
  }

  return(AGS_SNDFILE(user_data)->current - AGS_SNDFILE(user_data)->pointer);
}

sf_count_t
ags_sndfile_vio_read(void *ptr, sf_count_t count, void *user_data)
{
  guchar *retval;

  retval = memcpy(ptr, AGS_SNDFILE(user_data)->current, count * sizeof(guchar));

  return(retval - AGS_SNDFILE(user_data)->pointer);
}

sf_count_t
ags_sndfile_vio_write(const void *ptr, sf_count_t count, void *user_data)
{
  guchar *retval;

  retval = memcpy(AGS_SNDFILE(user_data)->current, ptr, count * sizeof(guchar));

  return(retval - AGS_SNDFILE(user_data)->pointer);
}

sf_count_t
ags_sndfile_vio_tell(const void *ptr, sf_count_t count, void *user_data)
{
  return(AGS_SNDFILE(user_data)->current - AGS_SNDFILE(user_data)->pointer);
}

/**
 * ags_sndfile_new:
 *
 * Creates a new instance of #AgsSndfile.
 *
 * Returns: the new #AgsSndfile.
 *
 * Since: 2.0.0
 */
AgsSndfile*
ags_sndfile_new()
{
  AgsSndfile *sndfile;

  sndfile = (AgsSndfile *) g_object_new(AGS_TYPE_SNDFILE,
					NULL);

  return(sndfile);
}
