/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_GSEQUENCER_ACTIONS__
#define __AGS_GSEQUENCER_ACTIONS__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_GSEQUENCER_ACTION_MACHINE_BUTTON_CLICKED "machine-button-clicked"
#define AGS_GSEQUENCER_ACTION_MACHINE_CHECK_BUTTON_TOGGLED "machine-check-button-toggled"
#define AGS_GSEQUENCER_ACTION_MACHINE_RADIO_BUTTON_TOGGLED "machine-radio-button-toggled"

#define AGS_GSEQUENCER_ACTION_MACHINE_RANGE_VALUE_CHANGE "machine-range-value-change"
#define AGS_GSEQUENCER_ACTION_MACHINE_SPIN_BUTTON_VALUE_CHANGE "machine-spin-button-value-change"

#define AGS_GSEQUENCER_ACTION_MACHINE_TREE_VIEW_ROW_ACTIVATED "machine-tree-view-row-activated"

#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_NOTEBOOK_TOGGLED "machine-note-edit-notebook-toggled"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_ADD_NOTE "machine-note-edit-add-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_DELETE_NOTE "machine-note-edit-delete-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_SELECT_NOTE "machine-note-edit-select-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_COPY_NOTE "machine-note-edit-copy-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_CUT_NOTE "machine-note-edit-cut-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_PASTE_NOTE "machine-note-edit-paste-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_POPOVER_POSITION_NOTATION_CURSOR "machine-note-edit-popover-position-notation-cursor"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_POPOVER_CROP_NOTE "machine-note-edit-popover-crop-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_POPOVER_MOVE_NOTE "machine-note-edit-popover-move-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_POPOVER_SELECT_NOTE "machine-note-edit-popover-select-note"
#define AGS_GSEQUENCER_ACTION_MACHINE_NOTE_EDIT_POPOVER_IMPORT_SMF "machine-note-edit-popover-import-smf"

#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_NOTEBOOK_TOGGLED "machine-automation-edit-notebook-toggled"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_ADD_ACCELERATION "machine-automation-edit-add-acceleration"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_DELETE_ACCELERATION "machine-automation-edit-delete-acceleration"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_SELECT_ACCELERATION "machine-automation-edit-select-acceleration"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_COPY_AUTOMATION "machine-automation-edit-copy-automation"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_CUT_AUTOMATION "machine-automation-edit-cut-automation"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_PASTE_AUTOMATION "machine-automation-edit-paste-automation"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_POPOVER_POSITION_AUTOMATION_CURSOR "machine-automation-edit-popover-position-automation-cursor"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_POPOVER_SELECT_ACCELERATION "machine-automation-edit-popover-select-acceleration"
#define AGS_GSEQUENCER_ACTION_MACHINE_AUTOMATION_EDIT_POPOVER_RAMP_ACCELERATION "machine-automation-edit-popover-ramp-acceleration"

#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_NOTEBOOK_TOGGLED "machine-wave-edit-notebook-toggled"
#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_SELECT_WAVE "machine-wave-edit-select-wave"
#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_COPY_WAVE "machine-wave-edit-copy-wave"
#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_CUT_WAVE "machine-wave-edit-cut-wave"
#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_PASTE_WAVE "machine-wave-edit-paste-wave"
#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_POPOVER_POSITION_WAVE_CURSOR "machine-wave-edit-popover-position-wave-cursor"
#define AGS_GSEQUENCER_ACTION_MACHINE_WAVE_EDIT_POPOVER_SELECT_BUFFER "machine-wave-edit-popover-select-buffer"

#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_ADD_MARKER "machine-tempo-edit-add-marker"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_DELETE_MARKER "machine-tempo-edit-delete-marker"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_SELECT_MARKER "machine-tempo-edit-select-marker"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_COPY_TEMPO "machine-tempo-edit-copy-tempo"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_CUT_TEMPO "machine-tempo-edit-cut-tempo"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_PASTE_TEMPO "machine-tempo-edit-paste-tempo"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_POPOVER_POSITION_TEMPO_CURSOR "machine-tempo-edit-popover-position-tempo-cursor"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_POPOVER_SELECT_MARKER "machine-tempo-edit-popover-select-marker"
#define AGS_GSEQUENCER_ACTION_MACHINE_TEMPO_EDIT_POPOVER_RAMP_MARKER "machine-tempo-edit-popover-ramp-marker"

G_END_DECLS

#endif /*__AGS_GSEQUENCER_ACTIONS__*/
