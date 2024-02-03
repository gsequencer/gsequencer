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

#ifndef __AGS_ENVELOPE_EDITOR_CALLBACKS_H__
#define __AGS_ENVELOPE_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/editor/ags_envelope_editor.h>

G_BEGIN_DECLS

void ags_envelope_editor_preset_callback(GtkWidget *combo_box,
					 AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_preset_add_callback(GtkWidget *button,
					     AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_preset_remove_callback(GtkWidget *button,
						AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_preset_rename_response_callback(GtkWidget *widget, gint response,
							 AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_attack_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_attack_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_decay_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_decay_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_sustain_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_sustain_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_release_x_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);
void ags_envelope_editor_release_y_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);

void ags_envelope_editor_ratio_callback(GtkWidget *dial, AgsEnvelopeEditor *envelope_editor);

G_END_DECLS

#endif /*__AGS_ENVELOPE_EDITOR_CALLBACKS_H__*/
