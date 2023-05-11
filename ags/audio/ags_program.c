/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/ags_program.h>

#include <ags/i18n.h>

#include <errno.h>

void ags_program_class_init(AgsProgramClass *program);
void ags_program_init(AgsProgram *program);
void ags_program_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_program_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_program_dispose(GObject *gobject);
void ags_program_finalize(GObject *gobject);

void ags_program_insert_native_level_from_clipboard(AgsProgram *program,
						    xmlNode *root_node, char *version,
						    char *x_boundary,
						    gboolean reset_x_offset, guint x_offset,
						    gboolean do_replace);

/**
 * SECTION:ags_program
 * @short_description: Program class supporting selection and clipboard
 * @title: AgsProgram
 * @section_id:
 * @include: ags/audio/ags_program.h
 *
 * #AgsProgram acts as a container of #AgsMarker. The `timestamp` property tells the
 * engine what the first x offset of #AgsMarker applies.
 *
 * You can lookup #AgsMarker by start x offet with ags_program_find_near_timestamp().
 * The next x offset is calculated as following:
 *
 * next_x_offset = x_offset + relative_offset;
 *
 * The relative_offset is calculated as following:
 *
 * relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;
 *
 * Use ags_program_add_marker() to add #AgsMarker to #AgsProgram and
 * ags_program_remove_marker() to remove it again.
 *
 * In order to copy or cut markers you select them first by calling ags_program_add_region_to_selection().  
 */

enum{
  PROP_0,
  PROP_TIMESTAMP,
  PROP_CONTROL_NAME,
  PROP_MARKER,
};

static gpointer ags_program_parent_class = NULL;

GType
ags_program_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_program = 0;

    static const GTypeInfo ags_program_info = {
      sizeof(AgsProgramClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_program_class_init,
      NULL,
      NULL,
      sizeof(AgsProgram),
      0,
      (GInstanceInitFunc) ags_program_init,
    };

    ags_type_program = g_type_register_static(G_TYPE_OBJECT,
					      "AgsProgram",
					      &ags_program_info,
					      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_program);
  }

  return g_define_type_id__volatile;
}

GType
ags_program_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_PROGRAM_BYPASS, "AGS_PROGRAM_BYPASS", "program-bypass" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsProgramFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void 
ags_program_class_init(AgsProgramClass *program)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_program_parent_class = g_type_class_peek_parent(program);

  gobject = (GObjectClass *) program;

  gobject->set_property = ags_program_set_property;
  gobject->get_property = ags_program_get_property;

  gobject->dispose = ags_program_dispose;
  gobject->finalize = ags_program_finalize;

  /* properties */
  /**
   * AgsProgram:timestamp:
   *
   * The program's timestamp.
   * 
   * Since: 5.1.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp of program"),
				   i18n_pspec("The timestamp of program"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);

  /**
   * AgsProgram:control-name:
   *
   * The program's control-name.
   * 
   * Since: 5.1.0
   */
  param_spec = g_param_spec_string("control-name",
				   i18n_pspec("control-name of program"),
				   i18n_pspec("The control name of program"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_NAME,
				  param_spec);
  
  /**
   * AgsProgram:marker: (type GList(AgsMarker)) (transfer full)
   *
   * The assigned #AgsMarker
   * 
   * Since: 5.1.0
   */
  param_spec = g_param_spec_pointer("marker",
				    i18n_pspec("marker of program"),
				    i18n_pspec("The marker of program"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARKER,
				  param_spec);
}

void
ags_program_init(AgsProgram *program)
{
  AgsConfig *config;

  program->flags = 0;

  /* program mutex */
  g_rec_mutex_init(&(program->obj_mutex));

  /* config */
  config = ags_config_get_instance();

  /* fields */    
  program->timestamp = ags_timestamp_new();

  program->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  program->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  program->timestamp->timer.ags_offset.offset = 0;

  g_object_ref(program->timestamp);

  program->control_name = NULL;

  program->port = NULL;
  
  program->marker = NULL;
  program->selection = NULL;
}

void
ags_program_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsProgram *program;

  GRecMutex *program_mutex;

  program = AGS_PROGRAM(gobject);

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  switch(prop_id){
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      g_rec_mutex_lock(program_mutex);

      if(timestamp == program->timestamp){
	g_rec_mutex_unlock(program_mutex);
	
	return;
      }

      if(program->timestamp != NULL){
	g_object_unref(G_OBJECT(program->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      program->timestamp = timestamp;

      g_rec_mutex_unlock(program_mutex);
    }
    break;
  case PROP_CONTROL_NAME:
    {
      gchar *control_name;

      control_name = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(program_mutex);

      if(control_name == program->control_name){
	g_rec_mutex_unlock(program_mutex);
	
	return;
      }

      g_free(program->control_name);

      program->control_name = g_strdup(control_name);

      g_rec_mutex_unlock(program_mutex);
    }
    break;
  case PROP_MARKER:
    {
      AgsMarker *marker;

      marker = (AgsMarker *) g_value_get_pointer(value);

      g_rec_mutex_lock(program_mutex);

      if(marker == NULL ||
	 g_list_find(program->marker, marker) != NULL){
	g_rec_mutex_unlock(program_mutex);

	return;
      }

      g_rec_mutex_unlock(program_mutex);

      ags_program_add_marker(program,
			     marker,
			     FALSE);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_program_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsProgram *program;

  GRecMutex *program_mutex;

  program = AGS_PROGRAM(gobject);

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  switch(prop_id){
  case PROP_TIMESTAMP:
    {
      g_rec_mutex_lock(program_mutex);

      g_value_set_object(value, program->timestamp);

      g_rec_mutex_unlock(program_mutex);
    }
    break;
  case PROP_CONTROL_NAME:
    {
      g_rec_mutex_lock(program_mutex);

      g_value_set_string(value, program->control_name);

      g_rec_mutex_unlock(program_mutex);
    }
    break;
  case PROP_MARKER:
    {
      g_rec_mutex_lock(program_mutex);

      g_value_set_pointer(value, g_list_copy_deep(program->marker,
						  (GCopyFunc) g_object_ref,
						  NULL));

      g_rec_mutex_unlock(program_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_program_dispose(GObject *gobject)
{
  AgsProgram *program;

  GList *list;
  
  program = AGS_PROGRAM(gobject);

  /* timestamp */
  if(program->timestamp != NULL){
    g_object_unref(program->timestamp);

    program->timestamp = NULL;
  }

  /* marker and selection */
  list = program->marker;

  while(list != NULL){
    g_object_run_dispose(G_OBJECT(list->data));
    
    list = list->next;
  }
  
  g_list_free_full(program->marker,
		   g_object_unref);
  g_list_free_full(program->selection,
		   g_object_unref);

  program->marker = NULL;
  program->selection = NULL;
    
  /* call parent */
  G_OBJECT_CLASS(ags_program_parent_class)->dispose(gobject);
}

void
ags_program_finalize(GObject *gobject)
{
  AgsProgram *program;

  program = AGS_PROGRAM(gobject);

  /* timestamp */
  if(program->timestamp != NULL){
    g_object_unref(program->timestamp);
  }
    
  /* marker and selection */
  g_list_free_full(program->marker,
		   g_object_unref);

  g_list_free_full(program->selection,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_program_parent_class)->finalize(gobject);
}

/**
 * ags_program_get_obj_mutex:
 * @program: the #AgsProgram
 * 
 * Get object mutex.
 * 
 * Returns: the #GRecMutex to lock @program
 * 
 * Since: 3.1.0
 */
GRecMutex*
ags_program_get_obj_mutex(AgsProgram *program)
{
  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  return(AGS_PROGRAM_GET_OBJ_MUTEX(program));
}

/**
 * ags_program_test_flags:
 * @program: the #AgsProgram
 * @flags: the flags
 * 
 * Test @flags to be set on @program.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 5.1.0
 */
gboolean
ags_program_test_flags(AgsProgram *program, AgsProgramFlags flags)
{
  gboolean retval;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(FALSE);
  }
      
  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* test */
  g_rec_mutex_lock(program_mutex);

  retval = (flags & (program->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(program_mutex);

  return(retval);
}

/**
 * ags_program_set_flags:
 * @program: the #AgsProgram
 * @flags: the flags
 * 
 * Set @flags on @program.
 * 
 * Since: 5.1.0
 */
void
ags_program_set_flags(AgsProgram *program, AgsProgramFlags flags)
{
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }
      
  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* set */
  g_rec_mutex_lock(program_mutex);

  program->flags |= flags;
  
  g_rec_mutex_unlock(program_mutex);
}

/**
 * ags_program_unset_flags:
 * @program: the #AgsProgram
 * @flags: the flags
 * 
 * Unset @flags on @program.
 * 
 * Since: 5.1.0
 */
void
ags_program_unset_flags(AgsProgram *program, AgsProgramFlags flags)
{
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }
      
  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* set */
  g_rec_mutex_lock(program_mutex);

  program->flags &= (~flags);
  
  g_rec_mutex_unlock(program_mutex);
}

/**
 * ags_program_find_near_timestamp:
 * @program: (element-type AgsAudio.Program) (transfer none): the #GList-struct containing #AgsProgram
 * @timestamp: the matching #AgsTimestamp, or %NULL to match any timestamp
 *
 * Retrieve appropriate program for timestamp.
 *
 * Returns: (element-type AgsAudio.Program) (transfer none): Next matching #GList-struct or %NULL if not found
 *
 * Since: 5.1.0
 */
GList*
ags_program_find_near_timestamp(GList *program,
				AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  guint current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(program == NULL){
    return(NULL);
  }

  current_start = program;
  current_end = g_list_last(program);
  
  length = g_list_length(program);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);

  if(ags_timestamp_test_flags(timestamp,
			      AGS_TIMESTAMP_OFFSET)){
    x = ags_timestamp_get_ags_offset(timestamp);

    use_ags_offset = TRUE;
  }else if(ags_timestamp_test_flags(timestamp,
				    AGS_TIMESTAMP_UNIX)){
    x = ags_timestamp_get_unix_time(timestamp);

    use_ags_offset = FALSE;
  }else{
    return(NULL);
  }

  current_x = 0;
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    guint relative_offset;

    /* check current - start */
    if(timestamp == NULL){
      retval = current_start;
	
      break;
    }

    g_object_get(current_start->data,
		 "timestamp", &current_timestamp,
		 NULL);

    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;

    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	g_object_unref(current_timestamp);

	if(current_x > x + relative_offset){
	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	g_object_unref(current_timestamp);

	if(current_x > x){
	  break;
	}
      }

      if(use_ags_offset){
	if(current_x >= x &&
	   current_x < x + relative_offset){
	  retval = current_start;
	    
	  break;
	}
      }else{
	if(current_x >= x &&
	   current_x < x + AGS_PROGRAM_DEFAULT_DURATION){
	  retval = current_start;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }

    /* check current - end */
    if(timestamp == NULL){
      retval = current_end;
	
      break;
    }

    g_object_get(current_end->data,
		 "timestamp", &current_timestamp,
		 NULL);

    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;
	  
    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);

	g_object_unref(current_timestamp);

	if(current_x < x){
	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	g_object_unref(current_timestamp);

	if(current_x < x){
	  break;
	}
      }

      if(use_ags_offset){
	if(current_x >= x &&
	   current_x < x + relative_offset){
	  retval = current_end;

	  break;
	}
      }else{
	if(current_x >= x &&
	   current_x < x + AGS_PROGRAM_DEFAULT_DURATION){
	  retval = current_end;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }

    /* check current - center */
    if(timestamp == NULL){
      retval = current;
	
      break;
    }
    
    g_object_get(current->data,
		 "timestamp", &current_timestamp,
		 NULL);

    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;

    if(current_timestamp != NULL){
      if(use_ags_offset){
	current_x = ags_timestamp_get_ags_offset(current_timestamp);

	g_object_unref(current_timestamp);
    
	if(current_x >= x &&
	   current_x < x + relative_offset){
	  retval = current;

	  break;
	}
      }else{
	current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	g_object_unref(current_timestamp);

	if(current_x >= x &&
	   current_x < x + AGS_PROGRAM_DEFAULT_DURATION){
	  retval = current;
	    
	  break;
	}
      }
    }else{
      g_warning("inconsistent data");
    }
    
    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

GList*
ags_program_find_near_timestamp_extended(GList *program,
					 gchar *control_name,
					 AgsTimestamp *timestamp)
{
  AgsTimestamp *current_timestamp;

  GList *retval;
  GList *current_start, *current_end, *current;

  guint current_x, x;
  guint length, position;
  gboolean use_ags_offset;
  gboolean success;

  if(program == NULL){
    return(NULL);
  }

  current_start = program;
  current_end = g_list_last(program);
  
  length = g_list_length(program);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);

  if(ags_timestamp_test_flags(timestamp,
			      AGS_TIMESTAMP_OFFSET)){
    x = ags_timestamp_get_ags_offset(timestamp);

    use_ags_offset = TRUE;
  }else if(ags_timestamp_test_flags(timestamp,
				    AGS_TIMESTAMP_UNIX)){
    x = ags_timestamp_get_unix_time(timestamp);

    use_ags_offset = FALSE;
  }else{
    return(NULL);
  }

  current_x = 0;
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    gchar *current_control_name;
    
    guint relative_offset;

    /* check current - start */
    if(timestamp == NULL){
      retval = current_start;
	
      break;
    }

    current_timestamp = NULL;
    current_control_name = NULL;

    g_object_get(current_start->data,
		 "timestamp", &current_timestamp,
		 "control-name", &current_control_name,
		 NULL);

    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;

    if(!g_strcmp0(control_name,
		  current_control_name)){
      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x > x + relative_offset){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x > x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + relative_offset){
	    retval = current_start;
	    
	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_PROGRAM_DEFAULT_DURATION){
	    retval = current_start;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }

    /* check current - end */
    if(timestamp == NULL){
      retval = current_end;
	
      break;
    }

    current_timestamp = NULL;
    current_control_name = NULL;
    
    g_object_get(current_end->data,
		 "timestamp", &current_timestamp,
		 "control-name", &current_control_name,
		 NULL);

    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;
	  
    if(!g_strcmp0(control_name,
		  current_control_name)){
      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);

	  g_object_unref(current_timestamp);

	  if(current_x < x){
	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x < x){
	    break;
	  }
	}

	if(use_ags_offset){
	  if(current_x >= x &&
	     current_x < x + relative_offset){
	    retval = current_end;

	    break;
	  }
	}else{
	  if(current_x >= x &&
	     current_x < x + AGS_PROGRAM_DEFAULT_DURATION){
	    retval = current_end;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }
    
    /* check current - center */
    if(timestamp == NULL){
      retval = current;
	
      break;
    }

    current_timestamp = NULL;
    current_control_name = NULL;
    
    g_object_get(current->data,
		 "timestamp", &current_timestamp,
		 "control-name", &current_control_name,
		 NULL);

    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;

    if(!g_strcmp0(control_name,
		  current_control_name)){
      if(current_timestamp != NULL){
	if(use_ags_offset){
	  current_x = ags_timestamp_get_ags_offset(current_timestamp);

	  g_object_unref(current_timestamp);
    
	  if(current_x >= x &&
	     current_x < x + relative_offset){
	    retval = current;

	    break;
	  }
	}else{
	  current_x = ags_timestamp_get_unix_time(current_timestamp);
	  
	  g_object_unref(current_timestamp);

	  if(current_x >= x &&
	     current_x < x + AGS_PROGRAM_DEFAULT_DURATION){
	    retval = current;
	    
	    break;
	  }
	}
      }else{
	g_warning("inconsistent data");
      }
    }    

    /* iterate */
    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }else{
      current_start = current_start->next;
      //NOTE:JK: we want progression
      //current_end = current_end->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  return(retval);
}

/**
 * ags_program_sort_func:
 * @a: the #AgsProgram
 * @b: another #AgsProgram
 * 
 * Compare @a and @b.
 * 
 * Returns: 0 if equal, -1 if smaller and 1 if bigger timestamp
 * 
 * Since: 5.1.0
 */
gint
ags_program_sort_func(gconstpointer a,
		      gconstpointer b)
{
  AgsTimestamp *timestamp_a, *timestamp_b;

  guint offset_a, offset_b;

  g_object_get(a,
	       "timestamp", &timestamp_a,
	       NULL);

  g_object_get(b,
	       "timestamp", &timestamp_b,
	       NULL);
    
  offset_a = ags_timestamp_get_ags_offset(timestamp_a);
  offset_b = ags_timestamp_get_ags_offset(timestamp_b);

  g_object_unref(timestamp_a);
  g_object_unref(timestamp_b);
    
  if(offset_a == offset_b){
    return(0);
  }else if(offset_a < offset_b){
    return(-1);
  }else if(offset_a > offset_b){
    return(1);
  }

  return(0);
}

/**
 * ags_program_get_timestamp:
 * @program: the #AgsProgram
 * 
 * Get timestamp.
 * 
 * Returns: (transfer full): the #AgsTimestamp
 * 
 * Since: 5.1.0
 */
AgsTimestamp*
ags_program_get_timestamp(AgsProgram *program)
{
  AgsTimestamp *timestamp;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  g_object_get(program,
	       "timestamp", &timestamp,
	       NULL);

  return(timestamp);
}

/**
 * ags_program_set_timestamp:
 * @program: the #AgsProgram
 * @timestamp: the #AgsTimestamp
 * 
 * Set timestamp.
 * 
 * Since: 5.1.0
 */
void
ags_program_set_timestamp(AgsProgram *program, AgsTimestamp *timestamp)
{
  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  g_object_set(program,
	       "timestamp", timestamp,
	       NULL);
}

/**
 * ags_program_get_marker:
 * @program: the #AgsProgram
 * 
 * Get marker.
 * 
 * Returns: (element-type AgsAudio.Marker) (transfer full): the #GList-struct containig #AgsMarker
 * 
 * Since: 5.1.0
 */
GList*
ags_program_get_marker(AgsProgram *program)
{
  GList *marker;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  g_object_get(program,
	       "marker", &marker,
	       NULL);

  return(marker);
}

/**
 * ags_program_set_marker:
 * @program: the #AgsProgram
 * @marker: (element-type AgsAudio.Marker) (transfer full): the #GList-struct containing #AgsMarker
 * 
 * Set marker by replacing existing.
 * 
 * Since: 5.1.0
 */
void
ags_program_set_marker(AgsProgram *program, GList *marker)
{
  GList *start_marker;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);
    
  g_rec_mutex_lock(program_mutex);

  start_marker = program->marker;
  program->marker = marker;
  
  g_rec_mutex_unlock(program_mutex);

  g_list_free_full(start_marker,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_program_add:
 * @program: (element-type AgsAudio.Program) (transfer none): the #GList-struct containing #AgsProgram
 * @new_program: the #AgsProgram to add
 * 
 * Add @new_program sorted to @program
 * 
 * Returns: (element-type AgsAudio.Program) (transfer none): the new beginning of @program
 * 
 * Since: 5.1.0
 */
GList*
ags_program_add(GList *program,
		AgsProgram *new_program)
{
  
  if(!AGS_IS_PROGRAM(new_program)){
    return(program);
  }
  
  program = g_list_insert_sorted(program,
				 new_program,
				 ags_program_sort_func);
  
  return(program);
}

/**
 * ags_program_add_marker:
 * @program: the #AgsProgram
 * @marker: the #AgsMarker to add
 * @use_selection_list: if %TRUE add to selection, else to default program
 *
 * Add @marker to @program.
 *
 * Since: 5.1.0
 */
void
ags_program_add_marker(AgsProgram *program,
		       AgsMarker *marker,
		       gboolean use_selection_list)
{
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program) ||
     !AGS_IS_MARKER(marker)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* insert sorted */
  g_rec_mutex_lock(program_mutex);

  if(use_selection_list){
    if(g_list_find(program->selection, marker) == NULL){
      g_object_ref(marker);
  
      program->selection = g_list_insert_sorted(program->selection,
						marker,
						(GCompareFunc) ags_marker_sort_func);
      ags_marker_set_flags(marker,
			   AGS_MARKER_IS_SELECTED);
    }
  }else{
    if(g_list_find(program->marker, marker) == NULL){
      g_object_ref(marker);
  
      program->marker = g_list_insert_sorted(program->marker,
					     marker,
					     (GCompareFunc) ags_marker_sort_func);
    }
  }
  
  g_rec_mutex_unlock(program_mutex);
}

/**
 * ags_program_remove_marker:
 * @program: the #AgsProgram
 * @marker: the #AgsMarker to remove
 * @use_selection_list: if %TRUE remove from selection, else from default program
 *
 * Removes @marker from @program.
 *
 * Since: 5.1.0
 */
void
ags_program_remove_marker(AgsProgram *program,
			  AgsMarker *marker,
			  gboolean use_selection_list)
{
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program) ||
     !AGS_IS_MARKER(marker)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* remove if found */
  g_rec_mutex_lock(program_mutex);
  
  if(!use_selection_list){
    if(g_list_find(program->marker,
		   marker) != NULL){
      program->marker = g_list_remove(program->marker,
				      marker);
      g_object_unref(marker);
    }
  }else{
    if(g_list_find(program->selection,
		   marker) != NULL){
      program->selection = g_list_remove(program->selection,
					 marker);
      g_object_unref(marker);
    }
  }

  g_rec_mutex_unlock(program_mutex);
}

/**
 * ags_program_remove_marker_at_position:
 * @program: the #AgsProgram
 * @x: x offset
 *
 * Removes one #AgsMarker of program.
 *
 * Returns: %TRUE if successfully removed marker, else %FALSE
 *
 * Since: 5.1.0
 */
gboolean
ags_program_remove_marker_at_position(AgsProgram *program,
				      guint x)
{
  AgsMarker *marker;
  
  GList *start_list, *list;

  guint current_x;
  gboolean retval;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(FALSE);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* find marker */
  g_rec_mutex_lock(program_mutex);
  
  list =
    start_list = g_list_copy_deep(program->marker,
				  (GCopyFunc) g_object_ref,
				  NULL);
  
  g_rec_mutex_unlock(program_mutex);

  marker = NULL;

  retval = FALSE;
  
  while(list != NULL){
    g_object_get(list->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x == x){
      marker = list->data;
      
      retval = TRUE;
      
      break;
    }

    if(current_x > x){
      break;
    }
    
    list = list->next;
  }

  /* delete link and unref */
  if(retval){
    g_rec_mutex_lock(program_mutex);
    
    program->marker = g_list_remove(program->marker,
				    marker);
    g_object_unref(marker);
  
    g_rec_mutex_unlock(program_mutex);
  }

  g_list_free_full(start_list,
		   g_object_unref);

  return(retval);
}

/**
 * ags_program_get_selection:
 * @program: the #AgsProgram
 *
 * Retrieve selection.
 *
 * Returns: (element-type AgsAudio.Marker) (transfer none): the selection.
 *
 * Since: 5.1.0
 */
GList*
ags_program_get_selection(AgsProgram *program)
{
  GList *selection;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* selection */
  g_rec_mutex_lock(program_mutex);

  selection = program->selection;
  
  g_rec_mutex_unlock(program_mutex);
  
  return(selection);
}

/**
 * ags_program_is_marker_selected:
 * @program: the #AgsProgram
 * @marker: the #AgsMarker to check for
 *
 * Check selection for marker.
 *
 * Returns: %TRUE if selected otherwise %FALSE
 *
 * Since: 5.1.0
 */
gboolean
ags_program_is_marker_selected(AgsProgram *program, AgsMarker *marker)
{
  GList *selection;

  guint x;
  guint current_x;
  gboolean retval;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program) ||
     !AGS_IS_MARKER(marker)){
    return(FALSE);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* get x */
  g_object_get(marker,
	       "x", &x,
	       NULL);
  
  /* match marker */
  g_rec_mutex_lock(program_mutex);

  selection = program->selection;
  retval = FALSE;
  
  while(selection != NULL){
    /* get current x */
    g_object_get(selection->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x){
      break;
    }
    
    if(selection->data == marker){
      retval = TRUE;

      break;
    }

    selection = selection->next;
  }

  g_rec_mutex_unlock(program_mutex);

  return(retval);
}

/**
 * ags_program_find_point:
 * @program: the #AgsProgram
 * @x: offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find markers by offset.
 *
 * Returns: (transfer none): the matching marker as #AgsMarker.
 *
 * Since: 5.1.0
 */
AgsMarker*
ags_program_find_point(AgsProgram *program,
		       guint x,
		       gboolean use_selection_list)
{
  AgsMarker *retval;
  
  GList *marker;
  GList *current_start, *current_end, *current;

  guint current_start_x, current_end_x, current_x;
  guint length, position;
  gboolean success;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* find marker */
  g_rec_mutex_lock(program_mutex);
  
  if(use_selection_list){
    marker = program->selection;
  }else{
    marker = program->marker;
  }
  
  current_start = marker;
  current_end = g_list_last(marker);
  
  length = g_list_length(marker);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    g_object_get(current_start->data,
		 "x", &current_start_x,
		 NULL);

    if(current_start_x > x){
      break;
    }
    
    if(current_start_x == x){
      retval = current_start->data;

      break;
    }
    
    g_object_get(current_end->data,
		 "x", &current_end_x,
		 NULL);

    if(current_end_x < x){
      break;
    }

    
    if(current_end_x == x){
      retval = current_end->data;

      break;
    }

    g_object_get(current->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x == x){
      retval = current->data;
      
      break;
    }

    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  g_rec_mutex_unlock(program_mutex);

  return(retval);
}

/**
 * ags_program_find_point:
 * @program: the #AgsProgram
 * @x: offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find markers by offset.
 *
 * Returns: (transfer none): the matching marker as #AgsMarker.
 *
 * Since: 3.14.6
 */
AgsMarker*
ags_program_find_exact_point(AgsProgram *program,
			     guint x,
			     gboolean use_selection_list)
{
  AgsMarker *retval;
  
  GList *marker;
  GList *current_start, *current_end, *current;

  guint current_start_x, current_end_x, current_x;
  guint length, position;
  gboolean success;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* find marker */
  g_rec_mutex_lock(program_mutex);

  if(use_selection_list){
    marker = program->selection;
  }else{
    marker = program->marker;
  }
  
  current_start = marker;
  current_end = g_list_last(marker);
  
  length = g_list_length(marker);
  position = (length - 1) / 2;

  current = g_list_nth(current_start,
		       position);
  
  retval = NULL;
  success = FALSE;
  
  while(!success && current != NULL){
    g_object_get(current_start->data,
		 "x", &current_start_x,
		 NULL);

    if(current_start_x > x){
      break;
    }
    
    if(current_start_x == x){
      retval = current_start->data;

      break;
    }
    
    g_object_get(current_end->data,
		 "x", &current_end_x,
		 NULL);
    
    if(current_end_x == x){
      retval = current_end->data;

      break;
    }

    g_object_get(current->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x == x){
      retval = current->data;
      
      break;
    }

    if(length <= 3){
      break;
    }

    if(current_x < x){
      current_start = current->next;
      current_end = current_end->prev;
    }else if(current_x > x){
      current_start = current_start->next;
      current_end = current->prev;
    }

    length = g_list_position(current_start,
			     current_end) + 1;
    position = (length - 1) / 2;

    current = g_list_nth(current_start,
			 position);
  }

  g_rec_mutex_unlock(program_mutex);

  return(retval);
}

/**
 * ags_program_find_region:
 * @program: the #AgsProgram
 * @x0: x start offset
 * @x1: x end offset
 * @use_selection_list: if %TRUE selection is searched
 *
 * Find markers by offset and region.
 *
 * Returns: (element-type AgsAudio.Marker) (transfer container): the matching markers as #GList.
 *
 * Since: 5.1.0
 */
GList*
ags_program_find_region(AgsProgram *program,
			guint x0,
			guint x1,
			gboolean use_selection_list)
{
  GList *marker;
  GList *region;

  guint current_x;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  if(x0 > x1){
    guint tmp;

    tmp = x1;
    x1 = x0;
    x0 = x1;
  }
  
  /* find marker */
  g_rec_mutex_lock(program_mutex);

  if(use_selection_list){
    marker = program->selection;
  }else{
    marker = program->marker;
  }

  while(marker != NULL){
    g_object_get(marker->data,
		 "x", &current_x,
		 NULL);
    
    if(current_x > x0){
      break;
    }
    
    marker = marker->next;
  }

  region = NULL;

  while(marker != NULL){
    g_object_get(marker->data,
		 "x", &current_x,
		 NULL);

    if(current_x > x1){
      break;
    }

    region = g_list_prepend(region,
			    marker->data);

    marker = marker->next;
  }

  g_rec_mutex_unlock(program_mutex);

  region = g_list_reverse(region);

  return(region);
}

/**
 * ags_program_free_selection:
 * @program: the #AgsProgram
 *
 * Clear selection.
 *
 * Since: 5.1.0
 */
void
ags_program_free_selection(AgsProgram *program)
{
  AgsMarker *marker;

  GList *list_start, *list;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* free selection */
  g_rec_mutex_lock(program_mutex);

  list =
    list_start = program->selection;
  
  while(list != NULL){
    ags_marker_unset_flags(list->data,
			   AGS_MARKER_IS_SELECTED);
    
    list = list->next;
  }

  program->selection = NULL;

  g_rec_mutex_unlock(program_mutex);
  
  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_program_free_all_selection:
 * @program: the #GList-struct containing #AgsProgram
 *
 * Clear all selection of @program.
 *
 * Since: 3.14.10
 */
void
ags_program_free_all_selection(GList *program)
{
  while(program != NULL){
    ags_program_free_selection(program->data);

    program = program->next;
  }
}

/**
 * ags_program_add_region_to_selection:
 * @program: the #AgsProgram
 * @x0: x start offset
 * @x1: x end offset
 * @replace_current_selection: if %TRUE selection is replaced
 *
 * Add marker within region to selection.
 *
 * Since: 5.1.0
 */
void
ags_program_add_region_to_selection(AgsProgram *program,
				    guint x0, guint x1,
				    gboolean replace_current_selection)
{
  AgsMarker *marker;

  GList *region, *list;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* find region */
  region = ags_program_find_region(program,
				   x0,
				   x1,
				   FALSE);

  if(replace_current_selection){
    ags_program_free_selection(program);

    list = region;

    while(list != NULL){
      guint current_x;

      g_object_get(list->data,
		   "x", &current_x,
		   NULL);
      
      ags_marker_set_flags(list->data,
			   AGS_MARKER_IS_SELECTED);
      g_object_ref(list->data);

      list = list->next;
    }

    /* replace */
    g_rec_mutex_lock(program_mutex);
     
    program->selection = region;

    g_rec_mutex_unlock(program_mutex);
  }else{
    list = region;
    
    while(list != NULL){
      guint current_x;

      g_object_get(list->data,
		   "x", &current_x,
		   NULL);

      if(!ags_program_is_marker_selected(program, list->data)){
	/* add */
	ags_program_add_marker(program,
			       list->data,
			       TRUE);
      }
      
      list = list->next;
    }
    
    g_list_free(region);
  }
}

/**
 * ags_program_remove_region_from_selection:
 * @program: the #AgsProgram
 * @x0: x start offset
 * @x1: x end offset
 *
 * Remove markers within region of selection.
 *
 * Since: 5.1.0
 */ 
void
ags_program_remove_region_from_selection(AgsProgram *program,
					 guint x0, guint x1)
{
  AgsMarker *marker;

  GList *region;
  GList *list;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* find region */
  region = ags_program_find_region(program,
				   x0,
				   x1,
				   TRUE);

  list = region;
  
  while(list != NULL){
    ags_marker_unset_flags(list->data,
			   AGS_MARKER_IS_SELECTED);

    /* remove */
    g_rec_mutex_lock(program_mutex);

    program->selection = g_list_remove(program->selection,
				       list->data);

    g_rec_mutex_unlock(program_mutex);

    g_object_unref(list->data);

    /* iterate */
    list = list->next;
  }

  g_list_free(region);
}

/**
 * ags_program_add_all_to_selection:
 * @program: the #AgsProgram
 *
 * Select all marker to selection.
 *
 * Since: 5.1.0
 */
void
ags_program_add_all_to_selection(AgsProgram *program)
{
  GList *list;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* select all */
  g_rec_mutex_lock(program_mutex);

  list = program->marker;

  while(list != NULL){
    guint current_x;

    g_object_get(list->data,
		 "x", &current_x,
		 NULL);
    
    ags_program_add_marker(program,
			   list->data, TRUE);
    g_object_set(list->data,
		 "selection-x0", current_x,
		 "selection-x1", current_x + 1,
		 NULL);
    
    list = list->next;
  }

  g_rec_mutex_unlock(program_mutex);
}

/**
 * ags_program_copy_selection:
 * @program: the #AgsProgram
 *
 * Copy selection to clipboard.
 *
 * Returns: (transfer none): the selection as XML.
 *
 * Since: 5.1.0
 */
xmlNode*
ags_program_copy_selection(AgsProgram *program)
{
  AgsMarker *marker;

  AgsTimestamp *timestamp;
  
  xmlNode *program_node, *current_marker;
  xmlNode *timestamp_node;

  GList *start_selection, *selection;

  xmlChar *str;
  gchar *gstr;
  
  guint x_boundary;

  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* create root node */
  program_node = xmlNewNode(NULL,
			    BAD_CAST "program");

  xmlNewProp(program_node,
	     BAD_CAST "program",
	     BAD_CAST "ags");
  xmlNewProp(program_node,
	     BAD_CAST "type",
	     BAD_CAST (AGS_PROGRAM_CLIPBOARD_TYPE));
  xmlNewProp(program_node,
	     BAD_CAST "version",
	     BAD_CAST (AGS_PROGRAM_CLIPBOARD_VERSION));
  xmlNewProp(program_node,
	     BAD_CAST "format",
	     BAD_CAST (AGS_PROGRAM_CLIPBOARD_FORMAT));

  /* timestamp */
  g_object_get(program,
	       "timestamp", &timestamp,
	       NULL);

  if(timestamp != NULL){
    timestamp_node = xmlNewNode(NULL,
				BAD_CAST "timestamp");
    xmlAddChild(program_node,
		timestamp_node);

    gstr = g_strdup_printf("%lu", ags_timestamp_get_ags_offset(timestamp));
    
    xmlNewProp(timestamp_node,
	       BAD_CAST "offset",
	       BAD_CAST (gstr));

    g_free(gstr);
    
    g_object_unref(timestamp);
  }
  
  /* selection */
  g_rec_mutex_lock(program_mutex);

  selection =
    start_selection = g_list_copy(program->selection);

  g_rec_mutex_unlock(program_mutex);

  x_boundary = 0;

  while(selection != NULL){
    GRecMutex *marker_mutex;

    marker = AGS_MARKER(selection->data);

    marker_mutex = AGS_MARKER_GET_OBJ_MUTEX(marker);
    
    current_marker = xmlNewChild(program_node,
				 NULL,
				 BAD_CAST "marker",
				 NULL);

    g_rec_mutex_lock(marker_mutex);

    //    g_message("copy - marker->x = %lu", marker->x);

    gstr = g_strdup_printf("%lu", marker->x);
    
    xmlNewProp(current_marker,
	       BAD_CAST "x",
	       BAD_CAST (gstr));

    g_free(gstr);

    gstr = g_strdup_printf("%f", marker->y);
    
    xmlNewProp(current_marker,
	       BAD_CAST "y",
	       BAD_CAST (gstr));

    g_free(gstr);
        
    g_rec_mutex_unlock(marker_mutex);

    selection = selection->next;
  }

  g_list_free(start_selection);

  gstr = g_strdup_printf("%lu", x_boundary);
  xmlNewProp(program_node,
	     BAD_CAST "x-boundary",
	     BAD_CAST (gstr));

  g_free(gstr);

  return(program_node);
}

/**
 * ags_program_cut_selection:
 * @program: the #AgsProgram
 *
 * Cut selection to clipboard.
 *
 * Returns: (transfer none): the selection as xmlNode
 *
 * Since: 5.1.0
 */
xmlNode*
ags_program_cut_selection(AgsProgram *program)
{
  xmlNode *program_node;
  
  GList *selection, *marker;
  
  GRecMutex *program_mutex;

  if(!AGS_IS_PROGRAM(program)){
    return(NULL);
  }

  /* get program mutex */
  program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(program);

  /* copy selection */
  program_node = ags_program_copy_selection(program);

  /* cut */
  g_rec_mutex_lock(program_mutex);

  selection = program->selection;

  while(selection != NULL){
    program->marker = g_list_remove(program->marker,
				    selection->data);
    g_object_unref(selection->data);

    selection = selection->next;
  }

  g_rec_mutex_unlock(program_mutex);

  /* free selection */
  ags_program_free_selection(program);

  return(program_node);
}

void
ags_program_insert_native_level_from_clipboard_version_5_1_0(AgsProgram *program,
							     xmlNode *root_node, char *version,
							     char *x_boundary,
							     gboolean reset_x_offset, guint x_offset,
							     gboolean do_replace,
							     guint relative_offset,
							     gboolean match_timestamp)
{
  AgsMarker *marker;

  AgsTimestamp *timestamp;

  xmlNode *node;

  void *clipboard_data;
  unsigned char *clipboard_cdata;

  xmlChar *x, *y;
  gchar *offset;
  char *endptr;

  guint timestamp_offset;
  guint x_boundary_val;
  guint x_val;
  gdouble y_val;
  guint base_x_difference;
  guint target_frame_count, frame_count;
  guint word_size;
  gsize clipboard_length;
  gboolean subtract_x;

  node = root_node->children;

  /* retrieve x values for resetting */
  base_x_difference = 0;
  subtract_x = FALSE;

  x_boundary_val = 0;
    
  if(reset_x_offset){
    if(x_boundary != NULL){
      errno = 0;
	
      x_boundary_val = strtoul(x_boundary,
			       &endptr,
			       10);

      if(errno == ERANGE){
	goto dont_reset_x_offset;
      } 
	
      if(x_boundary == endptr){
	goto dont_reset_x_offset;
      }

      if(x_boundary_val < x_offset){
	base_x_difference = x_offset - x_boundary_val;
	subtract_x = FALSE;
      }else{
	base_x_difference = x_boundary_val - x_offset;
	subtract_x = TRUE;
      }
    }else{
    dont_reset_x_offset:
      reset_x_offset = FALSE;
    }
  }

  /* parse */
  while(node != NULL){
    if(node->type == XML_ELEMENT_NODE){
      if(!xmlStrncmp("marker",
		     node->name,
		     7)){
	/* retrieve x offset */
	x = xmlGetProp(node,
		       "x");

	if(x == NULL){
	  node = node->next;
	  
	  continue;
	}
	  
	errno = 0;
	x_val = g_ascii_strtoull(x,
				 &endptr,
				 10);

	if(errno == ERANGE){
	  node = node->next;
	  
	  continue;
	} 

	if(x == endptr){
	  node = node->next;
	  
	  continue;
	}

	/* retrieve y value */
	y = xmlGetProp(node,
		       "y");

	if(y == NULL){
	  node = node->next;
	  
	  continue;
	}
	  
	errno = 0;
	y_val = g_strtod(x,
			 &endptr);

	if(errno == ERANGE){
	  node = node->next;
	  
	  continue;
	} 
	
	/* calculate new offset */
	if(reset_x_offset){
	  errno = 0;

	  if(subtract_x){
	    x_val -= base_x_difference;

	    if(errno != 0){
	      node = node->next;
	      
	      continue;
	    }
	  }else{
	    x_val += base_x_difference;

	    if(errno != 0){
	      node = node->next;
	      
	      continue;
	    }
	  }
	}
	
	/* add marker */
	g_object_get(program,
		     "timestamp", &timestamp,
		     NULL);

	timestamp_offset = ags_timestamp_get_ags_offset(timestamp);
	g_object_unref(timestamp);

	if(!match_timestamp ||
	   x_val < timestamp_offset + relative_offset){
	  /* find first */
	  marker = ags_program_find_point(program,
					  x_val,
					  FALSE);
	    
	  if(marker == NULL){
	    marker = ags_marker_new();
	      
	    marker->x = x_val;
	    marker->y = y_val;
	    
	    //	      g_message("created %d", x_val);
	      
	    ags_program_add_marker(program,
				   marker,
				   FALSE);
	  }
	    
	  /* find next */
	  marker = ags_program_find_point(program,
					  x_val,
					  FALSE);
	    
	  if(marker == NULL){
	    marker = ags_marker_new();

	    marker->x = x_val;
	    marker->y = y_val;
	      
	    ags_program_add_marker(program,
				   marker,
				   FALSE);
	  }
	}
      }
    }
      
    node = node->next;
  }
}

/**
 * ags_program_insert_native_level_from_clipboard:
 * @program: the #AgsProgram
 * @program_node: the clipboard XML data
 * @version: clipboard version
 * @x_boundary: region start offset
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @do_replace: if %TRUE current data is replaced, otherwise additive mixing is performed 
 *
 * Paste previously copied markers. 
 *
 * Since: 5.1.0
 */
void
ags_program_insert_native_level_from_clipboard(AgsProgram *program,
					       xmlNode *root_node, char *version,
					       char *x_boundary,
					       gboolean reset_x_offset, guint x_offset,
					       gboolean do_replace)
{
  xmlChar *str;
  
  guint relative_offset;
  
  gboolean match_timestamp;  

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  match_timestamp = TRUE;
  
  if(!xmlStrncmp("5.1.0",
		 version,
		 6)){
    /* changes contain only optional informations */
    relative_offset = AGS_PROGRAM_DEFAULT_OFFSET;
    
    ags_program_insert_native_level_from_clipboard_version_5_1_0(program,
								 root_node, version,
								 x_boundary,
								 reset_x_offset, x_offset,
								 do_replace,
								 relative_offset,
								 match_timestamp);
  }  
}

/**
 * ags_program_insert_from_clipboard:
 * @program: the #AgsProgram
 * @program_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 *
 * Paste previously copied markers. 
 *
 * Since: 5.1.0
 */
void
ags_program_insert_from_clipboard(AgsProgram *program,
				  xmlNode *program_node,
				  gboolean reset_x_offset, guint x_offset)
{
  ags_program_insert_from_clipboard_extended(program,
					     program_node,
					     reset_x_offset, x_offset,
					     FALSE);
}

/**
 * ags_program_insert_from_clipboard_extended:
 * @program: the #AgsProgram
 * @program_node: the clipboard XML data
 * @reset_x_offset: if %TRUE @x_offset used as cursor
 * @x_offset: region start cursor offset
 * @do_replace: if %TRUE current data is replaced, otherwise additive mixing is performed 
 * 
 * Paste previously copied markers. 
 * 
 * Since: 5.1.0
 */
void
ags_program_insert_from_clipboard_extended(AgsProgram *program,
					   xmlNode *program_node,
					   gboolean reset_x_offset, guint x_offset,
					   gboolean do_replace)
{
  char *program_name, *version, *type, *format;
  char *x_boundary;

  if(!AGS_IS_PROGRAM(program)){
    return;
  }

  while(program_node != NULL){
    if(program_node->type == XML_ELEMENT_NODE && !xmlStrncmp("program", program_node->name, 9)){
      break;
    }

    program_node = program_node->next;
  }

  if(program_node != NULL){
    program_name = xmlGetProp(program_node, "program");

    if(!xmlStrncmp("ags", program_name, 4)){
      version = xmlGetProp(program_node, "version");
      type = xmlGetProp(program_node, "type");
      format = xmlGetProp(program_node, "format");

      if(!xmlStrcmp(AGS_PROGRAM_CLIPBOARD_FORMAT,
		    format)){
	x_boundary = xmlGetProp(program_node, "x-boundary");

	ags_program_insert_native_level_from_clipboard(program,
						       program_node, version,
						       x_boundary,
						       reset_x_offset, x_offset,
						       do_replace);
      }
    }
  }
}

/**
 * ags_program_get_control_name_unique:
 * @program: (element-type AgsAudio.Program) (transfer none): the #GList-struct containing #AgsProgram
 *
 * Retrieve program port control_name.
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): a %NULL terminated string array
 *
 * Since: 5.1.0
 */
gchar**
ags_program_get_control_name_unique(GList *program)
{
  AgsProgram *current_program;
  
  gchar **control_name;
  gchar *current_control_name;
  
  guint length, i;
  gboolean contains_control_name;

  GRecMutex *program_mutex;

  if(program == NULL){
    return(NULL);
  }
  
  control_name = (gchar **) malloc(sizeof(gchar*));
  control_name[0] = NULL;
  length = 1;
  
  while(program != NULL){
    current_program = program->data;
    
    /* get program mutex */
    program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(current_program);

    /* duplicate control name */
    g_rec_mutex_lock(program_mutex);

    current_control_name = g_strdup(current_program->control_name);
    
    g_rec_mutex_unlock(program_mutex);
    
#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(control_name,
					    current_control_name);
#else
    contains_control_name = ags_strv_contains(control_name,
					      current_control_name);
#endif
    
    if(!contains_control_name){
      control_name = (gchar **) realloc(control_name,
					(length + 1) * sizeof(gchar *));
      control_name[length - 1] = current_control_name;
      control_name[length] = NULL;

      length++;
    }else{
      g_free(current_control_name);
    }

    /* iterate */
    program = program->next;
  }
  
  return(control_name);
}

/**
 * ags_program_find_control_name:
 * @program: (element-type AgsAudio.Program) (transfer none): the #GList-struct containing #AgsProgram
 * @control_name: the string control_name to find
 *
 * Find port control_name.
 *
 * Returns: (element-type AgsAudio.Program) (transfer none): Next matching #GList
 *
 * Since: 5.1.0
 */
GList*
ags_program_find_control_name(GList *program,
			      gchar *control_name)
{
  AgsProgram *current_program;
  
  gchar *current_control_name;

  gboolean success;
  
  GRecMutex *program_mutex;
 
  while(program != NULL){
    current_program = program->data;

    /* get program mutex */
    program_mutex = AGS_PROGRAM_GET_OBJ_MUTEX(current_program);

    /* duplicate control name */
    g_rec_mutex_lock(program_mutex);

    current_control_name = g_strdup(current_program->control_name);
    
    g_rec_mutex_unlock(program_mutex);

    /* check control name */
    success = (!g_ascii_strcasecmp(current_control_name,
				   control_name)) ? TRUE: FALSE;
    g_free(current_control_name);
    
    if(success){
      break;
    }

    program = program->next;
  }

  return(program);
}

/**
 * ags_program_new:
 * @audio: the assigned #AgsAudio
 *
 * Creates a new instance of #AgsProgram.
 *
 * Returns: a new #AgsProgram
 *
 * Since: 5.1.0
 */
AgsProgram*
ags_program_new(gchar *control_name)
{
  AgsProgram *program;
  
  program = (AgsProgram *) g_object_new(AGS_TYPE_PROGRAM,
					"control-name", control_name,
					NULL);
  
  return(program);
}
