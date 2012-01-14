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

#include "ags_dial.h"

#include <stdlib.h>
#include <math.h>

void ags_dial_class_init(AgsDialClass *dial);
void ags_dial_init(AgsDial *dial);
void ags_dial_show(GtkWidget *widget);

void ags_dial_realize(GtkWidget *widget);
gint ags_dial_expose(GtkWidget *widget,
		     GdkEventExpose *event);

void ags_dial_draw(AgsDial *dial,
		   guint r, 
		   guint scale_precision,
		   guint font_size);

static gpointer ags_dial_parent_class = NULL;

GType
ags_dial_get_type(void)
{
  static GType ags_type_dial = 0;

  if(!ags_type_dial){
    static const GTypeInfo ags_dial_info = {
      sizeof(AgsDialClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_dial_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDial),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dial_init,
    };

    ags_type_dial = g_type_register_static(GTK_TYPE_RANGE,
					   "AgsDial\0", &ags_dial_info,
					   0);
  }

  return(ags_type_dial);
}

void
ags_dial_class_init(AgsDialClass *dial)
{
  GtkWidgetClass *widget;

  ags_dial_parent_class = g_type_class_peek_parent(dial);

  widget = (GtkWidgetClass *) dial;

  widget->realize = ags_dial_realize;
  widget->expose_event = ags_dial_expose;
  widget->show = ags_dial_show;
}

void
ags_dial_init(AgsDial *dial)
{
  g_object_set(G_OBJECT(dial),
	       "app-paintable\0", TRUE,
	       NULL);
}

void
ags_dial_realize(GtkWidget *widget)
{
  AgsDial *dial;
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (AGS_IS_DIAL (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  dial = AGS_DIAL (widget);

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
  widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);

  widget->style = gtk_style_attach (widget->style, widget->window);

  gdk_window_set_user_data (widget->window, widget);

  gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);

  GTK_WIDGET_CLASS(ags_dial_parent_class)->realize(widget);
}

void
ags_dial_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_dial_parent_class)->show(widget);
}

gint
ags_dial_expose(GtkWidget *widget,
		GdkEventExpose *event)
{
  GTK_WIDGET_CLASS(ags_dial_parent_class)->expose_event(widget, event);

  ags_dial_draw(AGS_DIAL(widget), 50, 8, 14);

  return(FALSE);
}

/**
 * ags_dial_draw:
 * @dial an #AgsDial
 * @r the radius as a guint
 * @scale_precision the scale's precision
 * @font_size the font size of the indicators
 *
 * draws the widget
 */
void
ags_dial_draw(AgsDial *dial,
	      guint r, 
	      guint scale_precision,
	      guint font_size)
{
  GtkWidget *widget;
  GdkWindow *window;
  cairo_t *cr;
  cairo_text_extents_t te_up, te_down;
  gdouble button_width, button_height, margin_left, margin_right;
  gdouble radius, outline_strength;
  gdouble unused;
  gdouble scale_area, scale_width, scale_inverted_width;
  gdouble starter_angle;
  guint i;

  widget = GTK_WIDGET(dial);

  cr = gdk_cairo_create(widget->window);

  radius = (gdouble) r;
  outline_strength = 4.0;

  margin_left = 4.0;
  margin_right = 4.0;

  cairo_select_font_face (cr, "Georgia",
			  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, (gdouble) font_size);
  cairo_text_extents (cr, "-\0", &te_down);
  cairo_text_extents (cr, "+\0", &te_up);

  if(te_down.width < te_up.width){
    button_width = te_up.width * 3;
  }else{
    button_width = te_down.width * 3;
  }

  if(te_down.height < te_up.height){
    button_height = te_up.height * 2;
  }else{
    button_height = te_down.height * 2;
  }

  /* draw controller button down */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(cr, 2.0);

  cairo_rectangle(cr,
		  1.0, (2 * radius) - button_height,
		  button_width, button_height);
  cairo_stroke(cr);

  cairo_move_to (cr,
		 1.0 + 0.5 - te_down.width / 2 - te_down.x_bearing + button_width / 2.25,
		 0.5 - te_down.height / 2 - te_down.y_bearing + (radius * 2) - button_height / 2.0);
  cairo_show_text (cr, "-\0");

  /* background */
  cairo_set_source_rgba(cr, 0.9, 0.8, 0.0, 1.0);
  cairo_arc (cr,
	     1.0 + button_width + margin_left + radius,
	     radius + outline_strength,
	     radius,
	     -1.0 * M_PI,
	     1.0 * M_PI);
  cairo_fill(cr);

  /* light effect */
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.3);

  cairo_line_to(cr,
		1.0 + button_width + margin_left + radius,
		radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		1.0 + sin(-1 * (0.35 * M_PI) / (0.65 * M_PI)) + button_width + margin_left + radius,
		cos((0.65 * M_PI) / (0.35 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		1.0 + sin((0.65 * M_PI) / (0.35 * M_PI)) + button_width + margin_left + radius,
		-1 * cos((0.35 * M_PI) / (0.65 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_close_path(cr);

  cairo_arc (cr,
	     1.0 + button_width + margin_left + radius,
	     radius + outline_strength,
	     radius,
	     0.35 * M_PI,
	     0.65 * M_PI);
  cairo_fill(cr);

  cairo_line_to(cr,
		1.0 + cos((0.65 * M_PI) / (0.35 * M_PI)) + button_width + margin_left + radius,
		-1 * sin((0.35 * M_PI) / (0.65 * M_PI)) + 57.0);
  cairo_line_to(cr,
		1.0 + -1 * cos((0.35 * M_PI) / (0.65 * M_PI)) + button_width + margin_left + radius,
		sin((0.65 * M_PI) / (0.35 * M_PI)) + 57.0);
  cairo_line_to(cr,
		1.0 + button_width + margin_left + radius,
		57.0);
  cairo_close_path(cr);

  cairo_arc (cr,
	     1.0 + button_width + margin_left + radius,
	     radius + outline_strength,
	     radius,
	     -0.65 * M_PI,
	     -0.35 * M_PI);
  cairo_fill(cr);

  /* outline */
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
  //  cairo_set_line_width(cr, 1.0 - (2.0 / M_PI));
  cairo_set_line_width(cr, 1.0);
  cairo_arc (cr,
	     1.0 + button_width + margin_left + radius,
	     radius + outline_strength,
	     radius,
	     -1.0 * M_PI,
	     1.0 * M_PI);
  cairo_stroke(cr);

  /* scale */
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(cr, 3.0);

  unused = 0.25 * 2.0 * M_PI;
  starter_angle = (2.0 * M_PI - unused) * 0.5;

  scale_area = 2.0 * M_PI - starter_angle;

  //  scale_precision = 8;
  scale_inverted_width = (2.0 * (radius + outline_strength) * M_PI - ((radius + outline_strength) * unused)) / scale_precision - 4.0;
  scale_width = (2.0 * (radius + outline_strength) * M_PI - ((radius + outline_strength) * unused)) / scale_precision - scale_inverted_width;

  scale_inverted_width /= (radius + outline_strength);
  scale_width /= (radius + outline_strength);

  for(i = 0; i <= scale_precision; i++){
    cairo_arc (cr,
	       1.0 + button_width + margin_left + radius,
	       radius + outline_strength,
	       radius + outline_strength / M_PI,
	       starter_angle + ((gdouble) i * scale_inverted_width) + ((gdouble) i * scale_width),
	       starter_angle + ((gdouble) i * scale_inverted_width) + ((gdouble) i * scale_width) + scale_width);
    cairo_stroke(cr);
  }

  /* draw controller button up */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(cr, 2.0);

  cairo_rectangle(cr,
		  1.0 + (2.0 * radius) + button_width + margin_left + margin_right,
		  (2.0 * radius) - button_height,
		  button_width, button_height);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
  cairo_stroke(cr);

  cairo_move_to (cr,
		 1.0 + 0.5 - te_up.width / 2 - te_up.x_bearing + (radius * 2) + margin_left + margin_right + button_width + button_width / 2.25,
		 0.5 - te_up.height / 2 - te_up.y_bearing + (radius * 2) - button_height / 2.0);
  cairo_show_text (cr, "+\0");

}

AgsDial*
ags_dial_new()
{
  AgsDial *dial;

  dial = (AgsDial *) g_object_new(AGS_TYPE_DIAL, NULL);
  
  return(dial);
}
