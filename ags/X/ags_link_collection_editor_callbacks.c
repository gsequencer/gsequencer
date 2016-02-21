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

#include <ags/X/ags_link_collection_editor_callbacks.h>

#include <ags/X/ags_machine_editor.h>

int
ags_link_collection_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent,
					       AgsLinkCollectionEditor *link_collection_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

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
