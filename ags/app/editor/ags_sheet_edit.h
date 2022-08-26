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

#include <cairo.h>

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

#define AGS_SHEET_EDIT_SCRIPT(ptr) ((AgsSheetEditScript *)(ptr))

#define AGS_SHEET_EDIT_DEFAULT_PAPER_NAME (GTK_PAPER_NAME_LETTER)
#define AGS_SHEET_EDIT_DEFAULT_WIDTH (612)
#define AGS_SHEET_EDIT_DEFAULT_HEIGHT (792)

#define AGS_SHEET_EDIT_DEFAULT_FONT_SIZE (12.5)

#define AGS_SHEET_EDIT_LETTER_WIDTH (612)
#define AGS_SHEET_EDIT_LETTER_HEIGHT (792)

#define AGS_SHEET_EDIT_A4_WIDTH (595)
#define AGS_SHEET_EDIT_A4_HEIGHT (842)

#define AGS_SHEET_EDIT_DEFAULT_SPACING (6)

#define AGS_SHEET_EDIT_DEFAULT_SCRIPT_SPACING (24)

#define AGS_SHEET_EDIT_DEFAULT_GRAND_BRACE_STAFF_FONT_SIZE (24)

#define AGS_SHEET_EDIT_DEFAULT_CLEF_FONT_SIZE (20.0)

#define AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_FONT_SIZE (12.5)
#define AGS_SHEET_EDIT_DEFAULT_SHARP_FLAT_X0 (25.0)

#define AGS_SHEET_EDIT_DEFAULT_KEY_FONT_SIZE (16.0)
#define AGS_SHEET_EDIT_DEFAULT_REST_FONT_SIZE (16.0)
#define AGS_SHEET_EDIT_DEFAULT_KEY_X0 (50.0)
#define AGS_SHEET_EDIT_DEFAULT_KEY_PADDING (18.0)

#define AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_LEFT (18)
#define AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_RIGHT (12)
#define AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_TOP (24)
#define AGS_SHEET_EDIT_DEFAULT_PAGE_MARGIN_BOTTOM (8)

#define AGS_SHEET_EDIT_DEFAULT_NOTE_HEIGHT (6.0)

#define AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_X (0)
#define AGS_SHEET_EDIT_DEFAULT_CURSOR_POSITION_Y (0)

#define AGS_SHEET_EDIT_DEFAULT_FADER_WIDTH (3)

typedef struct _AgsSheetEdit AgsSheetEdit;
typedef struct _AgsSheetEditClass AgsSheetEditClass;
typedef struct _AgsSheetEditScript AgsSheetEditScript;

typedef enum{
  AGS_SHEET_EDIT_AUTO_SCROLL           = 1,
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

typedef enum{
  AGS_SHEET_EDIT_G_CLEF,
  AGS_SHEET_EDIT_F_CLEF,
}AgsSheetEditClef;

struct _AgsSheetEdit
{
  GtkGrid grid;

  guint flags;
  guint connectable_flags;
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
  
  guint page_orientation;  

  gchar *sheet_title;
  
  guint notation_x0;
  guint notation_x1;

  gchar *utf8_script_line;
  gchar *utf8_script_note;
  
  cairo_surface_t *ps_surface;  
  
  GtkDrawingArea *drawing_area;
  
  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;

  GList *script;
};

struct _AgsSheetEditClass
{
  GtkGridClass grid;
};

struct _AgsSheetEditScript
{
  gboolean is_primary;
  
  guint notation_x_start;
  guint notation_x_end;

  gdouble font_size;
  
  gdouble margin_top;
  gdouble margin_bottom;

  gdouble margin_left;
  gdouble margin_right;

  gboolean is_grand_staff;
  
  gdouble grand_staff_brace_translate_x;
  gdouble grand_staff_brace_translate_y;
  gdouble grand_staff_brace_translate_z;

  gdouble grand_staff_brace_font_size;

  guint clef;

  gdouble clef_translate_x;
  gdouble clef_translate_y;
  gdouble clef_translate_z;

  gdouble clef_font_size;

  gint staff_extends_top;
  gint staff_extends_bottom;
  
  gdouble staff_x0;
  gdouble staff_y0;
  
  gdouble staff_width;
  gdouble staff_height;

  gboolean is_minor;
  guint sharp_flat;

  gdouble sharp_translate_x;
  gdouble sharp_translate_y;
  gdouble sharp_translate_z;

  gdouble flat_translate_x;
  gdouble flat_translate_y;
  gdouble flat_translate_z;
  
  gdouble sharp_flat_font_size;

  gdouble key_translate_x;
  gdouble key_translate_y;
  gdouble key_translate_z;

  gdouble reverse_key_translate_x;
  gdouble reverse_key_translate_y;
  gdouble reverse_key_translate_z;
  
  gdouble key_font_size;

  gdouble rest_translate_x;
  gdouble rest_translate_y;
  gdouble rest_translate_z;

  gdouble rest_font_size;

  AgsSheetEditScript *companion_script;
};
  
GType ags_sheet_edit_get_type(void);

AgsSheetEditScript* ags_sheet_edit_script_alloc();
void ags_sheet_edit_script_free(AgsSheetEditScript *sheet_edit_script);

GList* ags_sheet_edit_get_script(AgsSheetEdit *sheet_edit);
void ags_sheet_edit_add_script(AgsSheetEdit *sheet_edit,
				  AgsSheetEditScript *sheet_edit_script);
void ags_sheet_edit_remove_script(AgsSheetEdit *sheet_edit,
				     AgsSheetEditScript *sheet_edit_script);

void ags_sheet_edit_draw_staff(AgsSheetEdit *sheet_edit, cairo_t *cr,
			       AgsSheetEditScript *sheet_edit_script,
			       gdouble x0, gdouble y0,
			       gdouble width, gdouble height,
			       gdouble font_size);

void ags_sheet_edit_draw_clef(AgsSheetEdit *sheet_edit, cairo_t *cr,
			      AgsSheetEditScript *sheet_edit_script,
			      gdouble x0, gdouble y0,
			      gdouble clef_font_size);

void ags_sheet_edit_draw_sharp_flat(AgsSheetEdit *sheet_edit, cairo_t *cr,
				    AgsSheetEditScript *sheet_edit_script,
				    gdouble x0, gdouble y0,
				    gdouble sharp_flat_font_size);

void ags_sheet_edit_draw_note(AgsSheetEdit *sheet_edit, cairo_t *cr,
			      AgsSheetEditScript *sheet_edit_script,
			      AgsNotation *notation,
			      AgsNote *note,
			      gdouble x0, gdouble y0,
			      gdouble key_font_size);

void ags_sheet_edit_draw_rest(AgsSheetEdit *sheet_edit, cairo_t *cr,
			      AgsSheetEditScript *sheet_edit_script,
			      guint rest_x0, guint rest_x1,
			      gdouble x0, gdouble y0,
			      gdouble rest_font_size);

void ags_sheet_edit_draw_notation(AgsSheetEdit *sheet_edit, cairo_t *cr,
				  AgsSheetEditScript *sheet_edit_script,
				  AgsNotation *notation,
				  guint notation_x0, guint notation_x1,
				  gdouble x0, gdouble y0,
				  gdouble key_font_size);

void ags_sheet_edit_draw(AgsSheetEdit *sheet_edit, cairo_t *cr);

AgsSheetEdit* ags_sheet_edit_new();

G_END_DECLS

#endif /*__AGS_SHEET_EDIT_H__*/
