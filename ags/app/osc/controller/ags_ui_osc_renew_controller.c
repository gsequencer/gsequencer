/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/osc/controller/ags_ui_osc_renew_controller.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_effect_bridge.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line.h>
#include <ags/app/ags_effect_pad.h>
#include <ags/app/ags_effect_line.h>
#include <ags/app/ags_line_member.h>
#include <ags/app/ags_bulk_member.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_spectrometer.h>
#include <ags/app/machine/ags_equalizer10.h>
#include <ags/app/machine/ags_mixer.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_synth.h>
#include <ags/app/machine/ags_fm_synth.h>
#include <ags/app/machine/ags_syncsynth.h>
#include <ags/app/machine/ags_fm_syncsynth.h>
#include <ags/app/machine/ags_ffplayer.h>
#include <ags/app/machine/ags_pitch_sampler.h>
#include <ags/app/machine/ags_audiorec.h>

#include <ags/i18n.h>

void ags_ui_osc_renew_controller_class_init(AgsUiOscRenewControllerClass *ui_osc_renew_controller);
void ags_ui_osc_renew_controller_osc_plugin_controller_interface_init(AgsOscPluginControllerInterface *osc_plugin_controller);
void ags_ui_osc_renew_controller_init(AgsUiOscRenewController *ui_osc_renew_controller);
void ags_ui_osc_renew_controller_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_ui_osc_renew_controller_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_ui_osc_renew_controller_dispose(GObject *gobject);
void ags_ui_osc_renew_controller_finalize(GObject *gobject);

gpointer ags_ui_osc_renew_controller_do_request(AgsOscPluginController *osc_plugin_controller,
						AgsOscConnection *osc_connection,
						unsigned char *message, guint message_size);

gpointer ags_ui_osc_renew_controller_set_data_machine(AgsUiOscRenewController *ui_osc_renew_controller,
						      AgsOscConnection *osc_connection,
						      AgsMachine *machine,
						      unsigned char *message, guint message_size,
						      gchar *type_tag,
						      gchar *path, guint path_offset);
gpointer ags_ui_osc_renew_controller_set_data_effect_bridge(AgsUiOscRenewController *ui_osc_renew_controller,
							    AgsOscConnection *osc_connection,
							    AgsEffectBridge *effect_bridge,
							    unsigned char *message, guint message_size,
							    gchar *type_tag,
							    gchar *path, guint path_offset);

gpointer ags_ui_osc_renew_controller_set_data_pad(AgsUiOscRenewController *ui_osc_renew_controller,
						  AgsOscConnection *osc_connection,
						  AgsPad *pad,
						  unsigned char *message, guint message_size,
						  gchar *type_tag,
						  gchar *path, guint path_offset);
gpointer ags_ui_osc_renew_controller_set_data_line(AgsUiOscRenewController *ui_osc_renew_controller,
						   AgsOscConnection *osc_connection,
						   AgsLine *line,
						   unsigned char *message, guint message_size,
						   gchar *type_tag,
						   gchar *path, guint path_offset);

gpointer ags_ui_osc_renew_controller_set_data_effect_pad(AgsUiOscRenewController *ui_osc_renew_controller,
							 AgsOscConnection *osc_connection,
							 AgsEffectPad *effect_pad,
							 unsigned char *message, guint message_size,
							 gchar *type_tag,
							 gchar *path, guint path_offset);
gpointer ags_ui_osc_renew_controller_set_data_effect_line(AgsUiOscRenewController *ui_osc_renew_controller,
							  AgsOscConnection *osc_connection,
							  AgsEffectLine *effect_line,
							  unsigned char *message, guint message_size,
							  gchar *type_tag,
							  gchar *path, guint path_offset);

gpointer ags_ui_osc_renew_controller_set_data_bulk_member(AgsUiOscRenewController *ui_osc_renew_controller,
							  AgsOscConnection *osc_connection,
							  AgsBulkMember *bulk_member,
							  unsigned char *message, guint message_size,
							  gchar *type_tag,
							  gchar *path, guint path_offset);
gpointer ags_ui_osc_renew_controller_set_data_line_member(AgsUiOscRenewController *ui_osc_renew_controller,
							  AgsOscConnection *osc_connection,
							  AgsLineMember *line_member,
							  unsigned char *message, guint message_size,
							  gchar *type_tag,
							  gchar *path, guint path_offset);

gpointer ags_ui_osc_renew_controller_real_set_data(AgsUiOscRenewController *ui_osc_renew_controller,
						   AgsOscConnection *osc_connection,
						   unsigned char *message, guint message_size);

/**
 * SECTION:ags_ui_osc_renew_controller
 * @short_description: UI OSC renew controller
 * @title: AgsUiOscRenewController
 * @section_id:
 * @include: ags/app/osc/controller/ags_ui_osc_renew_controller.h
 *
 * The #AgsUiOscRenewController implements the UI OSC renew controller.
 */

enum{
  PROP_0,
};

enum{
  SET_DATA,
  LAST_SIGNAL,
};

static gpointer ags_ui_osc_renew_controller_parent_class = NULL;
static guint ui_osc_renew_controller_signals[LAST_SIGNAL];

static GMutex regex_mutex;

GType
ags_ui_osc_renew_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ui_osc_renew_controller = 0;

    static const GTypeInfo ags_ui_osc_renew_controller_info = {
      sizeof (AgsUiOscRenewControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ui_osc_renew_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsUiOscRenewController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ui_osc_renew_controller_init,
    };    

    static const GInterfaceInfo ags_osc_plugin_controller_interface_info = {
      (GInterfaceInitFunc) ags_ui_osc_renew_controller_osc_plugin_controller_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_ui_osc_renew_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							      "AgsUiOscRenewController",
							      &ags_ui_osc_renew_controller_info,
							      0);

    g_type_add_interface_static(ags_type_ui_osc_renew_controller,
				AGS_TYPE_OSC_PLUGIN_CONTROLLER,
				&ags_osc_plugin_controller_interface_info);
    
    g_once_init_leave(&g_define_type_id__volatile, ags_type_ui_osc_renew_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_ui_osc_renew_controller_class_init(AgsUiOscRenewControllerClass *ui_osc_renew_controller)
{
  GObjectClass *gobject;
  
  ags_ui_osc_renew_controller_parent_class = g_type_class_peek_parent(ui_osc_renew_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) ui_osc_renew_controller;

  gobject->set_property = ags_ui_osc_renew_controller_set_property;
  gobject->get_property = ags_ui_osc_renew_controller_get_property;

  gobject->dispose = ags_ui_osc_renew_controller_dispose;
  gobject->finalize = ags_ui_osc_renew_controller_finalize;

  /* AgsUiOscRenewControllerClass */
  ui_osc_renew_controller->set_data = ags_ui_osc_renew_controller_real_set_data;

  /* signals */
  /**
   * AgsUiOscRenewController::set-data:
   * @ui_osc_renew_controller: the #AgsUiOscRenewController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::set-data signal is emited during get data of renew controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 3.0.0
   */
  ui_osc_renew_controller_signals[SET_DATA] =
    g_signal_new("set-data",
		 G_TYPE_FROM_CLASS(ui_osc_renew_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsUiOscRenewControllerClass, set_data),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_ui_osc_renew_controller_osc_plugin_controller_interface_init(AgsOscPluginControllerInterface *osc_plugin_controller)
{
  osc_plugin_controller->do_request = ags_ui_osc_renew_controller_do_request;
}

void
ags_ui_osc_renew_controller_init(AgsUiOscRenewController *ui_osc_renew_controller)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();

  g_signal_connect(application_context, "check-message",
		   G_CALLBACK(ags_ui_osc_renew_controller_check_message_callback), ui_osc_renew_controller);

  g_object_set(ui_osc_renew_controller,
	       "context-path", "/ui-renew",
	       NULL);
}

void
ags_ui_osc_renew_controller_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsUiOscRenewController *ui_osc_renew_controller;

  GRecMutex *osc_controller_mutex;

  ui_osc_renew_controller = AGS_UI_OSC_RENEW_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(ui_osc_renew_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ui_osc_renew_controller_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec)
{
  AgsUiOscRenewController *ui_osc_renew_controller;

  GRecMutex *osc_controller_mutex;

  ui_osc_renew_controller = AGS_UI_OSC_RENEW_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(ui_osc_renew_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ui_osc_renew_controller_dispose(GObject *gobject)
{
  AgsUiOscRenewController *ui_osc_renew_controller;

  ui_osc_renew_controller = AGS_UI_OSC_RENEW_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_ui_osc_renew_controller_parent_class)->dispose(gobject);
}

void
ags_ui_osc_renew_controller_finalize(GObject *gobject)
{
  AgsUiOscRenewController *ui_osc_renew_controller;
  
  AgsApplicationContext *application_context;

  ui_osc_renew_controller = AGS_UI_OSC_RENEW_CONTROLLER(gobject);

  application_context = ags_application_context_get_instance();

  g_object_disconnect(application_context,
		      "any_signal::check-message",
		      G_CALLBACK(ags_ui_osc_renew_controller_check_message_callback),
		      ui_osc_renew_controller,
		      NULL);
  
  /* call parent */
  G_OBJECT_CLASS(ags_ui_osc_renew_controller_parent_class)->finalize(gobject);
}

gpointer
ags_ui_osc_renew_controller_do_request(AgsOscPluginController *osc_plugin_controller,
				       AgsOscConnection *osc_connection,
				       unsigned char *message, guint message_size)
{
  gpointer response;
  
  response = ags_ui_osc_renew_controller_set_data(AGS_UI_OSC_RENEW_CONTROLLER(osc_plugin_controller),
						  osc_connection,
						  message, message_size);

  return(response);
}

gpointer
ags_ui_osc_renew_controller_set_data_machine(AgsUiOscRenewController *ui_osc_renew_controller,
					     AgsOscConnection *osc_connection,
					     AgsMachine *machine,
					     unsigned char *message, guint message_size,
					     gchar *type_tag,
					     gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  AgsMessageDelivery *message_delivery;
  AgsMessageQueue *message_queue;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_MACHINE(machine)){
    return(NULL);
  }

  start_response = NULL;

  message_delivery = ags_message_delivery_get_instance();

  message_queue = (AgsMessageQueue *) ags_message_delivery_find_sender_namespace(message_delivery,
										 "libgsequencer");
  
  real_packet_size = 0;
  packet_size = 0;

  if(!strncmp(path + path_offset,
	      "/GtkButton",
	      10)){
    path_offset += 10;

    if(path[path_offset] == '[' &&
       path[path_offset + 1] == '"'){
      gchar *button_specifier;
      gchar *offset;

      guint length;

      if((offset = strchr(path + path_offset + 2, '"')) == NULL){
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);

	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_CHUNK_SIZE_EXCEEDED,
		     NULL);
	  
	return(start_response);
      }

      length = offset - (path + path_offset + 2);

      button_specifier = (gchar *) malloc((length + 1) * sizeof(gchar));
      memcpy(button_specifier, path + path_offset + 2, (length) * sizeof(gchar));
      button_specifier[length] = '\0';
      
      if(!strncmp(button_specifier,
		  "play",
		  5)){
	path_offset += length;
	
	if(!strncmp(path + path_offset,
		    "::",
		    2)){
	  path_offset += 2;
	  
	  if(!strncmp(path + path_offset,
		      "clicked",
		      7)){
	    /* emit message */
	    if(message_queue != NULL){
	      AgsMessageEnvelope *message;

	      xmlDoc *doc;
	      xmlNode *root_node;

	      /* specify message body */
	      doc = xmlNewDoc(BAD_CAST "1.0");

	      root_node = xmlNewNode(NULL,
				     BAD_CAST "ags-command");
	      xmlDocSetRootElement(doc, root_node);    

	      xmlNewProp(root_node,
			 BAD_CAST "method",
			 BAD_CAST "AgsMachine:run::clicked");

	      /* add message */
	      message = ags_message_envelope_new((GObject *) machine,
						 NULL,
						 doc);

	      /* set parameter */
	      message->n_params = 0;

	      message->parameter_name = NULL;
	      message->value = NULL;
    
	      /* add message */
	      ags_message_delivery_add_message_envelope(message_delivery,
							"libgsequencer",
							(GObject *) message);
	    }
	  }
	}
      }
      
      free(button_specifier);
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
    if(AGS_IS_PANEL(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_SPECTROMETER(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_EQUALIZER10(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_MIXER(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_DRUM(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_MATRIX(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_SYNTH(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_FM_SYNTH(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_SYNCSYNTH(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_FM_SYNCSYNTH(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_FFPLAYER(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_PITCH_SAMPLER(machine)){
      //TODO:JK: implement me
    }else if(AGS_IS_AUDIOREC(machine)){
      //TODO:JK: implement me
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
  }
    
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_effect_bridge(AgsUiOscRenewController *ui_osc_renew_controller,
						   AgsOscConnection *osc_connection,
						   AgsEffectBridge *effect_bridge,
						   unsigned char *message, guint message_size,
						   gchar *type_tag,
						   gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_EFFECT_BRIDGE(effect_bridge)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_pad(AgsUiOscRenewController *ui_osc_renew_controller,
					 AgsOscConnection *osc_connection,
					 AgsPad *pad,
					 unsigned char *message, guint message_size,
					 gchar *type_tag,
					 gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_PAD(pad)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_line(AgsUiOscRenewController *ui_osc_renew_controller,
					  AgsOscConnection *osc_connection,
					  AgsLine *line,
					  unsigned char *message, guint message_size,
					  gchar *type_tag,
					  gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_LINE(line)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_effect_pad(AgsUiOscRenewController *ui_osc_renew_controller,
						AgsOscConnection *osc_connection,
						AgsEffectPad *effect_pad,
						unsigned char *message, guint message_size,
						gchar *type_tag,
						gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_EFFECT_PAD(effect_pad)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_effect_line(AgsUiOscRenewController *ui_osc_renew_controller,
						 AgsOscConnection *osc_connection,
						 AgsEffectLine *effect_line,
						 unsigned char *message, guint message_size,
						 gchar *type_tag,
						 gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_EFFECT_LINE(effect_line)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_bulk_member(AgsUiOscRenewController *ui_osc_renew_controller,
						 AgsOscConnection *osc_connection,
						 AgsBulkMember *bulk_member,
						 unsigned char *message, guint message_size,
						 gchar *type_tag,
						 gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_BULK_MEMBER(bulk_member)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_set_data_line_member(AgsUiOscRenewController *ui_osc_renew_controller,
						 AgsOscConnection *osc_connection,
						 AgsLineMember *line_member,
						 unsigned char *message, guint message_size,
						 gchar *type_tag,
						 gchar *path, guint path_offset)
{
  AgsOscResponse *osc_response;

  GList *start_response;

  unsigned char *packet;

  guint type_tag_length;
  guint path_length;
  guint real_packet_size;
  guint packet_size;
  
  if(!AGS_IS_LINE_MEMBER(line_member)){
    return(NULL);
  }

  start_response = NULL;

  real_packet_size = 0;
  packet_size = 0;

  //TODO:JK: implement me
  
  return(start_response);
}

gpointer
ags_ui_osc_renew_controller_real_set_data(AgsUiOscRenewController *ui_osc_renew_controller,
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
	      "/AgsUiProvider",
	      14)){
    AgsMessageDelivery *message_delivery;
    AgsMessageQueue *message_queue;

    unsigned char *duplicated_message;
    
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
      
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_OK);

    /* emit message */
    message_delivery = ags_message_delivery_get_instance();

    message_queue = (AgsMessageQueue *) ags_message_delivery_find_sender_namespace(message_delivery,
										   "libgsequencer");

    if(message_queue != NULL){
      AgsMessageEnvelope *message;

      xmlDoc *doc;
      xmlNode *root_node;

      /* specify message body */
      doc = xmlNewDoc(BAD_CAST "1.0");

      root_node = xmlNewNode(NULL,
			     BAD_CAST "ags-command");
      xmlDocSetRootElement(doc, root_node);    

      xmlNewProp(root_node,
		 BAD_CAST "method",
		 BAD_CAST "AgsUiOscRenewController::set-data");

      /* add message */
      message = ags_message_envelope_new((GObject *) ui_osc_renew_controller,
					 NULL,
					 doc);

      /* set parameter */
      message->n_params = 3;

      message->parameter_name = (gchar **) malloc(4 * sizeof(gchar *));
      message->value = g_new0(GValue,
			      3);

      /* osc-connection */
      message->parameter_name[0] = "osc-connection";
    
      g_value_init(&(message->value[0]),
		   G_TYPE_OBJECT);
      g_value_set_object(&(message->value[0]),
			 osc_connection);

      /* message */
      message->parameter_name[1] = "message";

      duplicated_message = (unsigned char *) malloc(message_size * sizeof(unsigned char));
      memcpy(duplicated_message, message, message_size * sizeof(unsigned char));
      
      g_value_init(&(message->value[1]),
		   G_TYPE_POINTER);
      g_value_set_pointer(&(message->value[1]),
			  duplicated_message);

      /* audio channels */
      message->parameter_name[2] = "message-size";
    
      g_value_init(&(message->value[2]),
		   G_TYPE_UINT);
      g_value_set_uint(&(message->value[2]),
		       message_size);
      
      /* terminate string vector */
      message->parameter_name[3] = NULL;
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
 * ags_ui_osc_renew_controller_set_data:
 * @ui_osc_renew_controller: the #AgsUiOscRenewController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Renew data.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
 * 
 * Since: 3.0.0
 */
gpointer
ags_ui_osc_renew_controller_set_data(AgsUiOscRenewController *ui_osc_renew_controller,
				     AgsOscConnection *osc_connection,
				     unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_UI_OSC_RENEW_CONTROLLER(ui_osc_renew_controller), NULL);
  
  g_object_ref((GObject *) ui_osc_renew_controller);
  g_signal_emit(G_OBJECT(ui_osc_renew_controller),
		ui_osc_renew_controller_signals[SET_DATA], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) ui_osc_renew_controller);

  return(osc_response);
}

void
ags_ui_osc_renew_controller_check_message_callback(GObject *application_context, AgsUiOscRenewController *ui_osc_renew_controller)
{
  AgsChannel *channel;
    
  AgsMessageDelivery *message_delivery;

  GList *start_message_envelope, *message_envelope;
    
  /* retrieve message */
  message_delivery = ags_message_delivery_get_instance();

  message_envelope = 
    start_message_envelope = ags_message_delivery_find_sender(message_delivery,
							      "libgsequencer",
							      (GObject *) ui_osc_renew_controller);
    
  while(message_envelope != NULL){
    xmlNode *root_node;

    root_node = xmlDocGetRootElement(AGS_MESSAGE_ENVELOPE(message_envelope->data)->doc);
      
    if(!xmlStrncmp(root_node->name,
		   BAD_CAST "ags-command",
		   12)){
      if(!xmlStrncmp(xmlGetProp(root_node,
				BAD_CAST "method"),
		     BAD_CAST "AgsUiOscRenewController::set-data",
		     34)){
	AgsOscConnection *osc_connection;
	AgsOscResponse *osc_response;

	AgsApplicationContext *application_context;
	  
	GList *start_response;

	unsigned char *duplicated_message;
	gchar *type_tag;
	gchar *path;
  
	guint message_size;
	guint path_offset;
	gint position;
	gboolean success;

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "osc-connection");
	osc_connection = g_value_get_object(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "message");
	duplicated_message = g_value_get_pointer(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));
	  
	position = ags_strv_index(AGS_MESSAGE_ENVELOPE(message_envelope->data)->parameter_name,
				  "message-size");
	message_size = g_value_get_uint(&(AGS_MESSAGE_ENVELOPE(message_envelope->data)->value[position]));

	start_response = NULL;

	/* read type tag */
	ags_osc_buffer_util_get_string(duplicated_message + 8,
				       &type_tag, NULL);

	success = (type_tag != NULL &&
		   !strncmp(type_tag, ",s", 2)) ? TRUE: FALSE;

	if(!success){
	  goto ags_ui_osc_renew_controller_message_monitor_timeout_NEXT;
	}
  
	/* read argument */
	ags_osc_buffer_util_get_string(duplicated_message + 8 + (4 * (guint) ceil((gdouble) (strlen(type_tag) + 1) / 4.0)),
				       &path, NULL);  

	if(path == NULL){
	  goto ags_ui_osc_renew_controller_message_monitor_timeout_NEXT;
	}

	/* create packet */
	application_context = ags_application_context_get_instance();

	path_offset = 0;
  
	if(!strncmp(path,
		    "/AgsUiProvider",
		    14)){
	  GType machine_type;
    
	  GList *start_machine, *machine;
    
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

	  path_offset = 14;

	  /* compile regex */
	  g_mutex_lock(&regex_mutex);
  
	  if(!regex_compiled){
	    regex_compiled = TRUE;
      
	    ags_regcomp(&single_access_regex, single_access_pattern, REG_EXTENDED);
	    ags_regcomp(&range_access_regex, range_access_pattern, REG_EXTENDED);
	    ags_regcomp(&voluntary_access_regex, voluntary_access_pattern, REG_EXTENDED);
	    ags_regcomp(&more_access_regex, more_access_pattern, REG_EXTENDED);
	    ags_regcomp(&wildcard_access_regex, wildcard_access_pattern, REG_EXTENDED);
	  }

	  g_mutex_unlock(&regex_mutex);

	  machine_type = G_TYPE_NONE;
    
	  if(!strncmp(path + path_offset,
		      "/AgsPanel",
		      9)){    
	    path_offset += 9;

	    machine_type = AGS_TYPE_PANEL;
	  }else if(!strncmp(path + path_offset,
			    "/AgsSpectrometer",
			    16)){
	    path_offset += 16;

	    machine_type = AGS_TYPE_SPECTROMETER;
	  }else if(!strncmp(path + path_offset,
			    "/AgsEqualizer10",
			    15)){
	    path_offset += 15;

	    machine_type = AGS_TYPE_EQUALIZER10;
	  }else if(!strncmp(path + path_offset,
			    "/AgsMixer",
			    9)){
	    path_offset += 9;

	    machine_type = AGS_TYPE_MIXER;
	  }else if(!strncmp(path + path_offset,
			    "/AgsDrum",
			    8)){
	    path_offset += 8;

	    machine_type = AGS_TYPE_DRUM;
	  }else if(!strncmp(path + path_offset,
			    "/AgsMatrix",
			    10)){
	    path_offset += 10;

	    machine_type = AGS_TYPE_MATRIX;
	  }else if(!strncmp(path + path_offset,
			    "/AgsSynth",
			    9)){
	    path_offset += 9;

	    machine_type = AGS_TYPE_SYNTH;
	  }else if(!strncmp(path + path_offset,
			    "/AgsFMSynth",
			    11)){
	    path_offset += 11;

	    machine_type = AGS_TYPE_FM_SYNTH;
	  }else if(!strncmp(path + path_offset,
			    "/AgsSyncsynth",
			    13)){
	    path_offset += 13;

	    machine_type = AGS_TYPE_SYNCSYNTH;
	  }else if(!strncmp(path + path_offset,
			    "/AgsFMSyncsynth",
			    15)){
	    path_offset += 15;

	    machine_type = AGS_TYPE_FM_SYNCSYNTH;
	  }else if(!strncmp(path + path_offset,
			    "/AgsFFPlayer",
			    11)){
	    path_offset += 11;

	    machine_type = AGS_TYPE_FFPLAYER;
	  }else if(!strncmp(path + path_offset,
			    "/AgsPitchSampler",
			    16)){
	    path_offset += 16;

	    machine_type = AGS_TYPE_PITCH_SAMPLER;
	  }else if(!strncmp(path + path_offset,
			    "/AgsAudiorec",
			    12)){
	    path_offset += 12;

	    machine_type = AGS_TYPE_AUDIOREC;
	  }

	  if(machine_type != G_TYPE_NONE){
	    machine = 
	      start_machine = ags_ui_provider_get_machine(AGS_UI_PROVIDER(application_context));
      
	    if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	      AgsMachine *current;
	
	      gchar *endptr;
      
	      guint i;
	      guint i_stop;

	      endptr = NULL;
	      i_stop = g_ascii_strtoull(path + path_offset + 1,
					&endptr,
					10);

	      current = NULL;
	
	      for(i = 0; machine != NULL; ){
		if(G_OBJECT_TYPE(machine->data) == machine_type){
		  i++;

		  if(i > i_stop){
		    current = machine->data;

		    break;
		  }
		}

		machine = machine->next;
	      }

	      g_list_free(start_machine);

	      path_offset += ((endptr + 1) - (path + path_offset));
	
	      start_response = ags_ui_osc_renew_controller_set_data_machine(ui_osc_renew_controller,
									    osc_connection,
									    current,
									    duplicated_message, message_size,
									    type_tag,
									    path, path_offset);
	    }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
	      AgsMachine *current;

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

	      for(i = 0; machine != NULL; ){
		GList *retval;

		if(G_OBJECT_TYPE(machine->data) == machine_type){
		  i++;
	    
		  if(i > i_start){
		    current = machine->data;

		    retval = ags_ui_osc_renew_controller_set_data_machine(ui_osc_renew_controller,
									  osc_connection,
									  current,
									  duplicated_message, message_size,
									  type_tag,
									  path, path_offset);

		    if(start_response != NULL){
		      start_response = g_list_concat(start_response,
						     retval);
		    }else{
		      start_response = retval;
		    }
		  }

		  if(i > i_stop){
		    current = machine->data;

		    break;
		  }
		}

		machine = machine->next;
	      }
	    }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	      path_offset += 3;

	      while(machine != NULL){
		if(G_OBJECT_TYPE(machine->data) == machine_type){
		  start_response = ags_ui_osc_renew_controller_set_data_machine(ui_osc_renew_controller,
										osc_connection,
										start_machine->data,
										duplicated_message, message_size,
										type_tag,
										path, path_offset);
		  break;
		}

		machine = machine->next;
	      }
	    }else if(ags_regexec(&more_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	      path_offset += 3;

	      if(start_machine == NULL){
		goto ags_ui_osc_renew_controller_message_monitor_timeout_NEXT;
	      }
      
	      while(machine != NULL){
		if(G_OBJECT_TYPE(machine->data) == machine_type){
		  GList *retval;
	
		  retval = ags_ui_osc_renew_controller_set_data_machine(ui_osc_renew_controller,
									osc_connection,
									machine->data,
									duplicated_message, message_size,
									type_tag,
									path, path_offset);

		  if(start_response != NULL){
		    start_response = g_list_concat(start_response,
						   retval);
		  }else{
		    start_response = retval;
		  }
		}
	  
		machine = machine->next;
	      }
	    }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	      path_offset += 3;

	      if(start_machine == NULL){
		goto ags_ui_osc_renew_controller_message_monitor_timeout_NEXT;
	      }
      
	      while(machine != NULL){
		if(G_OBJECT_TYPE(machine->data) == machine_type){
		  GList *retval;
	
		  retval = ags_ui_osc_renew_controller_set_data_machine(ui_osc_renew_controller,
									osc_connection,
									machine->data,
									duplicated_message, message_size,
									type_tag,
									path, path_offset);

		  if(start_response != NULL){
		    start_response = g_list_concat(start_response,
						   retval);
		  }else{
		    start_response = retval;
		  }
		}
	  
		machine = machine->next;
	      }
	    }else if(path[path_offset] == '[' &&
		     path[path_offset + 1] == '"'){
	      gchar *machine_name;
	      gchar *offset;

	      guint length;

	      if((offset = strchr(path + path_offset + 2, '"')) == NULL){
		goto ags_ui_osc_renew_controller_message_monitor_timeout_NEXT;
	      }

	      length = offset - (path + path_offset + 2);

	      machine_name = (gchar *) malloc((length + 1) * sizeof(gchar));
	      memcpy(machine_name, path + path_offset + 2, (length) * sizeof(gchar));
	      machine_name[length] = '\0';

	      while(machine != NULL){
		if(G_OBJECT_TYPE(machine->data) == machine_type &&
		   !g_ascii_strcasecmp(AGS_MACHINE(machine->data)->machine_name,
				       machine_name)){
		  GList *retval;
	
		  retval = ags_ui_osc_renew_controller_set_data_machine(ui_osc_renew_controller,
									osc_connection,
									machine->data,
									duplicated_message, message_size,
									type_tag,
									path, path_offset);

		  if(start_response != NULL){
		    start_response = g_list_concat(start_response,
						   retval);
		  }else{
		    start_response = retval;
		  }
		}
	  
		machine = machine->next;
	      }      
	    }	      
	  }
	}else{
	  goto ags_ui_osc_renew_controller_message_monitor_timeout_NEXT;
	}

      ags_ui_osc_renew_controller_message_monitor_timeout_NEXT:      
	if(duplicated_message != NULL){
	  free(duplicated_message);
	}
      }
    }
      
    message_envelope = message_envelope->next;
  }
    
  g_list_free_full(start_message_envelope,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_ui_osc_renew_controller_new:
 * 
 * Instantiate new #AgsUiOscRenewController.
 * 
 * Returns: the #AgsUiOscRenewController
 * 
 * Since: 3.0.0
 */
AgsUiOscRenewController*
ags_ui_osc_renew_controller_new()
{
  AgsUiOscRenewController *ui_osc_renew_controller;

  ui_osc_renew_controller = (AgsUiOscRenewController *) g_object_new(AGS_TYPE_UI_OSC_RENEW_CONTROLLER,
								     NULL);

  return(ui_osc_renew_controller);
}
