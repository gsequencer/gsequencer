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

#include <ags/audio/osc/controller/ags_osc_node_controller.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/osc/ags_osc_response.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

void ags_osc_node_controller_class_init(AgsOscNodeControllerClass *osc_node_controller);
void ags_osc_node_controller_init(AgsOscNodeController *osc_node_controller);
void ags_osc_node_controller_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_osc_node_controller_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_osc_node_controller_dispose(GObject *gobject);
void ags_osc_node_controller_finalize(GObject *gobject);

gpointer ags_osc_node_controller_get_data_soundcard(AgsOscNodeController *osc_node_controller,
						    AgsOscConnection *osc_connection,
						    GObject *soundcard,
						    unsigned char *message, guint message_size,
						    gchar *type_tag,
						    gchar *path, guint path_offset);
gpointer ags_osc_node_controller_get_data_sequencer(AgsOscNodeController *osc_node_controller,
						    AgsOscConnection *osc_connection,
						    GObject *sequencer,
						    unsigned char *message, guint message_size,
						    gchar *type_tag,
						    gchar *path, guint path_offset);

gpointer ags_osc_node_controller_get_data_audio(AgsOscNodeController *osc_node_controller,
						AgsOscConnection *osc_connection,
						AgsAudio *audio,
						unsigned char *message, guint message_size,
						gchar *type_tag,
						gchar *path, guint path_offset);
gpointer ags_osc_node_controller_get_data_channel(AgsOscNodeController *osc_node_controller,
						  AgsOscConnection *osc_connection,
						  AgsChannel *channel,
						  unsigned char *message, guint message_size,
						  gchar *type_tag,
						  gchar *path, guint path_offset);

gpointer ags_osc_node_controller_get_data_port(AgsOscNodeController *osc_node_controller,
					       AgsOscConnection *osc_connection,
					       AgsPort *port,
					       unsigned char *message, guint message_size,
					       gchar *type_tag,
					       gchar *path, guint path_offset);

gpointer ags_osc_node_controller_real_get_data(AgsOscNodeController *osc_node_controller,
					       AgsOscConnection *osc_connection,
					       unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_node_controller
 * @short_description: OSC node controller
 * @title: AgsOscNodeController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_node_controller.h
 *
 * The #AgsOscNodeController implements the OSC node controller.
 */

enum{
  PROP_0,
};

enum{
  GET_DATA,
  LAST_SIGNAL,
};

static gpointer ags_osc_node_controller_parent_class = NULL;
static guint osc_node_controller_signals[LAST_SIGNAL];

GType
ags_osc_node_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_node_controller = 0;

    static const GTypeInfo ags_osc_node_controller_info = {
      sizeof (AgsOscNodeControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_node_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscNodeController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_node_controller_init,
    };
    
    ags_type_osc_node_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							  "AgsOscNodeController",
							  &ags_osc_node_controller_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_node_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_node_controller_class_init(AgsOscNodeControllerClass *osc_node_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_node_controller_parent_class = g_type_class_peek_parent(osc_node_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_node_controller;

  gobject->set_property = ags_osc_node_controller_set_property;
  gobject->get_property = ags_osc_node_controller_get_property;

  gobject->dispose = ags_osc_node_controller_dispose;
  gobject->finalize = ags_osc_node_controller_finalize;

  /* properties */

  /* AgsOscNodeControllerClass */
  osc_node_controller->get_data = ags_osc_node_controller_real_get_data;

  /* signals */
  /**
   * AgsOscNodeController::get-data:
   * @osc_node_controller: the #AgsOscNodeController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::get-data signal is emited during get data of node controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.1.0
   */
  osc_node_controller_signals[GET_DATA] =
    g_signal_new("get-data",
		 G_TYPE_FROM_CLASS(osc_node_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscNodeControllerClass, get_data),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_node_controller_init(AgsOscNodeController *osc_node_controller)
{
  g_object_set(osc_node_controller,
	       "context-path", "/node",
	       NULL);
}

void
ags_osc_node_controller_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsOscNodeController *osc_node_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_node_controller = AGS_OSC_NODE_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_node_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_node_controller_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsOscNodeController *osc_node_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_node_controller = AGS_OSC_NODE_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_node_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_node_controller_dispose(GObject *gobject)
{
  AgsOscNodeController *osc_node_controller;

  osc_node_controller = AGS_OSC_NODE_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_node_controller_parent_class)->dispose(gobject);
}

void
ags_osc_node_controller_finalize(GObject *gobject)
{
  AgsOscNodeController *osc_node_controller;

  osc_node_controller = AGS_OSC_NODE_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_node_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_node_controller_get_data_soundcard(AgsOscNodeController *osc_node_controller,
					   AgsOscConnection *osc_connection,
					   GObject *soundcard,
					   unsigned char *message, guint message_size,
					   gchar *type_tag,
					   gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_SOUNDCARD(soundcard)){
    return(NULL);
  }
  
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

    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"device",
		7)){
      gchar *device;

      guint length;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",ss", -1);

      /* node path */
      packet_size += 4;
      
      length = strlen(path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }
      
      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard));
      length = strlen(device);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     device, -1);

      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "pcm-channels",
		      13)){
      guint pcm_channels;
      guint length;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;      

      length = strlen(path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
				&pcm_channels,
				NULL,
				NULL,
				NULL);	  
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    pcm_channels);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      guint samplerate;
      guint length;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);

      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));
      
      packet_size = 16 + length;      

      ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
				NULL,
				&samplerate,
				NULL,
				NULL);	  
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    samplerate);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      guint buffer_size;
      guint length;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
				NULL,
				NULL,
				&buffer_size,
				NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    buffer_size);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      guint format;
      guint length;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      length = strlen(path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;
      
      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
				NULL,
				NULL,
				NULL,
				&format);
	  
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    format);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
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
ags_osc_node_controller_get_data_sequencer(AgsOscNodeController *osc_node_controller,
					   AgsOscConnection *osc_connection,
					   GObject *sequencer,
					   unsigned char *message, guint message_size,
					   gchar *type_tag,
					   gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_SEQUENCER(sequencer)){
    return(NULL);
  }
  
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

    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"device",
		7)){
      gchar *device;

      guint length;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",ss", -1);

      /* node path */
      packet_size += 4;
      
      length = strlen(path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }
      
      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));
      
      device = ags_sequencer_get_device(AGS_SEQUENCER(sequencer));
      length = strlen(device);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     device, -1);

      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
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
ags_osc_node_controller_get_data_audio(AgsOscNodeController *osc_node_controller,
				       AgsOscConnection *osc_connection,
				       AgsAudio *audio,
				       unsigned char *message, guint message_size,
				       gchar *type_tag,
				       gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }
  
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

    path_offset += 1;
    
    /* properties */
    if(!strncmp(path + path_offset,
		"audio-channels",
		15)){
      guint audio_channels;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;
      
      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(audio,
		   "audio-channels", &audio_channels,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    audio_channels);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "output-pads",
		      12)){
      guint output_pads;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(audio,
		   "output-pads", &output_pads,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    output_pads);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "input-pads",
		      11)){
      guint input_pads;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(audio,
		   "input-pads", &input_pads,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    input_pads);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "buffer-size",
		      12)){
      guint buffer_size;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(audio,
		   "buffer-size", &buffer_size,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    buffer_size);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      guint samplerate;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(audio,
		   "samplerate", &samplerate,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    samplerate);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      guint format;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(audio,
		   "format", &format,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    format);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
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

    osc_response = ags_osc_node_controller_get_data_channel(osc_node_controller,
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

    osc_response = ags_osc_node_controller_get_data_channel(osc_node_controller,
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

      osc_response = ags_osc_node_controller_get_data_port(osc_node_controller,
							   osc_connection,
							   port,
							   message, message_size,
							   type_tag,
							   path, path_offset);

      g_list_free(start_port);
    }else if(path[path_offset + 1] == '"'){
      GList *list;
	
      gchar *specifier;

      guint length;
      guint offset;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	return(osc_response);
      }

      length = path - offset;

      specifier = malloc(length * sizeof(gchar));
      sscanf(path + path_offset, "", &specifier);

      g_object_get(audio,
		   "port", &start_port,
		   NULL);	

      list = ags_port_find_specifier(start_port,
				     specifier);

      if(list != NULL){
	port = list->data;
      }

      osc_response = ags_osc_node_controller_get_data_port(osc_node_controller,
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
ags_osc_node_controller_get_data_channel(AgsOscNodeController *osc_node_controller,
					 AgsOscConnection *osc_connection,
					 AgsChannel *channel,
					 unsigned char *message, guint message_size,
					 gchar *type_tag,
					 gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }
  
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
    
    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"buffer-size",
		12)){
      guint buffer_size;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(channel,
		   "buffer-size", &buffer_size,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    buffer_size);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "samplerate",
		      11)){
      guint samplerate;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(channel,
		   "samplerate", &samplerate,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    samplerate);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
    }else if(!strncmp(path + path_offset,
		      "format",
		      7)){
      guint format;
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;

      length = strlen(path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(osc_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     path, -1);
      
      /* node argument */
      packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

      g_object_get(channel,
		   "format", &format,
		   NULL);
      ags_osc_buffer_util_put_int32(packet + packet_size,
				    format);

      packet_size += 4;
      
      /* packet size */
      ags_osc_buffer_util_put_int32(packet,
				    packet_size);
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

      osc_response = ags_osc_node_controller_get_data_port(osc_node_controller,
							   osc_connection,
							   port,
							   message, message_size,
							   type_tag,
							   path, path_offset);

      g_list_free(start_port);
    }else if(path[path_offset + 1] == '"'){
      GList *list;
	
      gchar *specifier;

      guint length;
      guint offset;

      if((offset = index(path + path_offset + 2, '"')) == NULL){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	return(osc_response);
      }

      length = path - offset;

      specifier = malloc(length * sizeof(gchar));
      sscanf(path + path_offset, "", &specifier);
	
      g_object_get(channel,
		   "port", &start_port,
		   NULL);

      list = ags_port_find_specifier(start_port,
				     specifier);

      if(list != NULL){
	port = list->data;
      }

      osc_response = ags_osc_node_controller_get_data_port(osc_node_controller,
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
ags_osc_node_controller_get_data_port(AgsOscNodeController *osc_node_controller,
				      AgsOscConnection *osc_connection,
				      AgsPort *port,
				      unsigned char *message, guint message_size,
				      gchar *type_tag,
				      gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  unsigned char *packet;

  guint real_packet_size;
  guint packet_size;

  if(!AGS_IS_PORT(port)){
    return(NULL);
  }

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
    
    path_offset += 1;

    if(!strncmp(path + path_offset,
		"value",
		6)){
      GType port_value_type;

      guint port_value_length;
      gboolean port_value_is_pointer;
      
      pthread_mutex_t *port_mutex;

      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      packet_size += 8;

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
      
      if(port_value_is_pointer){
	gchar *type_tag;
	
	guint i;

	/* message type tag */
	if(packet_size + (4 * (guint) ceil((double) (port_value_length + 5) / 4.0)) > real_packet_size){
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);

	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		       NULL);

	  return(osc_response);
	}

	type_tag = (gchar *) malloc((port_value_length + 5) * sizeof(gchar));

	type_tag[0] = ',';
	type_tag[1] = 's';
	type_tag[2] = '[';
	type_tag[port_value_length + 4] = ']';
	
	if(port_value_type == G_TYPE_BOOLEAN){	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    gboolean is_active;
	    
	    is_active = port->port_value.ags_port_boolean_ptr[i];

	    if(is_active){
	      packet[packet_size + 3 + i] = 'T';
	    }else{
	      packet[packet_size + 3 + i] = 'F';
	    }
	  }
	  
	  pthread_mutex_unlock(port_mutex);
	  
	  /* node path */
	  packet_size += (4 * (guint) ceil((double) (port_value_length + 5) / 4.0));
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_INT64){
	  for(i = 0; i < port_value_length; i++){
	    packet[packet_size + 3 + i] = 'h';
	  }

	  /* node path */
	  packet_size += (4 * (guint) ceil((double) (port_value_length + 5) / 4.0));
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    gint64 value;

	    value = port->port_value.ags_port_int_ptr[i];
	    
	    ags_osc_buffer_util_put_int64(packet + packet_size + 3 + (i * 8),
					  value);
	  }

	  pthread_mutex_unlock(port_mutex);

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_UINT64){
	  for(i = 0; i < port_value_length; i++){
	    packet[packet_size + 3 + i] = 'h';
	  }

	  /* node path */
	  packet_size += (4 * (guint) ceil((double) (port_value_length + 5) / 4.0));
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    guint64 value;

	    value = port->port_value.ags_port_uint_ptr[i];

	    //FIXME:JK: unsafe sign
	    ags_osc_buffer_util_put_int64(packet + packet_size + 3 + (i * 8),
					  value);
	  }

	  pthread_mutex_unlock(port_mutex);

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_FLOAT){
	  for(i = 0; i < port_value_length; i++){
	    packet[packet_size + 3 + i] = 'f';
	  }

	  /* node path */
	  packet_size += (4 * (guint) ceil((double) (port_value_length + 5) / 4.0));
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 4) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    gfloat value;

	    value = port->port_value.ags_port_float_ptr[i];
	    
	    ags_osc_buffer_util_put_float(packet + packet_size + 3 + (i * 4),
					  value);
	  }

	  pthread_mutex_unlock(port_mutex);

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_DOUBLE){
	  for(i = 0; i < port_value_length; i++){
	    packet[packet_size + 3 + i] = 'd';
	  }
	  
	  /* node path */
	  packet_size += (4 * (guint) ceil((double) (port_value_length + 5) / 4.0));
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }
	  
	  pthread_mutex_lock(port_mutex);

	  for(i = 0; i < port_value_length; i++){
	    gdouble value;

	    value = port->port_value.ags_port_double_ptr[i];
	    
	    ags_osc_buffer_util_put_double(packet + packet_size + 3 + (i * 8),
					   value);
	  }

	  pthread_mutex_unlock(port_mutex);

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}
      }else{
	if(port_value_type == G_TYPE_BOOLEAN){
	  gboolean is_active;
	    
	  /* message type tag */
	  pthread_mutex_lock(port_mutex);

	  is_active = port->port_value.ags_port_boolean;
	  
	  pthread_mutex_unlock(port_mutex);

	  if(is_active){
	    ags_osc_buffer_util_put_string(packet + packet_size,
					   ",sT", -1);
	  }else{
	    ags_osc_buffer_util_put_string(packet + packet_size,
					   ",sF", -1);
	  }

	  /* node path */	    
	  packet_size += 4;
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_INT64){
	  gint64 value;
	  
	  /* message type tag */
	  pthread_mutex_lock(port_mutex);

	  value = port->port_value.ags_port_int;
	  
	  pthread_mutex_unlock(port_mutex);

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 ",sh", -1);

	  /* node path */	    
	  packet_size += 4;
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  ags_osc_buffer_util_put_int64(packet + packet_size,
					value);
	  
	  packet_size += 8;
	  
	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_UINT64){
	  guint64 value;
	  
	  /* message type tag */
	  pthread_mutex_lock(port_mutex);

	  value = port->port_value.ags_port_uint;
	  
	  pthread_mutex_unlock(port_mutex);

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 ",sh", -1);

	  /* node path */	    
	  packet_size += 4;
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  //FIXME:JK: unsafe sign
	  ags_osc_buffer_util_put_int64(packet + packet_size,
					value);
	  
	  packet_size += 8;
	  
	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_FLOAT){
	  gfloat value;
	  
	  /* message type tag */
	  pthread_mutex_lock(port_mutex);

	  value = port->port_value.ags_port_float;
	  
	  pthread_mutex_unlock(port_mutex);

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 ",sf", -1);

	  /* node path */	    
	  packet_size += 4;
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  ags_osc_buffer_util_put_float(packet + packet_size,
					value);
	  
	  packet_size += 4;
	  
	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}else if(port_value_type == G_TYPE_DOUBLE){
	  gdouble value;
	  
	  /* message type tag */
	  pthread_mutex_lock(port_mutex);

	  value = port->port_value.ags_port_double;
	  
	  pthread_mutex_unlock(port_mutex);

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 ",sd", -1);

	  /* node path */	    
	  packet_size += 4;
	  
	  length = strlen(path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(osc_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  ags_osc_buffer_util_put_double(packet + packet_size,
					 value);
	  
	  packet_size += 8;
	  
	  /* packet size */
	  ags_osc_buffer_util_put_int32(packet,
					packet_size);
	}
      }

      g_object_set(osc_response,
		   "packet-size", packet_size,
		   NULL);
    }
  }

  return(osc_response);
}

gpointer
ags_osc_node_controller_real_get_data(AgsOscNodeController *osc_node_controller,
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

  success = (!strncmp(type_tag, ",s", 3)) ? TRUE: FALSE;

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

      osc_response = ags_osc_node_controller_get_data_soundcard(osc_node_controller,
								osc_connection,
								soundcard,
								message, message_size,
								type_tag,
								path, path_offset);
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

      osc_response = ags_osc_node_controller_get_data_sequencer(osc_node_controller,
								osc_connection,
								sequencer,
								message, message_size,
								type_tag,
								path, path_offset);
    }else if(!strncmp(path + path_offset,
		      "/AgsAudio",
		      9)){
      AgsAudio *audio;
      
      GList *start_audio;
      
      guint nth_audio;
      int retval;
      
      start_audio = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
      path_offset += 9;

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

      osc_response = ags_osc_node_controller_get_data_audio(osc_node_controller,
							    osc_connection,
							    audio,
							    message, message_size,
							    type_tag,
							    path, path_offset);
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
 * ags_osc_node_controller_get_data:
 * @osc_node_controller: the #AgsOscNodeController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get node.
 * 
 * Returns: the #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_node_controller_get_data(AgsOscNodeController *osc_node_controller,
				 AgsOscConnection *osc_connection,
				 unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_NODE_CONTROLLER(osc_node_controller), NULL);
  
  g_object_ref((GObject *) osc_node_controller);
  g_signal_emit(G_OBJECT(osc_node_controller),
		osc_node_controller_signals[GET_DATA], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_node_controller);

  return(osc_response);
}

/**
 * ags_osc_node_controller_new:
 * 
 * Instantiate new #AgsOscNodeController
 * 
 * Returns: the #AgsOscNodeController
 * 
 * Since: 2.1.0
 */
AgsOscNodeController*
ags_osc_node_controller_new()
{
  AgsOscNodeController *osc_node_controller;

  osc_node_controller = (AgsOscNodeController *) g_object_new(AGS_TYPE_OSC_NODE_CONTROLLER,
							      NULL);

  return(osc_node_controller);
}
