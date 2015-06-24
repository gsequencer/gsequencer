/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_LINE_CALLBACKS_H__
#define __AGS_LINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_line.h>

int ags_line_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLine *line);

void ags_line_remove_recall_callback(AgsRecall *recall, AgsLine *line);

int ags_line_group_clicked_callback(GtkWidget *widget, AgsLine *line);

/* AgsLine */
void ags_line_volume_callback(GtkRange *range,
			      AgsLine *line);

/* AgsRecall - recall */
void ags_line_peak_run_post_callback(AgsRecall *peak_channel,
				     AgsLine *line);

void ags_line_copy_pattern_done(AgsRecall *recall,
				AgsLine *line);
void ags_line_copy_pattern_cancel(AgsRecall *recall,
				  AgsLine *line);
void ags_line_recall_volume_done(AgsRecall *recall,
				 AgsLine *line);
void ags_line_recall_volume_cancel(AgsRecall *recall,
				   AgsLine *line);

/* AgsRecall - play */
void ags_line_channel_done_callback(AgsChannel *channel, AgsRecallID *recall_id,
				    AgsLine *line);
void ags_line_play_channel_run_cancel(AgsRecall *recall,
				      AgsLine *line);
void ags_line_play_volume_done(AgsRecall *recall,
			       AgsLine *line);
void ags_line_play_volume_cancel(AgsRecall *recall,
				 AgsLine *line);

#endif /*__AGS_LINE_CALLBACKS_H__*/
