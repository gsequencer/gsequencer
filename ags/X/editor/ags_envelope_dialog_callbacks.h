/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_ENVELOPE_DIALOG_CALLBACKS_H__
#define __AGS_ENVELOPE_DIALOG_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_envelope_dialog.h>

void ags_envelope_dialog_attack_callback(GtkButton *button, AgsEnvelopeDialog *envelope_dialog);
void ags_envelope_dialog_decay_callback(GtkButton *button, AgsEnvelopeDialog *envelope_dialog);
void ags_envelope_dialog_sustain_callback(GtkButton *button, AgsEnvelopeDialog *envelope_dialog);
void ags_envelope_dialog_release_callback(GtkButton *button, AgsEnvelopeDialog *envelope_dialog);

void ags_envelope_dialog_ratio_callback(GtkButton *button, AgsEnvelopeDialog *envelope_dialog);

int ags_envelope_dialog_apply_callback(GtkWidget *widget, AgsEnvelopeDialog *envelope_dialog);
int ags_envelope_dialog_ok_callback(GtkWidget *widget, AgsEnvelopeDialog *envelope_dialog);
int ags_envelope_dialog_cancel_callback(GtkWidget *widget, AgsEnvelopeDialog *envelope_dialog);

#endif /*__AGS_ENVELOPE_DIALOG_CALLBACKS_H__*/
