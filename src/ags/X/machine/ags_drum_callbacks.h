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

#ifndef __AGS_DRUM_CALLBACKS_H__
#define __AGS_DRUM_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/X/machine/ags_drum.h>

void ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum);

void ags_drum_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				       guint attack,
				       AgsDrum *drum);
void ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum);
void ags_drum_run_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_run_delay_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrum *drum);

void ags_drum_tact_callback(GtkWidget *option_menu, AgsDrum *drum);
void ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum);

void ags_drum_index0_callback(GtkWidget *toggle_button, AgsDrum *drum);
void ags_drum_index1_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_pad_callback(GtkWidget *toggle_button, AgsDrum *drum);

void ags_drum_offset_callback(GtkWidget *widget, AgsDrum *drum);

#endif /*__AGS_DRUM_CALLBACKS_H__*/
