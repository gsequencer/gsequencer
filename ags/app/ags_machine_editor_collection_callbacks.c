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

#include <ags/app/ags_machine_editor_collection_callbacks.h>

void
ags_machine_editor_collection_add_bulk_callback(GtkButton *button, AgsMachineEditorCollection *machine_editor_collection)
{
  AgsMachineEditorBulk *bulk;

  bulk = ags_machine_editor_bulk_new();  
  ags_machine_editor_collection_add_bulk(machine_editor_collection,
					 bulk);

  ags_applicable_reset(AGS_APPLICABLE(bulk));
  ags_connectable_connect(AGS_CONNECTABLE(bulk));
}
