/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_WAVE_EDIT_H__
#define __AGS_WAVE_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_ruler.h>

#include <ags/X/editor/ags_level.h>

#define AGS_TYPE_WAVE_EDIT                (ags_wave_edit_get_type())
#define AGS_WAVE_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EDIT, AgsWaveEdit))
#define AGS_WAVE_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EDIT, AgsWaveEditClass))
#define AGS_IS_WAVE_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_EDIT))
#define AGS_IS_WAVE_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_EDIT))
#define AGS_WAVE_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_EDIT, AgsWaveEditClass))

#define AGS_WAVE_EDIT_MAX_CONTROLS (16 * 64 * 1200)
#define AGS_WAVE_EDIT_DEFAULT_MARGIN (8)
#define AGS_WAVE_EDIT_DEFAULT_WIDTH (64)

typedef struct _AgsWaveEdit AgsWaveEdit;
typedef struct _AgsWaveEditClass AgsWaveEditClass;

typedef enum{
  AGS_WAVE_EDIT_CONNECTED                   = 1,
  AGS_WAVE_EDIT_RESETING_VERTICALLY         = 1 <<  1,
  AGS_WAVE_EDIT_RESETING_HORIZONTALLY       = 1 <<  2,
  AGS_WAVE_EDIT_POSITION_CURSOR             = 1 <<  3,
  AGS_WAVE_EDIT_SELECTING_AUDIO_DATA        = 1 <<  4,
}AgsWaveEditFlags;

typedef enum{
  AGS_WAVE_EDIT_RESET_VSCROLLBAR   = 1,
  AGS_WAVE_EDIT_RESET_HSCROLLBAR   = 1 <<  1,
  AGS_WAVE_EDIT_RESET_WIDTH        = 1 <<  2,
  AGS_WAVE_EDIT_RESET_HEIGHT       = 1 <<  3, // reserved
}AgsWaveEditResetFlags;

typedef enum{
  AGS_WAVE_EDIT_KEY_L_CONTROL       = 1,
  AGS_WAVE_EDIT_KEY_R_CONTROL       = 1 <<  1,
  AGS_WAVE_EDIT_KEY_L_SHIFT         = 1 <<  2,
  AGS_WAVE_EDIT_KEY_R_SHIFT         = 1 <<  3,
}AgsWaveEditKeyMask;

struct _AgsWaveEdit
{
  GtkTable table;

  guint flags;

  guint key_mask;
  
  guint map_width;
  guint map_height;

  guint edit_x;
  guint edit_y;
  
  guint select_x0;
  guint select_y0;
  guint select_x1;
  guint select_y1;
  
  AgsRuler *ruler;

  AgsLevel *level;
  GtkDrawingArea *drawing_area;
  
  GtkVScrollbar *vscrollbar;
  GtkHScrollbar *hscrollbar;
};

struct _AgsWaveEditClass
{
  GtkTableClass table;
};

GType ags_wave_edit_get_type(void);

void ags_wave_edit_reset_vertically(AgsWaveEdit *wave_edit, guint flags);
void ags_wave_edit_reset_horizontally(AgsWaveEdit *wave_edit, guint flags);

void ags_wave_edit_draw_position(AgsWaveEdit *wave_edit, cairo_t *cr);
void ags_wave_edit_draw_scroll(AgsWaveEdit *wave_edit, cairo_t *cr,
			       gdouble position);

void ags_wave_edit_paint(AgsWaveEdit *wave_edit,
			 cairo_t *cr);

AgsWaveEdit* ags_wave_edit_new();

#endif /*__AGS_WAVE_EDIT_H__*/
