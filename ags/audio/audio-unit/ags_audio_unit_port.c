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

#include <ags/audio/audio-unit/ags_audio_unit_port.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/audio-unit/ags_audio_unit_server.h>
#include <ags/audio/audio-unit/ags_audio_unit_client.h>
#include <ags/audio/audio-unit/ags_audio_unit_devout.h>
#include <ags/audio/audio-unit/ags_audio_unit_devin.h>

#include <ags/config.h>
#include <ags/i18n.h>

#ifdef AGS_WITH_AUDIO_UNIT
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#endif

#include <time.h>

void ags_audio_unit_port_class_init(AgsAudioUnitPortClass *audio_unit_port);
void ags_audio_unit_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_port_init(AgsAudioUnitPort *audio_unit_port);
void ags_audio_unit_port_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_audio_unit_port_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_audio_unit_port_dispose(GObject *gobject);
void ags_audio_unit_port_finalize(GObject *gobject);

AgsUUID* ags_audio_unit_port_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_unit_port_has_resource(AgsConnectable *connectable);
gboolean ags_audio_unit_port_is_ready(AgsConnectable *connectable);
void ags_audio_unit_port_add_to_registry(AgsConnectable *connectable);
void ags_audio_unit_port_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_unit_port_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_unit_port_xml_compose(AgsConnectable *connectable);
void ags_audio_unit_port_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_audio_unit_port_is_connected(AgsConnectable *connectable);
void ags_audio_unit_port_connect(AgsConnectable *connectable);
void ags_audio_unit_port_disconnect(AgsConnectable *connectable);

#ifdef AGS_WITH_AUDIO_UNIT
OSStatus ags_audio_unit_port_output_render_callback(AgsAudioUnitPort *audio_unit_port,
						    AudioUnitRenderActionFlags *io_action_flags,
						    const AudioTimeStamp *in_time_stamp,
						    UInt32 in_bus_number,
						    UInt32 in_number_frames,
						    AudioBufferList *io_data);
#endif

/**
 * SECTION:ags_audio_unit_port
 * @short_description: core audio resource.
 * @title: AgsAudioUnitPort
 * @section_id:
 * @include: ags/audio/audio-unit/ags_audio_unit_port.h
 *
 * The #AgsAudioUnitPort represents either a core audio sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_AUDIO_UNIT_CLIENT,
  PROP_AUDIO_UNIT_DEVICE,
  PROP_PORT_NAME,
};

static gpointer ags_audio_unit_port_parent_class = NULL;

GType
ags_audio_unit_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_unit_port = 0;

    static const GTypeInfo ags_audio_unit_port_info = {
      sizeof(AgsAudioUnitPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_audio_unit_port = g_type_register_static(G_TYPE_OBJECT,
						      "AgsAudioUnitPort",
						      &ags_audio_unit_port_info,
						      0);

    g_type_add_interface_static(ags_type_audio_unit_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_unit_port);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_unit_port_class_init(AgsAudioUnitPortClass *audio_unit_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_unit_port_parent_class = g_type_class_peek_parent(audio_unit_port);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_port;

  gobject->set_property = ags_audio_unit_port_set_property;
  gobject->get_property = ags_audio_unit_port_get_property;

  gobject->dispose = ags_audio_unit_port_dispose;
  gobject->finalize = ags_audio_unit_port_finalize;

  /* properties */
  /**
   * AgsAudioUnitPort:audio-unit-client:
   *
   * The assigned #AgsAudioUnitClient.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-unit-client",
				   i18n_pspec("assigned core audio client"),
				   i18n_pspec("The assigned core audio client"),
				   AGS_TYPE_AUDIO_UNIT_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_CLIENT,
				  param_spec);

  /**
   * AgsAudioUnitPort:audio-unit-device:
   *
   * The assigned #AgsAudioUnitDevout.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-unit-device",
				   i18n_pspec("assigned core audio devout"),
				   i18n_pspec("The assigned core audio devout"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_DEVICE,
				  param_spec);

  /**
   * AgsAudioUnitPort:port-name:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("port-name",
				   i18n_pspec("port name"),
				   i18n_pspec("The port name"),
				   "hw:0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
}

void
ags_audio_unit_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_unit_port_get_uuid;
  connectable->has_resource = ags_audio_unit_port_has_resource;

  connectable->is_ready = ags_audio_unit_port_is_ready;
  connectable->add_to_registry = ags_audio_unit_port_add_to_registry;
  connectable->remove_from_registry = ags_audio_unit_port_remove_from_registry;

  connectable->list_resource = ags_audio_unit_port_list_resource;
  connectable->xml_compose = ags_audio_unit_port_xml_compose;
  connectable->xml_parse = ags_audio_unit_port_xml_parse;

  connectable->is_connected = ags_audio_unit_port_is_connected;  
  connectable->connect = ags_audio_unit_port_connect;
  connectable->disconnect = ags_audio_unit_port_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_unit_port_init(AgsAudioUnitPort *audio_unit_port)
{
  AgsConfig *config;

  gchar *str;

  guint word_size;
  guint fixed_size;
  guint i;
  
  /* flags */
  audio_unit_port->flags = 0;

  /* port mutex */
  g_rec_mutex_init(&(audio_unit_port->obj_mutex));

  /* parent */
  audio_unit_port->audio_unit_client = NULL;

  /* uuid */
  audio_unit_port->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_unit_port->uuid);

  /*  */
  audio_unit_port->audio_unit_device = NULL;
  
  audio_unit_port->port_uuid = ags_id_generator_create_uuid();
  audio_unit_port->port_name = NULL;
  
  /* read config */
  config = ags_config_get_instance();
  
  audio_unit_port->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  audio_unit_port->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio_unit_port->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio_unit_port->format = ags_soundcard_helper_config_get_format(config);

#ifdef AGS_WITH_AUDIO_UNIT
  audio_unit_port->data_format = (AudioStreamBasicDescription *) malloc(sizeof(AudioStreamBasicDescription));
  audio_unit_port->graph = (AUGraph *) malloc(sizeof(AUGraph));

  audio_unit_port->description = (AudioComponentDescription *) malloc(sizeof(AudioComponentDescription));
  memset(audio_unit_port->description, 0, sizeof(AudioComponentDescription));

  audio_unit_port->node = (AUNode *) malloc(sizeof(AUNode));

  audio_unit_port->audio_component = NULL;
  audio_unit_port->audio_unit = (AudioUnit *) malloc(sizeof(AudioUnit));

  audio_unit_port->render_callback = (AURenderCallbackStruct *) malloc(sizeof(AURenderCallbackStruct));
#else
  audio_unit_port->data_format = NULL;
  
  audio_unit_port->graph = NULL;
  
  audio_unit_port->description = NULL;
  audio_unit_port->node = NULL;
  
  audio_unit_port->audio_component = NULL;
  audio_unit_port->audio_unit = NULL;
  
  audio_unit_port->render_callback = NULL;
#endif
 
  audio_unit_port->midi_port_number = 0;
  
  g_atomic_int_set(&(audio_unit_port->is_empty),
		   FALSE);
  
  g_atomic_int_set(&(audio_unit_port->queued),
		   0);  
}

void
ags_audio_unit_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioUnitPort *audio_unit_port;

  GRecMutex *audio_unit_port_mutex;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(gobject);

  /* get audio_unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  switch(prop_id){
  case PROP_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *audio_unit_client;

      audio_unit_client = (AgsAudioUnitClient *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_port_mutex);

      if(audio_unit_port->audio_unit_client == (GObject *) audio_unit_client){
	g_rec_mutex_unlock(audio_unit_port_mutex);
	
	return;
      }

      if(audio_unit_port->audio_unit_client != NULL){
	g_object_unref(audio_unit_port->audio_unit_client);
      }

      if(audio_unit_client != NULL){
	g_object_ref(audio_unit_client);
      }
      
      audio_unit_port->audio_unit_client = (GObject *) audio_unit_client;

      g_rec_mutex_unlock(audio_unit_port_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_DEVICE:
    {
      GObject *audio_unit_device;

      audio_unit_device = g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_port_mutex);

      if(audio_unit_port->audio_unit_device == audio_unit_device){
	g_rec_mutex_unlock(audio_unit_port_mutex);
	
	return;
      }

      if(audio_unit_port->audio_unit_device != NULL){
	g_object_unref(audio_unit_port->audio_unit_device);
      }

      if(audio_unit_device != NULL){
	g_object_ref(audio_unit_device);
      }
      
      audio_unit_port->audio_unit_device = (GObject *) audio_unit_device;

      g_rec_mutex_unlock(audio_unit_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      g_rec_mutex_lock(audio_unit_port_mutex);

      if(audio_unit_port->port_name == port_name){
	g_rec_mutex_unlock(audio_unit_port_mutex);
	
	return;
      }

      if(audio_unit_port->port_name != NULL){
	g_free(audio_unit_port->port_name);
      }

      audio_unit_port->port_name = port_name;

      g_rec_mutex_unlock(audio_unit_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsAudioUnitPort *audio_unit_port;

  GRecMutex *audio_unit_port_mutex;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(gobject);

  /* get audio_unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);
  
  switch(prop_id){
  case PROP_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_port_mutex);

      g_value_set_object(value, audio_unit_port->audio_unit_client);

      g_rec_mutex_unlock(audio_unit_port_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_DEVICE:
    {
      g_rec_mutex_lock(audio_unit_port_mutex);

      g_value_set_object(value, audio_unit_port->audio_unit_device);

      g_rec_mutex_unlock(audio_unit_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_rec_mutex_lock(audio_unit_port_mutex);

      g_value_set_string(value, audio_unit_port->port_name);

      g_rec_mutex_unlock(audio_unit_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_port_dispose(GObject *gobject)
{
  AgsAudioUnitPort *audio_unit_port;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(gobject);

  /* core audio client */
  if(audio_unit_port->audio_unit_client != NULL){
    g_object_unref(audio_unit_port->audio_unit_client);

    audio_unit_port->audio_unit_client = NULL;
  }

  /* core audio device */
  if(audio_unit_port->audio_unit_device != NULL){
    g_object_unref(audio_unit_port->audio_unit_device);

    audio_unit_port->audio_unit_device = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_port_parent_class)->dispose(gobject);
}

void
ags_audio_unit_port_finalize(GObject *gobject)
{
  AgsAudioUnitPort *audio_unit_port;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(gobject);

  /* core audio client */
  if(audio_unit_port->audio_unit_client != NULL){
    g_object_unref(audio_unit_port->audio_unit_client);
  }

  /* core audio device */
  if(audio_unit_port->audio_unit_device != NULL){
    g_object_unref(audio_unit_port->audio_unit_device);
  }

  /* name */
  g_free(audio_unit_port->port_name);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_unit_port_get_uuid(AgsConnectable *connectable)
{
  AgsAudioUnitPort *audio_unit_port;
  
  AgsUUID *ptr;

  GRecMutex *audio_unit_port_mutex;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);

  /* get audio_unit port signal mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /* get UUID */
  g_rec_mutex_lock(audio_unit_port_mutex);

  ptr = audio_unit_port->uuid;

  g_rec_mutex_unlock(audio_unit_port_mutex);
  
  return(ptr);
}

gboolean
ags_audio_unit_port_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_audio_unit_port_is_ready(AgsConnectable *connectable)
{
  AgsAudioUnitPort *audio_unit_port;
  
  gboolean is_ready;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);

  /* check is added */
  is_ready = ags_audio_unit_port_test_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_audio_unit_port_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioUnitPort *audio_unit_port;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);

  ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_ADDED_TO_REGISTRY);
}

void
ags_audio_unit_port_remove_from_registry(AgsConnectable *connectable)
{
  AgsAudioUnitPort *audio_unit_port;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);

  ags_audio_unit_port_unset_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_audio_unit_port_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_unit_port_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_unit_port_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_unit_port_is_connected(AgsConnectable *connectable)
{
  AgsAudioUnitPort *audio_unit_port;
  
  gboolean is_connected;

  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);

  /* check is connected */
  is_connected = ags_audio_unit_port_test_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_CONNECTED);
  
  return(is_connected);
}

void
ags_audio_unit_port_connect(AgsConnectable *connectable)
{
  AgsAudioUnitPort *audio_unit_port;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);

  ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_CONNECTED);
}

void
ags_audio_unit_port_disconnect(AgsConnectable *connectable)
{

  AgsAudioUnitPort *audio_unit_port;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_port = AGS_AUDIO_UNIT_PORT(connectable);
  
  ags_audio_unit_port_unset_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_CONNECTED);
}

/**
 * ags_audio_unit_port_test_flags:
 * @audio_unit_port: the #AgsAudioUnitPort
 * @flags: the flags
 *
 * Test @flags to be set on @audio_unit_port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_unit_port_test_flags(AgsAudioUnitPort *audio_unit_port, guint flags)
{
  gboolean retval;  
  
  GRecMutex *audio_unit_port_mutex;

  if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port)){
    return(FALSE);
  }

  /* get audio_unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /* test */
  g_rec_mutex_lock(audio_unit_port_mutex);

  retval = (flags & (audio_unit_port->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_port_mutex);

  return(retval);
}

/**
 * ags_audio_unit_port_set_flags:
 * @audio_unit_port: the #AgsAudioUnitPort
 * @flags: see #AgsAudioUnitPortFlags-enum
 *
 * Enable a feature of @audio_unit_port.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_port_set_flags(AgsAudioUnitPort *audio_unit_port, guint flags)
{
  GRecMutex *audio_unit_port_mutex;

  if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port)){
    return;
  }

  /* get audio_unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_unit_port_mutex);

  audio_unit_port->flags |= flags;
  
  g_rec_mutex_unlock(audio_unit_port_mutex);
}
    
/**
 * ags_audio_unit_port_unset_flags:
 * @audio_unit_port: the #AgsAudioUnitPort
 * @flags: see #AgsAudioUnitPortFlags-enum
 *
 * Disable a feature of @audio_unit_port.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_port_unset_flags(AgsAudioUnitPort *audio_unit_port, guint flags)
{  
  GRecMutex *audio_unit_port_mutex;

  if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port)){
    return;
  }

  /* get audio_unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_unit_port_mutex);

  audio_unit_port->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_unit_port_mutex);
}

/**
 * ags_audio_unit_port_find:
 * @audio_unit_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @audio_unit_port.
 *
 * Returns: the next matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_audio_unit_port_find(GList *audio_unit_port,
			 gchar *port_name)
{
  gboolean success;
  
  GRecMutex *audio_unit_port_mutex;

  while(audio_unit_port != NULL){
    /* get audio_unit port mutex */
    audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port->data);

    /* check port name */
    g_rec_mutex_lock(audio_unit_port_mutex);

    success = (!g_ascii_strcasecmp(AGS_AUDIO_UNIT_PORT(audio_unit_port->data)->port_name,
				   port_name)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(audio_unit_port_mutex);

    if(success){
      return(audio_unit_port);
    }
  }

  return(NULL);
}

#ifdef AGS_WITH_AUDIO_UNIT
OSStatus
ags_audio_unit_port_output_render_callback(AgsAudioUnitPort *audio_unit_port,
					   AudioUnitRenderActionFlags *io_action_flags,
					   const AudioTimeStamp *in_time_stamp,
					   UInt32 in_bus_number,
					   UInt32 in_number_frames,
					   AudioBufferList *io_data)
{
}
#endif

void
ags_audio_unit_port_register(AgsAudioUnitPort *audio_unit_port,
			     gchar *port_name,
			     gboolean is_audio, gboolean is_midi,
			     gboolean is_output)
{
  if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port) ||
     port_name == NULL){
    return;
  }

  if(ags_audio_unit_port_test_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_REGISTERED)){
    return;
  }

  if(is_output){
    ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_IS_OUTPUT);
  }else{
    ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_IS_INPUT);
  }

#ifdef AGS_WITH_AUDIO_UNIT
  NewAUGraph(audio_unit_port->graph);
#endif
  
  if(is_audio){  
    ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_IS_AUDIO);

    if(is_output){
#ifdef AGS_WITH_AUDIO_UNIT
      audio_unit_port->description->componentType = kAudioUnitType_Output;
      audio_unit_port->description->componentSubType = kAudioUnitSubType_DefaultOutput;
      audio_unit_port->description->componentManufacturer = kAudioUnitManufacturer_Apple;

      AUGraphAddNode(audio_unit_port->graph[0],
		     audio_unit_port->description,
		     audio_unit_port->node);

      AUGraphOpen(audio_unit_port->graph[0]);
      AUGraphInitialize(audio_unit_port->graph[0]);

      AUGraphStart(audio_unit_port->graph[0]);

      audio_unit_port->audio_component = AudioComponentFindNext(NULL,
								audio_unit_port->description);
      AudioComponentInstanceNew(audio_unit_port->audio_component,
				audio_unit_port->audio_unit);

      audio_unit_port->render_callback->inputProc = ags_audio_unit_port_output_render_callback;
      audio_unit_port->render_callback->inputProcRefCon = audio_unit_port;

      AudioUnitSetProperty(audio_unit_port->audio_unit,
			   kAudioUnitProperty_SetRenderCallback,
			   kAudioUnitScope_Input,
			   0,
			   audio_unit_port->render_callback,
			   sizeof(AURenderCallbackStruct));
      
      AudioOutputUnitStart(audio_unit_port->audio_unit);
      
#endif
    }else{
#ifdef AGS_WITH_AUDIO_UNIT
#endif
    }
  }else if(is_midi){
    ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_IS_MIDI);

    if(is_output){
#ifdef AGS_WITH_AUDIO_UNIT
#endif
    }else{
#ifdef AGS_WITH_AUDIO_UNIT
#endif
    }
  }
  
  ags_audio_unit_port_set_flags(audio_unit_port, AGS_AUDIO_UNIT_PORT_REGISTERED);

  //TODO:JK: implement me
}

void
ags_audio_unit_port_unregister(AgsAudioUnitPort *audio_unit_port)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_port_set_format(AgsAudioUnitPort *audio_unit_port,
			       guint format)
{
  GRecMutex *audio_unit_port_mutex;

  /* get audio-unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /*  */
  g_rec_mutex_lock(audio_unit_port_mutex);

  audio_unit_port->format = format;
  
  g_rec_mutex_unlock(audio_unit_port_mutex);
}

void
ags_audio_unit_port_set_samplerate(AgsAudioUnitPort *audio_unit_port,
				   guint samplerate)
{
  GRecMutex *audio_unit_port_mutex;

  /* get audio-unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /*  */
  g_rec_mutex_lock(audio_unit_port_mutex);

#ifdef AGS_WITH_AUDIO_UNIT
  audio_unit_port->data_format->mSampleRate = (float) samplerate;
#endif

  audio_unit_port->samplerate = samplerate;
  
  g_rec_mutex_unlock(audio_unit_port_mutex);
}

void
ags_audio_unit_port_set_buffer_size(AgsAudioUnitPort *audio_unit_port,
				    guint buffer_size)
{  
  GRecMutex *audio_unit_port_mutex;

  /* get audio-unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /*  */
  g_rec_mutex_lock(audio_unit_port_mutex);

#ifdef AGS_WITH_AUDIO_UNIT
  //TODO:JK: implement me
#endif

  audio_unit_port->buffer_size = buffer_size;
  
  g_rec_mutex_unlock(audio_unit_port_mutex);
}

void
ags_audio_unit_port_set_pcm_channels(AgsAudioUnitPort *audio_unit_port,
				     guint pcm_channels)
{
  GRecMutex *audio_unit_port_mutex;

  /* get audio-unit port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /*  */
  g_rec_mutex_lock(audio_unit_port_mutex);

  audio_unit_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_AUDIO_UNIT
  audio_unit_port->data_format->mChannelsPerFrame = pcm_channels;
#endif

  g_rec_mutex_unlock(audio_unit_port_mutex);
}

/**
 * ags_audio_unit_port_new:
 * @audio_unit_client: the #AgsAudioUnitClient assigned to
 *
 * Create a new instance of #AgsAudioUnitPort.
 *
 * Returns: the new #AgsAudioUnitPort
 *
 * Since: 3.0.0
 */
AgsAudioUnitPort*
ags_audio_unit_port_new(GObject *audio_unit_client)
{
  AgsAudioUnitPort *audio_unit_port;

  audio_unit_port = (AgsAudioUnitPort *) g_object_new(AGS_TYPE_AUDIO_UNIT_PORT,
						      "audio-unit-client", audio_unit_client,
						      NULL);

  return(audio_unit_port);
}
