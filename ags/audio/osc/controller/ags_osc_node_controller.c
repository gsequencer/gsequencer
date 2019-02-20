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
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <regex.h>

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

gpointer ags_osc_node_controller_get_data_recall(AgsOscNodeController *osc_node_controller,
						 AgsOscConnection *osc_connection,
						 AgsRecall *recall,
						 unsigned char *message, guint message_size,
						 gchar *type_tag,
						 gchar *path, guint path_offset);

gpointer ags_osc_node_controller_get_data_port(AgsOscNodeController *osc_node_controller,
					       AgsOscConnection *osc_connection,
					       AgsRecall *parent,
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

static pthread_mutex_t regex_mutex = PTHREAD_MUTEX_INITIALIZER;

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

  AgsApplicationContext *application_context;  

  GList *start_response;
  GList *start_list;
  
  unsigned char *packet;
  gchar *current_path;
  
  guint real_packet_size;
  guint packet_size;
  gint nth_soundcard;
  
  if(!AGS_IS_SOUNDCARD(soundcard)){
    return(NULL);
  }
  
  start_response = NULL;

  application_context = ags_application_context_get_instance();
  
  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    /* get nth soundcard */
    start_list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

    nth_soundcard = g_list_index(start_list,
				 soundcard);

    g_list_free(start_list);
    
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

    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"device",
		7)){
      gchar *device;

      guint length;

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsSoundcard[%d]:device", nth_soundcard);    

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
      
      length = strlen(current_path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }
      
      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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

	return(start_response);
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

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsSoundcard[%d]:pcm-channels", nth_soundcard);

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

      length = strlen(current_path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsSoundcard[%d]:samplerate", nth_soundcard);
      
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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);

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

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsSoundcard[%d]:buffer-size", nth_soundcard);
      
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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsSoundcard[%d]:format", nth_soundcard);
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      /* message type tag */
      packet_size += 8;

      length = strlen(current_path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     ",si", -1);

      /* node path */
      packet_size += 4;
      
      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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

    return(start_response);
  }
  
  return(start_response);
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

  AgsApplicationContext *application_context;  

  GList *start_response;
  GList *start_list;

  unsigned char *packet;
  gchar *current_path;

  guint real_packet_size;
  guint packet_size;
  gint nth_sequencer;

  if(!AGS_IS_SEQUENCER(sequencer)){
    return(NULL);
  }
  
  start_response = NULL;

  application_context = ags_application_context_get_instance();
  
  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    /* get nth sequencer */
    start_list = ags_sound_provider_get_sequencer(AGS_SOUND_PROVIDER(application_context));

    nth_sequencer = g_list_index(start_list,
				 sequencer);

    g_list_free(start_list);

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

    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"device",
		7)){
      gchar *device;

      guint length;

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsSequencer[%d]:device", nth_sequencer);    

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
      
      length = strlen(current_path);

      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }
      
      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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

	return(start_response);
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

    return(start_response);
  }

  return(start_response);
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

  AgsApplicationContext *application_context;  

  GList *start_response;
  GList *start_list;

  unsigned char *packet;
  gchar *current_path;

  guint real_packet_size;
  guint packet_size;
  gint nth_audio;
  
  if(!AGS_IS_AUDIO(audio)){
    return(NULL);
  }
  
  start_response = NULL;

  application_context = ags_application_context_get_instance();

  real_packet_size = 0;
  packet_size = 0;
  
  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;
    
    /* get nth audio */
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

    nth_audio = g_list_index(start_list,
			     audio);

    g_list_free(start_list);

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

    path_offset += 1;
    
    /* properties */
    if(!strncmp(path + path_offset,
		"audio-channels",
		15)){
      guint audio_channels;
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]:audio-channels", nth_audio);    

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
      
      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]:output-pads", nth_audio);    

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]:input-pads", nth_audio);    

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]:buffer-size", nth_audio);    

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]:samplerate", nth_audio);    

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]:format", nth_audio);    

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
    AgsChannel *start_channel, *channel;
      
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

    if(!strncmp(path + path_offset,
		"/AgsOutput",
		10)){
      path_offset += 10;
      
      g_object_get(audio,
		   "output", &start_channel,
		   NULL);
      g_object_unref(start_channel);
    }else{
      path_offset += 9;
      
      g_object_get(audio,
		   "input", &start_channel,
		   NULL);
      g_object_unref(start_channel);
    }

    channel = start_channel;
    
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

      start_response = ags_osc_node_controller_get_data_channel(osc_node_controller,
								osc_connection,
								channel,
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

      channel = ags_channel_nth(start_channel,
				i_start);
      
      for(i = i_start; i <= i_stop; i++){
	GList *retval;
	
	retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	g_object_get(channel,
		     "next", &channel,
		     NULL);
	
	if(channel != NULL){
	  g_object_unref(channel);
	}
      }
    }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(start_channel != NULL){
	GList *retval;
	
	retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
							  osc_connection,
							  start_channel,
							  message, message_size,
							  type_tag,
							  path, path_offset);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	return(start_response);
      }
    }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(channel == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_SERVER_FAILURE,
		     NULL);

	return(start_response);
      }

      while(channel != NULL){
	GList *retval;
	
	retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	g_object_get(channel,
		     "next", &channel,
		     NULL);
	
	if(channel != NULL){
	  g_object_unref(channel);
	}
      }
    }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
      path_offset += 3;

      if(channel == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);

	return(start_response);
      }
      
      while(channel != NULL){
	GList *retval;
	
	retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	g_object_get(channel,
		     "next", &channel,
		     NULL);
	
	if(channel != NULL){
	  g_object_unref(channel);
	}
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
	retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	    retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	    retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	start_response = ags_osc_node_controller_get_data_recall(osc_node_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	current = recall->data;
	start_response = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	
	return(start_response);
      }
      
      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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

      return(start_response);
    }

    g_list_free_full(start_play,
		     g_object_unref);
    g_list_free_full(start_recall,
		     g_object_unref);
  }

  return(start_response);
}

gpointer
ags_osc_node_controller_get_data_channel(AgsOscNodeController *osc_node_controller,
					 AgsOscConnection *osc_connection,
					 AgsChannel *channel,
					 unsigned char *message, guint message_size,
					 gchar *type_tag,
					 gchar *path, guint path_offset)
{
  AgsAudio *audio;
  
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;  

  GList *start_response;
  GList *start_list;

  unsigned char *packet;
  gchar *current_path;

  guint real_packet_size;
  guint packet_size;
  gint nth_audio;
  gint nth_channel;
  
  if(!AGS_IS_CHANNEL(channel)){
    return(NULL);
  }
  
  start_response = NULL;

  application_context = ags_application_context_get_instance();

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;

    g_object_get(channel,
		 "audio", &audio,
		 "line", &nth_channel,
		 NULL);
    
    g_object_unref(audio);

    /* get nth audio */
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

    nth_audio = g_list_index(start_list,
			     audio);

    g_list_free(start_list);
    
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
    
    path_offset += 1;
	
    if(!strncmp(path + path_offset,
		"buffer-size",
		12)){
      guint buffer_size;
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s[%d]:buffer-size",
				     nth_audio,
				     ((AGS_IS_OUTPUT(channel)) ? "AgsOutput": "AgsInput"),
				     nth_channel);

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
      
      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s[%d]:samplerate",
				     nth_audio,
				     ((AGS_IS_OUTPUT(channel)) ? "AgsOutput": "AgsInput"),
				     nth_channel);

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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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

      /* create current path */
      current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s[%d]:format",
				     nth_audio,
				     ((AGS_IS_OUTPUT(channel)) ? "AgsOutput": "AgsInput"),
				     nth_channel);
      
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

      length = strlen(current_path);
      
      if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 4 > real_packet_size){
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);

	return(start_response);
      }

      ags_osc_buffer_util_put_string(packet + packet_size,
				     current_path, -1);
      
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
	retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	    retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	    retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	start_response = ags_osc_node_controller_get_data_recall(osc_node_controller,
								 osc_connection,
								 current,
								 message, message_size,
								 type_tag,
								 path, path_offset);
      }
      
      if(recall != NULL){
	AgsRecall *current;

	current = recall->data;
	start_response = ags_osc_node_controller_get_data_recall(osc_node_controller,
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

	g_list_free(start_play);
	g_list_free(start_recall);
	
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

	g_list_free(start_play);
	g_list_free(start_recall);
	
	return(start_response);
      }
      
      if(play != NULL){
	AgsRecall *current;

	current = play->data;	
	start_response = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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
	  retval = ags_osc_node_controller_get_data_recall(osc_node_controller,
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

      g_list_free(start_play);
      g_list_free(start_recall);

      return(start_response);
    }

    g_list_free(start_play);
    g_list_free(start_recall);    
  }

  return(start_response);
}

gpointer
ags_osc_node_controller_get_data_recall(AgsOscNodeController *osc_node_controller,
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

      start_response = ags_osc_node_controller_get_data_port(osc_node_controller,
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
	
	retval = ags_osc_node_controller_get_data_port(osc_node_controller,
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
	
	retval = ags_osc_node_controller_get_data_port(osc_node_controller,
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
	
	retval = ags_osc_node_controller_get_data_port(osc_node_controller,
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
	
	retval = ags_osc_node_controller_get_data_port(osc_node_controller,
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
      start_response  = ags_osc_node_controller_get_data_port(osc_node_controller,
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
ags_osc_node_controller_get_data_port(AgsOscNodeController *osc_node_controller,
				      AgsOscConnection *osc_connection,
				      AgsRecall *parent,
				      AgsPort *port,
				      unsigned char *message, guint message_size,
				      gchar *type_tag,
				      gchar *path, guint path_offset)
{
  AgsAudio *audio;
  AgsChannel *channel;
  
  AgsOscResponse *osc_response;

  AgsApplicationContext *application_context;

  GList *start_response;
  GList *start_list;

  unsigned char *packet;
  gchar *current_path;
  gchar *specifier;
  
  guint real_packet_size;
  guint packet_size;
  gint nth_audio;
  gint nth_channel;

  if(!AGS_IS_PORT(port)){
    return(NULL);
  }

  start_response = NULL;

  application_context = ags_application_context_get_instance();

  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      ":",
	      1)){
    guint length;

    audio = NULL;
    channel = NULL;
    
    if(AGS_IS_RECALL_CHANNEL(parent)){
      g_object_get(parent,
		   "source", &channel,
		   NULL);
      
      g_object_get(channel,
		   "audio", &audio,
		   "line", &nth_channel,
		   NULL);      

      g_object_unref(channel);

      g_object_unref(audio);
    }else if(AGS_IS_RECALL_AUDIO(parent)){
      g_object_get(parent,
		   "audio", &audio,
		   NULL);

      g_object_unref(audio);
    }
    
    /* get nth audio */
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

    nth_audio = g_list_index(start_list,
			     audio);

    g_list_free(start_list);

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

      /* create current path */
      pthread_mutex_lock(port_mutex);

      specifier = g_strdup(port->specifier);
      
      pthread_mutex_unlock(port_mutex);
      
      if(channel != NULL){
	current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s[%d]/%s/AgsPort[%s]:value",
				       nth_audio,
				       ((AGS_IS_OUTPUT(channel)) ? "AgsOutput": "AgsInput"),
				       nth_channel,
				       G_OBJECT_TYPE_NAME(parent),
				       specifier);
      }else{
	current_path = g_strdup_printf("/AgsSoundProvider/AgsAudio[%d]/%s/AgsPort[%s]:value",
				       nth_audio,
				       G_OBJECT_TYPE_NAME(parent),
				       specifier);
      }

      g_free(specifier);
      
      /* message path */
      packet_size = 4;

      ags_osc_buffer_util_put_string(packet + packet_size,
				     "/node", -1);
      
      packet_size += 8;

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

	  return(start_response);
	}

	type_tag = packet + packet_size; // (gchar *) malloc((port_value_length + 5) * sizeof(gchar));

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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 4) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

	  /* node argument */
	  packet_size += (4 * (guint) ceil((double) (length + 1) / 4.0));

	  if(packet_size + (4 * (guint) ceil((double) (port_value_length * 8) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

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
	  
	  length = strlen(current_path);

	  if(packet_size + (4 * (guint) ceil((double) (length + 1) / 4.0)) + 8 > real_packet_size){
	    ags_osc_response_set_flags(osc_response,
				       AGS_OSC_RESPONSE_ERROR);

	    g_object_set(osc_response,
			 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
			 NULL);

	    return(start_response);
	  }

	  ags_osc_buffer_util_put_string(packet + packet_size,
					 current_path, -1);

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

  return(start_response);
}

gpointer
ags_osc_node_controller_real_get_data(AgsOscNodeController *osc_node_controller,
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

	start_response = ags_osc_node_controller_get_data_soundcard(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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

	  g_list_free(start_soundcard);
      
	  free(type_tag);
	  free(path);

	  return(start_response);
	}

	soundcard = start_soundcard;
      
	while(soundcard != NULL){
	  GList *retval;
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	
	g_list_free(start_soundcard);
	
	free(type_tag);
	free(path);

	return(start_response);
      }    

      g_list_free(start_soundcard);
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

	start_response = ags_osc_node_controller_get_data_sequencer(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_channel(osc_node_controller,
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

	g_list_free(start_sequencer);
      
	free(type_tag);
	free(path);

	return(start_response);
      }    

      g_list_free(start_sequencer);
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

	start_response = ags_osc_node_controller_get_data_audio(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_audio(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_audio(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_audio(osc_node_controller,
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
	
	  retval = ags_osc_node_controller_get_data_audio(osc_node_controller,
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
	  
	  g_list_free(start_audio);
      
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
	start_response = ags_osc_node_controller_get_data_audio(osc_node_controller,
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

	g_list_free(start_audio);
      
	free(type_tag);
	free(path);

	return(start_response);
      }    

      g_list_free(start_audio);
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
 * ags_osc_node_controller_get_data:
 * @osc_node_controller: the #AgsOscNodeController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Get node.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
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
