/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_editor_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/X/ags_window.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_meter.h>
#include <ags/X/editor/ags_note_edit.h>

#include <math.h>
#include <string.h>
#include <cairo.h>

void
ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor)
{
  if(old_parent != NULL)
    return;
}

void
ags_editor_set_audio_channels_callback(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old,
				       AgsEditor *editor)
{
  GList *tabs;
  GList *notation;
  guint i;

  if(audio_channels_old < audio_channels){
    tabs = g_list_nth(editor->notebook->tabs,
		      audio_channels_old);
    notation = g_list_nth(audio->notation,
			  audio_channels_old);

    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(editor->notebook,
			      i);
      AGS_NOTEBOOK_TAB(tabs->data)->notation = notation->data;

      tabs = tabs->next;
      notation = notation->next;
    }
  }else{
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(editor->notebook,
			      i);
    }
  }
}

void
ags_editor_set_pads_callback(AgsAudio *audio,
			     GType channel_type,
			     guint pads, guint pads_old,
			     AgsEditor *editor)
{
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
    if(!g_type_is_a(channel_type, AGS_TYPE_INPUT)){
      return;
    }
  }else{
    if(!g_type_is_a(channel_type, AGS_TYPE_OUTPUT)){
      return;
    }
  }

  ags_note_edit_set_map_height(editor->edit.note_edit,
			       pads * editor->edit.note_edit->control_height);
}

void
ags_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
				    AgsEditor *editor)
{
  ags_editor_machine_changed(editor, machine);
}

void
ags_editor_change_position_callback(AgsNavigation *navigation, gdouble tact,
				    AgsEditor *editor)
{
  cairo_t *cr;
  gdouble loop_start, loop_end;
  gdouble position;

  if(!gtk_toggle_button_get_active(navigation->scroll)){
    return;
  }

  loop_start = gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_end = gtk_spin_button_get_value(navigation->loop_right_tact);

  if(!gtk_toggle_button_get_active(navigation->loop) || tact <= loop_end){
    position = tact * editor->edit.note_edit->control_current.control_width;
  }else{
    position = loop_start * editor->edit.note_edit->control_current.control_width;
  }

  /* scroll */
  if(position - (0.125 * editor->edit.note_edit->control_current.control_width) > 0.0){
    gtk_range_set_value(GTK_RANGE(editor->edit.note_edit->hscrollbar),
			position - (0.125 * editor->edit.note_edit->control_current.control_width));
  }
}

void
ags_editor_note_edit_vscrollbar_value_changed_callback(GtkWidget *note_edit,
						       AgsEditor *editor)
{
  if((AGS_MACHINE_IS_SYNTHESIZER & (editor->selected_machine->flags)) != 0){
    gtk_widget_queue_draw(editor->meter);
  }else if((AGS_MACHINE_IS_SEQUENCER & (editor->selected_machine->flags)) != 0){
  }else{
  }
}
