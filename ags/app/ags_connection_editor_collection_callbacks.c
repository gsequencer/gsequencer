/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_connection_editor_collection_callbacks.h>

#include <ags/app/ags_connection_editor.h>

void
ags_connection_editor_collection_add_bulk_callback(GtkButton *button, AgsConnectionEditorCollection *connection_editor_collection)
{
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorBulk *bulk;

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_collection,
								      AGS_TYPE_CONNECTION_EDITOR);

  bulk = ags_connection_editor_bulk_new();

  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT & (connection_editor->flags)) != 0){
    gtk_widget_set_visible(bulk->output_grid,
			   TRUE);
  }

  if((AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT & (connection_editor->flags)) != 0){
    gtk_widget_set_visible(bulk->input_grid,
			   TRUE);
  }
  
  ags_connection_editor_collection_add_bulk(connection_editor_collection,
					    bulk);

  ags_applicable_reset(AGS_APPLICABLE(bulk));
  ags_connectable_connect(AGS_APPLICABLE(bulk));
}
