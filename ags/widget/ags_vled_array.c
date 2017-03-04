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

#include "ags_vled_array.h"

void ags_vled_array_class_init(AgsVLedArrayClass *vled_array);
void ags_vled_array_init(AgsVLedArray *vled_array);

/**
 * SECTION:ags_vled_array
 * @short_description: A vertical led array widget
 * @title: AgsVLedArray
 * @section_id:
 * @include: ags/widget/ags_vled_array.h
 *
 * #AgsVLedArray is a widget representing an array of vertical leds.
 */

static gpointer ags_vled_array_parent_class = NULL;

GType
ags_vled_array_get_type(void)
{
  static GType ags_type_vled_array = 0;

  if(!ags_type_vled_array){
    static const GTypeInfo ags_vled_array_info = {
      sizeof(AgsVLedArrayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vled_array_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsVLedArray),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vled_array_init,
    };

    ags_type_vled_array = g_type_register_static(AGS_TYPE_VLED_ARRAY,
						 "AgsVLedArray\0",
						 &ags_vled_array_info,
						 0);
  }

  return(ags_type_vled_array);
}

void
ags_vled_array_class_init(AgsVLedArrayClass *vled_array)
{
  /* empty */
}

void
ags_vled_array_init(AgsVLedArray *vled_array)
{
  AGS_LED_ARRAY(vled_array)->box = gtk_vbox_new(FALSE,
						0);
}

/**
 * ags_vled_array_new:
 *
 * Creates an #AgsVLedArray.
 *
 * Returns: a new #AgsVLedArray
 *
 * Since: 0.7.122.7
 */
AgsVLedArray*
ags_vled_array_new()
{
  AgsVLedArray *vled_array;

  vled_array = (AgsVLedArray *) g_object_new(AGS_TYPE_VLED_ARRAY,
					     NULL);
  
  return(vled_array);
}
