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

#include <ags/X/ags_output_editor_callbacks.h>

#include <ags/X/ags_connection_editor.h>

int
ags_output_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsOutputEditor *output_editor)
{
  AgsConnectionEditor *connection_editor;

  if(old_parent != NULL){
    return(0);
  }

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(widget, 
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor != NULL &&
     connection_editor->machine != NULL){
    gtk_combo_box_set_model(GTK_COMBO_BOX(output_editor->soundcard),
			    GTK_TREE_MODEL(ags_machine_get_possible_audio_output_connections(connection_editor->machine)));

    ags_output_editor_check(output_editor);
  }

  return(0);
}

void
ags_output_editor_soundcard_callback(GtkComboBoxText *combo_text, AgsOutputEditor *output_editor)
{
  ags_output_editor_check(output_editor);
}
