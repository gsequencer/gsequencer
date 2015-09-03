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

#ifndef __AGS_CELL_PATTERN_CALLBACKS_H__
#define __AGS_CELL_PATTERN_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_cell_pattern.h>

/* controls */
gboolean ags_cell_pattern_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsCellPattern *cell_pattern);
gboolean ags_cell_pattern_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsCellPattern *cell_pattern);
gboolean ags_cell_pattern_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsCellPattern *cell_pattern);
void ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern);

#endif /*__AGS_CELL_PATTERN_CALLBACKS_H__*/
