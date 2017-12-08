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

#include <stdlib.h>

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
  PROP_LED_WIDTH,
  PROP_LED_HEIGHT,
  PROP_LED_COUNT,
};

static gpointer ags_led_array_parent_class = NULL;

GtkStyle *led_array_style = NULL;

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

    ags_type_led_array = g_type_register_static(GTK_TYPE_ALIGNMENT,
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
   * AgsLedArray:led-width:
   *
   * The width of one led.
   * 
   * Since: 1.0.0.7
   */
  param_spec = g_param_spec_uint("led-width\0",
				 "width of led\0",
				 "The width of one led\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LED_WIDTH,
				  param_spec);

  /**
   * AgsLedArray:led-height:
   *
   * The height of one led.
   * 
   * Since: 1.0.0.7
   */
  param_spec = g_param_spec_uint("led-height\0",
				 "height of led\0",
				 "The height of one led\0",
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LED_HEIGHT,
				  param_spec);

  /**
   * AgsLedArray:led-count:
   *
   * The count of leds available.
   * 
   * Since: 1.0.0.7
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
  if(led_array_style == NULL){
    led_array_style = gtk_style_copy(gtk_widget_get_style(led_array));
  }

  gtk_widget_set_style((GtkWidget *) led_array,
		       led_array_style);

  led_array->led_width = 12;
  led_array->led_height = 10;
  
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
  case PROP_LED_WIDTH:
    {
      guint led_width;
      
      led_array->led_width = g_value_get_uint(value);
    }
    break;
  case PROP_LED_HEIGHT:
    {
      guint led_height;
      
      led_array->led_height = g_value_get_uint(value);
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
  case PROP_LED_WIDTH:
    {
      g_value_set_uint(value, led_array->led_width);
    }
    break;
  case PROP_LED_HEIGHT:
    {
      g_value_set_uint(value, led_array->led_height);
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

    if(led_array->led == NULL){
      led_array->led = (AgsLed **) malloc(led_count * sizeof(AgsLed *));
    }else{
      led_array->led = (AgsLed **) realloc(led_array->led,
					   led_count * sizeof(AgsLed *));
    }
  }else{
    /* grow */
    if(led_array->led == NULL){
      led_array->led = (AgsLed **) malloc(led_count * sizeof(AgsLed *));
    }else{
      led_array->led = (AgsLed **) realloc(led_array->led,
					   led_count * sizeof(AgsLed *));
    }
    
    for(i = led_array->led_count; i < led_count; i++){
      led_array->led[i] = ags_led_new();
      gtk_widget_set_size_request((GtkWidget *) led_array->led[i],
				  led_array->led_width, led_array->led_height);
      gtk_box_pack_start(led_array->box,
			 led_array->led[i],
			 TRUE, FALSE,
			 0);
      gtk_widget_show(led_array->led[i]);
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
  if(led_array == NULL ||
     nth >= led_array->led_count){
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
 * Since: 1.0.0.7
 */
AgsLedArray*
ags_led_array_new()
{
  AgsLedArray *led_array;

  led_array = (AgsLedArray *) g_object_new(AGS_TYPE_LED_ARRAY,
					   NULL);
  
  return(led_array);
}
