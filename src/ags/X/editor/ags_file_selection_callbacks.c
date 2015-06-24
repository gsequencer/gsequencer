/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
#include <ags/X/editor/ags_file_selection_callbacks.h>

void
ags_file_selection_remove_callback(GtkButton *button, GtkTable *table)
{
  AgsFileSelection *file_selection;

  file_selection = (AgsFileSelection *) gtk_widget_get_ancestor(GTK_WIDGET(table),
								AGS_TYPE_FILE_SELECTION);

  ags_file_selection_remove_entry(file_selection, GTK_WIDGET(table));
}
