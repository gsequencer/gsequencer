/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/editor/ags_sheet_edit_callbacks.h>

#include <ags/app/ags_composite_editor.h>

void
ags_sheet_edit_draw_callback(GtkWidget *drawing_area,
			     cairo_t *cr,
			     gint width, gint height,
			     AgsSheetEdit *sheet_edit)
{
  ags_sheet_edit_draw(sheet_edit, cr);
}

void
ags_sheet_edit_drawing_area_resize_callback(GtkWidget *drawing_area,
					    gint width, gint height,
					    AgsSheetEdit *sheet_edit)
{  
  gtk_widget_queue_draw((GtkWidget *) sheet_edit);
}
