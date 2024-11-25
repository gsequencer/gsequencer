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

#include <ags/audio/osc/controller/ags_osc_config_controller.h>

#include <ags/audio/task/ags_apply_sound_config.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_osc_config_controller_class_init(AgsOscConfigControllerClass *osc_config_controller);
void ags_osc_config_controller_init(AgsOscConfigController *osc_config_controller);
void ags_osc_config_controller_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_config_controller_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_config_controller_dispose(GObject *gobject);
void ags_osc_config_controller_finalize(GObject *gobject);

gpointer ags_osc_config_controller_real_apply_config(AgsOscConfigController *osc_config_controller,
						     AgsOscConnection *osc_connection,
						     guchar *message, guint message_size);

/**
 * SECTION:ags_osc_config_controller
 * @short_description: OSC config controller
 * @title: AgsOscConfigController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_config_controller.h
 *
 * The #AgsOscConfigController implements the OSC config controller.
 */

enum{
  PROP_0,
};

enum{
  APPLY_CONFIG,
  LAST_SIGNAL,
};

static gpointer ags_osc_config_controller_parent_class = NULL;
static guint osc_config_controller_signals[LAST_SIGNAL];

GType
ags_osc_config_controller_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_config_controller = 0;

    static const GTypeInfo ags_osc_config_controller_info = {
      sizeof (AgsOscConfigControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_config_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscConfigController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_config_controller_init,
    };
    
    ags_type_osc_config_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							    "AgsOscConfigController",
							    &ags_osc_config_controller_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_config_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_config_controller_class_init(AgsOscConfigControllerClass *osc_config_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_config_controller_parent_class = g_type_class_peek_parent(osc_config_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_config_controller;

  gobject->set_property = ags_osc_config_controller_set_property;
  gobject->get_property = ags_osc_config_controller_get_property;

  gobject->dispose = ags_osc_config_controller_dispose;
  gobject->finalize = ags_osc_config_controller_finalize;

  /* properties */

  /* AgsOscConfigControllerClass */
  osc_config_controller->apply_config = ags_osc_config_controller_real_apply_config;

  /* signals */
  /**
   * AgsOscConfigController::apply-config:
   * @osc_config_controller: the #AgsOscConfigController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::apply-config signal is emited during get data of config controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 3.0.0
   */
  osc_config_controller_signals[APPLY_CONFIG] =
    g_signal_new("apply-config",
		 G_TYPE_FROM_CLASS(osc_config_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscConfigControllerClass, apply_config),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_config_controller_init(AgsOscConfigController *osc_config_controller)
{
  g_object_set(osc_config_controller,
	       "context-path", "/config",
	       NULL);
}

void
ags_osc_config_controller_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscConfigController *osc_config_controller;

  GRecMutex *osc_controller_mutex;

  osc_config_controller = AGS_OSC_CONFIG_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_config_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_config_controller_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscConfigController *osc_config_controller;

  GRecMutex *osc_controller_mutex;

  osc_config_controller = AGS_OSC_CONFIG_CONTROLLER(gobject);

  /* get osc controller mutex */
  osc_controller_mutex = AGS_OSC_CONTROLLER_GET_OBJ_MUTEX(osc_config_controller);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_config_controller_dispose(GObject *gobject)
{
  AgsOscConfigController *osc_config_controller;

  osc_config_controller = AGS_OSC_CONFIG_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_config_controller_parent_class)->dispose(gobject);
}

void
ags_osc_config_controller_finalize(GObject *gobject)
{
  AgsOscConfigController *osc_config_controller;

  osc_config_controller = AGS_OSC_CONFIG_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_config_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_config_controller_real_apply_config(AgsOscConfigController *osc_config_controller,
					    AgsOscConnection *osc_connection,
					    guchar *message, guint message_size)
{
  AgsOscResponse *osc_response;

  AgsApplySoundConfig *apply_sound_config;
  
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;

  AgsOscBufferUtil osc_buffer_util;

  GList *start_response;

  gchar *type_tag;
  gchar *config_data;

  gboolean success;

  start_response = NULL;

  osc_buffer_util.major = 1;
  osc_buffer_util.minor = 0;
  
  /* read type tag */
  ags_osc_buffer_util_get_string(&osc_buffer_util,
				 message + 8,
				 &type_tag, NULL);

  success = (type_tag != NULL &&
	     !strncmp(type_tag, ",s", 3)) ? TRUE: FALSE;

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
  
  /* read config data */
  ags_osc_buffer_util_get_string(&osc_buffer_util,
				 message + 12,
				 &config_data, NULL);

  if(config_data == NULL){
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

  /* get sound provider */
  application_context = ags_application_context_get_instance();

  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  apply_sound_config = ags_apply_sound_config_new(config_data);

  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) apply_sound_config);

  /* create response */
  osc_response = ags_osc_response_new();  
  start_response = g_list_prepend(start_response,
				  osc_response);

  ags_osc_response_set_flags(osc_response,
			     AGS_OSC_RESPONSE_OK);

  free(type_tag);
  free(config_data);

  g_object_unref(task_launcher);
  
  return(start_response);
}

/**
 * ags_osc_config_controller_apply_config:
 * @osc_config_controller: the #AgsOscConfigController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Apply config.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
 * 
 * Since: 3.0.0
 */
gpointer
ags_osc_config_controller_apply_config(AgsOscConfigController *osc_config_controller,
				       AgsOscConnection *osc_connection,
				       guchar *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_CONFIG_CONTROLLER(osc_config_controller), NULL);
  
  g_object_ref((GObject *) osc_config_controller);
  g_signal_emit(G_OBJECT(osc_config_controller),
		osc_config_controller_signals[APPLY_CONFIG], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_config_controller);

  return(osc_response);
}

/**
 * ags_osc_config_controller_new:
 * 
 * Instantiate new #AgsOscConfigController
 * 
 * Returns: the #AgsOscConfigController
 * 
 * Since: 3.0.0
 */
AgsOscConfigController*
ags_osc_config_controller_new()
{
  AgsOscConfigController *osc_config_controller;

  osc_config_controller = (AgsOscConfigController *) g_object_new(AGS_TYPE_OSC_CONFIG_CONTROLLER,
								  NULL);

  return(osc_config_controller);
}
