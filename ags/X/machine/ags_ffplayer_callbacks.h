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

#ifndef __AGS_FFPLAYER_CALLBACKS_H__ 
#define __AGS_FFPLAYER_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/machine/ags_ffplayer.h>

G_BEGIN_DECLS

void ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsFFPlayer *ffplayer);
void ags_ffplayer_destroy_callback(GtkWidget *widget, AgsFFPlayer *ffplayer);

void ags_ffplayer_draw_callback(GtkWidget *drawing_area, cairo_t *cr,
				AgsFFPlayer *ffplayer);

void ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer);
void ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
						AgsMachine *machine);
void ags_ffplayer_preset_changed_callback(GtkComboBox *preset, AgsFFPlayer *ffplayer);
void ags_ffplayer_instrument_changed_callback(GtkComboBox *instrument, AgsFFPlayer *ffplayer);

gboolean ags_ffplayer_drawing_area_button_press_callback(GtkWidget *widget, AgsFFPlayer *ffplayer);

void ags_ffplayer_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsFFPlayer *ffplayer);

void ags_ffplayer_update_callback(GtkWidget *widget, AgsFFPlayer *ffplayer);

G_END_DECLS

#endif /*__AGS_FFPLAYER_CALLBACKS_H__ */
