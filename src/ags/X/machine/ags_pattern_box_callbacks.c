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

#include <ags/main.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/X/ags_machine.h>

#include <gdk/gdkkeysyms.h>

void ags_pattern_box_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
					  AgsCellPattern *pattern_box);

void
ags_pattern_box_pad_callback(GtkWidget *toggle_button, AgsPatternBox *pattern_box)
{
  AgsLine *selected_line;

  AgsPattern *pattern;
  
  AgsTogglePatternBit *toggle_pattern_bit;

  AgsMutexManager *mutex_manager;

  GList *list, *list_start;
  GList *line, *line_start;
  GList *tasks;
  guint i, index0, index1, offset;

  pthread_mutex_t *audio_mutex;

  if(pattern_box->selected_pad == NULL){
    return;
  }

  if((AGS_PATTERN_BOX_BLOCK_PATTERN & (pattern_box->flags)) != 0){
#ifdef AGS_DEBUG
    g_message("AgsPatternBox pattern is blocked\n\0");
#endif

    return;
  }
  
  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) AGS_MACHINE(pattern_box)->audio);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  pthread_mutex_lock(audio_mutex);

  /* calculate offset */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  for(i = 0; i < 16 && toggle_button != list->data; i++)
    list = list->next;

  offset = i;
  g_list_free(list_start);

  /* calculate index 0 */
  index0 = ((guint) pattern_box->selected0->button.label_text[0] - 'a');
  
  /* calculate index 1 */
  index1 = ((guint) g_ascii_strtoull(pattern_box->selected1->button.label_text, NULL, 10)) - 1;
  
  /* calculate offset / page */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  for(i = 0; i < 4 && ! GTK_TOGGLE_BUTTON(list->data)->active; i++)
    list = list->next;

  offset += (i * 16);

  g_list_free(list_start);

  /**/
  line_start = 
    line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(pattern_box->selected_pad)->expander_set));
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
  ags_task_thread_append_tasks(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(pattern_box)->audio->devout)->ags_main)->main_loop)->task_thread),
			       tasks);

  pthread_mutex_unlock(audio_mutex);
}

void
ags_pattern_box_offset_callback(GtkWidget *widget, AgsPatternBox *pattern_box)
{
  ags_pattern_box_set_pattern(pattern_box);
}

gboolean
ags_pattern_box_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *pattern_box)
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
ags_pattern_box_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *pattern_box)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }
  
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
	pattern_box->cursor_x -= 1;
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(pattern_box->cursor_x < pattern_box->n_controls){
	pattern_box->cursor_x += 1;
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(pattern_box->cursor_y > 0){
	pattern_box->cursor_y -= 1;
      }

      if(pattern_box->cursor_y < GTK_RANGE(pattern_box->vscrollbar)->adjustment->value){
	gtk_range_set_value(GTK_RANGE(pattern_box->vscrollbar),
			    GTK_RANGE(pattern_box->vscrollbar)->adjustment->value - 1.0);
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(pattern_box->cursor_y < pattern_box->n_indices){
	pattern_box->cursor_y += 1;
      }

      if(pattern_box->cursor_y >= GTK_RANGE(pattern_box->vscrollbar)->adjustment->value + AGS_PATTERN_BOX_MAX_CONTROLS_SHOWN_VERTICALLY){
	gtk_range_set_value(GTK_RANGE(pattern_box->vscrollbar),
			    GTK_RANGE(pattern_box->vscrollbar)->adjustment->value + 1.0);
      }
    }
    break;
  case GDK_KEY_space:
    {
      AgsMachine *machine;
      
      AgsTogglePatternBit *toggle_pattern_bit;
      AgsChannel *channel;
      
      guint i, j;
      guint index0, index1;
      
      machine = gtk_widget_get_ancestor(pattern_box,
					AGS_TYPE_MACHINE);
      
      i = pattern_box->cursor_y;
      j = pattern_box->cursor_x;
      
      index0 = machine->bank_0;
      index1 = machine->bank_1;

      channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - i - 1);

      toggle_pattern_bit = ags_toggle_pattern_bit_new(channel->pattern->data,
						      channel->line,
						      index0, index1,
						      j);
      g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
		       G_CALLBACK(ags_pattern_box_refresh_gui_callback), pattern_box);
      
      ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(machine->audio->devout)->ags_main)->main_loop)->task_thread),
				  AGS_TASK(toggle_pattern_bit));
    }
    break;
  }

  return(TRUE);
}

void
ags_pattern_box_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				     AgsCellPattern *pattern_box)
{
  AgsMachine *machine;
  
  AgsChannel *channel;

  guint line;
  
  machine = gtk_widget_get_ancestor(pattern_box,
				    AGS_TYPE_MACHINE);
    
  channel = ags_channel_nth(machine->audio->input,
			    toggle_pattern_bit->line);
  line = machine->audio->input_pads - toggle_pattern_bit->line - (guint) GTK_RANGE(pattern_box->vscrollbar)->adjustment->value - 1;

  ags_pattern_box_redraw_gutter_point(pattern_box,
				      channel,
				      toggle_pattern_bit->bit,
				      line);
}
