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

#include <ags/X/osc/controller/ags_ui_osc_renew_controller.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/i18n.h>

void ags_ui_osc_renew_controller_class_init(AgsUiOscRenewControllerClass *ui_osc_renew_controller);
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

gpointer ags_ui_osc_renew_controller_real_set_data(AgsUiOscRenewController *ui_osc_renew_controller,
						   AgsOscConnection *osc_connection,
						   unsigned char *message, guint message_size);

/**
 * SECTION:ags_ui_osc_renew_controller
 * @short_description: OSC renew controller
 * @title: AgsUiOscRenewController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_ui_osc_renew_controller.h
 *
 * The #AgsUiOscRenewController implements the OSC renew controller.
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

static pthread_mutex_t regex_mutex = PTHREAD_MUTEX_INITIALIZER;

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
    
    ags_type_ui_osc_renew_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							      "AgsUiOscRenewController",
							      &ags_ui_osc_renew_controller_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ui_osc_renew_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_ui_osc_renew_controller_class_init(AgsUiOscRenewControllerClass *ui_osc_renew_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ui_osc_renew_controller_parent_class = g_type_class_peek_parent(ui_osc_renew_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) ui_osc_renew_controller;

  gobject->set_property = ags_ui_osc_renew_controller_set_property;
  gobject->get_property = ags_ui_osc_renew_controller_get_property;

  gobject->dispose = ags_ui_osc_renew_controller_dispose;
  gobject->finalize = ags_ui_osc_renew_controller_finalize;

  /* properties */

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
   * Since: 2.4.0
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
ags_ui_osc_renew_controller_init(AgsUiOscRenewController *ui_osc_renew_controller)
{
  g_object_set(ui_osc_renew_controller,
	       "context-path", "/renew",
	       NULL);
}

void
ags_ui_osc_renew_controller_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsUiOscRenewController *ui_osc_renew_controller;

  pthread_mutex_t *osc_controller_mutex;

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

  pthread_mutex_t *osc_controller_mutex;

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

  ui_osc_renew_controller = AGS_UI_OSC_RENEW_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_ui_osc_renew_controller_parent_class)->finalize(gobject);
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
    path_offset = 14;

    //TODO:JK: implement me
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
 * Since: 2.4.0
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

/**
 * ags_ui_osc_renew_controller_new:
 * 
 * Instantiate new #AgsUiOscRenewController
 * 
 * Returns: the #AgsUiOscRenewController
 * 
 * Since: 2.4.0
 */
AgsUiOscRenewController*
ags_ui_osc_renew_controller_new()
{
  AgsUiOscRenewController *ui_osc_renew_controller;

  ui_osc_renew_controller = (AgsUiOscRenewController *) g_object_new(AGS_TYPE_UI_OSC_RENEW_CONTROLLER,
								     NULL);

  return(ui_osc_renew_controller);
}
