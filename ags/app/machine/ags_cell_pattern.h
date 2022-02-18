/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_CELL_PATTERN_H__
#define __AGS_CELL_PATTERN_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_CELL_PATTERN                (ags_cell_pattern_get_type())
#define AGS_CELL_PATTERN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CELL_PATTERN, AgsCellPattern))
#define AGS_CELL_PATTERN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CELL_PATTERN, AgsCellPatternClass))
#define AGS_IS_CELL_PATTERN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CELL_PATTERN))
#define AGS_IS_CELL_PATTERN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CELL_PATTERN))
#define AGS_CELL_PATTERN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CELL_PATTERN, AgsCellPatternClass))

#define AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH   (12)
#define AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT  (10)

#define AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY (32)
#define AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY (10)

#define AGS_CELL_PATTERN_DEFAULT_CONTROLS_HORIZONTALLY (32)
#define AGS_CELL_PATTERN_DEFAULT_CONTROLS_VERTICALLY (78)

typedef struct _AgsCellPattern AgsCellPattern;
typedef struct _AgsCellPatternClass AgsCellPatternClass;

typedef enum{
  AGS_CELL_PATTERN_CONNECTED    = 1,
  AGS_CELL_PATTERN_CURSOR_ON    = 1 <<  1,
}AgsCellPatternFlags;

typedef enum{
  AGS_CELL_PATTERN_KEY_L_CONTROL       = 1,
  AGS_CELL_PATTERN_KEY_R_CONTROL       = 1 <<  1,
}AgsCellPatternKeyMask;

typedef enum{
  AGS_CELL_PATTERN_MOVE_LEFT,
  AGS_CELL_PATTERN_MOVE_RIGHT,
  AGS_CELL_PATTERN_MOVE_UP,
  AGS_CELL_PATTERN_MOVE_DOWN,
  AGS_CELL_PATTERN_TOGGLE_PAD,
  AGS_CELL_PATTERN_COPY_PATTERN,
}AgsCellPatternAction;

struct _AgsCellPattern
{
  GtkGrid grid;

  guint flags;
  
  guint key_mask;
  
  guint cell_width;
  guint cell_height;
  
  guint n_cols;
  guint n_rows;

  guint cursor_x;
  guint cursor_y;
  
  GtkDrawingArea *drawing_area;
  
  GtkVScrollbar *vscrollbar;
  GtkHScrollbar *hscrollbar;

  guint active_led;
  AgsHLedArray *hled_array;
};

struct _AgsCellPatternClass
{
  GtkGridClass grid;
};

GType ags_cell_pattern_get_type(void);

void ags_cell_pattern_draw_grid(AgsCellPattern *cell_pattern, cairo_t *cr);
void ags_cell_pattern_draw_matrix(AgsCellPattern *cell_pattern, cairo_t *cr);
void ags_cell_pattern_draw_cursor(AgsCellPattern *cell_pattern, cairo_t *cr);
void ags_cell_pattern_redraw_gutter_point(AgsCellPattern *cell_pattern, cairo_t *cr,
					  AgsChannel *channel,
					  guint j, guint i);
void ags_cell_pattern_highlight_gutter_point(AgsCellPattern *cell_pattern, cairo_t *cr,
					     guint j, guint i);
void ags_cell_pattern_unpaint_gutter_point(AgsCellPattern *cell_pattern, cairo_t *cr,
					   guint j, guint i);

void* ags_cell_pattern_blink_worker(void *data);

gboolean ags_cell_pattern_led_queue_draw_timeout(AgsCellPattern *cell_pattern);

AgsCellPattern* ags_cell_pattern_new();

G_END_DECLS

#endif /*__AGS_CELL_PATTERN_H__*/
