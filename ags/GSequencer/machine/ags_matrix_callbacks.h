/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_MATRIX_CALLBACKS_H__
#define __AGS_MATRIX_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/machine/ags_matrix.h>

G_BEGIN_DECLS

void ags_matrix_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsMatrix *matrix);

/* controls */
void ags_matrix_index_callback(GtkWidget *widget, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsMatrix *matrix);
void ags_matrix_adjustment_value_changed_callback(GtkWidget *widget, AgsMatrix *matrix);

void ags_matrix_bpm_callback(GtkWidget *spin_button, AgsMatrix *matrix);
void ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix);
void ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix);

/* forwarded callbacks */
void ags_matrix_stop_callback(AgsMatrix *matrix,
			      GList *recall_id, gint sound_scope,
			      gpointer data);

void ags_matrix_volume_callback(GtkRange *range, AgsMatrix *matrix);

G_END_DECLS

#endif /*__AGS_MATRIX_CALLBACKS_H__*/
