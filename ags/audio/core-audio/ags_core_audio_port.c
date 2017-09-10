/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_polling_thread.h>

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
#endif

#include <time.h>

void ags_core_audio_port_class_init(AgsCoreAudioPortClass *core_audio_port);
void ags_core_audio_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_port_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_core_audio_port_init(AgsCoreAudioPort *core_audio_port);
void ags_core_audio_port_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_core_audio_port_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_core_audio_port_connect(AgsConnectable *connectable);
void ags_core_audio_port_disconnect(AgsConnectable *connectable);
void ags_core_audio_port_dispose(GObject *gobject);
void ags_core_audio_port_finalize(GObject *gobject);

#ifdef AGS_WITH_CORE_AUDIO
void* ags_core_audio_port_output_thread(void **data);
OSStatus ags_core_audio_port_output_callback(void *in_user_data, AudioUnitRenderActionFlags* ioActionFlags,
					     const AudioTimeStamp* in_time_stamp,
					     UInt32 in_bus_number,
					     UInt32 in_number_frames,
					     AudioBufferList* io_data);

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

const int ags_core_audio_port_endian_i = 1;
#define is_bigendian() ( (*(char*)&ags_core_audio_port_endian_i) == 0 )

GType
ags_core_audio_port_get_type()
{
  static GType ags_type_core_audio_port = 0;

  if(!ags_type_core_audio_port){
    static const GTypeInfo ags_core_audio_port_info = {
      sizeof (AgsCoreAudioPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCoreAudioPort),
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
  }

  return (ags_type_core_audio_port);
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
   * Since: 0.9.24
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
   * Since: 0.9.24
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
   * Since: 0.9.24
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
  connectable->connect = ags_core_audio_port_connect;
  connectable->disconnect = ags_core_audio_port_disconnect;
}

void
ags_core_audio_port_init(AgsCoreAudioPort *core_audio_port)
{
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;

  guint samplerate;
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;
  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert port mutex */
  core_audio_port->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_port->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) core_audio_port,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  core_audio_port->flags = 0;

  /*  */
  core_audio_port->core_audio_client = NULL;
  core_audio_port->core_audio_device = NULL;
  
  core_audio_port->uuid = ags_id_generator_create_uuid();
  core_audio_port->name = NULL;
  
  /* read config */
  config = ags_config_get_instance();
  
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* pcm channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "pcm-channels");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "pcm-channels");
  }
  
  if(str != NULL){
    pcm_channels = g_ascii_strtoull(str,
				    NULL,
				    10);
    g_free(str);
  }

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);
    free(str);
  }

  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    format = g_ascii_strtoull(str,
			      NULL,
			      10);
    free(str);
  }
  
  core_audio_port->buffer_size = buffer_size;
  core_audio_port->format = format;
  core_audio_port->pcm_channels = pcm_channels;
  core_audio_port->samplerate = samplerate;

  word_size = sizeof(Float32);
  
#ifdef AGS_WITH_CORE_AUDIO
  /* Audio */
  core_audio_port->comp = NULL;
  
  memset(&(core_audio_port->desc), 0, sizeof(AudioComponentDescription));

  memset(&(core_audio_port->data_format), 0, sizeof(AudioStreamBasicDescription));

  size_t bytesPerSample = sizeof (AudioUnitSampleType);
  
  core_audio_port->data_format.mBitsPerChannel = 8 * bytesPerSample;
  core_audio_port->data_format.mBytesPerPacket = bytesPerSample;
  core_audio_port->data_format.mBytesPerFrame = bytesPerSample;
  
  core_audio_port->data_format.mFramesPerPacket = 1;
  core_audio_port->data_format.mChannelsPerFrame = pcm_channels;

  core_audio_port->data_format.mFormatID = kAudioFormatLinearPCM;
  core_audio_port->data_format.mFormatFlags = kAudioFormatFlagsAudioUnitCanonical;
  
  core_audio_port->data_format.mSampleRate = (float) samplerate;

  /* MIDI */
  core_audio_port->midi_client = (MIDIClientRef *) malloc(sizeof(MIDIClientRef));
  memset(core_audio_port->midi_client, 0, sizeof(MIDIClientRef));
  
  core_audio_port->midi_port = (MIDIPortRef *) malloc(sizeof(MIDIPortRef));
  memset(core_audio_port->midi_port, 0, sizeof(MIDIPortRef));
#else
  core_audio_port->comp = NULL;
  core_audio_port->desc = NULL;

  core_audio_port->data_format = NULL;

  core_audio_port->input = NULL;
  
  core_audio_port->midi_client = NULL;
  core_audio_port->midi_port = NULL;
#endif

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

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  switch(prop_id){
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      if(core_audio_port->core_audio_client == (GObject *) core_audio_client){
	return;
      }

      if(core_audio_port->core_audio_client != NULL){
	g_object_unref(core_audio_port->core_audio_client);
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_port->core_audio_client = (GObject *) core_audio_client;
    }
    break;
  case PROP_CORE_AUDIO_DEVICE:
    {
      GObject *core_audio_device;

      core_audio_device = g_value_get_object(value);

      if(core_audio_port->core_audio_device == core_audio_device){
	return;
      }

      if(core_audio_port->core_audio_device != NULL){
	g_object_unref(core_audio_port->core_audio_device);
      }

      if(core_audio_device != NULL){
	g_object_ref(core_audio_device);
      }
      
      core_audio_port->core_audio_device = (GObject *) core_audio_device;
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      if(core_audio_port->name == port_name ||
	 !g_ascii_strcasecmp(core_audio_port->name,
			     port_name)){
	return;
      }

      if(core_audio_port->name != NULL){
	g_free(core_audio_port->name);
      }

      core_audio_port->name = port_name;
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

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);
  
  switch(prop_id){
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_value_set_object(value, core_audio_port->core_audio_client);
    }
    break;
  case PROP_CORE_AUDIO_DEVICE:
    {
      g_value_set_object(value, core_audio_port->core_audio_device);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_value_set_string(value, core_audio_port->name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_port_connect(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  if((AGS_CORE_AUDIO_PORT_CONNECTED & (core_audio_port->flags)) != 0){
    return;
  }

  core_audio_port->flags |= AGS_CORE_AUDIO_PORT_CONNECTED;
}

void
ags_core_audio_port_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioPort *core_audio_port;

  core_audio_port = AGS_CORE_AUDIO_PORT(connectable);

  if((AGS_CORE_AUDIO_PORT_CONNECTED & (core_audio_port->flags)) == 0){
    return;
  }

  core_audio_port->flags &= (~AGS_CORE_AUDIO_PORT_CONNECTED);
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
  g_free(core_audio_port->name);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_port_parent_class)->dispose(gobject);
}

void
ags_core_audio_port_finalize(GObject *gobject)
{
  AgsCoreAudioPort *core_audio_port;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  core_audio_port = AGS_CORE_AUDIO_PORT(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* core audio client */
  if(core_audio_port->core_audio_client != NULL){
    g_object_unref(core_audio_port->core_audio_client);
  }

  /* core audio device */
  if(core_audio_port->core_audio_device != NULL){
    g_object_unref(core_audio_port->core_audio_device);
  }

  /* name */
  g_free(core_audio_port->name);

  if(core_audio_port->midi_client != NULL){
    free(core_audio_port->midi_client);
  }

  if(core_audio_port->midi_port != NULL){
    free(core_audio_port->midi_port);
  }

  pthread_mutex_destroy(core_audio_port->mutex);
  free(core_audio_port->mutex);

  pthread_mutexattr_destroy(core_audio_port->mutexattr);
  free(core_audio_port->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_port_parent_class)->finalize(gobject);
}

/**
 * ags_core_audio_port_find:
 * @core_audio_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @core_audio_port.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 0.9.24
 */
GList*
ags_core_audio_port_find(GList *core_audio_port,
			 gchar *port_name)
{
  while(core_audio_port != NULL){
    if(!g_ascii_strcasecmp(AGS_CORE_AUDIO_PORT(core_audio_port->data)->name,
			   port_name)){
      return(core_audio_port);
    }
  }

  return(NULL);
}

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
 * Since: 0.9.24
 */
void
ags_core_audio_port_register(AgsCoreAudioPort *core_audio_port,
			     gchar *port_name,
			     gboolean is_audio, gboolean is_midi,
			     gboolean is_output)
{
  GList *list;

  gchar *name, *uuid;

#ifdef AGS_WITH_CORE_AUDIO
  OSStatus retval;
#endif
  
  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
     port_name == NULL){
    return;
  }

  if(core_audio_port->core_audio_client == NULL){
    g_warning("ags_core_audio_port.c - no assigned AgsCoreAudioClient");
    
    return;
  }

  if((AGS_CORE_AUDIO_PORT_REGISTERED & (core_audio_port->flags)) != 0){
    return;
  }

  /* get core audio server and application context */
  if(core_audio_port->core_audio_client == NULL ||
     AGS_CORE_AUDIO_CLIENT(core_audio_port->core_audio_client)->core_audio_server == NULL){
    return;
  }

  uuid = core_audio_port->uuid;
  name = core_audio_port->name;

  if(AGS_CORE_AUDIO_CLIENT(core_audio_port->core_audio_client)->graph == NULL){
    return;
  }
  
  core_audio_port->name = g_strdup(port_name);

  /* create sequencer or soundcard */
  if(is_output){
    core_audio_port->flags |= AGS_CORE_AUDIO_PORT_IS_OUTPUT;
  }else{
    core_audio_port->flags |= AGS_CORE_AUDIO_PORT_IS_INPUT;
  }
  
  if(is_audio){  
    core_audio_port->flags |= AGS_CORE_AUDIO_PORT_IS_AUDIO;

    if(is_output){
      pthread_t *thread;

      void **arr;
      
      guint word_size;

      switch(core_audio_port->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(signed char);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(signed short);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(signed long);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(signed long);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(signed long long);
	}
	break;
      default:
	g_message("core audio - unsupported word size");
      }

      core_audio_port->desc.componentType = kAudioUnitType_Output;
      core_audio_port->desc.componentSubType = kAudioUnitSubType_DefaultOutput;
      core_audio_port->desc.componentManufacturer = kAudioUnitManufacturer_Apple;

      core_audio_port->comp = AudioComponentFindNext(NULL,
						     &(core_audio_port->desc));

      if(core_audio_port->comp == NULL) {
	g_message("can't get output unit");
	return;
      }
      
      retval = AudioComponentInstanceNew(core_audio_port->comp,
					 &(core_audio_port->au_unit));

      if(retval != noErr){
	return;
      }
     
      core_audio_port->input.inputProc = ags_core_audio_port_output_callback;
      core_audio_port->input.inputProcRefCon = core_audio_port;
      retval = AudioUnitSetProperty(core_audio_port->au_unit,
				    kAudioUnitProperty_SetRenderCallback,
				    kAudioUnitScope_Input,
				    0, &(core_audio_port->input), sizeof(AURenderCallbackStruct));
      
      if(retval != noErr){
	return;
      }

      retval = AudioUnitSetProperty(core_audio_port->au_unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0,
				    &(core_audio_port->data_format), sizeof(AudioStreamBasicDescription));
      retval = AudioUnitSetProperty(core_audio_port->au_unit,
				    kAudioUnitProperty_StreamFormat,
				    kAudioUnitScope_Input,
				    0, &(core_audio_port->data_format), sizeof(AudioStreamBasicDescription));

      if(retval != noErr){
	return;
      }

      SetCurrentIOBufferFrameSize(core_audio_port->au_unit,
				  core_audio_port->buffer_size);
      
      retval = AudioUnitInitialize(core_audio_port->au_unit);
      
      if(retval != noErr){
	return;
      }
      
      g_message("start audio unit");
      AudioOutputUnitStart(core_audio_port->au_unit);

    }else{
      //NOTE:JK: not implemented
    }
  }else if(is_midi){
    core_audio_port->flags |= AGS_CORE_AUDIO_PORT_IS_MIDI;

    if(is_output){
      //NOTE:JK: not implemented
    }else{
      retval = MIDIClientCreate(CFSTR("Advanced Gtk+ Sequencer - Core MIDI to System Sounds Demo"),
				ags_core_audio_port_midi_notify_callback,
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
    }
  }
  
  core_audio_port->flags |= AGS_CORE_AUDIO_PORT_REGISTERED;
}

void
ags_core_audio_port_unregister(AgsCoreAudioPort *core_audio_port)
{
  if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }

  if((AGS_CORE_AUDIO_PORT_IS_AUDIO & (core_audio_port->flags)) != 0){
    if((AGS_CORE_AUDIO_PORT_IS_OUTPUT & (core_audio_port->flags)) != 0){
      //NOTE:JK: not implemented
    }else if((AGS_CORE_AUDIO_PORT_IS_INPUT & (core_audio_port->flags)) != 0){
    }
  }else if((AGS_CORE_AUDIO_PORT_IS_MIDI & (core_audio_port->flags)) != 0){
    if((AGS_CORE_AUDIO_PORT_IS_OUTPUT & (core_audio_port->flags)) != 0){
      //NOTE:JK: not implemented
    }else if((AGS_CORE_AUDIO_PORT_IS_INPUT & (core_audio_port->flags)) != 0){
    }
  }

  core_audio_port->flags &= (~AGS_CORE_AUDIO_PORT_REGISTERED);
}

#ifdef AGS_WITH_CORE_AUDIO
void*
ags_core_audio_port_output_thread(void **data)
{
  AgsCoreAudioPort *core_audio_port;
  AudioQueueRef in_aq;
  AudioQueueBufferRef in_complete_aq_buffer;
  
  core_audio_port = data[0];
  in_aq = data[1];
  in_complete_aq_buffer = data[2];
  
  g_atomic_int_set(&(core_audio_port->running),
		     TRUE);
    
  while(g_atomic_int_get(&(core_audio_port->running))){
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.10, TRUE);
  }

  pthread_exit(NULL);
}

OSStatus
ags_core_audio_port_output_callback(void *in_user_data, AudioUnitRenderActionFlags* ioActionFlags,
				    const AudioTimeStamp* in_time_stamp,
				    UInt32 in_bus_number,
				    UInt32 in_number_frames,
				    AudioBufferList* io_data)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;
  
  AgsAudioLoop *audio_loop;

  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  guint word_size;
  guint nth_buffer;
  guint copy_mode;
  guint count;
  gboolean no_event;
  gboolean empty_run;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutex_t *device_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  core_audio_port = (AgsCoreAudioPort *) in_user_data;

  if(core_audio_port == NULL){
    return(noErr);
  }

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  application_context = ags_application_context_get_instance();
  
  /*  */  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_port);
  
  pthread_mutex_unlock(application_mutex);

  if(g_atomic_int_get(&(core_audio_port->queued)) > 0){
    g_warning("drop core audio output callback");
    
    return(noErr);
  }else{
    g_atomic_int_inc(&(core_audio_port->queued));
  }

  /*
   * process audio
   */
  /*  */  
  pthread_mutex_lock(application_mutex);

  if(application_context != NULL){
    audio_loop = (AgsAudioLoop *) application_context->main_loop;
    task_thread = (AgsTaskThread *) application_context->task_thread;
  }else{
    audio_loop = NULL;
    task_thread = NULL;
  }
  
  pthread_mutex_unlock(application_mutex);

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

  ags_audio_buffer_util_clear_float32(io_data->mBuffers[0].mData, 1,
				      core_audio_port->pcm_channels * core_audio_port->buffer_size);
  
  if(audio_loop == NULL){
    g_atomic_int_dec_and_test(&(core_audio_port->queued));

    return(noErr);
  }
  
  g_atomic_int_and(&(AGS_THREAD(audio_loop)->flags),
		   (~(AGS_THREAD_TIMING)));

  /*  */
  pthread_mutex_lock(mutex);

  core_audio_devout = core_audio_port->core_audio_device;

  pthread_mutex_unlock(mutex);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  device_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) core_audio_devout);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(device_mutex);

  empty_run = FALSE;

  if(!ags_soundcard_is_playing(AGS_SOUNDCARD(core_audio_devout))){
    empty_run = TRUE;
  }
  
  pthread_mutex_unlock(device_mutex);

  pthread_mutex_lock(mutex);

  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(signed long long);
    }
    break;
  default:
    empty_run = TRUE;
  }

  count = core_audio_port->pcm_channels * core_audio_port->buffer_size;

  pthread_mutex_unlock(mutex);

  pthread_mutex_lock(device_mutex);
  
  /* wait callback */
  no_event = TRUE;
  
  if((AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
    callback_mutex = core_audio_devout->callback_mutex;
    
    pthread_mutex_unlock(device_mutex);
	
    /* give back computing time until ready */
    pthread_mutex_lock(callback_mutex);
    
    if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
      g_atomic_int_or(&(core_audio_devout->sync_flags),
		      AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT);
      
      while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	    (AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	pthread_cond_wait(core_audio_devout->callback_cond,
			  callback_mutex);
      }
    
      g_atomic_int_and(&(core_audio_devout->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT |
			  AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE)));
    }
    
    pthread_mutex_unlock(callback_mutex);
    
    no_event = FALSE;

    pthread_mutex_lock(device_mutex);
  }

  /* get buffer */  
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
      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(signed long long);
    }
    break;
  default:
    empty_run = TRUE;
  }

  /* get copy mode */
  if(!empty_run){
    guint i;
    
    g_atomic_int_set(&(core_audio_port->is_empty),
		     FALSE);

    copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT32,
						    ags_audio_buffer_util_format_from_soundcard(core_audio_devout->format));

    for(i = 0; i < core_audio_port->pcm_channels; i++){
      ags_audio_buffer_util_copy_buffer_to_buffer(io_data->mBuffers[0].mData, 1, 0,
						  core_audio_devout->buffer[nth_buffer], core_audio_port->pcm_channels, i,
						  core_audio_port->buffer_size, copy_mode);
    }
  }

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
  
  pthread_mutex_unlock(device_mutex);

  if(empty_run){
    pthread_mutex_lock(mutex);
    
    pthread_mutex_unlock(mutex);
    
    g_atomic_int_set(&(core_audio_port->is_empty),
		     TRUE);
  }
  
  g_atomic_int_dec_and_test(&(core_audio_port->queued));

  return(noErr);
}
  
void
ags_core_audio_port_midi_notify_callback(const MIDINotification  *message,
					 void *ref_con)
{
}

void
ags_core_audio_port_midi_read_callback(const MIDIPacketList *pkt_list,
				       void *ref_con,
				       void *conn_ref_con)
{
}
#endif

void
ags_core_audio_port_set_samplerate(AgsCoreAudioPort *core_audio_port,
				   guint samplerate)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lock core_audio port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_port);

  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mSampleRate = (float) samplerate;
#endif

  core_audio_port->samplerate = samplerate;
  
  pthread_mutex_unlock(mutex);
}

void
ags_core_audio_port_set_buffer_size(AgsCoreAudioPort *core_audio_port,
				    guint buffer_size)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lock core_audio port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_port);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mBytesPerPacket = buffer_size;
#endif

  core_audio_port->buffer_size = buffer_size;
  
  pthread_mutex_unlock(mutex);
}

void
ags_core_audio_port_set_pcm_channels(AgsCoreAudioPort *core_audio_port,
				     guint pcm_channels)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lock core_audio port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_port);

  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  core_audio_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_port->data_format.mChannelsPerFrame = pcm_channels;
#endif

  pthread_mutex_unlock(mutex);
}

/**
 * ags_core_audio_port_new:
 * @core_audio_client: the #AgsCoreAudioClient assigned to
 *
 * Instantiate a new #AgsCoreAudioPort.
 *
 * Returns: the new #AgsCoreAudioPort
 *
 * Since: 0.9.24
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
