/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/ags_connection_editor_dialog_callbacks.h>

#include <ags/app/ags_ui_provider.h>

gboolean
ags_connection_editor_dialog_close_request_callback(GtkWindow *window, gpointer user_data)
{
  ags_connection_editor_dialog_response((AgsConnectionEditorDialog *) window,
					GTK_RESPONSE_CLOSE);	

  return(FALSE);
}
