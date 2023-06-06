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
void ags_led_array_finalize(GObject *gobject);

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
  PROP_SEGMENT_WIDTH,
  PROP_SEGMENT_HEIGHT,
  PROP_LED_COUNT,
};

static gpointer ags_led_array_parent_class = NULL;

GType
ags_led_array_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_led_array = 0;

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

    ags_type_led_array = g_type_register_static(GTK_TYPE_BOX,
						"AgsLedArray",
						&ags_led_array_info,
						0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_led_array);
  }

  return g_define_type_id__volatile;
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

  gobject->finalize = ags_led_array_finalize;
  
  /* properties */
  /**
   * AgsLedArray:segment-width:
   *
   * The segment width of one led.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("segment-width",
				 "width of segment",
				 "The width of segment",
				 0,
				 G_MAXUINT,
				 AGS_LED_ARRAY_DEFAULT_SEGMENT_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_WIDTH,
				  param_spec);

  /**
   * AgsLedArray:segment-height:
   *
   * The segment height of one led.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("segment-height",
				 "height of segment",
				 "The height of segment",
				 0,
				 G_MAXUINT,
				 AGS_LED_ARRAY_DEFAULT_SEGMENT_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_HEIGHT,
				  param_spec);

  /**
   * AgsLedArray:led-count:
   *
   * The count of leds available.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("led-count",
				 "count of leds",
				 "The count of leds available",
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
  led_array->segment_width = AGS_LED_ARRAY_DEFAULT_SEGMENT_WIDTH;
  led_array->segment_height = AGS_LED_ARRAY_DEFAULT_SEGMENT_HEIGHT;
  
  led_array->led = NULL;
  led_array->led_count = 0;
}

void
ags_led_array_finalize(GObject *gobject)
{
  AgsLedArray *led_array;

  led_array = AGS_LED_ARRAY(gobject);

  g_free(led_array->led);
  
  /* call parent */
  G_OBJECT_CLASS(ags_led_array_parent_class)->finalize(gobject);
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
  case PROP_SEGMENT_WIDTH:
  {
    guint segment_width;
    guint i;
      
    segment_width = g_value_get_uint(value);

    if(led_array->segment_width != segment_width){
      led_array->segment_width = segment_width;
      
      for(i = 0; i < led_array->led_count; i++){
	ags_led_set_segment_width(led_array->led[i],
				  segment_width);
      }
    }
  }
  break;
  case PROP_SEGMENT_HEIGHT:
  {
    guint segment_height;
    guint i;
      
    segment_height = g_value_get_uint(value);

    if(led_array->segment_height != segment_height){
      led_array->segment_height = segment_height;
      
      for(i = 0; i < led_array->led_count; i++){
	ags_led_set_segment_height(led_array->led[i],
				   segment_height);
      }
    }
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
  case PROP_SEGMENT_WIDTH:
  {
    g_value_set_uint(value, led_array->segment_width);
  }
  break;
  case PROP_SEGMENT_HEIGHT:
  {
    g_value_set_uint(value, led_array->segment_height);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

/**
 * ags_led_array_get_segment_width:
 * @led_array: the #AgsLedArray
 * 
 * Get led width of @led_array.
 *
 * Returns: the led width
 * 
 * Since: 3.2.0
 */
guint
ags_led_array_get_segment_width(AgsLedArray *led_array)
{
  guint segment_width;
  
  if(!AGS_IS_LED_ARRAY(led_array)){
    return(0);
  }

  g_object_get(led_array,
	       "segment-width", &segment_width,
	       NULL);

  return(segment_width);
}

/**
 * ags_led_array_set_segment_width:
 * @led_array: the #AgsLedArray
 * @segment_width: the led width
 * 
 * Set led width of @led_array.
 *
 * Since: 3.2.0
 */
void
ags_led_array_set_segment_width(AgsLedArray *led_array,
				guint segment_width)
{
  if(!AGS_IS_LED_ARRAY(led_array)){
    return;
  }

  g_object_get(led_array,
	       "segment-width", segment_width,
	       NULL);
}

/**
 * ags_led_array_get_segment_height:
 * @led_array: the #AgsLedArray
 * 
 * Set led height of @led_array.
 *
 * Returns: the led height
 * 
 * Since: 3.2.0
 */
guint
ags_led_array_get_segment_height(AgsLedArray *led_array)
{
  guint segment_height;
  
  if(!AGS_IS_LED_ARRAY(led_array)){
    return(0);
  }

  g_object_get(led_array,
	       "segment-height", &segment_height,
	       NULL);

  return(segment_height);
}

/**
 * ags_led_array_set_segment_height:
 * @led_array: the #AgsLedArray
 * @segment_height: the led height
 * 
 * Set led height of @led_array.
 *
 * Since: 3.2.0
 */
void
ags_led_array_set_segment_height(AgsLedArray *led_array,
				 guint segment_height)
{
  if(!AGS_IS_LED_ARRAY(led_array)){
    return;
  }

  g_object_get(led_array,
	       "segment-height", segment_height,
	       NULL);
}

/**
 * ags_led_array_get_led_count:
 * @led_array: the #AgsLedArray
 * 
 * Get led count of @led_array.
 *
 * Returns: the led count
 * 
 * Since: 3.2.0
 */
guint
ags_led_array_get_led_count(AgsLedArray *led_array)
{
  guint led_count;
  
  if(!AGS_IS_LED_ARRAY(led_array)){
    return(0);
  }

  g_object_get(led_array,
	       "led-count", &led_count,
	       NULL);

  return(led_count);
}

/**
 * ags_led_array_set_led_count:
 * @led_array: the #AgsLedArray
 * @led_count: the led count
 * 
 * Set led count of @led_array.
 *
 * Since: 3.0.0
 */
void
ags_led_array_set_led_count(AgsLedArray *led_array,
			    guint led_count)
{
  guint i;
  
  if(!AGS_IS_LED_ARRAY(led_array) ||
     led_array->led_count == led_count){
    return;
  }

  if(led_count < led_array->led_count){
    /* shrink */
    for(i = led_count; i < led_array->led_count; i++){
      gtk_box_remove(led_array,
		     (GtkWidget *) led_array->led[i]);      
      g_object_unref(led_array->led[i]);
    }

    if(led_array->led == NULL){
      led_array->led = (AgsLed **) g_malloc(led_count * sizeof(AgsLed *));
    }else{
      led_array->led = (AgsLed **) g_realloc(led_array->led,
					     led_count * sizeof(AgsLed *));
    }
  }else{
    /* grow */
    if(led_array->led == NULL){
      led_array->led = (AgsLed **) g_malloc(led_count * sizeof(AgsLed *));
    }else{
      led_array->led = (AgsLed **) g_realloc(led_array->led,
					     led_count * sizeof(AgsLed *));
    }
    
    for(i = led_array->led_count; i < led_count; i++){
      led_array->led[i] = ags_led_new(led_array->segment_width,
				      led_array->segment_height);
      gtk_box_append(led_array,
		     (GtkWidget *) led_array->led[i]);
      gtk_widget_show((GtkWidget *) led_array->led[i]);
    }
  }

  led_array->led_count = led_count;
}

/**
 * ags_led_array_unset_all:
 * @led_array: the #AgsLedArray
 * 
 * Unset all led active.
 * 
 * Since: 3.0.0
 */
void
ags_led_array_unset_all(AgsLedArray *led_array)
{
  guint i;

  if(!AGS_IS_LED_ARRAY(led_array)){
    return;
  }
  
  for(i = 0; i < led_array->led_count; i++){
    ags_led_set_active(led_array->led[i],
		       FALSE);
  }
}

/**
 * ags_led_array_set_nth:
 * @led_array: the #AgsLedArray
 * @nth: the nth led
 * 
 * Set @nth led active.
 * 
 * Since: 3.0.0
 */
void
ags_led_array_set_nth(AgsLedArray *led_array,
		      guint nth)
{
  if(!AGS_IS_LED_ARRAY(led_array) ||
     nth >= led_array->led_count){
    return;
  }
  
  ags_led_set_active(led_array->led[nth],
		     TRUE);
}

/**
 * ags_led_array_new:
 * @orientation: the orientation
 * @segment_width: the width of the segment
 * @segment_height: the height of the segment
 * @led_count: the LED count
 *
 * Create a new instance of #AgsLedArray.
 *
 * Returns: the new #AgsLedArray
 *
 * Since: 3.0.0
 */
AgsLedArray*
ags_led_array_new(GtkOrientation orientation,
		  guint segment_width,
		  guint segment_height,
		  guint led_count)
{
  AgsLedArray *led_array;

  led_array = (AgsLedArray *) g_object_new(AGS_TYPE_LED_ARRAY,
					   "orientation", orientation,
					   "segment-width", segment_width,
					   "segment-height", segment_height,
					   "led-count", led_count,
					   NULL);
  
  return(led_array);
}
