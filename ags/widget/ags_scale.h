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

#ifndef __AGS_SCALE_H__
#define __AGS_SCALE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_SCALE                (ags_scale_get_type())
#define AGS_SCALE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCALE, AgsScale))
#define AGS_SCALE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCALE, AgsScaleClass))
#define AGS_IS_SCALE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCALE))
#define AGS_IS_SCALE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCALE))
#define AGS_SCALE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCALE, AgsScaleClass))

#define AGS_SCALE_DEFAULT_WIDTH_REQUEST (60)
#define AGS_SCALE_DEFAULT_HEIGHT_REQUEST (128)

#define AGS_SCALE_DEFAULT_CONTROL_NAME "no name"

#define AGS_SCALE_DEFAULT_LOWER (0.0)
#define AGS_SCALE_DEFAULT_UPPER (1.0)
#define AGS_SCALE_DEFAULT_VALUE (0.0)

#define AGS_SCALE_DEFAULT_STEP_COUNT (16.0)
#define AGS_SCALE_DEFAULT_PAGE_SIZE (8.0)

typedef struct _AgsScale AgsScale;
typedef struct _AgsScaleClass AgsScaleClass;

typedef enum{
  AGS_SCALE_LOGARITHMIC       = 1,
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

  GtkOrientation orientation;

  guint key_mask;
  guint button_state;

  guint font_size;
  
  gchar *control_name;
  
  gdouble lower;
  gdouble upper;

  gdouble default_value;
  
  guint step_count;
  gdouble page_size;
  
  gint scale_step_count;
  gchar **scale_point;
  gdouble *scale_value;
};

struct _AgsScaleClass
{
  GtkWidgetClass widget;

  void (*value_changed)(AgsScale *scale,
			gdouble default_value);
};

GType ags_scale_get_type(void);

/* properties get/set */
void ags_scale_set_control_name(AgsScale *scale,
				gchar *control_name);
gchar* ags_scale_get_control_name(AgsScale *scale);

void ags_scale_set_upper(AgsScale *scale,
			 gdouble upper);
gdouble ags_scale_get_upper(AgsScale *scale);

void ags_scale_set_lower(AgsScale *scale,
			 gdouble lower);
gdouble ags_scale_get_lower(AgsScale *scale);

void ags_scale_set_default_value(AgsScale *scale,
				 gdouble default_value);
gdouble ags_scale_get_default_value(AgsScale *scale);

/* signal */
void ags_scale_value_changed(AgsScale *scale,
			     gdouble default_value);

/* instantiate */
AgsScale* ags_scale_new(GtkOrientation orientation,
			guint width_request,
			guint height_request);

G_END_DECLS

#endif /*__AGS_SCALE_H__*/
