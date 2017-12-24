/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_PATTERN_BOX_H__
#define __AGS_PATTERN_BOX_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#define AGS_TYPE_PATTERN_BOX                (ags_pattern_box_get_type())
#define AGS_PATTERN_BOX(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PATTERN_BOX, AgsPatternBox))
#define AGS_PATTERN_BOX_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PATTERN_BOX, AgsPatternBoxClass))
#define AGS_IS_PATTERN_BOX(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PATTERN_BOX))
#define AGS_IS_PATTERN_BOX_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PATTERN_BOX))
#define AGS_PATTERN_BOX_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PATTERN_BOX, AgsPatternBoxClass))

#define AGS_PATTERN_BOX_DEFAULT_PAD_WIDTH   (24)
#define AGS_PATTERN_BOX_DEFAULT_PAD_HEIGHT  (24)

#define AGS_PATTERN_BOX_LED_DEFAULT_WIDTH (10)
#define AGS_PATTERN_BOX_LED_DEFAULT_HEIGHT (10)

#define AGS_PATTERN_BOX_N_CONTROLS (16)
#define AGS_PATTERN_BOX_N_INDICES (4)

typedef struct _AgsPatternBox AgsPatternBox;
typedef struct _AgsPatternBoxClass AgsPatternBoxClass;

typedef enum{
  AGS_PATTERN_BOX_BLOCK_PATTERN    = 1,
  AGS_PATTERN_BOX_CONNECTED        = 1 <<  1,
}AgsPatternBoxFlags;

typedef enum{
  AGS_PATTERN_BOX_KEY_L_CONTROL       = 1,
  AGS_PATTERN_BOX_KEY_R_CONTROL       = 1 <<  1,
}AgsPatternBoxKeyMask;

typedef enum{
  AGS_PATTERN_BOX_MOVE_LEFT,
  AGS_PATTERN_BOX_MOVE_RIGHT,
  AGS_PATTERN_BOX_INDEX_DECREMENT,
  AGS_PATTERN_BOX_INDEX_INCREMENT,
  AGS_PATTERN_BOX_TOGGLE_PAD,
  AGS_PATTERN_BOX_COPY_PATTERN,
}AgsPatternBoxAction;

struct _AgsPatternBox
{
  GtkTable table;

  guint flags;
  
  guint key_mask;

  guint n_controls;
  guint n_indices;

  guint cursor_x;
  guint cursor_y;
  
  guint active_led;
  AgsHLedArray *hled_array;

  GtkHBox *pattern;

  GtkVBox *offset;
};

struct _AgsPatternBoxClass
{
  GtkTableClass table;
};

GType ags_pattern_box_get_type(void);

gboolean ags_pattern_box_led_queue_draw_timeout(AgsPatternBox *pattern_box);

void ags_pattern_box_set_pattern(AgsPatternBox *pattern_box);

AgsPatternBox* ags_pattern_box_new();

#endif /*__AGS_PATTERN_BOX_H__*/
