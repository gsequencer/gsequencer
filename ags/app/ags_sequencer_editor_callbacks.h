/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_SOUNCARD_EDITOR_CALLBACKS_H__
#define __AGS_SOUNCARD_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_sequencer_editor.h>

G_BEGIN_DECLS

void ags_sequencer_editor_backend_changed_callback(GtkComboBox *combo,
						   AgsSequencerEditor *sequencer_editor);

void ags_sequencer_editor_card_changed_callback(GtkComboBox *combo,
						AgsSequencerEditor *sequencer_editor);

void ags_sequencer_editor_add_source_callback(GtkWidget *button,
					      AgsSequencerEditor *sequencer_editor);
void ags_sequencer_editor_remove_source_callback(GtkWidget *button,
						 AgsSequencerEditor *sequencer_editor);

G_END_DECLS

#endif /*__AGS_SEQUENCER_EDITOR_CALLBACKS_H__*/
