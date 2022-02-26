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

#include "ags_dial.h"

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#include <math.h>

void ags_dial_class_init(AgsDialClass *dial);
void ags_dial_init(AgsDial *dial);
void ags_dial_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_dial_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);

void ags_dial_realize(GtkWidget *widget);

void ags_dial_measure(GtkWidget *widget,
		      GtkOrientation orientation,
		      int for_size,
		      int *minimum,
		      int *natural,
		      int *minimum_baseline,
		      int *natural_baseline);
void ags_dial_size_allocate(GtkWidget *widget,
			    int width,
			    int height,
			    int baseline);

void ags_dial_frame_clock_update_callback(GdkFrameClock *frame_clock,
					  AgsDial *dial);

void ags_dial_snapshot(GtkWidget *widget,
		       GtkSnapshot *snapshot);

gboolean ags_dial_button_press_is_down_event(AgsDial *dial,
					     gdouble x, gdouble y,
					     gint padding_left, guint padding_top);
gboolean ags_dial_button_press_is_up_event(AgsDial *dial,
					   gdouble x, gdouble y,
					   gint padding_left, guint padding_top);
gboolean ags_dial_button_press_is_dial_event(AgsDial *dial,
					     gdouble x, gdouble y,
					     gint padding_left, guint padding_top,
					     gint dial_left_position);

gboolean ags_dial_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						 gint n_press,
						 gdouble x,
						 gdouble y,
						 AgsDial *dial);
gboolean ags_dial_gesture_click_released_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsDial *dial);

gboolean ags_dial_key_pressed_callback(GtkEventControllerKey *event_controller,
				       guint keyval,
				       guint keycode,
				       GdkModifierType state,
				       AgsDial *dial);
gboolean ags_dial_key_released_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsDial *dial);
gboolean ags_dial_modifiers_callback(GtkEventControllerKey *event_controller,
				     GdkModifierType keyval,
				     AgsDial *dial);

void ags_dial_motion_notify_do_dial(AgsDial *dial,
				    gdouble x,
				    gdouble y);
void ags_dial_motion_notify_do_seemless_dial(AgsDial *dial,
					     gdouble x,
					     gdouble y);

gboolean ags_dial_motion_callback(GtkEventControllerMotion *event_controller,
				  gdouble x,
				  gdouble y,
				  AgsDial *dial);

void ags_dial_draw(AgsDial *dial,
		   cairo_t *cr,
		   gboolean is_animation);

void ags_dial_adjustment_changed_callback(GtkAdjustment *adjustment,
					  AgsDial *dial);

/**
 * SECTION:ags_dial
 * @short_description: A dial widget
 * @title: AgsDial
 * @section_id:
 * @include: ags/widget/ags_dial.h
 *
 * #AgsDial is a widget representing a #GtkAdjustment.
 */

enum{
  VALUE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_RADIUS,
  PROP_OUTLINE_STRENGTH,
  PROP_FONT_SIZE,
  PROP_BUTTON_WIDTH,
  PROP_BUTTON_HEIGHT,
  PROP_MARGIN_LEFT,
  PROP_MARGIN_RIGHT,
  PROP_ADJUSTMENT,
  PROP_SCALE_PRECISION,
};

static gpointer ags_dial_parent_class = NULL;
static guint dial_signals[LAST_SIGNAL];

GType
ags_dial_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_dial = 0;

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
					   "AgsDial", &ags_dial_info,
					   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_dial);
  }

  return g_define_type_id__volatile;
}

void
ags_dial_class_init(AgsDialClass *dial)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_dial_parent_class = g_type_class_peek_parent(dial);

  /* GObjectClass */
  gobject = (GObjectClass *) dial;

  gobject->set_property = ags_dial_set_property;
  gobject->get_property = ags_dial_get_property;

  /* properties */
  /**
   * AgsDial:radius:
   *
   * The radius of the knob.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("radius",
				 "radius",
				 "The knob's radius",
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_RADIUS,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RADIUS,
				  param_spec);

  /**
   * AgsDial:outline-strength:
   *
   * The outline strength of the knob.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("outline-strength",
				 "outline strength",
				 "The knob's outline strength",
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_OUTLINE_STRENGTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTLINE_STRENGTH,
				  param_spec);

  /**
   * AgsDial:font-size:
   *
   * The font size of the buttons.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("font-size",
				 "font size",
				 "The buttons font-size",
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_FONT_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FONT_SIZE,
				  param_spec);

  /**
   * AgsDial:button-width:
   *
   * The buttons width.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("button-width",
				"buttons width",
				"The buttons width",
				-1,
				G_MAXINT,
				AGS_DIAL_DEFAULT_BUTTON_WIDTH,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUTTON_WIDTH,
				  param_spec);

  /**
   * AgsDial:button-height:
   *
   * The buttons height.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("button-height",
				"buttons height",
				"The buttons height",
				-1,
				G_MAXINT,
				AGS_DIAL_DEFAULT_BUTTON_HEIGHT,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUTTON_HEIGHT,
				  param_spec);

  
  /**
   * AgsDial:margin-left:
   *
   * The button's margin left.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("margin-left",
				"button's margin left",
				"The button's margin left",
				-1,
				G_MAXINT,
				AGS_DIAL_DEFAULT_MARGIN_LEFT,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_LEFT,
				  param_spec);

  /**
   * AgsDial:margin-right:
   *
   * The button's margin right.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_int("margin-right",
				"button's margin right",
				"The button's margin right",
				-1,
				G_MAXINT,
				AGS_DIAL_DEFAULT_MARGIN_RIGHT,
				G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MARGIN_RIGHT,
				  param_spec);

  /**
   * AgsDial:adjustment:
   *
   * The adjustment storing current value and boundaries.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("adjustment",
				   "assigned adjustment",
				   "The adjustment it is assigned with",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ADJUSTMENT,
				  param_spec);

  /**
   * AgsDial:scale-precision:
   *
   * The precision of the scale.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("scale-precision",
				 "scale precision",
				 "The precision of the scale",
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_PRECISION,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_PRECISION,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) dial;

  widget->realize = ags_dial_realize;

  widget->measure = ags_dial_measure;
  widget->size_allocate = ags_dial_size_allocate;
  
  widget->snapshot = ags_dial_snapshot;

  /* AgsDialClass */
  dial->value_changed = NULL;

  /* signals */
  /**
   * AgsDial::value-changed:
   * @dial: the #AgsDial
   *
   * The ::value-changed signal notifies adjustment value changed.
   *
   * Since: 3.0.0
   */
  dial_signals[VALUE_CHANGED] =
    g_signal_new("value-changed",
		 G_TYPE_FROM_CLASS(dial),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDialClass, value_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_dial_init(AgsDial *dial)
{
  GtkEventController *event_controller;
  
  gtk_widget_set_can_focus((GtkWidget *) dial,
			   TRUE);

  gtk_widget_set_hexpand(dial,
			 TRUE);
  
  gtk_widget_set_vexpand(dial,
			 TRUE);  

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) dial,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_dial_key_pressed_callback), dial);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_dial_key_released_callback), dial);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_dial_modifiers_callback), dial);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) dial,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_dial_gesture_click_pressed_callback), dial);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_dial_gesture_click_released_callback), dial);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) dial,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_dial_motion_callback), dial);
  
  dial->flags = (AGS_DIAL_WITH_BUTTONS |
		 AGS_DIAL_SEEMLESS_MODE |
		 AGS_DIAL_INVERSE_LIGHT);

  dial->radius = AGS_DIAL_DEFAULT_RADIUS;
  dial->scale_precision = AGS_DIAL_DEFAULT_PRECISION;
  dial->scale_max_precision = AGS_DIAL_DEFAULT_PRECISION;
  dial->outline_strength = AGS_DIAL_DEFAULT_OUTLINE_STRENGTH;

  dial->font_size = AGS_DIAL_DEFAULT_FONT_SIZE;
  dial->button_width = AGS_DIAL_DEFAULT_BUTTON_WIDTH;
  dial->button_height = AGS_DIAL_DEFAULT_BUTTON_HEIGHT;
  dial->margin_left = AGS_DIAL_DEFAULT_MARGIN_LEFT;
  dial->margin_right = AGS_DIAL_DEFAULT_MARGIN_RIGHT;

  dial->tolerance = 0.9;
  dial->negated_tolerance = 1.1;
  
  dial->adjustment = NULL;

  dial->gravity_x = 0.0;
  dial->gravity_y = 0.0;
  dial->current_x = 0.0;
  dial->current_y = 0.0;
}

void
ags_dial_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsDial *dial;

  dial = AGS_DIAL(gobject);

  switch(prop_id){
  case PROP_RADIUS:
    {
      dial->radius = g_value_get_uint(value);
    }
    break;
  case PROP_OUTLINE_STRENGTH:
    {
      dial->outline_strength = g_value_get_uint(value);
    }
    break;
  case PROP_FONT_SIZE:
    {
      dial->font_size = g_value_get_uint(value);
    }
    break;
  case PROP_BUTTON_WIDTH:
    {
      dial->button_width = g_value_get_int(value);
    }
    break;
  case PROP_BUTTON_HEIGHT:
    {
      dial->button_height = g_value_get_int(value);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      dial->margin_left = g_value_get_int(value);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      dial->margin_right = g_value_get_int(value);
    }
    break;
  case PROP_ADJUSTMENT:
    {
      GtkAdjustment *adjustment;

      adjustment = (GtkAdjustment *) g_value_get_object(value);

      if(dial->adjustment == adjustment){
	return;
      }

      if(dial->adjustment != NULL){
	g_object_unref(G_OBJECT(dial->adjustment));
      }

      if(adjustment != NULL){
	g_object_ref(G_OBJECT(adjustment));
	g_signal_connect(adjustment, "value-changed",
			 G_CALLBACK(ags_dial_adjustment_changed_callback), dial);
      }

      dial->adjustment = adjustment;
    }
    break;
  case PROP_SCALE_PRECISION:
    {
      guint scale_precision;

      scale_precision = g_value_get_uint(value);

      if(scale_precision > dial->scale_max_precision){
	dial->scale_precision = dial->scale_max_precision;
      }else if(scale_precision == 0){
	dial->scale_precision = 1;
      }else{
	dial->scale_precision = scale_precision;
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dial_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsDial *dial;

  dial = AGS_DIAL(gobject);

  switch(prop_id){
  case PROP_RADIUS:
    {
      g_value_set_uint(value, dial->radius);
    }
    break;
  case PROP_OUTLINE_STRENGTH:
    {
      g_value_set_uint(value, dial->outline_strength);
    }
    break;
  case PROP_FONT_SIZE:
    {
      g_value_set_uint(value, dial->font_size);
    }
    break;
  case PROP_BUTTON_WIDTH:
    {
      g_value_set_int(value, dial->button_width);
    }
    break;
  case PROP_BUTTON_HEIGHT:
    {
      g_value_set_int(value, dial->button_height);
    }
    break;
  case PROP_MARGIN_LEFT:
    {
      g_value_set_int(value, dial->margin_left);
    }
    break;
  case PROP_MARGIN_RIGHT:
    {
      g_value_set_int(value, dial->margin_right);
    }
    break;
  case PROP_ADJUSTMENT:
  {
    g_value_set_object(value, dial->adjustment);
  }
  break;
  case PROP_SCALE_PRECISION:
    {
      g_value_set_uint(value, dial->scale_precision);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dial_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_dial_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_dial_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}

void
ags_dial_measure(GtkWidget *widget,
		 GtkOrientation orientation,
		 int for_size,
		 int *minimum,
		 int *natural,
		 int *minimum_baseline,
		 int *natural_baseline)
{
  AgsDial *dial;

  dial = (AgsDial *) widget;
  
  if(orientation == GTK_ORIENTATION_VERTICAL){
    minimum[0] =
      natural[0] = 2 * (dial->radius + dial->outline_strength + 1);
  }else{
    minimum[0] =
      natural[0] = 2 * (dial->button_width + dial->radius + dial->outline_strength + 1);
  }
}

void
ags_dial_size_allocate(GtkWidget *widget,
		       int width,
		       int height,
		       int baseline)
{
  AgsDial *dial;

  dial = (AgsDial *) widget;

  width = 2 * (dial->button_width + dial->radius + dial->outline_strength + 1);
  height = 2 * (dial->radius + dial->outline_strength + 1);
  
  GTK_WIDGET_CLASS(ags_dial_parent_class)->size_allocate(widget,
							 width,
							 height,
							 baseline);
}

void
ags_dial_frame_clock_update_callback(GdkFrameClock *frame_clock,
				     AgsDial *dial)
{
  gtk_widget_queue_draw((GtkWidget *) dial);
}

void
ags_dial_snapshot(GtkWidget *widget,
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

  ags_dial_draw((AgsDial *) widget,
		cr,
		TRUE);
  
  cairo_destroy(cr);
}

gboolean
ags_dial_button_press_is_down_event(AgsDial *dial,
				    gdouble x, gdouble y,
				    gint padding_left, guint padding_top)
{
  if(x >= padding_left &&
     x <= padding_left + dial->button_width &&
     y >= padding_top + 2 * dial->radius + 2 * dial->outline_strength - dial->button_height &&
     y <= padding_top + 2 * dial->radius + 2 * dial->outline_strength){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_dial_button_press_is_up_event(AgsDial *dial,
				  gdouble x, gdouble y,
				  gint padding_left, guint padding_top)
{
  gint offset;

  offset = padding_left + dial->button_width + 2 * dial->radius + dial->margin_left + dial->margin_right;

  if(x >= offset &&
     x <= offset + dial->button_width &&
     y >= padding_top + 2 * dial->radius + 2 * dial->outline_strength - dial->button_height &&
     y <= padding_top + 2 * dial->radius + 2 * dial->outline_strength){
    return(TRUE);
  }else{
    return(FALSE);
  }
}

gboolean
ags_dial_button_press_is_dial_event(AgsDial *dial,
				    gdouble x, gdouble y,
				    gint padding_left, guint padding_top,
				    gint dial_left_position)
{
  if(x >= dial_left_position &&
     x <= dial_left_position + 2 * dial->radius + 2 * dial->outline_strength){
    if((cos(y) < 0.0 && cos(y) >= -1.0) ||
       (sin(y) > 0.0 && sin(y) <= 1.0) ||
       (cos(y) < 0.0 && sin(y) >= -1.0) ||
       (sin(y) < 0.0 && cos(y) >= -1.0)){
      return(TRUE);
    }else{
      return(FALSE);
    }
  }else{
    return(FALSE);
  }
}

gboolean
ags_dial_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					gint n_press,
					gdouble x,
					gdouble y,
					AgsDial *dial)
{
  gint widget_width, widget_height;
  gdouble button_width, button_height, margin_left, margin_right;
  gdouble radius, outline_strength;
  guint width, height;
  gint padding_left, padding_top;
  gint dial_left_position;

  dial->flags |= AGS_DIAL_MOUSE_BUTTON_PRESSED;

  widget_width = gtk_widget_get_width((GtkWidget *) dial);
  widget_height = gtk_widget_get_height((GtkWidget *) dial);

  button_width = dial->button_width;
  button_height = dial->button_height;

  margin_left = (gdouble) dial->margin_left;
  margin_right = (gdouble) dial->margin_right;

  radius = (gdouble) dial->radius;
  outline_strength = (gdouble) dial->outline_strength;

  width = 2 * (button_height + radius + outline_strength + 2) + (margin_left + margin_right);
  height = 2 * (radius + outline_strength + 1);

  padding_top = (widget_height - height) / 2;
  padding_left = (widget_width - width) / 2;

  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    if(ags_dial_button_press_is_down_event(dial,
					   x, y,
					   padding_left, padding_top)){
      dial->flags |= AGS_DIAL_BUTTON_DOWN_PRESSED;
    }else if(ags_dial_button_press_is_up_event(dial,
					       x, y,
					       padding_left, padding_top)){
      dial->flags |= AGS_DIAL_BUTTON_UP_PRESSED;
    }else{
      dial_left_position = padding_left + dial->button_width;

      if(ags_dial_button_press_is_dial_event(dial,
					     x, y,
					     padding_left, padding_top,
					     dial_left_position)){
	dial->flags |= AGS_DIAL_MOTION_CAPTURING;
      }
    }
  }else{
    dial_left_position = padding_left;

    if(ags_dial_button_press_is_dial_event(dial,
					   x, y,
					   padding_left, padding_top,
					   dial_left_position)){
      dial->gravity_x = x;
      dial->gravity_y = y;
      dial->current_x = x;
      dial->current_y = y;

      dial->flags |= AGS_DIAL_MOTION_CAPTURING_INIT;
      dial->flags |= AGS_DIAL_MOTION_CAPTURING;
    }
  }

  return(FALSE);
}

gboolean
ags_dial_gesture_click_released_callback(GtkGestureClick *event_controller,
					 gint n_press,
					 gdouble x,
					 gdouble y,
					 AgsDial *dial)
{
  gtk_widget_grab_focus((GtkWidget *) dial);

  dial->flags &= (~AGS_DIAL_MOUSE_BUTTON_PRESSED);

  if((AGS_DIAL_BUTTON_DOWN_PRESSED & (dial->flags)) != 0){
    GtkAdjustment *adjustment;

    adjustment = dial->adjustment;

    if(gtk_adjustment_get_value(adjustment) > gtk_adjustment_get_lower(adjustment)){
      gtk_adjustment_set_value(adjustment,
			       gtk_adjustment_get_value(adjustment) - gtk_adjustment_get_page_increment(adjustment));

      gtk_widget_queue_draw(dial);
    }

    dial->flags &= (~AGS_DIAL_BUTTON_DOWN_PRESSED);
  }else if((AGS_DIAL_BUTTON_UP_PRESSED & (dial->flags)) != 0){
    GtkAdjustment *adjustment;

    adjustment = dial->adjustment;

    if(gtk_adjustment_get_value(adjustment) < gtk_adjustment_get_upper(adjustment)){
      gtk_adjustment_set_value(adjustment,
			       gtk_adjustment_get_value(adjustment) + gtk_adjustment_get_page_increment(adjustment));

      gtk_widget_queue_draw(dial);
    }

    dial->flags &= (~AGS_DIAL_BUTTON_UP_PRESSED);
  }else if((AGS_DIAL_MOTION_CAPTURING & (dial->flags)) != 0){
    dial->flags &= (~AGS_DIAL_MOTION_CAPTURING);
  }

  return(FALSE);
}

gboolean
ags_dial_key_pressed_callback(GtkEventControllerKey *event_controller,
			      guint keyval,
			      guint keycode,
			      GdkModifierType state,
			      AgsDial *dial)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

gboolean
ags_dial_key_released_callback(GtkEventControllerKey *event_controller,
			       guint keyval,
			       guint keycode,
			       GdkModifierType state,
			       AgsDial *dial)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Up:
    case GDK_KEY_uparrow:
    {
      gdouble value, step, upper;

      value = gtk_adjustment_get_value(dial->adjustment);
      step = gtk_adjustment_get_step_increment(dial->adjustment);
      upper = gtk_adjustment_get_upper(dial->adjustment);
      
      if(value + step > upper){
	gtk_adjustment_set_value(dial->adjustment,
				 upper);
      }else{
	gtk_adjustment_set_value(dial->adjustment,
				 value + step);
      }

      gtk_widget_queue_draw((GtkWidget *) dial);
    }
    break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
    {
      gdouble value, step, lower;

      value = gtk_adjustment_get_value(dial->adjustment);
      step = gtk_adjustment_get_step_increment(dial->adjustment);
      lower = gtk_adjustment_get_lower(dial->adjustment);
      
      if(value - step < lower){
	gtk_adjustment_set_value(dial->adjustment,
				 lower);
      }else{
	gtk_adjustment_set_value(dial->adjustment,
				 value - step);
      }

      gtk_widget_queue_draw((GtkWidget *) dial);
    }
    break;
    case GDK_KEY_Page_Up:
    case GDK_KEY_KP_Page_Up:
    {
      gdouble value, page, upper;

      value = gtk_adjustment_get_value(dial->adjustment);
      page = gtk_adjustment_get_page_increment(dial->adjustment);
      upper = gtk_adjustment_get_upper(dial->adjustment);
      
      if(value + page > upper){
	gtk_adjustment_set_value(dial->adjustment,
				 upper);
      }else{
	gtk_adjustment_set_value(dial->adjustment,
				 value + page);
      }

      gtk_widget_queue_draw((GtkWidget *) dial);
    }
    break;
    case GDK_KEY_Page_Down:
    case GDK_KEY_KP_Page_Down:
    {
      gdouble value, page, lower;

      value = gtk_adjustment_get_value(dial->adjustment);
      page = gtk_adjustment_get_page_increment(dial->adjustment);
      lower = gtk_adjustment_get_lower(dial->adjustment);
      
      if(value - page < lower){
	gtk_adjustment_set_value(dial->adjustment,
				 lower);
      }else{
	gtk_adjustment_set_value(dial->adjustment,
				 value - page);
      }

      gtk_widget_queue_draw((GtkWidget *) dial);
    }
    break;
    }
  }
  
  return(key_handled);
}

gboolean
ags_dial_modifiers_callback(GtkEventControllerKey *event_controller,
			    GdkModifierType keyval,
			    AgsDial *dial)
{
  return(FALSE);
}

gboolean
ags_dial_motion_callback(GtkEventControllerMotion *event_controller,
			 gdouble x,
			 gdouble y,
			 AgsDial *dial)
{
  if((AGS_DIAL_MOTION_CAPTURING & (dial->flags)) != 0){
    if((AGS_DIAL_SEEMLESS_MODE & (dial->flags)) != 0){
      if((AGS_DIAL_MOTION_CAPTURING_INIT & (dial->flags)) != 0){
	dial->current_x = x;
	dial->current_y = y;
      }else{
	dial->gravity_x = dial->current_x;
	dial->gravity_y = dial->current_y;
	dial->current_x = x;
	dial->current_y = y;
      }

      ags_dial_motion_notify_do_seemless_dial(dial,
					      x, y);
    }else{
      if((AGS_DIAL_MOTION_CAPTURING_INIT & (dial->flags)) != 0){
	dial->current_x = x;
	dial->current_y = y;

	dial->flags &= (~AGS_DIAL_MOTION_CAPTURING_INIT);

	ags_dial_motion_notify_do_dial(dial,
				       x, y);
      }else{
	dial->gravity_x = dial->current_x;
	dial->gravity_y = dial->current_y;
	dial->current_x = x;
	dial->current_y = y;

	ags_dial_motion_notify_do_dial(dial,
				       x, y);
      }
    }
  }

  return(FALSE);
}

void
ags_dial_motion_notify_do_dial(AgsDial *dial,
			       gdouble x,
			       gdouble y)
{
  GtkAdjustment *adjustment;

  guint i;
  gint sign_one;
  gboolean gravity_up;
    
  static const gboolean movement_matrix[] = {
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    TRUE,
    FALSE,
    TRUE,
    TRUE,
  };      

  adjustment = dial->adjustment;

  gravity_up = FALSE;

  for(i = 0; i < 8 ; i++){
    if(!movement_matrix[i]){
      sign_one = -1;
    }else{
      sign_one = 1;
    }
      
    if((movement_matrix[i] &&
	(sign_one * (dial->gravity_x - dial->current_x) < sign_one * (dial->gravity_y - dial->current_y))) ||
       (!movement_matrix[i] &&
	(sign_one * (dial->gravity_x - dial->current_x) > sign_one * (dial->gravity_y - dial->current_y)))){
      gravity_up = TRUE;
      break;
    }
  }

  if(!gravity_up){
    if(gtk_adjustment_get_value(adjustment) > gtk_adjustment_get_lower(adjustment)){
      gtk_adjustment_set_value(adjustment,
			       gtk_adjustment_get_value(adjustment) - gtk_adjustment_get_step_increment(adjustment));

      gtk_widget_queue_draw(dial);
    }
  }else{
    if(gtk_adjustment_get_value(adjustment) < gtk_adjustment_get_lower(adjustment)){
      gtk_adjustment_set_value(adjustment,
			       gtk_adjustment_get_value(adjustment) + gtk_adjustment_get_step_increment(adjustment));
	
      gtk_widget_queue_draw(dial);
    }
  }
}

void
ags_dial_motion_notify_do_seemless_dial(AgsDial *dial,
					gdouble x,
					gdouble y)
{
  GtkAdjustment *adjustment;

  cairo_t *cr;
    
  gint widget_width, widget_height;
  gdouble button_width, button_height, margin_left, margin_right;
  gdouble radius, outline_strength;
  guint width, height;
  gdouble padding_left, padding_top;
  gdouble range;
  gdouble a0, quarter;
  gdouble x0, y0, x1, y1;
  gdouble translated_x;
  gboolean x_toggled, y_toggled;
    
  //TODO:JK: optimize me
  adjustment = dial->adjustment;

  range = (gtk_adjustment_get_upper(dial->adjustment) - gtk_adjustment_get_lower(dial->adjustment));

  if(range == 0.0){
    return;
  }

  widget_width = gtk_widget_get_width((GtkWidget *) dial);
  widget_height = gtk_widget_get_height((GtkWidget *) dial);
    
  button_width = dial->button_width;
  button_height = dial->button_height;

  margin_left = (gdouble) dial->margin_left;
  margin_right = (gdouble) dial->margin_right;

  radius = (gdouble) dial->radius;
  outline_strength = (gdouble) dial->outline_strength;

  width = 2 * (button_height + radius + outline_strength + 2) + (margin_left + margin_right);
  height = 2 * (radius + outline_strength + 1);

  padding_top = (widget_height - height) / 2;
  padding_left = (widget_width - width) / 2;
    
  x1 = x - (1.0 + dial->button_width + dial->margin_left + radius + padding_left);
  y1 = y - (dial->outline_strength + radius + padding_top);
  y1 *= -1.0;    

  x_toggled = FALSE;
  y_toggled = FALSE;
    
  if(x1 < 0.0){
    x_toggled = TRUE;
  }

  if(y1 < 0.0){
    y_toggled = TRUE;
  }

  if(radius == 0.0){
    return;
  }

  //FIXME:JK: ugly hack
  x1 = round(x1 + 1.0);
  y1 = round(y1 - 1.0);

  if(x1 == 0.0 &&
     y1 == 0.0){
    return;
  }else if(x1 == 0.0){
    a0 = 1.0;

#if 0
    x0 = 0.0;
    y0 = 1.0 * radius;
#endif
  }else if(y1 == 0.0){
    a0 = 0.0;

#if 0
    x0 = 1.0 * radius;
    y0 = 0.0;
#endif
  }else{
    a0 = y1 / x1;
#if 0
    x0 = cos(a0) * radius;
    y0 = sin(a0) * radius;
#endif
  }

  /* origin correction */
#if 0
  if(x_toggled && y_toggled){
    if(x0 > 0.0){
      x0 *= -1.0;
    }
      
    if(y0 > 0.0){
      y0 *= -1.0;
    }
  }else if(y_toggled){
    if(x0 < 0.0){
      x0 *= -1.0;
    }
      
    if(y0 > 0.0){
      y0 *= -1.0;
    }
  }else if(x_toggled){
    if(y0 < 0.0){
      y0 *= -1.0;
    }

    if(x0 > 0.0){
      x0 *= -1.0;
    }
  }else{
    if(x0 < 0.0){
      x0 *= -1.0;
    }
      
    if(y0 < 0.0){
      y0 *= -1.0;
    }
  }
#endif

  if(a0 > 2.0 * M_PI / 4.0){
    a0 = (2.0 * M_PI / 4.0);
  }else if(a0 < -2.0 * M_PI / 4.0){
    a0 = (-2.0 * M_PI / 4.0);
  }
    
  /* translated_x */
  translated_x = a0 * radius;
    
  quarter =  (2.0 * M_PI / 4.0) * radius;
    
  if(x_toggled && y_toggled){
    if(translated_x < 0.0){
      translated_x = quarter + translated_x;
    }else{
      translated_x = quarter - translated_x;
    }
  }else if(y_toggled){
    if(translated_x < 0.0){
      translated_x *= -1.0;
    }
      
    translated_x += 3.0 * quarter;
  }else if(x_toggled){
    if(translated_x < 0.0){
      translated_x *= -1.0;
    }
      
    translated_x += quarter;
  }else{
    if(translated_x < 0.0){
      translated_x = quarter + translated_x;
    }else{
      translated_x = quarter - translated_x;
    }
      
    translated_x += 2.0 * quarter;
  }
    
  translated_x = ((4.0 / 3.0) * range) / (4.0 * quarter) * translated_x;
  translated_x -= (range / 3.0 / 2.0);
  translated_x = gtk_adjustment_get_lower(adjustment) + translated_x;  

  if(translated_x < gtk_adjustment_get_lower(adjustment)){
    translated_x = gtk_adjustment_get_lower(adjustment);
  }else if(translated_x > gtk_adjustment_get_upper(adjustment)){
    translated_x = gtk_adjustment_get_upper(adjustment);
  }

  gtk_adjustment_set_value(adjustment,
			   translated_x);
  gtk_widget_queue_draw(dial);
}

/**
 * ags_dial_draw:
 * @dial: the #AgsDial
 *
 * draws the widget
 *
 * Since: 3.0.0
 */
void
ags_dial_draw(AgsDial *dial,
	      cairo_t *cr,
	      gboolean is_animation)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  cairo_text_extents_t te_up, te_down;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;

  gchar *font_name;
  
  gint widget_width, widget_height;
  gdouble button_width, button_height, margin_left, margin_right;
  gdouble radius, outline_strength;
  guint width, height;
  guint padding_top, padding_left;
  gdouble unused;
  gdouble scale_area, scale_width, scale_inverted_width;
  gdouble starter_angle;
  gdouble translated_value;
  gdouble range;
  guint scale_precision;
  guint i;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean text_success;
  gboolean shadow_success;

  GValue value = {0,};
  
  style_context = gtk_widget_get_style_context((GtkWidget *) dial);

  settings = gtk_settings_get_default();

  font_name = NULL;
  
  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-font-name", &font_name,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

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
    
  text_success = gtk_style_context_lookup_color(style_context,
						"theme_text_color",
						&text_color);

  if(!fg_success ||
     !bg_success ||
     !shadow_success ||
     !text_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");

    gdk_rgba_parse(&shadow_color,
		   "#ffffff40");

    gdk_rgba_parse(&text_color,
		   "#1a1a1a");
  }

  widget_width = gtk_widget_get_width((GtkWidget *) dial);
  widget_height = gtk_widget_get_height((GtkWidget *) dial);

  /* dimension */
  button_width = dial->button_width;
  button_height = dial->button_height;

  margin_left = (gdouble) dial->margin_left;
  margin_right = (gdouble) dial->margin_right;

  radius = (gdouble) dial->radius;
  outline_strength = (gdouble) dial->outline_strength;

  width = 2 * (button_width + radius + outline_strength + 1);
  height = 2 * (radius + outline_strength + 1);
  
  padding_top = (widget_height - height) / 2;
  padding_left = (widget_width - width) / 2;
  
  /*  */
//  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);
  
  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;

    static const gchar *text = "-";
    
    /* draw controller button down */
    cairo_set_source_rgba(cr,
			  shadow_color.red,
			  shadow_color.green,
			  shadow_color.blue,
			  shadow_color.alpha);

    cairo_set_line_width(cr, 2.0);

    cairo_rectangle(cr,
		    (gdouble) padding_left + 1.0, (gdouble) padding_top + (2.0 * radius) - button_height + outline_strength,
		    (gdouble) button_width, (gdouble) button_height);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
    cairo_stroke(cr);

    /* text */
#if 0
    cairo_set_source_rgba(cr,
			  text_color.red,
			  text_color.green,
			  text_color.blue,
			  text_color.alpha);
#else
    cairo_set_source_rgba(cr,
			  fg_color.red,
			  fg_color.green,
			  fg_color.blue,
			  fg_color.alpha);
#endif
    
    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  text,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    dial->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			      &ink_rect,
			      &logical_rect);

    cairo_move_to(cr,
		  padding_left + 1.0 + 0.5 - (logical_rect.width / PANGO_SCALE) / 2.0 + button_width / 2.25,
		  padding_top + 0.5 - (logical_rect.height / PANGO_SCALE) / 2.0 + (radius * 2.0) - button_height / 2.0 + outline_strength - 1.0);

    pango_cairo_show_layout(cr,
			    layout);

    g_object_unref(layout);
  }
  
  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    PangoLayout *layout;
    PangoFontDescription *desc;

    PangoRectangle ink_rect, logical_rect;
    
    static const gchar *text = "+";
    
    /* draw controller button up */
    cairo_set_source_rgba(cr,
			  shadow_color.red,
			  shadow_color.green,
			  shadow_color.blue,
			  shadow_color.alpha);

    cairo_set_line_width(cr, 2.0);

    cairo_rectangle(cr,
		    padding_left + 1.0 + (2.0 * radius) + button_width + margin_left + margin_right, padding_top + (2.0 * radius) - button_height + outline_strength,
		    button_width, button_height);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
    cairo_stroke(cr);

    /* text */
#if 0
    cairo_set_source_rgba(cr,
			  text_color.red,
			  text_color.green,
			  text_color.blue,
			  text_color.alpha);
#else
    cairo_set_source_rgba(cr,
			  fg_color.red,
			  fg_color.green,
			  fg_color.blue,
			  fg_color.alpha);
#endif

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout,
			  text,
			  -1);
    desc = pango_font_description_from_string(font_name);
    pango_font_description_set_size(desc,
				    dial->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout,
				      desc);
    pango_font_description_free(desc);    

    pango_layout_get_extents(layout,
			      &ink_rect,
			      &logical_rect);

    cairo_move_to(cr,
		  padding_left + 1.0 + 0.5 - (logical_rect.width / PANGO_SCALE) / 2.0 + (radius * 2.0) + margin_left + margin_right + button_width + button_width / 2.25,
		  padding_top + 0.5 - (logical_rect.height / PANGO_SCALE) / 2.0 + (radius * 2.0) - button_height / 2.0 + outline_strength - 1.0);

    pango_cairo_show_layout(cr,
			    layout);

    g_object_unref(layout);
  }

  /* border fill * /
  cairo_set_source_rgb(cr,
		       dial_style->fg[0].red / white_gc,
		       dial_style->fg[0].green / white_gc,
		       dial_style->fg[0].blue / white_gc);
  cairo_arc(cr,
	    1.0 + button_width + margin_left + radius,
	    radius + outline_strength,
	    radius + 2.0,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_stroke(cr);
  */

  /* dial disk */
  cairo_set_source_rgba(cr,
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);
  
  cairo_arc(cr,
	    padding_left + 1.0 + button_width + margin_left + radius,
	    padding_top + radius + outline_strength,
	    radius + outline_strength - 1.25,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_fill(cr);
  
  /* dial disk border */
  cairo_set_source_rgba(cr,
			shadow_color.red,
			shadow_color.green,
			shadow_color.blue,
			shadow_color.alpha);

  cairo_arc(cr,
	    padding_left + 1.0 + button_width + margin_left + radius,
	    padding_top + radius + outline_strength,
	    radius,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_stroke(cr);
  
  /* light effect */
  if((AGS_DIAL_INVERSE_LIGHT & (dial->flags)) != 0){
    cairo_set_source_rgba(cr,
			  0.0,
			  0.0,
			  0.0,
			  1.0 / 3.0);
  }else{
    cairo_set_source_rgba(cr,
			  1.0,
			  1.0,
			  1.0,
			  1.0 / 3.0);
  }
  
  cairo_line_to(cr,
		padding_left + 1.0 + button_width + margin_left + radius,
		padding_top + radius + 2.0 * outline_strength - 2.0 - outline_strength);
  cairo_line_to(cr,
		padding_left + 1.0 + sin(-1 * (0.35 * M_PI) / (0.65 * M_PI)) + button_width + margin_left + radius,
		padding_top + cos((0.65 * M_PI) / (0.35 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		padding_left + 1.0 + sin((0.65 * M_PI) / (0.35 * M_PI)) + button_width + margin_left + radius,
		padding_top + -1 * cos((0.35 * M_PI) / (0.65 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_close_path(cr);

  cairo_arc(cr,
	    padding_left + 1.0 + button_width + margin_left + radius,
	    padding_top + radius + outline_strength,
	    radius,
	    0.35 * M_PI,
	    0.65 * M_PI);
  cairo_fill(cr);

  cairo_line_to(cr,
		padding_left + 1.0 + cos((0.65 * M_PI) / (0.35 * M_PI)) + button_width + margin_left + radius,
		padding_top + -1 * sin((0.35 * M_PI) / (0.65 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		padding_left + 1.0 + -1 * cos((0.35 * M_PI) / (0.65 * M_PI)) + button_width + margin_left + radius,
		padding_top + sin((0.65 * M_PI) / (0.35 * M_PI)) + radius + 2.0 * outline_strength - 1.0);
  cairo_line_to(cr,
		padding_left + 1.0 + button_width + margin_left + radius,
		padding_top + radius + 2.0 * outline_strength - 1.0);
  cairo_close_path(cr);

  cairo_arc(cr,
	    padding_left + 1.0 + button_width + margin_left + radius,
	    padding_top + radius + outline_strength,
	    radius,
	    -0.65 * M_PI,
	    -0.35 * M_PI);
  cairo_fill(cr);

  /* outline */
  cairo_set_source_rgba(cr,
			0.0,
			0.0,
			0.0,
			1.0);

  //  cairo_set_line_width(cr, 1.0 - (2.0 / M_PI));
  cairo_set_line_width(cr, 1.0);
  cairo_arc(cr,
	    padding_left + 1.0 + button_width + margin_left + radius,
	    padding_top + radius + outline_strength,
	    radius,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_stroke(cr);

  /* scale */
  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

  cairo_set_line_width(cr, 3.0);

  unused = 0.25 * 2.0 * M_PI;
  starter_angle = (2.0 * M_PI - unused) * 0.5;

  scale_precision = (gdouble) dial->scale_precision;
  scale_inverted_width = (2.0 * (radius + outline_strength) * M_PI - ((radius + outline_strength) * unused)) / scale_precision - 4.0;
  scale_width = (2.0 * (radius + outline_strength) * M_PI - ((radius + outline_strength) * unused)) / scale_precision - scale_inverted_width;

  scale_inverted_width /= (radius + outline_strength);
  scale_width /= (radius + outline_strength);

  for(i = 0; i <= scale_precision; i++){
    cairo_arc(cr,
	      padding_left + 1.0 + button_width + margin_left + radius,
	      padding_top + radius + outline_strength,
	      radius + outline_strength / M_PI,
	      starter_angle + ((gdouble) i * scale_inverted_width) + ((gdouble) i * scale_width),
	      starter_angle + ((gdouble) i * scale_inverted_width) + ((gdouble) i * scale_width) + scale_width);
    cairo_stroke(cr);
  }

  /* draw value */
  range = (gtk_adjustment_get_upper(dial->adjustment) - gtk_adjustment_get_lower(dial->adjustment));

  if(range != 0.0 &&
     gtk_adjustment_get_upper(dial->adjustment) > gtk_adjustment_get_lower(dial->adjustment)){
    /* this is odd */
    translated_value = (gtk_adjustment_get_value(dial->adjustment) - gtk_adjustment_get_lower(dial->adjustment));
    translated_value = (gdouble) scale_precision * (translated_value / range);

    //  g_message("value: %f\nupper: %f\ntranslated_value: %f\n", GTK_RANGE(dial)->adjustment->value, GTK_RANGE(dial)->adjustment->upper, translated_value);
    cairo_set_line_width(cr, 4.0);
    cairo_set_source_rgba(cr,
			  fg_color.red,
			  fg_color.green,
			  fg_color.blue,
			  fg_color.alpha);

    cairo_arc(cr,
	      padding_left + 1.0 + button_width + margin_left + radius,
	      padding_top + radius + outline_strength,
	      radius - (outline_strength + 4.0) / M_PI,
	      starter_angle + (translated_value * scale_inverted_width) + (translated_value * scale_width),
	      starter_angle + (translated_value * scale_inverted_width) + (translated_value * scale_width) + scale_width);
    cairo_stroke(cr);
  }else{
    g_warning("ags_dial.c - invalid range");
  }

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

//  cairo_surface_mark_dirty(cairo_get_target(cr));
}

/**
 * ags_dial_set_radius:
 * @dial: the #AgsDial
 * @radius: the radius
 * 
 * Set radius of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_radius(AgsDial *dial,
		    guint radius)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }

  g_object_set(dial,
	       "radius", radius,
	       NULL);
}

/**
 * ags_dial_get_radius:
 * @dial: the #AgsDial
 * 
 * Get radius of @dial.
 * 
 * Returns: the radius
 * 
 * Since: 3.2.0
 */
guint
ags_dial_get_radius(AgsDial *dial)
{
  guint radius;
  
  if(!AGS_IS_DIAL(dial)){
    return(0);
  }

  g_object_get(dial,
	       "radius", &radius,
	       NULL);

  return(radius);
}

/**
 * ags_dial_set_outline_strength:
 * @dial: the #AgsDial
 * @outline_strength: the outline strength
 * 
 * Set outline strength of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_outline_strength(AgsDial *dial,
			      guint outline_strength)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "outline-strength", outline_strength,
	       NULL);
}

/**
 * ags_dial_get_outline_strength:
 * @dial: the #AgsDial
 * 
 * Get outline strength of @dial.
 * 
 * Returns: the outline _strength
 * 
 * Since: 3.2.0
 */
guint
ags_dial_get_outline_strength(AgsDial *dial)
{
  guint outline_strength;
  
  if(!AGS_IS_DIAL(dial)){
    return(0);
  }
  
  g_object_get(dial,
	       "outline-strength", &outline_strength,
	       NULL);

  return(outline_strength);
}

/**
 * ags_dial_set_scale_precision:
 * @dial: the #AgsDial
 * @scale_precision: the scale precision
 * 
 * Set scale precision of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_scale_precision(AgsDial *dial,
			     guint scale_precision)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }

  g_object_set(dial,
	       "scale-precision", scale_precision,
	       NULL);
}

/**
 * ags_dial_get_scale_precision:
 * @dial: the #AgsDial
 * 
 * Get scale precision of @dial.
 * 
 * Returns: the scale precision
 * 
 * Since: 3.2.0
 */
guint
ags_dial_get_scale_precision(AgsDial *dial)
{
  guint scale_precision;
  
  if(!AGS_IS_DIAL(dial)){
    return(0);
  }

  g_object_get(dial,
	       "scale-precision", &scale_precision,
	       NULL);

  return(scale_precision);
}

/**
 * ags_dial_set_font_size:
 * @dial: the #AgsDial
 * @font_size: the font size
 * 
 * Set font size of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_font_size(AgsDial *dial,
		       guint font_size)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "font-size", font_size,
	       NULL);
}

/**
 * ags_dial_get_font_size:
 * @dial: the #AgsDial
 * 
 * Get font size of @dial.
 * 
 * Returns: the font size
 * 
 * Since: 3.2.0
 */
guint
ags_dial_get_font_size(AgsDial *dial)
{
  guint font_size;
  
  if(!AGS_IS_DIAL(dial)){
    return(0);
  }
  
  g_object_get(dial,
	       "font-size", &font_size,
	       NULL);

  return(font_size);
}

/**
 * ags_dial_set_button_width:
 * @dial: the #AgsDial
 * @button_width: the button width
 * 
 * Set button width of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_button_width(AgsDial *dial,
			  gint button_width)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "button-width", button_width,
	       NULL);
}

/**
 * ags_dial_get_button_width:
 * @dial: the #AgsDial
 * 
 * Get button width of @dial.
 * 
 * Returns: the button width
 * 
 * Since: 3.2.0
 */
gint
ags_dial_get_button_width(AgsDial *dial)
{
  gint button_width;
  
  if(!AGS_IS_DIAL(dial)){
    return(-1);
  }
  
  g_object_get(dial,
	       "button-width", &button_width,
	       NULL);

  return(button_width);
}

/**
 * ags_dial_set_button_height:
 * @dial: the #AgsDial
 * @button_height: the button height
 * 
 * Set button height of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_button_height(AgsDial *dial,
			   gint button_height)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "button-height", button_height,
	       NULL);
}

/**
 * ags_dial_get_button_height:
 * @dial: the #AgsDial
 * 
 * Get button height of @dial.
 * 
 * Returns: the button height
 * 
 * Since: 3.2.0
 */
gint
ags_dial_get_button_height(AgsDial *dial)
{
  gint button_height;
  
  if(!AGS_IS_DIAL(dial)){
    return(-1);
  }
  
  g_object_get(dial,
	       "button-height", &button_height,
	       NULL);

  return(button_height);
}

/**
 * ags_dial_set_margin_left:
 * @dial: the #AgsDial
 * @margin_left: the margin left
 * 
 * Set margin left of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_margin_left(AgsDial *dial,
			 gint margin_left)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "margin-left", margin_left,
	       NULL);
}

/**
 * ags_dial_get_margin_left:
 * @dial: the #AgsDial
 * 
 * Get margin_left of @dial.
 * 
 * Returns: the margin left
 * 
 * Since: 3.2.0
 */
gint
ags_dial_get_margin_left(AgsDial *dial)
{
  gint margin_left;
  
  if(!AGS_IS_DIAL(dial)){
    return(-1);
  }
  
  g_object_set(dial,
	       "margin-left", &margin_left,
	       NULL);

  return(margin_left);
}

/**
 * ags_dial_set_margin_right:
 * @dial: the #AgsDial
 * @margin_right: the margin right
 * 
 * Set margin right of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_margin_right(AgsDial *dial,
			  gint margin_right)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "margin-right", margin_right,
	       NULL);
}

/**
 * ags_dial_get_margin_right:
 * @dial: the #AgsDial
 * 
 * Get margin right of @dial.
 * 
 * Returns: the margin right
 * 
 * Since: 3.2.0
 */
gint
ags_dial_get_margin_right(AgsDial *dial)
{
  gint margin_right;
  
  if(!AGS_IS_DIAL(dial)){
    return(-1);
  }  
  
  g_object_get(dial,
	       "margin-right", &margin_right,
	       NULL);

  return(margin_right);
}

/**
 * ags_dial_set_adjustment:
 * @dial: the #AgsDial
 * @adjustment: (transfer none): the #GtkAdjustment
 * 
 * Set adjustment of @dial.
 * 
 * Since: 3.2.0
 */
void
ags_dial_set_adjustment(AgsDial *dial,
			GtkAdjustment *adjustment)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }
  
  g_object_set(dial,
	       "adjustment", adjustment,
	       NULL);
}

/**
 * ags_dial_get_adjustment:
 * @dial: the #AgsDial
 * 
 * Get adjustment of @dial.
 * 
 * Returns: (transfer full): the #GtkAdjustment
 * 
 * Since: 3.2.0
 */
GtkAdjustment*
ags_dial_get_adjustment(AgsDial *dial)
{
  GtkAdjustment *adjustment;
  
  if(!AGS_IS_DIAL(dial)){
    return(NULL);
  }
  
  g_object_get(dial,
	       "adjustment", &adjustment,
	       NULL);

  return(adjustment);
}

/**
 * ags_dial_value_changed:
 * @dial: the #AgsDial
 *
 * draws the widget
 *
 * Since: 3.0.0
 */
void
ags_dial_value_changed(AgsDial *dial)
{
  g_return_if_fail(AGS_IS_DIAL(dial));

  g_object_ref((GObject *) dial);
  g_signal_emit(G_OBJECT(dial),
		dial_signals[VALUE_CHANGED], 0);
  g_object_unref((GObject *) dial);
}

void
ags_dial_adjustment_changed_callback(GtkAdjustment *adjustment,
				     AgsDial *dial)
{
  ags_dial_value_changed(dial);
}

/**
 * ags_dial_set_value:
 * @dial: the #AgsDial
 * @value: the value to set
 * 
 * Set value
 *
 * Since: 3.0.0
 */
void
ags_dial_set_value(AgsDial *dial,
		   gdouble value)
{
  if(!AGS_IS_DIAL(dial)){
    return;
  }

  gtk_adjustment_set_value(dial->adjustment,
			   value);
  gtk_widget_queue_draw((GtkWidget *) dial);
}

/**
 * ags_dial_get_value:
 * @dial: the #AgsDial
 * 
 * Get value
 *
 * Returns: the value
 * 
 * Since: 3.14.0
 */
gdouble
ags_dial_get_value(AgsDial *dial)
{
  if(!AGS_IS_DIAL(dial)){
    return(0.0);
  }

  return(gtk_adjustment_get_value(dial->adjustment));
}

/**
 * ags_dial_new:
 *
 * Creates an #AgsDial
 *
 * Returns: a new #AgsDial
 *
 * Since: 3.0.0
 */
AgsDial*
ags_dial_new()
{
  AgsDial *dial;
  GtkAdjustment *adjustment;

  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.1, 0.0);

  dial = (AgsDial *) g_object_new(AGS_TYPE_DIAL,
				  "adjustment", adjustment,
				  NULL);
  
  return(dial);
}
