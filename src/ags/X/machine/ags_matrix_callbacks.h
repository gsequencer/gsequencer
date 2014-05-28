/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_MATRIX_CALLBACKS_H__
#define __AGS_MATRIX_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/X/machine/ags_matrix.h>

void ags_matrix_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMatrix *matrix);

void ags_matrix_run_callback(GtkWidget *widget, AgsMatrix *matrix);

void ags_matrix_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					 guint delay, guint attack,
					 AgsMatrix *matrix);

void ags_matrix_index_callback(GtkWidget *widget, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsMatrix *matrix);
gboolean ags_matrix_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsMatrix *matrix);
void ags_matrix_adjustment_value_changed_callback(GtkWidget *widget, AgsMatrix *matrix);

void ags_matrix_bpm_callback(GtkWidget *spin_button, AgsMatrix *matrix);
void ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix);
void ags_matrix_tact_callback(GtkWidget *combo_box, AgsMatrix *matrix);
void ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix);

void ags_matrix_run_delay_done(AgsRecall *recall, AgsMatrix *matrix);

void ags_matrix_copy_pattern_done(AgsRecall *recall, AgsMatrix *matrix);
void ags_matrix_copy_pattern_cancel(AgsRecall *recall, AgsMatrix *matrix);
void ags_matrix_copy_pattern_loop(AgsRecall *recall, AgsMatrix *matrix);

void ags_matrix_play_done(AgsRecall *recall, AgsMatrix *matrix);
void ags_matrix_play_cancel(AgsRecall *recall, AgsMatrix *matrix);

#endif /*__AGS_MATRIX_CALLBACKS_H__*/
