/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/editor/ags_crop_note_popover_callbacks.h>

void
ags_crop_note_popover_absolute_callback(GtkWidget *button,
					AgsCropNotePopover *crop_note_popover)
{
  if(gtk_toggle_button_get_active((GtkToggleButton *) crop_note_popover->absolute)){
    gtk_spin_button_set_range(crop_note_popover->crop_note,
			      0.0,
			      AGS_CROP_NOTE_POPOVER_MAX_WIDTH);
  }else{
    gtk_spin_button_set_range(crop_note_popover->crop_note,
			      -1.0 * AGS_CROP_NOTE_POPOVER_MAX_WIDTH,
			      AGS_CROP_NOTE_POPOVER_MAX_WIDTH);
  }
}
