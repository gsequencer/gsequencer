/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/file/ags_sfz_sample.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/file/ags_sound_resource.h>
#include <ags/audio/file/ags_sfz_group.h>
#include <ags/audio/file/ags_sfz_region.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_sfz_sample_class_init(AgsSFZSampleClass *sfz_sample);
void ags_sfz_sample_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sfz_sample_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource);
void ags_sfz_sample_init(AgsSFZSample *sfz_sample);
void ags_sfz_sample_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_sample_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_sfz_sample_dispose(GObject *gobject);
void ags_sfz_sample_finalize(GObject *gobject);

AgsUUID* ags_sfz_sample_get_uuid(AgsConnectable *connectable);
gboolean ags_sfz_sample_has_resource(AgsConnectable *connectable);
gboolean ags_sfz_sample_is_ready(AgsConnectable *connectable);
void ags_sfz_sample_add_to_registry(AgsConnectable *connectable);
void ags_sfz_sample_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_sfz_sample_list_resource(AgsConnectable *connectable);
xmlNode* ags_sfz_sample_xml_compose(AgsConnectable *connectable);
void ags_sfz_sample_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_sfz_sample_is_connected(AgsConnectable *connectable);
void ags_sfz_sample_connect(AgsConnectable *connectable);
void ags_sfz_sample_disconnect(AgsConnectable *connectable);

gboolean ags_sfz_sample_open(AgsSoundResource *sound_resource,
			     gchar *filename);
gboolean ags_sfz_sample_rw_open(AgsSoundResource *sound_resource,
				gchar *filename,
				guint audio_channels, guint samplerate,
				gboolean create);
gboolean ags_sfz_sample_info(AgsSoundResource *sound_resource,
			     guint *frame_count,
			     guint *loop_start, guint *loop_end);
void ags_sfz_sample_set_presets(AgsSoundResource *sound_resource,
				guint channels,
				guint samplerate,
				guint buffer_size,
				guint format);
void ags_sfz_sample_get_presets(AgsSoundResource *sound_resource,
				guint *channels,
				guint *samplerate,
				guint *buffer_size,
				guint *format);
guint ags_sfz_sample_read(AgsSoundResource *sound_resource,
			  void *dbuffer, guint daudio_channels,
			  guint audio_channel,
			  guint frame_count, guint format);
void ags_sfz_sample_write(AgsSoundResource *sound_resource,
			  void *sbuffer, guint saudio_channels,
			  guint audio_channel,
			  guint frame_count, guint format);
void ags_sfz_sample_flush(AgsSoundResource *sound_resource);
void ags_sfz_sample_seek(AgsSoundResource *sound_resource,
			 gint64 frame_count, gint whence);
void ags_sfz_sample_close(AgsSoundResource *sound_resource);

/**
 * SECTION:ags_sfz_sample
 * @short_description: interfacing SFZ samples
 * @title: AgsSFZSample
 * @section_id:
 * @include: ags/audio/file/ags_sfz_sample.h
 *
 * #AgsSFZSample is the base object to ineract with SFZ samples.
 */

enum{
  PROP_0,
  PROP_BUFFER_SIZE,
  PROP_FORMAT,
  PROP_LOOP_START,
  PROP_LOOP_END,
  PROP_GROUP,
  PROP_REGION,
};

static gpointer ags_sfz_sample_parent_class = NULL;

static pthread_mutex_t ags_sfz_sample_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_sfz_sample_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sfz_sample = 0;

    static const GTypeInfo ags_sfz_sample_info = {
      sizeof(AgsSFZSampleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sfz_sample_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsSFZSample),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sfz_sample_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sfz_sample_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_sound_resource_interface_info = {
      (GInterfaceInitFunc) ags_sfz_sample_sound_resource_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sfz_sample = g_type_register_static(G_TYPE_OBJECT,
						 "AgsSFZSample",
						 &ags_sfz_sample_info,
						 0);

    g_type_add_interface_static(ags_type_sfz_sample,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_sfz_sample,
				AGS_TYPE_SOUND_RESOURCE,
				&ags_sound_resource_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sfz_sample);
  }

  return g_define_type_id__volatile;
}

void
ags_sfz_sample_class_init(AgsSFZSampleClass *sfz_sample)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_sfz_sample_parent_class = g_type_class_peek_parent(sfz_sample);

  gobject = (GObjectClass *) sfz_sample;

  gobject->set_property = ags_sfz_sample_set_property;
  gobject->get_property = ags_sfz_sample_get_property;

  gobject->dispose = ags_sfz_sample_dispose;
  gobject->finalize = ags_sfz_sample_finalize;

  /* properties */
  /**
   * AgsSFZSample:buffer-size:
   *
   * The buffer size to be used.
   * 
   * Since: 2.3.0
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
   * AgsSFZSample:format:
   *
   * The format to be used.
   * 
   * Since: 2.3.0
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
   * AgsSFZSample:loop-start:
   *
   * The loop start to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("loop-start",
				 i18n_pspec("using loop start"),
				 i18n_pspec("The loop start to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_START,
				  param_spec);

  /**
   * AgsSFZSample:loop-end:
   *
   * The loop end to be used.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_uint("loop-end",
				 i18n_pspec("using loop end"),
				 i18n_pspec("The loop end to be used"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOOP_END,
				  param_spec);

  /**
   * AgsSFZSample:group:
   *
   * The group assigned with.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_object("group",
				   i18n_pspec("assigned group"),
				   i18n_pspec("The group it is assigned with"),
				   AGS_TYPE_SFZ_GROUP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GROUP,
				  param_spec);

  /**
   * AgsSFZSample:region:
   *
   * The region assigned with.
   * 
   * Since: 2.3.0
   */
  param_spec = g_param_spec_object("region",
				   i18n_pspec("assigned region"),
				   i18n_pspec("The region it is assigned with"),
				   AGS_TYPE_SFZ_REGION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REGION,
				  param_spec);
}

void
ags_sfz_sample_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_sfz_sample_get_uuid;
  connectable->has_resource = ags_sfz_sample_has_resource;
  connectable->is_ready = ags_sfz_sample_is_ready;

  connectable->add_to_registry = ags_sfz_sample_add_to_registry;
  connectable->remove_from_registry = ags_sfz_sample_remove_from_registry;

  connectable->list_resource = ags_sfz_sample_list_resource;
  connectable->xml_compose = ags_sfz_sample_xml_compose;
  connectable->xml_parse = ags_sfz_sample_xml_parse;

  connectable->is_connected = ags_sfz_sample_is_connected;
  
  connectable->connect = ags_sfz_sample_connect;
  connectable->disconnect = ags_sfz_sample_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_sfz_sample_sound_resource_interface_init(AgsSoundResourceInterface *sound_resource)
{
  sound_resource->open = ags_sfz_sample_open;
  sound_resource->rw_open = ags_sfz_sample_rw_open;

  sound_resource->load = NULL;

  sound_resource->info = ags_sfz_sample_info;

  sound_resource->set_presets = ags_sfz_sample_set_presets;
  sound_resource->get_presets = ags_sfz_sample_get_presets;
  
  sound_resource->read = ags_sfz_sample_read;

  sound_resource->write = ags_sfz_sample_write;
  sound_resource->flush = ags_sfz_sample_flush;
  
  sound_resource->seek = ags_sfz_sample_seek;

  sound_resource->close = ags_sfz_sample_close;
}

void
ags_sfz_sample_init(AgsSFZSample *sfz_sample)
{
  AgsConfig *config;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  sfz_sample->flags = 0;

  /* add audio file mutex */
  sfz_sample->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  sfz_sample->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);  

  /* uuid */
  sfz_sample->uuid = ags_uuid_alloc();
  ags_uuid_generate(sfz_sample->uuid);

  config = ags_config_get_instance();

  sfz_sample->audio_channels = 1;
  sfz_sample->audio_channel_written = (gint64 *) malloc(sfz_sample->audio_channels * sizeof(gint64));

  sfz_sample->audio_channel_written[0] = -1;

  sfz_sample->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  sfz_sample->format = AGS_SOUNDCARD_DOUBLE;

  sfz_sample->loop_start = 0;
  sfz_sample->loop_end = 0;

  sfz_sample->offset = 0;
  sfz_sample->buffer_offset = 0;

  sfz_sample->full_buffer = NULL;
  sfz_sample->buffer = ags_stream_alloc(sfz_sample->audio_channels * sfz_sample->buffer_size,
					sfz_sample->format);

  sfz_sample->pointer = NULL;
  sfz_sample->current = NULL;
  sfz_sample->length = 0;

  sfz_sample->info = NULL;
  sfz_sample->file = NULL;

  sfz_sample->group = NULL;
  sfz_sample->region = NULL;
}

void
ags_sfz_sample_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZSample *sfz_sample;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(gobject);

  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  switch(prop_id){
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    pthread_mutex_lock(sfz_sample_mutex);

    if(buffer_size == sfz_sample->buffer_size){
      pthread_mutex_unlock(sfz_sample_mutex);

      return;	
    }
      
    ags_stream_free(sfz_sample->buffer);

    sfz_sample->buffer_size = buffer_size;
    sfz_sample->buffer = ags_stream_alloc(sfz_sample->buffer_size,
					  sfz_sample->format);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    pthread_mutex_lock(sfz_sample_mutex);

    if(format == sfz_sample->format){
      pthread_mutex_unlock(sfz_sample_mutex);

      return;	
    }

    ags_stream_free(sfz_sample->buffer);

    sfz_sample->format = format;
    sfz_sample->buffer = ags_stream_alloc(sfz_sample->buffer_size,
					  sfz_sample->format);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    gint loop_start;

    loop_start = g_value_get_uint(value);

    pthread_mutex_lock(sfz_sample_mutex);

    if(loop_start == sfz_sample->loop_start){
      pthread_mutex_unlock(sfz_sample_mutex);

      return;	
    }
    
    sfz_sample->loop_start = loop_start;

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    gint loop_end;

    loop_end = g_value_get_uint(value);

    pthread_mutex_lock(sfz_sample_mutex);

    if(loop_end == sfz_sample->loop_end){
      pthread_mutex_unlock(sfz_sample_mutex);

      return;	
    }
    
    sfz_sample->loop_end = loop_end;

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_GROUP:
  {
    GObject *group;

    group = g_value_get_object(value);

    pthread_mutex_lock(sfz_sample_mutex);

    if(group == sfz_sample->group){
      pthread_mutex_unlock(sfz_sample_mutex);

      return;	
    }

    if(sfz_sample->group == NULL){
      g_object_unref(sfz_sample->group);
    }

    if(group != NULL){
      g_object_ref(group);
    }
    
    sfz_sample->group = group;

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_REGION:
  {
    GObject *region;

    region = g_value_get_object(value);

    pthread_mutex_lock(sfz_sample_mutex);

    if(region == sfz_sample->region){
      pthread_mutex_unlock(sfz_sample_mutex);

      return;	
    }

    if(sfz_sample->region == NULL){
      g_object_unref(sfz_sample->region);
    }

    if(region != NULL){
      g_object_ref(region);
    }
    
    sfz_sample->region = region;

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sfz_sample_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsSFZSample *sfz_sample;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = (AgsSFZSample *) gobject;

  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);
  
  switch(prop_id){
  case PROP_BUFFER_SIZE:
  {
    pthread_mutex_lock(sfz_sample_mutex);

    g_value_set_uint(value, sfz_sample->buffer_size);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    pthread_mutex_lock(sfz_sample_mutex);

    g_value_set_uint(value, sfz_sample->format);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_LOOP_START:
  {
    pthread_mutex_lock(sfz_sample_mutex);

    g_value_set_uint(value, sfz_sample->loop_start);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_LOOP_END:
  {
    pthread_mutex_lock(sfz_sample_mutex);

    g_value_set_uint(value, sfz_sample->loop_end);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_GROUP:
  {
    pthread_mutex_lock(sfz_sample_mutex);

    g_value_set_object(value, sfz_sample->group);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  case PROP_REGION:
  {
    pthread_mutex_lock(sfz_sample_mutex);

    g_value_set_object(value, sfz_sample->region);

    pthread_mutex_unlock(sfz_sample_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_sfz_sample_dispose(GObject *gobject)
{
  AgsSFZSample *sfz_sample;

  sfz_sample = AGS_SFZ_SAMPLE(gobject);

  if(sfz_sample->region != NULL){
    g_object_unref(sfz_sample->region);

    sfz_sample->region = NULL;
  }

  if(sfz_sample->group != NULL){
    g_object_unref(sfz_sample->group);

    sfz_sample->group = NULL;
  }

  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_sample_parent_class)->dispose(gobject);
}

void
ags_sfz_sample_finalize(GObject *gobject)
{
  AgsSFZSample *sfz_sample;

  sfz_sample = AGS_SFZ_SAMPLE(gobject);

  pthread_mutex_destroy(sfz_sample->obj_mutex);
  free(sfz_sample->obj_mutex);

  pthread_mutexattr_destroy(sfz_sample->obj_mutexattr);
  free(sfz_sample->obj_mutexattr);

  ags_stream_free(sfz_sample->buffer);

  if(sfz_sample->region != NULL){
    g_object_unref(sfz_sample->region);
  }
  
  if(sfz_sample->group != NULL){
    g_object_unref(sfz_sample->group);
  }
  
  /* call parent */  
  G_OBJECT_CLASS(ags_sfz_sample_parent_class)->finalize(gobject);
}

AgsUUID*
ags_sfz_sample_get_uuid(AgsConnectable *connectable)
{
  AgsSFZSample *sfz_sample;
  
  AgsUUID *ptr;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(connectable);

  /* get audio file mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* get UUID */
  pthread_mutex_lock(sfz_sample_mutex);

  ptr = sfz_sample->uuid;

  pthread_mutex_unlock(sfz_sample_mutex);
  
  return(ptr);
}

gboolean
ags_sfz_sample_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_sfz_sample_is_ready(AgsConnectable *connectable)
{
  AgsSFZSample *sfz_sample;
  
  gboolean is_ready;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(connectable);

  /* get audio file mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* check is ready */
  pthread_mutex_lock(sfz_sample_mutex);
  
  is_ready = (((AGS_SFZ_SAMPLE_ADDED_TO_REGISTRY & (sfz_sample->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(sfz_sample_mutex);

  return(is_ready);
}

void
ags_sfz_sample_add_to_registry(AgsConnectable *connectable)
{
  AgsSFZSample *sfz_sample;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }

  sfz_sample = AGS_SFZ_SAMPLE(connectable);

  ags_sfz_sample_set_flags(sfz_sample, AGS_SFZ_SAMPLE_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);
    g_value_set_object(entry->entry,
		       (gpointer) sfz_sample);
    ags_registry_add_entry(registry,
			   entry);
  }  
}

void
ags_sfz_sample_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_sfz_sample_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_sfz_sample_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_sfz_sample_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me  
}

gboolean
ags_sfz_sample_is_connected(AgsConnectable *connectable)
{
  AgsSFZSample *sfz_sample;
  
  gboolean is_connected;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(connectable);

  /* get audio file mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* check is connected */
  pthread_mutex_lock(sfz_sample_mutex);

  is_connected = (((AGS_SFZ_SAMPLE_CONNECTED & (sfz_sample->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(sfz_sample_mutex);

  return(is_connected);
}

void
ags_sfz_sample_connect(AgsConnectable *connectable)
{
  AgsSFZSample *sfz_sample;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_sample = AGS_SFZ_SAMPLE(connectable);
  
  ags_sfz_sample_set_flags(sfz_sample, AGS_SFZ_SAMPLE_CONNECTED);
}

void
ags_sfz_sample_disconnect(AgsConnectable *connectable)
{
  AgsSFZSample *sfz_sample;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  sfz_sample = AGS_SFZ_SAMPLE(connectable);

  ags_sfz_sample_unset_flags(sfz_sample, AGS_SFZ_SAMPLE_CONNECTED);
}

gboolean
ags_sfz_sample_open(AgsSoundResource *sound_resource,
		    gchar *filename)
{
  AgsSFZSample *sfz_sample;

  guint format;
  
  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* info */
  pthread_mutex_lock(sfz_sample_mutex);

  if(sfz_sample->info != NULL){
    pthread_mutex_unlock(sfz_sample_mutex);
    
    return(FALSE);
  }

  sfz_sample->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  sfz_sample->info->format = 0;
  sfz_sample->info->channels = 0;
  sfz_sample->info->samplerate = 0;

  pthread_mutex_unlock(sfz_sample_mutex);
  
  if(filename != NULL){
    sfz_sample->file = (SNDFILE *) sf_open(filename, SFM_READ, sfz_sample->info);
  }
  
  if(sfz_sample->file == NULL){
    return(FALSE);
  }

  format = AGS_SOUNDCARD_DOUBLE;

  switch(((SF_FORMAT_PCM_S8 |
	   SF_FORMAT_PCM_16 |
	   SF_FORMAT_PCM_24 |
	   SF_FORMAT_PCM_32 |
	   SF_FORMAT_FLOAT |
	   SF_FORMAT_DOUBLE) & sfz_sample->info->format)){
  case SF_FORMAT_PCM_S8:
  {
    format = AGS_SOUNDCARD_SIGNED_8_BIT;
  }
  break;
  case SF_FORMAT_PCM_16:
  {
    format = AGS_SOUNDCARD_SIGNED_16_BIT;
  }
  break;
  case SF_FORMAT_PCM_24:
  {
    format = AGS_SOUNDCARD_SIGNED_24_BIT;
  }
  break;
  case SF_FORMAT_PCM_32:
  {
    format = AGS_SOUNDCARD_SIGNED_32_BIT;
  }
  break;
  case SF_FORMAT_FLOAT:
  {
    format = AGS_SOUNDCARD_FLOAT;
  }
  break;
  case SF_FORMAT_DOUBLE:
  {
    format = AGS_SOUNDCARD_DOUBLE;
  }
  break;
  }
  
  g_object_set(sfz_sample,
	       "audio-channels", sfz_sample->info->channels,
	       "format", format,
	       NULL);
  
#ifdef AGS_DEBUG
  g_message("ags_sfz_sample_open(): channels %d frames %d", sfz_sample->info->channels, sfz_sample->info->frames);
#endif

  return(TRUE);
}

gboolean
ags_sfz_sample_rw_open(AgsSoundResource *sound_resource,
		       gchar *filename,
		       guint audio_channels, guint samplerate,
		       gboolean create)
{
  AgsSFZSample *sfz_sample;
  
  guint major_format;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* info */
  pthread_mutex_lock(sfz_sample_mutex);

  if(sfz_sample->info != NULL){
    pthread_mutex_unlock(sfz_sample_mutex);
    
    return(FALSE);
  }

  if(!create &&
     !g_file_test(filename,
		  (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))){
    pthread_mutex_unlock(sfz_sample_mutex);

    return(FALSE);
  }  

  sfz_sample->info = (SF_INFO *) malloc(sizeof(SF_INFO));
  memset(sfz_sample->info, 0, sizeof(SF_INFO));

  sfz_sample->info->samplerate = (int) samplerate;
  sfz_sample->info->channels = (int) audio_channels;

  pthread_mutex_unlock(sfz_sample_mutex);

  if(g_str_has_suffix(filename, ".wav")){
    major_format = SF_FORMAT_WAV;

    sfz_sample->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sfz_sample,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".flac")){    
    major_format = SF_FORMAT_FLAC;

    sfz_sample->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sfz_sample,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".aiff")){    
    major_format = SF_FORMAT_AIFF;

    sfz_sample->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sfz_sample,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }else if(g_str_has_suffix(filename, ".ogg")){
    major_format = SF_FORMAT_OGG;

    sfz_sample->info->format = major_format | SF_FORMAT_VORBIS;

    g_object_set(sfz_sample,
		 "format", AGS_SOUNDCARD_DOUBLE,
		 NULL);
  }else{
    major_format = SF_FORMAT_WAV;

    sfz_sample->info->format = major_format | SF_FORMAT_PCM_16;

    g_object_set(sfz_sample,
		 "format", AGS_SOUNDCARD_SIGNED_16_BIT,
		 NULL);
  }
  
  sfz_sample->info->frames = 0;
  sfz_sample->info->seekable = 0;
  sfz_sample->info->sections = 0;

  if(!sf_format_check(sfz_sample->info)){
    g_warning("invalid format");
  }

  if(filename != NULL){
    sfz_sample->file = (SNDFILE *) sf_open(filename, SFM_RDWR, sfz_sample->info);
  }

  g_object_set(sfz_sample,
	       "audio-channels", audio_channels,
	       NULL);

  if(sfz_sample->file == NULL){
    return(FALSE);
  }

#ifdef AGS_DEBUG
  g_message("ags_sfz_sample_rw_open(): channels %d frames %d", sfz_sample->info->channels, sfz_sample->info->frames);
#endif

  return(TRUE);
}

gboolean
ags_sfz_sample_info(AgsSoundResource *sound_resource,
		    guint *frame_count,
		    guint *loop_start, guint *loop_end)
{
  AgsSFZSample *sfz_sample;

  guint sample_frame_count;
  guint sample_loop_start, sample_loop_end;
  
  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* info */
  sample_frame_count = 0;

  pthread_mutex_lock(sfz_sample_mutex);

  if(sfz_sample->info != NULL){
    sample_frame_count = sfz_sample->info->frames;
  }
  
  pthread_mutex_unlock(sfz_sample_mutex);
  
  g_object_get(sfz_sample,
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
ags_sfz_sample_set_presets(AgsSoundResource *sound_resource,
			   guint channels,
			   guint samplerate,
			   guint buffer_size,
			   guint format)
{
  AgsSFZSample *sfz_sample;

  gint sample_format;
  
  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  g_object_set(sfz_sample,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
  
  if(sfz_sample->info == NULL){
    return;
  }

  sfz_sample->info->channels = channels;
  sfz_sample->info->samplerate = samplerate;
  sfz_sample->info->format &= (~(SF_FORMAT_PCM_S8 |
				 SF_FORMAT_PCM_16 |
				 SF_FORMAT_PCM_24 |
				 SF_FORMAT_PCM_32 |
				 SF_FORMAT_FLOAT |
				 SF_FORMAT_DOUBLE));
  
  switch(format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      sfz_sample->info->format |= SF_FORMAT_PCM_S8;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      sfz_sample->info->format |= SF_FORMAT_PCM_16;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      sfz_sample->info->format |= SF_FORMAT_PCM_24;
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      sfz_sample->info->format |= SF_FORMAT_PCM_32;
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      sfz_sample->info->format |= SF_FORMAT_FLOAT;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      sfz_sample->info->format |= SF_FORMAT_DOUBLE;
    }
    break;
  }
}

void
ags_sfz_sample_get_presets(AgsSoundResource *sound_resource,
			   guint *channels,
			   guint *samplerate,
			   guint *buffer_size,
			   guint *format)
{
  AgsSFZSample *sfz_sample;

  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);
  
  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  if(sfz_sample->info == NULL){
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
  
  pthread_mutex_lock(sfz_sample_mutex);

  if(channels != NULL){
    *channels = sfz_sample->info->channels;
  }

  if(samplerate != NULL){
    *samplerate = sfz_sample->info->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = sfz_sample->buffer_size;
  }

  if(format != NULL){
    switch(((SF_FORMAT_PCM_S8 |
	     SF_FORMAT_PCM_16 |
	     SF_FORMAT_PCM_24 |
	     SF_FORMAT_PCM_32 |
	     SF_FORMAT_FLOAT |
	     SF_FORMAT_DOUBLE) & sfz_sample->info->format)){
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

  pthread_mutex_unlock(sfz_sample_mutex);
}

guint
ags_sfz_sample_read(AgsSoundResource *sound_resource,
		    void *dbuffer, guint daudio_channels,
		    guint audio_channel,
		    guint frame_count, guint format)
{
  AgsSFZSample *sfz_sample;

  sf_count_t multi_frames;
  guint total_frame_count;
  guint read_count;
  guint copy_mode;
  gboolean use_cache;
  guint i;

  pthread_mutex_t *sfz_sample_mutex;
  
  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz_sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);
  
  pthread_mutex_lock(sfz_sample_mutex);

  if(sfz_sample->offset >= total_frame_count){
    pthread_mutex_unlock(sfz_sample_mutex);
    
    return(0);
  }

  if(sfz_sample->offset + frame_count >= total_frame_count){
    if(total_frame_count > sfz_sample->offset){
      frame_count = total_frame_count - sfz_sample->offset;
    }else{
      pthread_mutex_unlock(sfz_sample_mutex);
    
      return(0);
    }
  }

#if 0
  use_cache = FALSE;

  if(sfz_sample->buffer_offset == sfz_sample->offset &&
     frame_count <= sfz_sample->buffer_size){
    use_cache = TRUE;
  }
#endif
  
  sfz_sample->buffer_offset = sfz_sample->offset;
  
  read_count = sfz_sample->buffer_size;

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(sfz_sample->format));
  
  pthread_mutex_unlock(sfz_sample_mutex);
    
  for(i = 0; i < frame_count && sfz_sample->offset + i < total_frame_count; ){
    sf_count_t retval;
    
    if(sfz_sample->offset + frame_count > total_frame_count){
      read_count = total_frame_count - sfz_sample->offset;
    }

    multi_frames = read_count * sfz_sample->info->channels;

    retval = -1;
    
    //    if(!use_cache){
    //      g_message("read %d %d", sfz_sample->offset, sfz_sample->buffer_size);
      
      switch(sfz_sample->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  //TODO:JK: implement me
	  retval = 0;
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  retval = sf_read_short(sfz_sample->file, sfz_sample->buffer, multi_frames);
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
	  retval = sf_read_float(sfz_sample->file, sfz_sample->buffer, multi_frames);
	}
	break;
      case AGS_SOUNDCARD_DOUBLE:
	{
	  retval = sf_read_double(sfz_sample->file, sfz_sample->buffer, multi_frames);
	}
	break;
      }

      sfz_sample->offset += read_count;
      
      if(retval == -1){
	g_warning("read failed");
      }

      if(retval != multi_frames){
	break;
      }    
      //    }

    ags_audio_buffer_util_copy_buffer_to_buffer(dbuffer, daudio_channels, (i * daudio_channels),
						sfz_sample->buffer, sfz_sample->info->channels, audio_channel,
						read_count, copy_mode);
    //    g_message("[%d] %d", audio_channel, ags_synth_util_get_xcross_count_s16(dbuffer, read_count));
    
    i += read_count;
  }

  return(frame_count);
}

void
ags_sfz_sample_write(AgsSoundResource *sound_resource,
		     void *sbuffer, guint saudio_channels,
		     guint audio_channel,
		     guint frame_count, guint format)
{
  AgsSFZSample *sfz_sample;

  guint copy_mode;
  sf_count_t multi_frames;
  guint i;
  gboolean do_write;
  
  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz_sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);
  
  pthread_mutex_lock(sfz_sample_mutex);

  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(sfz_sample->format),
						  ags_audio_buffer_util_format_from_soundcard(format));
  
  ags_audio_buffer_util_copy_buffer_to_buffer(sfz_sample->buffer, sfz_sample->info->channels, audio_channel,
					      sbuffer, saudio_channels, audio_channel,
					      frame_count, copy_mode);

  sfz_sample->audio_channel_written[audio_channel] = frame_count;
  do_write = TRUE;

  for(i = 0; i < sfz_sample->audio_channels; i++){
    if(sfz_sample->audio_channel_written[i] == -1){
      do_write = FALSE;
      
      break;
    }
  }
  
  pthread_mutex_unlock(sfz_sample_mutex);

  if(do_write){
    multi_frames = frame_count * sfz_sample->info->channels;

    switch(sfz_sample->format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	//TODO:JK: implement me
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	sf_write_short(sfz_sample->file, sfz_sample->buffer, multi_frames);
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
	sf_write_float(sfz_sample->file, sfz_sample->buffer, multi_frames);
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	sf_write_double(sfz_sample->file, sfz_sample->buffer, multi_frames);
      }
      break;
    }
    
    for(i = 0; i < sfz_sample->audio_channels; i++){
      sfz_sample->audio_channel_written[i] = -1;
    }

    if(sfz_sample->format == AGS_SOUNDCARD_DOUBLE){
      ags_audio_buffer_util_clear_double(sfz_sample->buffer, sfz_sample->info->channels,
					 frame_count);
    }else if(sfz_sample->format == AGS_SOUNDCARD_FLOAT){
      ags_audio_buffer_util_clear_float(sfz_sample->buffer, sfz_sample->info->channels,
					frame_count);
    }else{
      ags_audio_buffer_util_clear_buffer(sfz_sample->buffer, sfz_sample->info->channels,
					 frame_count, ags_audio_buffer_util_format_from_soundcard(sfz_sample->format));
    }
    
    sfz_sample->offset += frame_count;
  }
}

void
ags_sfz_sample_flush(AgsSoundResource *sound_resource)
{
  AgsSFZSample *sfz_sample;
   
  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  if(sfz_sample->file == NULL){
    return;
  }
  
  sf_write_sync(sfz_sample->file);
}

void
ags_sfz_sample_seek(AgsSoundResource *sound_resource,
		    gint64 frame_count, gint whence)
{
  AgsSFZSample *sfz_sample;

  guint total_frame_count;
  sf_count_t retval;
  
  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz_sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  ags_sound_resource_info(sound_resource,
			  &total_frame_count,
			  NULL, NULL);

  pthread_mutex_lock(sfz_sample_mutex);

  if(whence == G_SEEK_CUR){
    if(frame_count >= 0){
      if(sfz_sample->offset + frame_count < total_frame_count){
	sfz_sample->offset += total_frame_count;
      }else{
	sfz_sample->offset = total_frame_count;
      }
    }else{
      if(sfz_sample->offset + frame_count >= 0){
	sfz_sample->offset += total_frame_count;
      }else{
	sfz_sample->offset = 0;
      }
    } 
  }else if(whence == G_SEEK_SET){
    if(frame_count >= 0){
      if(frame_count < total_frame_count){
	sfz_sample->offset = frame_count;
      }else{
	sfz_sample->offset = total_frame_count;
      }
    }else{
      sfz_sample->offset = 0;
    }
  }else if(whence == G_SEEK_END){
    if(frame_count > 0){
      sfz_sample->offset = total_frame_count;
    }else{
      if(total_frame_count + frame_count > 0){
	sfz_sample->offset = total_frame_count + total_frame_count;
      }else{
	sfz_sample->offset = 0;
      }
    }
  }

  pthread_mutex_unlock(sfz_sample_mutex);

  retval = sf_seek(sfz_sample->file, sfz_sample->offset, SEEK_SET);

  if(retval == -1){
    g_warning("seek failed");
  }
}

void
ags_sfz_sample_close(AgsSoundResource *sound_resource)
{
  AgsSFZSample *sfz_sample;
   
  pthread_mutex_t *sfz_sample_mutex;

  sfz_sample = AGS_SFZ_SAMPLE(sound_resource);

  /* get sfz sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  if(sfz_sample->file == NULL){
    return;
  }

  sf_close(sfz_sample->file);

  pthread_mutex_lock(sfz_sample_mutex);

  if(sfz_sample->info != NULL){
    free(sfz_sample->info);
  }

  sfz_sample->file = NULL;
  sfz_sample->info = NULL;

  pthread_mutex_unlock(sfz_sample_mutex);
}

/**
 * ags_sfz_sample_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.3.0
 */
pthread_mutex_t*
ags_sfz_sample_get_class_mutex()
{
  return(&ags_sfz_sample_class_mutex);
}

/**
 * ags_sfz_sample_test_flags:
 * @sfz_sample: the #AgsSFZSample
 * @flags: the flags
 *
 * Test @flags to be set on @sfz_sample.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.3.0
 */
gboolean
ags_sfz_sample_test_flags(AgsSFZSample *sfz_sample, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *sfz_sample_mutex;

  if(!AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return(FALSE);
  }

  /* get sfz_sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  /* test */
  pthread_mutex_lock(sfz_sample_mutex);

  retval = (flags & (sfz_sample->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(sfz_sample_mutex);

  return(retval);
}

/**
 * ags_sfz_sample_set_flags:
 * @sfz_sample: the #AgsSFZSample
 * @flags: see #AgsSFZSampleFlags-enum
 *
 * Enable a feature of @sfz_sample.
 *
 * Since: 2.3.0
 */
void
ags_sfz_sample_set_flags(AgsSFZSample *sfz_sample, guint flags)
{
  pthread_mutex_t *sfz_sample_mutex;

  if(!AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  /* get sfz_sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(sfz_sample_mutex);

  sfz_sample->flags |= flags;
  
  pthread_mutex_unlock(sfz_sample_mutex);
}
    
/**
 * ags_sfz_sample_unset_flags:
 * @sfz_sample: the #AgsSFZSample
 * @flags: see #AgsSFZSampleFlags-enum
 *
 * Disable a feature of @sfz_sample.
 *
 * Since: 2.3.0
 */
void
ags_sfz_sample_unset_flags(AgsSFZSample *sfz_sample, guint flags)
{  
  pthread_mutex_t *sfz_sample_mutex;

  if(!AGS_IS_SFZ_SAMPLE(sfz_sample)){
    return;
  }

  /* get sfz_sample mutex */
  sfz_sample_mutex = AGS_SFZ_SAMPLE_GET_OBJ_MUTEX(sfz_sample);

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(sfz_sample_mutex);

  sfz_sample->flags &= (~flags);
  
  pthread_mutex_unlock(sfz_sample_mutex);
}

/**
 * ags_sfz_sample_new:
 *
 * Creates a new instance of #AgsSFZSample.
 *
 * Returns: the new #AgsSFZSample.
 *
 * Since: 2.3.0
 */
AgsSFZSample*
ags_sfz_sample_new()
{
  AgsSFZSample *sfz_sample;

  sfz_sample = (AgsSFZSample *) g_object_new(AGS_TYPE_SFZ_SAMPLE,
					     NULL);

  return(sfz_sample);
}
