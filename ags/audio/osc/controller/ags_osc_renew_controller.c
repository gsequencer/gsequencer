/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/task/ags_resize_audio.h>
#include <ags/audio/task/ags_set_device.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_samplerate.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_format.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <regex.h>

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

gpointer ags_osc_renew_controller_set_data_recall(AgsOscRenewController *osc_renew_controller,
						  AgsOscConnection *osc_connection,
						  AgsRecall *recall,
						  unsigned char *message, guint message_size,
						  gchar *type_tag,
						  gchar *path, guint path_offset);

gpointer ags_osc_renew_controller_set_data_port(AgsOscRenewController *osc_renew_controller,
						AgsOscConnection *osc_connection,
						AgsRecall *parent,
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

static pthread_mutex_t regex_mutex = PTHREAD_MUTEX_INITIALIZER;

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
  
  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_SOUNDCARD(soundcard)){
    return(NULL);
  }

  start_response = NULL;
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
  
  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
        
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);

    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    type_tag_length = 4 * (guint) ceil((double) (strlen(type_tag) + 1) / 4.0);

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
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read device */
      ags_osc_buffer_util_get_string(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				     &device, NULL);

      set_device = ags_set_device_new(soundcard,
				      device);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_device);

      if(device != NULL){
	free(device);
      }
    }else if(!strncmp(path + path_offset,
		      "pcm-channels",
		      13)){
      AgsSetAudioChannels *set_audio_channels;
      
      guint pcm_channels;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read pcm channels */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &pcm_channels);

      set_audio_channels = ags_set_audio_channels_new(soundcard,
						      pcm_channels);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_audio_channels);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      AgsSetSamplerate *set_samplerate;
      
      guint samplerate;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read samplerate */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &samplerate);

      set_samplerate = ags_set_samplerate_new(soundcard,
					      samplerate);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_samplerate);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      AgsSetBufferSize *set_buffer_size;
      
      guint buffer_size;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read buffer size */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &buffer_size);

      set_buffer_size = ags_set_buffer_size_new(soundcard,
						buffer_size);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_buffer_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      AgsSetFormat *set_format;
      
      guint format;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read format */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &format);

      set_format = ags_set_format_new(soundcard,
				      format);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_format);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      g_object_unref(task_thread);

      return(start_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    g_object_unref(task_thread);

    return(start_response);
  }

  g_object_unref(task_thread);
  
  return(start_response);
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
  
  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_SEQUENCER(sequencer)){
    return(NULL);
  }

  start_response = NULL;

  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));
  
  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
        
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);

    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    type_tag_length = 4 * (guint) ceil((double) (strlen(type_tag) + 1) / 4.0);

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
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read device */
      ags_osc_buffer_util_get_string(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				     &device, NULL);

      set_device = ags_set_device_new(sequencer,
				      device);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_device);

      if(device != NULL){
	free(device);
      }
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      g_object_unref(task_thread);

      return(start_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		 NULL);

    g_object_unref(task_thread);

    return(start_response);
  }

  g_object_unref(task_thread);

  return(start_response);
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
  
  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }

  start_response = NULL;

  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);

    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    type_tag_length = 4 * (guint) ceil((double) (strlen(type_tag) + 1) / 4.0);

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
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read audio channels */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &audio_channels);

      g_object_get(audio,
		   "output-pads", &output_pads,
		   "input-pads", &input_pads,
		   NULL);
      
      resize_audio = ags_resize_audio_new(audio,
					  output_pads, input_pads,
					  audio_channels);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) resize_audio);
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
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read output pads */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &output_pads);

      g_object_get(audio,
		   "input-pads", &input_pads,
		   "audio-channels", &audio_channels,
		   NULL);
      
      resize_audio = ags_resize_audio_new(audio,
					  output_pads, input_pads,
					  audio_channels);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) resize_audio);
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
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read input pads */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &input_pads);

      g_object_get(audio,
		   "output-pads", &output_pads,
		   "audio-channels", &audio_channels,
		   NULL);
      
      resize_audio = ags_resize_audio_new(audio,
					  output_pads, input_pads,
					  audio_channels);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) resize_audio);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      AgsSetSamplerate *set_samplerate;
      
      guint samplerate;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read samplerate */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &samplerate);

      set_samplerate = ags_set_samplerate_new((GObject *) audio,
					      samplerate);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_samplerate);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      AgsSetBufferSize *set_buffer_size;
      
      guint buffer_size;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read buffer size */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &buffer_size);

      set_buffer_size = ags_set_buffer_size_new((GObject *) audio,
						buffer_size);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_buffer_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      AgsSetFormat *set_format;
      
      guint format;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read format */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &format);

      set_format = ags_set_format_new((GObject *) audio,
				      format);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_format);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      g_object_unref(task_thread);

      return(start_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else if(!strncmp(path + path_offset,
		    "/AgsOutput",
		    10) ||
	   !strncmp(path + path_offset,
		    "/AgsInput",
		    9)){
    AgsChannel *start_channel;
    AgsChannel *channel, *next_channel;
      
    regmatch_t match_arr[2];

    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 2;
    static const size_t index_max_matches = 1;

    start_channel = NULL;

    if(!strncmp(path + path_offset,
		"/AgsOutput",
		10)){
      path_offset += 10;
      
      g_object_get(audio,
		   "output", &start_channel,
		   NULL);
    }else{
      path_offset += 9;
      
      g_object_get(audio,
		   "input", &start_channel,
		   NULL);
    }
    
    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i_stop;

      endptr = NULL;
      i_stop = g_ascii_strtoull(path + path_offset + 1,
				&endptr,
				10);
      
      channel = ags_channel_nth(start_channel,
				i_stop);

      path_offset += ((endptr + 1) - (path + path_offset));

      start_response = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
								 osc_connection,
								 channel,
								 message, message_size,
								 type_tag,
								 path, path_offset);

      if(start_channel != NULL){
	g_object_unref(start_channel);
      }
      
      if(channel != NULL){
	g_object_unref(channel);
      }
    }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      endptr = NULL;
      i_start = g_ascii_strtoull(path + path_offset + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				&endptr,
				10);

      path_offset += ((endptr + 1) - (path + path_offset));

      channel = ags_channel_nth(start_channel,
				i_start);

      next_channel = NULL;
      
      for(i = i_start; i <= i_stop && channel != NULL; i++){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							   osc_connection,
							   channel,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_channel != NULL){
	g_object_unref(start_channel);
      }
      
      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel != NULL){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							   osc_connection,
							   start_channel,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	g_object_unref(start_channel);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_object_unref(task_thread);

	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }

      /* prepare */
      channel = start_channel;
      g_object_ref(channel);

      next_channel = NULL;
      
      while(channel != NULL){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							   osc_connection,
							   channel,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_object_unref(task_thread);

	return(start_response);
      }

      /* prepare */
      channel = start_channel;
      g_object_ref(channel);

      next_channel = NULL;
      
      while(channel != NULL){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							   osc_connection,
							   channel,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		   NULL);

      /* unref */
      g_object_unref(task_thread);

      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      return(start_response);
    }
  }else{
    GType recall_type;
    
    GList *start_play, *play;
    GList *start_recall, *recall;

    regmatch_t match_arr[3];

    gchar *type_name;    
    gchar *str;

    guint type_name_length;
    guint str_length;
    
    static regex_t recall_regex;
    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *recall_pattern = "^\\/([a-zA-Z]+)(\\/|\\[[0-9]+\\]|\\[[0-9]+\\-[0-9]+\\]|\\[\\?\\]|\\[\\+\\]|\\[\\*\\]|:)";
    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 3;
    static const size_t index_max_matches = 2;

    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
    
      ags_regcomp(&recall_regex, recall_pattern, REG_EXTENDED);

      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    if(ags_regexec(&recall_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      type_name_length = match_arr[1].rm_eo - match_arr[1].rm_so;
      type_name = g_strndup(path + path_offset + 1,
			    type_name_length);

      str_length = match_arr[2].rm_eo - match_arr[2].rm_so;
      str = g_strndup(path + path_offset + 1 + type_name_length,
		      str_length);
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		   NULL);

      g_object_unref(task_thread);

      return(start_response);
    }

    recall_type = g_type_from_name(type_name);
    
    g_object_get(audio,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    if(ags_regexec(&single_access_regex, str, index_max_matches, match_arr, 0) == 0){
      guint i;
      guint i_stop;

      path_offset += (type_name_length + 1) + str_length;
      
      i_stop = g_ascii_strtoull(str + 1,
				NULL,
				10);

      play = start_play;
      recall = start_recall;
      
      for(i = 0; i <= i_stop; i++){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(i + 1 < i_stop){
	  if(play != NULL){
	    play = play->next;
	  }
	  
	  if(recall != NULL){
	    recall = recall->next;
	  }
	}
      }
      
      if(play != NULL){
	AgsRecall *current;

	GList *retval;

	current = play->data;	
	retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							  osc_connection,
							  current,
							  message, message_size,
							  type_tag,
							  path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							  osc_connection,
							  current,
							  message, message_size,
							  type_tag,
							  path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
    }else if(ags_regexec(&range_access_regex, str, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      path_offset += (type_name_length + 1) + str_length;

      endptr = NULL;
      i_start = g_ascii_strtoull(str + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				NULL,
				10);
      
      play = start_play;
      recall = start_recall;
      
      for(i = 0; i <= i_stop; i++){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(i >= i_start){
	  if(play != NULL){
	    AgsRecall *current;

	    GList *retval;

	    current = play->data;	
	    retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							      osc_connection,
							      current,
							      message, message_size,
							      type_tag,
							      path, path_offset);

	    if(start_response != NULL){
	      start_response = g_list_concat(start_response,
					     retval);
	    }else{
	      start_response = retval;
	    }
	  }
      
	  if(recall != NULL){
	    AgsRecall *current;

	    GList *retval;

	    current = recall->data;
	    retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							      osc_connection,
							      current,
							      message, message_size,
							      type_tag,
							      path, path_offset);

	    if(start_response != NULL){
	      start_response = g_list_concat(start_response,
					     retval);
	    }else{
	      start_response = retval;
	    }
	  }
	}
	
	if(i + 1 < i_stop){
	  if(play != NULL){
	    play = play->next;
	  }
	  
	  if(recall != NULL){
	    recall = recall->next;
	  }
	}
      }
    }else if(ags_regexec(&voluntary_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	current = recall->data;
	start_response = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
      }

      if(play == NULL && recall == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	g_object_unref(task_thread);

	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play == NULL && recall == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							  osc_connection,
							  current,
							  message, message_size,
							  type_tag,
							  path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      while(play != NULL || recall != NULL){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = play->data;	
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  start_response = g_list_concat(start_response,
					 retval);
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = recall->data;
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  start_response = g_list_concat(start_response,
					 retval);
	}

	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }else if(ags_regexec(&wildcard_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      while(play != NULL || recall != NULL){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;
	  
	  current = play->data;	
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;
	  
	  current = recall->data;
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}

	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		   NULL);

      g_list_free_full(start_play,
		       g_object_unref);
      g_list_free_full(start_recall,
		       g_object_unref);

      g_object_unref(task_thread);

      return(start_response);
    }

    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);
  } 

  g_object_unref(task_thread);
  
  return(start_response);
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

  GList *start_response;
  
  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }

  start_response = NULL;
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);
  
    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;

    type_tag_length = 4 * (guint) ceil((double) (strlen(type_tag) + 1) / 4.0);
    
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
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read samplerate */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &samplerate);

      set_samplerate = ags_set_samplerate_new((GObject *) channel,
					      samplerate);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_samplerate);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      AgsSetBufferSize *set_buffer_size;
      
      guint buffer_size;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read buffer size */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &buffer_size);

      set_buffer_size = ags_set_buffer_size_new((GObject *) channel,
						buffer_size);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_buffer_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      AgsSetFormat *set_format;
      
      guint format;
      gboolean success;
      
      success = (!strncmp(type_tag + 2, "i", 2)) ? TRUE: FALSE;
      
      if(!success ||
	 message_size < 16 + path_length + 4){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      /* read format */
      ags_osc_buffer_util_get_int32(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + type_tag_length + path_length,
				    &format);

      set_format = ags_set_format_new((GObject *) channel,
				      format);
      ags_task_thread_append_task((AgsTaskThread *) task_thread,
				  (AgsTask *) set_format);
    }else{
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      g_object_unref(task_thread);

      return(start_response);
    }

    g_object_set(osc_response,
		 "packet-size", packet_size,
		 NULL);
  }else{
    GType recall_type;
    
    GList *start_play, *play;
    GList *start_recall, *recall;

    regmatch_t match_arr[3];

    gchar *type_name;    
    gchar *str;

    guint type_name_length;
    guint str_length;
    
    static regex_t recall_regex;
    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *recall_pattern = "^\\/([a-zA-Z]+)(\\/|\\[[0-9]+\\]|\\[[0-9]+\\-[0-9]+\\]|\\[\\?\\]|\\[\\+\\]|\\[\\*\\]|:)";
    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 3;
    static const size_t index_max_matches = 2;

    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
    
      ags_regcomp(&recall_regex, recall_pattern, REG_EXTENDED);

      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);

    if(ags_regexec(&recall_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      type_name_length = match_arr[1].rm_eo - match_arr[1].rm_so;
      type_name = g_strndup(path + path_offset + 1,
			    type_name_length);

      str_length = match_arr[2].rm_eo - match_arr[2].rm_so;
      str = g_strndup(path + path_offset + 1 + type_name_length,
		      str_length);
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_PATH,
		   NULL);

      g_object_unref(task_thread);

      return(start_response);
    }

    recall_type = g_type_from_name(type_name);
    
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    if(ags_regexec(&single_access_regex, str, index_max_matches, match_arr, 0) == 0){
      guint i;
      guint i_stop;

      path_offset += (type_name_length + 1) + str_length;
      
      i_stop = g_ascii_strtoull(str + 1,
				NULL,
				10);

      play = start_play;
      recall = start_recall;
      
      for(i = 0; i <= i_stop; i++){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(i + 1 < i_stop){
	  if(play != NULL){
	    play = play->next;
	  }
	  
	  if(recall != NULL){
	    recall = recall->next;
	  }
	}
      }
      
      if(play != NULL){
	AgsRecall *current;

	GList *retval;

	current = play->data;	
	retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							  osc_connection,
							  current,
							  message, message_size,
							  type_tag,
							  path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							  osc_connection,
							  current,
							  message, message_size,
							  type_tag,
							  path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
    }else if(ags_regexec(&range_access_regex, str, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      path_offset += (type_name_length + 1) + str_length;

      endptr = NULL;
      i_start = g_ascii_strtoull(str + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				NULL,
				10);
      
      play = start_play;
      recall = start_recall;
      
      for(i = 0; i <= i_stop; i++){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(i >= i_start){
	  if(play != NULL){
	    AgsRecall *current;

	    GList *retval;

	    current = play->data;	
	    retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							      osc_connection,
							      current,
							      message, message_size,
							      type_tag,
							      path, path_offset);

	    if(start_response != NULL){
	      start_response = g_list_concat(start_response,
					     retval);
	    }else{
	      start_response = retval;
	    }
	  }
      
	  if(recall != NULL){
	    AgsRecall *current;

	    GList *retval;

	    current = recall->data;
	    retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							      osc_connection,
							      current,
							      message, message_size,
							      type_tag,
							      path, path_offset);

	    if(start_response != NULL){
	      start_response = g_list_concat(start_response,
					     retval);
	    }else{
	      start_response = retval;
	    }
	  }
	}
	
	if(i + 1 < i_stop){
	  if(play != NULL){
	    play = play->next;
	  }
	  
	  if(recall != NULL){
	    recall = recall->next;
	  }
	}
      }
    }else if(ags_regexec(&voluntary_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	current = recall->data;
	start_response = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
      }

      if(play == NULL && recall == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      play = ags_recall_template_find_type(play, recall_type);
      recall = ags_recall_template_find_type(recall, recall_type);

      if(play == NULL && recall == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }
      
      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
								  osc_connection,
								  current,
								  message, message_size,
								  type_tag,
								  path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	GList *retval;

	current = recall->data;
	retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							  osc_connection,
							  current,
							  message, message_size,
							  type_tag,
							  path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}
      }
      
      while(play != NULL || recall != NULL){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = play->data;	
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  start_response = g_list_concat(start_response,
					 retval);
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;

	  current = recall->data;
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  start_response = g_list_concat(start_response,
					 retval);
	}

	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }else if(ags_regexec(&wildcard_access_regex, str, index_max_matches, match_arr, 0) == 0){
      path_offset += (type_name_length + 1) + str_length;

      play = start_play;
      recall = start_recall;

      while(play != NULL || recall != NULL){
	play = ags_recall_template_find_type(play, recall_type);
	recall = ags_recall_template_find_type(recall, recall_type);

	if(play != NULL){
	  AgsRecall *current;

	  GList *retval;
	  
	  current = play->data;	
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}
      
	if(recall != NULL){
	  AgsRecall *current;

	  GList *retval;
	  
	  current = recall->data;
	  retval = ags_osc_renew_controller_set_data_recall(osc_renew_controller,
							    osc_connection,
							    current,
							    message, message_size,
							    type_tag,
							    path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }
	}

	if(play != NULL){
	  play = play->next;
	}
	  
	if(recall != NULL){
	  recall = recall->next;
	}
      }
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);

      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		   NULL);

      g_list_free_full(start_play,
		       g_object_unref);
      g_list_free_full(start_recall,
		       g_object_unref);

      g_object_unref(task_thread);
	
      return(start_response);
    }

    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);	
  } 

  g_object_unref(task_thread);
  
  return(start_response);
}

gpointer
ags_osc_renew_controller_set_data_recall(AgsOscRenewController *osc_renew_controller,
					 AgsOscConnection *osc_connection,
					 AgsRecall *recall,
					 unsigned char *message, guint message_size,
					 gchar *type_tag,
					 gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;
  
  AgsApplicationContext *application_context;

  GList *start_response;
  
  unsigned char *packet;

  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_RECALL(recall)){
    return(NULL);
  }

  start_response = NULL;
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

    packet = (unsigned char *) malloc(AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    memset(packet, 0, AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE * sizeof(unsigned char));
    
    g_object_set(osc_response,
		 "packet", packet,
		 "packet-size", packet_size,
		 NULL);
  
    real_packet_size = AGS_OSC_RESPONSE_DEFAULT_CHUNK_SIZE;
    
    path_length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);
    path_offset += 1;
	
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		 NULL);

    g_object_unref(task_thread);

    return(start_response);
  }else if(!strncmp(path + path_offset,
		    "/AgsPort",
		    8)){
    GList *start_port, *port;
    
    regmatch_t match_arr[3];

    static regex_t single_access_regex;
    static regex_t range_access_regex;
    static regex_t voluntary_access_regex;
    static regex_t more_access_regex;
    static regex_t wildcard_access_regex;
    
    static gboolean regex_compiled = FALSE;

    static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
    static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
    static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
    static const gchar *more_access_pattern = "^\\[(\\+)\\]";
    static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
    static const size_t max_matches = 3;
    static const size_t index_max_matches = 2;

    path_offset += 8;

    /* compile regex */
    pthread_mutex_lock(&regex_mutex);
  
    if(!regex_compiled){
      regex_compiled = TRUE;
      
      ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
      ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
      ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
      ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
      ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
    }

    pthread_mutex_unlock(&regex_mutex);
    
    g_object_get(recall,
		 "port", &start_port,
		 NULL);

    port = start_port;
    
    if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      AgsPort *current;
      
      gchar *endptr;
      
      guint i_stop;

      endptr = NULL;
      i_stop = g_ascii_strtoull(path + path_offset + 1,
				&endptr,
				10);
      
      current = g_list_nth_data(start_port,
				i_stop);

      path_offset += ((endptr + 1) - (path + path_offset));

      start_response = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							      osc_connection,
							      recall,
							      current,
							      message, message_size,
							      type_tag,
							      path, path_offset);
    }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
      gchar *endptr;
      
      guint i;
      guint i_start, i_stop;

      endptr = NULL;
      i_start = g_ascii_strtoull(path + path_offset + 1,
				 &endptr,
				 10);

      i_stop = g_ascii_strtoull(endptr + 1,
				&endptr,
				10);

      path_offset += ((endptr + 1) - (path + path_offset));

      port = g_list_nth(start_port,
			i_start);
      
      for(i = i_start; i <= i_stop; i++){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							osc_connection,
							recall,
							port->data,
							message, message_size,
							type_tag,
							path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	port = port->next;
      }
    }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_port != NULL){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							osc_connection,
							recall,
							start_port->data,
							message, message_size,
							type_tag,
							path, path_offset);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_object_unref(task_thread);
	
	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_port == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_object_unref(task_thread);

	return(start_response);
      }

      port = start_port;
      
      while(port != NULL){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							osc_connection,
							recall,
							port->data,
							message, message_size,
							type_tag,
							path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	port = port->next;
      }
    }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_port == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	g_object_unref(task_thread);

	return(start_response);
      }
      
      while(port != NULL){
	GList *retval;
	
	retval = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							osc_connection,
							recall,
							port->data,
							message, message_size,
							type_tag,
							path, path_offset);

	if(start_response != NULL){
	  start_response = g_list_concat(start_response,
					 retval);
	}else{
	  start_response = retval;
	}

	port = port->next;
      }
    }else if(path[path_offset] == '[' &&
	     path[path_offset + 1] == '"'){
      AgsPort *current;

      gchar *port_specifier;
      gchar *offset;

      guint length;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);

	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	g_list_free_full(start_port,
			 g_object_unref);

	g_object_unref(task_thread);
	
	return(start_response);
      }

      length = offset - (path + path_offset + 2);

      port_specifier = (gchar *) malloc((length + 1) * sizeof(gchar));
      memcpy(port_specifier, path + path_offset + 2, (length) * sizeof(gchar));
      port_specifier[length] = '\0';

      current = NULL;
      port = ags_port_find_specifier(start_port,
				     port_specifier);
	
      if(port != NULL){
	current = port->data;
      }	

      path_offset += (length + 4);
      start_response  = ags_osc_renew_controller_set_data_port(osc_renew_controller,
							       osc_connection,
							       recall,
							       current,
							       message, message_size,
							       type_tag,
							       path, path_offset);
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		   NULL);

      g_list_free_full(start_port,
		       g_object_unref);

      g_object_unref(task_thread);
      
      return(start_response);
    }    

    g_list_free_full(start_port,
		     g_object_unref);
  }else{
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		 NULL);

    g_object_unref(task_thread);

    return(start_response);
  }

  g_object_unref(task_thread);
  
  return(start_response);
}

gpointer
ags_osc_renew_controller_set_data_port(AgsOscRenewController *osc_renew_controller,
				       AgsOscConnection *osc_connection,
				       AgsRecall *parent,
				       AgsPort *port,
				       unsigned char *message, guint message_size,
				       gchar *type_tag,
				       gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;
  
  AgsThread *task_thread;
  
  GList *start_response;

  unsigned char *packet;

  guint path_length;
  guint type_tag_offset;
  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_PORT(port)){
    return(NULL);
  }

  start_response = NULL;
  
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  real_packet_size = 0;
  packet_size = 0;

  if(path[path_offset] == ':'){
    guint length;
    
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);

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
      
      path_offset = 4 * (guint) ceil((double) (path_offset + 7) / 4.0);
      
      if(port_value_is_pointer){
	guint i;

	if(message_size < 16 + path_length + 2 ||
	   type_tag[2] != '['){
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		       NULL);

	  g_object_unref(task_thread);

	  return(start_response);
	}
	
	if(port_value_type == G_TYPE_BOOLEAN){
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; 16 + path_length + 3 + i < message_size && type_tag[3 + i] != '\0' && type_tag[3 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[3 + i] == 'T'){
	      port->port_value.ags_port_boolean_ptr[i] = TRUE;
	    }else if(type_tag[3 + i] == 'F'){
	      port->port_value.ags_port_boolean_ptr[i] = FALSE;
	    }else{
	      success = FALSE;

	      break;
	    }
	  }

	  pthread_mutex_unlock(port_mutex);

	  if(!success ||
	     type_tag[3 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }
	  
	  value_count = i;
	}else if(port_value_type == G_TYPE_INT64){
	  gint64 value;
	  guint value_count;
	  gboolean success;

	  value_count = 0;
	  success = TRUE;
	  
	  for(i = 0; 16 + path_length + 3 + i < message_size && type_tag[3 + i] != '\0' && type_tag[3 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[3 + i] != 'h'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[3 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (5 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 8) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_int64(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset + (i * 8),
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
	  
	  for(i = 0; 16 + path_length + 3 + i < message_size && type_tag[3 + i] != '\0' && type_tag[3 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[3 + i] != 'h'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[3 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (5 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 8) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_int64(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset + (i * 8),
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
	  
	  for(i = 0; 16 + path_length + 3 + i < message_size && type_tag[3 + i] != '\0' && type_tag[3 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[3 + i] != 'f'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[3 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (5 + i) / 4.0);
	  value_count = i;
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 4) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_float(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset + (i * 4),
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
	  
	  for(i = 0; 16 + path_length + 3 + i < message_size && type_tag[3 + i] != '\0' && type_tag[3 + i] != ']' && i < port_value_length; i++){
	    if(type_tag[3 + i] != 'd'){
	      success = FALSE;

	      break;
	    }
	  }

	  if(!success ||
	     type_tag[3 + i] != ']'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }

	  type_tag_offset = 4 * (guint) ceil((double) (5 + i) / 4.0);
	  value_count = i;

	  pthread_mutex_lock(port_mutex);

	  for(i = 0; path_offset + type_tag_offset + (i * 8) < message_size && i < value_count; i++){
	    ags_osc_buffer_util_get_double(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset + (i * 8),
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
	    if(type_tag[2] == 'T'){
	      port->port_value.ags_port_boolean = TRUE;
	    }else if(type_tag[2] == 'F'){
	      port->port_value.ags_port_boolean = FALSE;
	    }else{
	      success = FALSE;
	    }
	  }else{
	    success = FALSE;
	  }
	  
	  pthread_mutex_unlock(port_mutex);

	  if(!success ||
	     type_tag[3] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }
	}else if(port_value_type == G_TYPE_INT64){
	  gint64 value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size >= 8 + path_offset + type_tag_offset + 8){
	    if(type_tag[2] == 'h'){
	      ags_osc_buffer_util_get_int64(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset,
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
	     type_tag[3] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }
	}else if(port_value_type == G_TYPE_UINT64){
	  guint64 value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size >= 8 + path_offset + type_tag_offset + 8){
	    if(type_tag[2] == 'h'){
	      ags_osc_buffer_util_get_int64(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset,
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
	     type_tag[3] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }
	}else if(port_value_type == G_TYPE_FLOAT){
	  gfloat value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;

	  if(message_size >= 8 + path_offset + type_tag_offset + 4){
	    if(type_tag[2] == 'f'){
	      ags_osc_buffer_util_get_float(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset,
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
	     type_tag[3] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }
	}else if(port_value_type == G_TYPE_DOUBLE){
	  gdouble value;
	  gboolean success;

	  type_tag_offset = 4;

	  success = TRUE;
	  
	  if(message_size >= 8 + path_offset + type_tag_offset + 8){
	    if(type_tag[2] == 'd'){
	      ags_osc_buffer_util_get_double(message + AGS_OSC_RENEW_CONTROLLER_CONTEXT_PATH_LENGTH + path_offset + type_tag_offset,
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
	     type_tag[3] != '\0'){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
			 NULL);

	    g_object_unref(task_thread);

	    return(start_response);
	  }
	}
      }
    }
  }

  g_object_unref(task_thread);

  return(start_response);
}

gpointer
ags_osc_renew_controller_real_set_data(AgsOscRenewController *osc_renew_controller,
				       AgsOscConnection *osc_connection,
				       unsigned char *message, guint message_size)
{
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;

  GList *start_response;
  
  gchar *type_tag;
  gchar *path;
  
  guint path_offset;
  gboolean success;

  start_response = NULL;
  
  /* read type tag */
  ags_osc_buffer_util_get_string(message + 8,
				 &type_tag, NULL);

  success = (type_tag != NULL &&
	     !strncmp(type_tag, ",s", 2)) ? TRUE: FALSE;

  if(!success){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
    
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    if(type_tag != NULL){
      free(type_tag);
    }
    
    return(start_response);
  }
  
  /* read argument */
  ags_osc_buffer_util_get_string(message + 8 + (4 * (guint) ceil((gdouble) (strlen(type_tag) + 1) / 4.0)),
				 &path, NULL);  

  if(path == NULL){
    osc_response = ags_osc_response_new();  
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    free(type_tag);
    
    return(start_response);
  }

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
      GList *start_soundcard, *soundcard;

      regmatch_t match_arr[2];

      static regex_t single_access_regex;
      static regex_t range_access_regex;
      static regex_t voluntary_access_regex;
      static regex_t more_access_regex;
      static regex_t wildcard_access_regex;
    
      static gboolean regex_compiled = FALSE;

      static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
      static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
      static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
      static const gchar *more_access_pattern = "^\\[(\\+)\\]";
      static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
      static const size_t max_matches = 2;
      static const size_t index_max_matches = 1;

      path_offset += 13;

      /* compile regex */
      pthread_mutex_lock(&regex_mutex);
  
      if(!regex_compiled){
	regex_compiled = TRUE;
      
	ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
	ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
	ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
	ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
	ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
      }

      pthread_mutex_unlock(&regex_mutex);

      soundcard = 
	start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

      if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	GObject *current;
      
	gchar *endptr;
      
	guint i_stop;

	endptr = NULL;
	i_stop = g_ascii_strtoull(path + path_offset + 1,
				  &endptr,
				  10);
      
	current = g_list_nth_data(start_soundcard,
				  i_stop);

	path_offset += ((endptr + 1) - (path + path_offset));

	start_response = ags_osc_renew_controller_set_data_soundcard(osc_renew_controller,
								     osc_connection,
								     current,
								     message, message_size,
								     type_tag,
								     path, path_offset);
      }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
	gchar *endptr;
      
	guint i;
	guint i_start, i_stop;

	endptr = NULL;
	i_start = g_ascii_strtoull(path + path_offset + 1,
				   &endptr,
				   10);

	i_stop = g_ascii_strtoull(endptr + 1,
				  &endptr,
				  10);

	path_offset += ((endptr + 1) - (path + path_offset));

	soundcard = g_list_nth(start_soundcard,
			       i_start);
      
	for(i = i_start; i <= i_stop; i++){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     soundcard->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  soundcard = soundcard->next;
	}
      }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_soundcard != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     start_soundcard->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);
	}else{
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}
      }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_soundcard == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		       NULL);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}

	soundcard = start_soundcard;
      
	while(soundcard != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     soundcard->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  soundcard = soundcard->next;
	}
      }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_soundcard == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}
      
	while(soundcard != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     soundcard->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  soundcard = soundcard->next;
	}
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_soundcard,
			 g_object_unref);
      
	free(type_tag);
	free(path);
	  
	return(start_response);
      }    

      g_list_free_full(start_soundcard,
		       g_object_unref);
    }else if(!strncmp(path + path_offset,
		      "/AgsSequencer",
		      13)){
      GList *start_sequencer, *sequencer;

      regmatch_t match_arr[2];

      static regex_t single_access_regex;
      static regex_t range_access_regex;
      static regex_t voluntary_access_regex;
      static regex_t more_access_regex;
      static regex_t wildcard_access_regex;
    
      static gboolean regex_compiled = FALSE;

      static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
      static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
      static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
      static const gchar *more_access_pattern = "^\\[(\\+)\\]";
      static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
      static const size_t max_matches = 2;
      static const size_t index_max_matches = 1;

      path_offset += 13;

      /* compile regex */
      pthread_mutex_lock(&regex_mutex);
  
      if(!regex_compiled){
	regex_compiled = TRUE;
      
	ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
	ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
	ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
	ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
	ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
      }

      pthread_mutex_unlock(&regex_mutex);

      sequencer = 
	start_sequencer = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

      if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	GObject *current;
      
	gchar *endptr;
      
	guint i_stop;

	endptr = NULL;
	i_stop = g_ascii_strtoull(path + path_offset + 1,
				  &endptr,
				  10);
      
	current = g_list_nth_data(start_sequencer,
				  i_stop);

	path_offset += ((endptr + 1) - (path + path_offset));

	start_response = ags_osc_renew_controller_set_data_sequencer(osc_renew_controller,
								     osc_connection,
								     current,
								     message, message_size,
								     type_tag,
								     path, path_offset);
      }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
	gchar *endptr;
      
	guint i;
	guint i_start, i_stop;

	endptr = NULL;
	i_start = g_ascii_strtoull(path + path_offset + 1,
				   &endptr,
				   10);

	i_stop = g_ascii_strtoull(endptr + 1,
				  &endptr,
				  10);

	path_offset += ((endptr + 1) - (path + path_offset));

	sequencer = g_list_nth(start_sequencer,
			       i_start);
      
	for(i = i_start; i <= i_stop; i++){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     sequencer->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  sequencer = sequencer->next;
	}
      }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_sequencer != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     start_sequencer->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);
	}else{
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}
      }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_sequencer == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		       NULL);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}

	sequencer = start_sequencer;
      
	while(sequencer != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     sequencer->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  sequencer = sequencer->next;
	}
      }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_sequencer == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}
      
	while(sequencer != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_channel(osc_renew_controller,
							     osc_connection,
							     sequencer->data,
							     message, message_size,
							     type_tag,
							     path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  sequencer = sequencer->next;
	}
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_sequencer,
			 g_object_unref);
      
	free(type_tag);
	free(path);
	  
	return(start_response);
      }    

      g_list_free_full(start_sequencer,
		       g_object_unref);
    }else if(!strncmp(path + path_offset,
		      "/AgsAudio",
		      9)){
      GList *start_audio, *audio;
      
      regmatch_t match_arr[2];

      static regex_t single_access_regex;
      static regex_t range_access_regex;
      static regex_t voluntary_access_regex;
      static regex_t more_access_regex;
      static regex_t wildcard_access_regex;
    
      static gboolean regex_compiled = FALSE;

      static const gchar *single_access_pattern = "^\\[([0-9]+)\\]";
      static const gchar *range_access_pattern = "^\\[([0-9]+)\\-([0-9]+)\\]";
      static const gchar *voluntary_access_pattern = "^\\[(\\?)\\]";
      static const gchar *more_access_pattern = "^\\[(\\+)\\]";
      static const gchar *wildcard_access_pattern = "^\\[(\\*)\\]";
    
      static const size_t max_matches = 2;
      static const size_t index_max_matches = 1;

      path_offset += 9;
      
      /* compile regex */
      pthread_mutex_lock(&regex_mutex);
  
      if(!regex_compiled){
	regex_compiled = TRUE;
      
	ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
	ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
	ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
	ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
	ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
      }

      pthread_mutex_unlock(&regex_mutex);

      audio = 
	start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

      if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	AgsAudio *current;
      
	gchar *endptr;
      
	guint i_stop;

	endptr = NULL;
	i_stop = g_ascii_strtoull(path + path_offset + 1,
				  &endptr,
				  10);
      
	current = g_list_nth_data(start_audio,
				  i_stop);

	path_offset += ((endptr + 1) - (path + path_offset));

	start_response = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);
      }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
	gchar *endptr;
      
	guint i;
	guint i_start, i_stop;

	endptr = NULL;
	i_start = g_ascii_strtoull(path + path_offset + 1,
				   &endptr,
				   10);

	i_stop = g_ascii_strtoull(endptr + 1,
				  &endptr,
				  10);

	path_offset += ((endptr + 1) - (path + path_offset));

	audio = g_list_nth(start_audio,
			   i_start);
      
	for(i = i_start; i <= i_stop; i++){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
							   osc_connection,
							   audio->data,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  audio = audio->next;
	}
      }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_audio != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
							   osc_connection,
							   start_audio->data,
							   message, message_size,
							   type_tag,
							   path, path_offset);
	}else{
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}
      }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_audio == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		       NULL);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}

	audio = start_audio;
      
	while(audio != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
							   osc_connection,
							   audio->data,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  audio = audio->next;
	}
      }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	path_offset += 3;

	if(start_audio == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
      
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);

	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}
      
	while(audio != NULL){
	  GList *retval;
	
	  retval = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
							   osc_connection,
							   audio->data,
							   message, message_size,
							   type_tag,
							   path, path_offset);

	  if(start_response != NULL){
	    start_response = g_list_concat(start_response,
					   retval);
	  }else{
	    start_response = retval;
	  }

	  audio = audio->next;
	}
      }else if(path[path_offset] == '[' &&
	       path[path_offset + 1] == '"'){
	AgsAudio *current;

	gchar *audio_name;
	gchar *offset;

	guint length;

	if((offset = index(path + path_offset + 2, '"')) == NULL){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);

	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		       NULL);
	  
	  g_list_free_full(start_audio,
			   g_object_unref);
      
	  free(type_tag);
	  free(path);
	  
	  return(start_response);
	}

	length = offset - (path + path_offset + 2);

	audio_name = (gchar *) malloc((length + 1) * sizeof(gchar));
	memcpy(audio_name, path + path_offset + 2, (length) * sizeof(gchar));
	audio_name[length] = '\0';

	current = NULL;
	audio = ags_audio_find_name(start_audio,
				    audio_name);
	
	if(audio != NULL){
	  current = audio->data;
	}	

	path_offset += (length + 4);
	start_response = ags_osc_renew_controller_set_data_audio(osc_renew_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	g_list_free_full(start_audio,
			 g_object_unref);
      
	free(type_tag);
	free(path);
	  
	return(start_response);
      }    

      g_list_free_full(start_audio,
		       g_object_unref);
    }else{
      osc_response = ags_osc_response_new();
      start_response = g_list_prepend(start_response,
				      osc_response);
      
      ags_osc_response_set_flags(osc_response,
				 AGS_OSC_RESPONSE_ERROR);

      g_object_set(osc_response,
		   "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_UNKNOW_ARGUMENT,
		   NULL);

      free(type_tag);
      free(path);
	  
      return(start_response);
    }
  }

  if(start_response == NULL){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		 NULL);

    free(type_tag);
    free(path);
	  
    return(start_response);
  }
  
  free(type_tag);
  free(path);
	  
  return(start_response);
}

/**
 * ags_osc_renew_controller_set_data:
 * @osc_renew_controller: the #AgsOscRenewController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Renew data.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
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
