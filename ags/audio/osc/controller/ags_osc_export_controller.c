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

#include <ags/audio/osc/controller/ags_osc_export_controller.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio.h>

#include <ags/audio/task/ags_export_output.h>

#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

void ags_osc_export_controller_class_init(AgsOscExportControllerClass *osc_export_controller);
void ags_osc_export_controller_init(AgsOscExportController *osc_export_controller);
void ags_osc_export_controller_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_export_controller_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_export_controller_dispose(GObject *gobject);
void ags_osc_export_controller_finalize(GObject *gobject);

gpointer ags_osc_export_controller_real_do_export(AgsOscExportController *osc_export_controller,
						  AgsOscConnection *osc_connection,
						  unsigned char *message, guint message_size);

/**
 * SECTION:ags_osc_export_controller
 * @short_description: OSC export controller
 * @title: AgsOscExportController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_export_controller.h
 *
 * The #AgsOscExportController implements the OSC export controller.
 */

enum{
  PROP_0,
};

enum{
  DO_EXPORT,
  LAST_SIGNAL,
};

static gpointer ags_osc_export_controller_parent_class = NULL;
static guint osc_export_controller_signals[LAST_SIGNAL];

GType
ags_osc_export_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_export_controller = 0;

    static const GTypeInfo ags_osc_export_controller_info = {
      sizeof (AgsOscExportControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_export_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscExportController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_export_controller_init,
    };
    
    ags_type_osc_export_controller = g_type_register_static(AGS_TYPE_OSC_CONTROLLER,
							    "AgsOscExportController",
							    &ags_osc_export_controller_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_export_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_export_controller_class_init(AgsOscExportControllerClass *osc_export_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_export_controller_parent_class = g_type_class_peek_parent(osc_export_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_export_controller;

  gobject->set_property = ags_osc_export_controller_set_property;
  gobject->get_property = ags_osc_export_controller_get_property;

  gobject->dispose = ags_osc_export_controller_dispose;
  gobject->finalize = ags_osc_export_controller_finalize;

  /* properties */

  /* AgsOscExportControllerClass */
  osc_export_controller->do_export = ags_osc_export_controller_real_do_export;

  /* signals */
  /**
   * AgsOscExportController::do-export:
   * @osc_export_controller: the #AgsOscExportController
   * @osc_connection: the #AgsOscConnection
   * @message: the message received
   * @message_size: the message size
   *
   * The ::do-export signal is emited during get data of export controller.
   *
   * Returns: the #AgsOscResponse
   * 
   * Since: 2.2.0
   */
  osc_export_controller_signals[DO_EXPORT] =
    g_signal_new("do-export",
		 G_TYPE_FROM_CLASS(osc_export_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscExportControllerClass, do_export),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__OBJECT_POINTER_UINT,
		 G_TYPE_POINTER, 3,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
}

void
ags_osc_export_controller_init(AgsOscExportController *osc_export_controller)
{
  g_object_set(osc_export_controller,
	       "context-path", "/export",
	       NULL);
}

void
ags_osc_export_controller_dispose(GObject *gobject)
{
  AgsOscExportController *osc_export_controller;

  osc_export_controller = AGS_OSC_EXPORT_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_export_controller_parent_class)->dispose(gobject);
}

void
ags_osc_export_controller_finalize(GObject *gobject)
{
  AgsOscExportController *osc_export_controller;

  osc_export_controller = AGS_OSC_EXPORT_CONTROLLER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_export_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_export_controller_real_do_export(AgsOscExportController *osc_export_controller,
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
  gchar *filename;
  
  guint format;
  guint64 tic;
  gboolean live_performance;
  guint length;
  guint path_offset;
  gboolean success;

  start_response = NULL;
  
  /* read type tag */
  ags_osc_buffer_util_get_string(message + 8,
				 &type_tag, NULL);

  success = (type_tag != NULL &&
	     !strncmp(type_tag, ",ssihs", 6)) ? TRUE: FALSE;

  if(success){
    success = (!strncmp(type_tag + 6, "T", 2) || strncmp(type_tag + 6, "F", 2)) ? TRUE: FALSE;
  }
  
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

  length = 4 * (guint) ceil((double) (strlen(path) + 1) / 4.0);

  /* read filename */
  ags_osc_buffer_util_get_string(message + 12 + length,
				 &filename, NULL);

  /* check argument */
  if(filename == NULL){
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

  length += 4 * (guint) ceil((double) (strlen(filename) + 1) / 4.0);

  /* read format */
  ags_osc_buffer_util_get_int32(message + 12 + length,
				&format, NULL);

  /* check argument */
  if(format == 0){
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
    free(filename);

    return(start_response);
  }
  
  /* read tic */
  ags_osc_buffer_util_get_int64(message + 16 + length,
				&tic, NULL);

  /* check argument */
  if(tic == 0){
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
    free(filename);

    return(start_response);
  }

  /* read live export */
  live_export = (type_tag[6] == 'T') ? TRUE: FALSE;

  /* get sound provider */
  application_context = ags_application_context_get_instance();

  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  path_offset = 0;

  if(!strncmp(path, "/AgsSoundProvider", 17)){
    path_offset += 17;
      
    if(!strncmp(path + path_offset, "/AgsSoundcard", 13)){
      AgsExportThread *export_thread;

      AgsThread *main_loop;
      
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

      main_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
      
      export_thread = (AgsExportThread *) ags_thread_find_type(main_loop,
							       AGS_TYPE_EXPORT_THREAD);

      soundcard = 
	start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));

      if(ags_regexec(&single_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	AgsExportOutput *export_output;
      
	AgsExportThread *current_export_thread;

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

	current_export_thread = ags_export_thread_find_soundcard(export_thread,
								 current);

	export_output = ags_export_output_new(current_export_thread,
					      current,
					      filename,
					      tic,
					      live_performance);
	
	ags_task_thread_append_task((AgsTaskThread *) task_thread,
				    (AgsTask *) export_output);

	/* create response */
	osc_response = ags_osc_response_new();  
	start_response = g_list_prepend(start_response,
					osc_response);
  
	ags_osc_response_set_flags(osc_response,
				   AGS_OSC_RESPONSE_OK);	
      }else if(ags_regexec(&range_access_regex, path + path_offset, max_matches, match_arr, 0) == 0){
	AgsExportOutput *export_output;
      
	AgsExportThread *current_export_thread;

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
	  current_export_thread = ags_export_thread_find_soundcard(export_thread,
								   soundcard->data);

	  export_output = ags_export_output_new(current_export_thread,
						soundcard->data,
						filename,
						tic,
						live_performance);

	  ags_task_thread_append_task((AgsTaskThread *) task_thread,
				      (AgsTask *) export_output);

	  /* create response */
	  osc_response = ags_osc_response_new();  
	  start_response = g_list_prepend(start_response,
					  osc_response);
  
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);	  

	  soundcard = soundcard->next;
	}
      }else if(ags_regexec(&voluntary_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	AgsExportOutput *export_output;
      
	AgsExportThread *current_export_thread;

	path_offset += 3;

	if(start_soundcard != NULL){
	  current_export_thread = ags_export_thread_find_soundcard(export_thread,
								   start_soundcard->data);

	  export_output = ags_export_output_new(current_export_thread,
						start_soundcard->data,
						filename,
						tic,
						live_performance);

	  ags_task_thread_append_task((AgsTaskThread *) task_thread,
				      (AgsTask *) export_output);

	  /* create response */
	  osc_response = ags_osc_response_new();  
	  start_response = g_list_prepend(start_response,
					  osc_response);
  
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);	  
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
	AgsExportOutput *export_output;
      
	AgsExportThread *current_export_thread;

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
	  current_export_thread = ags_export_thread_find_soundcard(export_thread,
								   soundcard->data);

	  export_output = ags_export_output_new(current_export_thread,
						soundcard->data,
						filename,
						tic,
						live_performance);

	  ags_task_thread_append_task((AgsTaskThread *) task_thread,
				      (AgsTask *) export_output);

	  /* create response */
	  osc_response = ags_osc_response_new();  
	  start_response = g_list_prepend(start_response,
					  osc_response);
  
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);	  

	  soundcard = soundcard->next;
	}
      }else if(ags_regexec(&wildcard_access_regex, path + path_offset, index_max_matches, match_arr, 0) == 0){
	AgsExportOutput *export_output;
      
	AgsExportThread *current_export_thread;

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
	
	  current_export_thread = ags_export_thread_find_soundcard(export_thread,
								   soundcard->data);

	  export_output = ags_export_output_new(current_export_thread,
						soundcard->data,
						filename,
						tic,
						live_performance);

	  ags_task_thread_append_task((AgsTaskThread *) task_thread,
				      (AgsTask *) export_output);

	  /* create response */
	  osc_response = ags_osc_response_new();  
	  start_response = g_list_prepend(start_response,
					  osc_response);
  
	  ags_osc_response_set_flags(osc_response,
				     AGS_OSC_RESPONSE_OK);	  

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
    }
  }
  
  return(start_response);
}

/**
 * ags_osc_export_controller_do_export:
 * @osc_export_controller: the #AgsOscExportController
 * @osc_connection: the #AgsOscConnection
 * @message: the message received
 * @message_size: the message size
 * 
 * Run export.
 * 
 * Returns: the #GList-struct containing #AgsOscResponse
 * 
 * Since: 2.2.0
 */
gpointer
ags_osc_export_controller_do_export(AgsOscExportController *osc_export_controller,
				    AgsOscConnection *osc_connection,
				    unsigned char *message, guint message_size)
{
  gpointer osc_response;
  
  g_return_val_if_fail(AGS_IS_OSC_EXPORT_CONTROLLER(osc_export_controller), NULL);
  
  g_object_ref((GObject *) osc_export_controller);
  g_signal_emit(G_OBJECT(osc_export_controller),
		osc_export_controller_signals[DO_EXPORT], 0,
		osc_connection,
		message, message_size,
		&osc_response);
  g_object_unref((GObject *) osc_export_controller);

  return(osc_response);
}

/**
 * ags_osc_export_controller_new:
 * 
 * Instantiate new #AgsOscExportController
 * 
 * Returns: the #AgsOscExportController
 * 
 * Since: 2.2.0
 */
AgsOscExportController*
ags_osc_export_controller_new()
{
  AgsOscExportController *osc_export_controller;

  osc_export_controller = (AgsOscExportController *) g_object_new(AGS_TYPE_OSC_EXPORT_CONTROLLER,
								  NULL);

  return(osc_export_controller);
}
