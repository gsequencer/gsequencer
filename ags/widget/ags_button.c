/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include "ags_button.h"

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#include <math.h>

void ags_button_class_init(AgsButtonClass *button);
void ags_button_init(AgsButton *button);
void ags_button_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_button_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_button_dispose(GObject *gobject);

void ags_button_realize(GtkWidget *widget);
void ags_button_unrealize(GtkWidget *widget);

void ags_button_measure(GtkWidget *widget,
			GtkOrientation orientation,
			int for_size,
			int *minimum,
			int *natural,
			int *minimum_baseline,
			int *natural_baseline);
void ags_button_size_allocate(GtkWidget *widget,
			      int width,
			      int height,
			      int baseline);

void ags_button_frame_clock_update_callback(GdkFrameClock *frame_clock,
					    AgsButton *button);

void ags_button_snapshot(GtkWidget *widget,
			 GtkSnapshot *snapshot);

void ags_button_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					       gint n_press,
					       gdouble x,
					       gdouble y,
					       AgsButton *button);
void ags_button_gesture_click_released_callback(GtkGestureClick *event_controller,
						gint n_press,
						gdouble x,
						gdouble y,
						AgsButton *button);

gboolean ags_button_key_pressed_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsButton *button);
void ags_button_key_released_callback(GtkEventControllerKey *event_controller,
				      guint keyval,
				      guint keycode,
				      GdkModifierType state,
				      AgsButton *button);
gboolean ags_button_modifiers_callback(GtkEventControllerKey *event_controller,
				       GdkModifierType keyval,
				       AgsButton *button);

void ags_button_motion_callback(GtkEventControllerMotion *event_controller,
				gdouble x,
				gdouble y,
				AgsButton *button);

void ags_button_draw(AgsButton *button,
		     cairo_t *cr,
		     gboolean is_animation);

/**
 * SECTION:ags_button
 * @short_description: A button widget
 * @title: AgsButton
 * @section_id:
 * @include: ags/widget/ags_button.h
 *
 * #AgsButton is a widget emitting clicked event on mouse click.
 */

enum{
  CLICKED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_FONT_SIZE,
  PROP_FONT_NAME,
  PROP_LABEL,
  PROP_ICON_NAME,
};

static gpointer ags_button_parent_class = NULL;
static guint button_signals[LAST_SIGNAL];

GType
ags_button_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_button = 0;

    static const GTypeInfo ags_button_info = {
      sizeof(AgsButtonClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_button_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsButton),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_button_init,
    };

    ags_type_button = g_type_register_static(GTK_TYPE_WIDGET,
					     "AgsButton", &ags_button_info,
					     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_button);
  }

  return(g_define_type_id__static);
}

GType
ags_button_flags_get_type(void)
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_BUTTON_SHOW_LABEL, "AGS_BUTTON_SHOW_LABEL", "button-show-label" },
      { AGS_BUTTON_SHOW_ICON, "AGS_BUTTON_SHOW_ICON", "button-show-icon" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsButtonFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

GType
ags_button_size_get_type(void)
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_BUTTON_SIZE_INHERIT, "AGS_BUTTON_SIZE_INHERIT", "button-size-inherit" },
      { AGS_BUTTON_SIZE_SMALL, "AGS_BUTTON_SIZE_SMALL", "button-size-small" },
      { AGS_BUTTON_SIZE_NORMAL, "AGS_BUTTON_SIZE_NORMAL", "button-size-normal" },
      { AGS_BUTTON_SIZE_LARGE, "AGS_BUTTON_SIZE_LARGE", "button-size-large" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsButtonSize"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_button_class_init(AgsButtonClass *button)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_button_parent_class = g_type_class_peek_parent(button);
  
  /* GObjectClass */
  gobject = (GObjectClass *) button;

  gobject->set_property = ags_button_set_property;
  gobject->get_property = ags_button_get_property;

  gobject->dispose = ags_button_dispose;
  
  /* properties */
  /**
   * AgsButton:font-size:
   *
   * The font size of the buttons.
   * 
   * Since: 8.0.0
   */
  param_spec = g_param_spec_uint("font-size",
				 "font size",
				 "The buttons font-size",
				 0,
				 G_MAXUINT,
				 AGS_BUTTON_DEFAULT_FONT_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FONT_SIZE,
				  param_spec);
  
  /**
   * AgsButton:font-name:
   *
   * The font name of the buttons.
   * 
   * Since: 8.0.0
   */
  param_spec = g_param_spec_string("font-name",
				   "font name",
				   "The buttons font-name",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FONT_NAME,
				  param_spec);
  
  /**
   * AgsButton:label:
   *
   * The label of the buttons.
   * 
   * Since: 8.0.0
   */
  param_spec = g_param_spec_string("label",
				   "label",
				   "The buttons label",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LABEL,
				  param_spec);
  
  /**
   * AgsButton:icon-name:
   *
   * The icon name of the buttons.
   * 
   * Since: 8.0.0
   */
  param_spec = g_param_spec_string("icon-name",
				   "icon name",
				   "The buttons icon-name",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ICON_NAME,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) button;

  widget->realize = ags_button_realize;
  widget->unrealize = ags_button_unrealize;

  widget->measure = ags_button_measure;
  widget->size_allocate = ags_button_size_allocate;
  
  widget->snapshot = ags_button_snapshot;

  gtk_widget_class_set_accessible_role(widget,
				       GTK_ACCESSIBLE_ROLE_SLIDER);
  
  /* AgsButtonClass */
  button->clicked = NULL;

  /* signals */
  /**
   * AgsButton::clicked:
   * @button: the #AgsButton
   *
   * The ::clicked signal notifies about button press and release.
   *
   * Since: 8.0.0
   */
  button_signals[CLICKED] =
    g_signal_new("clicked",
		 G_TYPE_FROM_CLASS(button),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsButtonClass, clicked),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_button_init(AgsButton *button)
{
  GtkEventController *event_controller;

  button->flags = AGS_BUTTON_SHOW_LABEL;
  button->button_size = AGS_BUTTON_SIZE_INHERIT;
  
  /*  */
  gtk_widget_set_can_focus((GtkWidget *) button,
			   TRUE);

  gtk_widget_set_hexpand((GtkWidget *) button,
			 TRUE);
  
  gtk_widget_set_vexpand((GtkWidget *) button,
			 TRUE);  

  /* key controller */
  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) button,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_button_key_pressed_callback), button);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_button_key_released_callback), button);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_button_modifiers_callback), button);

  /* gesture controller */
  event_controller = (GtkEventController *) gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) button,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_button_gesture_click_pressed_callback), button);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_button_gesture_click_released_callback), button);

  /* motion controller */
  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) button,
			    event_controller);

  g_signal_connect_after(event_controller, "motion",
			 G_CALLBACK(ags_button_motion_callback), button);

  /* fields */
  button->font_size = AGS_BUTTON_DEFAULT_FONT_SIZE;

  button->font_name = NULL;

  button->label = NULL;

  button->icon_name = NULL;
}

void
ags_button_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsButton *button;

  button = AGS_BUTTON(gobject);

  switch(prop_id){
  case PROP_FONT_SIZE:
    {
      button->font_size = g_value_get_uint(value);
    }
    break;
  case PROP_FONT_NAME:
    {
      gchar *font_name;
      
      font_name = g_value_get_string(value);

      g_free(button->font_name);

      button->font_name = g_strdup(font_name);
    }
    break;
  case PROP_LABEL:
    {
      gchar *label;
      
      label = g_value_get_string(value);

      g_free(button->label);

      button->label = g_strdup(label);
    }
    break;
  case PROP_ICON_NAME:
    {
      gchar *icon_name;
      
      icon_name = g_value_get_string(value);

      g_free(button->icon_name);

      button->icon_name = g_strdup(icon_name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_button_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsButton *button;

  button = AGS_BUTTON(gobject);

  switch(prop_id){
  case PROP_FONT_SIZE:
    {
      g_value_set_uint(value, button->font_size);
    }
    break;
  case PROP_FONT_NAME:
    {
      g_value_set_string(value, button->font_name);
    }
    break;
  case PROP_LABEL:
    {
      g_value_set_string(value, button->label);
    }
    break;
  case PROP_ICON_NAME:
    {
      g_value_set_string(value, button->icon_name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_button_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_button_parent_class)->dispose(gobject);
}

void
ags_button_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_button_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_button_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}

void
ags_button_unrealize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_object_disconnect(frame_clock,
		      "any_signal::update", 
		      G_CALLBACK(ags_button_frame_clock_update_callback),
		      widget,
		      NULL);

  gdk_frame_clock_end_updating(frame_clock);
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_button_parent_class)->unrealize(widget);
}

void
ags_button_measure(GtkWidget *widget,
		   GtkOrientation orientation,
		   int for_size,
		   int *minimum,
		   int *natural,
		   int *minimum_baseline,
		   int *natural_baseline)
{
  AgsButton *button;

  button = (AgsButton *) widget;

  //TODO:JK: implement me
}

void
ags_button_size_allocate(GtkWidget *widget,
			 int width,
			 int height,
			 int baseline)
{
  AgsButton *button;

  button = (AgsButton *) widget;
  
  //TODO:JK: implement me
}

void
ags_button_frame_clock_update_callback(GdkFrameClock *frame_clock,
				       AgsButton *button)
{
  gtk_widget_queue_draw((GtkWidget *) button);
}

void
ags_button_snapshot(GtkWidget *widget,
		    GtkSnapshot *snapshot)
{
  GtkStyleContext *style_context;

  cairo_t *cr;

  graphene_rect_t rect;
  
  gint width, height;
  
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

  ags_button_draw((AgsButton *) widget,
		  cr,
		  TRUE);
  
  cairo_destroy(cr);
}

void
ags_button_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					  gint n_press,
					  gdouble x,
					  gdouble y,
					  AgsButton *button)
{
  //TODO:JK: implement me
}

void
ags_button_gesture_click_released_callback(GtkGestureClick *event_controller,
					   gint n_press,
					   gdouble x,
					   gdouble y,
					   AgsButton *button)
{
  gtk_widget_grab_focus((GtkWidget *) button);
  
  //TODO:JK: implement me
}

gboolean
ags_button_key_pressed_callback(GtkEventControllerKey *event_controller,
				guint keyval,
				guint keycode,
				GdkModifierType state,
				AgsButton *button)
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
  
  //TODO:JK: implement me
  
  return(key_handled);
}

void
ags_button_key_released_callback(GtkEventControllerKey *event_controller,
				 guint keyval,
				 guint keycode,
				 GdkModifierType state,
				 AgsButton *button)
{
  //TODO:JK: implement me
}

gboolean
ags_button_modifiers_callback(GtkEventControllerKey *event_controller,
			      GdkModifierType keyval,
			      AgsButton *button)
{
  //TODO:JK: implement me
  
  return(FALSE);
}

void
ags_button_motion_callback(GtkEventControllerMotion *event_controller,
			   gdouble x,
			   gdouble y,
			   AgsButton *button)
{
  //TODO:JK: implement me
}

/**
 * ags_button_draw:
 * @button: the #AgsButton
 *
 * draws the widget
 *
 * Since: 8.0.0
 */
void
ags_button_draw(AgsButton *button,
		cairo_t *cr,
		gboolean is_animation)
{
  GtkStyleContext *style_context;

  GtkSettings *settings;

  GdkRGBA fg_color;
  GdkRGBA bg_color;
  GdkRGBA shadow_color;
  GdkRGBA text_color;

  gchar *font_name;

  gint widget_width, widget_height;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean shadow_success;
  gboolean text_success;

  settings = gtk_settings_get_default();

  style_context = gtk_widget_get_style_context((GtkWidget *) button);
  
  font_name = NULL;
    
  dark_theme = TRUE;

  if(button->font_name == NULL){
    g_object_get(settings,
		 "gtk-font-name", &font_name,
		 NULL);

    ags_button_set_font_name(button,
			     font_name);
  }else{
    font_name = button->font_name;
  }

  if(font_name == NULL){
    font_name = "sans";
  }
  
  g_object_get(settings,
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
    if(!dark_theme){
      gdk_rgba_parse(&fg_color,
		     "#101010");
      
      gdk_rgba_parse(&bg_color,
		     "#cbd5d9");
    
      gdk_rgba_parse(&shadow_color,
		     "#ffffff40");
      
      gdk_rgba_parse(&text_color,
		     "#000000");
    }else{
      gdk_rgba_parse(&fg_color,
		     "#cbd5d9");
      
      gdk_rgba_parse(&bg_color,
		     "#101010");
      
      gdk_rgba_parse(&shadow_color,
		     "#202020");

      gdk_rgba_parse(&text_color,
		     "#ffffff");
    }
  }

  widget_width = gtk_widget_get_width((GtkWidget *) button);
  widget_height = gtk_widget_get_height((GtkWidget *) button);

  //TODO:JK: implement me
}

/**
 * ags_button_test_flags:
 * @button: the #AgsButton
 * @flags: the flags
 * 
 * Test flags of @button.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 8.0.0
 */
gboolean
ags_button_test_flags(AgsButton *button,
		      AgsButtonFlags flags)
{
  gboolean success;

  success = ((flags & (button->flags)) != 0) ? TRUE: FALSE;

  return(success);
}

/**
 * ags_button_set_flags:
 * @button: the #AgsButton
 * @flags: the flags
 * 
 * Set flags of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_set_flags(AgsButton *button,
		     AgsButtonFlags flags)
{
  button->flags |= flags;
}

/**
 * ags_button_unset_flags:
 * @button: the #AgsButton
 * @flags: the flags
 * 
 * Set flags of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_unset_flags(AgsButton *button,
		       AgsButtonFlags flags)
{
  button->flags &= (~flags);
}

/**
 * ags_button_test_size:
 * @button: the #AgsButton
 * @size: the size
 * 
 * Test size of @button.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 8.0.0
 */
gboolean
ags_button_test_size(AgsButton *button,
		     AgsButtonSize size)
{
  gboolean success;

  success = (size == button->size) ? TRUE: FALSE;

  return(success);
}

/**
 * ags_button_set_size:
 * @button: the #AgsButton
 * @size: the size
 * 
 * Set size of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_set_size(AgsButton *button,
		    AgsButtonSize size)
{
  button->size = size;
}

/**
 * ags_button_set_font_size:
 * @button: the #AgsButton
 * @font_size: the font size
 * 
 * Set font size of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_set_font_size(AgsButton *button,
			 guint font_size)
{
  if(!AGS_IS_BUTTON(button)){
    return;
  }
  
  g_object_set(button,
	       "font-size", font_size,
	       NULL);
}

/**
 * ags_button_get_font_size:
 * @button: the #AgsButton
 * 
 * Get font size of @button.
 * 
 * Returns: the font size
 * 
 * Since: 8.0.0
 */
guint
ags_button_get_font_size(AgsButton *button)
{
  guint font_size;
  
  if(!AGS_IS_BUTTON(button)){
    return(0);
  }

  font_size = 0;
  
  g_object_get(button,
	       "font-size", &font_size,
	       NULL);

  return(font_size);
}

/**
 * ags_button_set_font_name:
 * @button: the #AgsButton
 * @font_name: the font name
 * 
 * Set font name of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_set_font_name(AgsButton *button,
			 gchar *font_name)
{
  if(!AGS_IS_BUTTON(button)){
    return;
  }
  
  g_object_set(button,
	       "font-name", font_name,
	       NULL);
}

/**
 * ags_button_get_font_name:
 * @button: the #AgsButton
 * 
 * Get font name of @button.
 * 
 * Returns: the font name
 * 
 * Since: 8.0.0
 */
gchar*
ags_button_get_font_name(AgsButton *button)
{
  gchar *font_name;
  
  if(!AGS_IS_BUTTON(button)){
    return(0);
  }

  font_name = NULL;
  
  g_object_get(button,
	       "font-name", &font_name,
	       NULL);

  return(font_name);
}

/**
 * ags_button_set_label:
 * @button: the #AgsButton
 * @label: the label
 * 
 * Set label of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_set_label(AgsButton *button,
		     gchar *label)
{
  if(!AGS_IS_BUTTON(button)){
    return;
  }
  
  g_object_set(button,
	       "label", label,
	       NULL);
}

/**
 * ags_button_get_label:
 * @button: the #AgsButton
 * 
 * Get label of @button.
 * 
 * Returns: the label
 * 
 * Since: 8.0.0
 */
gchar*
ags_button_get_label(AgsButton *button)
{
  gchar *label;
  
  if(!AGS_IS_BUTTON(button)){
    return(0);
  }

  label = NULL;
  
  g_object_get(button,
	       "label", &label,
	       NULL);

  return(label);
}

/**
 * ags_button_set_icon_name:
 * @button: the #AgsButton
 * @icon_name: the icon name
 * 
 * Set icon name of @button.
 * 
 * Since: 8.0.0
 */
void
ags_button_set_icon_name(AgsButton *button,
			 gchar *icon_name)
{
  if(!AGS_IS_BUTTON(button)){
    return;
  }
  
  g_object_set(button,
	       "icon-name", icon_name,
	       NULL);
}

/**
 * ags_button_get_icon_name:
 * @button: the #AgsButton
 * 
 * Get icon name of @button.
 * 
 * Returns: the icon name
 * 
 * Since: 8.0.0
 */
gchar*
ags_button_get_icon_name(AgsButton *button)
{
  gchar *icon_name;
  
  if(!AGS_IS_BUTTON(button)){
    return(0);
  }

  icon_name = NULL;
  
  g_object_get(button,
	       "icon-name", &icon_name,
	       NULL);

  return(icon_name);
}

/**
 * ags_button_clicked:
 * @button: the #AgsButton
 *
 * draws the widget
 *
 * Since: 8.0.0
 */
void
ags_button_clicked(AgsButton *button)
{
  g_return_if_fail(AGS_IS_BUTTON(button));

  g_object_ref((GObject *) button);
  g_signal_emit(G_OBJECT(button),
		button_signals[CLICKED], 0);
  g_object_unref((GObject *) button);
}

/**
 * ags_button_new:
 *
 * Creates an #AgsButton
 *
 * Returns: a new #AgsButton
 *
 * Since: 8.0.0
 */
AgsButton*
ags_button_new()
{
  AgsButton *button;

  button = (AgsButton *) g_object_new(AGS_TYPE_BUTTON,
				      NULL);
  
  return(button);
}

/**
 * ags_button_new:
 *
 * Creates an #AgsButton
 *
 * Returns: a new #AgsButton
 *
 * Since: 8.0.0
 */
AgsButton*
ags_button_new_with_label(gchar *label)
{
  AgsButton *button;

  button = (AgsButton *) g_object_new(AGS_TYPE_BUTTON,
				      "label", label,
				      NULL);
  
  return(button);
}

/**
 * ags_button_new:
 *
 * Creates an #AgsButton
 *
 * Returns: a new #AgsButton
 *
 * Since: 8.0.0
 */
AgsButton*
ags_button_new_from_icon_name(gchar *icon_name)
{
  AgsButton *button;

  button = (AgsButton *) g_object_new(AGS_TYPE_BUTTON,
				      "icon-name", icon_name,
				      NULL);
  
  return(button);
}
