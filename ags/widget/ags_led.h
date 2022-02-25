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

#ifndef __AGS_LED_H__
#define __AGS_LED_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AGS_TYPE_LED                (ags_led_get_type())
#define AGS_LED(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LED, AgsLed))
#define AGS_LED_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LED, AgsLedClass))
#define AGS_IS_LED(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LED))
#define AGS_IS_LED_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LED))
#define AGS_LED_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LED, AgsLedClass))

#define AGS_LED_DEFAULT_SEGMENT_WIDTH (12)
#define AGS_LED_DEFAULT_SEGMENT_HEIGHT (8)

typedef struct _AgsLed AgsLed;
typedef struct _AgsLedClass AgsLedClass;

struct _AgsLed
{
  GtkWidget widget;

  guint segment_width;
  guint segment_height;

  gboolean active;
};

struct _AgsLedClass
{
  GtkWidgetClass widget;
};

GType ags_led_get_type(void);

guint ags_led_get_segment_width(AgsLed *led);
void ags_led_set_segment_width(AgsLed *led,
			       guint segment_width);

guint ags_led_get_segment_height(AgsLed *led);
void ags_led_set_segment_height(AgsLed *led,
				guint segment_height);

gboolean ags_led_is_active(AgsLed *led);

void ags_led_set_active(AgsLed *led,
			gboolean active);

AgsLed* ags_led_new(guint segment_width,
		    guint segment_height);

G_END_DECLS

#endif /*__AGS_LED_H__*/
