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

#include <<ags/object/ags_application_context.h>>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <gdk/gdkkeysyms.h>

void ags_pattern_box_init_channel_launch_callback(AgsTask *task, AgsPatternBox *pattern_box);
void ags_pattern_box_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
					  AgsPatternBox *pattern_box);

extern pthread_mutex_t ags_application_mutex;

gboolean
ags_pattern_box_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box)
{
  GList *list;

  list = gtk_container_get_children(pattern_box->pattern);
  gtk_widget_set_state(g_list_nth_data(list,
				       pattern_box->cursor_x),
		       GTK_STATE_PRELIGHT);
  g_list_free(list);
  
  return(TRUE);
}

gboolean
ags_pattern_box_focus_out_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box)
{
  GList *list;

  list = gtk_container_get_children(pattern_box->pattern);

  if(!gtk_toggle_button_get_active(g_list_nth_data(list,
						   pattern_box->cursor_x - 1))){
    gtk_widget_set_state(g_list_nth_data(list,
					 pattern_box->cursor_x),
			 GTK_STATE_NORMAL);
  }else{
    gtk_widget_set_state(g_list_nth_data(list,
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
  AgsThread *audio_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  GList *list, *list_start;
  GList *line, *line_start;
  GList *tasks;
  guint i, index0, index1, offset;

  pthread_mutex_t *audio_mutex;

  machine = gtk_widget_get_ancestor(pattern_box,
				    AGS_TYPE_MACHINE);

  if(machine->selected_input_pad == NULL){
    return;
  }

  /*  */
  if((AGS_PATTERN_BOX_BLOCK_PATTERN & (pattern_box->flags)) != 0){
#ifdef AGS_DEBUG
    g_message("AgsPatternBox pattern is blocked\n\0");
#endif

    return;
  }

  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  application_context = window->application_context;
  
  /* get audio loop */
  pthread_mutex_lock(&(ags_application_mutex));

  audio_loop = application_context->main_loop;

  pthread_mutex_unlock(&(ags_application_mutex));
  
  /* find task thread */
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(&(ags_application_mutex));

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
  ags_task_thread_append_tasks(task_thread,
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

	machine = gtk_widget_get_ancestor(pattern_box,
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

  AgsThread *audio_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }
  
  machine = gtk_widget_get_ancestor(pattern_box,
				    AGS_TYPE_MACHINE);

  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  application_context = window->application_context;
  
  /* get audio loop */
  pthread_mutex_lock(&(ags_application_mutex));

  audio_loop = application_context->main_loop;

  pthread_mutex_unlock(&(ags_application_mutex));
  
  /* find task thread */
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

  
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

	list = gtk_container_get_children(pattern_box->pattern);
	
	gtk_widget_set_state(g_list_nth_data(list,
					     pattern_box->cursor_x),
			     GTK_STATE_PRELIGHT);

	if(!gtk_toggle_button_get_active(g_list_nth_data(list,
							 pattern_box->cursor_x + 1))){
	  gtk_widget_set_state(g_list_nth_data(list,
					       pattern_box->cursor_x + 1),
			       GTK_STATE_NORMAL);
	}else{
	  gtk_widget_set_state(g_list_nth_data(list,
					       pattern_box->cursor_x + 1),
			       GTK_STATE_ACTIVE);
	}

	if(gtk_toggle_button_get_active(g_list_nth_data(list,
							pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  /* give audible feedback */
	  ags_pad_play(machine->selected_input_pad);
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

	list = gtk_container_get_children(pattern_box->pattern);

      	gtk_widget_set_state(g_list_nth_data(list,
					     pattern_box->cursor_x),
			     GTK_STATE_PRELIGHT);
	
	if(!gtk_toggle_button_get_active(g_list_nth_data(list,
							 pattern_box->cursor_x - 1))){
	  gtk_widget_set_state(g_list_nth_data(list,
					       pattern_box->cursor_x - 1),
			       GTK_STATE_NORMAL);
	}else{	  
	  gtk_widget_set_state(g_list_nth_data(list,
					       pattern_box->cursor_x - 1),
			       GTK_STATE_ACTIVE);
	}

	if(gtk_toggle_button_get_active(g_list_nth_data(list,
							pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  /* give audible feedback */
	  ags_pad_play(machine->selected_input_pad);
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

	list = gtk_container_get_children(pattern_box->offset);

      	gtk_button_clicked(g_list_nth_data(list,
					   pattern_box->cursor_y));

	g_list_free(list);

	/* give audible feedback */
	pad = gtk_container_get_children(pattern_box->pattern);

	if(gtk_toggle_button_get_active(g_list_nth_data(pad,
							pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  ags_pad_play(machine->selected_input_pad);
	}
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(pattern_box->cursor_y + 1 < pattern_box->n_indices){
	GList *list, *pad;

	list = gtk_container_get_children(pattern_box->offset);
	pattern_box->cursor_y += 1;

	gtk_button_clicked(g_list_nth_data(list,
					   pattern_box->cursor_y));

	g_list_free(list);
	
	/* give audible feedback */
	pad = gtk_container_get_children(pattern_box->pattern);

	if(gtk_toggle_button_get_active(g_list_nth_data(pad,
							pattern_box->cursor_y * pattern_box->n_controls + pattern_box->cursor_x))){
	  ags_pad_play(machine->selected_input_pad);
	}
      }
    }
    break;
  case GDK_KEY_space:
    {
      AgsLine *selected_line;

      AgsTogglePatternBit *toggle_pattern_bit;
      AgsChannel *channel;

      GList *line, *line_start;
      GList *tasks;
      
      guint i, j;
      guint offset;
      guint index0, index1;
      
      i = pattern_box->cursor_y;
      j = pattern_box->cursor_x;

      offset = (i * pattern_box->n_controls) + j;
      
      index0 = machine->bank_0;
      index1 = machine->bank_1;

      channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - i - 1);

      line_start = 
	line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(machine->selected_input_pad)->expander_set));
      tasks = NULL;

      while((line = ags_line_find_next_grouped(line)) != NULL){
	selected_line = AGS_LINE(line->data);

	/* create toggle pattern bit task */
	toggle_pattern_bit = ags_toggle_pattern_bit_new(selected_line->channel->pattern->data,
							selected_line->channel->line,
							index0, index1,
							offset);
	
	g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
			 G_CALLBACK(ags_pattern_box_refresh_gui_callback), pattern_box);

	tasks = g_list_prepend(tasks,
			       toggle_pattern_bit);

	line = line->next;
      }

      g_list_free(line_start);

      /* append tasks to task thread */
      ags_task_thread_append_tasks(task_thread,
				   tasks);

      /* give audible feedback */
      ags_pad_play(machine->selected_input_pad);
    }
    break;
  }

  return(TRUE);
}

void
ags_pattern_box_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				     AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  
  AgsChannel *channel;

  GList *radio;
  
  machine = gtk_widget_get_ancestor(pattern_box,
				    AGS_TYPE_MACHINE);
    
  channel = ags_channel_nth(machine->audio->input,
			    toggle_pattern_bit->line);

  if(!gtk_toggle_button_get_active(AGS_LINE(channel->line_widget)->group)){
    return;
  }
  
  radio = gtk_container_get_children(pattern_box->offset);
  
  if(gtk_toggle_button_get_active(g_list_nth_data(radio,
						  toggle_pattern_bit->bit / pattern_box->n_controls))){
    GList *list;

    /* apply pattern to GUI */
    list = gtk_container_get_children(pattern_box->pattern);

    pattern_box->flags |= AGS_PATTERN_BOX_BLOCK_PATTERN;
    
    gtk_toggle_button_set_active(g_list_nth_data(list,
						 toggle_pattern_bit->bit % pattern_box->n_controls),
				 TRUE);
    
    pattern_box->flags &= (~AGS_PATTERN_BOX_BLOCK_PATTERN);

    g_list_free(list);
    
    /* give audible feedback */
    ags_pad_play(AGS_PAD(machine->selected_input_pad));
  }

  g_list_free(radio);
}
