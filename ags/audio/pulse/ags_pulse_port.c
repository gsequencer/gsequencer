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

#include <ags/audio/pulse/ags_pulse_port.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_notify_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/config.h>
#include <ags/i18n.h>

#include <time.h>

void ags_pulse_port_class_init(AgsPulsePortClass *pulse_port);
void ags_pulse_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_port_init(AgsPulsePort *pulse_port);
void ags_pulse_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_pulse_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_pulse_port_dispose(GObject *gobject);
void ags_pulse_port_finalize(GObject *gobject);

AgsUUID* ags_pulse_port_get_uuid(AgsConnectable *connectable);
gboolean ags_pulse_port_has_resource(AgsConnectable *connectable);
gboolean ags_pulse_port_is_ready(AgsConnectable *connectable);
void ags_pulse_port_add_to_registry(AgsConnectable *connectable);
void ags_pulse_port_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pulse_port_list_resource(AgsConnectable *connectable);
xmlNode* ags_pulse_port_xml_compose(AgsConnectable *connectable);
void ags_pulse_port_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_pulse_port_is_connected(AgsConnectable *connectable);
void ags_pulse_port_connect(AgsConnectable *connectable);
void ags_pulse_port_disconnect(AgsConnectable *connectable);

#ifdef AGS_WITH_PULSE
void ags_pulse_port_cached_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port);
void ags_pulse_port_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port);
void ags_pulse_port_stream_underflow_callback(pa_stream *stream, AgsPulsePort *pulse_port);
#endif

/**
 * SECTION:ags_pulse_port
 * @short_description: pulseaudio resource.
 * @title: AgsPulsePort
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_port.h
 *
 * The #AgsPulsePort represents either a pulseaudio sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_PULSE_CLIENT,
  PROP_PULSE_DEVOUT,
  PROP_PULSE_DEVIN,
  PROP_PORT_NAME,
};

static gpointer ags_pulse_port_parent_class = NULL;

static pthread_mutex_t ags_pulse_port_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_pulse_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pulse_port = 0;

    static const GTypeInfo ags_pulse_port_info = {
      sizeof(AgsPulsePortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPulsePort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pulse_port = g_type_register_static(G_TYPE_OBJECT,
						 "AgsPulsePort",
						 &ags_pulse_port_info,
						 0);

    g_type_add_interface_static(ags_type_pulse_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pulse_port);
  }

  return g_define_type_id__volatile;
}

void
ags_pulse_port_class_init(AgsPulsePortClass *pulse_port)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_pulse_port_parent_class = g_type_class_peek_parent(pulse_port);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_port;

  gobject->set_property = ags_pulse_port_set_property;
  gobject->get_property = ags_pulse_port_get_property;

  gobject->dispose = ags_pulse_port_dispose;
  gobject->finalize = ags_pulse_port_finalize;

  /* properties */
  /**
   * AgsPulsePort:pulse-client:
   *
   * The assigned #AgsPulseClient.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("pulse-client",
				   i18n_pspec("assigned pulseaudio client"),
				   i18n_pspec("The assigned pulseaudio client"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulsePort:pulse-devout:
   *
   * The assigned #AgsPulseDevout.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("pulse-devout",
				   i18n_pspec("assigned pulseaudio devout"),
				   i18n_pspec("The assigned pulseaudio devout"),
				   AGS_TYPE_PULSE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_DEVOUT,
				  param_spec);

  /**
   * AgsPulsePort:pulse-devin:
   *
   * The assigned #AgsPulseDevout.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("pulse-devin",
				   i18n_pspec("assigned pulseaudio devin"),
				   i18n_pspec("The assigned pulseaudio devin"),
				   AGS_TYPE_PULSE_DEVIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_DEVIN,
				  param_spec);

  /**
   * AgsPulsePort:port-name:
   *
   * The pulse soundcard indentifier
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("port-name",
				   i18n_pspec("port name"),
				   i18n_pspec("The port name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
}

void
ags_pulse_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pulse_port_get_uuid;
  connectable->has_resource = ags_pulse_port_has_resource;

  connectable->is_ready = ags_pulse_port_is_ready;
  connectable->add_to_registry = ags_pulse_port_add_to_registry;
  connectable->remove_from_registry = ags_pulse_port_remove_from_registry;

  connectable->list_resource = ags_pulse_port_list_resource;
  connectable->xml_compose = ags_pulse_port_xml_compose;
  connectable->xml_parse = ags_pulse_port_xml_parse;

  connectable->is_connected = ags_pulse_port_is_connected;  
  connectable->connect = ags_pulse_port_connect;
  connectable->disconnect = ags_pulse_port_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pulse_port_init(AgsPulsePort *pulse_port)
{
  AgsConfig *config;
  
  gchar *str;

  guint word_size;
  guint fixed_size;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  pulse_port->flags = 0;

  /* port mutex */
  pulse_port->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  pulse_port->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  pulse_port->pulse_client = NULL;

  /* uuid */
  pulse_port->uuid = ags_uuid_alloc();
  ags_uuid_generate(pulse_port->uuid);

  /*  */
  pulse_port->pulse_devout = NULL;
  pulse_port->pulse_devin = NULL;
  
  pulse_port->port_uuid = ags_id_generator_create_uuid();
  pulse_port->port_name = NULL;

  pulse_port->stream = NULL;
  
  /* presets */
  config = ags_config_get_instance();

  pulse_port->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  pulse_port->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  pulse_port->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  pulse_port->format = ags_soundcard_helper_config_get_format(config);

  pulse_port->use_cache = TRUE;
  pulse_port->cache_buffer_size = AGS_PULSE_PORT_DEFAULT_CACHE_BUFFER_SIZE;

  pulse_port->current_cache = 0;
  pulse_port->completed_cache = 0;
  pulse_port->cache_offset = 0;

  pulse_port->cache = (void **) malloc(4 * sizeof(void *));
  
#ifdef AGS_WITH_PULSE
  pulse_port->sample_spec = (pa_sample_spec *) malloc(sizeof(pa_sample_spec));
  pulse_port->sample_spec->rate = pulse_port->samplerate;
  pulse_port->sample_spec->channels = pulse_port->pcm_channels;
#else
  pulse_port->sample_spec = NULL;
#endif

  word_size = 0;
  
  switch(pulse_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
#ifdef AGS_WITH_PULSE
      if(ags_endian_host_is_be()){
	pulse_port->sample_spec->format = PA_SAMPLE_S16BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S16LE;
      }
#endif

      pulse_port->cache[0] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint16));
      pulse_port->cache[1] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint16));
      pulse_port->cache[2] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint16));
      pulse_port->cache[3] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint16));

      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
#ifdef AGS_WITH_PULSE
      if(ags_endian_host_is_be()){
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32LE;
      }
#endif

      pulse_port->cache[0] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));

      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
#ifdef AGS_WITH_PULSE
      if(ags_endian_host_is_be()){
	pulse_port->sample_spec->format = PA_SAMPLE_S32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S32LE;
      }
#endif

      pulse_port->cache[0] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) malloc(pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));

      word_size = sizeof(gint32);
    }
    break;
  default:
    g_warning("pulse devout/devin - unsupported format");
  }
  
  fixed_size = pulse_port->pcm_channels * pulse_port->cache_buffer_size * word_size;

  memset(pulse_port->cache[0], 0, pulse_port->pcm_channels * pulse_port->cache_buffer_size * word_size);
  memset(pulse_port->cache[1], 0, pulse_port->pcm_channels * pulse_port->cache_buffer_size * word_size);
  memset(pulse_port->cache[2], 0, pulse_port->pcm_channels * pulse_port->cache_buffer_size * word_size);
  memset(pulse_port->cache[3], 0, pulse_port->pcm_channels * pulse_port->cache_buffer_size * word_size);

#ifdef AGS_WITH_PULSE
  pulse_port->buffer_attr = (pa_buffer_attr *) malloc(sizeof(pa_buffer_attr));
  pulse_port->buffer_attr->fragsize = (uint32_t) -1;
  pulse_port->buffer_attr->maxlength = (uint32_t) -1;
  pulse_port->buffer_attr->minreq = (uint32_t) fixed_size;
  pulse_port->buffer_attr->prebuf = (uint32_t) 0;
  pulse_port->buffer_attr->tlength = (uint32_t) fixed_size;
#else
  pulse_port->buffer_attr = NULL;
#endif

  pulse_port->empty_buffer = ags_stream_alloc(8 * pulse_port->pcm_channels * pulse_port->buffer_size,
					      AGS_SOUNDCARD_DEFAULT_FORMAT);

  g_atomic_int_set(&(pulse_port->is_empty),
		   TRUE);
  g_atomic_int_set(&(pulse_port->underflow),
		   0);
  g_atomic_int_set(&(pulse_port->restart),
		   FALSE);
  
  pulse_port->nth_empty_buffer = 0;
  
  g_atomic_int_set(&(pulse_port->queued),
		   0);
}

void
ags_pulse_port_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsPulsePort *pulse_port;

  pthread_mutex_t *pulse_port_mutex;

  pulse_port = AGS_PULSE_PORT(gobject);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  switch(prop_id){
  case PROP_PULSE_CLIENT:
    {
      AgsPulseClient *pulse_client;

      pulse_client = (AgsPulseClient *) g_value_get_object(value);

      pthread_mutex_lock(pulse_port_mutex);

      if(pulse_port->pulse_client == (GObject *) pulse_client){
	pthread_mutex_unlock(pulse_port_mutex);
	
	return;
      }

      if(pulse_port->pulse_client != NULL){
	g_object_unref(pulse_port->pulse_client);
      }

      if(pulse_client != NULL){
	g_object_ref(pulse_client);
      }
      
      pulse_port->pulse_client = (GObject *) pulse_client;

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  case PROP_PULSE_DEVOUT:
    {
      AgsPulseDevout *pulse_devout;

      pulse_devout = (AgsPulseDevout *) g_value_get_object(value);

      pthread_mutex_lock(pulse_port_mutex);

      if(pulse_port->pulse_devout == (GObject *) pulse_devout){
	pthread_mutex_unlock(pulse_port_mutex);
	
	return;
      }

      if(pulse_port->pulse_devout != NULL){
	g_object_unref(pulse_port->pulse_devout);
      }

      if(pulse_devout != NULL){
	g_object_ref(pulse_devout);
      }
      
      pulse_port->pulse_devout = (GObject *) pulse_devout;

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  case PROP_PULSE_DEVIN:
    {
      AgsPulseDevin *pulse_devin;

      pulse_devin = (AgsPulseDevin *) g_value_get_object(value);

      pthread_mutex_lock(pulse_port_mutex);

      if(pulse_port->pulse_devin == (GObject *) pulse_devin){
	pthread_mutex_unlock(pulse_port_mutex);
	
	return;
      }

      if(pulse_port->pulse_devin != NULL){
	g_object_unref(pulse_port->pulse_devin);
      }

      if(pulse_devin != NULL){
	g_object_ref(pulse_devin);
      }
      
      pulse_port->pulse_devin = (GObject *) pulse_devin;

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      pthread_mutex_lock(pulse_port_mutex);

      if(pulse_port->port_name == port_name ||
	 !g_ascii_strcasecmp(pulse_port->port_name,
			     port_name)){
	pthread_mutex_unlock(pulse_port_mutex);
	
	return;
      }

      if(pulse_port->port_name != NULL){
	g_free(pulse_port->port_name);
      }

      pulse_port->port_name = port_name;

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_port_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsPulsePort *pulse_port;

  pthread_mutex_t *pulse_port_mutex;

  pulse_port = AGS_PULSE_PORT(gobject);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
  
  switch(prop_id){
  case PROP_PULSE_CLIENT:
    {
      pthread_mutex_lock(pulse_port_mutex);

      g_value_set_object(value, pulse_port->pulse_client);

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  case PROP_PULSE_DEVOUT:
    {
      pthread_mutex_lock(pulse_port_mutex);

      g_value_set_object(value, pulse_port->pulse_devout);

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  case PROP_PULSE_DEVIN:
    {
      pthread_mutex_lock(pulse_port_mutex);

      g_value_set_object(value, pulse_port->pulse_devin);

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      pthread_mutex_lock(pulse_port_mutex);

      g_value_set_string(value, pulse_port->port_name);

      pthread_mutex_unlock(pulse_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_port_dispose(GObject *gobject)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  /* pulse client */
  if(pulse_port->pulse_client != NULL){
    g_object_unref(pulse_port->pulse_client);

    pulse_port->pulse_client = NULL;
  }

  /* pulse devout */
  if(pulse_port->pulse_devout != NULL){
    g_object_unref(pulse_port->pulse_devout);

    pulse_port->pulse_devout = NULL;
  }

  /* pulse devin */
  if(pulse_port->pulse_devin != NULL){
    g_object_unref(pulse_port->pulse_devin);

    pulse_port->pulse_devin = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_port_parent_class)->dispose(gobject);
}

void
ags_pulse_port_finalize(GObject *gobject)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  pthread_mutex_destroy(pulse_port->obj_mutex);
  free(pulse_port->obj_mutex);

  pthread_mutexattr_destroy(pulse_port->obj_mutexattr);
  free(pulse_port->obj_mutexattr);

  /* pulse client */
  if(pulse_port->pulse_client != NULL){
    g_object_unref(pulse_port->pulse_client);
  }

  /* pulse devout */
  if(pulse_port->pulse_devout != NULL){
    g_object_unref(pulse_port->pulse_devout);
  }

  /* pulse devin */
  if(pulse_port->pulse_devin != NULL){
    g_object_unref(pulse_port->pulse_devin);
  }

  /* name */
  g_free(pulse_port->port_name);

  if(pulse_port->sample_spec != NULL){
    free(pulse_port->sample_spec);
  }
  
  if(pulse_port->buffer_attr != NULL){
    free(pulse_port->buffer_attr);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pulse_port_get_uuid(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;
  
  AgsUUID *ptr;

  pthread_mutex_t *pulse_port_mutex;

  pulse_port = AGS_PULSE_PORT(connectable);

  /* get pulse port signal mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* get UUID */
  pthread_mutex_lock(pulse_port_mutex);

  ptr = pulse_port->uuid;

  pthread_mutex_unlock(pulse_port_mutex);
  
  return(ptr);
}

gboolean
ags_pulse_port_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_pulse_port_is_ready(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;
  
  gboolean is_ready;

  pthread_mutex_t *pulse_port_mutex;

  pulse_port = AGS_PULSE_PORT(connectable);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* check is added */
  pthread_mutex_lock(pulse_port_mutex);

  is_ready = (((AGS_PULSE_PORT_ADDED_TO_REGISTRY & (pulse_port->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(pulse_port_mutex);
  
  return(is_ready);
}

void
ags_pulse_port_add_to_registry(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pulse_port = AGS_PULSE_PORT(connectable);

  ags_pulse_port_set_flags(pulse_port, AGS_PULSE_PORT_ADDED_TO_REGISTRY);
}

void
ags_pulse_port_remove_from_registry(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  pulse_port = AGS_PULSE_PORT(connectable);

  ags_pulse_port_unset_flags(pulse_port, AGS_PULSE_PORT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_pulse_port_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pulse_port_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pulse_port_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pulse_port_is_connected(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;
  
  gboolean is_connected;

  pthread_mutex_t *pulse_port_mutex;

  pulse_port = AGS_PULSE_PORT(connectable);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* check is connected */
  pthread_mutex_lock(pulse_port_mutex);

  is_connected = (((AGS_PULSE_PORT_CONNECTED & (pulse_port->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(pulse_port_mutex);
  
  return(is_connected);
}

void
ags_pulse_port_connect(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_port = AGS_PULSE_PORT(connectable);

  ags_pulse_port_set_flags(pulse_port, AGS_PULSE_PORT_CONNECTED);
}

void
ags_pulse_port_disconnect(AgsConnectable *connectable)
{

  AgsPulsePort *pulse_port;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_port = AGS_PULSE_PORT(connectable);
  
  ags_pulse_port_unset_flags(pulse_port, AGS_PULSE_PORT_CONNECTED);
}

/**
 * ags_pulse_port_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_pulse_port_get_class_mutex()
{
  return(&ags_pulse_port_class_mutex);
}

/**
 * ags_pulse_port_test_flags:
 * @pulse_port: the #AgsPulsePort
 * @flags: the flags
 *
 * Test @flags to be set on @pulse_port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_pulse_port_test_flags(AgsPulsePort *pulse_port, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *pulse_port_mutex;

  if(!AGS_IS_PULSE_PORT(pulse_port)){
    return(FALSE);
  }

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* test */
  pthread_mutex_lock(pulse_port_mutex);

  retval = (flags & (pulse_port->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(pulse_port_mutex);

  return(retval);
}

/**
 * ags_pulse_port_set_flags:
 * @pulse_port: the #AgsPulsePort
 * @flags: see #AgsPulsePortFlags-enum
 *
 * Enable a feature of @pulse_port.
 *
 * Since: 2.0.0
 */
void
ags_pulse_port_set_flags(AgsPulsePort *pulse_port, guint flags)
{
  pthread_mutex_t *pulse_port_mutex;

  if(!AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_port->flags |= flags;
  
  pthread_mutex_unlock(pulse_port_mutex);
}
    
/**
 * ags_pulse_port_unset_flags:
 * @pulse_port: the #AgsPulsePort
 * @flags: see #AgsPulsePortFlags-enum
 *
 * Disable a feature of @pulse_port.
 *
 * Since: 2.0.0
 */
void
ags_pulse_port_unset_flags(AgsPulsePort *pulse_port, guint flags)
{  
  pthread_mutex_t *pulse_port_mutex;

  if(!AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_port->flags &= (~flags);
  
  pthread_mutex_unlock(pulse_port_mutex);
}

/**
 * ags_pulse_port_find:
 * @pulse_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @pulse_port.
 *
 * Returns: the next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_pulse_port_find(GList *pulse_port,
		    gchar *port_name)
{
  gboolean success;
  
  pthread_mutex_t *pulse_port_mutex;

  while(pulse_port != NULL){
    /* get pulse port mutex */
    pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port->data);

    /* check port name */
    pthread_mutex_lock(pulse_port_mutex);

    success = (!g_ascii_strcasecmp(AGS_PULSE_PORT(pulse_port->data)->port_name,
				   port_name)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(pulse_port_mutex);

    if(success){
      return(pulse_port);
    }
  }

  return(NULL);
}

/**
 * ags_pulse_port_register:
 * @pulse_port: the #AgsPulsePort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new pulseaudio port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 2.0.0
 */
void
ags_pulse_port_register(AgsPulsePort *pulse_port,
			gchar *port_name,
			gboolean is_audio, gboolean is_midi,
			gboolean is_output)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

#ifdef AGS_WITH_PULSE
  pa_context *context;
  pa_stream *stream;
  pa_sample_spec *sample_spec;
  pa_buffer_attr *buffer_attr;
#else
  gpointer context;
  gpointer stream;
  gpointer sample_spec;
  gpointer buffer_attr;
#endif

  GList *list;

  int r;
  gboolean use_cache;
  
  pthread_mutex_t *pulse_client_mutex;
  pthread_mutex_t *pulse_port_mutex;
  
  if(!AGS_IS_PULSE_PORT(pulse_port) ||
     port_name == NULL){
    return;
  }

  g_object_get(pulse_port,
	       "pulse-client", &pulse_client,
	       NULL);
  
  if(pulse_client == NULL){
    g_warning("ags_pulse_port.c - no assigned AgsPulseClient");
    
    return;
  }

  if(ags_pulse_port_test_flags(pulse_port, AGS_PULSE_PORT_REGISTERED)){
    g_object_unref(pulse_client);
    
    return;
  }

  /* get pulse server and application context */
  g_object_get(pulse_client,
	       "pulse-server", &pulse_server,
	       NULL);
  
  if(pulse_server == NULL){
    g_object_unref(pulse_client);

    return;
  }

  /* get pulse client mutex */
  pulse_client_mutex = AGS_PULSE_CLIENT_GET_OBJ_MUTEX(pulse_client);

  /* get context */
  pthread_mutex_lock(pulse_client_mutex);

  context = pulse_client->context;
  
  pthread_mutex_unlock(pulse_client_mutex);

  if(context == NULL){
    g_object_unref(pulse_client);

    g_object_unref(pulse_server);
    
    return;
  }

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* get port name */
  //FIXME:JK: memory leak?
  pthread_mutex_lock(pulse_port_mutex);

  sample_spec = pulse_port->sample_spec;
  buffer_attr =  pulse_port->buffer_attr;
  
  port_name = g_strdup(pulse_port->port_name);

  use_cache = pulse_port->use_cache;
  
  pthread_mutex_unlock(pulse_port_mutex);

  /* create sequencer or soundcard */
  if(is_output){
    ags_pulse_port_set_flags(pulse_port, AGS_PULSE_PORT_IS_OUTPUT);
  }

#ifdef AGS_WITH_PULSE
  stream = pa_stream_new(context, "Playback", sample_spec, NULL);
#else
  stream = NULL;
#endif

  pthread_mutex_lock(pulse_port_mutex);

  pulse_port->stream = stream;

  pthread_mutex_unlock(pulse_port_mutex);
  
  if(stream == NULL){
    return;
  }

#ifdef AGS_WITH_PULSE
  if(is_audio){
    ags_pulse_port_set_flags(pulse_port, AGS_PULSE_PORT_IS_AUDIO);

    if(use_cache){
      pa_stream_set_write_callback(stream,
				   ags_pulse_port_cached_stream_request_callback,
				   pulse_port);
    }else{
      pa_stream_set_write_callback(stream,
				   ags_pulse_port_stream_request_callback,
				   pulse_port);
    }

    pa_stream_set_underflow_callback(stream,
				     ags_pulse_port_stream_underflow_callback,
				     pulse_port);
    
    r = pa_stream_connect_playback(stream, NULL, buffer_attr,
				   (PA_STREAM_INTERPOLATE_TIMING |
				    PA_STREAM_ADJUST_LATENCY |
				    PA_STREAM_AUTO_TIMING_UPDATE),
				   NULL,
				   NULL);

    if(r < 0){
      // Old pulse audio servers don't like the ADJUST_LATENCY flag, so retry without that
      r = pa_stream_connect_playback(stream, NULL, buffer_attr,
				     (PA_STREAM_INTERPOLATE_TIMING |
				      PA_STREAM_AUTO_TIMING_UPDATE),
				     NULL,
				     NULL);
    }

    if(r < 0){
      return;
    }
  }else if(is_midi){
    ags_pulse_port_set_flags(pulse_port, AGS_PULSE_PORT_IS_MIDI);
    
    //NOTE:JK: not implemented
  }
  
  if(stream != NULL){
    ags_pulse_port_set_flags(pulse_port, AGS_PULSE_PORT_REGISTERED);
  }
#endif

  g_object_unref(pulse_client);

  g_object_unref(pulse_server);
}

void
ags_pulse_port_unregister(AgsPulsePort *pulse_port)
{
#ifdef AGS_WITH_PULSE
  pa_stream *stream;
#else
  gpointer stream;
#endif
  
  pthread_mutex_t *pulse_port_mutex;

  if(!AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* get port */
  pthread_mutex_lock(pulse_port_mutex);

  stream = pulse_port->stream;
  
  pthread_mutex_unlock(pulse_port_mutex);

#ifdef AGS_WITH_PULSE
  if(stream != NULL){
    pa_stream_disconnect(stream);

    /* unset port and flags */
    pthread_mutex_lock(pulse_port_mutex);

    pulse_port->stream = NULL;

    pthread_mutex_unlock(pulse_port_mutex);

    ags_pulse_port_unset_flags(pulse_port, AGS_PULSE_PORT_REGISTERED);
  }
#endif
}

#ifdef AGS_WITH_PULSE
void
ags_pulse_port_cached_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port)
{
  AgsPulseDevout *pulse_devout;
  AgsPulseDevin *pulse_devin;
    
  AgsAudioLoop *audio_loop;

  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;

  guint current_cache;
  guint next_cache, completed_cache;
  guint played_cache;
  guint word_size;
  guint frame_size;
  
  pthread_mutex_t *pulse_port_mutex;

  if(pulse_port == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  if(audio_loop != NULL){
    pthread_mutex_lock(audio_loop->timing_mutex);
  
    g_atomic_int_set(&(audio_loop->time_spent),
		     audio_loop->time_cycle);
  
    pthread_mutex_unlock(audio_loop->timing_mutex);
  }

  g_atomic_int_and(&(AGS_THREAD(audio_loop)->flags),
		   (~(AGS_THREAD_TIMING)));

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /*  */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_devout = (AgsPulseDevout *) pulse_port->pulse_devout;
  pulse_devin = (AgsPulseDevin *) pulse_port->pulse_devin;

  current_cache = pulse_port->current_cache;

  word_size = 0;
  
  switch(pulse_port->format){
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
  }

  frame_size = pulse_port->sample_spec->channels * pulse_port->cache_buffer_size * word_size;

  pthread_mutex_unlock(pulse_port_mutex);

  soundcard = NULL;
  
  if(pulse_devout != NULL){
    soundcard = (GObject *) pulse_devout;
  }else if(pulse_devin != NULL){
    soundcard = (GObject *) pulse_devin;
  }

  if(ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
    if(current_cache == 3){
      next_cache = 0;
    }else{
      next_cache = current_cache + 1;
    }
  }else{
    next_cache = 0;
  }
  
  /* wait until cache ready */
  pthread_mutex_lock(pulse_port_mutex);

  completed_cache = pulse_port->completed_cache;

  pthread_mutex_unlock(pulse_port_mutex);

  if(AGS_IS_PULSE_DEVOUT(soundcard)){
    size_t n_bytes;

    struct timespec idle_time = {
      0,
      0,
    };

    if(ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
      idle_time.tv_nsec = ags_pulse_port_get_latency(pulse_port) / 8;
    
      while(next_cache == completed_cache &&
	    ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
	nanosleep(&idle_time, NULL);

	pthread_mutex_lock(pulse_port_mutex);

	completed_cache = pulse_port->completed_cache;

	pthread_mutex_unlock(pulse_port_mutex);
      }

      if(current_cache == 0){
	played_cache = 3;
      }else{
	played_cache = current_cache - 1;
      }
    }else{
      played_cache = 0;
    }
    
    n_bytes = 0;
    pa_stream_begin_write(stream,
			  &(pulse_port->cache[played_cache]),
			  &n_bytes);

    pa_stream_write(stream,
		    pulse_port->cache[played_cache],
		    frame_size,
		    NULL,
		    0,
		    PA_SEEK_RELATIVE);
  }else{
    if(current_cache == 3){
      played_cache = 0;
    }else{
      played_cache = current_cache + 1;
    }

    pa_stream_peek(stream,
		   &(pulse_port->cache[played_cache]),
		   &frame_size);

    pthread_mutex_lock(pulse_port_mutex);
  }

  /* seek current cache */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_port->current_cache = next_cache;

  pthread_mutex_unlock(pulse_port_mutex);

  /* unref */
  g_object_unref(audio_loop);
  g_object_unref(task_thread);
}

void
ags_pulse_port_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port)
{
  AgsPulseDevout *pulse_devout;
  AgsPulseDevin *pulse_devin;
  
  AgsAudioLoop *audio_loop;

  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  guint word_size;
  size_t count;
  guint nth_buffer;
  guint nth_empty_buffer, next_nth_empty_buffer;
  size_t n_bytes;
  gint remaining;
  guint i;
  
  gboolean no_event;
  gboolean empty_run;
  
  pthread_mutex_t *pulse_port_mutex;
  pthread_mutex_t *device_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
    
  if(pulse_port == NULL){
    return;
  }
  
  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  if(g_atomic_int_get(&(pulse_port->queued)) > 0){
    g_warning("drop pulseaudio callback");
    
    return;
  }else{
    g_atomic_int_inc(&(pulse_port->queued));
  }

  /*
   * process audio
   */
  /*  */  
  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* interrupt GUI */
  if(task_thread != NULL){
    pthread_mutex_lock(task_thread->launch_mutex);
  }
  
  if(audio_loop != NULL){
    pthread_mutex_lock(audio_loop->timing_mutex);
  
    g_atomic_int_set(&(audio_loop->time_spent),
		     audio_loop->time_cycle);
  
    pthread_mutex_unlock(audio_loop->timing_mutex);
  
    //    ags_main_loop_interrupt(AGS_MAIN_LOOP(audio_loop),
    //			    AGS_THREAD_SUSPEND_SIG,
    //			    0, &time_spent);
  }

  if(task_thread != NULL){
    pthread_mutex_unlock(task_thread->launch_mutex);
  }

  g_atomic_int_and(&(AGS_THREAD(audio_loop)->flags),
		   (~(AGS_THREAD_TIMING)));

  /*  */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_devout = (AgsPulseDevout *) pulse_port->pulse_devout;
  pulse_devin = (AgsPulseDevin *) pulse_port->pulse_devin;

  soundcard = NULL;
  
  if(pulse_devout != NULL){
    soundcard = (GObject *) pulse_devout;
  }else if(pulse_devin != NULL){
    soundcard = (GObject *) pulse_devin;
  }
  
  stream = pulse_port->stream;

  nth_empty_buffer = pulse_port->nth_empty_buffer;

  if(nth_empty_buffer >= 7){
    next_nth_empty_buffer = 0;
  }else{
    next_nth_empty_buffer = nth_empty_buffer + 1;
  }
  
  pthread_mutex_unlock(pulse_port_mutex);
  
  /* get device mutex */
  if(AGS_IS_PULSE_DEVOUT(soundcard)){
    device_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    device_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(soundcard);
  }else{
    g_object_unref(audio_loop);
    g_object_unref(task_thread);
    
    return;
  }

  /*  */
  pthread_mutex_lock(device_mutex);

  empty_run = FALSE;

  if((pulse_devout == NULL ||
      !ags_soundcard_is_playing(AGS_SOUNDCARD(pulse_devout))) &&
     (pulse_devin == NULL ||
      !ags_soundcard_is_recording(AGS_SOUNDCARD(pulse_devin)))){
    empty_run = TRUE;
  }
  
  pthread_mutex_unlock(device_mutex);

  /* check buffer flag */
  pthread_mutex_lock(pulse_port_mutex);

  word_size = 0;
  
  switch(pulse_port->format){
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
  }
        
  count = pulse_port->sample_spec->channels * pulse_port->buffer_size * word_size;

  pthread_mutex_unlock(pulse_port_mutex);

  remaining = length;

  if(empty_run){
    void *empty_buffer;
    void *next_empty_buffer;
    
    n_bytes = 0;
    empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
    pa_stream_begin_write(stream, &empty_buffer, &n_bytes);
  
    // remaining = n_bytes;
    remaining = length;
    
    /* iterate */
    for(i = 0; remaining > 0; i++){
      pthread_mutex_lock(pulse_port_mutex);

      empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
      next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

      nth_empty_buffer = next_nth_empty_buffer;
	  
      if(nth_empty_buffer >= 7){
	next_nth_empty_buffer = 0;
      }else{
	next_nth_empty_buffer = nth_empty_buffer + 1;
      }

      pulse_port->nth_empty_buffer = nth_empty_buffer;

      pa_stream_write(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
    
      pthread_mutex_unlock(pulse_port_mutex);

      memset(next_empty_buffer, 0, count * sizeof(unsigned char));
    
      remaining -= count;
    }

    g_atomic_int_set(&(pulse_port->is_empty),
		     TRUE);
  }

  pthread_mutex_lock(device_mutex);

  /* wait callback */
  no_event = TRUE;

  if(pulse_devout != NULL){
    if((AGS_PULSE_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
      callback_mutex = pulse_devout->callback_mutex;

      pthread_mutex_unlock(device_mutex);
	
      /* give back computing time until ready */
      pthread_mutex_lock(callback_mutex);
    
      if((AGS_PULSE_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
	struct timespec timeout = {
	  0,
	  0,
	};

	guint latency;
      
	g_atomic_int_or(&(pulse_devout->sync_flags),
			AGS_PULSE_DEVOUT_CALLBACK_WAIT);

	pthread_mutex_lock(pulse_port_mutex);
      
	latency = ags_pulse_port_get_latency(pulse_port);

	pthread_mutex_unlock(pulse_port_mutex);
      
	latency /= 8;
      
	if(timeout.tv_nsec + latency < NSEC_PER_SEC){
	  timeout.tv_nsec += latency;
	}else{
	  timeout.tv_sec += 1;
	  timeout.tv_nsec = timeout.tv_nsec + latency - NSEC_PER_SEC;
	}
      
	while((AGS_PULSE_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0 &&
	      (AGS_PULSE_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	  void *empty_buffer;
	  void *next_empty_buffer;
	
	  pthread_cond_timedwait(pulse_devout->callback_cond,
				 callback_mutex,
				 &timeout);

	  pthread_mutex_unlock(callback_mutex);

	  if(g_atomic_int_get(&(pulse_port->underflow)) > 0){
	    n_bytes = 0;
	    empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);

#if 0
	    pa_stream_begin_write(stream, &empty_buffer, &n_bytes);	
#endif
	    
	    remaining = length;
	  }
	
	  /* feed */
	  for(i = 0; g_atomic_int_get(&(pulse_port->underflow)) > 0 || remaining > 0; i++){
	    pthread_mutex_lock(pulse_port_mutex);

	    empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	    next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

	    nth_empty_buffer = next_nth_empty_buffer;
	  
	    if(nth_empty_buffer >= 7){
	      next_nth_empty_buffer = 0;
	    }else{
	      next_nth_empty_buffer = nth_empty_buffer + 1;
	    }

	    pulse_port->nth_empty_buffer = nth_empty_buffer;

#if 0
	    pa_stream_write(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
#endif
	    
	    pthread_mutex_unlock(pulse_port_mutex);

	    memset(next_empty_buffer, 0, count * sizeof(unsigned char));

	    if(g_atomic_int_get(&(pulse_port->underflow)) > 0){
	      g_atomic_int_dec_and_test(&(pulse_port->underflow));
	    }
	  
	    remaining -= count;
	  }

	  pthread_mutex_lock(pulse_port_mutex);
	  
	  latency = ags_pulse_port_get_latency(pulse_port);

	  pthread_mutex_unlock(pulse_port_mutex);

	  latency /= 8;

	  clock_gettime(CLOCK_MONOTONIC, &timeout);
	  
	  if(timeout.tv_nsec + latency < NSEC_PER_SEC){
	    timeout.tv_nsec += latency;
	  }else{
	    timeout.tv_sec += 1;
	    timeout.tv_nsec = timeout.tv_nsec + latency - NSEC_PER_SEC;
	  }

	  pthread_mutex_lock(callback_mutex);
	}
      }
    
      g_atomic_int_and(&(pulse_devout->sync_flags),
		       (~(AGS_PULSE_DEVOUT_CALLBACK_WAIT |
			  AGS_PULSE_DEVOUT_CALLBACK_DONE)));
    
      pthread_mutex_unlock(callback_mutex);

      no_event = FALSE;

      pthread_mutex_lock(device_mutex);
    }

    /* get buffer */  
    if((AGS_PULSE_DEVOUT_BUFFER0 & (pulse_devout->flags)) != 0){
      nth_buffer = 7;
    }else if((AGS_PULSE_DEVOUT_BUFFER1 & (pulse_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_PULSE_DEVOUT_BUFFER2 & (pulse_devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_PULSE_DEVOUT_BUFFER3 & (pulse_devout->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_PULSE_DEVOUT_BUFFER4 & (pulse_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_PULSE_DEVOUT_BUFFER5 & (pulse_devout->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_PULSE_DEVOUT_BUFFER6 & (pulse_devout->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_PULSE_DEVOUT_BUFFER7 & (pulse_devout->flags)) != 0){
      nth_buffer = 6;
    }else{
      empty_run = TRUE;
    }
  }else if(pulse_devin != NULL){
    if((AGS_PULSE_DEVIN_PASS_THROUGH & (g_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
      callback_mutex = pulse_devin->callback_mutex;

      pthread_mutex_unlock(device_mutex);
	
      /* give back computing time until ready */
      pthread_mutex_lock(callback_mutex);
    
      if((AGS_PULSE_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(pulse_devin->sync_flags),
			AGS_PULSE_DEVIN_CALLBACK_WAIT);
      
	while((AGS_PULSE_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devin->sync_flags)))) == 0 &&
	      (AGS_PULSE_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
	  pthread_cond_wait(pulse_devin->callback_cond,
			    callback_mutex);
	}
	
	g_atomic_int_and(&(pulse_devin->sync_flags),
			 (~(AGS_PULSE_DEVIN_CALLBACK_WAIT |
			    AGS_PULSE_DEVIN_CALLBACK_DONE)));
	
	pthread_mutex_unlock(callback_mutex);
	
	no_event = FALSE;
	
	pthread_mutex_lock(device_mutex);
      }
    }
    
    /* get buffer */  
    if((AGS_PULSE_DEVIN_BUFFER0 & (pulse_devin->flags)) != 0){
      nth_buffer = 7;
    }else if((AGS_PULSE_DEVIN_BUFFER1 & (pulse_devin->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_PULSE_DEVIN_BUFFER2 & (pulse_devin->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_PULSE_DEVIN_BUFFER3 & (pulse_devin->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_PULSE_DEVIN_BUFFER4 & (pulse_devin->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_PULSE_DEVIN_BUFFER5 & (pulse_devin->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_PULSE_DEVIN_BUFFER6 & (pulse_devin->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_PULSE_DEVIN_BUFFER7 & (pulse_devin->flags)) != 0){
      nth_buffer = 6;
    }else{
      empty_run = TRUE;
    }
  }  

  switch(pulse_port->format){
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
  default:
    empty_run = TRUE;
  }
  
  /* write */
  count = pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size;

  if(pulse_devout != NULL){
    if(!empty_run){
      n_bytes = 0;
      pa_stream_begin_write(stream, &(pulse_devout->buffer[nth_buffer]), &n_bytes);
    
      //    g_message("%d", ags_synth_util_get_xcross_count_s16(pulse_devout->buffer[nth_buffer],
      //							pulse_devout->pcm_channels * pulse_devout->buffer_size));

      ags_soundcard_lock_buffer(AGS_SOUNDCARD(pulse_devout), pulse_devout->buffer[nth_buffer]);	    

      pa_stream_write(stream,
		      pulse_devout->buffer[nth_buffer],
		      count,
		      NULL,
		      0,
		      PA_SEEK_RELATIVE);
	  
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(pulse_devout), pulse_devout->buffer[nth_buffer]);	    

      g_atomic_int_set(&(pulse_port->is_empty),
		       FALSE);
    }

    /* signal finish */
    if(!no_event){        
      callback_finish_mutex = pulse_devout->callback_finish_mutex;
	
      pthread_mutex_lock(callback_finish_mutex);

      g_atomic_int_or(&(pulse_devout->sync_flags),
		      AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE);
    
      if((AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	pthread_cond_signal(pulse_devout->callback_finish_cond);
      }

      pthread_mutex_unlock(callback_finish_mutex);
    }

    pthread_mutex_unlock(device_mutex);

    if(empty_run){
      void *empty_buffer;
      void *next_empty_buffer;

      empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
    
      n_bytes = 0;
      pa_stream_begin_write(stream, &empty_buffer, &n_bytes);
    
      remaining = length;
    
      for(i = 0; remaining > 0; i++){
	/* feed */
	pthread_mutex_lock(pulse_port_mutex);

	empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

	nth_empty_buffer = next_nth_empty_buffer;
      
	if(nth_empty_buffer >= 7){
	  next_nth_empty_buffer = 0;
	}else{
	  next_nth_empty_buffer = nth_empty_buffer + 1;
	}

	pulse_port->nth_empty_buffer = nth_empty_buffer;

	pa_stream_write(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
	  
	pthread_mutex_unlock(pulse_port_mutex);

	memset(next_empty_buffer, 0, count * sizeof(unsigned char));

	remaining -= count;
      }
    }
  }else if(pulse_devin != NULL){
    if(!empty_run){
      pa_stream_peek(stream,
		     &(pulse_devin->buffer[nth_buffer]),
		     &count);
      
      g_atomic_int_set(&(pulse_port->is_empty),
		       FALSE);
    }

    /* signal finish */
    if(!no_event){        
      callback_finish_mutex = pulse_devin->callback_finish_mutex;
	
      pthread_mutex_lock(callback_finish_mutex);

      g_atomic_int_or(&(pulse_devin->sync_flags),
		      AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE);
    
      if((AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
	pthread_cond_signal(pulse_devin->callback_finish_cond);
      }

      pthread_mutex_unlock(callback_finish_mutex);
    }

    pthread_mutex_unlock(device_mutex);

    if(empty_run){
      void *empty_buffer;
      void *next_empty_buffer;
      
      remaining = pa_stream_readable_size(stream);
    
      for(i = 0; remaining > 0; i++){
	/* feed */
	pthread_mutex_lock(pulse_port_mutex);

	empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

	nth_empty_buffer = next_nth_empty_buffer;
      
	if(nth_empty_buffer >= 7){
	  next_nth_empty_buffer = 0;
	}else{
	  next_nth_empty_buffer = nth_empty_buffer + 1;
	}

	pulse_port->nth_empty_buffer = nth_empty_buffer;

	pa_stream_peek(stream,
		       &empty_buffer,
		       &count);
	
	pthread_mutex_unlock(pulse_port_mutex);
	
	memset(next_empty_buffer, 0, count * sizeof(unsigned char));
	
	remaining -= count;
      }
    }
  }  
  
  g_atomic_int_dec_and_test(&(pulse_port->queued));

  /* unref */
  g_object_unref(audio_loop);
  g_object_unref(task_thread);
}

void
ags_pulse_port_stream_underflow_callback(pa_stream *stream, AgsPulsePort *pulse_port)
{
  AgsAudioLoop *audio_loop;

  AgsPollingThread *polling_thread;

  AgsApplicationContext *application_context;
  
  guint time_spent;

  pthread_mutex_t *pulse_port_mutex;
      
  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  
  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* polling thread */
  polling_thread = (AgsPollingThread *) ags_thread_find_type((AgsThread *) audio_loop,
							     AGS_TYPE_POLLING_THREAD);
  
  pthread_mutex_lock(audio_loop->timing_mutex);

  g_atomic_int_set(&(audio_loop->time_spent),
		   audio_loop->time_cycle);

  pthread_mutex_unlock(audio_loop->timing_mutex);

  g_atomic_int_add(&(pulse_port->underflow),
		   1);

  if(polling_thread != NULL){
    g_atomic_int_or(&(polling_thread->flags),
		    AGS_POLLING_THREAD_OMIT);

    /* just omit three times since we don't poll pulse */
    g_atomic_int_add(&(polling_thread->omit_count),
		     4);
  }

  /* unref */
  g_object_unref(audio_loop);
}
#endif

guint
ags_pulse_port_get_fixed_size(AgsPulsePort *pulse_port)
{
  AgsPulseDevout *pulse_devout;
    
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;
  guint fixed_size;

  pthread_mutex_t *pulse_port_mutex;
  pthread_mutex_t *pulse_devout_mutex;

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* get pulse devout */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_devout = (AgsPulseDevout *) pulse_port->pulse_devout;

  if(pulse_port->use_cache){
    buffer_size = pulse_port->cache_buffer_size;
  }else{
    buffer_size = pulse_port->buffer_size;
  }

  pthread_mutex_unlock(pulse_port_mutex);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /*  */
  pthread_mutex_lock(pulse_devout_mutex);

  pcm_channels = pulse_devout->pcm_channels;
  
  format = pulse_devout->format;
  
  pthread_mutex_unlock(pulse_devout_mutex);
  
  switch(format){
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
  default:
    g_warning("pulse devout - unsupported format");

    return(0);
  }

  fixed_size = pcm_channels * buffer_size * word_size;
  
  return(fixed_size);
}

void
ags_pulse_port_set_samplerate(AgsPulsePort *pulse_port,
			      guint samplerate)
{
  guint fixed_size;

  pthread_mutex_t *pulse_port_mutex;

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
  
  /* lock pulse port */
  pthread_mutex_lock(pulse_port_mutex);

#ifdef AGS_WITH_PULSE
  pulse_port->sample_spec->rate = samplerate;

  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  pthread_mutex_unlock(pulse_port_mutex);
}

void
ags_pulse_port_set_buffer_size(AgsPulsePort *pulse_port,
			       guint buffer_size)
{
  guint fixed_size;

  pthread_mutex_t *pulse_port_mutex;

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
  
  /* lock pulse port */
  pthread_mutex_lock(pulse_port_mutex);

#ifdef AGS_WITH_PULSE
  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  pulse_port->buffer_size = buffer_size;
  
  if(pulse_port->empty_buffer != NULL){
    free(pulse_port->empty_buffer);
  }

  pulse_port->empty_buffer = ags_stream_alloc(pulse_port->pcm_channels * buffer_size,
					      pulse_port->format);

  pthread_mutex_unlock(pulse_port_mutex);
}

void
ags_pulse_port_set_pcm_channels(AgsPulsePort *pulse_port,
				guint pcm_channels)
{
  guint fixed_size;

  pthread_mutex_t *pulse_port_mutex;

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
  
  /* lock pulse port */
  pthread_mutex_lock(pulse_port_mutex);

  pulse_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_PULSE
  pulse_port->sample_spec->channels = pcm_channels;

  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  if(pulse_port->empty_buffer != NULL){
    free(pulse_port->empty_buffer);
  }
  
  pulse_port->empty_buffer = ags_stream_alloc(pcm_channels * pulse_port->buffer_size,
					      pulse_port->format);

  pthread_mutex_unlock(pulse_port_mutex);
}

void
ags_pulse_port_set_format(AgsPulsePort *pulse_port,
			  guint format)
{
  guint fixed_size;

  pthread_mutex_t *pulse_port_mutex;
  
  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
    
  /* lock pulse port */
  pthread_mutex_lock(pulse_port_mutex);

#ifdef AGS_WITH_PULSE
  switch(format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      if(ags_endian_host_is_be()){
	pulse_port->sample_spec->format = PA_SAMPLE_S16BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S16LE;
      }

      pulse_port->cache[0] = (void *) realloc(pulse_port->cache[0],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) realloc(pulse_port->cache[1],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) realloc(pulse_port->cache[2],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) realloc(pulse_port->cache[3],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      if(ags_endian_host_is_be()){
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32LE;
      }

      pulse_port->cache[0] = (void *) realloc(pulse_port->cache[0],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) realloc(pulse_port->cache[1],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) realloc(pulse_port->cache[2],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) realloc(pulse_port->cache[3],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      if(ags_endian_host_is_be()){
	pulse_port->sample_spec->format = PA_SAMPLE_S32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S32LE;
      }

      pulse_port->cache[0] = (void *) realloc(pulse_port->cache[0],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) realloc(pulse_port->cache[1],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) realloc(pulse_port->cache[2],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) realloc(pulse_port->cache[3],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  default:
    g_warning("pulse devout - unsupported format");
  }
  
  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  pulse_port->format = format;
  
  if(pulse_port->empty_buffer != NULL){
    free(pulse_port->empty_buffer);
  }
  
  pulse_port->empty_buffer = ags_stream_alloc(pulse_port->pcm_channels * pulse_port->buffer_size,
					      format);

  pthread_mutex_unlock(pulse_port_mutex);
}

void
ags_pulse_port_set_cache_buffer_size(AgsPulsePort *pulse_port,
				     guint cache_buffer_size)
{
  pthread_mutex_t *pulse_port_mutex;

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
    
  /* lock pulse port */
  pthread_mutex_lock(pulse_port_mutex);

  switch(pulse_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      pulse_port->cache[0] = (void *) realloc(pulse_port->cache[0],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) realloc(pulse_port->cache[1],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) realloc(pulse_port->cache[2],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) realloc(pulse_port->cache[3],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      pulse_port->cache[0] = (void *) realloc(pulse_port->cache[0],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) realloc(pulse_port->cache[1],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) realloc(pulse_port->cache[2],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) realloc(pulse_port->cache[3],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      pulse_port->cache[0] = (void *) realloc(pulse_port->cache[0],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[1] = (void *) realloc(pulse_port->cache[1],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[2] = (void *) realloc(pulse_port->cache[2],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
      pulse_port->cache[3] = (void *) realloc(pulse_port->cache[3],
					      pulse_port->pcm_channels * pulse_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  default:
    g_warning("pulse devout - unsupported format");
  }

  pthread_mutex_unlock(pulse_port_mutex);
}

/**
 * ags_pulse_port_get_latency:
 * @pulse_port: the #AgsPulsePort
 * 
 * Gets latency.
 * 
 * Since: 2.0.0
 */
guint
ags_pulse_port_get_latency(AgsPulsePort *pulse_port)
{
  guint latency;

  pthread_mutex_t *pulse_port_mutex;

  /* get pulse port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);
    
  /* lock pulse port */
  pthread_mutex_lock(pulse_port_mutex);

  latency = 0;
  
#ifdef AGS_WITH_PULSE
  if(pulse_port->use_cache){
    latency = (guint) floor((gdouble) NSEC_PER_SEC / (gdouble) pulse_port->sample_spec->rate * (gdouble) pulse_port->cache_buffer_size);
  }else{
    latency = (guint) floor((gdouble) NSEC_PER_SEC / (gdouble) pulse_port->sample_spec->rate * (gdouble) pulse_port->buffer_size);
  }
#endif

  pthread_mutex_unlock(pulse_port_mutex);

  return(latency);
}

/**
 * ags_pulse_port_new:
 * @pulse_client: the #AgsPulseClient assigned to
 *
 * Create a new instance of #AgsPulsePort.
 *
 * Returns: the new #AgsPulsePort
 *
 * Since: 2.0.0
 */
AgsPulsePort*
ags_pulse_port_new(GObject *pulse_client)
{
  AgsPulsePort *pulse_port;

  pulse_port = (AgsPulsePort *) g_object_new(AGS_TYPE_PULSE_PORT,
					     "pulse-client", pulse_client,
					     NULL);

  return(pulse_port);
}
