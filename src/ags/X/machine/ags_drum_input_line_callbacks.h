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

#ifndef __AGS_DRUM_INPUT_LINE_CALLBACKS_H__
#define __AGS_DRUM_INPUT_LINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_drum_input_line.h>

#include <ags/audio/ags_audio.h>

void ags_drum_input_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrumInputLine *drum_input_line);

/* AgsAudio */
void ags_drum_input_line_audio_set_pads_callback(AgsAudio *audio, GType type,
						 guint pads, guint pads_old,
						 AgsDrumInputLine *drum_input_line);

/* AgsRecall - recall */
void ags_drum_input_line_copy_pattern_done(AgsRecall *recall,
					   AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_copy_pattern_cancel(AgsRecall *recall,
					     AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_recall_volume_done(AgsRecall *recall,
					    AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_recall_volume_cancel(AgsRecall *recall,
					      AgsDrumInputLine *drum_input_line);

/* AgsRecall - play */
void ags_drum_input_line_channel_done_callback(AgsChannel *channel,
					       AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_play_channel_run_cancel(AgsRecall *recall,
						 AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_play_volume_done(AgsRecall *recall,
					  AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_play_volume_cancel(AgsRecall *recall,
					    AgsDrumInputLine *drum_input_line);

#endif /*__AGS_DRUM_INPUT_LINE_CALLBACKS_H__*/
