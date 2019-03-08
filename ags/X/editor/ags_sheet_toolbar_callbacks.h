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

#ifndef __AGS_SHEET_TOOLBAR_CALLBACKS_H__
#define __AGS_SHEET_TOOLBAR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/editor/ags_sheet_toolbar.h>

void ags_sheet_toolbar_position_callback(GtkToggleToolButton *toggle_button, AgsSheetToolbar *sheet_toolbar);

void ags_sheet_toolbar_clear_callback(GtkToggleToolButton *toggle_button, AgsSheetToolbar *sheet_toolbar);
void ags_sheet_toolbar_select_callback(GtkToggleToolButton *toggle_button, AgsSheetToolbar *sheet_toolbar);

void ags_sheet_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsSheetToolbar *sheet_toolbar);
void ags_sheet_toolbar_paste_callback(GtkWidget *widget, AgsSheetToolbar *sheet_toolbar);

#endif /*__AGS_SHEET_TOOLBAR_CALLBACKS_H__*/
