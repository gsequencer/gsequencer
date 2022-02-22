/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/app/ags_input_collection_editor_callbacks.h>

#include <ags/app/ags_connection_editor.h>

void
ags_input_collection_editor_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent,
						AgsInputCollectionEditor *input_collection_editor)
{
  AgsConnectionEditor *connection_editor;

  if(old_parent != NULL){
    return;
  }

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(widget, 
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor != NULL &&
     connection_editor->machine != NULL){
    gtk_combo_box_set_model(GTK_COMBO_BOX(input_collection_editor->soundcard),
			    GTK_TREE_MODEL(ags_machine_get_possible_audio_input_connections(connection_editor->machine)));

    ags_input_collection_editor_check(input_collection_editor);
  }
}

void
ags_input_collection_editor_soundcard_callback(GtkWidget *combo_box,
					       AgsInputCollectionEditor *input_collection_editor)
{
  ags_input_collection_editor_check(input_collection_editor);
}
