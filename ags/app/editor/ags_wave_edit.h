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

#ifndef __AGS_WAVE_EDIT_H__
#define __AGS_WAVE_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_WAVE_EDIT                (ags_wave_edit_get_type())
#define AGS_WAVE_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAVE_EDIT, AgsWaveEdit))
#define AGS_WAVE_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAVE_EDIT, AgsWaveEditClass))
#define AGS_IS_WAVE_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_WAVE_EDIT))
#define AGS_IS_WAVE_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_WAVE_EDIT))
#define AGS_WAVE_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_WAVE_EDIT, AgsWaveEditClass))

#define AGS_WAVE_EDIT_DEFAULT_HEIGHT (512)
#define AGS_WAVE_EDIT_DEFAULT_WIDTH (512)

#define AGS_WAVE_EDIT_DEFAULT_CONTROL_WIDTH (64)
#define AGS_WAVE_EDIT_DEFAULT_CONTROL_HEIGHT (8)

#define AGS_WAVE_EDIT_DEFAULT_STEP_COUNT (16)

#define AGS_WAVE_EDIT_DEFAULT_CURSOR_POSITION_X (0)
#define AGS_WAVE_EDIT_DEFAULT_CURSOR_POSITION_Y (0.0)

#define AGS_WAVE_EDIT_DEFAULT_SELECTED_BUFFER_BORDER (2)

#define AGS_WAVE_EDIT_DEFAULT_FADER_WIDTH (3)

#define AGS_WAVE_EDIT_CURSOR_WIDTH (5)
#define AGS_WAVE_EDIT_CURSOR_HEIGHT (5)

#define AGS_WAVE_EDIT_MIN_ZOOM (1.0 / 16.0)
#define AGS_WAVE_EDIT_MAX_ZOOM (4.0)

#define AGS_WAVE_EDIT_DEFAULT_LOWER (0.0)
#define AGS_WAVE_EDIT_DEFAULT_UPPER (1.0)
#define AGS_WAVE_EDIT_DEFAULT_VALUE (0.0)

#define AGS_WAVE_EDIT_DEFAULT_PADDING (8)

#define AGS_WAVE_EDIT_X_RESOLUTION (16.0 * 64.0)

typedef struct _AgsWaveEdit AgsWaveEdit;
typedef struct _AgsWaveEditClass AgsWaveEditClass;

typedef enum{
  AGS_WAVE_EDIT_AUTO_SCROLL                 = 1,
  AGS_WAVE_EDIT_SHOW_RULER                  = 1 <<  1,
  AGS_WAVE_EDIT_SHOW_VSCROLLBAR             = 1 <<  2,
  AGS_WAVE_EDIT_SHOW_HSCROLLBAR             = 1 <<  3,
  AGS_WAVE_EDIT_BLOCK_RESET_VSCROLLBAR      = 1 <<  4,
  AGS_WAVE_EDIT_BLOCK_RESET_HSCROLLBAR      = 1 <<  5,
}AgsWaveEditFlags;

typedef enum{
  AGS_WAVE_EDIT_NO_EDIT_MODE,
  AGS_WAVE_EDIT_POSITION_CURSOR,
  AGS_WAVE_EDIT_SELECT_BUFFER,
}AgsWaveEditMode;

typedef enum{
  AGS_WAVE_EDIT_BUTTON_1            = 1,
}AgsWaveEditButtonMask;

typedef enum{
  AGS_WAVE_EDIT_KEY_L_CONTROL       = 1,
  AGS_WAVE_EDIT_KEY_R_CONTROL       = 1 <<  1,
  AGS_WAVE_EDIT_KEY_L_SHIFT         = 1 <<  2,
  AGS_WAVE_EDIT_KEY_R_SHIFT         = 1 <<  3,
  AGS_WAVE_EDIT_KEY_L_META          = 1 <<  4,
  AGS_WAVE_EDIT_KEY_R_META          = 1 <<  5,
}AgsWaveEditKeyMask;

struct _AgsWaveEdit
{
  GtkGrid grid;

  guint flags;
  guint connectable_flags;
  guint mode;

  guint button_mask;
  guint key_mask;

  guint line;
  
  guint note_offset;
  guint note_offset_absolute;

  guint control_width;
  guint control_height;

  guint step_count;

  guint cursor_position_x;
  gdouble cursor_position_y;

  guint selected_buffer_border;
  
  guint selection_x0;
  guint selection_x1;
  guint selection_y0;
  guint selection_y1;
  
  AgsRuler *ruler;

  gdouble lower;
  gdouble upper;
  gdouble default_value;

  GtkDrawingArea *drawing_area;
  unsigned char *wave_data;
  int stride;
  
  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;
};

struct _AgsWaveEditClass
{
  GtkGridClass grid;
};

GType ags_wave_edit_get_type(void);

void ags_wave_edit_reset_vscrollbar(AgsWaveEdit *wave_edit);
void ags_wave_edit_reset_hscrollbar(AgsWaveEdit *wave_edit);

void ags_wave_edit_draw_segment(AgsWaveEdit *wave_edit, cairo_t *cr);
void ags_wave_edit_draw_position(AgsWaveEdit *wave_edit, cairo_t *cr);

void ags_wave_edit_draw_cursor(AgsWaveEdit *wave_edit, cairo_t *cr);
void ags_wave_edit_draw_selection(AgsWaveEdit *wave_edit, cairo_t *cr);

void ags_wave_edit_draw_buffer(AgsWaveEdit *wave_edit,
			       AgsBuffer *buffer,
			       cairo_t *cr,
			       gdouble bpm,
			       gdouble opacity);
void ags_wave_edit_draw_wave(AgsWaveEdit *wave_edit, cairo_t *cr);

void ags_wave_edit_draw(AgsWaveEdit *wave_edit, cairo_t *cr);

AgsWaveEdit* ags_wave_edit_new(guint line);

G_END_DECLS

#endif /*__AGS_WAVE_EDIT_H__*/
