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

#ifndef __AGS_PAD_CALLBACKS_H__
#define __AGS_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_pad.h>

#include <ags/thread/ags_task.h>
#include <ags/thread/ags_task_completion.h>

int ags_pad_group_clicked_callback(GtkWidget *widget, AgsPad *pad);
int ags_pad_mute_clicked_callback(GtkWidget *widget, AgsPad *pad);
int ags_pad_solo_clicked_callback(GtkWidget *widget, AgsPad *pad);

void ags_pad_start_complete_callback(AgsTaskCompletion *task_completion,
				     AgsPad *pad);
void ags_pad_init_channel_launch_callback(AgsTask *task, AgsPad *input_pad);

#endif /*__AGS_PAD_CALLBACKS_H__*/
