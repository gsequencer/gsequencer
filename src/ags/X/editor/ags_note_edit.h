/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011, 2014 Joël Krähemann
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

#ifndef __AGS_NOTE_EDIT_H__
#define __AGS_NOTE_EDIT_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_notation.h>

#define AGS_TYPE_NOTE_EDIT                (ags_note_edit_get_type())
#define AGS_NOTE_EDIT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTE_EDIT, AgsNoteEdit))
#define AGS_NOTE_EDIT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTE_EDIT, AgsNoteEditClass))
#define AGS_IS_NOTE_EDIT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_NOTE_EDIT))
#define AGS_IS_NOTE_EDIT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_NOTE_EDIT))
#define AGS_NOTE_EDIT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_NOTE_EDIT, AgsNoteEditClass))

#define AGS_NOTE_EDIT_MAX_CONTROLS 1200

typedef struct _AgsNoteEdit AgsNoteEdit;
typedef struct _AgsNoteEditClass AgsNoteEditClass;

typedef enum{
  AGS_NOTE_EDIT_RESETING_VERTICALLY    = 1,
  AGS_NOTE_EDIT_RESETING_HORIZONTALLY  = 1 <<  1,
  AGS_NOTE_EDIT_POSITION_CURSOR        = 1 <<  2,
  AGS_NOTE_EDIT_ADDING_NOTE            = 1 <<  3,
  AGS_NOTE_EDIT_DELETING_NOTE          = 1 <<  4,
  AGS_NOTE_EDIT_SELECTING_NOTES        = 1 <<  5,
}AgsNoteEditFlags;

typedef enum{
  AGS_NOTE_EDIT_RESET_VSCROLLBAR   = 1,
  AGS_NOTE_EDIT_RESET_HSCROLLBAR   = 1 <<  1,
  AGS_NOTE_EDIT_RESET_WIDTH        = 1 <<  2,
  AGS_NOTE_EDIT_RESET_HEIGHT       = 1 <<  3, // reserved
}AgsNoteEditResetFlags;

struct _AgsNoteEdit
{
  GtkTable table;

  guint flags;

  GtkDrawingArea *drawing_area;

  struct _AgsNoteEditControl{ // values retrieved by mouse pressed and released callback
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

  struct _AgsNoteEditControlCurrent{ // values for drawing refering to current tic and zoom
    guint control_count;

    guint control_width;

    guint x0;
    guint x1;

    guint nth_x;
  }control_current;

  struct _AgsNoteEditControlUnit{ // values for drawing refering to smallest tic and current zoom
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

struct _AgsNoteEditClass
{
  GtkTableClass table;
};

GType ags_note_edit_get_type(void);

void ags_note_edit_reset_vertically(AgsNoteEdit *note_edit, guint flags);
void ags_note_edit_reset_horizontally(AgsNoteEdit *note_edit, guint flags);

void ags_note_edit_draw_segment(AgsNoteEdit *note_edit, cairo_t *cr);
void ags_note_edit_draw_position(AgsNoteEdit *note_edit, cairo_t *cr);
void ags_note_edit_draw_notation(AgsNoteEdit *note_edit, cairo_t *cr);

void ags_note_edit_draw_scroll(AgsNoteEdit *note_edit, cairo_t *cr,
			       gdouble position);

AgsNoteEdit* ags_note_edit_new();

#endif /*__AGS_NOTE_EDIT_H__*/
