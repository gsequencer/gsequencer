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

#include <ags/widget/ags_scale.h>

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_scale_class_init(AgsScaleClass *scale);
void ags_scale_orientable_interface_init(GtkOrientableIface *orientable);
void ags_scale_init(AgsScale *scale);
void ags_scale_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_scale_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_scale_finalize(GObject *gobject);

void ags_scale_realize(GtkWidget *widget);
void ags_scale_unrealize(GtkWidget *widget);

void ags_scale_frame_clock_update_callback(GdkFrameClock *frame_clock,
					   AgsScale *scale);

void ags_scale_snapshot(GtkWidget *widget,
			GtkSnapshot *snapshot);

gboolean ags_scale_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsScale *scale);
gboolean ags_scale_gesture_click_released_callback(GtkGestureClick *event_controller,
						   gint n_press,
						   gdouble x,
						   gdouble y,
						   AgsScale *scale);

gboolean ags_scale_key_pressed_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsScale *scale);
gboolean ags_scale_key_released_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsScale *scale);
gboolean ags_scale_modifiers_callback(GtkEventControllerKey *event_controller,
				      GdkModifierType keyval,
				      AgsScale *scale);

gboolean ags_scale_motion_callback(GtkEventControllerMotion *event_controller,
				   gdouble x,
				   gdouble y,
				   AgsScale *scale);

void ags_scale_draw(AgsScale *scale,
		    cairo_t *cr,
		    gboolean is_animation);

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
  PROP_ORIENTATION,
  PROP_CONTROL_NAME,
  PROP_LOWER,
  PROP_UPPER,
  PROP_DEFAULT_VALUE,
};

static gpointer ags_scale_parent_class = NULL;
static guint scale_signals[LAST_SIGNAL];

GType
ags_scale_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_scale = 0;
 
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

    static const GInterfaceInfo ags_orientable_interface_info = {
      (GInterfaceInitFunc) ags_scale_orientable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_scale = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsScale", &ags_scale_info,
					    0);

    g_type_add_interface_static(ags_type_scale,
				GTK_TYPE_ORIENTABLE,
				&ags_orientable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_scale);
  }

  return g_define_type_id__volatile;
}

void
ags_scale_orientable_interface_init(GtkOrientableIface *orientable)
{
  //empty
}

void
ags_scale_class_init(AgsScaleClass *scale)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_scale_parent_class = g_type_class_peek_parent(scale);

  /* GObjectClass */
  gobject = (GObjectClass *) scale;

  gobject->set_property = ags_scale_set_property;
  gobject->get_property = ags_scale_get_property;

  gobject->finalize = ags_scale_finalize;

  g_object_class_override_property(gobject, PROP_ORIENTATION, "orientation");  

  /* properties */
  /**
   * AgsScale:control-name:
   *
   * The scale's control name.
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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

  widget->realize = ags_scale_realize;
  widget->unrealize = ags_scale_unrealize;
  
  widget->snapshot = ags_scale_snapshot;

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
   * Since: 3.0.0
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
ags_scale_init(AgsScale *scale)
{
  GtkEventController *event_controller;

  g_object_set(G_OBJECT(scale),
	       "can-focus", TRUE,
	       "width-request", AGS_SCALE_DEFAULT_WIDTH_REQUEST,
	       "height-request", AGS_SCALE_DEFAULT_HEIGHT_REQUEST,
	       NULL);

  gtk_widget_set_hexpand(scale,
			 FALSE);
  
  gtk_widget_set_vexpand(scale,
			 FALSE);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) scale,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_scale_key_pressed_callback), scale);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_scale_key_released_callback), scale);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_scale_modifiers_callback), scale);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) scale,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_scale_gesture_click_pressed_callback), scale);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_scale_gesture_click_released_callback), scale);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) scale,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_scale_motion_callback), scale);

  scale->flags = 0;

  scale->orientation = GTK_ORIENTATION_VERTICAL;

  scale->key_mask = 0;
  scale->button_state = 0;

  scale->font_size = 11;

  scale->control_name = g_strdup(AGS_SCALE_DEFAULT_CONTROL_NAME);

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
  case PROP_ORIENTATION:
  {
    GtkOrientation orientation;

    orientation = g_value_get_enum(value);

    if(orientation != scale->orientation){
      scale->orientation = orientation;
      
      gtk_widget_queue_resize(GTK_WIDGET(scale));

      g_object_notify_by_pspec(gobject,
			       param_spec);
    }
  }
  break;
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

    gtk_widget_queue_draw((GtkWidget *) scale);
  }
  break;
  case PROP_UPPER:
  {
    scale->upper = g_value_get_double(value);

    gtk_widget_queue_draw((GtkWidget *) scale);
  }
  break;
  case PROP_DEFAULT_VALUE:
  {
    scale->default_value = g_value_get_double(value);

    gtk_widget_queue_draw((GtkWidget *) scale);
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
  case PROP_ORIENTATION:
  {
    g_value_set_enum(value, scale->orientation);
  }
  break;
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
ags_scale_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_scale_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_scale_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}

void
ags_scale_unrealize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_object_disconnect(frame_clock,
		      "any_signal::update", 
		      G_CALLBACK(ags_scale_frame_clock_update_callback),
		      widget,
		      NULL);

  gdk_frame_clock_end_updating(frame_clock);
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_scale_parent_class)->unrealize(widget);
}

void
ags_scale_frame_clock_update_callback(GdkFrameClock *frame_clock,
				      AgsScale *scale)
{
  gtk_widget_queue_draw((GtkWidget *) scale);
}

void
ags_scale_snapshot(GtkWidget *widget,
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

  ags_scale_draw((AgsScale *) widget,
		 cr,
		 TRUE);
  
  cairo_destroy(cr);
}

gboolean
ags_scale_key_pressed_callback(GtkEventControllerKey *event_controller,
			       guint keyval,
			       guint keycode,
			       GdkModifierType state,
			       AgsScale *scale)
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
ags_scale_key_released_callback(GtkEventControllerKey *event_controller,
				guint keyval,
				guint keycode,
				GdkModifierType state,
				AgsScale *scale)
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

      gtk_widget_queue_draw((GtkWidget *) scale);

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

      gtk_widget_queue_draw((GtkWidget *) scale);

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
      
      page = scale->page_size * (c_range / scale->step_count);

      if(scale->default_value + log(page) > scale->upper){
	scale->default_value = scale->upper;
      }else{
	scale->default_value += log(page);
      }

      gtk_widget_queue_draw((GtkWidget *) scale);

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
      
      page = scale->page_size * (c_range / scale->step_count);

      if(scale->default_value - log(page) < scale->lower){
	scale->default_value = scale->lower;
      }else{
	scale->default_value -= log(page);
      }

      gtk_widget_queue_draw((GtkWidget *) scale);

      ags_scale_value_changed(scale,
			      scale->default_value);
    }
    break;
    }
  }

  return(key_handled);
}

gboolean
ags_scale_modifiers_callback(GtkEventControllerKey *event_controller,
			     GdkModifierType keyval,
			     AgsScale *scale)
{
  return(FALSE);
}

gboolean
ags_scale_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					 gint n_press,
					 gdouble x,
					 gdouble y,
					 AgsScale *scale)
{
  guint width, height;
  guint x_start, y_start;

  width = gtk_widget_get_width((GtkWidget *) scale);
  height = gtk_widget_get_height((GtkWidget *) scale);

  x_start = 0;
  y_start = 0;

  if(x >= x_start &&
     x < width &&
     y >= y_start &&
     y < height){
    gdouble c_range;
    gdouble default_value;
      
    scale->button_state |= AGS_SCALE_BUTTON_1_PRESSED;

    if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
      c_range = exp(scale->upper) - exp(scale->lower);
    }else{
      c_range = scale->upper - scale->lower;
    }

    default_value = 0.0;
      
    if(scale->orientation == GTK_ORIENTATION_VERTICAL){
      default_value = y / c_range;
    }else if(scale->orientation == GTK_ORIENTATION_HORIZONTAL){
      default_value = x / c_range;
    }

    scale->default_value = default_value;
    gtk_widget_queue_draw((GtkWidget *) scale);

    ags_scale_value_changed(scale,
			    default_value);
  }

  return(TRUE);
}

gboolean
ags_scale_gesture_click_released_callback(GtkGestureClick *event_controller,
					  gint n_press,
					  gdouble x,
					  gdouble y,
					  AgsScale *scale)
{
  gtk_widget_grab_focus((GtkWidget *) scale);

  if((AGS_SCALE_BUTTON_1_PRESSED & (scale->button_state)) != 0){
    gdouble c_range;
    gdouble default_value;
      
    scale->button_state |= AGS_SCALE_BUTTON_1_PRESSED;

    if((AGS_SCALE_LOGARITHMIC & (scale->flags)) != 0){
      c_range = exp(scale->upper) - exp(scale->lower);
    }else{
      c_range = scale->upper - scale->lower;
    }
      
    default_value = 0.0;

    if(scale->orientation == GTK_ORIENTATION_VERTICAL){
      default_value = y / c_range;
    }else if(scale->orientation == GTK_ORIENTATION_HORIZONTAL){
      default_value = x / c_range;
    }

    scale->default_value = default_value;
    gtk_widget_queue_draw((GtkWidget *) scale);

    ags_scale_value_changed(scale,
			    default_value);
    
    scale->button_state &= (~AGS_SCALE_BUTTON_1_PRESSED);
  }

  return(TRUE);
}

gboolean
ags_scale_motion_callback(GtkEventControllerMotion *event_controller,
			  gdouble x,
			  gdouble y,
			  AgsScale *scale)
{
  guint width, height;
  guint x_start, y_start;
  
  width = gtk_widget_get_width((GtkWidget *) scale);
  height = gtk_widget_get_height((GtkWidget *) scale);

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
      
    new_default_value = 0.0;

    if(scale->orientation == GTK_ORIENTATION_VERTICAL){
      new_default_value = y / c_range;
    }else if(scale->orientation == GTK_ORIENTATION_HORIZONTAL){
      new_default_value = x / c_range;
    }

    if(new_default_value != scale->default_value){
      scale->default_value = new_default_value;
      gtk_widget_queue_draw((GtkWidget *) scale);
      
      ags_scale_value_changed(scale,
			      new_default_value);
    }
  }

  return(TRUE);
}

void
ags_scale_draw(AgsScale *scale,
	       cairo_t *cr,
	       gboolean is_animation)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  PangoLayout *layout;
  PangoFontDescription *desc;

  PangoRectangle ink_rect, logical_rect;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;
    
  gchar *font_name;
  gchar *text;

  guint width, height;
  guint x_start, y_start;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean text_success;
  gboolean shadow_success;

  style_context = gtk_widget_get_style_context((GtkWidget *) scale);

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

  width = gtk_widget_get_width((GtkWidget *) scale);
  height = gtk_widget_get_height((GtkWidget *) scale);

  x_start = 0;
  y_start = 0;

  //  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  /* background */
  cairo_set_source_rgb(cr,
		       0.0,
		       0.0,
		       0.0);
  cairo_rectangle(cr,
		  (gdouble) x_start, (gdouble) y_start,
		  (gdouble) width, (gdouble) height);
  cairo_fill(cr);

  /* box */
  cairo_set_source_rgb(cr,
		       0.5, 0.4, 0.0);
  cairo_set_line_width(cr,
		       1.0);
  cairo_rectangle(cr,
		  (gdouble) x_start, (gdouble) y_start,
		  (gdouble) width, (gdouble) height);
  cairo_stroke(cr);

  /* draw scale */
  cairo_set_line_width(cr,
		       2.0);

  cairo_set_source_rgb(cr,
		       0.5, 0.4, 0.0);

  if(scale->orientation == GTK_ORIENTATION_VERTICAL){
    /* upper */
    cairo_move_to(cr,
		  (double) width - 11.0,
		  1.0);
  
    cairo_line_to(cr,
		  (double) width - 1.0,
		  1.0);

    /* center */
    cairo_move_to(cr,
		  (double) width - 11.0,
		  (double) height / 2.0 - 1.0);
  
    cairo_line_to(cr,
		  (double) width - 1.0,
		  (double) height / 2.0 - 1.0);

    /* lower */  
    cairo_move_to(cr,
		  (double) width - 11.0,
		  height - 1.0);
  
    cairo_line_to(cr,
		  (double) width - 1.0,
		  height - 1.0);
  
    /* small scale */
    cairo_set_line_width(cr,
			 1.0);

    /* upper center */
    cairo_move_to(cr,
		  (double) width - 6.0,
		  (double) height * 0.25 - 0.5);
  
    cairo_line_to(cr,
		  (double) width - 1.0,
		  (double) height * 0.25 - 0.5);
  
    /* lower center */
    cairo_move_to(cr,
		  (double) width - 6.0,
		  (double) height * 0.75 - 0.5);
  
    cairo_line_to(cr,
		  (double) width - 1.0,
		  (double) height * 0.75 - 0.5);
  }else{
    /* upper */
    cairo_move_to(cr,
		  1.0,
		  (double) height - 11.0);
  
    cairo_line_to(cr,
		  1.0,
		  (double) height - 1.0);

    /* center */
    cairo_move_to(cr,
		  (double) width / 2.0 - 1.0,
		  (double) height - 11.0);
  
    cairo_line_to(cr,
		  (double) width / 2.0 - 1.0,
		  (double) height - 1.0);

    /* lower */  
    cairo_move_to(cr,
		  width - 1.0,
		  (double) height - 11.0);
  
    cairo_line_to(cr,
		  width - 1.0,
		  (double) height - 1.0);
  
    /* small scale */
    cairo_set_line_width(cr,
			 1.0);

    /* upper center */
    cairo_move_to(cr,
		  (double) width * 0.25 - 0.5,
		  (double) height - 6.0);
  
    cairo_line_to(cr,
		  (double) width * 0.25 - 0.5,
		  (double) height - 1.0);
  
    /* lower center */
    cairo_move_to(cr,
		  (double) width * 0.75 - 0.5,
		  (double) height - 6.0);
  
    cairo_line_to(cr,
		  (double) width * 0.75 - 0.5,
		  (double) height - 1.0);
  }
  
  cairo_stroke(cr);

  /* show text */
  text = g_strdup_printf("%s\n[%.2f - %.2f]",
			 scale->control_name,
			 scale->lower,
			 scale->upper);

  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout,
			text,
			-1);
  desc = pango_font_description_from_string(font_name);
  pango_font_description_set_size(desc,
				  scale->font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout,
				    desc);
  pango_font_description_free(desc);    

  pango_layout_get_extents(layout,
			   &ink_rect,
			   &logical_rect);

  cairo_set_source_rgb(cr,
		       1.0,
		       1.0,
		       1.0);

  if(scale->orientation == GTK_ORIENTATION_VERTICAL){
    cairo_move_to(cr,
		  x_start + (logical_rect.height / PANGO_SCALE) / 4.0,
		  y_start + height - 1.0);
    cairo_rotate(cr,
		 2.0 * M_PI * 0.75);
  }else{
    cairo_move_to(cr,
		  x_start,
		  y_start + (logical_rect.height / PANGO_SCALE) / 4.0 + 1.0);
  }
  
  pango_cairo_show_layout(cr,
			  layout);

  g_object_unref(layout);

  g_free(font_name);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

/**
 * ags_scale_set_control_name:
 * @scale: the #AgsScale
 * @control_name: the scale height
 * 
 * Set @control_name of @scale.
 * 
 * Since: 3.2.2
 */
void
ags_scale_set_control_name(AgsScale *scale,
			   gchar *control_name)
{
  if(!AGS_IS_SCALE(scale)){
    return;
  }

  g_object_set(scale,
	       "control-name", control_name,
	       NULL);
}

/**
 * ags_scale_get_control_name:
 * @scale: the #AgsScale
 * 
 * Get scale height of @scale.
 * 
 * Returns: the scale height
 * 
 * Since: 3.2.2
 */
gchar*
ags_scale_get_control_name(AgsScale *scale)
{
  gchar *control_name;
  
  if(!AGS_IS_SCALE(scale)){
    return(NULL);
  }

  g_object_get(scale,
	       "control-name", &control_name,
	       NULL);

  return(control_name);
}

/**
 * ags_scale_set_upper:
 * @scale: the #AgsScale
 * @upper: the upper
 * 
 * Set @upper of @scale.
 * 
 * Since: 3.2.2
 */
void
ags_scale_set_upper(AgsScale *scale,
		    gdouble upper)
{
  if(!AGS_IS_SCALE(scale)){
    return;
  }

  g_object_set(scale,
	       "upper", upper,
	       NULL);
}

/**
 * ags_scale_get_upper:
 * @scale: the #AgsScale
 * 
 * Get upper of @scale.
 * 
 * Returns: the upper
 * 
 * Since: 3.2.2
 */
gdouble
ags_scale_get_upper(AgsScale *scale)
{
  gdouble upper;
  
  if(!AGS_IS_SCALE(scale)){
    return(0.0);
  }

  g_object_get(scale,
	       "upper", &upper,
	       NULL);

  return(upper);
}

/**
 * ags_scale_set_lower:
 * @scale: the #AgsScale
 * @lower: the lower
 * 
 * Set @lower of @scale.
 * 
 * Since: 3.2.2
 */
void
ags_scale_set_lower(AgsScale *scale,
		    gdouble lower)
{
  if(!AGS_IS_SCALE(scale)){
    return;
  }

  g_object_set(scale,
	       "lower", lower,
	       NULL);
}

/**
 * ags_scale_get_lower:
 * @scale: the #AgsScale
 * 
 * Get lower of @scale.
 * 
 * Returns: the lower
 * 
 * Since: 3.2.2
 */
gdouble
ags_scale_get_lower(AgsScale *scale)
{
  gdouble lower;
  
  if(!AGS_IS_SCALE(scale)){
    return(0.0);
  }

  g_object_get(scale,
	       "lower", &lower,
	       NULL);

  return(lower);
}

/**
 * ags_scale_set_default_value:
 * @scale: the #AgsScale
 * @default_value: the normalized volume
 * 
 * Set @default_value of @scale.
 * 
 * Since: 3.2.2
 */
void
ags_scale_set_default_value(AgsScale *scale,
				gdouble default_value)
{
  if(!AGS_IS_SCALE(scale)){
    return;
  }

  g_object_set(scale,
	       "default-value", default_value,
	       NULL);
}

/**
 * ags_scale_get_default_value:
 * @scale: the #AgsScale
 * 
 * Get normalized volume of @scale.
 * 
 * Returns: the normalized volume
 * 
 * Since: 3.2.2
 */
gdouble
ags_scale_get_default_value(AgsScale *scale)
{
  gdouble default_value;
  
  if(!AGS_IS_SCALE(scale)){
    return(0.0);
  }

  g_object_get(scale,
	       "default-value", &default_value,
	       NULL);

  return(default_value);
}

/**
 * ags_scale_value_changed:
 * @scale: the #AgsScale
 * @default_value: the default value
 * 
 * Emits ::value-changed event.
 * 
 * Since: 3.0.0
 */
void
ags_scale_value_changed(AgsScale *scale,
			gdouble default_value)
{
  g_return_if_fail(AGS_IS_SCALE(scale));
  
  g_object_ref((GObject *) scale);
  g_signal_emit(G_OBJECT(scale),
		scale_signals[VALUE_CHANGED], 0,
		default_value);
  g_object_unref((GObject *) scale);
}

/**
 * ags_scale_new:
 * @orientation: the #GtkOrientation
 * @width_request: the scale's width
 * @height_request: the scale's height
 * 
 * Create a new instance of #AgsScale.
 * 
 * Returns: the new #AgsScale instance
 * 
 * Since: 3.0.0
 */
AgsScale*
ags_scale_new(GtkOrientation orientation,
	      guint width_request,
	      guint height_request)
{
  AgsScale *scale;

  scale = (AgsScale *) g_object_new(AGS_TYPE_SCALE,
				    "orientation", orientation,
				    "width-request", width_request,
				    "height-request", height_request,
				    NULL);
  
  return(scale);
}
