/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "ags_led.h"

#include <stdlib.h>
#include <math.h>

void ags_led_class_init(AgsLedClass *led);
void ags_led_init(AgsLed *led);
void ags_led_show(GtkWidget *widget);

void ags_led_realize(GtkWidget *widget);
gint ags_led_expose(GtkWidget *widget,
		     GdkEventExpose *event);
void ags_led_size_request(GtkWidget *widget,
			   GtkRequisition   *requisition);
void ags_led_size_allocate(GtkWidget *widget,
			    GtkAllocation *allocation);
gboolean ags_led_expose(GtkWidget *widget,
			 GdkEventExpose *event);

void ags_led_draw(AgsLed *led);

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
  static GType ags_type_led = 0;

  if(!ags_type_led){
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
					  "AgsLed\0", &ags_led_info,
					  0);
  }

  return(ags_type_led);
}

void
ags_led_class_init(AgsLedClass *led)
{
  GtkWidgetClass *widget;

  ags_led_parent_class = g_type_class_peek_parent(led);

  widget = (GtkWidgetClass *) led;

  widget->realize = ags_led_realize;
  widget->expose_event = ags_led_expose;
  widget->show = ags_led_show;
}

void
ags_led_init(AgsLed *led)
{
  g_object_set(G_OBJECT(led),
	       "app-paintable\0", TRUE,
	       NULL);

  led->flags = 0;
}

void
ags_led_realize(GtkWidget *widget)
{
  AgsLed *led;
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (AGS_IS_LED (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  led = AGS_LED (widget);

  /*  */
  //TODO:JK: apply borders of container widgets
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget) | 
    GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
    GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
    GDK_POINTER_MOTION_HINT_MASK;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attributes_mask);

  widget->style = gtk_style_attach(widget->style, widget->window);

  gdk_window_set_user_data(widget->window, widget);

  gtk_style_set_background(widget->style, widget->window, GTK_STATE_ACTIVE);


  GTK_WIDGET_CLASS(ags_led_parent_class)->realize(widget);
}

void
ags_led_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_led_parent_class)->show(widget);
}

gboolean
ags_led_expose(GtkWidget *widget,
		GdkEventExpose *event)
{
  GTK_WIDGET_CLASS(ags_led_parent_class)->expose_event(widget, event);

  ags_led_draw(AGS_LED(widget));

  return(FALSE);
}

void
ags_led_draw(AgsLed *led)
{
  GtkWidget *widget;
  GdkWindow *window;
  GtkStyle *style;
  cairo_t *cr;

  widget = GTK_WIDGET(led);

  cr = gdk_cairo_create(widget->window);

  style = widget->style;

  /*  */
  if((AGS_LED_ACTIVE & (led->flags)) != 0){
    cairo_set_source_rgba (cr,
			   1.0,
			   0.0,
			   0.0,
			   1.0);
  }else{
    cairo_set_source_rgba (cr,
			   0.3,
			   0.0,
			   0.0,
			   1.0);
  }

  cairo_rectangle(cr,
		  widget->allocation.x, widget->allocation.y,
		  widget->allocation.width, widget->allocation.height);
  cairo_fill(cr);

  /*  */
  cairo_set_source_rgba (cr,
			 1.0 / G_MAXUINT16 * style->fg[0].red,
			 1.0 / G_MAXUINT16 * style->fg[0].green,
			 1.0 / G_MAXUINT16 * style->fg[0].blue,
			 1.0);
  cairo_set_line_width(cr, 1.0);

  cairo_rectangle(cr,
		  widget->allocation.x, widget->allocation.y,
		  widget->allocation.width, widget->allocation.height);
  cairo_stroke(cr);

  /*  */
  cairo_destroy(cr);
}

void
ags_led_set_active(AgsLed *led)
{
  led->flags |= AGS_LED_ACTIVE;

  gtk_widget_queue_draw(led);

  //  ags_led_draw(led);
}

void
ags_led_unset_active(AgsLed *led)
{
  led->flags &= (~AGS_LED_ACTIVE);

  gtk_widget_queue_draw(led);

  //  ags_led_draw(led);
}

/**
 * ags_led_new:
 *
 * Creates an #AgsLed.
 *
 * Returns: a new #AgsLed
 *
 * Since: 0.4
 */
AgsLed*
ags_led_new()
{
  AgsLed *led;

  led = (AgsLed *) g_object_new(AGS_TYPE_LED,
				NULL);
  
  return(led);
}
