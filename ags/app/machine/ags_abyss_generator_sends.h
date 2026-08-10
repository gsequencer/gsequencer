/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_ABYSS_GENERATOR_SENDS_H__
#define __AGS_ABYSS_GENERATOR_SENDS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_ABYSS_GENERATOR_SENDS                (ags_abyss_generator_sends_get_type())
#define AGS_ABYSS_GENERATOR_SENDS(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ABYSS_GENERATOR_SENDS, AgsAbysssGeneratorSends))
#define AGS_ABYSS_GENERATOR_SENDS_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ABYSS_GENERATOR_SENDS, AgsAbysssGeneratorSendsClass))
#define AGS_IS_ABYSS_GENERATOR_SENDS(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ABYSS_GENERATOR_SENDS))
#define AGS_IS_ABYSS_GENERATOR_SENDS_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ABYSS_GENERATOR_SENDS))
#define AGS_ABYSS_GENERATOR_SENDS_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ABYSS_GENERATOR_SENDS, AgsAbysssGeneratorSendsClass))

#define AGS_ABYSS_GENERATOR_SENDS_DEFAULT_CONTROL_WIDTH   (96)
#define AGS_ABYSS_GENERATOR_SENDS_ROTATED_CONTROL_WIDTH   (128)

#define AGS_ABYSS_GENERATOR_SENDS_DEFAULT_CELL_WIDTH   (24)
#define AGS_ABYSS_GENERATOR_SENDS_DEFAULT_CELL_HEIGHT  (16)

#define AGS_ABYSS_GENERATOR_SENDS_DEFAULT_CONTROLS_HORIZONTALLY (15)
#define AGS_ABYSS_GENERATOR_SENDS_DEFAULT_CONTROLS_VERTICALLY (13)

typedef struct _AgsAbysssGeneratorSends AgsAbysssGeneratorSends;
typedef struct _AgsAbysssGeneratorSendsClass AgsAbysssGeneratorSendsClass;

typedef enum{
  AGS_ABYSS_GENERATOR_SENDS_CURSOR_ON    = 1,
}AgsAbysssGeneratorSendsFlags;

typedef enum{
  AGS_ABYSS_GENERATOR_SENDS_KEY_L_CONTROL       = 1,
  AGS_ABYSS_GENERATOR_SENDS_KEY_R_CONTROL       = 1 <<  1,
}AgsAbysssGeneratorSendsKeyMask;

typedef enum{
  AGS_ABYSS_GENERATOR_SENDS_MOVE_LEFT,
  AGS_ABYSS_GENERATOR_SENDS_MOVE_RIGHT,
  AGS_ABYSS_GENERATOR_SENDS_MOVE_UP,
  AGS_ABYSS_GENERATOR_SENDS_MOVE_DOWN,
  AGS_ABYSS_GENERATOR_SENDS_TOGGLE_PAD,
  AGS_ABYSS_GENERATOR_SENDS_COPY_MATRIX,
}AgsAbysssGeneratorSendsAction;

struct _AgsAbysssGeneratorSends
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

struct _AgsAbysssGeneratorSendsClass
{
  GtkBoxClass box;

  void (*toggled)(AgsAbysssGeneratorSends *abyss_generator_sends,
		  gint x, gint y);
};

GType ags_abyss_generator_sends_get_type(void);

void ags_abyss_generator_sends_set_enabled(AgsAbysssGeneratorSends *abyss_generator_sends,
					   gint x, gint y,
					   gboolean enabled);
gboolean ags_abyss_generator_sends_get_enabled(AgsAbysssGeneratorSends *abyss_generator_sends,
					       gint x, gint y);

void ags_abyss_generator_sends_toggled(AgsAbysssGeneratorSends *abyss_generator_sends,
				       gint x, gint y);

void ags_abyss_generator_sends_draw(AgsAbysssGeneratorSends *abyss_generator_sends,
				    cairo_t *cr);

AgsAbysssGeneratorSends* ags_abyss_generator_sends_new();

G_END_DECLS

#endif /*__AGS_ABYSS_GENERATOR_SENDS_H__*/
