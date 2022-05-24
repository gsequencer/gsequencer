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

#ifndef __AGS_LED_ARRAY_H__
#define __AGS_LED_ARRAY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "ags_led.h"

G_BEGIN_DECLS

#define AGS_TYPE_LED_ARRAY                (ags_led_array_get_type())
#define AGS_LED_ARRAY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LED_ARRAY, AgsLedArray))
#define AGS_LED_ARRAY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LED_ARRAY, AgsLedArrayClass))
#define AGS_IS_LED_ARRAY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LED_ARRAY))
#define AGS_IS_LED_ARRAY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LED_ARRAY))
#define AGS_LED_ARRAY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LED_ARRAY, AgsLedArrayClass))

#define AGS_LED_ARRAY_DEFAULT_SEGMENT_WIDTH (10)
#define AGS_LED_ARRAY_DEFAULT_SEGMENT_HEIGHT (10)

typedef struct _AgsLedArray AgsLedArray;
typedef struct _AgsLedArrayClass AgsLedArrayClass;

struct _AgsLedArray
{
  GtkBox box;

  guint segment_width;
  guint segment_height;
  
  AgsLed **led;
  guint led_count;
};

struct _AgsLedArrayClass
{
  GtkBoxClass box;
};

GType ags_led_array_get_type(void);

/* properties */
guint ags_led_array_get_segment_width(AgsLedArray *led_array);
void ags_led_array_set_segment_width(AgsLedArray *led_array,
				     guint segment_width);

guint ags_led_array_get_segment_height(AgsLedArray *led_array);
void ags_led_array_set_segment_height(AgsLedArray *led_array,
				      guint segment_height);

guint ags_led_array_get_led_count(AgsLedArray *led_array);
void ags_led_array_set_led_count(AgsLedArray *led_array,
				 guint led_count);

/* control */
void ags_led_array_unset_all(AgsLedArray *led_array);
void ags_led_array_set_nth(AgsLedArray *led_array,
			   guint nth);

/* instantiate */
AgsLedArray* ags_led_array_new(GtkOrientation orientation,
			       guint segment_width,
			       guint segment_height,
			       guint led_count);

G_END_DECLS

#endif /*__AGS_LED_ARRAY_H__*/
