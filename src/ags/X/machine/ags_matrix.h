/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_MATRIX_H__
#define __AGS_MATRIX_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_MATRIX                (ags_matrix_get_type())
#define AGS_MATRIX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MATRIX, AgsMatrix))
#define AGS_MATRIX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MATRIX, AgsMatrixClass))
#define AGS_IS_MATRIX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MATRIX))
#define AGS_IS_MATRIX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MATRIX))
#define AGS_MATRIX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MATRIX, AgsMatrixClass))

typedef struct _AgsMatrix AgsMatrix;
typedef struct _AgsMatrixClass AgsMatrixClass;

struct _AgsMatrix
{
  AgsMachine machine;

  guint flags;

  guint mapped_output_pad;
  guint mapped_input_pad;

  GtkTable *table;

  GtkToggleButton *run;
  GtkToggleButton *selected;
  GtkToggleButton *index[9];

  GtkDrawingArea *drawing_area;
  guint active_led;
  GtkHBox *led;

  GtkAdjustment *adjustment;

  GtkSpinButton *length_spin;
  GtkOptionMenu *tact;
  GtkCheckButton *loop_button;
};

struct _AgsMatrixClass
{
  AgsMachineClass machine;
};

GType ags_matrix_get_type(void);

void ags_matrix_input_map_recall(AgsMatrix *matrix, guint input_pad_start);
void ags_matrix_output_map_recall(AgsMatrix *matrix, guint output_pad_start);

void ags_matrix_draw_gutter(AgsMatrix *matrix);
void ags_matrix_draw_matrix(AgsMatrix *matrix);
void ags_matrix_redraw_gutter_point(AgsMatrix *matrix, AgsChannel *channel, guint j, guint i);
void ags_matrix_highlight_gutter_point(AgsMatrix *matrix, guint j, guint i);
void ags_matrix_unpaint_gutter_point(AgsMatrix *matrix, guint j, guint i);

AgsMatrix* ags_matrix_new(GObject *devout);

#endif /*__AGS_MATRIX_H__*/
