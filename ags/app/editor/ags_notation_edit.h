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

#ifndef __AGS_NOTATION_EDIT_H__
#define __AGS_NOTATION_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_NOTATION_EDIT                (ags_notation_edit_get_type ())
#define AGS_NOTATION_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION_EDIT, AgsNotationEdit))
#define AGS_NOTATION_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION_EDIT, AgsNotationEditClass))
#define AGS_IS_NOTATION_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTATION_EDIT))
#define AGS_IS_NOTATION_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTATION_EDIT))
#define AGS_NOTATION_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_NOTATION_EDIT, AgsNotationEditClass))

#define AGS_NOTATION_EDIT_DEFAULT_CONTROL_WIDTH (64)
#define AGS_NOTATION_EDIT_DEFAULT_CONTROL_HEIGHT (14)

#define AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_X (0)
#define AGS_NOTATION_EDIT_DEFAULT_CONTROL_MARGIN_Y (2)

#define AGS_NOTATION_EDIT_DEFAULT_KEY_COUNT (128)

#define AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_X (0)
#define AGS_NOTATION_EDIT_DEFAULT_CURSOR_POSITION_Y (0)

#define AGS_NOTATION_EDIT_DEFAULT_SELECTED_NOTE_BORDER (2)

#define AGS_NOTATION_EDIT_DEFAULT_FADER_WIDTH (3)

#define AGS_NOTATION_EDIT_MIN_ZOOM (1.0 / 16.0)
#define AGS_NOTATION_EDIT_MAX_ZOOM (4.0)
#define AGS_NOTATION_EDIT_MAX_ZOOM_CONTROL_WIDTH (64.0 * AGS_NOTATION_EDIT_DEFAULT_CONTROL_WIDTH)

typedef struct _AgsNotationEdit AgsNotationEdit;
typedef struct _AgsNotationEditClass AgsNotationEditClass;

typedef enum{
  AGS_NOTATION_EDIT_CONNECTED                  = 1,
  AGS_NOTATION_EDIT_AUTO_SCROLL                = 1 <<  1,
  AGS_NOTATION_EDIT_SHOW_RULER                 = 1 <<  2,
  AGS_NOTATION_EDIT_SHOW_VSCROLLBAR            = 1 <<  3,
  AGS_NOTATION_EDIT_SHOW_HSCROLLBAR            = 1 <<  4,
  AGS_NOTATION_EDIT_BLOCK_RESET_VSCROLLBAR     = 1 <<  5,
  AGS_NOTATION_EDIT_BLOCK_RESET_HSCROLLBAR     = 1 <<  6,
}AgsNotationEditFlags;

typedef enum{
  AGS_NOTATION_EDIT_NO_EDIT_MODE,
  AGS_NOTATION_EDIT_POSITION_CURSOR,
  AGS_NOTATION_EDIT_ADD_NOTE,
  AGS_NOTATION_EDIT_DELETE_NOTE,
  AGS_NOTATION_EDIT_SELECT_NOTE,
}AgsNotationEditMode;

typedef enum{
  AGS_NOTATION_EDIT_BUTTON_1            = 1,
}AgsNotationEditButtonMask;

typedef enum{
  AGS_NOTATION_EDIT_KEY_L_CONTROL       = 1,
  AGS_NOTATION_EDIT_KEY_R_CONTROL       = 1 <<  1,
  AGS_NOTATION_EDIT_KEY_L_SHIFT         = 1 <<  2,
  AGS_NOTATION_EDIT_KEY_R_SHIFT         = 1 <<  3,
  AGS_NOTATION_EDIT_KEY_L_META          = 1 <<  4,
  AGS_NOTATION_EDIT_KEY_R_META          = 1 <<  5,
}AgsNotationEditKeyMask;

struct _AgsNotationEdit
{
  GtkGrid grid;

  guint flags;
  guint connectable_flags;
  guint mode;

  guint button_mask;
  guint key_mask;

  guint note_offset;
  guint note_offset_absolute;
  
  guint control_width;
  guint control_height;

  guint control_margin_x;
  guint control_margin_y;

  guint key_count;
  
  guint cursor_position_x;
  guint cursor_position_y;

  guint selected_note_border;
  
  guint selection_x0;
  guint selection_x1;
  guint selection_y0;
  guint selection_y1;

  AgsNote *current_note;
  
  AgsRuler *ruler;

  GtkDrawingArea *drawing_area;

  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;

  GtkEventController *key_controller;
  GtkEventController *gesture_controller;
  GtkEventController *motion_controller;
};

struct _AgsNotationEditClass
{
  GtkGridClass grid;
};

GType ags_notation_edit_get_type(void);

void ags_notation_edit_reset_vscrollbar(AgsNotationEdit *notation_edit);
void ags_notation_edit_reset_hscrollbar(AgsNotationEdit *notation_edit);

void ags_notation_edit_draw_segment(AgsNotationEdit *notation_edit, cairo_t *cr);
void ags_notation_edit_draw_position(AgsNotationEdit *notation_edit, cairo_t *cr);

void ags_notation_edit_draw_cursor(AgsNotationEdit *notation_edit, cairo_t *cr);
void ags_notation_edit_draw_selection(AgsNotationEdit *notation_edit, cairo_t *cr);

void ags_notation_edit_draw_note(AgsNotationEdit *notation_edit,
				 AgsNote *note,
				 cairo_t *cr,
				 gdouble opacity);
void ags_notation_edit_draw_notation(AgsNotationEdit *notation_edit, cairo_t *cr);

void ags_notation_edit_draw(AgsNotationEdit *notation_edit, cairo_t *cr);

AgsNotationEdit* ags_notation_edit_new();

G_END_DECLS

#endif /*__AGS_NOTATION_EDIT_H__*/
