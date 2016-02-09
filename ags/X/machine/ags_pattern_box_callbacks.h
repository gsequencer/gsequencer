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

#ifndef __AGS_PATTERN_BOX_CALLBACKS_H__
#define __AGS_PATTERN_BOX_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_pattern_box.h>

/* controls */
gboolean ags_pattern_box_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box);
gboolean ags_pattern_box_focus_out_callback(GtkWidget *widget, GdkEvent *event, AgsPatternBox *pattern_box);

gboolean ags_pattern_box_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsPatternBox *pattern_box);
gboolean ags_pattern_box_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsPatternBox *pattern_box);

void ags_pattern_box_pad_callback(GtkWidget *toggle_button, AgsPatternBox *pattern_box);
void ags_pattern_box_offset_callback(GtkWidget *widget, AgsPatternBox *pattern_box);

#endif /*__AGS_PATTERN_BOX_CALLBACKS_H__*/
