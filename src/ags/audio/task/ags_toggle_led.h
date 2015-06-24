/* This file is part of GSequencer.
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

#ifndef __AGS_TOGGLE_LED_H__
#define __AGS_TOGGLE_LED_H__

#include <glib-object.h>

#include <ags/audio/ags_task.h>

#define AGS_TYPE_TOGGLE_LED                (ags_toggle_led_get_type())
#define AGS_TOGGLE_LED(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TOGGLE_LED, AgsToggleLed))
#define AGS_TOGGLE_LED_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TOGGLE_LED, AgsToggleLedClass))
#define AGS_IS_TOGGLE_LED(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TOGGLE_LED))
#define AGS_IS_TOGGLE_LED_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_TOGGLE_LED))
#define AGS_TOGGLE_LED_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TOGGLE_LED, AgsToggleLedClass))

typedef struct _AgsToggleLed AgsToggleLed;
typedef struct _AgsToggleLedClass AgsToggleLedClass;

struct _AgsToggleLed
{
  AgsTask task;

  GList *led;
  guint set_active;
  guint unset_active;
};

struct _AgsToggleLedClass
{
  AgsTaskClass task;
};

GType ags_toggle_led_get_type();

AgsToggleLed* ags_toggle_led_new(GList *led,
				 guint set_active,
				 guint unset_active);

#endif /*__AGS_TOGGLE_LED_H__*/
