/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/ags_input_editor_callbacks.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_line_editor.h>

int
ags_input_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsInputEditor *input_editor)
{
  AgsMachine *machine;
  AgsLineEditor *line_editor;

  AgsAudio *audio;
  AgsChannel *channel;

  if(old_parent != NULL){
    return(0);
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
	
	gtk_combo_box_set_model(GTK_COMBO_BOX(input_editor->soundcard),
				GTK_TREE_MODEL(ags_machine_get_possible_audio_input_connections(machine)));
	
	ags_input_editor_check(input_editor);
      }
    }
  }
  
  return(0);
}

void
ags_input_editor_soundcard_callback(GtkComboBoxText *combo_text, AgsInputEditor *input_editor)
{
  ags_input_editor_check(input_editor);
}
