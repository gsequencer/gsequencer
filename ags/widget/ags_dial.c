/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <atk/atk.h>

#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#include <math.h>

static GType ags_accessible_dial_get_type(void);
void ags_dial_class_init(AgsDialClass *dial);
void ags_accessible_dial_class_init(AtkObject *object);
void ags_accessible_dial_value_interface_init(AtkValueIface *value);
void ags_accessible_dial_action_interface_init(AtkActionIface *action);
void ags_dial_init(AgsDial *dial);
void ags_dial_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_dial_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
AtkObject* ags_dial_get_accessible(GtkWidget *widget);
void ags_dial_show(GtkWidget *widget);

void ags_accessible_dial_get_value_and_text(AtkValue *value,
					    gdouble *current_value,
					    gchar **text);
#ifdef HAVE_ATK_2_12  
AtkRange* ags_accessible_dial_get_range(AtkValue *value);
#endif
gdouble ags_accessible_dial_get_increment(AtkValue *value);
void ags_accessible_dial_set_value(AtkValue *value,
				   gdouble new_value);

gboolean ags_accessible_dial_do_action(AtkAction *action,
				       gint i);
gint ags_accessible_dial_get_n_actions(AtkAction *action);
const gchar* ags_accessible_dial_get_description(AtkAction *action,
						 gint i);
const gchar* ags_accessible_dial_get_name(AtkAction *action,
					  gint i);
const gchar* ags_accessible_dial_get_keybinding(AtkAction *action,
						gint i);
gboolean ags_accessible_dial_set_description(AtkAction *action,
					     gint i);
gchar* ags_accessible_dial_get_localized_name(AtkAction *action,
					      gint i);

void ags_dial_map(GtkWidget *widget);
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
				 GdkEventButton *event);
gboolean ags_dial_key_press(GtkWidget *widget,
			    GdkEventKey *event);
gboolean ags_dial_key_release(GtkWidget *widget,
			      GdkEventKey *event);
gboolean ags_dial_motion_notify(GtkWidget *widget,
				GdkEventMotion *event);

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
  PROP_ADJUSTMENT,
  PROP_SCALE_PRECISION,
};

static gpointer ags_dial_parent_class = NULL;
static guint dial_signals[LAST_SIGNAL];

static GQuark quark_accessible_object = 0;

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

static GType
ags_accessible_dial_get_type(void)
{
  static GType ags_type_accessible_dial = 0;

  if(!ags_type_accessible_dial){
    const GTypeInfo ags_accesssible_dial_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_dial_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_value_interface_info = {
      (GInterfaceInitFunc) ags_accessible_dial_value_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_dial_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_accessible_dial = g_type_register_static(GTK_TYPE_ACCESSIBLE,
						      "AgsAccessibleDial\0", &ags_accesssible_dial_info,
						      0);

    g_type_add_interface_static(ags_type_accessible_dial,
				ATK_TYPE_VALUE,
				&atk_value_interface_info);

    g_type_add_interface_static(ags_type_accessible_dial,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_dial);
}

void
ags_dial_class_init(AgsDialClass *dial)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_dial_parent_class = g_type_class_peek_parent(dial);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object\0");
  
  /* GObjectClass */
  gobject = (GObjectClass *) dial;

  gobject->set_property = ags_dial_set_property;
  gobject->get_property = ags_dial_get_property;

  /* properties */
  /**
   * AgsDial:adjustment:
   *
   * The adjustment storing current value and boundaries.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("adjustment\0",
				   "assigned adjustment\0",
				   "The adjustment it is assigned with\0",
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
   * Since: 1.0.0
   */
  param_spec = g_param_spec_uint("scale-precision\0",
				 "scale precision\0",
				 "The precision of the scale\0",
				 0,
				 G_MAXUINT,
				 AGS_DIAL_DEFAULT_PRECISION,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SCALE_PRECISION,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) dial;

  widget->get_accessible = ags_dial_get_accessible;
  //  widget->map = ags_dial_map;
  widget->realize = ags_dial_realize;
  widget->expose_event = ags_dial_expose;
  widget->size_request = ags_dial_size_request;
  widget->size_allocate = ags_dial_size_allocate;
  widget->button_press_event = ags_dial_button_press;
  widget->button_release_event = ags_dial_button_release;
  widget->key_press_event = ags_dial_key_press;
  widget->key_release_event = ags_dial_key_release;
  widget->motion_notify_event = ags_dial_motion_notify;
  widget->show = ags_dial_show;

  /* AgsDialClass */
  dial->value_changed = NULL;

  /* signals */
  /**
   * AgsDial::value-changed:
   * @dial: the #AgsDial
   * @port_data: the port's data
   *
   * The ::value-changed signal notifies adjustment value changed.
   *
   * Since: 1.0.0
   */
  dial_signals[VALUE_CHANGED] =
    g_signal_new("value-changed\0",
		 G_TYPE_FROM_CLASS(dial),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDialClass, value_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

}

void
ags_accessible_dial_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_dial_value_interface_init(AtkValueIface *value)
{
  value->get_current_value = NULL;
  value->get_maximum_value = NULL;
  value->get_minimum_value = NULL;
  value->set_current_value = NULL;
  value->get_minimum_increment = NULL;

#ifdef HAVE_ATK_2_12  
  value->get_value_and_text = ags_accessible_dial_get_value_and_text;
  value->get_range = ags_accessible_dial_get_range;
  value->get_increment = ags_accessible_dial_get_increment;
  value->get_sub_ranges = NULL;
  value->set_value = ags_accessible_dial_set_value;
#endif
}

void
ags_accessible_dial_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_dial_do_action;
  action->get_n_actions = ags_accessible_dial_get_n_actions;
  action->get_description = ags_accessible_dial_get_description;
  action->get_name = ags_accessible_dial_get_name;
  action->get_keybinding = ags_accessible_dial_get_keybinding;
  action->set_description = ags_accessible_dial_set_description;
  action->get_localized_name = ags_accessible_dial_get_localized_name;
}

void
ags_dial_init(AgsDial *dial)
{
  AtkObject *accessible;

  gtk_widget_set_can_focus((GtkWidget *) dial,
			   TRUE);

  accessible = gtk_widget_get_accessible((GtkWidget *) dial);

  g_object_set(accessible,
	       "accessible-name\0", "dial\0",
	       "accessible-description\0", "Adjust a value\0",
	       NULL);
  
  g_object_set(G_OBJECT(dial),
  	       "app-paintable\0", TRUE,
	       "can-focus\0", TRUE,
  	       NULL);

  dial->flags = (AGS_DIAL_WITH_BUTTONS |
		 AGS_DIAL_SEEMLESS_MODE);

  dial->adjustment = NULL;

  dial->radius = AGS_DIAL_DEFAULT_RADIUS;
  dial->scale_precision = AGS_DIAL_DEFAULT_PRECISION;
  dial->scale_max_precision = AGS_DIAL_DEFAULT_PRECISION;
  dial->outline_strength = AGS_DIAL_DEFAULT_OUTLINE_STRENGTH;

  dial->font_size = 6;
  dial->button_width = AGS_DIAL_DEFAULT_BUTTON_WIDTH;
  dial->button_height = 0;
  dial->margin_left = AGS_DIAL_DEFAULT_MARGIN;
  dial->margin_right = AGS_DIAL_DEFAULT_MARGIN;

  dial->tolerance = 0.9;
  dial->negated_tolerance = 1.1;

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
	g_signal_connect(adjustment, "value-changed\0",
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
  case PROP_ADJUSTMENT:
    g_value_set_object(value, dial->adjustment);
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
ags_accessible_dial_get_value_and_text(AtkValue *value,
				       gdouble *current_value,
				       gchar **text)
{
  AgsDial *dial;
  
  dial = (AgsDial *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  if(current_value != NULL){
    *current_value = gtk_adjustment_get_value(dial->adjustment);
  }

  if(text != NULL){
    *text = g_strdup_printf("%f\0",
			    gtk_adjustment_get_value(dial->adjustment));
  }
}

#ifdef HAVE_ATK_2_12
AtkRange*
ags_accessible_dial_get_range(AtkValue *value)
{
  AgsDial *dial;
  AtkRange *range;
  
  dial = (AgsDial *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  range = atk_range_new(gtk_adjustment_get_lower(dial->adjustment),
			gtk_adjustment_get_upper(dial->adjustment),
			"Valid lower and upper input range of this dial\0");
}
#endif

gdouble
ags_accessible_dial_get_increment(AtkValue *value)
{
  AgsDial *dial;

  dial = (AgsDial *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  return(gtk_adjustment_get_step_increment(dial->adjustment));
}

void
ags_accessible_dial_set_value(AtkValue *value,
			      gdouble new_value)
{
  AgsDial *dial;

  dial = (AgsDial *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));
  gtk_adjustment_set_value(dial->adjustment,
			   new_value);
  gtk_widget_queue_draw((GtkWidget *) dial);
}

gboolean
ags_accessible_dial_do_action(AtkAction *action,
			      gint i)
{
  AgsDial *dial;
  
  GdkEventKey *key_press, *key_release;
  
  if(!(i >= 0 && i < 2)){
    return(FALSE);
  }

  dial = (AgsDial *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = gdk_event_new(GDK_KEY_PRESS);
  key_release = gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_DIAL_INCREMENT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) dial,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) dial,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_DIAL_DECREMENT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) dial,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) dial,
		       (GdkEvent *) key_release);
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_dial_get_n_actions(AtkAction *action)
{
  return(2);
}

const gchar*
ags_accessible_dial_get_description(AtkAction *action,
				    gint i)
{
  static const gchar **actions = {
    "increment dial value\0",
    "decrement dial value\0",
  };

  if(i >= 0 && i < 2){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_dial_get_name(AtkAction *action,
			     gint i)
{
  static const gchar **actions = {
    "increment\0",
    "decrement\0",
  };
  
  if(i >= 0 && i < 2){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_dial_get_keybinding(AtkAction *action,
				   gint i)
{
  static const gchar **actions = {
    "up\0",
    "down\0",
  };
  
  if(i >= 0 && i < 2){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_dial_set_description(AtkAction *action,
				    gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_dial_get_localized_name(AtkAction *action,
				       gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_dial_map(GtkWidget *widget)
{
  if (gtk_widget_get_realized (widget) && !gtk_widget_get_mapped (widget)) {
    GTK_WIDGET_CLASS (ags_dial_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    ags_dial_draw((AgsDial *) widget);
  }
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

  dial = AGS_DIAL(widget);

  gtk_widget_set_realized (widget, TRUE);

  /* calculate some display dependend fields */
  buttons_width = 0;

  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    cairo_t *cr;
    cairo_text_extents_t te_up, te_down;

    cr = gdk_cairo_create(widget->parent->window);

    if(cr == NULL){
      return;
    }
    
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

  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;
  
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  attributes.window_type = GDK_WINDOW_CHILD;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
			    GDK_KEY_PRESS_MASK | 
			    GDK_KEY_RELEASE_MASK |
                            GDK_BUTTON1_MOTION_MASK |
                            GDK_BUTTON3_MOTION_MASK |
                            GDK_POINTER_MOTION_HINT_MASK |
                            GDK_POINTER_MOTION_MASK |
                            GDK_ENTER_NOTIFY_MASK |
                            GDK_LEAVE_NOTIFY_MASK);

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				   &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, dial);

  widget->style = gtk_style_attach(widget->style, widget->window);
  gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);

  gtk_widget_queue_resize (widget);
}

AtkObject*
ags_dial_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_dial_get_type(),
			      NULL);
    
    g_object_set_qdata(G_OBJECT(widget),
		       quark_accessible_object,
		       accessible);
    gtk_accessible_set_widget(GTK_ACCESSIBLE(accessible),
			      widget);
  }
  
  return(accessible);
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
  requisition->height = AGS_DIAL_DEFAULT_HEIGHT;
  requisition->width = AGS_DIAL_DEFAULT_WIDTH;

  //TODO:JK: improve me
}

void
ags_dial_size_allocate(GtkWidget *widget,
		       GtkAllocation *allocation)
{  
  allocation->height = AGS_DIAL_DEFAULT_HEIGHT;
  allocation->width = AGS_DIAL_DEFAULT_WIDTH;
  
  widget->allocation = *allocation;

  //TODO:JK: improve me

  GTK_WIDGET_CLASS(ags_dial_parent_class)->size_allocate(widget,
							 allocation);
}

gboolean
ags_dial_expose(GtkWidget *widget,
		GdkEventExpose *event)
{
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

  //  GTK_WIDGET_CLASS(ags_dial_parent_class)->button_press_event(widget, event);

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

  //  GTK_WIDGET_CLASS(ags_dial_parent_class)->button_release_event(widget, event);

  gtk_widget_grab_focus(widget);

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
ags_dial_key_press(GtkWidget *widget,
		   GdkEventKey *event)
{
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    return(GTK_WIDGET_CLASS(ags_dial_parent_class)->key_press_event(widget, event));
  }
  
  return(TRUE);
}

gboolean
ags_dial_key_release(GtkWidget *widget,
		     GdkEventKey *event)
{
  AgsDial *dial;
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    return(GTK_WIDGET_CLASS(ags_dial_parent_class)->key_release_event(widget, event));
  }

  dial = AGS_DIAL(widget);
  
  switch(event->keyval){
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

      gtk_widget_queue_draw(widget);
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

      gtk_widget_queue_draw(widget);
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_dial_motion_notify(GtkWidget *widget,
		       GdkEventMotion *event)
{
  AgsDial *dial;

  auto void ags_dial_motion_notify_do_dial();
  auto void ags_dial_motion_notify_do_seemless_dial();
  
  void ags_dial_motion_notify_do_dial(){
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
      if(adjustment->value > adjustment->lower){
	gtk_adjustment_set_value(adjustment,
				 adjustment->value - adjustment->step_increment);
	
	ags_dial_draw(dial);
      }
    }else{
      if(adjustment->value < adjustment->upper){
	gtk_adjustment_set_value(adjustment,
				 adjustment->value + adjustment->step_increment);
	
	ags_dial_draw(dial);
      }
    }
  }

  void ags_dial_motion_notify_do_seemless_dial(){
    GtkAdjustment *adjustment;

    cairo_t *cr;
    
    gdouble radius;
    gdouble border_left, border_top;
    gdouble range;
    gdouble a0, quarter;
    gdouble x0, y0, x1, y1;
    gdouble translated_x;
    gboolean x_toggled, y_toggled;
    
    //TODO:JK: optimize me
    adjustment = dial->adjustment;

    range = (dial->adjustment->upper - dial->adjustment->lower);

    if(range == 0.0){
      return;
    }
    
    radius = dial->radius;

    border_left = 0.0;
    border_top = 0.0;
    
    x1 = event->x - (1.0 + dial->button_width + dial->margin_left + radius + border_left);
    y1 = event->y - (dial->outline_strength + radius + border_top);
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
    translated_x = adjustment->lower + translated_x;  

    if(translated_x < adjustment->lower){
      translated_x = adjustment->lower;
    }else if(translated_x > adjustment->upper){
      translated_x = adjustment->upper;
    }

    gtk_adjustment_set_value(adjustment,
    			     translated_x);
    ags_dial_draw(dial);
  }

  //  GTK_WIDGET_CLASS(ags_dial_parent_class)->motion_notify_event(widget, event);
  dial = AGS_DIAL(widget);
  
  if((AGS_DIAL_MOTION_CAPTURING & (dial->flags)) != 0){
    if((AGS_DIAL_SEEMLESS_MODE & (dial->flags)) != 0){
      if((AGS_DIAL_MOTION_CAPTURING_INIT & (dial->flags)) != 0){
	dial->current_x = event->x;
	dial->current_y = event->y;
      }else{
	dial->gravity_x = dial->current_x;
	dial->gravity_y = dial->current_y;
	dial->current_x = event->x;
	dial->current_y = event->y;
      }

      ags_dial_motion_notify_do_seemless_dial();
    }else{
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
  }
  
  return(FALSE);
}

/**
 * ags_dial_draw:
 * @dial: an #AgsDial
 *
 * draws the widget
 *
 * Since: 1.0.0
 */
void
ags_dial_draw(AgsDial *dial)
{
  GtkWidget *widget;

  GtkStyle *dial_style;
  cairo_t *cr;
  cairo_text_extents_t te_up, te_down;

  gdouble button_width, button_height, margin_left, margin_right;
  gdouble radius, outline_strength;
  gdouble unused;
  gdouble scale_area, scale_width, scale_inverted_width;
  gdouble starter_angle;
  gdouble translated_value;
  gdouble range;
  guint scale_precision;
  guint i;

  static const gdouble white_gc = 65535.0;

  widget = GTK_WIDGET(dial);
  dial_style = gtk_widget_get_style(widget);

  cr = gdk_cairo_create(widget->window);

  if(cr == NULL){
    return;
  }
  
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

  /* clear bg */
  cairo_set_source_rgb(cr,
		       dial_style->fg[0].red / white_gc,
		       dial_style->fg[0].green / white_gc,
		       dial_style->fg[0].blue / white_gc);

  cairo_rectangle(cr,
		  0.0, 0.0,
		  widget->allocation.width, widget->allocation.height);
  cairo_fill(cr);

  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    /* bg */
    cairo_set_source_rgb(cr,
			 dial_style->bg[0].red / white_gc,
			 dial_style->bg[0].green / white_gc,
			 dial_style->bg[0].blue / white_gc);

    cairo_rectangle(cr,
		    0.0, (2.0 * radius) - button_height + outline_strength - 1.0,
		    button_width + 2.0, button_height + 2.0);
    cairo_fill(cr);
    
    /* draw controller button down */
    cairo_set_source_rgb(cr,
			 dial_style->fg[0].red / white_gc,
			 dial_style->fg[0].green / white_gc,
			 dial_style->fg[0].blue / white_gc);

    cairo_set_line_width(cr, 2.0);

    cairo_rectangle(cr,
		    1.0, (2.0 * radius) - button_height + outline_strength,
		    button_width, button_height);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
    cairo_stroke(cr);

    cairo_move_to (cr,
		   1.0 + 0.5 - te_down.width / 2.0 - te_down.x_bearing + button_width / 2.25,
		   0.5 - te_down.height / 2.0 - te_down.y_bearing + (radius * 2.0) - button_height / 2.0 + outline_strength);
    cairo_show_text (cr, "-\0");
  }

  
  if((AGS_DIAL_WITH_BUTTONS & (dial->flags)) != 0){
    /* bg */
    cairo_set_source_rgb(cr,
			 dial_style->bg[0].red / white_gc,
			 dial_style->bg[0].green / white_gc,
			 dial_style->bg[0].blue / white_gc);

    cairo_rectangle(cr,
		    (2.0 * radius) + button_width + margin_left + margin_right,
		    (2.0 * radius) - button_height + outline_strength - 1.0,
		    button_width + 2.0, button_height + 2.0);
    cairo_fill(cr);

    /* draw controller button up */
    cairo_set_source_rgb(cr,
			 dial_style->fg[0].red / white_gc,
			 dial_style->fg[0].green / white_gc,
			 dial_style->fg[0].blue / white_gc);
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

  /* bg */
  cairo_set_source_rgb(cr,
		       dial_style->bg[0].red / white_gc,
		       dial_style->bg[0].green / white_gc,
		       dial_style->bg[0].blue / white_gc);
  
  cairo_arc(cr,
	    1.0 + button_width + margin_left + radius,
	    radius + outline_strength,
	    radius + outline_strength - 1.25,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_fill(cr);
  
  /* background */
  cairo_set_source_rgb(cr,
		       dial_style->dark[0].red / white_gc,
		       dial_style->dark[0].green / white_gc,
		       dial_style->dark[0].blue / white_gc);
  cairo_arc(cr,
	    1.0 + button_width + margin_left + radius,
	    radius + outline_strength,
	    radius,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_fill(cr);
  
  /* light effect */
  cairo_set_source_rgba(cr,
			dial_style->light[0].red / white_gc,
			dial_style->light[0].green / white_gc,
			dial_style->light[0].blue / white_gc,
			0.3);

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

  cairo_arc(cr,
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

  cairo_arc(cr,
	    1.0 + button_width + margin_left + radius,
	    radius + outline_strength,
	    radius,
	    -0.65 * M_PI,
	    -0.35 * M_PI);
  cairo_fill(cr);

  /* outline */
  cairo_set_source_rgb(cr,
		       dial_style->fg[0].red / white_gc,
		       dial_style->fg[0].green / white_gc,
		       dial_style->fg[0].blue / white_gc);
  //  cairo_set_line_width(cr, 1.0 - (2.0 / M_PI));
  cairo_set_line_width(cr, 1.0);
  cairo_arc(cr,
	    1.0 + button_width + margin_left + radius,
	    radius + outline_strength,
	    radius,
	    -1.0 * M_PI,
	    1.0 * M_PI);
  cairo_stroke(cr);

  /* scale */
  cairo_set_source_rgb(cr,
		       dial_style->fg[0].red / white_gc,
		       dial_style->fg[0].green / white_gc,
		       dial_style->fg[0].blue / white_gc);
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
	      1.0 + button_width + margin_left + radius,
	      radius + outline_strength,
	      radius + outline_strength / M_PI,
	      starter_angle + ((gdouble) i * scale_inverted_width) + ((gdouble) i * scale_width),
	      starter_angle + ((gdouble) i * scale_inverted_width) + ((gdouble) i * scale_width) + scale_width);
    cairo_stroke(cr);
  }

  /* draw value */
  range = (dial->adjustment->upper - dial->adjustment->lower);

  if(range == 0.0){
    g_warning("ags_dial.c - range = 0.0\0");
    return;
  }

  /* this is odd */
  translated_value = (dial->adjustment->value - dial->adjustment->lower);
  translated_value = (gdouble) scale_precision * (translated_value / range);

  //  g_message("value: %f\nupper: %f\ntranslated_value: %f\n\0", GTK_RANGE(dial)->adjustment->value, GTK_RANGE(dial)->adjustment->upper, translated_value);
  cairo_set_line_width(cr, 4.0);
  cairo_set_source_rgb(cr,
		       dial_style->fg[0].red / white_gc,
		       dial_style->fg[0].green / white_gc,
		       dial_style->fg[0].blue / white_gc);

  cairo_arc(cr,
	    1.0 + button_width + margin_left + radius,
	    radius + outline_strength,
	    radius - (outline_strength + 4.0) / M_PI,
	    starter_angle + (translated_value * scale_inverted_width) + (translated_value * scale_width),
	    starter_angle + (translated_value * scale_inverted_width) + (translated_value * scale_width) + scale_width);
  cairo_stroke(cr);

  cairo_destroy(cr);
}

/**
 * ags_dial_value_changed:
 * @dial: an #AgsDial
 *
 * draws the widget
 *
 * Since: 1.0.0
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
 * ags_dial_value_changed:
 * @dial: an #AgsDial
 * @value: the value to set
 * 
 * Set value
 *
 * Since: 1.0.0
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
  gtk_widget_queue_draw(dial);
}

/**
 * ags_dial_new:
 *
 * Creates an #AgsDial
 *
 * Returns: a new #AgsDial
 *
 * Since: 1.0.0
 */
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
