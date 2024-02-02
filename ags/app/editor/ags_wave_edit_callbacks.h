/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_WAVE_EDIT_CALLBACKS_H__
#define __AGS_WAVE_EDIT_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/editor/ags_wave_edit.h>

G_BEGIN_DECLS

void ags_wave_edit_update_ui_callback(GObject *ui_provider,
				      AgsWaveEdit *wave_edit);

void ags_wave_edit_draw_callback(GtkWidget *drawing_area,
				 cairo_t *cr,
				 gint width, gint height,
				 AgsWaveEdit *wave_edit);

void ags_wave_edit_drawing_area_resize_callback(GtkWidget *drawing_area,
						gint width, gint height,
						AgsWaveEdit *wave_edit);

void ags_wave_edit_vscrollbar_value_changed(GtkAdjustment *adjustment, AgsWaveEdit *wave_edit);
void ags_wave_edit_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsWaveEdit *wave_edit);

G_END_DECLS

#endif /*__AGS_WAVE_EDIT_CALLBACKS_H__*/
