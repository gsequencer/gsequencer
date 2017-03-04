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

#include "ags_led_array.h"

void ags_led_array_class_init(AgsLedArrayClass *led_array);
void ags_led_array_init(AgsLedArray *led_array);
void ags_led_array_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_led_array_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);

/**
 * SECTION:ags_led_array
 * @short_description: A led array widget
 * @title: AgsLedArray
 * @section_id:
 * @include: ags/widget/ags_led_array.h
 *
 * #AgsLedArray is a widget representing an array of leds.
 */

enum{
  PROP_0,
  PROP_LED_COUNT,
};

static gpointer ags_led_array_parent_class = NULL;

GtkStyle *led_array_style;

GType
ags_led_array_get_type(void)
{
  static GType ags_type_led_array = 0;

  if(!ags_type_led_array){
    static const GTypeInfo ags_led_array_info = {
      sizeof(AgsLedArrayClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_led_array_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLedArray),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_led_array_init,
    };

    ags_type_led_array = g_type_register_static(GTK_TYPE_BIN,
						"AgsLedArray\0",
						&ags_led_array_info,
						0);
  }

  return(ags_type_led_array);
}

void
ags_led_array_class_init(AgsLedArrayClass *led_array)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_led_array_parent_class = g_type_class_peek_parent(led_array);

  /* GObjectClass */
  gobject = (GObjectClass *) led_array;

  gobject->set_property = ags_led_array_set_property;
  gobject->get_property = ags_led_array_get_property;

  /* properties */
  /**
   * AgsLedArray:led-count:
   *
   * The count of leds available.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_uint("led-count\0",
				 "count of leds\0",
				 "The count of leds available\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LED_COUNT,
				  param_spec);
}

void
ags_led_array_init(AgsLedArray *led_array)
{
  gtk_widget_set_style((GtkWidget *) led_array,
		       led_array_style);

  led_array->led = NULL;
  led_array->led_count = 0;
}

void
ags_led_array_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsLedArray *led_array;

  led_array = AGS_LED_ARRAY(gobject);

  switch(prop_id){
  case PROP_LED_COUNT:
    {
      guint led_count;
      
      led_count = g_value_get_uint(value);

      ags_led_array_set_led_count(led_array,
				  led_count);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_led_array_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsLedArray *led_array;

  led_array = AGS_LED_ARRAY(gobject);

  switch(prop_id){
  case PROP_LED_COUNT:
    {
      g_value_set_uint(value, led_array->led_count);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_led_array_set_led_count(AgsLedArray *led_array,
			    guint led_count)
{
  guint i;
  
  if(led_array == NULL ||
     led_array->led_count == led_count){
    return;
  }

  if(led_count < led_array->led_count){
    /* shrink */
    for(i = led_count; i < led_array->led_count; i++){
      gtk_widget_destroy(led_array->led[i]);
    }

    led_array->led = (AgsLed **) realloc(led_array->led,
					 led_count);
  }else{
    /* grow */
    led_array->led = (AgsLed **) realloc(led_array->led,
					 led_count);

    for(i = led_array->led_count; i < led_count; i++){
      led_array->led[i] = ags_led_new();
      gtk_box_pack_start(led_array->box,
			 led_array->led[i],
			 FALSE, FALSE,
			 0);
    }
  }

  led_array->led_count = led_count;
}

void
ags_led_array_unset_all(AgsLedArray *led_array)
{
  guint i;

  if(led_array == NULL){
    return;
  }
  
  for(i = 0; i < led_array->led_count; i++){
    ags_led_unset_active(led_array->led[i]);
  }
}

void
ags_led_array_set_nth(AgsLedArray *led_array,
		      guint nth)
{
  if(led_array == NULL){
    return;
  }
  
  ags_led_set_active(led_array->led[nth]);
}

/**
 * ags_led_array_new:
 *
 * Creates an #AgsLedArray. Note, use rather its implementation #AgsVLedArray or
 * #AgsHLedArray.
 *
 * Returns: a new #AgsLedArray
 *
 * Since: 0.7.122.7
 */
AgsLedArray*
ags_led_array_new()
{
  AgsLedArray *led_array;

  led_array = (AgsLedArray *) g_object_new(AGS_TYPE_LED_ARRAY,
					   NULL);
  
  return(led_array);
}
