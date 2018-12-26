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

#include <ags/audio/osc/controller/ags_osc_action_controller.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>

#include <ags/audio/task/ags_start_audio.h>
#include <ags/audio/task/ags_cancel_audio.h>
#include <ags/audio/task/ags_start_soundcard.h>
#include <ags/audio/task/ags_stop_soundcard.h>
#include <ags/audio/task/ags_start_sequencer.h>
#include <ags/audio/task/ags_stop_sequencer.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

#include <stdlib.h>

void ags_osc_action_controller_class_init(AgsOscActionControllerClass *osc_action_controller);
void ags_osc_action_controller_init(AgsOscActionController *osc_action_controller);
void ags_osc_action_controller_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_action_controller_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_action_controller_dispose(GObject *gobject);
void ags_osc_action_controller_finalize(GObject *gobject);

gpointer ags_osc_action_controller_real_run_action(AgsOscActionController *osc_action_controller,
						   AgsOscConnection *osc_connection,
						   unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_action_controller
 * @short_description: OSC action controller
 * @title: AgsOscActionController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_action_controller.h
 *
 * The #AgsOscActionController implements the OSC action controller.
 */

enum{
  PROP_0,
};

enum{
  RUN_ACTION,
  LAST_SIGNAL,
};

static gpointer ags_osc_action_controller_parent_class = NULL;
static guint osc_action_controller_signals[LAST_SIGNAL];

GType
ags_osc_action_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_action_controller = 0;

    static const GTypeInfo ags_osc_action_controller_info = {
      sizeof (AgsOscActionControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_action_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscActionController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_action_controller_init,
    };
    
    ags_type_osc_action_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							    "AgsOscActionController",
							    &ags_osc_action_controller_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_action_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_action_controller_class_init(AgsOscActionControllerClass *osc_action_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_action_controller_parent_class = g_type_class_peek_parent(osc_action_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_action_controller;

  gobject->set_property = ags_osc_action_controller_set_property;
  gobject->get_property = ags_osc_action_controller_get_property;

  gobject->dispose = ags_osc_action_controller_dispose;
  gobject->finalize = ags_osc_action_controller_finalize;

  /* properties */

  /* AgsOscActionControllerClass */
  osc_action_controller->run_action = ags_osc_action_controller_real_run_action;

  /* signals */
  /**
   * AgsOscActionController::run-action:
   * @osc_action_controller: the #AgsOscActionController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::run-action signal is emited during get data of action controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.1.0
   */
  osc_action_controller_signals[RUN_ACTION] =
    g_signal_new("run-action",
		 G_TYPE_FROM_CLASS(osc_action_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscActionControllerClass, run_action),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_action_controller_init(AgsOscActionController *osc_action_controller)
{
  g_object_set(osc_action_controller,
	       "context-path", "/action",
	       NULL);
}

void
ags_osc_action_controller_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscActionController *osc_action_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_action_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_action_controller_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscActionController *osc_action_controller;

  pthread_mutex_t *osc_controller_mutex;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);

  /* get osc controller mutex */
  pthread_mutex_lock(ags_osc_controller_get_class_mutex());
  
  osc_controller_mutex = AGS_OSC_CONTROLLER(osc_action_controller)->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_controller_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_action_controller_dispose(GObject *gobject)
{
  AgsOscActionController *osc_action_controller;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_action_controller_parent_class)->dispose(gobject);
}

void
ags_osc_action_controller_finalize(GObject *gobject)
{
  AgsOscActionController *osc_action_controller;

  osc_action_controller = AGS_OSC_ACTION_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_action_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_action_controller_real_run_action(AgsOscActionController *osc_action_controller,
					  AgsOscConnection *osc_connection,
					  unsigned char *message, guint message_size)
{
  AgsOscResponse *osc_response;

  AgsThread *task_thread;
  AgsTask *task;
  
  AgsApplicationContext *application_context;

  GList *start_response;
  GList *start_list, *list;
  
  gchar *type_tag;
  gchar *path;
  gchar *action;

  guint length;
  guint path_offset;
  gboolean success;

  start_response = NULL;
  
  /* read type tag */
  ags_osc_buffer_util_get_string(message + 8,
				 &type_tag, NULL);

  success = (type_tag != NULL &&
	     !strncmp(type_tag, ",ss", 4)) ? TRUE: FALSE;

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
  
  /* read path */
  ags_osc_buffer_util_get_string(message + 12,
				 &path, NULL);

  /* check argument */
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

  length = strlen(path);
  
  /* read action */
  ags_osc_buffer_util_get_string(message + 12 + (4 * (guint) ceil((double) (length + 1) / 4.0)),
				 &action, NULL);

  /* check argument */
  if(action == NULL){
    osc_response = ags_osc_response_new();
    start_response = g_list_prepend(start_response,
				    osc_response);
    
    ags_osc_response_set_flags(osc_response,
			       AGS_OSC_RESPONSE_ERROR);

    g_object_set(osc_response,
		 "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MALFORMED_REQUEST,
		 NULL);

    free(type_tag);
    free(path);

    return(start_response);
  }
  
  /* get sound provider */
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  path_offset = 0;

  if(!strncmp(path, "/AgsSoundProvider", 17)){
    path_offset += 17;
      
    if(!strncmp(path + path_offset, "/AgsSoundcard", 13)){
      path_offset += 13;
      
      if(!g_strcmp0("start",
		    action)){
	task = ags_start_soundcard_new(application_context);

	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    task);
      }else if(!g_strcmp0("stop",
			  action)){
	task = ags_stop_soundcard_new(application_context);

	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    task);
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
	free(action);
	
	return(start_response);
      }
    }else if(!strncmp(path + path_offset, "/AgsSequencer", 13)){
      path_offset += 13;

      if(!g_strcmp0("start",
		    action)){
	task = ags_start_sequencer_new(application_context);

	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    task);
      }else if(!g_strcmp0("stop",
			  action)){
	task = ags_stop_sequencer_new(application_context);

	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    task);
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
	free(action);

	return(start_response);
      }
    }else if(!strncmp(path + path_offset, "/AgsAudio", 9)){
      AgsAudio *audio;
      
      guint nth_audio;
      int retval;

      audio = NULL;
      
      path_offset += 9;

      if(g_ascii_isdigit(path[path_offset + 1])){
	start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
	
	retval = sscanf(path + path_offset, "[%d]",
			&nth_audio);
      
	if(retval <= 0){
	  osc_response = ags_osc_response_new();
	  start_response = g_list_prepend(start_response,
					  osc_response);
	  
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_ERROR);
	  
	  g_object_set(osc_response,
		       "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		       NULL);

	  free(type_tag);
	  free(path);
	  free(action);
	  
	  return(start_response);
	}
	
	audio = g_list_nth_data(start_list,
				nth_audio);
      
	path_offset = index(path + path_offset, ']') - path + 1;
	
	g_list_free(start_list);
      }else if(path[path_offset + 1] == '"'){
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

	  free(type_tag);
	  free(path);
	  free(action);
	  
	  return(start_response);
	}

	length = offset - (path + path_offset + 3);

	audio_name = malloc((length + 1) * sizeof(gchar));
	sscanf(path + path_offset, "%ms", &audio_name);

	start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

	list = ags_audio_find_name(start_list,
				   audio_name);

	if(list != NULL){
	  audio = list->data;
	}

	path_offset = path_offset + strlen(audio_name) + 2;
	
	g_list_free(start_list);
      }else{
	osc_response = ags_osc_response_new();
	start_response = g_list_prepend(start_response,
					osc_response);
      
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_ERROR);

	g_object_set(osc_response,
		     "error-message", AGS_OSC_RESPONSE_ERROR_MESSAGE_MISSING_INDEX,
		     NULL);

	free(type_tag);
	free(path);
	free(action);

	return(start_response);
      }

      if(!g_strcmp0("start",
		    action)){
	task = ags_start_audio_new(audio,
				   -1);

	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    task);
      }else if(!g_strcmp0("stop",
			  action)){
	task = ags_cancel_audio_new(audio,
				    -1);

	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    task);
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
	free(action);

	return(start_response);
      }
    }
  }
  
  /* create response */
  osc_response = ags_osc_response_new();  
  start_response = g_list_prepend(start_response,
				  osc_response);
  
  ags_osc_response_set_flags(osc_response,
			     AGS_OSC_RESPONSE_OK);

  free(type_tag);
  free(path);
  free(action);
  
  return(start_response);
}

/**
 * ags_osc_action_controller_run_action:
 * @osc_action_controller: the #AgsOscActionController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Run action.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
 * 
 * Since: 2.1.0
 */
gpointer
ags_osc_action_controller_run_action(AgsOscActionController *osc_action_controller,
				     AgsOscConnection *osc_connection,
				     unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_ACTION_CONTROLLER(osc_action_controller), NULL);
  
  g_object_ref((GObject *) osc_action_controller);
  g_signal_emit(G_OBJECT(osc_action_controller),
		osc_action_controller_signals[RUN_ACTION], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_action_controller);

  return(osc_response);
}

/**
 * ags_osc_action_controller_new:
 * 
 * Instantiate new #AgsOscActionController
 * 
 * Returns: the #AgsOscActionController
 * 
 * Since: 2.1.0
 */
AgsOscActionController*
ags_osc_action_controller_new()
{
  AgsOscActionController *osc_action_controller;

  osc_action_controller = (AgsOscActionController *) g_object_new(AGS_TYPE_OSC_ACTION_CONTROLLER,
								  NULL);

  return(osc_action_controller);
}
