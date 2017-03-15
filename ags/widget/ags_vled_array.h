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

#ifndef __AGS_VLED_ARRAY_H__
#define __AGS_VLED_ARRAY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include "ags_led_array.h"

#define AGS_TYPE_VLED_ARRAY                (ags_vled_array_get_type())
#define AGS_VLED_ARRAY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VLED_ARRAY, AgsVLedArray))
#define AGS_VLED_ARRAY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VLED_ARRAY, AgsVLedArrayClass))
#define AGS_IS_VLED_ARRAY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_VLED_ARRAY))
#define AGS_IS_VLED_ARRAY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_VLED_ARRAY))
#define AGS_VLED_ARRAY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_VLED_ARRAY, AgsVLedArrayClass))

typedef struct _AgsVLedArray AgsVLedArray;
typedef struct _AgsVLedArrayClass AgsVLedArrayClass;

struct _AgsVLedArray
{
  AgsLedArray led_array;
};

struct _AgsVLedArrayClass
{
  AgsLedArrayClass led_array;
};

GType ags_vled_array_get_type(void);

AgsVLedArray* ags_vled_array_new();

#endif /*__AGS_VLED_ARRAY_H__*/
