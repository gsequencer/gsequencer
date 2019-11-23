/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

void ags_led_get_preferred_width(GtkWidget *widget,
				 gint *minimal_width,
				 gint *natural_width);
void ags_led_get_preferred_height(GtkWidget *widget,
				  gint *minimal_height,
				  gint *natural_height);
void ags_led_size_allocate(AgsLed *led,
			   GtkAllocation *allocation);
void ags_led_realize(GtkWidget *widget);
void ags_led_show(GtkWidget *widget);

void ags_led_draw(AgsLed *led, cairo_t *cr);

/**
 * SECTION:ags_led
 * @short_description: A led widget
 * @title: AgsLed
 * @section_id:
 * @include: ags/widget/ags_led.h
 *
 * #AgsLed is a widget visualizing a #gboolean value.
 */

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

    ags_type_led = g_type_register_static(GTK_TYPE_BIN,
					  "AgsLed", &ags_led_info,
					  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_led);
  }

  return g_define_type_id__volatile;
}

void
ags_led_class_init(AgsLedClass *led)
{
  GtkWidgetClass *widget;

  ags_led_parent_class = g_type_class_peek_parent(led);

  widget = (GtkWidgetClass *) led;

  widget->get_preferred_width = ags_led_get_preferred_width;
  widget->get_preferred_height = ags_led_get_preferred_height;
  widget->size_allocate = ags_led_size_allocate;
  widget->realize = ags_led_realize;
  widget->draw = ags_led_draw;
  widget->show = ags_led_show;
}

void
ags_led_init(AgsLed *led)
{
  g_object_set(G_OBJECT(led),
	       "app-paintable", TRUE,
	       NULL);

  led->flags = 0;
}

void
ags_led_size_allocate(AgsLed *led,
		      GtkAllocation *allocation)
{
  allocation->width = AGS_LED_DEFAULT_WIDTH;
  allocation->height = AGS_LED_DEFAULT_HEIGHT;
}

void
ags_led_get_preferred_width(GtkWidget *widget,
			    gint *minimal_width,
			    gint *natural_width)
{
  minimal_width[0] =
    natural_width[0] = AGS_LED_DEFAULT_WIDTH;
}

void
ags_led_get_preferred_height(GtkWidget *widget,
			     gint *minimal_height,
			     gint *natural_height)
{
  minimal_height[0] =
    natural_height[0] = AGS_LED_DEFAULT_HEIGHT;
}

void
ags_led_realize(GtkWidget *widget)
{
  GdkWindow *window;

  GtkAllocation allocation;
  GdkWindowAttr attributes;

  gint attributes_mask;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_LED (widget));

  gtk_widget_set_realized(widget, TRUE);

  gtk_widget_get_allocation(widget,
			    &allocation);

  /*  */
  //TODO:JK: apply borders of container widgets
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;

  attributes.window_type = GDK_WINDOW_CHILD;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual(widget);  
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
			    GDK_BUTTON_PRESS_MASK | 
			    GDK_BUTTON_RELEASE_MASK |
			    GDK_POINTER_MOTION_MASK |
			    GDK_POINTER_MOTION_HINT_MASK);

  window = gdk_window_new(gtk_widget_get_parent_window(widget),
			  &attributes, attributes_mask);
  gtk_widget_set_window(widget, window);
  gdk_window_set_user_data(window, widget);

  /* call parent */
  GTK_WIDGET_CLASS(ags_led_parent_class)->realize(widget);
}

void
ags_led_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_led_parent_class)->show(widget);
}

void
ags_led_draw(AgsLed *led, cairo_t *cr)
{
  GtkWidget *widget;

  GdkWindow *window;

  GtkStyleContext *led_style_context;

  GtkAllocation allocation;

  GdkRGBA *fg_color;
  GdkRGBA *bg_color;
  GdkRGBA *border_color;
  
  GValue value = {0,};

  widget = GTK_WIDGET(led);

  gtk_widget_get_allocation(widget,
			    &allocation);

  /* style context */
  led_style_context = gtk_widget_get_style_context(GTK_WIDGET(led));

  gtk_style_context_get_property(led_style_context,
				 "color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  fg_color = g_value_get_pointer(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(led_style_context,
				 "background-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  bg_color = g_value_get_pointer(&value);
  g_value_unset(&value);
  
  gtk_style_context_get_property(led_style_context,
				 "border-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  border_color = g_value_get_pointer(&value);
  g_value_unset(&value);

  /*  */
  if((AGS_LED_ACTIVE & (led->flags)) != 0){
    /* active */
    cairo_set_source_rgba(cr,
			  fg_color->red,
			  fg_color->green,
			  fg_color->blue,
			  fg_color->alpha);
  }else{
    /* normal */
    cairo_set_source_rgba(cr,
			  bg_color->red,
			  bg_color->green,
			  bg_color->blue,
			  bg_color->alpha);
  }

  cairo_rectangle(cr,
		  allocation.x, allocation.y,
		  allocation.width, allocation.height);
  cairo_fill(cr);

  /* outline */
  cairo_set_source_rgba(cr,
			border_color->red,
			border_color->green,
			border_color->blue,
			border_color->alpha);
  cairo_set_line_width(cr, 1.0);
  
  cairo_rectangle(cr,
		  allocation.x, allocation.y,
		  allocation.width, allocation.height);
  cairo_stroke(cr);
}

void
ags_led_set_active(AgsLed *led)
{
  led->flags |= AGS_LED_ACTIVE;

  gtk_widget_queue_draw((GtkWidget *) led);

  //  ags_led_draw(led);
}

void
ags_led_unset_active(AgsLed *led)
{
  led->flags &= (~AGS_LED_ACTIVE);

  gtk_widget_queue_draw((GtkWidget *) led);

  //  ags_led_draw(led);
}

/**
 * ags_led_new:
 *
 * Create a new instance of #AgsLed.
 *
 * Returns: the new #AgsLed
 *
 * Since: 2.0.0
 */
AgsLed*
ags_led_new()
{
  AgsLed *led;

  led = (AgsLed *) g_object_new(AGS_TYPE_LED,
				NULL);
  
  return(led);
}
