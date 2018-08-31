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

#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_notify_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <ags/config.h>
#include <ags/i18n.h>

#ifdef AGS_WITH_CORE_AUDIO
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#endif

#include <time.h>

void ags_core_audio_port_class_init(AgsCoreAudioPortClass *core_audio_port);
void ags_core_audio_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_port_init(AgsCoreAudioPort *core_audio_port);
void ags_core_audio_port_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_core_audio_port_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_core_audio_port_dispose(GObject *gobject);
void ags_core_audio_port_finalize(GObject *gobject);

AgsUUID* ags_core_audio_port_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_port_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_port_is_ready(AgsConnectable *connectable);
void ags_core_audio_port_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_port_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_port_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_port_xml_compose(AgsConnectable *connectable);
void ags_core_audio_port_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_core_audio_port_is_connected(AgsConnectable *connectable);
void ags_core_audio_port_connect(AgsConnectable *connectable);
void ags_core_audio_port_disconnect(AgsConnectable *connectable);

#ifdef AGS_WITH_CORE_AUDIO
void* ags_core_audio_port_output_thread(AgsCoreAudioPort *core_audio_port);

void ags_core_audio_port_handle_output_buffer(AgsCoreAudioPort *core_audio_port,
					      AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer);

void ags_core_audio_port_midi_notify_callback(const MIDINotification  *message,
					      void *ref_con);
void ags_core_audio_port_midi_read_callback(const MIDIPacketList *pkt_list,
					    void *ref_con,
					    void *conn_ref_con);
#endif

/**
 * SECTION:ags_core_audio_port
 * @short_description: core audio resource.
 * @title: AgsCoreAudioPort
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_port.h
 *
 * The #AgsCoreAudioPort represents either a core audio sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_DEVICE,
  PROP_PORT_NAME,
};

static gpointer ags_core_audio_port_parent_class = NULL;

static pthread_mutex_t ags_core_audio_port_class_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef AGS_WITH_CORE_AUDIO
volatile gint ags_core_audio_port_run_loop_initialized;
CFRunLoopRef ags_core_audio_port_run_loop = NULL;
#endif

GType
ags_core_audio_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_core_audio_port;

    static const GTypeInfo ags_core_audio_port_info = {
      sizeof(AgsCoreAudioPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_core_audio_port = g_type_register_static(G_TYPE_OBJECT,
						      "AgsCoreAudioPort",
						      &ags_core_audio_port_info,
						      0);

    g_type_add_interface_static(ags_type_core_audio_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_core_audio_port);
  }

  return g_define_type_id__volatile;
}

void
ags_core_audio_port_class_init(AgsCoreAudioPortClass *core_audio_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_core_audio_port_parent_class = g_type_class_peek_parent(core_audio_port);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_port;

  gobject->set_property = ags_core_audio_port_set_property;
  gobject->get_property = ags_core_audio_port_get_property;

  gobject->dispose = ags_core_audio_port_dispose;
  gobject->finalize = ags_core_audio_port_finalize;

  /* properties */
  /**
   * AgsCoreAudioPort:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("core-audio-client",
				   i18n_pspec("assigned core audio client"),
				   i18n_pspec("The assigned core audio client"),
				   AGS_TYPE_CORE_AUDIO_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);

  /**
   * AgsCoreAudioPort:core-audio-device:
   *
   * The assigned #AgsCoreAudioDevout.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("core-audio-device",
				   i18n_pspec("assigned core audio devout"),
				   i18n_pspec("The assigned core audio devout"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_DEVICE,
				  param_spec);

  /**
   * AgsCoreAudioPort:port-name:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 2.0.0
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
ags_core_audio_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_core_audio_port_get_uuid;
  connectable->has_resource = ags_core_audio_port_has_resource;

  connectable->is_ready = ags_core_audio_port_is_ready;
  connectable->add_to_registry = ags_core_audio_port_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_port_remove_from_registry;

  connectable->list_resource = ags_core_audio_port_list_resource;
  connectable->xml_compose = ags_core_audio_port_xml_compose;
  connectable->xml_parse = ags_core_audio_port_xml_parse;

  connectable->is_connected = ags_core_audio_port_is_connected;  
  connectable->connect = ags_core_audio_port_connect;
  connectable->disconnect = ags_core_audio_port_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_port_init(AgsCoreAudioPort *core_audio_port)
{
  AgsConfig *config;

  gchar *str;

  guint i;
  
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  core_audio_port->flags = 0;

  /* port mutex */
  core_audio_port->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_port->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  core_audio_port->core_audio_client = NULL;

  /* uuid */
  core_audio_port->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_port->uuid);

  /*  */
  core_audio_port->core_audio_device = NULL;
  
  core_audio_port->port_uuid = ags_id_generator_create_uuid();
  core_audio_port->port_name = NULL;
  
  /* read config */
  config = ags_config_get_instance();
  
  core_audio_port->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  core_audio_port->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  core_audio_port->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  core_audio_port->format = ags_soundcard_helper_config_get_format(config);

#ifdef AGS_WITH_CORE_AUDIO
  /* Audio */
  //core_audio_port->aq_ref = (AudioQueueRef *) malloc(sizeof(AudioQueueRef));
  memset(&(core_audio_port->aq_ref), 0, sizeof(AudioQueueRef));

  //core_audio_port->data_format = (AudioStreamBasicDescription *) malloc(sizeof(AudioStreamBasicDescription));
  memset(&(core_audio_port->data_format), 0, sizeof(AudioStreamBasicDescription));

  size_t bytesPerSample = sizeof(gint16);

  core_audio_port->data_format.mBitsPerChannel = 8 * bytesPerSample;

  core_audio_port->data_format.mBytesPerPacket = core_audio_port->pcm_channels * bytesPerSample;
  core_audio_port->data_format.mBytesPerFrame = core_audio_port->pcm_channels * bytesPerSample;
  
  core_audio_port->data_format.mFramesPerPacket = 1;
  core_audio_port->data_format.mChannelsPerFrame = core_audio_port->pcm_channels;

  core_audio_port->data_format.mFormatID = kAudioFormatLinearPCM;
  core_audio_port->data_format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
  
  core_audio_port->data_format.mSampleRate = (float) core_audio_port->samplerate;
  
  /* MIDI */
  core_audio_port->midi_client = (MIDIClientRef *) malloc(sizeof(MIDIClientRef));
  memset(core_audio_port->midi_client, 0, sizeof(MIDIClientRef));
  
  core_audio_port->midi_port = (MIDIPortRef *) malloc(sizeof(MIDIPortRef));
  memset(core_audio_port->midi_port, 0, sizeof(MIDIPortRef));
#else
  core_audio_port->aq_ref = NULL;

  core_audio_port->data_format = NULL;
  
  core_audio_port->midi_client = NULL;
  core_audio_port->midi_port = NULL;
#endif

  core_audio_port->midi_port_number = 0;
  
  g_atomic_int_set(&(core_audio_port->is_empty),
		   FALSE);
  
  g_atomic_int_set(&(core_audio_port->queued),
		   0);  
}

void
ags_core_audio_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsCoreAudioPort *core_audio_port;

  pthread_mutex_t *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  switch(prop_id){
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      pthread_mutex_lock(core_audio_port_mutex);

      if(core_audio_port->core_audio_client == (GObject *) core_audio_client){
	pthread_mutex_unlock(core_audio_port_mutex);
	
	return;
      }

      if(core_audio_port->core_audio_client != NULL){
	g_object_unref(core_audio_port->core_audio_client);
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_port->core_audio_client = (GObject *) core_audio_client;

      pthread_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_CORE_AUDIO_DEVICE:
    {
      GObject *core_audio_device;

      core_audio_device = g_value_get_object(value);

      pthread_mutex_lock(core_audio_port_mutex);

      if(core_audio_port->core_audio_device == core_audio_device){
	pthread_mutex_unlock(core_audio_port_mutex);
	
	return;
      }

      if(core_audio_port->core_audio_device != NULL){
	g_object_unref(core_audio_port->core_audio_device);
      }

      if(core_audio_device != NULL){
	g_object_ref(core_audio_device);
      }
      
      core_audio_port->core_audio_device = (GObject *) core_audio_device;

      pthread_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      pthread_mutex_lock(core_audio_port_mutex);

      if(core_audio_port->port_name == port_name ||
	 !g_ascii_strcasecmp(core_audio_port->port_name,
			     port_name)){
	pthread_mutex_unlock(core_audio_port_mutex);
	
	return;
      }

      if(core_audio_port->port_name != NULL){
	g_free(core_audio_port->port_name);
      }

      core_audio_port->port_name = port_name;

      pthread_mutex_unlock(core_audio_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsCoreAudioPort *core_audio_port;

  pthread_mutex_t *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());
  
  switch(prop_id){
  case PROP_CORE_AUDIO_CLIENT:
    {
      pthread_mutex_lock(core_audio_port_mutex);

      g_value_set_object(value, core_audio_port->core_audio_client);
      pthread_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_CORE_AUDIO_DEVICE:
    {
      pthread_mutex_lock(core_audio_port_mutex);

      g_value_set_object(value, core_audio_port->core_audio_device);

      pthread_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      pthread_mutex_lock(core_audio_port_mutex);

      g_value_set_string(value, core_audio_port->port_name);

      pthread_mutex_unlock(core_audio_port_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_port_dispose(GObject *gobject)
{
  AgsCoreAudioPort *core_audio_port;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  /* core audio client */
  if(core_audio_port->core_audio_client != NULL){
    g_object_unref(core_audio_port->core_audio_client);

    core_audio_port->core_audio_client = NULL;
  }

  /* core audio device */
  if(core_audio_port->core_audio_device != NULL){
    g_object_unref(core_audio_port->core_audio_device);

    core_audio_port->core_audio_device = NULL;
  }

  /* name */
  g_free(core_audio_port->port_name);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_port_parent_class)->dispose(gobject);
}

void
ags_core_audio_port_finalize(GObject *gobject)
{
  AgsCoreAudioPort *core_audio_port;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  pthread_mutex_destroy(core_audio_port->obj_mutex);
  free(core_audio_port->obj_mutex);

  pthread_mutexattr_destroy(core_audio_port->obj_mutexattr);
  free(core_audio_port->obj_mutexattr);

  /* core audio client */
  if(core_audio_port->core_audio_client != NULL){
    g_object_unref(core_audio_port->core_audio_client);
  }

  /* core audio device */
  if(core_audio_port->core_audio_device != NULL){
    g_object_unref(core_audio_port->core_audio_device);
  }

  /* name */
  g_free(core_audio_port->port_name);

  if(core_audio_port->midi_client != NULL){
    free(core_audio_port->midi_client);
  }

  if(core_audio_port->midi_port != NULL){
    free(core_audio_port->midi_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_port_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;
  
  AgsUUID *ptr;

  pthread_mutex_t *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core_audio port signal mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(core_audio_port_mutex);

  ptr = core_audio_port->uuid;

  pthread_mutex_unlock(core_audio_port_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_port_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_port_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;
  
  gboolean is_ready;

  pthread_mutex_t *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(core_audio_port_mutex);

  is_ready = (((AGS_CORE_AUDIO_PORT_ADDED_TO_REGISTRY & (core_audio_port->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(core_audio_port_mutex);
  
  return(is_ready);
}

void
ags_core_audio_port_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_ADDED_TO_REGISTRY);
}

void
ags_core_audio_port_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  ags_core_audio_port_unset_flags(core_audio_port, AGS_CORE_AUDIO_PORT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_core_audio_port_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_port_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_port_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_port_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;
  
  gboolean is_connected;

  pthread_mutex_t *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(core_audio_port_mutex);

  is_connected = (((AGS_CORE_AUDIO_PORT_CONNECTED & (core_audio_port->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(core_audio_port_mutex);
  
  return(is_connected);
}

void
ags_core_audio_port_connect(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_CONNECTED);
}

void
ags_core_audio_port_disconnect(AgsConnectable *connectable)
{

  AgsCoreAudioPort *core_audio_port;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);
  
  ags_core_audio_port_unset_flags(core_audio_port, AGS_CORE_AUDIO_PORT_CONNECTED);
}

/**
 * ags_core_audio_port_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_core_audio_port_get_class_mutex()
{
  return(&ags_core_audio_port_class_mutex);
}

/**
 * ags_core_audio_port_test_flags:
 * @core_audio_port: the #AgsCoreAudioPort
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_port.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_core_audio_port_test_flags(AgsCoreAudioPort *core_audio_port, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return(FALSE);
  }

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /* test */
  pthread_mutex_lock(core_audio_port_mutex);

  retval = (flags & (core_audio_port->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(core_audio_port_mutex);

  return(retval);
}

/**
 * ags_core_audio_port_set_flags:
 * @core_audio_port: the #AgsCoreAudioPort
 * @flags: see #AgsCoreAudioPortFlags-enum
 *
 * Enable a feature of @core_audio_port.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_port_set_flags(AgsCoreAudioPort *core_audio_port, guint flags)
{
  pthread_mutex_t *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(core_audio_port_mutex);

  core_audio_port->flags |= flags;
  
  pthread_mutex_unlock(core_audio_port_mutex);
}
    
/**
 * ags_core_audio_port_unset_flags:
 * @core_audio_port: the #AgsCoreAudioPort
 * @flags: see #AgsCoreAudioPortFlags-enum
 *
 * Disable a feature of @core_audio_port.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_port_unset_flags(AgsCoreAudioPort *core_audio_port, guint flags)
{  
  pthread_mutex_t *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(core_audio_port_mutex);

  core_audio_port->flags &= (~flags);
  
  pthread_mutex_unlock(core_audio_port_mutex);
}

/**
 * ags_core_audio_port_find:
 * @core_audio_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @core_audio_port.
 *
 * Returns: the next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_core_audio_port_find(GList *core_audio_port,
			 gchar *port_name)
{
  gboolean success;
  
  pthread_mutex_t *core_audio_port_mutex;

  while(core_audio_port != NULL){
    /* get core_audio port mutex */
    pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
    core_audio_port_mutex = AGS_CORE_AUDIO_PORT(core_audio_port->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

    /* check port name */
    pthread_mutex_lock(core_audio_port_mutex);

    success = (!g_ascii_strcasecmp(AGS_CORE_AUDIO_PORT(core_audio_port->data)->port_name,
				   port_name)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(core_audio_port_mutex);

    if(success){
      return(core_audio_port);
    }
  }

  return(NULL);
}

#ifdef AGS_WITH_CORE_AUDIO
OSStatus
SetCurrentIOBufferFrameSize(AudioObjectID inDeviceID,
			    UInt32 inIOBufferFrameSize)
{
  AudioObjectPropertyAddress theAddress = { kAudioDevicePropertyBufferFrameSize,
					    kAudioObjectPropertyScopeGlobal,
					    kAudioObjectPropertyElementMaster };
 
  return AudioObjectSetPropertyData(inDeviceID,
				    &theAddress,
				    0,
				    NULL,
				    sizeof(UInt32), &inIOBufferFrameSize);
}
#endif

#ifdef AGS_WITH_CORE_AUDIO
void*
ags_core_audio_port_output_thread(AgsCoreAudioPort *core_audio_port)
{
  ags_core_audio_port_run_loop = CFRunLoopGetCurrent();

  g_atomic_int_set(&ags_core_audio_port_run_loop_initialized,
		   TRUE);
  
  do{
    CFRunLoopRunInMode(kCFRunLoopDefaultMode,
		       0,
		       TRUE);
  }while(g_atomic_int_get(&(core_audio_port->running)));
  
  pthread_exit(NULL);
}
#endif

/**
 * ags_core_audio_port_register:
 * @core_audio_port: the #AgsCoreAudioPort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new core audio port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_port_register(AgsCoreAudioPort *core_audio_port,
			     gchar *port_name,
			     gboolean is_audio, gboolean is_midi,
			     gboolean is_output)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list;

  gchar *name, *uuid;

  guint format;
  guint i;
  
#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
  OSStatus retval;
#else
  gpointer graph;
#endif
  
  pthread_mutex_t *core_audio_client_mutex;
  pthread_mutex_t *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
     port_name == NULL){
    return;
  }

  g_object_get(core_audio_port,
	       "core-audio-client", &core_audio_client,
	       NULL);
  
  if(core_audio_client == NULL){
    g_warning("ags_core_audio_port.c - no assigned AgsCore_AudioClient");
    
    return;
  }

  if(ags_core_audio_port_test_flags(core_audio_port, AGS_CORE_AUDIO_PORT_REGISTERED)){
    return;
  }

  /* get core_audio server and application context */
  g_object_get(core_audio_client,
	       "core-audio-server", &core_audio_server,
	       NULL);
  
  if(core_audio_server == NULL){
    return;
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* get graph */
  pthread_mutex_lock(core_audio_client_mutex);

  graph = core_audio_client->graph;
  
  pthread_mutex_unlock(core_audio_client_mutex);

  if(graph == NULL){
    return;
  }

  /* get core_audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());
  
  /* get port name */
  //FIXME:JK: memory leak?
  pthread_mutex_lock(core_audio_port_mutex);

  port_name = g_strdup(core_audio_port->port_name);

  format = core_audio_port->format;
  
  pthread_mutex_unlock(core_audio_port_mutex);

  /* create sequencer or soundcard */
  if(is_output){
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_OUTPUT);
  }else{
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_INPUT);
  }

  if(is_audio){  
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_AUDIO);

    if(is_output){
      pthread_t *thread;

      void **arr;
      
      guint word_size;

      switch(format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(gint8);
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
      default:
	g_message("core audio - unsupported word size");
      }

#ifdef AGS_WITH_CORE_AUDIO
      g_atomic_int_set(&ags_core_audio_port_run_loop_initialized,
		       FALSE);
      g_atomic_int_set(&(core_audio_port->running),
		       TRUE);
      pthread_create(&thread, NULL, &ags_core_audio_port_output_thread, core_audio_port);

      while(!g_atomic_int_get(&ags_core_audio_port_run_loop_initialized)){
	usleep(400);
      }
      
      AudioQueueNewOutput(&(core_audio_port->data_format),
			  ags_core_audio_port_handle_output_buffer,
			  core_audio_port,
			  ags_core_audio_port_run_loop, NULL,
			  0,
			  &(core_audio_port->aq_ref));
      
      for(i = 0; i < 8; i++){
	AudioQueueAllocateBuffer(core_audio_port->aq_ref,
				 (core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gint16)),
				 &(core_audio_port->buf_ref[i]));
    
	ags_core_audio_port_handle_output_buffer(core_audio_port,
						 core_audio_port->aq_ref, core_audio_port->buf_ref[i]);
      }
      
      AudioQueueSetParameter(core_audio_port->aq_ref, 
			     kAudioQueueParam_Volume, 1.0);
    
      AudioQueueStart(core_audio_port->aq_ref,
		      NULL);
  
#endif
    }else{
      //NOTE:JK: not implemented
    }
  }else if(is_midi){
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_MIDI);

    if(is_output){
      //NOTE:JK: not implemented
    }else{
#ifdef AGS_WITH_CORE_AUDIO
      MIDIEndpointRef endpoint;
      
      retval = MIDIClientCreate(CFSTR("Advanced Gtk+ Sequencer - Core MIDI to System Sounds Demo"),
				NULL,
				core_audio_port,
				core_audio_port->midi_client);

      if(retval != noErr){
	return;
      }
      
      retval = MIDIInputPortCreate(*(core_audio_port->midi_client), CFSTR("Input port"),
				   ags_core_audio_port_midi_read_callback,
				   core_audio_port,
				   core_audio_port->midi_port);
	
      if(retval != noErr){
	return;
      }

      endpoint = MIDIGetSource(core_audio_port->midi_port_number);

      if(endpoint == 0){
	return;
      }
      
      retval = MIDIPortConnectSource(*(core_audio_port->midi_port), endpoint, NULL);

      if(retval != noErr){
	return;
      }
 #endif
    }
  }
  
  ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_REGISTERED);
}

void
ags_core_audio_port_unregister(AgsCoreAudioPort *core_audio_port)
{
  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }
  
  g_atomic_int_set(&(core_audio_port->running),
		   FALSE);
  //NOTE:JK: not implemented

  ags_core_audio_port_unset_flags(core_audio_port, AGS_CORE_AUDIO_PORT_REGISTERED);
}

#ifdef AGS_WITH_CORE_AUDIO
void
ags_core_audio_port_handle_output_buffer(AgsCoreAudioPort *core_audio_port,
					 AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  AgsAudioLoop *audio_loop;

  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  guint word_size;
  guint nth_buffer;
  guint copy_mode;
  guint count;
  guint i;
  gboolean is_playing;
  gboolean pass_through;
  gboolean no_event;
  gboolean empty_run;

  pthread_mutex_t *core_audio_port_mutex;
  pthread_mutex_t *device_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  if(core_audio_port == NULL){
    return;
  }
  
  /* get core-audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  if(g_atomic_int_get(&(core_audio_port->queued)) > 0){
    g_warning("drop core audio output callback");
    
    return;
  }else{
    g_atomic_int_inc(&(core_audio_port->queued));
  }

  /*
   * process audio
   */
  /*  */  
  application_context = ags_application_context_get_instance();

  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       "task-thread", &task_thread,
	       NULL);

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

  in_buffer->mAudioDataByteSize = core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gint16);
  ags_audio_buffer_util_clear_buffer(in_buffer->mAudioData, 1,
				     (in_buffer->mAudioDataByteSize / sizeof(gint16)), AGS_AUDIO_BUFFER_UTIL_S16);

  if(audio_loop == NULL){
    g_atomic_int_dec_and_test(&(core_audio_port->queued));

    return;
  }
  
  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

  core_audio_devout = core_audio_port->core_audio_device;

  pthread_mutex_unlock(core_audio_port_mutex);

  /* get device mutex */
  pthread_mutex_lock(ags_core_audio_devout_get_class_mutex());

  device_mutex = core_audio_devout->obj_mutex;

  pthread_mutex_unlock(ags_core_audio_devout_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

  empty_run = FALSE;
  
  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
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
  default:
    empty_run = TRUE;
  }

  count = core_audio_port->pcm_channels * core_audio_port->buffer_size;

  pthread_mutex_unlock(core_audio_port_mutex);
  
  /* wait callback */
  is_playing = ags_soundcard_is_playing(AGS_SOUNDCARD(core_audio_devout));

  if(is_playing){
    g_atomic_int_and(&(core_audio_devout->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH)));
  }
  
  no_event = TRUE;
  
  if(is_playing){
    pthread_mutex_lock(device_mutex);
    
    callback_mutex = core_audio_devout->callback_mutex;
    
    pthread_mutex_unlock(device_mutex);
	
    /* give back computing time until ready */
    if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);
      
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devout->sync_flags),
		      AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT);
	
	while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	  pthread_cond_wait(core_audio_devout->callback_cond,
			    callback_mutex);
	}
      }else{
	g_atomic_int_and(&(core_audio_devout->sync_flags),
			 (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));
      }
    }
    
    g_atomic_int_and(&(core_audio_devout->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT |
			AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE)));
    
    pthread_mutex_unlock(callback_mutex);
    
    no_event = FALSE;
  }

  /* get buffer */
  pthread_mutex_lock(device_mutex);
  
  if((AGS_CORE_AUDIO_DEVOUT_BUFFER0 & (core_audio_devout->flags)) != 0){
    nth_buffer = 7;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER1 & (core_audio_devout->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER2 & (core_audio_devout->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER3 & (core_audio_devout->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER4 & (core_audio_devout->flags)) != 0){
    nth_buffer = 3;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER5 & (core_audio_devout->flags)) != 0){
    nth_buffer = 4;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER6 & (core_audio_devout->flags)) != 0){
    nth_buffer = 5;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER7 & (core_audio_devout->flags)) != 0){
    nth_buffer = 6;
  }else{
    empty_run = TRUE;
  }

  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
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
  default:
    empty_run = TRUE;
  }
  
  pthread_mutex_unlock(device_mutex);
  
  /* get copy mode */
  if(!empty_run &&
     is_playing){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     FALSE);

    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_S16,
						    ags_audio_buffer_util_format_from_soundcard(core_audio_devout->format));

    ags_audio_buffer_util_copy_buffer_to_buffer(in_buffer->mAudioData, 1, 0,
						core_audio_devout->buffer[nth_buffer], 1, 0,
						core_audio_port->pcm_channels * core_audio_port->buffer_size, copy_mode);
  }

  AudioQueueEnqueueBuffer(core_audio_port->aq_ref,
			  in_buffer,
			  0,
			  NULL);
  
  /* signal finish */ 
  if(!no_event){
    callback_finish_mutex = core_audio_devout->callback_finish_mutex;
	
    pthread_mutex_lock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE);
    
    if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
      pthread_cond_signal(core_audio_devout->callback_finish_cond);
    }

    pthread_mutex_unlock(callback_finish_mutex);
  }

  if(empty_run){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }
    
  g_atomic_int_dec_and_test(&(core_audio_port->queued));
}
  
void
ags_core_audio_port_midi_notify_callback(const MIDINotification  *message,
					 void *ref_con)
{
  //NOTE:JK: unused
}

void
ags_core_audio_port_midi_read_callback(const MIDIPacketList *pkt_list,
				       void *ref_con,
				       void *conn_ref_con)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioMidiin *core_audio_midiin;
  
  AgsAudioLoop *audio_loop;

  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  MIDIPacket *packet;
  
  guint nth_buffer;
  gint num_packets;
  gint i;
  gboolean no_event;

  pthread_mutex_t *core_audio_port_mutex;
  pthread_mutex_t *device_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  core_audio_port = (AgsCoreAudioPort *) ref_con;

  if(core_audio_port == NULL){
    return;
  }

  /* get core-audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /*
   * process midi
   */
  /*  */  
  application_context = ags_application_context_get_instance();

  g_object_get(application_context,
	       "main-loop", &audio_loop,
	       "task-thread", &task_thread,
	       NULL);

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

  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

  core_audio_midiin = core_audio_port->core_audio_device;

  pthread_mutex_unlock(core_audio_port_mutex);
  
  /* get device mutex */
  pthread_mutex_lock(ags_core_audio_devout_get_class_mutex());

  device_mutex = core_audio_midiin->obj_mutex;

  pthread_mutex_unlock(ags_core_audio_devout_get_class_mutex());
  
  /*  */
  pthread_mutex_lock(device_mutex);

  callback_mutex = core_audio_midiin->callback_mutex;
  callback_finish_mutex = core_audio_midiin->callback_finish_mutex;
  
  /* wait callback */
  g_atomic_int_inc(&(core_audio_port->queued));

  no_event = TRUE;
  
  if((AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
    pthread_mutex_unlock(device_mutex);

    /* force wait */
    g_atomic_int_or(&(core_audio_midiin->sync_flags),
		    AGS_CORE_AUDIO_MIDIIN_DO_SYNC);
    
    /* wait callback */
    pthread_mutex_lock(callback_mutex);

    if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
      g_atomic_int_and(&(core_audio_midiin->sync_flags),
		       (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE));
      
      while((AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0 &&
	    (AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
	pthread_cond_wait(core_audio_midiin->callback_cond,
			  core_audio_midiin->callback_mutex);
      }
    }
    
    pthread_mutex_unlock(callback_mutex);

    g_atomic_int_or(&(core_audio_midiin->sync_flags),
		     (AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT | AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE));
    
    /* lock device */
    pthread_mutex_lock(device_mutex);
  
    no_event = FALSE;
  }else{
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }

  /* get buffer */  
  if((AGS_CORE_AUDIO_MIDIIN_BUFFER0 & (core_audio_midiin->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER1 & (core_audio_midiin->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER2 & (core_audio_midiin->flags)) != 0){
    nth_buffer = 3;
  }else if((AGS_CORE_AUDIO_MIDIIN_BUFFER3 & (core_audio_midiin->flags)) != 0){
    nth_buffer = 0;
  }
  
  packet = &pkt_list->packet[0];
  num_packets = pkt_list->numPackets;

  for(i = 0; i < num_packets; i++){
    guint length;

    length = packet->length;
    
    if(length == 0){
      packet = MIDIPacketNext(packet);
      
      continue;
    }

    if(ceil((core_audio_midiin->buffer_size[nth_buffer] + length) / 4096.0) > ceil(core_audio_midiin->buffer_size[nth_buffer] / 4096.0)){
      if(core_audio_midiin->buffer[nth_buffer] == NULL){
	core_audio_midiin->buffer[nth_buffer] = malloc(4096 * sizeof(char));
      }else{
	core_audio_midiin->buffer[nth_buffer] = realloc(core_audio_midiin->buffer[nth_buffer],
						  (ceil(core_audio_midiin->buffer_size[nth_buffer] / 4096.0) * 4096 + 4096) * sizeof(char));
      }
    }

    memcpy(&(core_audio_midiin->buffer[nth_buffer][core_audio_midiin->buffer_size[nth_buffer]]),
	   packet->data,
	   length);
    core_audio_midiin->buffer_size[nth_buffer] += length;

    packet = MIDIPacketNext(packet);
  }

  /* signal finish */  
  if(!no_event){
    pthread_mutex_unlock(device_mutex);

    /* signal client - wait callback finish */
    pthread_mutex_lock(callback_finish_mutex);

    g_atomic_int_and(&(core_audio_midiin->sync_flags),
		     (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT));

    if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
      pthread_cond_signal(core_audio_midiin->callback_finish_cond);
    }

    pthread_mutex_unlock(callback_finish_mutex);
  }

  g_atomic_int_dec_and_test(&(core_audio_port->queued));
}
#endif

void
ags_core_audio_port_set_format(AgsCoreAudioPort *core_audio_port,
			       guint format)
{
  pthread_mutex_t *core_audio_port_mutex;

  /* get core-audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

  core_audio_port->format = format;
  
  pthread_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_samplerate(AgsCoreAudioPort *core_audio_port,
				   guint samplerate)
{
  pthread_mutex_t *core_audio_port_mutex;

  /* get core-audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mSampleRate = (float) samplerate;
#endif

  core_audio_port->samplerate = samplerate;
  
  pthread_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_buffer_size(AgsCoreAudioPort *core_audio_port,
				    guint buffer_size)
{  
  pthread_mutex_t *core_audio_port_mutex;

  /* get core-audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

#ifdef AGS_WITH_CORE_AUDIO
  //TODO:JK: implement me
#endif

  core_audio_port->buffer_size = buffer_size;
  
  pthread_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_pcm_channels(AgsCoreAudioPort *core_audio_port,
				     guint pcm_channels)
{
  pthread_mutex_t *core_audio_port_mutex;

  /* get core-audio port mutex */
  pthread_mutex_lock(ags_core_audio_port_get_class_mutex());
  
  core_audio_port_mutex = core_audio_port->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_port_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_port_mutex);

  core_audio_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mChannelsPerFrame = pcm_channels;
#endif

  pthread_mutex_unlock(core_audio_port_mutex);
}

/**
 * ags_core_audio_port_new:
 * @core_audio_client: the #AgsCoreAudioClient assigned to
 *
 * Create a new instance of #AgsCoreAudioPort.
 *
 * Returns: the new #AgsCoreAudioPort
 *
 * Since: 2.0.0
 */
AgsCoreAudioPort*
ags_core_audio_port_new(GObject *core_audio_client)
{
  AgsCoreAudioPort *core_audio_port;

  core_audio_port = (AgsCoreAudioPort *) g_object_new(AGS_TYPE_CORE_AUDIO_PORT,
						      "core-audio-client", core_audio_client,
						      NULL);

  return(core_audio_port);
}
