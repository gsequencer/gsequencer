/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_pattern_edit.h>
#include <ags/X/editor/ags_pattern_edit_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/X/ags_editor.h>

#include <ags/X/editor/ags_pattern_edit.h>

void ags_pattern_edit_class_init(AgsPatternEditClass *pattern_edit);
void ags_pattern_edit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_edit_init(AgsPatternEdit *pattern_edit);
void ags_pattern_edit_connect(AgsConnectable *connectable);
void ags_pattern_edit_disconnect(AgsConnectable *connectable);

void ags_pattern_edit_paint(AgsPatternEdit *pattern_edit);

/**
 * SECTION:ags_pattern_edit
 * @short_description: edit patterns
 * @title: AgsPatternEdit
 * @section_id:
 * @include: ags/X/editor/ags_pattern_edit.h
 *
 * The #AgsPatternEdit lets you edit patterns.
 */

GtkStyle *pattern_edit_style;

GType
ags_pattern_edit_get_type(void)
{
  static GType ags_type_pattern_edit = 0;

  if(!ags_type_pattern_edit){
    static const GTypeInfo ags_pattern_edit_info = {
      sizeof (AgsPatternEditClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_edit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPatternEdit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_edit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_edit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pattern_edit = g_type_register_static(GTK_TYPE_TABLE,
						"AgsPatternEdit\0", &ags_pattern_edit_info,
						0);
    
    g_type_add_interface_static(ags_type_pattern_edit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_pattern_edit);
}

void
ags_pattern_edit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pattern_edit_connect;
  connectable->disconnect = ags_pattern_edit_disconnect;
}

void
ags_pattern_edit_class_init(AgsPatternEditClass *pattern_edit)
{
}

void
ags_pattern_edit_init(AgsPatternEdit *pattern_edit)
{
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);

  pattern_edit->ruler = ags_ruler_new();
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->ruler,
		   0, 1, 0, 1,
		   GTK_FILL|GTK_EXPAND, GTK_FILL,
		   0, 0);

  pattern_edit->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_style((GtkWidget *) pattern_edit->drawing_area, pattern_edit_style);
  gtk_widget_set_events(GTK_WIDGET (pattern_edit->drawing_area), GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_BUTTON_RELEASE_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			);

  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->drawing_area,
		   0, 1, 1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  pattern_edit->width = 0;
  pattern_edit->height = 0;
  pattern_edit->map_width = AGS_PATTERN_EDIT_MAX_CONTROLS * 64;
  pattern_edit->map_height = 78;

  pattern_edit->control_height = 14;
  pattern_edit->control_margin_y = 2;

  pattern_edit->control_width = 16;

  /* GtkScrollbars */
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  pattern_edit->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->vscrollbar,
		   1, 2, 1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 1.0, 1.0, 1.0);
  pattern_edit->hscrollbar = (GtkHScrollbar *) gtk_hscrollbar_new(adjustment);
  gtk_table_attach(GTK_TABLE(pattern_edit), (GtkWidget *) pattern_edit->hscrollbar,
		   0, 1, 2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_pattern_edit_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_pattern_edit_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_set_map_height:
 * @pattern_edit: the #AgsPatternEdit
 * @map_height: the new height
 *
 * Set the map height in pixel.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_set_map_height(AgsPatternEdit *pattern_edit, guint map_height)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_reset_vertically:
 * @pattern_edit: the #AgsPatternEdit
 * @flags: the #AgsPatternEditResetFlags
 *
 * Reset @pattern_edit as configured vertically.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_reset_vertically(AgsPatternEdit *pattern_edit, guint flags)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_reset_horizontally:
 * @pattern_edit: the #AgsPatternEdit
 * @flags: the #AgsPatternEditResetFlags
 *
 * Reset @pattern_edit as configured horizontally.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_reset_horizontally(AgsPatternEdit *pattern_edit, guint flags)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_draw_segment:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 *
 * Draws horizontal and vertical lines.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_segment(AgsPatternEdit *pattern_edit, cairo_t *cr)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_draw_position:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 *
 * Draws the cursor.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_position(AgsPatternEdit *pattern_edit, cairo_t *cr)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_draw_pattern:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 *
 * Draw the #AgsPattern of selected #AgsMachine on @pattern_edit.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_pattern(AgsPatternEdit *pattern_edit, cairo_t *cr)
{
  //TODO:JK: implement me
}

/**
 * ags_pattern_edit_draw_scroll:
 * @pattern_edit: the #AgsPatternEdit
 * @cr: the #cairo_t surface
 * @position: the new position
 *
 * Change visible x-position of @pattern_edit.
 *
 * Since: 0.4
 */
void
ags_pattern_edit_draw_scroll(AgsPatternEdit *pattern_edit, cairo_t *cr,
			     gdouble position)
{
  double x, y;
  double width, height;

  y = 0.0;
  x = (position) - (GTK_RANGE(pattern_edit->hscrollbar)->adjustment->value * pattern_edit->control_current.control_width);

  height = (double) GTK_WIDGET(pattern_edit->drawing_area)->allocation.height;
  width = 3.0;

  /* draw */
  cairo_set_source_rgba(cr, 0.79, 0.0, 1.0, 0.5);
  cairo_rectangle(cr, (double) x, (double) y, (double) width, (double) height);
  cairo_fill(cr);
}

/**
 * ags_pattern_edit_new:
 *
 * Create a new #AgsPatternEdit.
 *
 * Since: 0.4
 */
AgsPatternEdit*
ags_pattern_edit_new()
{
  AgsPatternEdit *pattern_edit;

  pattern_edit = (AgsPatternEdit *) g_object_new(AGS_TYPE_PATTERN_EDIT, NULL);

  return(pattern_edit);
}
