/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/app/ags_output_editor_callbacks.h>

#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_line_editor.h>

void
ags_output_editor_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsOutputEditor *output_editor)
{
  AgsMachine *machine;
  AgsLineEditor *line_editor;

  AgsAudio *audio;
  AgsChannel *channel;

  if(old_parent != NULL){
    return;
  }

  //TODO:JK: missing mutex
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_LINE_EDITOR);

  if(line_editor != NULL){
    channel = line_editor->channel;
  
    if(channel != NULL){
      GtkTreeIter iter;

      audio = AGS_AUDIO(channel->audio);

      if(audio != NULL){
	machine = AGS_MACHINE(audio->machine_widget);
	
	gtk_combo_box_set_model(GTK_COMBO_BOX(output_editor->soundcard),
				GTK_TREE_MODEL(ags_machine_get_possible_audio_output_connections(machine)));
	
	ags_output_editor_check(output_editor);
      }
    }
  }
}

void
ags_output_editor_soundcard_callback(GtkComboBoxText *combo_text, AgsOutputEditor *output_editor)
{
  ags_output_editor_check(output_editor);
}
