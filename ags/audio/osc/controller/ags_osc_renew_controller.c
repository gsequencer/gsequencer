/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/audio/osc/controller/ags_osc_renew_controller.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/task/ags_resize_audio.h>
#include <ags/audio/task/ags_set_device.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_samplerate.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_format.h>

#include <ags/audio/osc/ags_osc_response.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

void ags_osc_renew_controller_class_init(AgsOscRenewControllerClass *osc_renew_controller);
void ags_osc_renew_controller_init(AgsOscRenewController *osc_renew_controller);
void ags_osc_renew_controller_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_osc_renew_controller_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_osc_renew_controller_dispose(GObject *gobject);
void ags_osc_renew_controller_finalize(GObject *gobject);

gpointer ags_osc_renew_controller_set_data_soundcard(AgsOscRenewController *osc_renew_controller,
						     AgsOscConnection *osc_connection,
						     GObject *soundcard,
						     unsigned char *message, guint message_size,
						     gchar *type_tag,
						     gchar *path, guint path_offset);
gpointer ags_osc_renew_controller_set_data_sequencer(AgsOscRenewController *osc_renew_controller,
						     AgsOscConnection *osc_connection,
						     GObject *sequencer,
						     unsigned char *message, guint message_size,
						     gchar *type_tag,
						     gchar *path, guint path_offset);

gpointer ags_osc_renew_controller_set_data_audio(AgsOscRenewController *osc_renew_controller,
						 AgsOscConnection *osc_connection,
						 AgsAudio *audio,
						 unsigned char *message, guint message_size,
						 gchar *type_tag,
						 gchar *path, guint path_offset);
gpointer ags_osc_renew_controller_set_data_channel(AgsOscRenewController *osc_renew_controller,
						   AgsOscConnection *osc_connection,
						   AgsChannel *channel,
						   unsigned char *message, guint message_size,
						   gchar *type_tag,
						   gchar *path, guint path_offset);

gpointer ags_osc_renew_controller_set_data_port(AgsOscRenewController *osc_renew_controller,
						AgsOscConnection *osc_connection,
						AgsPort *port,
						unsigned char *message, guint message_size,
						gchar *type_tag,
						gchar *path, guint path_offset);

gpointer ags_osc_renew_controller_real_set_data(AgsOscRenewController *osc_renew_controller,
						AgsOscConnection *osc_connection,
						unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_renew_controller
 * @short_description: OSC renew controller
 * @title: AgsOscRenewController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_renew_controller.h
 *
 * The #AgsOscRenewController implements the OSC renew controller.
 */

enum{
  PROP_0,
};

enum{
  SET_DATA,
  LAST_SIGNAL,
};

static gpointer ags_osc_renew_controller_parent_class = NULL;
static guint osc_renew_controller_signals[LAST_SIGNAL];

GType
ags_osc_renew_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_renew_controller = 0;

    static const GTypeInfo ags_osc_renew_controller_info = {
      sizeof (AgsOscRenewControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_renew_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscRenewController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_renew_controller_init,
    };
    
    ags_type_osc_renew_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							   "AgsOscRenewController",
							   &ags_osc_renew_controller_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_renew_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_renew_controller_class_init(AgsOscRenewControllerClass *osc_renew_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_renew_controller_parent_class = g_type_class_peek_parent(osc_renew_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_renew_controller;

  gobject->set_property = ags_osc_renew_controller_set_property;
  gobject->get_property = ags_osc_renew_controller_get_property;

  gobject->dispose = ags_osc_renew_controller_dispose;
  gobject->finalize = ags_osc_renew_controller_finalize;

  /* properties */

  /* AgsOscRenewControllerClass */
  osc_renew_controller->set_data = ags_osc_renew_controller_real_set_data;

  /* signals */
  /**
   * AgsOscRenewController::set-data:
   * @osc_renew_controller: the #AgsOscRenewController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::set-data signal is emited during get data of renew controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.1.0
   */
  osc_renew_controller_signals[SET_DATA] =
    g_signal_new("set-data",
		 G_TYPE_FROM_CLASS(osc_renew_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscRenewControllerClass, set_data),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_renew_controller_init(AgsOscRenewController *osc_renew_controller)
{
  g_object_set(osc_renew_controller,
	       "context-path", "/renew",
	       NULL);
}

void
ags_osc_renew_controller_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscRenewController *osc_renew_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_renew_controller = AGS_OSC_RENEW_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_renew_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_renew_controller_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsOscRenewController *osc_renew_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_renew_controller = AGS_OSC_RENEW_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_renew_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_renew_controller_dispose(GObject *gobject)
{
  AgsOscRenewController *osc_renew_controller;

  osc_renew_controller = AGS_OSC_RENEW_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_renew_controller_parent_class)->dispose(gobject);
}

void
ags_osc_renew_controller_finalize(GObject *gobject)
{
  AgsOscRenewController *osc_renew_controller;

  osc_renew_controller = AGS_OSC_RENEW_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_renew_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_renew_controller_set_data_soundcard(AgsOscRenewController *osc_renew_controller,
					    AgsOscConnection *osc_connection,
					    GObject *soundcard,
					    unsigned char *message, guint message_size,
					    gchar *type_tag,
					    gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;
  
  AgsApplicationContext *application_context;
  
  unsigned char *packet;

  guint path_length;
  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_SOUNDCARD(soundcard)){
    return(NULL);
  }
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
  
  osc_response = NULL;
  
  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    osc_response = ags_osc_response_new();

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
        
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);

    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"device",
		7)){
      AgsSetDevice *set_device;
      
      gchar *device;
      
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "s", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read device */
      ags_osc_buffer_util_get_string(message + 16 + path_length,
				     &device, NULL);

      set_device = ags_set_device_new(soundcard,
				      device);
      ags_task_thread_append_task(task_thread,
				  set_device);
    }else if(!strncmp(path + path_offset,
		      "pcm-channels",
		      13)){
      AgsSetAudioChannels *set_audio_channels;
      
      guint pcm_channels;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read pcm channels */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &pcm_channels);

      set_audio_channels = ags_set_audio_channels_new(soundcard,
						      pcm_channels);
      ags_task_thread_append_task(task_thread,
				  set_audio_channels);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      AgsSetSamplerate *set_samplerate;
      
      guint samplerate;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read samplerate */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &samplerate);

      set_samplerate = ags_set_samplerate_new(soundcard,
					      samplerate);
      ags_task_thread_append_task(task_thread,
				  set_samplerate);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      AgsSetBufferSize *set_buffer_size;
      
      guint buffer_size;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read buffer size */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &buffer_size);

      set_buffer_size = ags_set_buffer_size_new(soundcard,
						buffer_size);
      ags_task_thread_append_task(task_thread,
				  set_buffer_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      AgsSetFormat *set_format;
      
      guint format;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read format */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &format);

      set_format = ags_set_format_new(soundcard,
				      format);
      ags_task_thread_append_task(task_thread,
				  set_format);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      return(osc_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else{
    osc_response = ags_osc_response_new();
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    return(osc_response);
  }
  
  return(osc_response);
}

gpointer
ags_osc_renew_controller_set_data_sequencer(AgsOscRenewController *osc_renew_controller,
					    AgsOscConnection *osc_connection,
					    GObject *sequencer,
					    unsigned char *message, guint message_size,
					    gchar *type_tag,
					    gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;

  AgsApplicationContext *application_context;
  
  unsigned char *packet;

  guint path_length;
  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_SEQUENCER(sequencer)){
    return(NULL);
  }
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  osc_response = NULL;
  
  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    osc_response = ags_osc_response_new();

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
        
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);

    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"device",
		12)){
      AgsSetDevice *set_device;
      
      gchar *device;
      
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "s", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read device */
      ags_osc_buffer_util_get_string(message + 16 + path_length,
				     &device, NULL);

      set_device = ags_set_device_new(sequencer,
				      device);
      ags_task_thread_append_task(task_thread,
				  set_device);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      return(osc_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else{
    osc_response = ags_osc_response_new();
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    return(osc_response);
  }

  return(osc_response);
}

gpointer
ags_osc_renew_controller_set_data_audio(AgsOscRenewController *osc_renew_controller,
					AgsOscConnection *osc_connection,
					AgsAudio *audio,
					unsigned char *message, guint message_size,
					gchar *type_tag,
					gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;

  AgsApplicationContext *application_context;
  
  unsigned char *packet;

  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
  
  osc_response = NULL;

  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);

    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;
    
    /* properties */
    if(!strncmp(path + path_offset,
		"audio-channels",
		15)){
      AgsResizeAudio *resize_audio;

      guint output_pads, input_pads;
      guint audio_channels;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read audio channels */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &audio_channels);

      g_object_get(audio,
		   "output-pads", &output_pads,
		   "input-pads", &input_pads,
		   NULL);
      
      resize_audio = ags_resize_audio_new(audio,
					  output_pads, input_pads,
					  audio_channels);
      ags_task_thread_append_task(task_thread,
				  resize_audio);
    }else if(!strncmp(path + path_offset,
		      "output-pads",
		      12)){
      AgsResizeAudio *resize_audio;

      guint output_pads, input_pads;
      guint audio_channels;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read output pads */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &output_pads);

      g_object_get(audio,
		   "input-pads", &input_pads,
		   "audio-channels", &audio_channels,
		   NULL);
      
      resize_audio = ags_resize_audio_new(audio,
					  output_pads, input_pads,
					  audio_channels);
      ags_task_thread_append_task(task_thread,
				  resize_audio);
    }else if(!strncmp(path + path_offset,
		      "input-pads",
		      11)){
      AgsResizeAudio *resize_audio;

      guint output_pads, input_pads;
      guint audio_channels;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read input pads */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &input_pads);

      g_object_get(audio,
		   "output-pads", &output_pads,
		   "audio-channels", &audio_channels,
		   NULL);
      
      resize_audio = ags_resize_audio_new(audio,
					  output_pads, input_pads,
					  audio_channels);
      ags_task_thread_append_task(task_thread,
				  resize_audio);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      AgsSetSamplerate *set_samplerate;
      
      guint samplerate;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read samplerate */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &samplerate);

      set_samplerate = ags_set_samplerate_new(audio,
					      samplerate);
      ags_task_thread_append_task(task_thread,
				  set_samplerate);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      AgsSetBufferSize *set_buffer_size;
      
      guint buffer_size;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read buffer size */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &buffer_size);

      set_buffer_size = ags_set_buffer_size_new(audio,
						buffer_size);
      ags_task_thread_append_task(task_thread,
				  set_buffer_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      AgsSetFormat *set_format;
      
      guint format;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read format */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &format);

      set_format = ags_set_format_new(audio,
				      format);
      ags_task_thread_append_task(task_thread,
				  set_format);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      return(osc_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else if(!strncmp(path + path_offset,
		    "/AgsOutput",
		    11)){
    AgsChannel *start_channel, *channel;
      
    guint nth_channel;
    int retval;

    path_offset += 11;

    g_object_get(audio,
		 "output", &start_channel,
		 NULL);
    
    retval = sscanf(path + path_offset, "[%d]",
		    &nth_channel);
      
    channel = ags_channel_nth(start_channel,
			      nth_channel);
      
    path_offset = index(path + path_offset, ']') - path + 1;

    osc_response = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     channel,
							     message, message_size,
							     type_tag,
							     path, path_offset);
  }else if(!strncmp(path + path_offset,
		    "/AgsInput",
		    10)){
    AgsChannel *start_channel, *channel;
      
    guint nth_channel;
    int retval;

    path_offset += 11;

    g_object_get(audio,
		 "input", &start_channel,
		 NULL);
    
    retval = sscanf(path + path_offset, "[%d]",
		    &nth_channel);
      
    channel = ags_channel_nth(start_channel,
			      nth_channel);
      
    path_offset = index(path + path_offset, ']') - path + 1;

    osc_response = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     channel,
							     message, message_size,
							     type_tag,
							     path, path_offset);
  }else if(!strncmp(path + path_offset,
		    "/AgsPort",
		    8)){
    AgsPort *port;
      
    GList *start_port;
      
    guint nth_port;
    int retval;

    port = NULL;
      
    path_offset += 8;

    if(g_ascii_isdigit(path[path_offset + 1])){
      g_object_get(audio,
		   "port", &start_port,
		   NULL);
	
      retval = sscanf(path + path_offset, "[%d]",
		      &nth_port);
      
      port = g_list_nth_data(start_port,
			     nth_port);
      
      path_offset = index(path + path_offset, ']') - path + 1;

      osc_response = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							    osc_connection,
							    port,
							    message, message_size,
							    type_tag,
							    path, path_offset);

      g_list_free(start_port);
    }else if(path[path_offset + 1] == '"'){
      GList *list;
	
      gchar *specifier;
      gchar *offset;

      guint length;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	return(osc_response);
      }

      length = offset - (path + path_offset + 3);

      specifier = malloc(length * sizeof(gchar));
      sscanf(path + path_offset, "%s", &specifier);

      g_object_get(audio,
		   "port", &start_port,
		   NULL);	

      list = ags_port_find_specifier(start_port,
				     specifier);

      if(list != NULL){
	port = list->data;
      }

      path_offset = path_offset + strlen(specifier) + 2;
      
      osc_response = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							    osc_connection,
							    port,
							    message, message_size,
							    type_tag,
							    path, path_offset);
	
      g_list_free(start_port);
    }else{
      osc_response = ags_osc_response_new();
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		   NULL);

      return(osc_response);
    }
  }else{
    osc_response = ags_osc_response_new();
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    return(osc_response);
  }

  return(osc_response);
}

gpointer
ags_osc_renew_controller_set_data_channel(AgsOscRenewController *osc_renew_controller,
					  AgsOscConnection *osc_connection,
					  AgsChannel *channel,
					  unsigned char *message, guint message_size,
					  gchar *type_tag,
					  gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;
  
  AgsApplicationContext *application_context;
  
  unsigned char *packet;

  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  osc_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);
  
    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;
    
    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      AgsSetSamplerate *set_samplerate;
      
      guint samplerate;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read samplerate */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &samplerate);

      set_samplerate = ags_set_samplerate_new(channel,
					      samplerate);
      ags_task_thread_append_task(task_thread,
				  set_samplerate);
    }else if(!strncmp(path + path_offset,
		"buffer-size",
		12)){
      AgsSetBufferSize *set_buffer_size;
      
      guint buffer_size;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read buffer size */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &buffer_size);

      set_buffer_size = ags_set_buffer_size_new(channel,
						buffer_size);
      ags_task_thread_append_task(task_thread,
				  set_buffer_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      AgsSetFormat *set_format;
      
      guint format;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	return(osc_response);
      }
      
      /* read format */
      ags_osc_buffer_util_get_int32(message + 16 + path_length,
				    &format);

      set_format = ags_set_format_new(channel,
				      format);
      ags_task_thread_append_task(task_thread,
				  set_format);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      return(osc_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else if(!strncmp(path + path_offset,
		    "/AgsPort",
		    8)){
    AgsPort *port;
      
    GList *start_port;
      
    guint nth_port;
    int retval;

    port = NULL;
      
    path_offset += 8;

    if(g_ascii_isdigit(path[path_offset + 1])){
      g_object_get(channel,
		   "port", &start_port,
		   NULL);
	
      retval = sscanf(path + path_offset, "[%d]",
		      &nth_port);
      
      port = g_list_nth_data(start_port,
			     nth_port);
      
      path_offset = index(path + path_offset, ']') - path + 1;

      osc_response = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							    osc_connection,
							    port,
							    message, message_size,
							    type_tag,
							    path, path_offset);

      g_list_free(start_port);
    }else if(path[path_offset + 1] == '"'){
      GList *list;
	
      gchar *specifier;
      gchar *offset;

      guint length;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	return(osc_response);
      }

      length = offset - (path + path_offset + 3);

      specifier = malloc(length * sizeof(gchar));
      sscanf(path + path_offset, "%s", &specifier);
	
      g_object_get(channel,
		   "port", &start_port,
		   NULL);

      list = ags_port_find_specifier(start_port,
				     specifier);

      if(list != NULL){
	port = list->data;
      }

      path_offset = path_offset + strlen(specifier) + 2;

      osc_response = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							    osc_connection,
							    port,
							    message, message_size,
							    type_tag,
							    path, path_offset);
	
      g_list_free(start_port);
    }else{
      osc_response = ags_osc_response_new();
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		   NULL);

      return(osc_response);
    }
  }else{
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    return(osc_response);
  }

  return(osc_response);
}

gpointer
ags_osc_renew_controller_set_data_port(AgsOscRenewController *osc_renew_controller,
				       AgsOscConnection *osc_connection,
				       AgsPort *port,
				       unsigned char *message, guint message_size,
				       gchar *type_tag,
				       gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;
  
  AgsThread *task_thread;
  
  unsigned char *packet;

  guint path_length;
  guint type_tag_offset;
  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_PORT(port)){
    return(NULL);
  }
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  osc_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);
  
    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;
    
    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;

    if(!strncmp(path + path_offset,
		"value",
		6)){
      GType port_value_type;

      guint port_value_length;
      gboolean port_value_is_pointer;
      
      pthread_mutex_t *port_mutex;

      /* get port mutex */
      pthread_mutex_lock(ags_port_get_class_mutex());
      
      port_mutex = port->obj_mutex;
      
      pthread_mutex_unlock(ags_port_get_class_mutex());

      /* check array type */
      g_object_get(port,
		   "port-value-is-pointer", &port_value_is_pointer,
		   "port-value-type", &port_value_type,
		   "port-value-length", &port_value_length,
		   NULL);
      
      path_offset = 4 * (guint) ceil((double) (path_offset + 6) / 4.0);
      
      if(port_value_is_pointer){
	guint i;

	if(message_size < 16 + path_length + 2 ||
	   type_tag[1] != '['){
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		       NULL);

	  return(osc_response);
	}
	
	if(port_value_type == G_TYPE_BOOLEAN){
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; 16 + path_length + 2 + i < message_size && type_tag[2 + i] != '\0' && type_tag[2 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[2 + i] == 'T'){
	      port->port_value.ags_port_boolean_ptr[i] = TRUE;
	    }else if(type_tag[2 + i] == 'F'){
	      port->port_value.ags_port_boolean_ptr[i] = FALSE;
	    }else{
	      success = FALSE;

	      break;
	    }
	  }

	  pthread_mutex_unlock(port_mutex);

	  if(!success ||
	     type_tag[2 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }
	  
	  value_count = i;
	}else if(port_value_type == G_TYPE_INT64){
	  gint64 value;
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  for(i = 0; 16 + path_length + 2 + i < message_size && type_tag[2 + i] != '\0' && type_tag[2 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[2 + i] != 'h'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[2 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (3 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 8) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_int64(message + path_offset + type_tag_offset + (i * 8),
					  &value);

	    port->port_value.ags_port_int_ptr[i] = value;
	  }
	  
	  pthread_mutex_unlock(port_mutex);
	}else if(port_value_type == G_TYPE_UINT64){
	  guint64 value;
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  for(i = 0; 16 + path_length + 2 + i < message_size && type_tag[2 + i] != '\0' && type_tag[2 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[2 + i] != 'h'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[2 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (3 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 8) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_int64(message + path_offset + type_tag_offset + (i * 8),
					  &value);

	    port->port_value.ags_port_uint_ptr[i] = value;
	  }
	  
	  pthread_mutex_unlock(port_mutex);
	}else if(port_value_type == G_TYPE_FLOAT){
	  gfloat value;
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  for(i = 0; 16 + path_length + 2 + i < message_size && type_tag[2 + i] != '\0' && type_tag[2 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[2 + i] != 'f'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[2 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (3 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 4) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_float(message + path_offset + type_tag_offset + (i * 4),
					  &value);

	    port->port_value.ags_port_float_ptr[i] = value;
	  }
	  
	  pthread_mutex_unlock(port_mutex);
	}else if(port_value_type == G_TYPE_DOUBLE){
	  gdouble value;
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  for(i = 0; 16 + path_length + 2 + i < message_size && type_tag[2 + i] != '\0' && type_tag[2 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[2 + i] != 'd'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[2 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (3 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 8) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_double(message + path_offset + type_tag_offset + (i * 8),
					   &value);

	    port->port_value.ags_port_double_ptr[i] = value;
	  }
	  
	  pthread_mutex_unlock(port_mutex);
	}
      }else{
	if(port_value_type == G_TYPE_BOOLEAN){
	  gboolean success;

	  success = TRUE;
	  
	  pthread_mutex_lock(port_mutex);

	  if(message_size < 16 + path_length + 2){
	    if(type_tag[1] == 'T'){
	      port->port_value.ags_port_boolean = TRUE;
	    }else if(type_tag[1] == 'F'){
	      port->port_value.ags_port_boolean = FALSE;
	    }else{
	      success = FALSE;
	    }
	  }else{
	    success = FALSE;
	  }
	  
	  pthread_mutex_unlock(port_mutex);

	  if(!success ||
	     type_tag[2] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }
	}else if(port_value_type == G_TYPE_INT64){
	  gint64 value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size < 16 + path_length + 10){
	    if(type_tag[1] == 'h'){
	      ags_osc_buffer_util_get_int64(message + path_offset + type_tag_offset,
					    &value);

	      /* set value */
	      pthread_mutex_lock(port_mutex);

	      port->port_value.ags_port_int = value;

	      pthread_mutex_unlock(port_mutex);
	    }else{
	      success = FALSE;
	    }
	  }else{
	    success = FALSE;
	  }
	  
	  if(!success ||
	     type_tag[2] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }
	}else if(port_value_type == G_TYPE_UINT64){
	  guint64 value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size < 16 + path_length + 10){
	    if(type_tag[1] == 'h'){
	      ags_osc_buffer_util_get_int64(message + path_offset + type_tag_offset,
					    &value);

	      /* set value */
	      pthread_mutex_lock(port_mutex);

	      port->port_value.ags_port_uint = value;

	      pthread_mutex_unlock(port_mutex);
	    }else{
	      success = FALSE;
	    }
	  }else{
	    success = FALSE;
	  }

	  if(!success ||
	     type_tag[2] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }
	}else if(port_value_type == G_TYPE_FLOAT){
	  gfloat value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size < 16 + path_length + 6){
	    if(type_tag[1] == 'f'){
	      ags_osc_buffer_util_get_float(message + path_offset + type_tag_offset,
					    &value);

	      /* set value */
	      pthread_mutex_lock(port_mutex);

	      port->port_value.ags_port_float = value;

	      pthread_mutex_unlock(port_mutex);
	    }else{
	      success = FALSE;
	    }
	  }else{
	    success = FALSE;
	  }
	  
	  if(!success ||
	     type_tag[2] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }
	}else if(port_value_type == G_TYPE_DOUBLE){
	  gdouble value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size < 16 + path_length + 10){
	    if(type_tag[1] == 'd'){
	      ags_osc_buffer_util_get_double(message + path_offset + type_tag_offset,
					     &value);

	      /* set value */
	      pthread_mutex_lock(port_mutex);

	      port->port_value.ags_port_double = value;

	      pthread_mutex_unlock(port_mutex);
	    }else{
	      success = FALSE;
	    }
	  }else{
	    success = FALSE;
	  }
	  
	  if(!success ||
	     type_tag[2] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    return(osc_response);
	  }
	}
      }
    }
  }

  return(osc_response);
}

gpointer
ags_osc_renew_controller_real_set_data(AgsOscRenewController *osc_renew_controller,
				       AgsOscConnection *osc_connection,
				       unsigned char *message, guint message_size)
{
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;

  gchar *type_tag;
  gchar *path;
  
  guint path_offset;
  gboolean success;

  /* read type tag */
  ags_osc_buffer_util_get_string(message + 8,
				 &type_tag, NULL);

  success = (!strncmp(type_tag, ",s", 2)) ? TRUE: FALSE;

  if(!success){
    osc_response = ags_osc_response_new();
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    return(osc_response);
  }
  
  /* read argument */
  ags_osc_buffer_util_get_string(message + 12,
				 &path, NULL);
  
  /* create packet */
  application_context = ags_application_context_get_instance();

  path_offset = 0;
  
  if(!strncmp(path,
	      "/AgsSoundProvider",
	      17)){
    path_offset = 17;
    
    if(!strncmp(path + path_offset,
		"/AgsSoundcard",
		13)){
      GObject *soundcard;
      
      GList *start_soundcard;

      guint nth_soundcard;
      int retval;
      
      start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
      path_offset += 13;

      retval = sscanf(path + path_offset, "[%d]",
		      &nth_soundcard);
      

      if(retval <= 0){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		     NULL);

	return(osc_response);
      }

      soundcard = g_list_nth_data(start_soundcard,
				  nth_soundcard);
      
      path_offset = index(path + path_offset, ']') - path + 1;

      osc_response = ags_osc_renew_controller_set_data_soundcard(osc_renew_controller,
								 osc_connection,
								 soundcard,
								 message, message_size,
								 type_tag,
								 path, path_offset);

      g_list_free(start_soundcard);
    }else if(!strncmp(path + path_offset,
		      "/AgsSequencer",
		      13)){
      GObject *sequencer;
      
      GList *start_sequencer;

      guint nth_sequencer;
      int retval;
      
      start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));
      path_offset += 13;

      retval = sscanf(path + path_offset, "[%d]",
		      &nth_sequencer);
      
      if(retval <= 0){
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		     NULL);

	return(osc_response);
      }

      sequencer = g_list_nth_data(start_sequencer,
				  nth_sequencer);
      
      path_offset = index(path + path_offset, ']') - path + 1;

      osc_response = ags_osc_renew_controller_set_data_sequencer(osc_renew_controller,
								 osc_connection,
								 sequencer,
								 message, message_size,
								 type_tag,
								 path, path_offset);

      g_list_free(start_sequencer);
    }else if(!strncmp(path + path_offset,
		      "/AgsAudio",
		      9)){
      AgsAudio *audio;
      
      GList *start_audio;
      
      guint nth_audio;
      int retval;
      
      audio = NULL;
      
      path_offset += 9;

      if(g_ascii_isdigit(path[path_offset + 1])){
	start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

	retval = sscanf(path + path_offset, "[%d]",
			&nth_audio);
	
	if(retval <= 0){
	  osc_response = ags_osc_response_new();
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		       NULL);

	  return(osc_response);
	}

	audio = g_list_nth_data(start_audio,
				nth_audio);
      
	path_offset = index(path + path_offset, ']') - path + 1;

	osc_response = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
							       osc_connection,
							       audio,
							       message, message_size,
							       type_tag,
							       path, path_offset);
	
	g_list_free(start_audio);
      }else if(path[path_offset + 1] == '"'){
	GList *list;
	
	gchar *audio_name;
	gchar *offset;

	guint length;

	if((offset = index(path + path_offset + 2, '"')) == NULL){
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		       NULL);
	  
	  return(osc_response);
	}

	length = offset - (path + path_offset + 3);

	audio_name = malloc((length + 1) * sizeof(gchar));
	sscanf(path + path_offset, "%s", &audio_name);

	start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

	list = ags_audio_find_name(start_audio,
				   audio_name);

	if(list != NULL){
	  audio = list->data;
	}

	path_offset = path_offset + strlen(audio_name) + 2;

	osc_response = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
							       osc_connection,
							       audio,
							       message, message_size,
							       type_tag,
							       path, path_offset);
	
	g_list_free(start_audio);
      }else{
	osc_response = ags_osc_response_new();
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		     NULL);

	return(osc_response);
      }
    }else{
      osc_response = ags_osc_response_new();
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      return(osc_response);
    }
  }

  if(osc_response == NULL){
    osc_response = ags_osc_response_new();
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		 NULL);

    return(osc_response);
  }
  
  return(osc_response);
}

/**
 * ags_osc_renew_controller_set_data:
 * @osc_renew_controller: the #AgsOscRenewController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get renew.
 * 
 * Returns: the #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_renew_controller_set_data(AgsOscRenewController *osc_renew_controller,
				  AgsOscConnection *osc_connection,
				  unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_RENEW_CONTROLLER(osc_renew_controller), NULL);
  
  g_object_ref((GObject *) osc_renew_controller);
  g_signal_emit(G_OBJECT(osc_renew_controller),
		osc_renew_controller_signals[SET_DATA], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_renew_controller);

  return(osc_response);
}

/**
 * ags_osc_renew_controller_new:
 * 
 * Instantiate new #AgsOscRenewController
 * 
 * Returns: the #AgsOscRenewController
 * 
 * Since: 2.1.0
 */
AgsOscRenewController*
ags_osc_renew_controller_new()
{
  AgsOscRenewController *osc_renew_controller;

  osc_renew_controller = (AgsOscRenewController *) g_object_new(AGS_TYPE_OSC_RENEW_CONTROLLER,
								NULL);

  return(osc_renew_controller);
}
