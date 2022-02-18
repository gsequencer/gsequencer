/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_DESK_PAD_CALLBACKS_H__
#define __AGS_DESK_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_desk.h>
#include <ags/app/machine/ags_desk_pad.h>

void ags_desk_pad_play_callback(GtkButton *button, AgsDeskPad *desk_pad);
void ags_desk_pad_grab_filename_callback(GtkButton *button, AgsDeskPad *desk_pad);

void ags_desk_pad_volume_callback(GtkRange *scale, AgsDeskPad *desk_pad);

void ags_desk_pad_move_up_callback(GtkButton *button, AgsDeskPad *desk_pad);
void ags_desk_pad_move_down_callback(GtkButton *button, AgsDeskPad *desk_pad);
void ags_desk_pad_add_callback(GtkButton *button, AgsDeskPad *desk_pad);
void ags_desk_pad_remove_callback(GtkButton *button, AgsDeskPad *desk_pad);

#endif /*__AGS_DESK_PAD_CALLBACKS_H__*/
