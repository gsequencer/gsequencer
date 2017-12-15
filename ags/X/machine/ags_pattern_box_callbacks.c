/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/machine/ags_pattern_box_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <gdk/gdkkeysyms.h>

gboolean
ags_pattern_box_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box)
{
  GList *list;

  list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);
  gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
						     pattern_box->cursor_x),
		       GTK_STATE_PRELIGHT);
  g_list_free(list);
  
  return(TRUE);
}

gboolean
ags_pattern_box_focus_out_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box)
{
  GList *list;

  list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  if(!gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(list,
								       pattern_box->cursor_x - 1))){
    gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
						       pattern_box->cursor_x),
			 GTK_STATE_NORMAL);
  }else{
    gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
						       pattern_box->cursor_x),
			 GTK_STATE_ACTIVE);
  }
  
  g_list_free(list);
  
  return(TRUE);
}

void
ags_pattern_box_pad_callback(GtkWidget *toggle_button, AgsPatternBox *pattern_box)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsLine *selected_line;

  AgsPattern *pattern;
  
  AgsTogglePatternBit *toggle_pattern_bit;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;
  
  AgsApplicationContext *application_context;

  GList *list, *list_start;
  GList *line, *line_start;
  GList *tasks;
  guint i, index0, index1, offset;

  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *application_mutex;
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);

  if(machine->selected_input_pad == NULL){
    return;
  }

  /*  */
  if((AGS_PATTERN_BOX_BLOCK_PATTERN & (pattern_box->flags)) != 0){
#ifdef AGS_DEBUG
    g_message("AgsPatternBox pattern is blocked\n");
#endif

    return;
  }

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);
  
  /* find task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						     AGS_TYPE_GUI_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* calculate offset */
  pthread_mutex_lock(audio_mutex);

  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  for(i = 0; i < pattern_box->n_controls && toggle_button != list->data; i++){
    list = list->next;
  }

  offset = i;
  g_list_free(list_start);

  /* retrieve indices */
  index0 = machine->bank_0;
  index1 = machine->bank_1;
  
  /* calculate offset / page */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  for(i = 0; i < 4 && !GTK_TOGGLE_BUTTON(list->data)->active; i++){
    list = list->next;
  }

  offset += (i * pattern_box->n_controls);

  g_list_free(list_start);

  /**/
  line_start = 
    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(machine->selected_input_pad)->expander_set));
  tasks = NULL;

  while((line = ags_line_find_next_grouped(line)) != NULL){
    selected_line = AGS_LINE(line->data);

    toggle_pattern_bit = ags_toggle_pattern_bit_new(selected_line->channel->pattern->data,
						    selected_line->channel->line,
						    index0, index1,
						    offset);

    tasks = g_list_prepend(tasks,
			   toggle_pattern_bit);

    line = line->next;
  }

  g_list_free(line_start);

  /* append AgsTogglePatternBit */
  ags_gui_thread_schedule_task_list(gui_thread,
				    tasks);

  pthread_mutex_unlock(audio_mutex);
}

void
ags_pattern_box_offset_callback(GtkWidget *widget, AgsPatternBox *pattern_box)
{
  ags_pattern_box_set_pattern(pattern_box);
}

gboolean
ags_pattern_box_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsPatternBox *pattern_box)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      pattern_box->key_mask |= AGS_PATTERN_BOX_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      pattern_box->key_mask |= AGS_PATTERN_BOX_KEY_R_CONTROL;
    }
    break;
  case GDK_KEY_c:
    {
      /* copy notes */
      if((AGS_PATTERN_BOX_KEY_L_CONTROL & (pattern_box->key_mask)) != 0 || (AGS_PATTERN_BOX_KEY_R_CONTROL & (pattern_box->key_mask)) != 0){
	AgsMachine *machine;

	machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
							 AGS_TYPE_MACHINE);
	
	ags_machine_copy_pattern(machine);
      }
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_pattern_box_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsPatternBox *pattern_box)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      pattern_box->key_mask &= (~AGS_PATTERN_BOX_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      pattern_box->key_mask &= (~AGS_PATTERN_BOX_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Left:
  case GDK_KEY_leftarrow:
    {
      if(pattern_box->cursor_x > 0){
	GList *list;
	
	pattern_box->cursor_x -= 1;

	list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);
	
	gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
							   pattern_box->cursor_x),
			     GTK_STATE_PRELIGHT);

	if(!gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(list,
									     pattern_box->cursor_x + 1))){
	  gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
							     pattern_box->cursor_x + 1),
			       GTK_STATE_NORMAL);
	}else{
	  gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
							     pattern_box->cursor_x + 1),
			       GTK_STATE_ACTIVE);
	}

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(list,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  /* give audible feedback */
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
	
	g_list_free(list);
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(pattern_box->cursor_x + 1 < pattern_box->n_controls){
	GList *list;
	
	pattern_box->cursor_x += 1;

	list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

      	gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
							   pattern_box->cursor_x),
			     GTK_STATE_PRELIGHT);
	
	if(!gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(list,
									     pattern_box->cursor_x - 1))){
	  gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
							     pattern_box->cursor_x - 1),
			       GTK_STATE_NORMAL);
	}else{	  
	  gtk_widget_set_state((GtkWidget *) g_list_nth_data(list,
							     pattern_box->cursor_x - 1),
			       GTK_STATE_ACTIVE);
	}

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(list,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  /* give audible feedback */
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
	
	g_list_free(list);
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(pattern_box->cursor_y > 0){
	GList *list, *pad;

	pattern_box->cursor_y -= 1;

	list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

      	gtk_button_clicked(g_list_nth_data(list,
					   pattern_box->cursor_y));

	g_list_free(list);

	/* give audible feedback */
	pad = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(pad,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(pattern_box->cursor_y + 1 < pattern_box->n_indices){
	GList *list, *pad;

	list = gtk_container_get_children((GtkContainer *) pattern_box->offset);
	pattern_box->cursor_y += 1;

	gtk_button_clicked(g_list_nth_data(list,
					   pattern_box->cursor_y));

	g_list_free(list);
	
	/* give audible feedback */
	pad = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

	if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(pad,
									    pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  ags_pad_play((AgsPad *) machine->selected_input_pad);
	}
      }
    }
    break;
  case GDK_KEY_space:
    {
      AgsLine *selected_line;

      AgsChannel *channel;
      
      GList *line, *line_start;
      GList *tasks;
      
      guint i, j;
      guint offset;
      guint index0, index1;
      
      pthread_mutex_t *application_mutex;
      pthread_mutex_t *channel_mutex;

      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
      
      i = pattern_box->cursor_y;
      j = pattern_box->cursor_x;

      offset = (i * pattern_box->n_controls) + j;
      
      index0 = machine->bank_0;
      index1 = machine->bank_1;

      line_start = 
	line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(machine->selected_input_pad)->expander_set));
      tasks = NULL;

      while((line = ags_line_find_next_grouped(line)) != NULL){
	selected_line = AGS_LINE(line->data);
	channel = selected_line->channel;
	
	/* get channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
  
	pthread_mutex_unlock(application_mutex);
	
	/* toggle pattern */
	pthread_mutex_lock(channel_mutex);
    
	ags_pattern_toggle_bit(channel->pattern->data,
			       index0, index1,
			       offset);

	pthread_mutex_unlock(channel_mutex);

	line = line->next;
      }

      g_list_free(line_start);

      /* give audible feedback */
      ags_pad_play((AgsPad *) machine->selected_input_pad);
    }
    break;
  }

  return(TRUE);
}

//TODO:JK: remove
#if 0
void
ags_pattern_box_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				     AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  
  AgsChannel *channel;

  GList *radio;
  
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);
    
  channel = ags_channel_nth(machine->audio->input,
			    toggle_pattern_bit->line);

  if(!gtk_toggle_button_get_active((GtkToggleButton *) AGS_LINE(channel->line_widget)->group)){
    return;
  }
  
  radio = gtk_container_get_children((GtkContainer *) pattern_box->offset);
  
  if(gtk_toggle_button_get_active((GtkToggleButton *) g_list_nth_data(radio,
								      toggle_pattern_bit->bit / pattern_box->n_controls))){
    GList *list;

    /* apply pattern to GUI */
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

    pattern_box->flags |= AGS_PATTERN_BOX_BLOCK_PATTERN;
    
    gtk_toggle_button_set_active((GtkToggleButton *) g_list_nth_data(list,
								     toggle_pattern_bit->bit % pattern_box->n_controls),
				 TRUE);
    
    pattern_box->flags &= (~AGS_PATTERN_BOX_BLOCK_PATTERN);

    g_list_free(list);
    
    /* give audible feedback */
    ags_pad_play((AgsPad *) machine->selected_input_pad);
  }

  g_list_free(radio);
}
#endif
