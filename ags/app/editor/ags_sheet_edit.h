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

#ifndef __AGS_SHEET_EDIT_H__
#define __AGS_SHEET_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_SHEET_EDIT                (ags_sheet_edit_get_type ())
#define AGS_SHEET_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SHEET_EDIT, AgsSheetEdit))
#define AGS_SHEET_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SHEET_EDIT, AgsSheetEditClass))
#define AGS_IS_SHEET_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SHEET_EDIT))
#define AGS_IS_SHEET_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SHEET_EDIT))
#define AGS_SHEET_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_SHEET_EDIT, AgsSheetEditClass))

#define AGS_SHEET_EDIT_DEFAULT_PAPER_NAME (GTK_PAPER_NAME_LETTER)
#define AGS_SHEET_EDIT_DEFAULT_WIDTH (612)
#define AGS_SHEET_EDIT_DEFAULT_HEIGHT (792)

#define AGS_SHEET_EDIT_DEFAULT_SPACING (6)

#define AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_X (0)
#define AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_Y (0)

#define AGS_SHEET_EDIT_DEFAULT_FADER_WIDTH (3)

typedef struct _AgsSheetEdit AgsSheetEdit;
typedef struct _AgsSheetEditClass AgsSheetEditClass;

typedef enum{
  AGS_SHEET_EDIT_CONNECTED             = 1,
  AGS_SHEET_EDIT_AUTO_SCROLL           = 1 <<  1,
}AgsSheetEditFlags;

typedef enum{
  AGS_SHEET_EDIT_NO_EDIT_MODE,
  AGS_SHEET_EDIT_POSITION_CURSOR,
  AGS_SHEET_EDIT_ADD_NOTE,
  AGS_SHEET_EDIT_DELETE_NOTE,
  AGS_SHEET_EDIT_SELECT_NOTE,
}AgsSheetEditMode;

typedef enum{
  AGS_SHEET_EDIT_BUTTON_1            = 1,
}AgsSheetEditButtonMask;

typedef enum{
  AGS_SHEET_EDIT_KEY_L_CONTROL       = 1,
  AGS_SHEET_EDIT_KEY_R_CONTROL       = 1 <<  1,
  AGS_SHEET_EDIT_KEY_L_SHIFT         = 1 <<  2,
  AGS_SHEET_EDIT_KEY_R_SHIFT         = 1 <<  3,
}AgsSheetEditKeyMask;

struct _AgsSheetEdit
{
  GtkGrid grid;

  guint flags;
  guint mode;

  guint button_mask;
  guint key_mask;

  guint note_offset;
  guint note_offset_absolute;

  guint cursor_position_x;
  guint cursor_position_y;

  guint selection_x0;
  guint selection_x1;
  guint selection_y0;
  guint selection_y1;

  gchar *paper_name;

  gchar *sheet_title;
  
  guint notation_x0;
  guint notation_x1;

  gchar *utf8_tablature_line;
  gchar *utf8_tablature_note;
  
  cairo_surface_t *ps_surface;  
  
  GtkDrawingArea *drawing_area;
  
  GtkVScrollbar *vscrollbar;
  GtkHScrollbar *hscrollbar;
};

struct _AgsSheetEditClass
{
  GtkGridClass grid;
};

GType ags_sheet_edit_get_type(void);

AgsSheetEdit* ags_sheet_edit_new();

G_END_DECLS

#endif /*__AGS_SHEET_EDIT_H__*/
