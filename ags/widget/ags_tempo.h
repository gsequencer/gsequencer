/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_TEMPO_H__
#define __AGS_TEMPO_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_TEMPO                (ags_tempo_get_type())
#define AGS_TEMPO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TEMPO, AgsTempo))
#define AGS_TEMPO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TEMPO, AgsTempoClass))
#define AGS_IS_TEMPO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TEMPO))
#define AGS_IS_TEMPO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TEMPO))
#define AGS_TEMPO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_TEMPO, AgsTempoClass))

#define AGS_TEMPO_DEFAULT_WIDTH_REQUEST (60)
#define AGS_TEMPO_DEFAULT_HEIGHT_REQUEST (128)

#define AGS_TEMPO_DEFAULT_CONTROL_NAME "tempo [BPM]"

#define AGS_TEMPO_DEFAULT_LOWER (0.0)
#define AGS_TEMPO_DEFAULT_UPPER (240.0)
#define AGS_TEMPO_DEFAULT_VALUE (120.0)

#define AGS_TEMPO_DEFAULT_STEP_COUNT (240.0)
#define AGS_TEMPO_DEFAULT_PAGE_SIZE (8.0)

typedef struct _AgsTempo AgsTempo;
typedef struct _AgsTempoClass AgsTempoClass;

typedef enum{
  AGS_TEMPO_BUTTON_1_PRESSED     = 1,
}AgsTempoButtonState;

typedef enum{
  AGS_TEMPO_KEY_L_CONTROL       = 1,
  AGS_TEMPO_KEY_R_CONTROL       = 1 <<  1,
  AGS_TEMPO_KEY_L_SHIFT         = 1 <<  2,
  AGS_TEMPO_KEY_R_SHIFT         = 1 <<  3,
}AgsTempoKeyMask;

typedef enum{
  AGS_TEMPO_LAYOUT_VERTICAL,
  AGS_TEMPO_LAYOUT_HORIZONTAL,
}AgsTempoLayout;

typedef enum{
  AGS_TEMPO_STEP_UP,
  AGS_TEMPO_STEP_DOWN,
  AGS_TEMPO_PAGE_UP,
  AGS_TEMPO_PAGE_DOWN,
}AgsTempoAction;

struct _AgsTempo
{
  GtkWidget widget;

  GtkOrientation orientation;

  AgsTempoKeyMask key_mask;
  AgsTempoButtonState button_state;

  guint font_size;
  
  gchar *control_name;
  
  gdouble lower;
  gdouble upper;

  gdouble default_value;
  
  guint step_count;
  gdouble page_size;
  
  gint tempo_step_count;
  gchar **tempo_point;
  gdouble *tempo_value;
};

struct _AgsTempoClass
{
  GtkWidgetClass widget;

  void (*value_changed)(AgsTempo *tempo,
			gdouble default_value);
};

GType ags_tempo_get_type(void);

/* properties get/set */
void ags_tempo_set_control_name(AgsTempo *tempo,
				gchar *control_name);
gchar* ags_tempo_get_control_name(AgsTempo *tempo);

void ags_tempo_set_upper(AgsTempo *tempo,
			 gdouble upper);
gdouble ags_tempo_get_upper(AgsTempo *tempo);

void ags_tempo_set_lower(AgsTempo *tempo,
			 gdouble lower);
gdouble ags_tempo_get_lower(AgsTempo *tempo);

void ags_tempo_set_default_value(AgsTempo *tempo,
				 gdouble default_value);
gdouble ags_tempo_get_default_value(AgsTempo *tempo);

/* signal */
void ags_tempo_value_changed(AgsTempo *tempo,
			     gdouble default_value);

/* instantiate */
AgsTempo* ags_tempo_new(GtkOrientation orientation,
			guint width_request,
			guint height_request);

G_END_DECLS

#endif /*__AGS_TEMPO_H__*/
