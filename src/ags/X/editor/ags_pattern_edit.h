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

#ifndef __AGS_PATTERN_EDIT_H__
#define __AGS_PATTERN_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_ruler.h>

#define AGS_TYPE_PATTERN_EDIT                (ags_pattern_edit_get_type())
#define AGS_PATTERN_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PATTERN_EDIT, AgsPatternEdit))
#define AGS_PATTERN_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PATTERN_EDIT, AgsPatternEditClass))
#define AGS_IS_PATTERN_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PATTERN_EDIT))
#define AGS_IS_PATTERN_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PATTERN_EDIT))
#define AGS_PATTERN_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PATTERN_EDIT, AgsPatternEditClass))

typedef struct _AgsPatternEdit AgsPatternEdit;
typedef struct _AgsPatternEditClass AgsPatternEditClass;

struct _AgsPatternEdit
{
  GtkTable table;

  guint flags;

  AgsRuler *ruler;
  GtkDrawingArea *drawing_area;
  
  guint width;
  guint height;
  guint map_width;
  guint map_height;

  guint control_height;
  guint control_margin_y;

  guint control_width;

  GtkVScrollbar *vscrollbar;
  GtkHScrollbar *hscrollbar;
};

struct _AgsPatternEditClass
{
  GtkTableClass table;
};

GType ags_pattern_edit_get_type(void);

void ags_pattern_edit_set_map_height(AgsPatternEdit *pattern_edit, guint map_height);

void ags_pattern_edit_reset_vertically(AgsPatternEdit *pattern_edit, guint flags);
void ags_pattern_edit_reset_horizontally(AgsPatternEdit *pattern_edit, guint flags);

void ags_pattern_edit_draw_segment(AgsPatternEdit *pattern_edit, cairo_t *cr);
void ags_pattern_edit_draw_position(AgsPatternEdit *pattern_edit, cairo_t *cr);
void ags_pattern_edit_draw_pattern(AgsPatternEdit *pattern_edit, cairo_t *cr);

void ags_pattern_edit_draw_scroll(AgsPatternEdit *pattern_edit, cairo_t *cr,
				  gdouble position);

AgsPatternEdit* ags_pattern_edit_new();

#endif /*__AGS_PATTERN_EDIT_H__*/
