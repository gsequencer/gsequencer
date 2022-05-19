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

#include <ags/audio/gstreamer/ags_gstreamer_port.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/gstreamer/ags_gstreamer_server.h>
#include <ags/audio/gstreamer/ags_gstreamer_client.h>
#include <ags/audio/gstreamer/ags_gstreamer_devout.h>
#include <ags/audio/gstreamer/ags_gstreamer_devin.h>

#include <ags/config.h>
#include <ags/i18n.h>

#include <time.h>

void ags_gstreamer_port_class_init(AgsGstreamerPortClass *gstreamer_port);
void ags_gstreamer_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gstreamer_port_init(AgsGstreamerPort *gstreamer_port);
void ags_gstreamer_port_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_gstreamer_port_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_gstreamer_port_dispose(GObject *gobject);
void ags_gstreamer_port_finalize(GObject *gobject);

AgsUUID* ags_gstreamer_port_get_uuid(AgsConnectable *connectable);
gboolean ags_gstreamer_port_has_resource(AgsConnectable *connectable);
gboolean ags_gstreamer_port_is_ready(AgsConnectable *connectable);
void ags_gstreamer_port_add_to_registry(AgsConnectable *connectable);
void ags_gstreamer_port_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_gstreamer_port_list_resource(AgsConnectable *connectable);
xmlNode* ags_gstreamer_port_xml_compose(AgsConnectable *connectable);
void ags_gstreamer_port_xml_parse(AgsConnectable *connectable,
				  xmlNode *node);
gboolean ags_gstreamer_port_is_connected(AgsConnectable *connectable);
void ags_gstreamer_port_connect(AgsConnectable *connectable);
void ags_gstreamer_port_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_gstreamer_port
 * @short_description: gstreamer resource.
 * @title: AgsGstreamerPort
 * @section_id:
 * @include: ags/audio/gstreamer/ags_gstreamer_port.h
 *
 * The #AgsGstreamerPort represents either a gstreamer sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_GSTREAMER_CLIENT,
  PROP_GSTREAMER_DEVOUT,
  PROP_GSTREAMER_DEVIN,
  PROP_PORT_NAME,
};

static gpointer ags_gstreamer_port_parent_class = NULL;

GType
ags_gstreamer_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_port = 0;

    static const GTypeInfo ags_gstreamer_port_info = {
      sizeof(AgsGstreamerPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsGstreamerPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_gstreamer_port = g_type_register_static(G_TYPE_OBJECT,
						     "AgsGstreamerPort",
						     &ags_gstreamer_port_info,
						     0);

    g_type_add_interface_static(ags_type_gstreamer_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_port);
  }

  return g_define_type_id__volatile;
}

GType
ags_gstreamer_port_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_GSTREAMER_PORT_REGISTERED, "AGS_GSTREAMER_PORT_REGISTERED", "gstreamer-port-registered" },
      { AGS_GSTREAMER_PORT_IS_AUDIO, "AGS_GSTREAMER_PORT_IS_AUDIO", "gstreamer-port-is-audio" },
      { AGS_GSTREAMER_PORT_IS_MIDI, "AGS_GSTREAMER_PORT_IS_MIDI", "gstreamer-port-is-midi" },
      { AGS_GSTREAMER_PORT_IS_OUTPUT, "AGS_GSTREAMER_PORT_IS_OUTPUT", "gstreamer-port-is-output" },
      { AGS_GSTREAMER_PORT_IS_INPUT, "AGS_GSTREAMER_PORT_IS_INPUT", "gstreamer-port-is-input" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsGstreamerPortFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_gstreamer_port_class_init(AgsGstreamerPortClass *gstreamer_port)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_gstreamer_port_parent_class = g_type_class_peek_parent(gstreamer_port);

  /* GObjectClass */
  gobject = (GObjectClass *) gstreamer_port;

  gobject->set_property = ags_gstreamer_port_set_property;
  gobject->get_property = ags_gstreamer_port_get_property;

  gobject->dispose = ags_gstreamer_port_dispose;
  gobject->finalize = ags_gstreamer_port_finalize;

  /* properties */
  /**
   * AgsGstreamerPort:gstreamer-client:
   *
   * The assigned #AgsGstreamerClient.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_object("gstreamer-client",
				   i18n_pspec("assigned gstreamer client"),
				   i18n_pspec("The assigned gstreamer client"),
				   AGS_TYPE_GSTREAMER_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_CLIENT,
				  param_spec);

  /**
   * AgsGstreamerPort:gstreamer-devout:
   *
   * The assigned #AgsGstreamerDevout.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_object("gstreamer-devout",
				   i18n_pspec("assigned gstreamer devout"),
				   i18n_pspec("The assigned gstreamer devout"),
				   AGS_TYPE_GSTREAMER_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_DEVOUT,
				  param_spec);

  /**
   * AgsGstreamerPort:gstreamer-devin:
   *
   * The assigned #AgsGstreamerDevout.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_object("gstreamer-devin",
				   i18n_pspec("assigned gstreamer devin"),
				   i18n_pspec("The assigned gstreamer devin"),
				   AGS_TYPE_GSTREAMER_DEVIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_DEVIN,
				  param_spec);

  /**
   * AgsGstreamerPort:port-name:
   *
   * The gstreamer soundcard indentifier
   * 
   * Since: 3.6.0
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
ags_gstreamer_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_gstreamer_port_get_uuid;
  connectable->has_resource = ags_gstreamer_port_has_resource;

  connectable->is_ready = ags_gstreamer_port_is_ready;
  connectable->add_to_registry = ags_gstreamer_port_add_to_registry;
  connectable->remove_from_registry = ags_gstreamer_port_remove_from_registry;

  connectable->list_resource = ags_gstreamer_port_list_resource;
  connectable->xml_compose = ags_gstreamer_port_xml_compose;
  connectable->xml_parse = ags_gstreamer_port_xml_parse;

  connectable->is_connected = ags_gstreamer_port_is_connected;  
  connectable->connect = ags_gstreamer_port_connect;
  connectable->disconnect = ags_gstreamer_port_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_gstreamer_port_init(AgsGstreamerPort *gstreamer_port)
{
  AgsConfig *config;
  
  gchar *str;

  guint word_size;
  guint fixed_size;

  /* flags */
  gstreamer_port->flags = 0;
  gstreamer_port->connectable_flags = 0;

  /* port mutex */
  g_rec_mutex_init(&(gstreamer_port->obj_mutex));

  /* parent */
  gstreamer_port->gstreamer_client = NULL;

  /* uuid */
  gstreamer_port->uuid = ags_uuid_alloc();
  ags_uuid_generate(gstreamer_port->uuid);

  /*  */
  gstreamer_port->gstreamer_devout = NULL;
  gstreamer_port->gstreamer_devin = NULL;
  
  gstreamer_port->port_uuid = ags_id_generator_create_uuid();
  gstreamer_port->port_name = NULL;
  
  /* presets */
  config = ags_config_get_instance();

  gstreamer_port->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  gstreamer_port->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  gstreamer_port->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  gstreamer_port->format = ags_soundcard_helper_config_get_format(config);

  gstreamer_port->use_cache = TRUE;
  gstreamer_port->cache_buffer_size = AGS_GSTREAMER_PORT_DEFAULT_CACHE_BUFFER_SIZE;

  gstreamer_port->current_cache = 0;
  gstreamer_port->completed_cache = 0;
  gstreamer_port->cache_offset = 0;

  gstreamer_port->cache = (void **) malloc(4 * sizeof(void *));
  
  word_size = 0;
  
  switch(gstreamer_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    gstreamer_port->cache[0] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint16));
    gstreamer_port->cache[1] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint16));
    gstreamer_port->cache[2] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint16));
    gstreamer_port->cache[3] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint16));

    word_size = sizeof(gint16);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    gstreamer_port->cache[0] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));

    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    gstreamer_port->cache[0] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) malloc(gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));

    word_size = sizeof(gint32);
  }
  break;
  default:
    g_warning("gstreamer devout/devin - unsupported format");
  }
  
  fixed_size = gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * word_size;

  memset(gstreamer_port->cache[0], 0, gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * word_size);
  memset(gstreamer_port->cache[1], 0, gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * word_size);
  memset(gstreamer_port->cache[2], 0, gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * word_size);
  memset(gstreamer_port->cache[3], 0, gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * word_size);

  gstreamer_port->empty_buffer = ags_stream_alloc(8 * gstreamer_port->pcm_channels * gstreamer_port->buffer_size,
						  AGS_SOUNDCARD_DEFAULT_FORMAT);

  g_atomic_int_set(&(gstreamer_port->is_empty),
		   TRUE);
  g_atomic_int_set(&(gstreamer_port->underflow),
		   0);
  g_atomic_int_set(&(gstreamer_port->restart),
		   FALSE);
  
  gstreamer_port->nth_empty_buffer = 0;
  
  g_atomic_int_set(&(gstreamer_port->queued),
		   0);
}

void
ags_gstreamer_port_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsGstreamerPort *gstreamer_port;

  GRecMutex *gstreamer_port_mutex;

  gstreamer_port = AGS_GSTREAMER_PORT(gobject);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  switch(prop_id){
  case PROP_GSTREAMER_CLIENT:
  {
    AgsGstreamerClient *gstreamer_client;

    gstreamer_client = (AgsGstreamerClient *) g_value_get_object(value);

    g_rec_mutex_lock(gstreamer_port_mutex);

    if(gstreamer_port->gstreamer_client == (GObject *) gstreamer_client){
      g_rec_mutex_unlock(gstreamer_port_mutex);
	
      return;
    }

    if(gstreamer_port->gstreamer_client != NULL){
      g_object_unref(gstreamer_port->gstreamer_client);
    }

    if(gstreamer_client != NULL){
      g_object_ref(gstreamer_client);
    }
      
    gstreamer_port->gstreamer_client = (GObject *) gstreamer_client;

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  case PROP_GSTREAMER_DEVOUT:
  {
    AgsGstreamerDevout *gstreamer_devout;

    gstreamer_devout = (AgsGstreamerDevout *) g_value_get_object(value);

    g_rec_mutex_lock(gstreamer_port_mutex);

    if(gstreamer_port->gstreamer_devout == (GObject *) gstreamer_devout){
      g_rec_mutex_unlock(gstreamer_port_mutex);
	
      return;
    }

    if(gstreamer_port->gstreamer_devout != NULL){
      g_object_unref(gstreamer_port->gstreamer_devout);
    }

    if(gstreamer_devout != NULL){
      g_object_ref(gstreamer_devout);
    }
      
    gstreamer_port->gstreamer_devout = (GObject *) gstreamer_devout;

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  case PROP_GSTREAMER_DEVIN:
  {
    AgsGstreamerDevin *gstreamer_devin;

    gstreamer_devin = (AgsGstreamerDevin *) g_value_get_object(value);

    g_rec_mutex_lock(gstreamer_port_mutex);

    if(gstreamer_port->gstreamer_devin == (GObject *) gstreamer_devin){
      g_rec_mutex_unlock(gstreamer_port_mutex);
	
      return;
    }

    if(gstreamer_port->gstreamer_devin != NULL){
      g_object_unref(gstreamer_port->gstreamer_devin);
    }

    if(gstreamer_devin != NULL){
      g_object_ref(gstreamer_devin);
    }
      
    gstreamer_port->gstreamer_devin = (GObject *) gstreamer_devin;

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  case PROP_PORT_NAME:
  {
    gchar *port_name;

    port_name = g_value_get_string(value);

    g_rec_mutex_lock(gstreamer_port_mutex);

    if(gstreamer_port->port_name == port_name ||
       !g_ascii_strcasecmp(gstreamer_port->port_name,
			   port_name)){
      g_rec_mutex_unlock(gstreamer_port_mutex);
	
      return;
    }

    if(gstreamer_port->port_name != NULL){
      g_free(gstreamer_port->port_name);
    }

    gstreamer_port->port_name = port_name;

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_port_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsGstreamerPort *gstreamer_port;

  GRecMutex *gstreamer_port_mutex;

  gstreamer_port = AGS_GSTREAMER_PORT(gobject);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
  
  switch(prop_id){
  case PROP_GSTREAMER_CLIENT:
  {
    g_rec_mutex_lock(gstreamer_port_mutex);

    g_value_set_object(value, gstreamer_port->gstreamer_client);

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  case PROP_GSTREAMER_DEVOUT:
  {
    g_rec_mutex_lock(gstreamer_port_mutex);

    g_value_set_object(value, gstreamer_port->gstreamer_devout);

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  case PROP_GSTREAMER_DEVIN:
  {
    g_rec_mutex_lock(gstreamer_port_mutex);

    g_value_set_object(value, gstreamer_port->gstreamer_devin);

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  case PROP_PORT_NAME:
  {
    g_rec_mutex_lock(gstreamer_port_mutex);

    g_value_set_string(value, gstreamer_port->port_name);

    g_rec_mutex_unlock(gstreamer_port_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_port_dispose(GObject *gobject)
{
  AgsGstreamerPort *gstreamer_port;

  gstreamer_port = AGS_GSTREAMER_PORT(gobject);

  /* gstreamer client */
  if(gstreamer_port->gstreamer_client != NULL){
    g_object_unref(gstreamer_port->gstreamer_client);

    gstreamer_port->gstreamer_client = NULL;
  }

  /* gstreamer devout */
  if(gstreamer_port->gstreamer_devout != NULL){
    g_object_unref(gstreamer_port->gstreamer_devout);

    gstreamer_port->gstreamer_devout = NULL;
  }

  /* gstreamer devin */
  if(gstreamer_port->gstreamer_devin != NULL){
    g_object_unref(gstreamer_port->gstreamer_devin);

    gstreamer_port->gstreamer_devin = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_port_parent_class)->dispose(gobject);
}

void
ags_gstreamer_port_finalize(GObject *gobject)
{
  AgsGstreamerPort *gstreamer_port;

  gstreamer_port = AGS_GSTREAMER_PORT(gobject);

  /* gstreamer client */
  if(gstreamer_port->gstreamer_client != NULL){
    g_object_unref(gstreamer_port->gstreamer_client);
  }

  /* gstreamer devout */
  if(gstreamer_port->gstreamer_devout != NULL){
    g_object_unref(gstreamer_port->gstreamer_devout);
  }

  /* gstreamer devin */
  if(gstreamer_port->gstreamer_devin != NULL){
    g_object_unref(gstreamer_port->gstreamer_devin);
  }

  /* name */
  g_free(gstreamer_port->port_name);

  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_gstreamer_port_get_uuid(AgsConnectable *connectable)
{
  AgsGstreamerPort *gstreamer_port;
  
  AgsUUID *ptr;

  GRecMutex *gstreamer_port_mutex;

  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port signal mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  /* get UUID */
  g_rec_mutex_lock(gstreamer_port_mutex);

  ptr = gstreamer_port->uuid;

  g_rec_mutex_unlock(gstreamer_port_mutex);
  
  return(ptr);
}

gboolean
ags_gstreamer_port_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_gstreamer_port_is_ready(AgsConnectable *connectable)
{
  AgsGstreamerPort *gstreamer_port;
  
  gboolean is_ready;

  GRecMutex *gstreamer_port_mutex;

  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  /* check is ready */
  g_rec_mutex_lock(gstreamer_port_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (gstreamer_port->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_port_mutex);
  
  return(is_ready);
}

void
ags_gstreamer_port_add_to_registry(AgsConnectable *connectable)
{
  AgsGstreamerPort *gstreamer_port;

  GRecMutex *gstreamer_port_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(gstreamer_port_mutex);
}

void
ags_gstreamer_port_remove_from_registry(AgsConnectable *connectable)
{
  AgsGstreamerPort *gstreamer_port;

  GRecMutex *gstreamer_port_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(gstreamer_port_mutex);
}

xmlNode*
ags_gstreamer_port_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_gstreamer_port_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_gstreamer_port_xml_parse(AgsConnectable *connectable,
			     xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_gstreamer_port_is_connected(AgsConnectable *connectable)
{
  AgsGstreamerPort *gstreamer_port;
  
  gboolean is_connected;

  GRecMutex *gstreamer_port_mutex;

  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  /* check is connected */
  g_rec_mutex_lock(gstreamer_port_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (gstreamer_port->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_port_mutex);
  
  return(is_connected);
}

void
ags_gstreamer_port_connect(AgsConnectable *connectable)
{
  AgsGstreamerPort *gstreamer_port;

  GRecMutex *gstreamer_port_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(gstreamer_port_mutex);
}

void
ags_gstreamer_port_disconnect(AgsConnectable *connectable)
{

  AgsGstreamerPort *gstreamer_port;

  GRecMutex *gstreamer_port_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_port = AGS_GSTREAMER_PORT(connectable);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(gstreamer_port_mutex);
}

/**
 * ags_gstreamer_port_test_flags:
 * @gstreamer_port: the #AgsGstreamerPort
 * @flags: the flags
 *
 * Test @flags to be set on @gstreamer_port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_port_test_flags(AgsGstreamerPort *gstreamer_port, guint flags)
{
  gboolean retval;  
  
  GRecMutex *gstreamer_port_mutex;

  if(!AGS_IS_GSTREAMER_PORT(gstreamer_port)){
    return(FALSE);
  }

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  /* test */
  g_rec_mutex_lock(gstreamer_port_mutex);

  retval = (flags & (gstreamer_port->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_port_mutex);

  return(retval);
}

/**
 * ags_gstreamer_port_set_flags:
 * @gstreamer_port: the #AgsGstreamerPort
 * @flags: see #AgsGstreamerPortFlags-enum
 *
 * Enable a feature of @gstreamer_port.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_port_set_flags(AgsGstreamerPort *gstreamer_port, guint flags)
{
  GRecMutex *gstreamer_port_mutex;

  if(!AGS_IS_GSTREAMER_PORT(gstreamer_port)){
    return;
  }

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->flags |= flags;
  
  g_rec_mutex_unlock(gstreamer_port_mutex);
}
    
/**
 * ags_gstreamer_port_unset_flags:
 * @gstreamer_port: the #AgsGstreamerPort
 * @flags: see #AgsGstreamerPortFlags-enum
 *
 * Disable a feature of @gstreamer_port.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_port_unset_flags(AgsGstreamerPort *gstreamer_port, guint flags)
{  
  GRecMutex *gstreamer_port_mutex;

  if(!AGS_IS_GSTREAMER_PORT(gstreamer_port)){
    return;
  }

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->flags &= (~flags);
  
  g_rec_mutex_unlock(gstreamer_port_mutex);
}

/**
 * ags_gstreamer_port_find:
 * @gstreamer_port: (element-type AgsAudio.GstreamerPort) (transfer none): the #GList-struct containig #AgsGstreamerPort
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @gstreamer_port.
 *
 * Returns: (element-type AgsAudio.GstreamerPort) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.6.0
 */
GList*
ags_gstreamer_port_find(GList *gstreamer_port,
			gchar *port_name)
{
  gboolean success;
  
  GRecMutex *gstreamer_port_mutex;

  while(gstreamer_port != NULL){
    /* get gstreamer port mutex */
    gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port->data);

    /* check port name */
    g_rec_mutex_lock(gstreamer_port_mutex);

    success = (!g_ascii_strcasecmp(AGS_GSTREAMER_PORT(gstreamer_port->data)->port_name,
				   port_name)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(gstreamer_port_mutex);

    if(success){
      return(gstreamer_port);
    }
  }

  return(NULL);
}

/**
 * ags_gstreamer_port_register:
 * @gstreamer_port: the #AgsGstreamerPort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new gstreamer port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_port_register(AgsGstreamerPort *gstreamer_port,
			    gchar *port_name,
			    gboolean is_audio, gboolean is_midi,
			    gboolean is_output)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *gstreamer_client;

#if defined(AGS_WITH_GSTREAMER)
  GstElement *pipeline;
#else
  gpointer pipeline;
#endif
  
  GList *list;

  int r;
  gboolean use_cache;
  
  GRecMutex *gstreamer_client_mutex;
  GRecMutex *gstreamer_port_mutex;
  
  if(!AGS_IS_GSTREAMER_PORT(gstreamer_port) ||
     port_name == NULL){
    return;
  }

  g_object_get(gstreamer_port,
	       "gstreamer-client", &gstreamer_client,
	       NULL);
  
  if(gstreamer_client == NULL){
    g_warning("ags_gstreamer_port.c - no assigned AgsGstreamerClient");
    
    return;
  }

  if(ags_gstreamer_port_test_flags(gstreamer_port, AGS_GSTREAMER_PORT_REGISTERED)){
    g_object_unref(gstreamer_client);
    
    return;
  }

  /* get gstreamer server and application context */
  g_object_get(gstreamer_client,
	       "gstreamer-server", &gstreamer_server,
	       NULL);
  
  if(gstreamer_server == NULL){
    g_object_unref(gstreamer_client);

    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* get pipeline */
  g_rec_mutex_lock(gstreamer_client_mutex);

  pipeline = gstreamer_client->pipeline;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);

  if(pipeline == NULL){
    g_object_unref(gstreamer_client);

    g_object_unref(gstreamer_server);
    
    return;
  }

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  /* get port name */
  //FIXME:JK: memory leak?
  g_rec_mutex_lock(gstreamer_port_mutex);
  
  port_name = g_strdup(gstreamer_port->port_name);

  use_cache = gstreamer_port->use_cache;
  
  g_rec_mutex_unlock(gstreamer_port_mutex);

  /* create sequencer or soundcard */
  if(is_output){
    ags_gstreamer_port_set_flags(gstreamer_port, AGS_GSTREAMER_PORT_IS_OUTPUT);
  }

#ifdef AGS_WITH_GSTREAMER
  if(is_audio){
    ags_gstreamer_port_set_flags(gstreamer_port, AGS_GSTREAMER_PORT_IS_AUDIO);

    //TODO:JK: implement me
  }else if(is_midi){
    ags_gstreamer_port_set_flags(gstreamer_port, AGS_GSTREAMER_PORT_IS_MIDI);
    
    //TODO:JK: implement me
  }
  
  if(pipeline != NULL){
    ags_gstreamer_port_set_flags(gstreamer_port, AGS_GSTREAMER_PORT_REGISTERED);
  }
#endif

  g_object_unref(gstreamer_client);

  g_object_unref(gstreamer_server);
}

void
ags_gstreamer_port_unregister(AgsGstreamerPort *gstreamer_port)
{
  GRecMutex *gstreamer_port_mutex;

  if(!AGS_IS_GSTREAMER_PORT(gstreamer_port)){
    return;
  }

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
    
  //TODO:JK: implement me
}

guint
ags_gstreamer_port_get_fixed_size(AgsGstreamerPort *gstreamer_port)
{
  AgsGstreamerDevout *gstreamer_devout;
    
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;
  guint fixed_size;

  GRecMutex *gstreamer_port_mutex;
  GRecMutex *gstreamer_devout_mutex;

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);

  /* get gstreamer devout */
  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_devout = (AgsGstreamerDevout *) gstreamer_port->gstreamer_devout;

  if(gstreamer_port->use_cache){
    buffer_size = gstreamer_port->cache_buffer_size;
  }else{
    buffer_size = gstreamer_port->buffer_size;
  }

  g_rec_mutex_unlock(gstreamer_port_mutex);

  /* get gstreamer devout mutex */
  gstreamer_devout_mutex = AGS_GSTREAMER_DEVOUT_GET_OBJ_MUTEX(gstreamer_devout);

  /*  */
  g_rec_mutex_lock(gstreamer_devout_mutex);

  pcm_channels = gstreamer_devout->pcm_channels;
  
  format = gstreamer_devout->format;
  
  g_rec_mutex_unlock(gstreamer_devout_mutex);
  
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
    g_warning("gstreamer devout - unsupported format");

    return(0);
  }

  fixed_size = pcm_channels * buffer_size * word_size;
  
  return(fixed_size);
}

void
ags_gstreamer_port_set_samplerate(AgsGstreamerPort *gstreamer_port,
				  guint samplerate)
{
  guint fixed_size;

  GRecMutex *gstreamer_port_mutex;

  fixed_size = ags_gstreamer_port_get_fixed_size(gstreamer_port);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
  
  //TODO:JK: implement me
}

void
ags_gstreamer_port_set_buffer_size(AgsGstreamerPort *gstreamer_port,
				   guint buffer_size)
{
  guint fixed_size;

  GRecMutex *gstreamer_port_mutex;

  fixed_size = ags_gstreamer_port_get_fixed_size(gstreamer_port);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
  
  /* lock gstreamer port */
  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->buffer_size = buffer_size;
  
  if(gstreamer_port->empty_buffer != NULL){
    free(gstreamer_port->empty_buffer);
  }

  gstreamer_port->empty_buffer = ags_stream_alloc(gstreamer_port->pcm_channels * buffer_size,
						  gstreamer_port->format);

  g_rec_mutex_unlock(gstreamer_port_mutex);
}

void
ags_gstreamer_port_set_pcm_channels(AgsGstreamerPort *gstreamer_port,
				    guint pcm_channels)
{
  guint fixed_size;

  GRecMutex *gstreamer_port_mutex;

  fixed_size = ags_gstreamer_port_get_fixed_size(gstreamer_port);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
  
  /* lock gstreamer port */
  g_rec_mutex_lock(gstreamer_port_mutex);

  gstreamer_port->pcm_channels = pcm_channels;

  if(gstreamer_port->empty_buffer != NULL){
    free(gstreamer_port->empty_buffer);
  }
  
  gstreamer_port->empty_buffer = ags_stream_alloc(pcm_channels * gstreamer_port->buffer_size,
						  gstreamer_port->format);

  g_rec_mutex_unlock(gstreamer_port_mutex);
}

void
ags_gstreamer_port_set_format(AgsGstreamerPort *gstreamer_port,
			      guint format)
{
  guint fixed_size;

  GRecMutex *gstreamer_port_mutex;
  
  fixed_size = ags_gstreamer_port_get_fixed_size(gstreamer_port);

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
    
  /* lock gstreamer port */
  g_rec_mutex_lock(gstreamer_port_mutex);

#ifdef AGS_WITH_GSTREAMER
  switch(format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    gstreamer_port->cache[0] = (void *) realloc(gstreamer_port->cache[0],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) realloc(gstreamer_port->cache[1],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) realloc(gstreamer_port->cache[2],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) realloc(gstreamer_port->cache[3],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    gstreamer_port->cache[0] = (void *) realloc(gstreamer_port->cache[0],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) realloc(gstreamer_port->cache[1],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) realloc(gstreamer_port->cache[2],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) realloc(gstreamer_port->cache[3],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    gstreamer_port->cache[0] = (void *) realloc(gstreamer_port->cache[0],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) realloc(gstreamer_port->cache[1],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) realloc(gstreamer_port->cache[2],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) realloc(gstreamer_port->cache[3],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
  }
  break;
  default:
    g_warning("gstreamer devout - unsupported format");
  }
#endif

  gstreamer_port->format = format;
  
  if(gstreamer_port->empty_buffer != NULL){
    free(gstreamer_port->empty_buffer);
  }
  
  gstreamer_port->empty_buffer = ags_stream_alloc(gstreamer_port->pcm_channels * gstreamer_port->buffer_size,
						  format);

  g_rec_mutex_unlock(gstreamer_port_mutex);
}

void
ags_gstreamer_port_set_cache_buffer_size(AgsGstreamerPort *gstreamer_port,
					 guint cache_buffer_size)
{
  GRecMutex *gstreamer_port_mutex;

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
    
  /* lock gstreamer port */
  g_rec_mutex_lock(gstreamer_port_mutex);

  switch(gstreamer_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
    gstreamer_port->cache[0] = (void *) realloc(gstreamer_port->cache[0],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) realloc(gstreamer_port->cache[1],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) realloc(gstreamer_port->cache[2],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) realloc(gstreamer_port->cache[3],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
    gstreamer_port->cache[0] = (void *) realloc(gstreamer_port->cache[0],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) realloc(gstreamer_port->cache[1],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) realloc(gstreamer_port->cache[2],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) realloc(gstreamer_port->cache[3],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
    gstreamer_port->cache[0] = (void *) realloc(gstreamer_port->cache[0],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[1] = (void *) realloc(gstreamer_port->cache[1],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[2] = (void *) realloc(gstreamer_port->cache[2],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
    gstreamer_port->cache[3] = (void *) realloc(gstreamer_port->cache[3],
						gstreamer_port->pcm_channels * gstreamer_port->cache_buffer_size * sizeof(gint32));
  }
  break;
  default:
    g_warning("gstreamer devout - unsupported format");
  }

  g_rec_mutex_unlock(gstreamer_port_mutex);
}

/**
 * ags_gstreamer_port_get_latency:
 * @gstreamer_port: the #AgsGstreamerPort
 * 
 * Gets latency.
 * 
 * Since: 3.6.0
 */
guint
ags_gstreamer_port_get_latency(AgsGstreamerPort *gstreamer_port)
{
  guint latency;

  GRecMutex *gstreamer_port_mutex;

  /* get gstreamer port mutex */
  gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(gstreamer_port);
    
  /* lock gstreamer port */
  g_rec_mutex_lock(gstreamer_port_mutex);

  latency = 0;
  
#ifdef AGS_WITH_GSTREAMER
  if(gstreamer_port->use_cache){
    latency = (guint) floor((gdouble) G_TIME_SPAN_SECOND / (gdouble) gstreamer_port->samplerate * (gdouble) gstreamer_port->cache_buffer_size);
  }else{
    latency = (guint) floor((gdouble) G_TIME_SPAN_SECOND / (gdouble) gstreamer_port->samplerate * (gdouble) gstreamer_port->buffer_size);
  }
#endif

  g_rec_mutex_unlock(gstreamer_port_mutex);

  return(latency);
}

/**
 * ags_gstreamer_port_new:
 * @gstreamer_client: the #AgsGstreamerClient assigned to
 *
 * Create a new instance of #AgsGstreamerPort.
 *
 * Returns: the new #AgsGstreamerPort
 *
 * Since: 3.6.0
 */
AgsGstreamerPort*
ags_gstreamer_port_new(GObject *gstreamer_client)
{
  AgsGstreamerPort *gstreamer_port;

  gstreamer_port = (AgsGstreamerPort *) g_object_new(AGS_TYPE_GSTREAMER_PORT,
						     "gstreamer-client", gstreamer_client,
						     NULL);

  return(gstreamer_port);
}
