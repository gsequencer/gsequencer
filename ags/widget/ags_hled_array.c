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

#include "ags_hled_array.h"

void ags_hled_array_class_init(AgsHLedArrayClass *hled_array);
void ags_hled_array_init(AgsHLedArray *hled_array);

/**
 * SECTION:ags_hled_array
 * @short_description: A horizontal led array widget
 * @title: AgsHLedArray
 * @section_id:
 * @include: ags/widget/ags_hled_array.h
 *
 * #AgsHLedArray is a widget representing an array of horizontal leds.
 */

static gpointer ags_hled_array_parent_class = NULL;

GType
ags_hled_array_get_type(void)
{
  static GType ags_type_hled_array = 0;

  if(!ags_type_hled_array){
    static const GTypeInfo ags_hled_array_info = {
      sizeof(AgsHLedArrayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_hled_array_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsHLedArray),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_hled_array_init,
    };

    ags_type_hled_array = g_type_register_static(AGS_TYPE_HLED_ARRAY,
						 "AgsHLedArray\0",
						 &ags_hled_array_info,
						 0);
  }

  return(ags_type_hled_array);
}

void
ags_hled_array_class_init(AgsHLedArrayClass *hled_array)
{
  /* empty */
}

void
ags_hled_array_init(AgsHLedArray *hled_array)
{
  AGS_LED_ARRAY(hled_array)->box = gtk_hbox_new(FALSE,
						0);
}

/**
 * ags_hled_array_new:
 *
 * Creates an #AgsHLedArray.
 *
 * Returns: a new #AgsHLedArray
 *
 * Since: 0.7.122.7
 */
AgsHLedArray*
ags_hled_array_new()
{
  AgsHLedArray *hled_array;

  hled_array = (AgsHLedArray *) g_object_new(AGS_TYPE_HLED_ARRAY,
					     NULL);
  
  return(hled_array);
}
