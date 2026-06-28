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

#ifndef __AGS_GSEQUENCER_ACTION_UTIL__
#define __AGS_GSEQUENCER_ACTION_UTIL__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSEQUENCER_ACTION_UTIL                (ags_gsequencer_action_util_get_type())
#define AGS_GSEQUENCER_ACTION_UTIL(ptr) ((AgsGsequencerActionUtil *)(ptr))

typedef struct _AgsGsequencerActionUtil AgsGsequencerActionUtil;

struct _AgsGsequencerActionUtil
{
};

GType ags_gsequencer_action_util_get_type(void);

AgsGsequencerActionUtil* ags_gsequencer_action_util_alloc();

gpointer ags_gsequencer_action_util_copy(AgsGsequencerActionUtil *ptr);
void ags_gsequencer_action_util_free(AgsGsequencerActionUtil *ptr);

void ags_gsequencer_action_util_redo_machine_button_clicked(AgsGsequencerActionUtil *action_util,
							    AgsGSequencerAction *action);
void ags_gsequencer_action_util_undo_machine_button_clicked(AgsGsequencerActionUtil *action_util,
							    AgsGSequencerAction *action);

void ags_gsequencer_action_util_redo_machine_check_button_toggle(AgsGsequencerActionUtil *action_util,
								 AgsGSequencerAction *action);
void ags_gsequencer_action_util_undo_machine_check_button_toggle(AgsGsequencerActionUtil *action_util,
								 AgsGSequencerAction *action);

void ags_gsequencer_action_util_redo_machine_radio_button_toggle(AgsGsequencerActionUtil *action_util,
								 AgsGSequencerAction *action);
void ags_gsequencer_action_util_undo_machine_radio_button_toggle(AgsGsequencerActionUtil *action_util,
								 AgsGSequencerAction *action);

void ags_gsequencer_action_util_redo_machine_range_value_change(AgsGsequencerActionUtil *action_util,
								AgsGSequencerAction *action);
void ags_gsequencer_action_util_undo_machine_range_value_change(AgsGsequencerActionUtil *action_util,
								AgsGSequencerAction *action);

void ags_gsequencer_action_util_redo_machine_spin_button_value_change(AgsGsequencerActionUtil *action_util,
								      AgsGSequencerAction *action);
void ags_gsequencer_action_util_undo_machine_spin_button_value_change(AgsGsequencerActionUtil *action_util,
								      AgsGSequencerAction *action);

G_END_DECLS

#endif /*__AGS_GSEQUENCER_ACTION_UTIL__*/
