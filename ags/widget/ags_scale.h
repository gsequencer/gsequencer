/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_SCALE_H__
#define __AGS_SCALE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_SCALE                (ags_scale_get_type())
#define AGS_SCALE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCALE, AgsScale))
#define AGS_SCALE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCALE, AgsScaleClass))
#define AGS_IS_SCALE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCALE))
#define AGS_IS_SCALE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCALE))
#define AGS_SCALE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCALE, AgsScaleClass))

#define AGS_SCALE_DEFAULT_WIDTH (60)
#define AGS_SCALE_DEFAULT_HEIGHT (128)

#define AGS_SCALE_DEFAULT_LOWER (0.0)
#define AGS_SCALE_DEFAULT_UPPER (1.0)
#define AGS_SCALE_DEFAULT_VALUE (1.0)

typedef struct _AgsScale AgsScale;
typedef struct _AgsScaleClass AgsScaleClass;

typedef enum{
  AGS_SCALE_DRAW_LINEAR            = 1,
  AGS_SCALE_DRAW_LOGARITHMIC       = 1 <<  1,
}AgsScaleFlags;

typedef enum{
  AGS_SCALE_BUTTON_1_PRESSED     = 1,
}AgsScaleButtonState;

typedef enum{
  AGS_SCALE_KEY_L_CONTROL       = 1,
  AGS_SCALE_KEY_R_CONTROL       = 1 <<  1,
  AGS_SCALE_KEY_L_SHIFT         = 1 <<  2,
  AGS_SCALE_KEY_R_SHIFT         = 1 <<  3,
}AgsScaleKeyMask;

typedef enum{
  AGS_SCALE_LAYOUT_VERTICAL,
  AGS_SCALE_LAYOUT_HORIZONTAL,
}AgsScaleLayout;

typedef enum{
  AGS_SCALE_STEP_UP,
  AGS_SCALE_STEP_DOWN,
  AGS_SCALE_PAGE_UP,
  AGS_SCALE_PAGE_DOWN,
}AgsScaleAction;

struct _AgsScale
{
  GtkWidget widget;

  guint flags;

  guint key_mask;
  guint button_state;
  guint layout;

  guint font_size;

  guint scale_width;
  guint scale_height;
  
  gchar *control_name;
  
  gdouble lower;
  gdouble upper;
  
  guint step_count;

  gdouble default_value;
  
  gint scale_step_count;
  gchar **scale_point;
  gdouble *scale_value;
};

struct _AgsScaleClass
{
  GtkWidgetClass widget;

  void (*value_changed)(AgsScale *scale,
			gdouble value);
};

GType ags_scale_get_type(void);

AgsScale* ags_scale_new();

#endif /*__AGS_SCALE_H__*/
