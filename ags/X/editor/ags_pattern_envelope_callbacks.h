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

#ifndef __AGS_PATTERN_ENVELOPE_CALLBACKS_H__
#define __AGS_PATTERN_ENVELOPE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/editor/ags_pattern_envelope.h>

void ags_pattern_envelope_edit_callback(GtkCellRendererToggle *cell_renderer,
					gchar *path_str,
					AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_plot_callback(GtkCellRendererToggle *cell_renderer,
					gchar *path_str,
					AgsPatternEnvelope *pattern_envelope);

void ags_pattern_envelope_preset_move_up_callback(GtkWidget *button,
						  AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_preset_move_down_callback(GtkWidget *button,
						    AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_preset_add_callback(GtkWidget *button,
					      AgsPatternEnvelope *pattern_envelope);
void ags_pattern_envelope_preset_remove_callback(GtkWidget *button,
						 AgsPatternEnvelope *pattern_envelope);

int ags_pattern_envelope_preset_rename_response_callback(GtkWidget *widget, gint response,
							 AgsPatternEnvelope *pattern_envelope);

#endif /*__AGS_PATTERN_ENVELOPE_CALLBACKS_H__*/
