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

#include <ags/app/ags_link_collection_editor_callbacks.h>

#include <ags/app/ags_machine_editor.h>

void
ags_link_collection_editor_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent,
					       AgsLinkCollectionEditor *link_collection_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL){
    return;
  }

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(widget, 
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor != NULL &&
     machine_editor->machine != NULL){
    gtk_combo_box_set_model(link_collection_editor->link,
			    GTK_TREE_MODEL(ags_machine_get_possible_links(machine_editor->machine)));

    ags_link_collection_editor_check(link_collection_editor);
  }
}

void ags_link_collection_editor_link_callback(GtkComboBox *combo,
					      AgsLinkCollectionEditor *link_collection_editor)
{
  ags_link_collection_editor_check(link_collection_editor);
}

void ags_link_collection_editor_first_line_callback(GtkSpinButton *spin_button,
						    AgsLinkCollectionEditor *link_collection_editor)
{
  ags_link_collection_editor_check(link_collection_editor);
}

void ags_link_collection_editor_first_link_callback(GtkSpinButton *spin_button,
						    AgsLinkCollectionEditor *link_collection_editor)
{
  ags_link_collection_editor_check(link_collection_editor);
}
