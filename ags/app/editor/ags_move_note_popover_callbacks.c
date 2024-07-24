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

#include <ags/app/editor/ags_move_note_popover_callbacks.h>

void
ags_move_note_popover_relative_callback(GtkWidget *button,
					AgsMoveNotePopover *move_note_popover)
{
  gtk_spin_button_set_range(move_note_popover->move_x,
			    -1 * AGS_MOVE_NOTE_POPOVER_MAX_X,
			    AGS_MOVE_NOTE_POPOVER_MAX_X);

  gtk_spin_button_set_range(move_note_popover->move_y,
			    -1 * AGS_MOVE_NOTE_POPOVER_MAX_Y,
			    AGS_MOVE_NOTE_POPOVER_MAX_Y);
}

void
ags_move_note_popover_absolute_callback(GtkWidget *button,
					AgsMoveNotePopover *move_note_popover)
{
  gtk_spin_button_set_range(move_note_popover->move_x,
			    0,
			    AGS_MOVE_NOTE_POPOVER_MAX_X);

  gtk_spin_button_set_range(move_note_popover->move_y,
			    0,
			    AGS_MOVE_NOTE_POPOVER_MAX_Y);
}
