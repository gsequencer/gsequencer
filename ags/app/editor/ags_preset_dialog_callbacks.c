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

#include <ags/app/editor/ags_preset_dialog_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

int
ags_preset_dialog_ok_callback(GtkWidget *widget, AgsPresetDialog *preset_dialog)
{
  //  ags_applicable_set_update(AGS_APPLICABLE(preset_dialog), FALSE);
  ags_connectable_disconnect(AGS_CONNECTABLE(preset_dialog));
  ags_applicable_apply(AGS_APPLICABLE(preset_dialog));
 
  preset_dialog->machine->preset_dialog = NULL;
  gtk_window_destroy(preset_dialog);

  return(0);
}
