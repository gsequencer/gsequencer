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

#include <ags/X/ags_output_listing_editor_callbacks.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>

int
ags_output_listing_editor_parent_set_callback(GtkWidget *widget,
					      GtkObject *old_parent,
					      AgsOutputListingEditor *output_listing_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(widget,
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor->machine != NULL){
    ags_output_listing_editor_add_children(output_listing_editor,
					   machine_editor->machine->audio, 0,
					   FALSE);
  }
  
  return(0);
}

void
ags_output_listing_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
					    guint pads, guint pads_old,
					    AgsOutputListingEditor *output_listing_editor)
{
}
