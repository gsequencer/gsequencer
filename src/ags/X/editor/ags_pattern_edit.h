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

#ifndef __AGS_PATTERN_EDIT_H__
#define __AGS_PATTERN_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_ruler.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

#define AGS_TYPE_PATTERN_EDIT                (ags_pattern_edit_get_type())
#define AGS_PATTERN_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PATTERN_EDIT, AgsPatternEdit))
#define AGS_PATTERN_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PATTERN_EDIT, AgsPatternEditClass))
#define AGS_IS_PATTERN_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PATTERN_EDIT))
#define AGS_IS_PATTERN_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PATTERN_EDIT))
#define AGS_PATTERN_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PATTERN_EDIT, AgsPatternEditClass))

#define AGS_PATTERN_EDIT_MAX_CONTROLS 1200

typedef struct _AgsPatternEdit AgsPatternEdit;
typedef struct _AgsPatternEditClass AgsPatternEditClass;

typedef enum{
  AGS_PATTERN_EDIT_RESETING_VERTICALLY    = 1,
  AGS_PATTERN_EDIT_RESETING_HORIZONTALLY  = 1 <<  1,
  AGS_PATTERN_EDIT_POSITION_CURSOR        = 1 <<  2,
  AGS_PATTERN_EDIT_ADDING_NOTE            = 1 <<  3,
  AGS_PATTERN_EDIT_DELETING_NOTE          = 1 <<  4,
  AGS_PATTERN_EDIT_SELECTING_NOTES        = 1 <<  5,
}AgsPatternEditFlags;

typedef enum{
  AGS_PATTERN_EDIT_RESET_VSCROLLBAR   = 1,
  AGS_PATTERN_EDIT_RESET_HSCROLLBAR   = 1 <<  1,
  AGS_PATTERN_EDIT_RESET_WIDTH        = 1 <<  2,
  AGS_PATTERN_EDIT_RESET_HEIGHT       = 1 <<  3, // reserved
}AgsPatternEditResetFlags;

typedef enum{
  AGS_PATTERN_EDIT_KEY_L_CONTROL       = 1,
  AGS_PATTERN_EDIT_KEY_R_CONTROL       = 1 <<  1,
}AgsPatternEditKeyMask;

struct _AgsPatternEdit
{
  GtkTable table;

  guint flags;

  guint key_mask;

  AgsRuler *ruler;
  GtkDrawingArea *drawing_area;

  struct _AgsPatternEditControl{ // values retrieved by mouse pressed and released callback
    AgsNote *note;

    guint x0_offset;
    guint y0_offset;
    guint x0;
    guint y0;

    guint x1_offset;
    guint y1_offset;
    guint x1;
    guint y1;
  }control;

  guint width;
  guint height;
  guint map_width;
  guint map_height;

  guint control_height;
  guint control_margin_y;

  guint control_width;

  guint y0;
  guint y1;

  guint nth_y;
  guint stop_y;

  struct _AgsPatternEditControlCurrent{ // values for drawing refering to current tic and zoom
    guint control_count;

    guint control_width;

    guint x0;
    guint x1;

    guint nth_x;
  }control_current;

  struct _AgsPatternEditControlUnit{ // values for drawing refering to smallest tic and current zoom
    guint control_count;

    guint control_width;

    guint x0;
    guint x1;

    guint nth_x;
    guint stop_x;
  }control_unit;

  guint selected_x;
  guint selected_y;
  
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
void ags_pattern_edit_draw_notation(AgsPatternEdit *pattern_edit, cairo_t *cr);

void ags_pattern_edit_draw_scroll(AgsPatternEdit *pattern_edit, cairo_t *cr,
				  gdouble position);

AgsPatternEdit* ags_pattern_edit_new();

#endif /*__AGS_PATTERN_EDIT_H__*/
