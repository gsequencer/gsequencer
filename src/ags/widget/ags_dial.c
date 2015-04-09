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
void ags_dial_size_request(GtkWidget *widget,
			   GtkRequisition   *requisition);
void ags_dial_size_allocate(GtkWidget *widget,
			    GtkAllocation *allocation);
gboolean ags_dial_expose(GtkWidget *widget,
			 GdkEventExpose *event);
gboolean ags_dial_button_press(GtkWidget *widget,
			       GdkEventButton *event);
gboolean ags_dial_button_release(GtkWidget *widget,
				 GdkEventButton   *event);
gboolean ags_dial_motion_notify(GtkWidget *widget,
				GdkEventMotion *event);

void ags_dial_draw(AgsDial *dial);
void* ags_dial_idle(void *dial0);

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

    ags_type_dial = g_type_register_static(GTK_TYPE_WIDGET,
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
  widget->size_request = ags_dial_size_request;
  widget->size_allocate = ags_dial_size_allocate;
  widget->button_press_event = ags_dial_button_press;
  widget->button_release_event = ags_dial_button_release;
  widget->motion_notify_event = ags_dial_motion_notify;
  widget->show = ags_dial_show;
}

void
ags_dial_init(AgsDial *dial)
{
  g_object_set(G_OBJECT(dial),
	       "app-paintable\0", TRUE,
	       NULL);

  dial->flags = AGS_DIAL_WITH_BUTTONS;

  dial->radius = 8;
  dial->scale_precision = 8;
  dial->outline_strength = 4;

  dial->font_size = 14;
  dial->button_width = 0;
  dial->button_height = 0;
  dial->margin_left = 4.0;
  dial->margin_right = 4.0;

  dial->tolerance = 0.9;
  dial->negated_tolerance = 1.1;

  dial->sleep_interval = 500;

  pthread_attr_init(&dial->idle_thread_attr);
  pthread_attr_setschedpolicy(&dial->idle_thread_attr, SCHED_RR);
  pthread_attr_setinheritsched(&dial->idle_thread_attr, PTHREAD_INHERIT_SCHED);

  pthread_mutexattr_init(&dial->idle_mutex_attr);
  //  pthread_mutexattr_setprotocol(&dial->idle_mutex_attr, PTHREAD_PRIO_INHERIT);
  pthread_mutex_init(&dial->idle_mutex, &dial->idle_mutex_attr);

  pthread_cond_init(&(dial->idle_cond), NULL);

  dial->gravity_x = 0.0;
  dial->gravity_y = 0.0;
  dial->current_x = 0.0;
  dial->current_y = 0.0;
}

void
ags_dial_realize(GtkWidget *widget)
{
  AgsDial *dial;
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint buttons_width;
  gint border_left, border_top;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (AGS_IS_DIAL (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
  dial = AGS_DIAL (widget);

  /* calculate some display dependend fields */
  buttons_width = 0;

  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    cairo_t *cr;
    cairo_text_extents_t te_up, te_down;

    cr = gdk_cairo_create(widget->parent->window);
    
    cairo_select_font_face (cr, "Georgia\0",
			    CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, (gdouble) dial->font_size);
    cairo_text_extents (cr, "-\0", &te_down);
    cairo_text_extents (cr, "+\0", &te_up);
    
    if(te_down.width < te_up.width){
      dial->button_width = te_up.width * 3;
    }else{
      dial->button_width = te_down.width * 3;
    }
    
    buttons_width = 2 * dial->button_width;

    if(te_down.height < te_up.height){
      dial->button_height = te_up.height * 2;
    }else{
      dial->button_height = te_down.height * 2;
    }

    cairo_destroy(cr);
  }
  
  gtk_widget_set_size_request(widget,
			      2 * dial->radius + 2 * dial->outline_strength + buttons_width,
			      2 * dial->radius + 2 * dial->outline_strength);

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

void
ags_dial_size_request(GtkWidget *widget,
		      GtkRequisition *requisition)
{
  GTK_WIDGET_CLASS(ags_dial_parent_class)->size_request(widget, requisition);

  /* implement me */
  //TODO:JK:
}

void
ags_dial_size_allocate(GtkWidget *widget,
		       GtkAllocation *allocation)
{
  GTK_WIDGET_CLASS(ags_dial_parent_class)->size_allocate(widget, allocation);

  /* implement me */
  //TODO:JK:
}

gboolean
ags_dial_expose(GtkWidget *widget,
		GdkEventExpose *event)
{
  GTK_WIDGET_CLASS(ags_dial_parent_class)->expose_event(widget, event);

  ags_dial_draw(AGS_DIAL(widget));

  return(FALSE);
}

gboolean
ags_dial_button_press(GtkWidget *widget,
		      GdkEventButton *event)
{
  AgsDial *dial;
  gint border_left, border_top;
  gint dial_left_position;
  auto gboolean ags_dial_button_press_is_down_event();
  auto gboolean ags_dial_button_press_is_up_event();
  auto gboolean ags_dial_button_press_is_dial_event();
  gboolean ags_dial_button_press_is_down_event(){
    if(event->x >= border_left &&
       event->x <= border_left + dial->button_width &&
       event->y >= border_top + 2 * dial->radius + 2 * dial->outline_strength - dial->button_height &&
       event->y <= border_top + 2 * dial->radius + 2 * dial->outline_strength){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }
  gboolean ags_dial_button_press_is_up_event(){
    gint offset;

    offset = border_left + dial->button_width + 2 * dial->radius + dial->margin_left + dial->margin_right;

    if(event->x >= offset &&
       event->x <= offset + dial->button_width &&
       event->y >= border_top + 2 * dial->radius + 2 * dial->outline_strength - dial->button_height &&
       event->y <= border_top + 2 * dial->radius + 2 * dial->outline_strength){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }
  gboolean ags_dial_button_press_is_dial_event(){
    if(event->x >= dial_left_position &&
       event->x <= dial_left_position + 2 * dial->radius + 2 * dial->outline_strength){
      if((cos(event->y) < 0.0 && cos(event->y) >= -1.0) ||
	 (sin(event->y) > 0.0 && sin(event->y) <= 1.0) ||
	 (cos(event->y) < 0.0 && sin(event->y) >= -1.0) ||
	 (sin(event->y) < 0.0 && cos(event->y) >= -1.0)){
	return(TRUE);
      }else{
	return(FALSE);
      }
    }else{
      return(FALSE);
    }
  }

  GTK_WIDGET_CLASS(ags_dial_parent_class)->button_press_event(widget, event);

  dial = AGS_DIAL(widget);
  dial->flags |= AGS_DIAL_MOUSE_BUTTON_PRESSED;

  //TODO:JK: retrieve borders
  border_left = 0;
  border_top = 0;

  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    if(ags_dial_button_press_is_down_event()){
      dial->flags |= AGS_DIAL_BUTTON_DOWN_PRESSED;
    }else if(ags_dial_button_press_is_up_event()){
      dial->flags |= AGS_DIAL_BUTTON_UP_PRESSED;
    }else{
      dial_left_position = border_left + dial->button_width;

      if(ags_dial_button_press_is_dial_event()){
	dial->flags |= AGS_DIAL_MOTION_CAPTURING;
      }
    }
  }else{
    dial_left_position = border_left;

    if(ags_dial_button_press_is_dial_event()){
      dial->gravity_x = event->x;
      dial->gravity_y = event->y;
      dial->current_x = event->x;
      dial->current_y = event->y;

      dial->flags |= AGS_DIAL_MOTION_CAPTURING_INIT;
      dial->flags |= AGS_DIAL_MOTION_CAPTURING;
    }
  }

  return(FALSE);
}

gboolean
ags_dial_button_release(GtkWidget *widget,
			GdkEventButton *event)
{
  AgsDial *dial;

  GTK_WIDGET_CLASS(ags_dial_parent_class)->button_release_event(widget, event);

  dial = AGS_DIAL(widget);
  dial->flags &= (~AGS_DIAL_MOUSE_BUTTON_PRESSED);

  if((AGS_DIAL_BUTTON_DOWN_PRESSED & (dial->flags)) != 0){
    GtkAdjustment *adjustment;

    adjustment = dial->adjustment;

    if(adjustment->value > adjustment->lower){
      gtk_adjustment_set_value(adjustment,
			       adjustment->value - adjustment->step_increment);

      ags_dial_draw(dial);
    }

    dial->flags &= (~AGS_DIAL_BUTTON_DOWN_PRESSED);
  }else if((AGS_DIAL_BUTTON_UP_PRESSED & (dial->flags)) != 0){
    GtkAdjustment *adjustment;

    adjustment = dial->adjustment;

    if(adjustment->value < adjustment->upper){
      gtk_adjustment_set_value(adjustment,
			       adjustment->value + adjustment->step_increment);

      ags_dial_draw(dial);
    }

    dial->flags &= (~AGS_DIAL_BUTTON_UP_PRESSED);
  }else if((AGS_DIAL_MOTION_CAPTURING & (dial->flags)) != 0){
    dial->flags &= (~AGS_DIAL_MOTION_CAPTURING);
  }

  return(FALSE);
}

gboolean
ags_dial_motion_notify(GtkWidget *widget,
		       GdkEventMotion *event)
{
  AgsDial *dial;
  auto void ags_dial_motion_notify_do_dial();
  void ags_dial_motion_notify_do_dial(){
    GtkAdjustment *adjustment;
    gboolean gravity_up;

    adjustment = dial->adjustment;

    if(dial->gravity_y < dial->current_y){
      if(dial->gravity_x < dial->current_x){
	gravity_up = ((dial->gravity_x - dial->current_x) * dial->tolerance < (dial->gravity_y - dial->current_y) * dial->tolerance) ? TRUE: FALSE;
      }else{
	gravity_up = (-1 * (dial->gravity_x - dial->current_x) * dial->tolerance < (dial->gravity_y - dial->current_y) * dial->tolerance) ? TRUE: FALSE;
      }
    }else{
      if(dial->gravity_x < dial->current_x){
	gravity_up = ((dial->gravity_x - dial->current_x) * dial->tolerance < -1 * (dial->gravity_y - dial->current_y) * dial->tolerance) ? TRUE: FALSE;
      }else{
	gravity_up = ((dial->gravity_x - dial->current_x) * dial->tolerance < (dial->gravity_y - dial->current_y) * dial->tolerance) ? TRUE: FALSE;
      }
    }

    if(!gravity_up){
      if(adjustment->value > adjustment->lower){
	gtk_adjustment_set_value(adjustment,
				 adjustment->value - adjustment->step_increment);
	
	ags_dial_draw(dial);
	pthread_create(&(dial->idle_thread), NULL, &ags_dial_idle, dial);
	//	usleep(dial->sleep_interval);
      }
    }else{
      if(adjustment->value < adjustment->upper){
	gtk_adjustment_set_value(adjustment,
				 adjustment->value + adjustment->step_increment);
	
	ags_dial_draw(dial);
	pthread_create(&(dial->idle_thread), NULL, &ags_dial_idle, dial);
	//	usleep(dial->sleep_interval);
      }
    }
  }

  GTK_WIDGET_CLASS(ags_dial_parent_class)->motion_notify_event(widget, event);

  dial = AGS_DIAL(widget);

  if((AGS_DIAL_IDLE & (dial->flags)) != 0)
    return(TRUE);

  if((AGS_DIAL_MOTION_CAPTURING & (dial->flags)) != 0){
    if((AGS_DIAL_MOTION_CAPTURING_INIT & (dial->flags)) != 0){
      dial->current_x = event->x;
      dial->current_y = event->y;

      dial->flags &= (~AGS_DIAL_MOTION_CAPTURING_INIT);

      ags_dial_motion_notify_do_dial();
    }else{
      dial->gravity_x = dial->current_x;
      dial->gravity_y = dial->current_y;
      dial->current_x = event->x;
      dial->current_y = event->y;

      ags_dial_motion_notify_do_dial();
    }
  }

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
ags_dial_draw(AgsDial *dial)
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
  gdouble translated_value;
  guint scale_precision;
  guint i;

  widget = GTK_WIDGET(dial);

  cr = gdk_cairo_create(widget->window);

  radius = (gdouble) dial->radius;
  outline_strength = (gdouble) dial->outline_strength;

  margin_left = (gdouble) dial->margin_left;
  margin_right = (gdouble) dial->margin_right;

  cairo_select_font_face (cr, "Georgia\0",
			  CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size (cr, (gdouble) dial->font_size);
  cairo_text_extents (cr, "-\0", &te_down);
  cairo_text_extents (cr, "+\0", &te_up);

  button_width = dial->button_width;
  button_height = dial->button_height;

  /* draw controller button down */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(cr, 2.0);

  cairo_rectangle(cr,
		  1.0, (2.0 * radius) - button_height + outline_strength,
		  button_width, button_height);
  cairo_stroke(cr);

  cairo_move_to (cr,
		 1.0 + 0.5 - te_down.width / 2.0 - te_down.x_bearing + button_width / 2.25,
		 0.5 - te_down.height / 2.0 - te_down.y_bearing + (radius * 2.0) - button_height / 2.0 + outline_strength);
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
		radius + 2.0 * outline_strength - 2.0 - outline_strength);
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
		-1 * sin((0.35 * M_PI) / (0.65 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		1.0 + -1 * cos((0.35 * M_PI) / (0.65 * M_PI)) + button_width + margin_left + radius,
		sin((0.65 * M_PI) / (0.35 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		1.0 + button_width + margin_left + radius,
		radius + 2.0 * outline_strength - 1.0);
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

  scale_precision = (gdouble) dial->scale_precision;
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

  /* draw value */
  translated_value = (gdouble) scale_precision / dial->adjustment->upper * dial->adjustment->value;

  //  g_message("value: %f\nupper: %f\ntranslated_value: %f\n\0", GTK_RANGE(dial)->adjustment->value, GTK_RANGE(dial)->adjustment->upper, translated_value);
  cairo_set_line_width(cr, 4.0);
  cairo_set_source_rgba (cr, 0.6, 0.0, 0.0, 1.0);

  cairo_arc (cr,
	     1.0 + button_width + margin_left + radius,
	     radius + outline_strength,
	     radius - (outline_strength + 4.0) / M_PI,
	     starter_angle + (translated_value * scale_inverted_width) + (translated_value * scale_width),
	     starter_angle + (translated_value * scale_inverted_width) + (translated_value * scale_width) + scale_width);
  cairo_stroke(cr);


  /* draw controller button up */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
  cairo_set_line_width(cr, 2.0);

  cairo_rectangle(cr,
		  1.0 + (2.0 * radius) + button_width + margin_left + margin_right,
		  (2.0 * radius) - button_height + outline_strength,
		  button_width, button_height);
  cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
  cairo_stroke(cr);

  cairo_move_to (cr,
		 1.0 + 0.5 - te_up.width / 2.0 - te_up.x_bearing + (radius * 2.0) + margin_left + margin_right + button_width + button_width / 2.25,
		 0.5 - te_up.height / 2.0 - te_up.y_bearing + (radius * 2.0) - button_height / 2.0 + outline_strength);
  cairo_show_text (cr, "+\0");

  cairo_destroy(cr);
}

void*
ags_dial_idle(void *dial0)
{
  AgsDial *dial;

  dial = AGS_DIAL(dial0);

  usleep(dial->sleep_interval);
  dial->flags &= (~AGS_DIAL_IDLE);

  return(NULL);
}

AgsDial*
ags_dial_new()
{
  AgsDial *dial;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  dial = (AgsDial *) g_object_new(AGS_TYPE_DIAL,
				  "adjustment\0", adjustment,
				  NULL);
  
  return(dial);
}
