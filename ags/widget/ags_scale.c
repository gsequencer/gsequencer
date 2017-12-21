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

#include <ags/widget/ags_scale.h>

#include <atk/atk.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

static GType ags_accessible_scale_get_type(void);
void ags_scale_class_init(AgsScaleClass *scale);
void ags_scale_init(AgsScale *scale);
void ags_accessible_scale_class_init(AtkObject *object);
void ags_accessible_scale_value_interface_init(AtkValueIface *value);
void ags_accessible_scale_action_interface_init(AtkActionIface *action);
void ags_scale_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_scale_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_scale_finalize(GObject *gobject);
AtkObject* ags_scale_get_accessible(GtkWidget *widget);
void ags_scale_show(GtkWidget *widget);

void ags_accessible_scale_get_value_and_text(AtkValue *value,
					     gdouble *current_value,
					     gchar **text);
#ifdef HAVE_ATK_2_12  
AtkRange* ags_accessible_scale_get_range(AtkValue *value);
#endif
gdouble ags_accessible_scale_get_increment(AtkValue *value);
void ags_accessible_scale_set_value(AtkValue *value,
				    gdouble new_value);

gboolean ags_accessible_scale_do_action(AtkAction *action,
					gint i);
gint ags_accessible_scale_get_n_actions(AtkAction *action);
const gchar* ags_accessible_scale_get_description(AtkAction *action,
						  gint i);
const gchar* ags_accessible_scale_get_name(AtkAction *action,
					   gint i);
const gchar* ags_accessible_scale_get_keybinding(AtkAction *action,
						 gint i);
gboolean ags_accessible_scale_set_description(AtkAction *action,
					      gint i);
gchar* ags_accessible_scale_get_localized_name(AtkAction *action,
					       gint i);

void ags_scale_map(GtkWidget *widget);
void ags_scale_realize(GtkWidget *widget);
void ags_scale_size_request(GtkWidget *widget,
			    GtkRequisition   *requisition);
void ags_scale_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation);
gboolean ags_scale_expose(GtkWidget *widget,
			  GdkEventExpose *event);
gboolean ags_scale_button_press(GtkWidget *widget,
				GdkEventButton *event);
gboolean ags_scale_button_release(GtkWidget *widget,
				  GdkEventButton *event);
gboolean ags_scale_key_press(GtkWidget *widget,
			     GdkEventKey *event);
gboolean ags_scale_key_release(GtkWidget *widget,
			       GdkEventKey *event);
gboolean ags_scale_motion_notify(GtkWidget *widget,
				 GdkEventMotion *event);

void ags_scale_draw(AgsScale *scale);

/**
 * SECTION:ags_scale
 * @short_description: A scale widget
 * @title: AgsScale
 * @section_id:
 * @include: ags/widget/ags_scale.h
 *
 * #AgsScale is a widget that shows you a meter.
 */

enum{
  VALUE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CONTROL_NAME,
  PROP_LOWER,
  PROP_UPPER,
  PROP_DEFAULT_VALUE,
};

static gpointer ags_scale_parent_class = NULL;
static guint scale_signals[LAST_SIGNAL];

static GQuark quark_accessible_object = 0;

GType
ags_scale_get_type(void)
{
  static GType ags_type_scale = 0;

  if(!ags_type_scale){
    static const GTypeInfo ags_scale_info = {
      sizeof(AgsScaleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_scale_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsScale),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_scale_init,
    };

    ags_type_scale = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsScale", &ags_scale_info,
					    0);
  }

  return(ags_type_scale);
}

static GType
ags_accessible_scale_get_type(void)
{
  static GType ags_type_accessible_scale = 0;

  if(!ags_type_accessible_scale){
    const GTypeInfo ags_accesssible_scale_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_scale_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_value_interface_info = {
      (GInterfaceInitFunc) ags_accessible_scale_value_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_scale_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_accessible_scale = g_type_register_static(GTK_TYPE_ACCESSIBLE,
						       "AgsAccessibleScale", &ags_accesssible_scale_info,
						       0);

    g_type_add_interface_static(ags_type_accessible_scale,
				ATK_TYPE_VALUE,
				&atk_value_interface_info);

    g_type_add_interface_static(ags_type_accessible_scale,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_scale);
}

void
ags_scale_class_init(AgsScaleClass *scale)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_scale_parent_class = g_type_class_peek_parent(scale);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");

  /* GObjectClass */
  gobject = (GObjectClass *) scale;

  gobject->set_property = ags_scale_set_property;
  gobject->get_property = ags_scale_get_property;

  gobject->finalize = ags_scale_finalize;

  /* properties */
  /**
   * AgsScale:control-name:
   *
   * The scale's control name.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_string("control-name",
				   "control name",
				   "The control name of scale",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTROL_NAME,
				  param_spec);

  /**
   * AgsScale:lower:
   *
   * The scale's lower range.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_double("lower",
				   "lower",
				   "The lower of scale",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_SCALE_DEFAULT_LOWER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER,
				  param_spec);

  /**
   * AgsScale:upper:
   *
   * The scale's upper range.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_double("upper",
				   "upper",
				   "The upper of scale",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_SCALE_DEFAULT_UPPER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER,
				  param_spec);

  /**
   * AgsScale:default-value:
   *
   * The scale's default value.
   * 
   * Since: 1.3.0
   */
  param_spec = g_param_spec_double("default-value",
				   "default value",
				   "The default value of scale",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_SCALE_DEFAULT_VALUE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_VALUE,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) scale;

  widget->get_accessible = ags_scale_get_accessible;
  //  widget->map = ags_scale_map;
  widget->realize = ags_scale_realize;
  widget->expose_event = ags_scale_expose;
  widget->size_request = ags_scale_size_request;
  widget->size_allocate = ags_scale_size_allocate;
  widget->button_press_event = ags_scale_button_press;
  widget->button_release_event = ags_scale_button_release;
  widget->key_press_event = ags_scale_key_press;
  widget->key_release_event = ags_scale_key_release;
  widget->motion_notify_event = ags_scale_motion_notify;
  widget->show = ags_scale_show;

  /* AgsScaleClass */  
  scale->value_changed = NULL;

  /* signals */
  /**
   * AgsScale::value-changed:
   * @scale: the #AgsScale
   * @default_value: the changed default value
   *
   * The ::value-changed signal notifies about modified default value.
   *
   * Since: 1.3.0
   */
  scale_signals[VALUE_CHANGED] =
    g_signal_new("value-changed",
		 G_TYPE_FROM_CLASS(scale),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsScaleClass, value_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);
}

void
ags_accessible_scale_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_scale_value_interface_init(AtkValueIface *value)
{
  value->get_current_value = NULL;
  value->get_maximum_value = NULL;
  value->get_minimum_value = NULL;
  value->set_current_value = NULL;
  value->get_minimum_increment = NULL;

#ifdef HAVE_ATK_2_12  
  value->get_value_and_text = ags_accessible_scale_get_value_and_text;
  value->get_range = ags_accessible_scale_get_range;
  value->get_increment = ags_accessible_scale_get_increment;
  value->get_sub_ranges = NULL;
  value->set_value = ags_accessible_scale_set_value;
#endif
}

void
ags_accessible_scale_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_scale_do_action;
  action->get_n_actions = ags_accessible_scale_get_n_actions;
  action->get_description = ags_accessible_scale_get_description;
  action->get_name = ags_accessible_scale_get_name;
  action->get_keybinding = ags_accessible_scale_get_keybinding;
  action->set_description = ags_accessible_scale_set_description;
  action->get_localized_name = ags_accessible_scale_get_localized_name;
}

void
ags_scale_init(AgsScale *scale)
{
  AtkObject *accessible;

  g_object_set(G_OBJECT(scale),
	       "app-paintable", TRUE,
	       "can-focus", TRUE,
	       NULL);

  accessible = gtk_widget_get_accessible((GtkWidget *) scale);

  g_object_set(accessible,
	       "accessible-name", "scale",
	       "accessible-description", "Specify a default value",
	       NULL);

  scale->flags = 0;

  scale->key_mask = 0;
  scale->button_state = 0;
  scale->layout = AGS_SCALE_LAYOUT_VERTICAL;

  scale->font_size = 12;

  scale->scale_width = AGS_SCALE_DEFAULT_WIDTH;
  scale->scale_height = AGS_SCALE_DEFAULT_HEIGHT;

  scale->control_name = NULL;

  scale->lower = AGS_SCALE_DEFAULT_LOWER;
  scale->upper = AGS_SCALE_DEFAULT_UPPER;

  scale->default_value = AGS_SCALE_DEFAULT_VALUE;
  
  scale->step_count = AGS_SCALE_DEFAULT_STEP_COUNT;
  scale->page_size = AGS_SCALE_DEFAULT_PAGE_SIZE;

  scale->scale_step_count = -1;
  scale->scale_point = NULL;
  scale->scale_value = NULL;
}

void
ags_scale_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsScale *scale;

  scale = AGS_SCALE(gobject);

  switch(prop_id){
  case PROP_CONTROL_NAME:
    {
      gchar *control_name;

      control_name = g_value_get_string(value);

      g_free(scale->control_name);

      scale->control_name = g_strdup(control_name);
    }
    break;
  case PROP_LOWER:
    {
      scale->lower = g_value_get_double(value);

      gtk_widget_queue_draw(scale);
    }
    break;
  case PROP_UPPER:
    {
      scale->upper = g_value_get_double(value);

      gtk_widget_queue_draw(scale);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      scale->default_value = g_value_get_double(value);

      gtk_widget_queue_draw(scale);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scale_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsScale *scale;

  scale = AGS_SCALE(gobject);

  switch(prop_id){
  case PROP_CONTROL_NAME:
    {
      g_value_set_string(value,
			 scale->control_name);
    }
    break;
  case PROP_LOWER:
    {
      g_value_set_double(value,
			 scale->lower);
    }
    break;
  case PROP_UPPER:
    {
      g_value_set_double(value,
			 scale->upper);
    }
    break;
  case PROP_DEFAULT_VALUE:
    {
      g_value_set_double(value,
			 scale->default_value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_scale_finalize(GObject *gobject)
{
  AgsScale *scale;

  scale = AGS_SCALE(gobject);

  g_free(scale->control_name);
  
  /* call parent */
  G_OBJECT_CLASS(ags_scale_parent_class)->finalize(gobject);
}

void
ags_accessible_scale_get_value_and_text(AtkValue *value,
					gdouble *current_value,
					gchar **text)
{
  AgsScale *scale;
  
  scale = (AgsScale *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  if(current_value != NULL){
    *current_value = scale->default_value;
  }

  if(text != NULL){
    *text = g_strdup_printf("%f",
			    scale->default_value);
  }
}

#ifdef HAVE_ATK_2_12
AtkRange*
ags_accessible_scale_get_range(AtkValue *value)
{
  AgsScale *scale;
  AtkRange *range;
  
  scale = (AgsScale *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  range = atk_range_new(scale->lower,
			scale->upper,
			"Valid lower and upper input range of this scale");
}
#endif

gdouble
ags_accessible_scale_get_increment(AtkValue *value)
{
  AgsScale *scale;

  scale = (AgsScale *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  return((scale->upper - scale->lower) / scale->step_count);
}

void
ags_accessible_scale_set_value(AtkValue *value,
			       gdouble new_value)
{
  AgsScale *scale;

  scale = (AgsScale *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));
  scale->default_value = new_value;
  gtk_widget_queue_draw((GtkWidget *) scale);
}

gboolean
ags_accessible_scale_do_action(AtkAction *action,
			       gint i)
{
  AgsScale *scale;
  
  GdkEventKey *key_press, *key_release;
  
  if(!(i >= 0 && i < 4)){
    return(FALSE);
  }

  scale = (AgsScale *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = gdk_event_new(GDK_KEY_PRESS);
  key_release = gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_SCALE_STEP_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_SCALE_STEP_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_SCALE_PAGE_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Page_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_SCALE_PAGE_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Page_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) scale,
		       (GdkEvent *) key_release);
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_scale_get_n_actions(AtkAction *action)
{
  return(4);
}

const gchar*
ags_accessible_scale_get_description(AtkAction *action,
				     gint i)
{
  static const gchar **actions = {
    "step up scale default value",
    "step down scale default value",
    "page up scale default value",
    "page down scale default value",
  };

  if(i >= 0 && i < 4){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_scale_get_name(AtkAction *action,
			      gint i)
{
  static const gchar **actions = {
    "step-up",
    "step-down",
    "page-up",
    "page-down",
  };
  
  if(i >= 0 && i < 4){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_scale_get_keybinding(AtkAction *action,
				    gint i)
{
  static const gchar **actions = {
    "up",
    "down",
    "page-up",
    "page-down",
  };
  
  if(i >= 0 && i < 4){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_scale_set_description(AtkAction *action,
				     gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_scale_get_localized_name(AtkAction *action,
					gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_scale_map(GtkWidget *widget)
{
  if(gtk_widget_get_realized (widget) && !gtk_widget_get_mapped(widget)){
    GTK_WIDGET_CLASS(ags_scale_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    ags_scale_draw((AgsScale *) widget);
  }
}

void
ags_scale_realize(GtkWidget *widget)
{
  AgsScale *scale;

  GdkWindowAttr attributes;

  gint attributes_mask;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_SCALE(widget));

  scale = AGS_SCALE(widget);

  gtk_widget_set_realized(widget, TRUE);

  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;
  
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK);

  widget->window = gdk_window_new(gtk_widget_get_parent_window (widget),
				  &attributes, attributes_mask);
  gdk_window_set_user_data(widget->window, scale);

  widget->style = gtk_style_attach(widget->style,
				   widget->window);
  gtk_style_set_background(widget->style,
			   widget->window,
			   GTK_STATE_NORMAL);

  gtk_widget_queue_resize(widget);
}

AtkObject*
ags_scale_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_scale_get_type(),
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
ags_scale_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_scale_parent_class)->show(widget);
}

void
ags_scale_size_request(GtkWidget *widget,
		       GtkRequisition *requisition)
{
  AgsScale *scale;

  scale = AGS_SCALE(widget);

  if(scale->layout == AGS_SCALE_LAYOUT_VERTICAL){
    requisition->width = scale->scale_width;
    requisition->height = scale->scale_height;
  }else if(scale->layout == AGS_SCALE_LAYOUT_HORIZONTAL){
    requisition->width = scale->scale_height;
    requisition->height = scale->scale_width;
  }
}

void
ags_scale_size_allocate(GtkWidget *widget,
			GtkAllocation *allocation)
{
  AgsScale *scale;

  scale = AGS_SCALE(widget);
  
  widget->allocation = *allocation;

  if(scale->layout == AGS_SCALE_LAYOUT_VERTICAL){
    widget->allocation.width = scale->scale_width;
    widget->allocation.height = scale->scale_height;
  }else if(scale->layout == AGS_SCALE_LAYOUT_HORIZONTAL){
    widget->allocation.width = scale->scale_height;
    widget->allocation.height = scale->scale_width;
  }
}

gboolean
ags_scale_expose(GtkWidget *widget,
		 GdkEventExpose *event)
{
  ags_scale_draw(AGS_SCALE(widget));

  return(FALSE);
}

gboolean
ags_scale_button_press(GtkWidget *widget,
		       GdkEventButton *event)
{
  AgsScale *scale;

  guint width, height;
  guint x_start, y_start;

  scale = AGS_SCALE(widget);

  width = widget->allocation.width;
  height = widget->allocation.height;

  x_start = 0;
  y_start = 0;

  if(event->x >= x_start &&
     event->x < width &&
     event->y >= y_start &&
     event->y < height){
    if(event->button == 1){
      gdouble c_range;
      gdouble default_value;
      
      scale->button_state |= AGS_SCALE_BUTTON_1_PRESSED;

      if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
	c_range = exp(scale->upper) - exp(scale->lower);
      }else{
	c_range = scale->upper - scale->lower;
      }
      
      if(scale->layout == AGS_SCALE_LAYOUT_VERTICAL){
	default_value = event->y / c_range;
      }else if(scale->layout == AGS_SCALE_LAYOUT_HORIZONTAL){
	default_value = event->x / c_range;
      }

      scale->default_value = default_value;
      gtk_widget_queue_draw(scale);

      ags_scale_value_changed(scale,
			      default_value);
    }
  }
  
  return(FALSE);
}

gboolean
ags_scale_button_release(GtkWidget *widget,
			 GdkEventButton *event)
{
  AgsScale *scale;

  gtk_widget_grab_focus(widget);

  scale = AGS_SCALE(widget);
  
  if(event->button == 1){
    if((AGS_SCALE_BUTTON_1_PRESSED & (scale->button_state)) != 0){
      gdouble c_range;
      gdouble default_value;
      
      scale->button_state |= AGS_SCALE_BUTTON_1_PRESSED;

      if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
	c_range = exp(scale->upper) - exp(scale->lower);
      }else{
	c_range = scale->upper - scale->lower;
      }
      
      if(scale->layout == AGS_SCALE_LAYOUT_VERTICAL){
	default_value = event->y / c_range;
      }else if(scale->layout == AGS_SCALE_LAYOUT_HORIZONTAL){
	default_value = event->x / c_range;
      }

      scale->default_value = default_value;
      gtk_widget_queue_draw(scale);

      ags_scale_value_changed(scale,
			      default_value);
    }
    
    scale->button_state &= (~AGS_SCALE_BUTTON_1_PRESSED);
  }

  return(FALSE);
}

gboolean
ags_scale_key_press(GtkWidget *widget,
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
    return(GTK_WIDGET_CLASS(ags_scale_parent_class)->key_press_event(widget, event));
  }

  return(TRUE);
}

gboolean
ags_scale_key_release(GtkWidget *widget,
		      GdkEventKey *event)
{
  AgsScale *scale;

  //TODO:JK: implement me
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    return(GTK_WIDGET_CLASS(ags_scale_parent_class)->key_release_event(widget, event));
  }

  scale = AGS_SCALE(widget);
  
  switch(event->keyval){
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      gdouble c_range;
      gdouble step;

      if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
	c_range = exp(scale->upper) - exp(scale->lower);
      }else{
	c_range = scale->upper - scale->lower;
      }
      
      step = c_range / scale->step_count;

      if(scale->default_value + log(step) > scale->upper){
	scale->default_value = scale->upper;
      }else{
	scale->default_value += log(step);
      }

      gtk_widget_queue_draw(widget);

      ags_scale_value_changed(scale,
			      scale->default_value);
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      gdouble c_range;
      gdouble step;

      if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
	c_range = exp(scale->upper) - exp(scale->lower);
      }else{
	c_range = scale->upper - scale->lower;
      }
      
      step = c_range / scale->step_count;

      if(scale->default_value - log(step) < scale->lower){
	scale->default_value = scale->lower;
      }else{
	scale->default_value -= log(step);
      }

      gtk_widget_queue_draw(widget);

      ags_scale_value_changed(scale,
			      scale->default_value);
    }
    break;
  case GDK_KEY_Page_Up:
  case GDK_KEY_KP_Page_Up:
    {
      gdouble c_range;
      gdouble page;

      if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
	c_range = exp(scale->upper) - exp(scale->lower);
      }else{
	c_range = scale->upper - scale->lower;
      }
      
      page = scale->page_size * (c_range / scale->page_count);

      if(scale->default_value + log(page) > scale->upper){
	scale->default_value = scale->upper;
      }else{
	scale->default_value += log(page);
      }

      gtk_widget_queue_draw(widget);

      ags_scale_value_changed(scale,
			      scale->default_value);
    }
    break;
  case GDK_KEY_Page_Down:
  case GDK_KEY_KP_Page_Down:
    {
      gdouble c_range;
      gdouble page;

      if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
	c_range = exp(scale->upper) - exp(scale->lower);
      }else{
	c_range = scale->upper - scale->lower;
      }
      
      page = scale->page_size * (c_range / scale->page_count);

      if(scale->default_value - log(page) < scale->lower){
	scale->default_value = scale->lower;
      }else{
	scale->default_value -= log(page);
      }

      gtk_widget_queue_draw(widget);

      ags_scale_value_changed(scale,
			      scale->default_value);
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_scale_motion_notify(GtkWidget *widget,
			GdkEventMotion *event)
{
  AgsScale *scale;

  guint width, height;
  guint x_start, y_start;
  
  scale = AGS_SCALE(widget);

  width = widget->allocation.width;
  height = widget->allocation.height;

  x_start = 0;
  y_start = 0;

  if((AGS_SCALE_BUTTON_1_PRESSED & (scale->button_state)) != 0){
    gdouble c_range;
    gdouble new_default_value;
      
    scale->button_state |= AGS_SCALE_BUTTON_1_PRESSED;

    if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
      c_range = exp(scale->upper) - exp(scale->lower);
    }else{
      c_range = scale->upper - scale->lower;
    }
      
    if(scale->layout == AGS_SCALE_LAYOUT_VERTICAL){
      new_default_value = event->y / c_range;
    }else if(scale->layout == AGS_SCALE_LAYOUT_HORIZONTAL){
      new_default_value = event->x / c_range;
    }

    if(new_default_value != scale->default_value){
      scale->default_value = new_default_value;
      gtk_widget_queue_draw(scale);
      
      ags_scale_value_changed(scale,
			      new_default_value);
    }
  }
    
  return(FALSE);
}

void
ags_scale_draw(AgsScale *scale)
{
  cairo_t *cr;
  
  static const gdouble white_gc = 65535.0;

  if(!AGS_IS_SCALE(scale)){
    return;
  }

  cr = gdk_cairo_create(GTK_WIDGET(scale)->window);
  
  if(cr == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_scale_new:
 * 
 * Create a new instance of #AgsScale.
 * 
 * Returns: the new #AgsScale instance
 * 
 * Since: 1.3.0
 */
AgsScale*
ags_scale_new()
{
  AgsScale *scale;

  scale = (AgsScale *) g_object_new(AGS_TYPE_SCALE,
				    NULL);
  
  return(scale);
}
