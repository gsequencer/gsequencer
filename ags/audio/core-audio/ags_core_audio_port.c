/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_signal.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <ags/config.h>
#include <ags/i18n.h>

#ifdef AGS_WITH_CORE_AUDIO
#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#endif

#include <time.h>

#define AGS_CORE_AUDIO_PORT_USE_HW (1)

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
OSStatus ags_core_audio_port_hw_output_callback(AudioObjectID device,
						const AudioTimeStamp* time_now,
						const AudioBufferList* in,
						const AudioTimeStamp* in_time,
						AudioBufferList* out,
						const AudioTimeStamp* out_tim,
						AgsCoreAudioPort *core_audio_port);
OSStatus ags_core_audio_port_hw_input_callback(AudioObjectID device,
					       const AudioTimeStamp* time_now,
					       const AudioBufferList* in,
					       const AudioTimeStamp* in_time,
					       AudioBufferList* out,
					       const AudioTimeStamp* out_tim,
					       AgsCoreAudioPort *core_audio_port);

void* ags_core_audio_port_cached_output_thread(AgsCoreAudioPort *core_audio_port);
void* ags_core_audio_port_cached_input_thread(AgsCoreAudioPort *core_audio_port);

void* ags_core_audio_port_output_thread(AgsCoreAudioPort *core_audio_port);
void* ags_core_audio_port_input_thread(AgsCoreAudioPort *core_audio_port);

void ags_core_audio_port_cached_handle_output_buffer(AgsCoreAudioPort *core_audio_port,
						     AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer);
void ags_core_audio_port_cached_handle_input_buffer(AgsCoreAudioPort *core_audio_port,
						    AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer,
						    const AudioTimeStamp *in_start_time, UInt32 in_num_packets,
						    const AudioStreamPacketDescription *in_packet_desc);

void ags_core_audio_port_handle_output_buffer(AgsCoreAudioPort *core_audio_port,
					      AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer);
void ags_core_audio_port_handle_input_buffer(AgsCoreAudioPort *core_audio_port,
					     AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer,
					     const AudioTimeStamp *in_start_time, UInt32 in_num_packets,
					     const AudioStreamPacketDescription *in_packet_desc);

void ags_core_audio_port_midi_notify_callback(const MIDINotification  *message,
					      void *ref_con);
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

#ifdef AGS_WITH_CORE_AUDIO
volatile gint ags_core_audio_port_output_run_loop_initialized;
volatile gint ags_core_audio_port_input_run_loop_initialized;

#ifdef AGS_MAC_BUNDLE
void ags_core_audio_port_premain() __attribute__ ((constructor));
#endif

void ags_core_audio_port_premain()
{
  g_atomic_int_set(&ags_core_audio_port_output_run_loop_initialized,
		   FALSE);
  g_atomic_int_set(&ags_core_audio_port_input_run_loop_initialized,
		   FALSE);
}

CFRunLoopRef ags_core_audio_port_output_run_loop = NULL;
CFRunLoopRef ags_core_audio_port_input_run_loop = NULL;
#endif

GType
ags_core_audio_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_core_audio_port = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_core_audio_port);
  }

  return g_define_type_id__volatile;
}

GType
ags_core_audio_port_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_CORE_AUDIO_PORT_REGISTERED, "AGS_CORE_AUDIO_PORT_REGISTERED", "core-audio-port-registered" },
      { AGS_CORE_AUDIO_PORT_IS_AUDIO, "AGS_CORE_AUDIO_PORT_IS_AUDIO", "core-audio-port-is-audio" },
      { AGS_CORE_AUDIO_PORT_IS_MIDI, "AGS_CORE_AUDIO_PORT_IS_MIDI", "core-audio-port-is-midi" },
      { AGS_CORE_AUDIO_PORT_IS_OUTPUT, "AGS_CORE_AUDIO_PORT_IS_OUTPUT", "core-audio-port-is-output" },
      { AGS_CORE_AUDIO_PORT_IS_INPUT, "AGS_CORE_AUDIO_PORT_IS_INPUT", "core-audio-port-is-input" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsCoreAudioPortFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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

  guint word_size;
  guint fixed_size;
  guint i;
  
  /* flags */
  core_audio_port->flags = 0;
  core_audio_port->connectable_flags = 0;

  /* port mutex */
  g_rec_mutex_init(&(core_audio_port->obj_mutex));

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

#if 0
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
  core_audio_port->use_cache = FALSE;
#else
  core_audio_port->use_cache = TRUE;
#endif

  core_audio_port->cache_buffer_size = AGS_CORE_AUDIO_PORT_DEFAULT_CACHE_BUFFER_SIZE;
#else
  core_audio_port->use_cache = ags_soundcard_helper_config_get_use_cache(config);

  core_audio_port->cache_buffer_size = ags_soundcard_helper_config_get_cache_buffer_size(config);
#endif
  
  core_audio_port->current_cache = 0;
  core_audio_port->completed_cache = 0;
  core_audio_port->cache_offset = 0;
  
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
  core_audio_port->cache = NULL;
#else
  core_audio_port->cache = (void **) malloc(4 * sizeof(void *));
#endif
  
#ifdef AGS_WITH_CORE_AUDIO
  /* Audio */
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
  /* output */
  core_audio_port->output_samplerate_property_address = (AudioObjectPropertyAddress *) malloc(sizeof(AudioObjectPropertyAddress));
  memset(core_audio_port->output_samplerate_property_address, 0, sizeof(AudioObjectPropertyAddress));

  core_audio_port->output_samplerate_property_address->mSelector = kAudioDevicePropertyNominalSampleRate;
  core_audio_port->output_samplerate_property_address->mElement = kAudioObjectPropertyElementMaster;
  core_audio_port->output_samplerate_property_address->mScope = kAudioObjectPropertyScopeGlobal;
  
  core_audio_port->output_buffer_size_property_address = (AudioObjectPropertyAddress *) malloc(sizeof(AudioObjectPropertyAddress));
  memset(core_audio_port->output_buffer_size_property_address, 0, sizeof(AudioObjectPropertyAddress));

  core_audio_port->output_buffer_size_property_address->mSelector = kAudioDevicePropertyBufferSize;
  core_audio_port->output_buffer_size_property_address->mElement = kAudioObjectPropertyElementMaster;
  core_audio_port->output_buffer_size_property_address->mScope = kAudioObjectPropertyScopeGlobal;
  
  core_audio_port->output_property_address = (AudioObjectPropertyAddress *) malloc(sizeof(AudioObjectPropertyAddress));
  memset(core_audio_port->output_property_address, 0, sizeof(AudioObjectPropertyAddress));

  core_audio_port->output_property_address->mSelector = kAudioHardwarePropertyDefaultOutputDevice;
  core_audio_port->output_property_address->mElement = kAudioObjectPropertyElementMaster;
  core_audio_port->output_property_address->mScope = kAudioObjectPropertyScopeGlobal;

  core_audio_port->output_device = 0;

  /* input */
  core_audio_port->input_samplerate_property_address = (AudioObjectPropertyAddress *) malloc(sizeof(AudioObjectPropertyAddress));
  memset(core_audio_port->input_samplerate_property_address, 0, sizeof(AudioObjectPropertyAddress));

  core_audio_port->input_samplerate_property_address->mSelector = kAudioDevicePropertyNominalSampleRate;
  core_audio_port->input_samplerate_property_address->mElement = kAudioObjectPropertyElementMaster;
  core_audio_port->input_samplerate_property_address->mScope = kAudioObjectPropertyScopeGlobal;
  
  core_audio_port->input_buffer_size_property_address = (AudioObjectPropertyAddress *) malloc(sizeof(AudioObjectPropertyAddress));
  memset(core_audio_port->input_buffer_size_property_address, 0, sizeof(AudioObjectPropertyAddress));
  
  core_audio_port->input_buffer_size_property_address->mSelector = kAudioDevicePropertyBufferSize;
  core_audio_port->input_buffer_size_property_address->mElement = kAudioObjectPropertyElementMaster;
  core_audio_port->input_buffer_size_property_address->mScope = kAudioObjectPropertyScopeGlobal;
  
  core_audio_port->input_property_address = (AudioObjectPropertyAddress *) malloc(sizeof(AudioObjectPropertyAddress));
  memset(core_audio_port->input_property_address, 0, sizeof(AudioObjectPropertyAddress));

  core_audio_port->input_property_address->mSelector = kAudioHardwarePropertyDefaultInputDevice;
  core_audio_port->input_property_address->mElement = kAudioObjectPropertyElementMaster;
  core_audio_port->input_property_address->mScope = kAudioObjectPropertyScopeGlobal;

  core_audio_port->input_device = 0;  
#else
  //core_audio_port->aq_ref = (AudioQueueRef *) malloc(sizeof(AudioQueueRef));
  memset(&(core_audio_port->aq_ref), 0, sizeof(AudioQueueRef));
  memset(&(core_audio_port->record_aq_ref), 0, sizeof(AudioQueueRef));

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

  memset(&(core_audio_port->record_format), 0, sizeof(AudioStreamBasicDescription));

  core_audio_port->record_format.mBitsPerChannel = 8 * bytesPerSample;

  core_audio_port->record_format.mBytesPerPacket = core_audio_port->pcm_channels * bytesPerSample;
  core_audio_port->record_format.mBytesPerFrame = core_audio_port->pcm_channels * bytesPerSample;
  
  core_audio_port->record_format.mFramesPerPacket = 1;
  core_audio_port->record_format.mChannelsPerFrame = core_audio_port->pcm_channels;

  core_audio_port->record_format.mFormatID = kAudioFormatLinearPCM;
  core_audio_port->record_format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
  
  core_audio_port->record_format.mSampleRate = (float) core_audio_port->samplerate;
#endif
  
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

#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  word_size = 0;
  
  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      core_audio_port->cache[0] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint16));
      core_audio_port->cache[1] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint16));
      core_audio_port->cache[2] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint16));
      core_audio_port->cache[3] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint16));

      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      core_audio_port->cache[0] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[1] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[2] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[3] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));

      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      core_audio_port->cache[0] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[1] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[2] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[3] = (void *) malloc(core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));

      word_size = sizeof(gint32);
    }
    break;
  default:
    g_warning("core audio devout/devin - unsupported format");
  }
  
  fixed_size = core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * word_size;

  memset(core_audio_port->cache[0], 0, core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * word_size);
  memset(core_audio_port->cache[1], 0, core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * word_size);
  memset(core_audio_port->cache[2], 0, core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * word_size);
  memset(core_audio_port->cache[3], 0, core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * word_size);
#endif
  
  core_audio_port->midi_port_number = 0;
  
  g_atomic_int_set(&(core_audio_port->is_empty),
		   FALSE);
  
  g_atomic_int_set(&(core_audio_port->queued),
		   0);  

  core_audio_port->audio_buffer_util = ags_audio_buffer_util_alloc();
}

void
ags_core_audio_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsCoreAudioPort *core_audio_port;

  GRecMutex *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  switch(prop_id){
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      g_rec_mutex_lock(core_audio_port_mutex);

      if(core_audio_port->core_audio_client == (GObject *) core_audio_client){
	g_rec_mutex_unlock(core_audio_port_mutex);
	
	return;
      }

      if(core_audio_port->core_audio_client != NULL){
	g_object_unref(core_audio_port->core_audio_client);
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_port->core_audio_client = (GObject *) core_audio_client;

      g_rec_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_CORE_AUDIO_DEVICE:
    {
      GObject *core_audio_device;

      core_audio_device = g_value_get_object(value);

      g_rec_mutex_lock(core_audio_port_mutex);

      if(core_audio_port->core_audio_device == core_audio_device){
	g_rec_mutex_unlock(core_audio_port_mutex);
	
	return;
      }

      if(core_audio_port->core_audio_device != NULL){
	g_object_unref(core_audio_port->core_audio_device);
      }

      if(core_audio_device != NULL){
	g_object_ref(core_audio_device);
      }
      
      core_audio_port->core_audio_device = (GObject *) core_audio_device;

      g_rec_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      g_rec_mutex_lock(core_audio_port_mutex);

      if(core_audio_port->port_name == port_name){
	g_rec_mutex_unlock(core_audio_port_mutex);
	
	return;
      }

      if(core_audio_port->port_name != NULL){
	g_free(core_audio_port->port_name);
      }

      core_audio_port->port_name = g_strdup(port_name);

      g_rec_mutex_unlock(core_audio_port_mutex);
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

  GRecMutex *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);
  
  switch(prop_id){
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_port_mutex);

      g_value_set_object(value, core_audio_port->core_audio_client);

      g_rec_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_CORE_AUDIO_DEVICE:
    {
      g_rec_mutex_lock(core_audio_port_mutex);

      g_value_set_object(value, core_audio_port->core_audio_device);

      g_rec_mutex_unlock(core_audio_port_mutex);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_rec_mutex_lock(core_audio_port_mutex);

      g_value_set_string(value, core_audio_port->port_name);

      g_rec_mutex_unlock(core_audio_port_mutex);
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

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_port_parent_class)->dispose(gobject);
}

void
ags_core_audio_port_finalize(GObject *gobject)
{
  AgsCoreAudioPort *core_audio_port;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

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

  ags_audio_buffer_util_set_source(core_audio_port->audio_buffer_util,
				   NULL);
      
  ags_audio_buffer_util_set_destination(core_audio_port->audio_buffer_util,
					NULL);

  ags_audio_buffer_util_free(core_audio_port->audio_buffer_util);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_port_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_port_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;
  
  AgsUUID *ptr;

  GRecMutex *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port signal mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* get UUID */
  g_rec_mutex_lock(core_audio_port_mutex);

  ptr = core_audio_port->uuid;

  g_rec_mutex_unlock(core_audio_port_mutex);
  
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

  GRecMutex *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check is ready */
  g_rec_mutex_lock(core_audio_port_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (core_audio_port->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_port_mutex);
  
  return(is_ready);
}

void
ags_core_audio_port_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  GRecMutex *core_audio_port_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  GRecMutex *core_audio_port_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(core_audio_port_mutex);
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

  GRecMutex *core_audio_port_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check is connected */
  g_rec_mutex_lock(core_audio_port_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (core_audio_port->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_port_mutex);
  
  return(is_connected);
}

void
ags_core_audio_port_connect(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  GRecMutex *core_audio_port_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  GRecMutex *core_audio_port_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(core_audio_port_mutex);
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
 * Since: 3.0.0
 */
gboolean
ags_core_audio_port_test_flags(AgsCoreAudioPort *core_audio_port, AgsCoreAudioPortFlags flags)
{
  gboolean retval;  
  
  GRecMutex *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return(FALSE);
  }

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* test */
  g_rec_mutex_lock(core_audio_port_mutex);

  retval = (flags & (core_audio_port->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

  return(retval);
}

/**
 * ags_core_audio_port_set_flags:
 * @core_audio_port: the #AgsCoreAudioPort
 * @flags: see #AgsCoreAudioPortFlags-enum
 *
 * Enable a feature of @core_audio_port.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_port_set_flags(AgsCoreAudioPort *core_audio_port, AgsCoreAudioPortFlags flags)
{
  GRecMutex *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->flags |= flags;
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}
    
/**
 * ags_core_audio_port_unset_flags:
 * @core_audio_port: the #AgsCoreAudioPort
 * @flags: see #AgsCoreAudioPortFlags-enum
 *
 * Disable a feature of @core_audio_port.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_port_unset_flags(AgsCoreAudioPort *core_audio_port, AgsCoreAudioPortFlags flags)
{  
  GRecMutex *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->flags &= (~flags);
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}

/**
 * ags_core_audio_port_find:
 * @core_audio_port: (element-type AgsAudio.CoreAudioPort) (transfer none): the #GList-struct containing #AgsCoreAudioPort
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @core_audio_port.
 *
 * Returns: (element-type AgsAudio.CoreAudioPort) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_core_audio_port_find(GList *core_audio_port,
			 gchar *port_name)
{
  gboolean success;
  
  GRecMutex *core_audio_port_mutex;

  while(core_audio_port != NULL){
    /* get core audio port mutex */
    core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port->data);

    /* check port name */
    g_rec_mutex_lock(core_audio_port_mutex);

    success = (!g_ascii_strcasecmp(AGS_CORE_AUDIO_PORT(core_audio_port->data)->port_name,
				   port_name)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(core_audio_port_mutex);

    if(success){
      return(core_audio_port);
    }

    core_audio_port = core_audio_port->next;
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
  ags_core_audio_port_output_run_loop = CFRunLoopGetCurrent();

  g_atomic_int_set(&ags_core_audio_port_output_run_loop_initialized,
		   TRUE);
  
  do{
    CFRunLoopRunInMode(kCFRunLoopDefaultMode,
		       0,
		       TRUE);
  }while(g_atomic_int_get(&(core_audio_port->output_running)));
  
  g_thread_exit(NULL);

  return(NULL);
}

void*
ags_core_audio_port_input_thread(AgsCoreAudioPort *core_audio_port)
{
  ags_core_audio_port_input_run_loop = CFRunLoopGetCurrent();

  g_atomic_int_set(&ags_core_audio_port_input_run_loop_initialized,
		   TRUE);
  
  do{
    CFRunLoopRunInMode(kCFRunLoopDefaultMode,
		       0,
		       TRUE);
  }while(g_atomic_int_get(&(core_audio_port->input_running)));
  
  g_thread_exit(NULL);

  return(NULL);
}

OSStatus
ags_core_audio_port_hw_output_callback(AudioObjectID device,
				       const AudioTimeStamp* time_now,
				       const AudioBufferList* in,
				       const AudioTimeStamp* in_time,
				       AudioBufferList* out,
				       const AudioTimeStamp* out_tim,
				       AgsCoreAudioPort *core_audio_port)
{
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioDevin *core_audio_devin;

  GObject *soundcard;

  AudioBuffer *out_buffer;
  
  gpointer buffer;

  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint copy_mode;
  guint i;
  gboolean is_playing;
  gboolean pass_through;
  gboolean no_event;
  gboolean empty_run;
  
  GRecMutex *core_audio_port_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  if(core_audio_port == NULL){
    return(-1);
  }

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  if(g_atomic_int_get(&(core_audio_port->queued)) > 0){
    g_warning("drop core audio output callback");
    
    return(0);
  }else{
    g_atomic_int_inc(&(core_audio_port->queued));
  }
  
  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_devout = (AgsCoreAudioDevout *) core_audio_port->core_audio_device;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

  core_audio_devin = NULL;
  
  soundcard = NULL;
  
  if(core_audio_devout != NULL){
    soundcard = (GObject *) core_audio_devout;
  }else if(core_audio_devin != NULL){
    soundcard = (GObject *) core_audio_devin;
  }

  is_playing = FALSE;

    /* wait callback */
  is_playing = ags_soundcard_is_playing(AGS_SOUNDCARD(core_audio_devout));

  if(is_playing){
    g_atomic_int_and(&(core_audio_devout->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH)));
  }
  
  no_event = TRUE;
  
  if(is_playing){
    callback_mutex = &(core_audio_devout->callback_mutex);
	
    /* give back computing time until ready */
    if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);
      
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devout->sync_flags),
		      AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT);
	
	while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	  g_cond_wait(&(core_audio_devout->callback_cond),
		      callback_mutex);
	}
      }else{
	g_atomic_int_and(&(core_audio_devout->sync_flags),
			 (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));
      }
    
      g_atomic_int_and(&(core_audio_devout->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT |
			  AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE)));
    
      g_mutex_unlock(callback_mutex);
    
      no_event = FALSE;
    }
  }
  
  // g_message("p %d", played_cache);
  out_buffer = out->mBuffers;
  
  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
				     out_buffer->mData, 1,
				     out_buffer->mNumberChannels * (out_buffer->mDataByteSize / sizeof(float)), AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(is_playing){
    pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
    
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;
    
    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      &pcm_channels,
			      NULL,
			      &buffer_size,
			      &format);
  
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								AGS_AUDIO_BUFFER_UTIL_FLOAT,
								ags_audio_buffer_util_format_from_soundcard(core_audio_port->audio_buffer_util,
													    format));
  
    buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(soundcard));

    //TODO:JK: improve misconfigured hw
    if(out_buffer->mDataByteSize / sizeof(float) >= buffer_size &&
       out_buffer->mNumberChannels >= pcm_channels){
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(soundcard),
				buffer);
  
      for(i = 0; i < pcm_channels; i++){
	ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						    out_buffer->mData, out_buffer->mNumberChannels, i,
						    buffer, pcm_channels, i,
						    buffer_size, copy_mode);
      }
      
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(soundcard),
				  buffer);
    }else if(out_buffer->mDataByteSize / sizeof(float) >= buffer_size){
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(soundcard),
				buffer);
  
      for(i = 0; i < pcm_channels && i < out_buffer->mNumberChannels; i++){
	ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						    out_buffer->mData, out_buffer->mNumberChannels, i,
						    buffer, pcm_channels, i,
						    buffer_size, copy_mode);
      }
      
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(soundcard),
				  buffer);      
    }else{
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(soundcard),
				buffer);
  
      for(i = 0; i < pcm_channels && i < out_buffer->mNumberChannels; i++){
	ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						    out_buffer->mData, out_buffer->mNumberChannels, i,
						    buffer, pcm_channels, i,
						    out_buffer->mDataByteSize / sizeof(float), copy_mode);
      }
      
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(soundcard),
				  buffer);
    }
  }else{
    empty_run = TRUE;
  }
  
  /* signal finish */ 
  if(!no_event){
    callback_finish_mutex = &(core_audio_devout->callback_finish_mutex);
	
    g_mutex_lock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE);
    
    if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
      g_cond_signal(&(core_audio_devout->callback_finish_cond));
    }

    g_mutex_unlock(callback_finish_mutex);
  }

  if(empty_run){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }
    
  g_atomic_int_dec_and_test(&(core_audio_port->queued));
  
  return(noErr);
}


OSStatus
ags_core_audio_port_hw_input_callback(AudioObjectID device,
				      const AudioTimeStamp* time_now,
				      const AudioBufferList* in,
				      const AudioTimeStamp* in_time,
				      AudioBufferList* out,
				      const AudioTimeStamp* out_tim,
				      AgsCoreAudioPort *core_audio_port)
{
  AgsCoreAudioDevin *core_audio_devin;
  AgsCoreAudioDevout *core_audio_devout;

  GObject *soundcard;

  AudioBuffer *in_buffer;
  
  gpointer buffer;

  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint copy_mode;
  guint i;
  gboolean is_recording;
  gboolean pass_through;
  gboolean no_event;
  gboolean empty_run;
  
  GRecMutex *core_audio_port_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  if(core_audio_port == NULL){
    return(-1);
  }

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  if(g_atomic_int_get(&(core_audio_port->queued)) > 0){
    g_warning("drop core audio input callback");
    
    return(0);
  }else{
    g_atomic_int_inc(&(core_audio_port->queued));
  }
  
  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_devin = (AgsCoreAudioDevin *) core_audio_port->core_audio_device;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

  core_audio_devout = NULL;
  
  soundcard = NULL;
  
  if(core_audio_devout != NULL){
    soundcard = (GObject *) core_audio_devout;
  }else if(core_audio_devin != NULL){
    soundcard = (GObject *) core_audio_devin;
  }

  is_recording = FALSE;

    /* wait callback */
  is_recording = ags_soundcard_is_recording(AGS_SOUNDCARD(core_audio_devin));

  if(is_recording){
    g_atomic_int_and(&(core_audio_devin->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVIN_PASS_THROUGH)));
  }
  
  no_event = TRUE;
  
  if(is_recording){
    callback_mutex = &(core_audio_devin->callback_mutex);
	
    /* give back computing time until ready */
    if((AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);
      
      if((AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devin->sync_flags),
		      AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT);
	
	while((AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	  g_cond_wait(&(core_audio_devin->callback_cond),
		      callback_mutex);
	}
      }else{
	g_atomic_int_and(&(core_audio_devin->sync_flags),
			 (~AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK));
      }
    
      g_atomic_int_and(&(core_audio_devin->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT |
			  AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE)));
    
      g_mutex_unlock(callback_mutex);
    
      no_event = FALSE;
    }
  }
  
  // g_message("p %d", played_cache);
  in_buffer = in->mBuffers;
  
  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
				     in_buffer->mData, 1,
				     in_buffer->mNumberChannels * (in_buffer->mDataByteSize / sizeof(float)), AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(is_recording){
    pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
    
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    
    format = AGS_SOUNDCARD_DEFAULT_FORMAT;

    ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			      &pcm_channels,
			      NULL,
			      &buffer_size,
			      &format);
  
    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(core_audio_port->audio_buffer_util,
													    format),
								AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
    buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(soundcard));

    //TODO:JK: improve misconfigured hw
    if(in_buffer->mDataByteSize / sizeof(float) >= buffer_size &&
       in_buffer->mNumberChannels >= pcm_channels){
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(soundcard),
				buffer);

      for(i = 0; i < pcm_channels; i++){
	ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						    buffer, pcm_channels, i,
						    in_buffer->mData, in_buffer->mNumberChannels, i,
						    buffer_size, copy_mode);
      }
      
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(soundcard),
				  buffer);
    }else if(in_buffer->mDataByteSize / sizeof(float) >= buffer_size){
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(soundcard),
				buffer);

      for(i = 0; i < pcm_channels && i < in_buffer->mNumberChannels; i++){
	ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						    buffer, pcm_channels, i,
						    in_buffer->mData, in_buffer->mNumberChannels, i,
						    buffer_size, copy_mode);
      }
      
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(soundcard),
				  buffer);
    }else{
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(soundcard),
				buffer);

      for(i = 0; i < pcm_channels && i < in_buffer->mNumberChannels; i++){
	ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						    buffer, pcm_channels, i,
						    in_buffer->mData, in_buffer->mNumberChannels, i,
						    in_buffer->mDataByteSize / sizeof(float), copy_mode);
      }
      
      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(soundcard),
				  buffer);
    }
  }else{
    empty_run = TRUE;
  }
  
  /* signal finish */ 
  if(!no_event){
    callback_finish_mutex = &(core_audio_devin->callback_finish_mutex);
	
    g_mutex_lock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_devin->sync_flags),
		    AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE);
    
    if((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
      g_cond_signal(&(core_audio_devin->callback_finish_cond));
    }

    g_mutex_unlock(callback_finish_mutex);
  }

  if(empty_run){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }
    
  g_atomic_int_dec_and_test(&(core_audio_port->queued));

  return(noErr);
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
 * Since: 3.0.0
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

  AgsSoundcardFormat format;
  guint i;
  gboolean use_cache;

#ifdef AGS_WITH_CORE_AUDIO
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
  Float64 output_samplerate;
  Float64 input_samplerate;

  UInt32 property_size;
  int output_buffer_size_bytes;
  int input_buffer_size_bytes;
#else
  AUGraph *graph;
#endif
  OSStatus retval;
#else
  gpointer graph;
#endif
  
  GRecMutex *core_audio_client_mutex;
  GRecMutex *core_audio_port_mutex;

  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
     port_name == NULL){
    return;
  }

  g_object_get(core_audio_port,
	       "core-audio-client", &core_audio_client,
	       NULL);
  
  if(core_audio_client == NULL){
    g_warning("ags_core_audio_port.c - no assigned AgsCoreAudioClient");
    
    return;
  }

  if(ags_core_audio_port_test_flags(core_audio_port, AGS_CORE_AUDIO_PORT_REGISTERED)){
    g_object_unref(core_audio_client);
    
    return;
  }

  /* get core audio server and application context */
  g_object_get(core_audio_client,
	       "core-audio-server", &core_audio_server,
	       NULL);
  
  if(core_audio_server == NULL){
    g_object_unref(core_audio_client);

    return;
  }

  /* get core audio client mutex */
  core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(core_audio_client);

  /* get graph */
#ifdef AGS_WITH_CORE_AUDIO
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  g_rec_mutex_lock(core_audio_client_mutex);
  
  graph = core_audio_client->graph;
  
  g_rec_mutex_unlock(core_audio_client_mutex);

  if(graph == NULL){
    g_object_unref(core_audio_client);

    g_object_unref(core_audio_server);
    
    return;
  }
#endif
#endif
  
  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);
  
  /* get port name */
  //FIXME:JK: memory leak?
  g_rec_mutex_lock(core_audio_port_mutex);

  port_name = g_strdup(core_audio_port->port_name);

  format = core_audio_port->format;
  
  use_cache = core_audio_port->use_cache;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

  /* create sequencer or soundcard */
  if(is_output){
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_OUTPUT);
  }else{
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_INPUT);
  }

  if(is_audio){  
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_AUDIO);

    if(is_output){
#ifdef AGS_WITH_CORE_AUDIO
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
      AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
				     core_audio_port->output_property_address,
				     0,
				     NULL,
				     &property_size);
	
      AudioObjectGetPropertyData(kAudioObjectSystemObject, 
				 core_audio_port->output_property_address,
				 0, 
				 NULL, 
				 &property_size, 
				 &(core_audio_port->output_device));
      
      output_samplerate = (Float64) core_audio_port->samplerate;

      AudioObjectSetPropertyData(core_audio_port->output_device,
				 core_audio_port->output_samplerate_property_address,
				 0,
				 NULL,
				 sizeof(output_samplerate),
				 &output_samplerate);

      output_buffer_size_bytes = core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gfloat);

      AudioObjectSetPropertyData(core_audio_port->output_device,
				 core_audio_port->output_buffer_size_property_address,
				 0,
				 NULL,
				 sizeof(output_buffer_size_bytes),
				 &output_buffer_size_bytes);
      
      AudioDeviceCreateIOProcID(core_audio_port->output_device,
				(OSStatus (*)(AudioObjectID inDevice, const AudioTimeStamp *inNow, const AudioBufferList *inInputData, const AudioTimeStamp *inInputTime, AudioBufferList *outOutputData, const AudioTimeStamp *inOutputTime, void *inClientData)) ags_core_audio_port_hw_output_callback,
				core_audio_port,
				&(core_audio_port->output_proc_id));

      AudioDeviceStart(core_audio_port->output_device,
		       core_audio_port->output_proc_id);
#else
      GThread *thread;

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

      if(!g_atomic_int_get(&ags_core_audio_port_output_run_loop_initialized)){
	g_atomic_int_set(&(core_audio_port->output_running),
			 TRUE);
	thread = g_thread_new("Advanced Gtk+ Sequencer - core-audio output",
			      ags_core_audio_port_output_thread,
			      core_audio_port);

	while(!g_atomic_int_get(&ags_core_audio_port_output_run_loop_initialized)){
	  usleep(400);
	}
      }

      if(use_cache){
	AudioQueueNewOutput(&(core_audio_port->data_format),
			    ,
			    core_audio_port,
			    ags_core_audio_port_output_run_loop, NULL,
			    0,
			    &(core_audio_port->aq_ref));
      }else{
	AudioQueueNewOutput(&(core_audio_port->data_format),
			    ags_core_audio_port_handle_output_buffer,
			    core_audio_port,
			    ags_core_audio_port_output_run_loop, NULL,
			    0,
			    &(core_audio_port->aq_ref));
      }
      
      for(i = 0; i < 8; i++){
	if(use_cache){
	  AudioQueueAllocateBuffer(core_audio_port->aq_ref,
				   (core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint16)),
				   &(core_audio_port->buf_ref[i]));
	}else{
	  AudioQueueAllocateBuffer(core_audio_port->aq_ref,
				   (core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gint16)),
				   &(core_audio_port->buf_ref[i]));
	}
	
	ags_core_audio_port_handle_output_buffer(core_audio_port,
						 core_audio_port->aq_ref,
						 core_audio_port->buf_ref[i]);
      }
      
      AudioQueueSetParameter(core_audio_port->aq_ref, 
			     kAudioQueueParam_Volume, 1.0);
    
      AudioQueueStart(core_audio_port->aq_ref,
		      NULL);
  
#endif
#endif
    }else{
#ifdef AGS_WITH_CORE_AUDIO
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
      AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
				     core_audio_port->input_property_address,
				     0,
				     NULL,
				     &property_size);
	
      AudioObjectGetPropertyData(kAudioObjectSystemObject, 
				 core_audio_port->input_property_address,
				 0, 
				 NULL, 
				 &property_size, 
				 &(core_audio_port->input_device));
      
      input_samplerate = (Float64) core_audio_port->samplerate;
      
      AudioObjectSetPropertyData(core_audio_port->input_device,
				 core_audio_port->input_samplerate_property_address,
				 0,
				 NULL,
				 sizeof(input_samplerate),
				 &input_samplerate);

      input_buffer_size_bytes = core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gfloat);

      AudioObjectSetPropertyData(core_audio_port->input_device,
				 core_audio_port->input_buffer_size_property_address,
				 0,
				 NULL,
				 sizeof(input_buffer_size_bytes),
				 &input_buffer_size_bytes);
      
      AudioDeviceCreateIOProcID(core_audio_port->input_device,
				(OSStatus (*)(AudioObjectID inDevice, const AudioTimeStamp *inNow, const AudioBufferList *inInputData, const AudioTimeStamp *inInputTime, AudioBufferList *outOutputData, const AudioTimeStamp *inOutputTime, void *inClientData)) ags_core_audio_port_hw_input_callback,
				core_audio_port,
				&(core_audio_port->input_proc_id));

      AudioDeviceStart(core_audio_port->input_device,
		       core_audio_port->input_proc_id);
#else
      GThread *thread;

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

      if(!g_atomic_int_get(&ags_core_audio_port_input_run_loop_initialized)){
	g_atomic_int_set(&(core_audio_port->input_running),
			 TRUE);
	thread = g_thread_new("Advanced Gtk+ Sequencer - core-audio input",
			      ags_core_audio_port_input_thread,
			      core_audio_port);

	while(!g_atomic_int_get(&ags_core_audio_port_input_run_loop_initialized)){
	  usleep(400);
	}
      }

#if 0
      if(use_cache){
	AudioQueueNewInput(&(core_audio_port->record_format),
			   ags_core_audio_port_cached_handle_input_buffer,
			   core_audio_port,
			   ags_core_audio_port_input_run_loop, kCFRunLoopDefaultMode,
			   0,
			   &(core_audio_port->record_aq_ref));
      }else{
	
      }
#endif
      
      AudioQueueNewInput(&(core_audio_port->record_format),
			 ags_core_audio_port_handle_input_buffer,
			 core_audio_port,
			 ags_core_audio_port_input_run_loop, kCFRunLoopDefaultMode,
			 0,
			 &(core_audio_port->record_aq_ref));
      
      for(i = 0; i < 16; i++){
	AudioQueueAllocateBuffer(core_audio_port->record_aq_ref,
				 (core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gint16)),
				 &(core_audio_port->record_buf_ref[i]));

 	AudioQueueEnqueueBuffer(core_audio_port->record_aq_ref,
				core_audio_port->record_buf_ref[i],
				0,
				NULL);
      }
      
      AudioQueueSetParameter(core_audio_port->record_aq_ref, 
			     kAudioQueueParam_Volume, 1.0);
    
      AudioQueueStart(core_audio_port->record_aq_ref,
		      NULL);
#endif
#endif
    }
  }else if(is_midi){
    ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_MIDI);

    if(is_output){
      //NOTE:JK: not implemented
    }else{
#ifdef AGS_WITH_CORE_AUDIO
      MIDIEndpointRef endpoint;
      
      retval = MIDIClientCreate(CFSTR("Advanced Gtk+ Sequencer - Core MIDI to System Sounds"),
				NULL,
				core_audio_port,
				core_audio_port->midi_client);

      if(retval != noErr){
	goto ags_core_audio_port_register_END;
      }
      
      // Objective-C block      
      retval = MIDIInputPortCreateWithProtocol(*(core_audio_port->midi_client),
					       CFSTR("Input port"),
					       kMIDIProtocol_2_0,
					       core_audio_port->midi_port,
					       ^(const MIDIEventList *evtlist, void *src_conn_ref_con){
						 AgsCoreAudioPort *core_audio_port;
						 AgsCoreAudioMidiin *core_audio_midiin;
  
						 AgsAudioLoop *audio_loop;
  
						 AgsApplicationContext *application_context;

						 // MIDIPacket *packet;
						 MIDIEventPacket *event_packet;
						 
						 guint nth_buffer;
						 gint num_packets;
						 gint i;
						 gboolean no_event;

						 GRecMutex *core_audio_port_mutex;
						 GRecMutex *device_mutex;
						 GMutex *callback_mutex;
						 GMutex *callback_finish_mutex;
						 
						 core_audio_port = AGS_CORE_AUDIO_PORT(src_conn_ref_con);

						 if(core_audio_port == NULL){
						   return;
						 }

						 /* get core-audio port mutex */
						 core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

						 /*
						  * process midi
						  */
						 /*  */  
						 application_context = ags_application_context_get_instance();

						 audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

						 /*  */
						 g_rec_mutex_lock(core_audio_port_mutex);

						 core_audio_midiin = core_audio_port->core_audio_device;

						 g_rec_mutex_unlock(core_audio_port_mutex);
  
						 /* get device mutex */
						 device_mutex = AGS_CORE_AUDIO_MIDIIN_GET_OBJ_MUTEX(core_audio_midiin);
  
						 /*  */
						 g_rec_mutex_lock(device_mutex);

						 callback_mutex = &(core_audio_midiin->callback_mutex);
						 callback_finish_mutex = &(core_audio_midiin->callback_finish_mutex);
						   
						 /* wait callback */
						 g_atomic_int_inc(&(core_audio_port->queued));

						 no_event = TRUE;
  
						 if((AGS_CORE_AUDIO_MIDIIN_PASS_THROUGH & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
						   g_rec_mutex_unlock(device_mutex);
						   
						   /* force wait */
						   g_atomic_int_or(&(core_audio_midiin->sync_flags),
								   AGS_CORE_AUDIO_MIDIIN_DO_SYNC);
    
						   /* wait callback */
						   g_mutex_lock(callback_mutex);

						   if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
						     g_atomic_int_and(&(core_audio_midiin->sync_flags),
								      (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE));
      
						     while((AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0 &&
							   (AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) != 0){
						       g_cond_wait(&(core_audio_midiin->callback_cond),
								   callback_mutex);
						     }
						   }
    
						   g_mutex_unlock(callback_mutex);

						   g_atomic_int_or(&(core_audio_midiin->sync_flags),
								   (AGS_CORE_AUDIO_MIDIIN_CALLBACK_WAIT | AGS_CORE_AUDIO_MIDIIN_CALLBACK_DONE));
    
						   /* lock device */
						   g_rec_mutex_lock(device_mutex);
						 
						   no_event = FALSE;
						 }else{
						   g_atomic_int_set(&(core_audio_port->is_empty),
								    TRUE);
						 }

						 /* get buffer */  
						 nth_buffer = 0;
						 
						 if((AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_0 & (core_audio_midiin->flags)) != 0){
						   nth_buffer = 1;
						 }else if((AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_1 & (core_audio_midiin->flags)) != 0){
						   nth_buffer = 2;
						 }else if((AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_2 & (core_audio_midiin->flags)) != 0){
						   nth_buffer = 3;
						 }else if((AGS_CORE_AUDIO_MIDIIN_APP_BUFFER_3 & (core_audio_midiin->flags)) != 0){
						   nth_buffer = 0;
						 }

						 core_audio_midiin->app_buffer_size[nth_buffer] = 0;
						 
						 memset(core_audio_midiin->app_buffer[nth_buffer], 0, core_audio_midiin->allocated_app_buffer_size[nth_buffer] * sizeof(char));
						 
						 event_packet = &evtlist->packet[0];
						 num_packets = evtlist->numPackets;

						 for(i = 0; i < num_packets; i++){
						   guint length;

						   length = event_packet->wordCount;
    
						   if(length == 0){
						     event_packet = MIDIEventPacketNext(event_packet);
      
						     continue;
						   }

						   if(core_audio_midiin->app_buffer_size[nth_buffer] + (4 * length) >= core_audio_midiin->allocated_app_buffer_size[nth_buffer]){
						     if(core_audio_midiin->app_buffer[nth_buffer] == NULL){
						       core_audio_midiin->app_buffer[nth_buffer] = (char *) g_malloc(AGS_CORE_AUDIO_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));
						     }else{
						       core_audio_midiin->app_buffer[nth_buffer] = (char *) g_realloc(core_audio_midiin->app_buffer[nth_buffer],
														      (core_audio_midiin->allocated_app_buffer_size[nth_buffer] + AGS_CORE_AUDIO_MIDIIN_DEFAULT_BUFFER_SIZE) * sizeof(char));
						     }
		  
						     memset(core_audio_midiin->app_buffer[nth_buffer] + core_audio_midiin->allocated_app_buffer_size[nth_buffer], 0, AGS_CORE_AUDIO_MIDIIN_DEFAULT_BUFFER_SIZE * sizeof(char));

						     core_audio_midiin->allocated_app_buffer_size[nth_buffer] = core_audio_midiin->allocated_app_buffer_size[nth_buffer] + AGS_CORE_AUDIO_MIDIIN_DEFAULT_BUFFER_SIZE;
						   }

						   memcpy(core_audio_midiin->app_buffer[nth_buffer] + core_audio_midiin->app_buffer_size[nth_buffer], event_packet->words, (4 * length));
		
						   core_audio_midiin->app_buffer_size[nth_buffer] = core_audio_midiin->app_buffer_size[nth_buffer] + (4 * length);

						   event_packet = MIDIEventPacketNext(event_packet);
						 }

						 g_rec_mutex_unlock(device_mutex);

						 /* signal finish */  
						 if(!no_event){
						   /* signal client - wait callback finish */
						   g_mutex_lock(callback_finish_mutex);

						   g_atomic_int_and(&(core_audio_midiin->sync_flags),
								    (~AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_WAIT));

						   if((AGS_CORE_AUDIO_MIDIIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_midiin->sync_flags)))) == 0){
						     g_cond_signal(&(core_audio_midiin->callback_finish_cond));
						   }

						   g_mutex_unlock(callback_finish_mutex);
						 }

						 g_atomic_int_dec_and_test(&(core_audio_port->queued));
					       });
	
      if(retval != noErr){
	goto ags_core_audio_port_register_END;
      }

      endpoint = MIDIGetSource(core_audio_port->midi_port_number);

      if(endpoint == 0){
	goto ags_core_audio_port_register_END;
      }
      
      retval = MIDIPortConnectSource(*(core_audio_port->midi_port), endpoint, core_audio_port);

      if(retval != noErr){
	goto ags_core_audio_port_register_END;
      }
 #endif
    }
  }
  
  ags_core_audio_port_set_flags(core_audio_port, AGS_CORE_AUDIO_PORT_REGISTERED);

ags_core_audio_port_register_END:
  
  g_object_unref(core_audio_client);

  g_object_unref(core_audio_server);    
}

void
ags_core_audio_port_unregister(AgsCoreAudioPort *core_audio_port)
{
  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }
  
  g_atomic_int_set(&(core_audio_port->output_running),
		   FALSE);
  
  g_atomic_int_set(&(core_audio_port->input_running),
		   FALSE);
  
#ifdef AGS_WITH_CORE_AUDIO
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
  if(ags_core_audio_port_test_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_AUDIO)){
    if(ags_core_audio_port_test_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_OUTPUT)){
      AudioDeviceStop(core_audio_port->output_device,
		      core_audio_port->output_proc_id);
    }else{
      AudioDeviceStop(core_audio_port->input_device,
		      core_audio_port->input_proc_id);
    }
  }
#else
  //NOTE:JK: not implemented
#endif
  
  if(ags_core_audio_port_test_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_INPUT) &&
     ags_core_audio_port_test_flags(core_audio_port, AGS_CORE_AUDIO_PORT_IS_MIDI)){
    MIDIEndpointRef endpoint;

    endpoint = MIDIGetSource(core_audio_port->midi_port_number);

    MIDIPortDisconnectSource(*(core_audio_port->midi_port), endpoint);
    MIDIPortDispose(*(core_audio_port->midi_port));

    core_audio_port->midi_port = NULL;

    MIDIClientDispose(*(core_audio_port->midi_client));

    core_audio_port->midi_client = NULL;
  }
#endif
  
  ags_core_audio_port_unset_flags(core_audio_port, AGS_CORE_AUDIO_PORT_REGISTERED);
}

#ifdef AGS_WITH_CORE_AUDIO
void
ags_core_audio_port_cached_handle_output_buffer(AgsCoreAudioPort *core_audio_port,
						AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer)
{
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioDevin *core_audio_devin;
    
  AgsAudioLoop *audio_loop;

  AgsApplicationContext *application_context;

  GObject *soundcard;

  struct timespec idle_time = {
    0,
    0,
  };
    
  guint current_cache;
  guint next_cache, completed_cache;
  guint played_cache;
  guint word_size;
  guint frame_size;
  guint copy_mode;
  gboolean is_playing;
  
  GRecMutex *core_audio_port_mutex;

  if(core_audio_port == NULL){
    return;
  }

  application_context = ags_application_context_get_instance();

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_devout = (AgsCoreAudioDevout *) core_audio_port->core_audio_device;
  
  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(gint16);

      copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								  AGS_AUDIO_BUFFER_UTIL_S16,
								  AGS_AUDIO_BUFFER_UTIL_S16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      word_size = sizeof(gint32);

      copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								  AGS_AUDIO_BUFFER_UTIL_S16,
								  AGS_AUDIO_BUFFER_UTIL_S24);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(gint32);

      copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								  AGS_AUDIO_BUFFER_UTIL_S16,
								  AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  }

  frame_size = core_audio_port->pcm_channels * core_audio_port->cache_buffer_size;
  in_buffer->mAudioDataByteSize = frame_size * sizeof(gint16);
  
  current_cache = core_audio_port->current_cache;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

  soundcard = NULL;
  
  if(core_audio_devout != NULL){
    soundcard = (GObject *) core_audio_devout;
  }else if(core_audio_devin != NULL){
    soundcard = (GObject *) core_audio_devin;
  }

  is_playing = FALSE;
  
  if(ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
    if(current_cache == 3){
      next_cache = 0;
    }else{
      next_cache = current_cache + 1;
    }

    is_playing = TRUE;
  }else{
    next_cache = 0;
  }
  
  /* wait until cache ready */
  g_rec_mutex_lock(core_audio_port_mutex);

  completed_cache = core_audio_port->completed_cache;

  g_rec_mutex_unlock(core_audio_port_mutex);

  if(is_playing){
    idle_time.tv_nsec = ags_core_audio_port_get_latency(core_audio_port) / 8;
    
    while(next_cache == completed_cache &&
	  ags_soundcard_is_playing(AGS_SOUNDCARD(soundcard))){
      ags_time_nanosleep(&idle_time);

      g_rec_mutex_lock(core_audio_port_mutex);

      completed_cache = core_audio_port->completed_cache;

      g_rec_mutex_unlock(core_audio_port_mutex);
    }

    if(current_cache == 0){
      played_cache = 3;
    }else{
      played_cache = current_cache - 1;
    }
  }else{
    played_cache = 0;
  }

  // g_message("p %d", played_cache);
  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
				     in_buffer->mAudioData, 1,
				     (in_buffer->mAudioDataByteSize / sizeof(gint16)), AGS_AUDIO_BUFFER_UTIL_S16);
  ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
					      in_buffer->mAudioData, 1, 0,
					      core_audio_port->cache[played_cache], 1, 0,
					      frame_size, copy_mode);

  AudioQueueEnqueueBuffer(core_audio_port->aq_ref,
			  in_buffer,
			  0,
			  NULL);
 
  /* seek current cache */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->current_cache = next_cache;

  g_rec_mutex_unlock(core_audio_port_mutex);

  /* unref */
  g_object_unref(audio_loop);
}

void
ags_core_audio_port_cached_handle_input_buffer(AgsCoreAudioPort *core_audio_port,
					       AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer,
					       const AudioTimeStamp *in_start_time, UInt32 in_num_packets,
					       const AudioStreamPacketDescription *in_packet_desc)
{
  //TODO:JK: implement me
}

void
ags_core_audio_port_handle_output_buffer(AgsCoreAudioPort *core_audio_port,
					 AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  AgsAudioLoop *audio_loop;

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

  GRecMutex *core_audio_port_mutex;
  GRecMutex *device_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  if(core_audio_port == NULL){
    return;
  }
  
  /* get core-audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

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

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  in_buffer->mAudioDataByteSize = core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gint16);
  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
				     in_buffer->mAudioData, 1,
				     (in_buffer->mAudioDataByteSize / sizeof(gint16)), AGS_AUDIO_BUFFER_UTIL_S16);

  if(audio_loop == NULL){
    g_atomic_int_dec_and_test(&(core_audio_port->queued));

    return;
  }
  
  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_devout = core_audio_port->core_audio_device;

  g_rec_mutex_unlock(core_audio_port_mutex);

  /* get device mutex */
  device_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

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

  g_rec_mutex_unlock(core_audio_port_mutex);
  
  /* wait callback */
  is_playing = ags_soundcard_is_playing(AGS_SOUNDCARD(core_audio_devout));

  if(is_playing){
    g_atomic_int_and(&(core_audio_devout->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH)));
  }
  
  no_event = TRUE;
  
  if(is_playing){
    callback_mutex = &(core_audio_devout->callback_mutex);
	
    /* give back computing time until ready */
    if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);
      
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devout->sync_flags),
		      AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT);
	
	while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	  g_cond_wait(&(core_audio_devout->callback_cond),
		      callback_mutex);
	}
      }else{
	g_atomic_int_and(&(core_audio_devout->sync_flags),
			 (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));
      }
    
      g_atomic_int_and(&(core_audio_devout->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT |
			  AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE)));
    
      g_mutex_unlock(callback_mutex);
    
      no_event = FALSE;
    }
  }

  /* get buffer */
  g_rec_mutex_lock(device_mutex);
  
  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    nth_buffer = 7;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    nth_buffer = 0;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    nth_buffer = 1;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
      nth_buffer = 2;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    nth_buffer = 3;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    nth_buffer = 4;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    nth_buffer = 5;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
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
  
  g_rec_mutex_unlock(device_mutex);
  
  /* get copy mode */
  if(!empty_run &&
     is_playing){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     FALSE);

    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								AGS_AUDIO_BUFFER_UTIL_S16,
								ags_audio_buffer_util_format_from_soundcard(core_audio_port->audio_buffer_util,
													    core_audio_devout->format));

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(core_audio_devout), core_audio_devout->app_buffer[nth_buffer]);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						in_buffer->mAudioData, 1, 0,
						core_audio_devout->app_buffer[nth_buffer], 1, 0,
						core_audio_port->pcm_channels * core_audio_port->buffer_size, copy_mode);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(core_audio_devout), core_audio_devout->app_buffer[nth_buffer]);
  }

  AudioQueueEnqueueBuffer(core_audio_port->aq_ref,
			  in_buffer,
			  0,
			  NULL);
  
  /* signal finish */ 
  if(!no_event){
    callback_finish_mutex = &(core_audio_devout->callback_finish_mutex);
	
    g_mutex_lock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE);
    
    if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
      g_cond_signal(&(core_audio_devout->callback_finish_cond));
    }

    g_mutex_unlock(callback_finish_mutex);
  }

  if(empty_run){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }
    
  g_atomic_int_dec_and_test(&(core_audio_port->queued));

  /* unref */
  g_object_unref(audio_loop);
}

void
ags_core_audio_port_handle_input_buffer(AgsCoreAudioPort *core_audio_port,
					AudioQueueRef in_audio_queue, AudioQueueBufferRef in_buffer,
					const AudioTimeStamp *in_start_time, UInt32 in_num_packets,
					const AudioStreamPacketDescription *in_packet_desc)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  AgsAudioLoop *audio_loop;
  
  AgsApplicationContext *application_context;

  guint word_size;
  guint nth_buffer;
  guint copy_mode;
  guint count;
  guint i;
  gboolean is_recording;
  gboolean pass_through;
  gboolean no_event;
  gboolean empty_run;

  GRecMutex *core_audio_port_mutex;
  GRecMutex *device_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  if(core_audio_port == NULL){
    return;
  }
  
  /* get core-audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

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

  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));

  if(audio_loop == NULL){
    g_atomic_int_dec_and_test(&(core_audio_port->queued));

    return;
  }
  
  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_devin = core_audio_port->core_audio_device;

  g_rec_mutex_unlock(core_audio_port_mutex);

  /* get device mutex */
  device_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

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

  g_rec_mutex_unlock(core_audio_port_mutex);
  
  /* wait callback */
  is_recording = ags_soundcard_is_recording(AGS_SOUNDCARD(core_audio_devin));

  if(is_recording){
    g_atomic_int_and(&(core_audio_devin->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVIN_PASS_THROUGH)));
  }
  
  no_event = TRUE;
  
  if(is_recording){
    callback_mutex = &(core_audio_devin->callback_mutex);
	
    /* give back computing time until ready */
    if((AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);
      
      if((AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devin->sync_flags),
		      AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT);
	
	while((AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	  g_cond_wait(&(core_audio_devin->callback_cond),
		      callback_mutex);
	}
      }else{
	g_atomic_int_and(&(core_audio_devin->sync_flags),
			 (~AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK));
      }

      g_atomic_int_and(&(core_audio_devin->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT |
			  AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE)));
    
      g_mutex_unlock(callback_mutex);

      no_event = FALSE;
    }
  }

  /* get buffer */
  g_rec_mutex_lock(device_mutex);
  
  if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0){
    nth_buffer = 0;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1){
    nth_buffer = 1;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2){
    nth_buffer = 2;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3){
    nth_buffer = 3;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4){
    nth_buffer = 4;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5){
    nth_buffer = 5;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6){
    nth_buffer = 6;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7){
    nth_buffer = 7;
  }else{
    empty_run = TRUE;
  }

  word_size = 0;
  
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
  
  g_rec_mutex_unlock(device_mutex);

  /* get copy mode */
  if(!empty_run &&
     is_recording){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     FALSE);

    copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(core_audio_port->audio_buffer_util,
								ags_audio_buffer_util_format_from_soundcard(core_audio_port->audio_buffer_util,
													    core_audio_devin->format),
								AGS_AUDIO_BUFFER_UTIL_S16);

#if 0
    g_message("%d: %x %x %x %x", core_audio_port->pcm_channels * core_audio_port->buffer_size,
	      ((gint16 *) in_buffer->mAudioData)[0],
	      ((gint16 *) in_buffer->mAudioData)[1],
	      ((gint16 *) in_buffer->mAudioData)[2],
	      ((gint16 *) in_buffer->mAudioData)[3]);
#endif

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(core_audio_devin), core_audio_devin->app_buffer[nth_buffer]);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(core_audio_port->audio_buffer_util,
						core_audio_devin->app_buffer[nth_buffer], 1, 0,
						in_buffer->mAudioData, 1, 0,
						core_audio_port->pcm_channels * core_audio_port->buffer_size, copy_mode);
    
    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(core_audio_devin), core_audio_devin->app_buffer[nth_buffer]);    
  }

  in_buffer->mAudioDataByteSize = core_audio_port->pcm_channels * core_audio_port->buffer_size * sizeof(gint16);
  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
				     in_buffer->mAudioData, 1,
				     (in_buffer->mAudioDataByteSize / sizeof(gint16)), AGS_AUDIO_BUFFER_UTIL_S16);
  AudioQueueEnqueueBuffer(core_audio_port->record_aq_ref,
			  in_buffer,
			  0,
			  NULL);
  
  /* signal finish */ 
  if(!no_event){
    callback_finish_mutex = &(core_audio_devin->callback_finish_mutex);
	
    g_mutex_lock(callback_finish_mutex);

    g_atomic_int_or(&(core_audio_devin->sync_flags),
		    AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE);
    
    if((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
      g_cond_signal(&(core_audio_devin->callback_finish_cond));
    }

    g_mutex_unlock(callback_finish_mutex);
  }

  if(empty_run){
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }
    
  g_atomic_int_dec_and_test(&(core_audio_port->queued));

  /* unref */
  g_object_unref(audio_loop);
}
  
void
ags_core_audio_port_midi_notify_callback(const MIDINotification *message,
					 void *ref_con)
{
  //NOTE:JK: unused
}
#endif

void
ags_core_audio_port_set_format(AgsCoreAudioPort *core_audio_port,
			       AgsSoundcardFormat format)
{
  GRecMutex *core_audio_port_mutex;

  /* get core-audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->format = format;
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_samplerate(AgsCoreAudioPort *core_audio_port,
				   guint samplerate)
{
  GRecMutex *core_audio_port_mutex;

  /* get core-audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mSampleRate = (float) samplerate;
#endif

  core_audio_port->samplerate = samplerate;
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_buffer_size(AgsCoreAudioPort *core_audio_port,
				    guint buffer_size)
{  
  GRecMutex *core_audio_port_mutex;

  /* get core-audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

#ifdef AGS_WITH_CORE_AUDIO
  //TODO:JK: implement me
#endif

  core_audio_port->buffer_size = buffer_size;
  
  g_rec_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_pcm_channels(AgsCoreAudioPort *core_audio_port,
				     guint pcm_channels)
{
  GRecMutex *core_audio_port_mutex;

  /* get core-audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /*  */
  g_rec_mutex_lock(core_audio_port_mutex);

  core_audio_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mChannelsPerFrame = pcm_channels;
#endif

  g_rec_mutex_unlock(core_audio_port_mutex);
}

void
ags_core_audio_port_set_cache_buffer_size(AgsCoreAudioPort *core_audio_port,
					  guint cache_buffer_size)
{
  GRecMutex *core_audio_port_mutex;
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);
    
  /* lock core audio port */
  g_rec_mutex_lock(core_audio_port_mutex);

  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      core_audio_port->cache[0] = (void *) realloc(core_audio_port->cache[0],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[1] = (void *) realloc(core_audio_port->cache[1],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[2] = (void *) realloc(core_audio_port->cache[2],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[3] = (void *) realloc(core_audio_port->cache[3],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      core_audio_port->cache[0] = (void *) realloc(core_audio_port->cache[0],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[1] = (void *) realloc(core_audio_port->cache[1],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[2] = (void *) realloc(core_audio_port->cache[2],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[3] = (void *) realloc(core_audio_port->cache[3],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      core_audio_port->cache[0] = (void *) realloc(core_audio_port->cache[0],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[1] = (void *) realloc(core_audio_port->cache[1],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[2] = (void *) realloc(core_audio_port->cache[2],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
      core_audio_port->cache[3] = (void *) realloc(core_audio_port->cache[3],
						   core_audio_port->pcm_channels * core_audio_port->cache_buffer_size * sizeof(gint32));
    }
    break;
  default:
    g_warning("core_audio devout - unsupported format");
  }

  g_rec_mutex_unlock(core_audio_port_mutex);
#endif
}

/**
 * ags_core_audio_port_get_latency:
 * @core_audio_port: the #AgsCoreAudioPort
 * 
 * Gets latency.
 * 
 * Since: 3.0.0
 */
guint
ags_core_audio_port_get_latency(AgsCoreAudioPort *core_audio_port)
{
  guint latency;

  GRecMutex *core_audio_port_mutex;

  /* get core audio port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);
    
  /* lock core audio port */
  g_rec_mutex_lock(core_audio_port_mutex);

  latency = 0;
  
#ifdef AGS_WITH_CORE_AUDIO
  if(core_audio_port->use_cache){
    latency = (guint) floor((gdouble) NSEC_PER_SEC / (gdouble) core_audio_port->samplerate * (gdouble) core_audio_port->cache_buffer_size);
  }else{
    latency = (guint) floor((gdouble) NSEC_PER_SEC / (gdouble) core_audio_port->samplerate * (gdouble) core_audio_port->buffer_size);
  }
#endif

  g_rec_mutex_unlock(core_audio_port_mutex);

  return(latency);
}

/**
 * ags_core_audio_port_new:
 * @core_audio_client: the #AgsCoreAudioClient assigned to
 *
 * Create a new instance of #AgsCoreAudioPort.
 *
 * Returns: the new #AgsCoreAudioPort
 *
 * Since: 3.0.0
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
