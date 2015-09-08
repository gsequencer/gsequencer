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

#include <ags/X/machine/ags_cell_pattern_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/X/ags_machine.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_cell_pattern_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
					   AgsCellPattern *cell_pattern);

gboolean
ags_cell_pattern_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsCellPattern *cell_pattern)
{
  pthread_t thread;
  
  pthread_create(&thread, NULL,
		 ags_cell_pattern_blink_worker, cell_pattern);
  
  return(TRUE);
}

gboolean
ags_cell_pattern_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_paint(cell_pattern);
  
  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_paint(cell_pattern);

  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsCellPattern *cell_pattern)
{
  if(event->button == 1){
    AgsMachine *machine;
    
    AgsTogglePatternBit *toggle_pattern_bit;
    AgsChannel *channel;
    
    guint i, j;
    guint index1;

    machine = gtk_widget_get_ancestor(cell_pattern,
				      AGS_TYPE_MACHINE);
        
    i = (guint) floor((double) event->y / (double) cell_pattern->cell_height);
    j = (guint) floor((double) event->x / (double) cell_pattern->cell_width);

    index1 = machine->bank_1;

    channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - ((guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + i) - 1);

    toggle_pattern_bit = ags_toggle_pattern_bit_new(channel->pattern->data,
						    channel->line,
						    0, index1,
						    j);
    g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
		     G_CALLBACK(ags_cell_pattern_refresh_gui_callback), cell_pattern);

    ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(machine->audio->devout)->ags_main)->main_loop)->task_thread),
				AGS_TASK(toggle_pattern_bit));
  }else if (event->button == 3){
  }

  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *cell_pattern)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      cell_pattern->key_mask |= AGS_CELL_PATTERN_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      cell_pattern->key_mask |= AGS_CELL_PATTERN_KEY_R_CONTROL;
    }
    break;
  case GDK_KEY_c:
    {
      /* copy notes */
      if((AGS_CELL_PATTERN_KEY_L_CONTROL & (cell_pattern->key_mask)) != 0 || (AGS_CELL_PATTERN_KEY_R_CONTROL & (cell_pattern->key_mask)) != 0){
	AgsMachine *machine;

	machine = gtk_widget_get_ancestor(cell_pattern,
					  AGS_TYPE_MACHINE);
	
	ags_machine_copy_pattern(machine);
      }
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_cell_pattern_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *cell_pattern)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }
  
  switch(event->keyval){
  case GDK_KEY_Control_L:
    {
      cell_pattern->key_mask &= (~AGS_CELL_PATTERN_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      cell_pattern->key_mask &= (~AGS_CELL_PATTERN_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Left:
  case GDK_KEY_leftarrow:
    {
      if(cell_pattern->cursor_x > 0){
	cell_pattern->cursor_x -= 1;
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(cell_pattern->cursor_x < cell_pattern->n_cols){
	cell_pattern->cursor_x += 1;
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(cell_pattern->cursor_y > 0){
	cell_pattern->cursor_y -= 1;
      }

      if(cell_pattern->cursor_y < GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value - 1.0);
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(cell_pattern->cursor_y < cell_pattern->n_rows){
	cell_pattern->cursor_y += 1;
      }

      if(cell_pattern->cursor_y >= GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + 1.0);
      }
    }
    break;
  case GDK_KEY_space:
    {
      AgsMachine *machine;
      
      AgsTogglePatternBit *toggle_pattern_bit;
      AgsChannel *channel;
      
      guint i, j;
      guint index1;
      
      machine = gtk_widget_get_ancestor(cell_pattern,
					AGS_TYPE_MACHINE);
      
      i = cell_pattern->cursor_y;
      j = cell_pattern->cursor_x;
      
      index1 = machine->bank_1;

      channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - i - 1);

      toggle_pattern_bit = ags_toggle_pattern_bit_new(channel->pattern->data,
						      channel->line,
						      0, index1,
						      j);
      g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
		       G_CALLBACK(ags_cell_pattern_refresh_gui_callback), cell_pattern);
      
      ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(machine->audio->devout)->ags_main)->main_loop)->task_thread),
				  AGS_TASK(toggle_pattern_bit));
    }
    break;
  }

  return(TRUE);
}

void
ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_draw_matrix(cell_pattern);
}

void
ags_cell_pattern_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				      AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;
  
  AgsChannel *channel;

  guint line;
  
  machine = gtk_widget_get_ancestor(cell_pattern,
				    AGS_TYPE_MACHINE);
    
  channel = ags_channel_nth(machine->audio->input,
			    toggle_pattern_bit->line);
  line = machine->audio->input_pads - toggle_pattern_bit->line - (guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value - 1;

  ags_cell_pattern_redraw_gutter_point(cell_pattern,
				       channel,
				       toggle_pattern_bit->bit,
				       line);
}
