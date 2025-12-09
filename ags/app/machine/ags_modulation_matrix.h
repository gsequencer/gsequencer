/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_MODULATION_MATRIX_H__
#define __AGS_MODULATION_MATRIX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_MODULATION_MATRIX                (ags_modulation_matrix_get_type())
#define AGS_MODULATION_MATRIX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MODULATION_MATRIX, AgsModulationMatrix))
#define AGS_MODULATION_MATRIX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MODULATION_MATRIX, AgsModulationMatrixClass))
#define AGS_IS_MODULATION_MATRIX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MODULATION_MATRIX))
#define AGS_IS_MODULATION_MATRIX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MODULATION_MATRIX))
#define AGS_MODULATION_MATRIX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MODULATION_MATRIX, AgsModulationMatrixClass))

#define AGS_MODULATION_MATRIX_DEFAULT_CONTROL_WIDTH   (96)
#define AGS_MODULATION_MATRIX_ROTATED_CONTROL_WIDTH   (128)

#define AGS_MODULATION_MATRIX_DEFAULT_CELL_WIDTH   (24)
#define AGS_MODULATION_MATRIX_DEFAULT_CELL_HEIGHT  (16)

#define AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_HORIZONTALLY (8)
#define AGS_MODULATION_MATRIX_DEFAULT_CONTROLS_VERTICALLY (5)

typedef struct _AgsModulationMatrix AgsModulationMatrix;
typedef struct _AgsModulationMatrixClass AgsModulationMatrixClass;

typedef enum{
  AGS_MODULATION_MATRIX_CURSOR_ON    = 1,
}AgsModulationMatrixFlags;

typedef enum{
  AGS_MODULATION_MATRIX_KEY_L_CONTROL       = 1,
  AGS_MODULATION_MATRIX_KEY_R_CONTROL       = 1 <<  1,
}AgsModulationMatrixKeyMask;

typedef enum{
  AGS_MODULATION_MATRIX_MOVE_LEFT,
  AGS_MODULATION_MATRIX_MOVE_RIGHT,
  AGS_MODULATION_MATRIX_MOVE_UP,
  AGS_MODULATION_MATRIX_MOVE_DOWN,
  AGS_MODULATION_MATRIX_TOGGLE_PAD,
  AGS_MODULATION_MATRIX_COPY_MATRIX,
}AgsModulationMatrixAction;

struct _AgsModulationMatrix
{
  GtkBox box;
  
  guint flags;
  guint connectable_flags;
  
  guint key_mask;
  
  guint font_size;

  guint cell_width;
  guint cell_height;
  
  guint n_cols;
  guint n_rows;

  guint cursor_x;
  guint cursor_y;

  gchar **label_x;
  gchar **label_y;

  guint64 *matrix_enabled;
  
  GtkGrid *grid;

  GtkDrawingArea *drawing_area;
  
  GtkScrollbar *vscrollbar;
  GtkScrollbar *hscrollbar;
};

struct _AgsModulationMatrixClass
{
  GtkBoxClass box;
};

GType ags_modulation_matrix_get_type(void);

void ags_modulation_matrix_set_enabled(AgsModulationMatrix *modulation_matrix,
				       gint x, gint y,
				       gboolean enabled);
gboolean ags_modulation_matrix_get_enabled(AgsModulationMatrix *modulation_matrix,
					   gint x, gint y);

void ags_modulation_matrix_draw(AgsModulationMatrix *modulation_matrix,
				cairo_t *cr);

AgsModulationMatrix* ags_modulation_matrix_new();

G_END_DECLS

#endif /*__AGS_MODULATION_MATRIX_H__*/
