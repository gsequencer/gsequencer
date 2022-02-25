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

#include "ags_led.h"

#include <stdlib.h>
#include <math.h>

void ags_led_class_init(AgsLedClass *led);
void ags_led_init(AgsLed *led);
void ags_led_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec);
void ags_led_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec);

void ags_led_realize(GtkWidget *widget);

void ags_led_measure(GtkWidget *widget,
		     GtkOrientation orientation,
		     int for_size,
		     int *minimum,
		     int *natural,
		     int *minimum_baseline,
		     int *natural_baseline);
void ags_led_size_allocate(GtkWidget *widget,
			   int width,
			   int height,
			   int baseline);

void ags_led_frame_clock_update_callback(GdkFrameClock *frame_clock,
					 AgsLed *led);

void ags_led_snapshot(GtkWidget *widget,
		      GtkSnapshot *snapshot);

void ags_led_draw(AgsLed *led,
		  cairo_t *cr,
		  gboolean is_animation);

/**
 * SECTION:ags_led
 * @short_description: A led widget
 * @title: AgsLed
 * @section_id:
 * @include: ags/widget/ags_led.h
 *
 * #AgsLed is a widget visualizing a #gboolean value.
 */

enum{
  PROP_0,
  PROP_SEGMENT_WIDTH,
  PROP_SEGMENT_HEIGHT,
};

static gpointer ags_led_parent_class = NULL;

GType
ags_led_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_led = 0;

    static const GTypeInfo ags_led_info = {
      sizeof(AgsLedClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_led_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLed),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_led_init,
    };

    ags_type_led = g_type_register_static(GTK_TYPE_WIDGET,
					  "AgsLed", &ags_led_info,
					  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_led);
  }

  return g_define_type_id__volatile;
}

void
ags_led_class_init(AgsLedClass *led)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_led_parent_class = g_type_class_peek_parent(led);

  /* GObjectClass */
  gobject = (GObjectClass *) led;

  gobject->set_property = ags_led_set_property;
  gobject->get_property = ags_led_get_property;
  
  /* properties */
  /**
   * AgsLed:segment-width:
   *
   * The led's segment width.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("segment-width",
				 "segment width",
				 "The led's segment width",
				 1,
				 G_MAXUINT,
				 AGS_LED_DEFAULT_SEGMENT_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_WIDTH,
				  param_spec);

  /**
   * AgsLed:segment-height:
   *
   * The led's segment height.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("segment-height",
				 "segment height",
				 "The led's segment height",
				 1,
				 G_MAXUINT,
				 AGS_LED_DEFAULT_SEGMENT_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEGMENT_HEIGHT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) led;

  widget->realize = ags_led_realize;

  widget->measure = ags_led_measure;
  widget->size_allocate = ags_led_size_allocate;
  
  widget->snapshot = ags_led_snapshot;
}

void
ags_led_init(AgsLed *led)
{
  gtk_widget_set_hexpand(led,
			 TRUE);
  
  gtk_widget_set_vexpand(led,
			 TRUE);

  led->active = FALSE;

  /* segment alignment */
  led->segment_width = AGS_LED_DEFAULT_SEGMENT_WIDTH;
  led->segment_height = AGS_LED_DEFAULT_SEGMENT_HEIGHT;
}

void
ags_led_set_property(GObject *gobject,
		     guint prop_id,
		     const GValue *value,
		     GParamSpec *param_spec)
{
  AgsLed *led;

  led = AGS_LED(gobject);

  switch(prop_id){
  case PROP_SEGMENT_WIDTH:
  {
    led->segment_width = g_value_get_uint(value);
  }
  break;
  case PROP_SEGMENT_HEIGHT:
  {
    led->segment_height = g_value_get_uint(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_led_get_property(GObject *gobject,
		     guint prop_id,
		     GValue *value,
		     GParamSpec *param_spec)
{
  AgsLed *led;

  led = AGS_LED(gobject);

  switch(prop_id){
  case PROP_SEGMENT_WIDTH:
  {
    g_value_set_uint(value, led->segment_width);
  }
  break;
  case PROP_SEGMENT_HEIGHT:
  {
    g_value_set_uint(value, led->segment_height);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_led_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_led_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_led_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}


void
ags_led_measure(GtkWidget *widget,
		GtkOrientation orientation,
		int for_size,
		int *minimum,
		int *natural,
		int *minimum_baseline,
		int *natural_baseline)
{
  AgsLed *led;

  led = (AgsLed *) widget;
  
  if(orientation == GTK_ORIENTATION_VERTICAL){
    minimum[0] = 
      natural[0] = led->segment_height;
  }else{
    minimum[0] = 
      natural[0] = led->segment_width;
  }
}

void
ags_led_size_allocate(GtkWidget *widget,
		      int width,
		      int height,
		      int baseline)
{
  AgsLed *led;

  led = (AgsLed *) widget;
  
  GTK_WIDGET_CLASS(ags_led_parent_class)->size_allocate(widget,
							width,
							height,
							baseline);
}

void
ags_led_frame_clock_update_callback(GdkFrameClock *frame_clock,
					  AgsLed *led)
{
  gtk_widget_queue_draw((GtkWidget *) led);
}

void
ags_led_snapshot(GtkWidget *widget,
		 GtkSnapshot *snapshot)
{
  GtkStyleContext *style_context;

  cairo_t *cr;

  graphene_rect_t rect;
  
  int width, height;
  
  style_context = gtk_widget_get_style_context((GtkWidget *) widget);  

  width = gtk_widget_get_width(widget);
  height = gtk_widget_get_height(widget);
  
  graphene_rect_init(&rect,
		     0.0, 0.0,
		     (float) width, (float) height);
  
  cr = gtk_snapshot_append_cairo(snapshot,
				 &rect);
  
  /* clear bg */
  gtk_render_background(style_context,
			cr,
			0.0, 0.0,
			(gdouble) width, (gdouble) height);

  ags_led_draw((AgsLed *) widget,
	       cr,
	       TRUE);
  
  cairo_destroy(cr);
}

void
ags_led_draw(AgsLed *led,
	     cairo_t *cr,
	     gboolean is_animation)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;

  int width, height;
  gint padding_top, padding_left;
  guint segment_width, segment_height;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean shadow_success;

  style_context = gtk_widget_get_style_context((GtkWidget *) led);
  
  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  width = gtk_widget_get_width((GtkWidget *) led);
  height = gtk_widget_get_height((GtkWidget *) led);

  segment_width = ags_led_get_segment_width(led);
  segment_height = ags_led_get_segment_height(led);
  
  padding_top = (height - segment_height) / 2;
  padding_left = (width - segment_width) / 2;
  
  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);
    
  shadow_success = gtk_style_context_lookup_color(style_context,
						  "theme_shadow_color",
						  &shadow_color);

  if(!fg_success ||
     !bg_success ||
     !shadow_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");
  }
  
  /*  */  
  cairo_push_group(cr);

  if(led->active){
    /* active */
    cairo_set_source_rgba(cr,
			  fg_color.red,
			  fg_color.green,
			  fg_color.blue,
			  fg_color.alpha);
  }else{
    /* normal */
    cairo_set_source_rgba(cr,
			  bg_color.red,
			  bg_color.green,
			  bg_color.blue,
			  bg_color.alpha);
  }

  cairo_rectangle(cr,
		  (double) padding_left, (double) padding_top,
		  (double) segment_width, (double) segment_height);
  cairo_fill(cr);

  /* outline */
  cairo_set_source_rgba(cr,
			shadow_color.red,
			shadow_color.green,
			shadow_color.blue,
			shadow_color.alpha);
  cairo_set_line_width(cr,
		       1.25);

  cairo_rectangle(cr,
		  (double) padding_left, (double) padding_top,
		  (double) segment_width, (double) segment_height);
  cairo_stroke(cr);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

/**
 * ags_led_get_segment_width:
 * @led: the #AgsLed
 * 
 * Get segment width.
 * 
 * Returns: the segment width
 * 
 * Since: 4.0.0
 */
guint
ags_led_get_segment_width(AgsLed *led)
{
  guint segment_width;

  if(!AGS_IS_LED(led)){
    return(0);
  }

  g_object_get(led,
	       "segment-width", &segment_width,
	       NULL);

  return(segment_width);
}

/**
 * ags_led_set_segment_width:
 * @led: the #AgsLed
 * @segment_width: the segment width
 * 
 * Set segment width.
 * 
 * Since: 4.0.0
 */
void
ags_led_set_segment_width(AgsLed *led,
			  guint segment_width)
{
  if(!AGS_IS_LED(led)){
    return;
  }

  g_object_set(led,
	       "segment-width", segment_width,
	       NULL);
}

/**
 * ags_led_get_segment_height:
 * @led: the #AgsLed
 * 
 * Get segment height.
 * 
 * Returns: the segment height
 * 
 * Since: 4.0.0
 */
guint
ags_led_get_segment_height(AgsLed *led)
{
  guint segment_height;
  
  if(!AGS_IS_LED(led)){
    return(0);
  }

  g_object_get(led,
	       "segment-height", &segment_height,
	       NULL);

  return(segment_height);
}

/**
 * ags_led_set_segment_height:
 * @led: the #AgsLed
 * @segment_height: the segment height
 * 
 * Set segment height.
 * 
 * Since: 4.0.0
 */
void
ags_led_set_segment_height(AgsLed *led,
			   guint segment_height)
{
  if(!AGS_IS_LED(led)){
    return;
  }

  g_object_set(led,
	       "segment-height", segment_height,
	       NULL);
}

/**
 * ags_led_is_active:
 * @led: the #AgsLed
 * 
 * Set @led to active state.
 * 
 * Returns: %TRUE if led active, otherwise %FALSE
 * 
 * Since: 3.2.0
 */
gboolean
ags_led_is_active(AgsLed *led)
{
  if(!AGS_IS_LED(led)){
    return(FALSE);
  }

  return(led->active);
}

/**
 * ags_led_set_active:
 * @led: the #AgsLed
 * @active: %TRUE if active, otherwise %FALSE
 * 
 * Set @led active by @active.
 * 
 * Since: 3.0.0
 */
void
ags_led_set_active(AgsLed *led,
		   gboolean active)
{
  if(!AGS_IS_LED(led)){
    return;
  }

  if(active){
    if(!led->active){
      led->active = TRUE;
      
      gtk_widget_queue_draw((GtkWidget *) led);
    }
  }else{
    if(led->active){
      led->active = FALSE;
      
      gtk_widget_queue_draw((GtkWidget *) led);
    }    
  }
}

/**
 * ags_led_new:
 * @segment_width: the width of the segment
 * @segment_height: the height of the segment
 *
 * Create a new instance of #AgsLed.
 *
 * Returns: the new #AgsLed
 *
 * Since: 3.0.0
 */
AgsLed*
ags_led_new(guint segment_width,
	    guint segment_height)
{
  AgsLed *led;

  led = (AgsLed *) g_object_new(AGS_TYPE_LED,
				"segment-width", segment_width,
				"segment-height", segment_height,					    
				NULL);
  
  return(led);
}
