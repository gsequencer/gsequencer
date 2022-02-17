/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
#ifndef __AGS_DRUM_INPUT_PAD_CALLBACKS_H__
#define __AGS_DRUM_INPUT_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/machine/ags_drum_input_pad.h>

G_BEGIN_DECLS

void ags_drum_input_pad_open_callback(GtkWidget *button, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_play_callback(GtkToggleButton *button, AgsDrumInputPad *drum_input_pad);
void ags_drum_input_pad_edit_callback(GtkWidget *toggle_button, AgsDrumInputPad *drum_input_pad);

G_END_DECLS

#endif /*__AGS_DRUM_INPUT_PAD_CALLBACKS_H__*/
