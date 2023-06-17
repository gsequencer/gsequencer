/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_TEMPO_EDIT_H__
#define __AGS_TEMPO_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_TEMPO_EDIT                (ags_tempo_edit_get_type ())
#define AGS_TEMPO_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TEMPO_EDIT, AgsTempoEdit))
#define AGS_TEMPO_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TEMPO_EDIT, AgsTempoEditClass))
#define AGS_IS_TEMPO_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TEMPO_EDIT))
#define AGS_IS_TEMPO_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TEMPO_EDIT))
#define AGS_TEMPO_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_TEMPO_EDIT, AgsTempoEditClass))

#define AGS_TEMPO_EDIT_DEFAULT_HEIGHT (128)
#define AGS_TEMPO_EDIT_DEFAULT_WIDTH (512)

#define AGS_TEMPO_EDIT_DEFAULT_POINT_RADIUS (1.75)

#define AGS_TEMPO_EDIT_DEFAULT_SCAN_HEIGHT (8)
#define AGS_TEMPO_EDIT_DEFAULT_SCAN_WIDTH (64)

#define AGS_TEMPO_EDIT_DEFAULT_CONTROL_WIDTH (64)
#define AGS_TEMPO_EDIT_DEFAULT_CONTROL_HEIGHT (8)

#define AGS_TEMPO_EDIT_DEFAULT_STEP_COUNT (16)

#define AGS_TEMPO_EDIT_DEFAULT_CURSOR_POSITION_X (0)
#define AGS_TEMPO_EDIT_DEFAULT_CURSOR_POSITION_Y (0.0)

#define AGS_TEMPO_EDIT_DEFAULT_SELECTED_MARKER_BORDER (2)

#define AGS_TEMPO_EDIT_DEFAULT_FADER_WIDTH (3)

#define AGS_TEMPO_EDIT_CURSOR_WIDTH (5)
#define AGS_TEMPO_EDIT_CURSOR_HEIGHT (5)

#define AGS_TEMPO_EDIT_MIN_ZOOM (1.0 / 16.0)
#define AGS_TEMPO_EDIT_MAX_ZOOM (4.0)

typedef struct _AgsTempoEdit AgsTempoEdit;
typedef struct _AgsTempoEditClass AgsTempoEditClass;

typedef enum{
  AGS_TEMPO_EDIT_CONNECTED                  = 1,
  AGS_TEMPO_EDIT_AUTO_SCROLL                = 1 <<  1,
  AGS_TEMPO_EDIT_SHOW_RULER                 = 1 <<  2,
  AGS_TEMPO_EDIT_SHOW_VSCROLLBAR            = 1 <<  3,
  AGS_TEMPO_EDIT_SHOW_HSCROLLBAR            = 1 <<  4,
  AGS_TEMPO_EDIT_BLOCK_RESET_VSCROLLBAR     = 1 <<  5,
  AGS_TEMPO_EDIT_BLOCK_RESET_HSCROLLBAR     = 1 <<  6,
}AgsTempoEditFlags;

typedef enum{
  AGS_TEMPO_EDIT_NO_EDIT_MODE,
  AGS_TEMPO_EDIT_POSITION_CURSOR,
  AGS_TEMPO_EDIT_ADD_MARKER,
  AGS_TEMPO_EDIT_DELETE_MARKER,
  AGS_TEMPO_EDIT_SELECT_MARKER,
}AgsTempoEditMode;

typedef enum{
  AGS_TEMPO_EDIT_BUTTON_1            = 1,
}AgsTempoEditButtonMask;

typedef enum{
  AGS_TEMPO_EDIT_KEY_L_CONTROL       = 1,
  AGS_TEMPO_EDIT_KEY_R_CONTROL       = 1 <<  1,
  AGS_TEMPO_EDIT_KEY_L_SHIFT         = 1 <<  2,
  AGS_TEMPO_EDIT_KEY_R_SHIFT         = 1 <<  3,
  AGS_TEMPO_EDIT_KEY_L_META          = 1 <<  4,
  AGS_TEMPO_EDIT_KEY_R_META          = 1 <<  5,
}AgsTempoEditKeyMask;

struct _AgsTempoEdit
{
  GtkGrid grid;

  guint flags;
  guint connectable_flags;
  guint mode;

  guint button_mask;
  guint key_mask;

  guint note_offset;
  guint note_offset_absolute;

  guint point_radius;
  
  guint scan_width;
  guint scan_height;

  guint control_width;
  guint control_height;

  guint step_count;
  
  guint cursor_position_x;
  gdouble cursor_position_y;

  guint selected_marker_border;
  
  guint selection_x0;
  guint selection_x1;
  guint selection_y0;
  guint selection_y1;

  AgsMarker *current_marker;

  AgsRuler *ruler;

  GtkDrawingArea *drawing_area;

  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;

  GtkEventController *key_controller;
  GtkEventController *gesture_controller;
  GtkEventController *motion_controller;
};

struct _AgsTempoEditClass
{
  GtkGridClass grid;
};

GType ags_tempo_edit_get_type(void);

void ags_tempo_edit_reset_vscrollbar(AgsTempoEdit *tempo_edit);
void ags_tempo_edit_reset_hscrollbar(AgsTempoEdit *tempo_edit);

/*  */
gint ags_tempo_edit_compare_x_offset_func(gconstpointer a,
					  gconstpointer b,
					  AgsTempoEdit *tempo_edit,
					  gdouble x_offset);

GList* ags_tempo_edit_find_first_drawn_func(AgsTempoEdit *tempo_edit,
					    GList *tempo);
GList* ags_tempo_edit_find_last_drawn_func(AgsTempoEdit *tempo_edit,
					   GList *tempo);

/*  */
void ags_tempo_edit_draw_segment(AgsTempoEdit *tempo_edit, cairo_t *cr);
void ags_tempo_edit_draw_position(AgsTempoEdit *tempo_edit, cairo_t *cr);

void ags_tempo_edit_draw_cursor(AgsTempoEdit *tempo_edit, cairo_t *cr);
void ags_tempo_edit_draw_selection(AgsTempoEdit *tempo_edit, cairo_t *cr);

void ags_tempo_edit_draw_marker(AgsTempoEdit *tempo_edit,
				AgsMarker *marker_a, AgsMarker *marker_b,
				cairo_t *cr,
				gdouble opacity);
void ags_tempo_edit_draw_tempo(AgsTempoEdit *tempo_edit, cairo_t *cr);

void ags_tempo_edit_draw(AgsTempoEdit *tempo_edit, cairo_t *cr);

/*  */
AgsTempoEdit* ags_tempo_edit_new();

G_END_DECLS

#endif /*__AGS_TEMPO_EDIT_H__*/
