/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_DIAL_H__
#define __AGS_DIAL_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_TYPE_DIAL                (ags_dial_get_type())
#define AGS_DIAL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DIAL, AgsDial))
#define AGS_DIAL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DIAL, AgsDialClass))
#define AGS_IS_DIAL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DIAL))
#define AGS_IS_DIAL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DIAL))
#define AGS_DIAL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DIAL, AgsDialClass))

#define AGS_DIAL_DEFAULT_PRECISION (8.0)

#define AGS_DIAL_DEFAULT_RADIUS (8)
#define AGS_DIAL_DEFAULT_OUTLINE_STRENGTH (4)
#define AGS_DIAL_DEFAULT_FONT_SIZE (6)
#define AGS_DIAL_DEFAULT_BUTTON_WIDTH (12)
#define AGS_DIAL_DEFAULT_BUTTON_HEIGHT (8)
#define AGS_DIAL_DEFAULT_MARGIN (4.0)
#define AGS_DIAL_DEFAULT_MARGIN_LEFT (AGS_DIAL_DEFAULT_MARGIN)
#define AGS_DIAL_DEFAULT_MARGIN_RIGHT (AGS_DIAL_DEFAULT_MARGIN)

#define AGS_DIAL_DEFAULT_HEIGHT (2 * (AGS_DIAL_DEFAULT_RADIUS + AGS_DIAL_DEFAULT_OUTLINE_STRENGTH + 1))
#define AGS_DIAL_DEFAULT_WIDTH (2 * (AGS_DIAL_DEFAULT_BUTTON_WIDTH + AGS_DIAL_DEFAULT_MARGIN + AGS_DIAL_DEFAULT_RADIUS + AGS_DIAL_DEFAULT_OUTLINE_STRENGTH + 2))

typedef struct _AgsDial AgsDial;
typedef struct _AgsDialClass AgsDialClass;

typedef enum{
  AGS_DIAL_WITH_BUTTONS           = 1,
  AGS_DIAL_MOUSE_BUTTON_PRESSED   = 1 <<  1,
  AGS_DIAL_BUTTON_DOWN_PRESSED    = 1 <<  2,
  AGS_DIAL_BUTTON_UP_PRESSED      = 1 <<  3,
  AGS_DIAL_MOTION_CAPTURING_INIT  = 1 <<  4,
  AGS_DIAL_MOTION_CAPTURING       = 1 <<  5,
  AGS_DIAL_SEEMLESS_MODE          = 1 <<  6,
  AGS_DIAL_INVERSE_LIGHT          = 1 <<  7,
}AgsDialFlags;

typedef enum{
  AGS_DIAL_INCREMENT,
  AGS_DIAL_DECREMENT,
}AgsDialAction;

struct _AgsDial
{
  GtkWidget widget;

  guint flags;

  guint radius;
  guint outline_strength;
  guint scale_precision;
  guint scale_max_precision;
  
  guint font_size;
  gint button_width;
  gint button_height;
  gint margin_left;
  gint margin_right;

  gdouble tolerance;
  gdouble negated_tolerance;

  GtkAdjustment *adjustment;

  gdouble gravity_x;
  gdouble gravity_y;
  gdouble current_x;
  gdouble current_y;
};

struct _AgsDialClass
{
  GtkWidgetClass widget;

  void (*value_changed)(AgsDial *dial);
};

GType ags_dial_get_type(void);

/* getter and setter */
void ags_dial_set_radius(AgsDial *dial,
			 guint radius);
guint ags_dial_get_radius(AgsDial *dial);

void ags_dial_set_outline_strength(AgsDial *dial,
				   guint outline_strength);
guint ags_dial_get_outline_strength(AgsDial *dial);

void ags_dial_set_scale_precision(AgsDial *dial,
				  guint scale_precision);
guint ags_dial_get_scale_precision(AgsDial *dial);

void ags_dial_set_font_size(AgsDial *dial,
			    guint font_size);
guint ags_dial_get_font_size(AgsDial *dial);

void ags_dial_set_button_width(AgsDial *dial,
			       gint button_width);
gint ags_dial_get_button_width(AgsDial *dial);

void ags_dial_set_button_height(AgsDial *dial,
				gint button_height);
gint ags_dial_get_button_height(AgsDial *dial);

void ags_dial_set_margin_left(AgsDial *dial,
			      gint margin_left);
gint ags_dial_get_margin_left(AgsDial *dial);

void ags_dial_set_margin_right(AgsDial *dial,
			       gint margin_right);
gint ags_dial_get_margin_right(AgsDial *dial);

void ags_dial_set_adjustment(AgsDial *dial,
			     GtkAdjustment *adjustment);
GtkAdjustment* ags_dial_get_adjustment(AgsDial *dial);

/* events related */
void ags_dial_value_changed(AgsDial *dial);

void ags_dial_set_value(AgsDial *dial,
			gdouble value);

/* instantiate */
AgsDial* ags_dial_new();

#endif /*__AGS_DIAL_H__*/
