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

#include <math.h>

void ags_cell_pattern_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
					   AgsCellPattern *cell_pattern);

gboolean
ags_cell_pattern_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_draw_matrix(cell_pattern);

  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_draw_gutter(cell_pattern);
  ags_cell_pattern_draw_matrix(cell_pattern);

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
