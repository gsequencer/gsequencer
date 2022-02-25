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

#include <ags/widget/ags_level.h>

#include <pango/pango.h>

#ifndef __APPLE__
#include <pango/pangofc-fontmap.h>
#endif

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_level_class_init(AgsLevelClass *level);
void ags_level_orientable_interface_init(GtkOrientableIface *orientable);
void ags_level_init(AgsLevel *level);
void ags_level_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_level_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_level_finalize(GObject *gobject);

void ags_level_realize(GtkWidget *widget);

void ags_level_frame_clock_update_callback(GdkFrameClock *frame_clock,
					   AgsLevel *level);

void ags_level_snapshot(GtkWidget *widget,
			GtkSnapshot *snapshot);

gboolean ags_level_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsLevel *level);
gboolean ags_level_gesture_click_released_callback(GtkGestureClick *event_controller,
						   gint n_press,
						   gdouble x,
						   gdouble y,
						   AgsLevel *level);

gboolean ags_level_key_pressed_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsLevel *level);
gboolean ags_level_key_released_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsLevel *level);
gboolean ags_level_modifiers_callback(GtkEventControllerKey *event_controller,
				      GdkModifierType keyval,
				      AgsLevel *level);

gboolean ags_level_motion_callback(GtkEventControllerMotion *event_controller,
				   gdouble x,
				   gdouble y,
				   AgsLevel *level);

void ags_level_draw(AgsLevel *level,
		    cairo_t *cr,
		    gboolean is_animation);

/**
 * SECTION:ags_level
 * @short_description: A level widget
 * @title: AgsLevel
 * @section_id:
 * @include: ags/widget/ags_level.h
 *
 * #AgsLevel is a widget that shows you a meter.
 */

enum{
  VALUE_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_ORIENTATION,
  PROP_DATA_FORMAT,
  PROP_LOWER,
  PROP_UPPER,
  PROP_NORMALIZED_VOLUME,
};

static gpointer ags_level_parent_class = NULL;
static guint level_signals[LAST_SIGNAL];

GType
ags_level_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_level = 0;

    static const GTypeInfo ags_level_info = {
      sizeof(AgsLevelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_level_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLevel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_level_init,
    };

    static const GInterfaceInfo ags_orientable_interface_info = {
      (GInterfaceInitFunc) ags_level_orientable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_level = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsLevel", &ags_level_info,
					    0);
    
    g_type_add_interface_static(ags_type_level,
				GTK_TYPE_ORIENTABLE,
				&ags_orientable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_level);
  }

  return g_define_type_id__volatile;
}

void
ags_level_orientable_interface_init(GtkOrientableIface *orientable)
{
  //empty
}

void
ags_level_class_init(AgsLevelClass *level)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_level_parent_class = g_type_class_peek_parent(level);

  /* GObjectClass */
  gobject = (GObjectClass *) level;

  gobject->set_property = ags_level_set_property;
  gobject->get_property = ags_level_get_property;

  gobject->finalize = ags_level_finalize;

  g_object_class_override_property(gobject, PROP_ORIENTATION, "orientation");
  
  /* properties */
  /**
   * AgsLevel:data-format:
   *
   * The data format.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_uint("data-format",
				 "data format",
				 "The data format",
				 0,
				 G_MAXUINT,
				 AGS_LEVEL_DEFAULT_DATA_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA_FORMAT,
				  param_spec);

  /**
   * AgsLevel:lower:
   *
   * The level's lower range.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("lower",
				   "lower",
				   "The lower of level",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_LEVEL_DEFAULT_LOWER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOWER,
				  param_spec);

  /**
   * AgsLevel:upper:
   *
   * The level's upper range.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("upper",
				   "upper",
				   "The upper of level",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_LEVEL_DEFAULT_UPPER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UPPER,
				  param_spec);

  /**
   * AgsLevel:normalized-volume:
   *
   * The level's default value.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_double("normalized-volume",
				   "normalized volume",
				   "The normalized volume of level",
				   -G_MAXDOUBLE,
				   G_MAXDOUBLE,
				   AGS_LEVEL_DEFAULT_NORMALIZED_VOLUME,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NORMALIZED_VOLUME,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) level;

  widget->realize = ags_level_realize;
  
  widget->snapshot = ags_level_snapshot;

  /* AgsLevelClass */  
  level->value_changed = NULL;

  /* signals */
  /**
   * AgsLevel::value-changed:
   * @level: the #AgsLevel
   * @normalized_volume: the changed default value
   *
   * The ::value-changed signal notifies about modified default value.
   *
   * Since: 3.0.0
   */
  level_signals[VALUE_CHANGED] =
    g_signal_new("value-changed",
		 G_TYPE_FROM_CLASS(level),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLevelClass, value_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);
}

void
ags_level_init(AgsLevel *level)
{
  GtkEventController *event_controller;
  
  g_object_set(G_OBJECT(level),
	       "can-focus", TRUE,
	       "width-request", AGS_LEVEL_DEFAULT_WIDTH_REQUEST,
	       "height-request", AGS_LEVEL_DEFAULT_HEIGHT_REQUEST,
	       NULL);

  gtk_widget_set_hexpand(level,
			 FALSE);
  
  gtk_widget_set_vexpand(level,
			 FALSE);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) level,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_level_key_pressed_callback), level);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_level_key_released_callback), level);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_level_modifiers_callback), level);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) level,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_level_gesture_click_pressed_callback), level);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_level_gesture_click_released_callback), level);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) level,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_level_motion_callback), level);

  level->flags = 0;

  level->orientation = GTK_ORIENTATION_VERTICAL;

  level->key_mask = 0;
  level->button_state = 0;

  level->data_format = AGS_LEVEL_DEFAULT_DATA_FORMAT;
  
  level->font_size = 11;

  level->lower = AGS_LEVEL_DEFAULT_LOWER;
  level->upper = AGS_LEVEL_DEFAULT_UPPER;

  level->normalized_volume = AGS_LEVEL_DEFAULT_NORMALIZED_VOLUME;
  
  level->step_count = AGS_LEVEL_DEFAULT_STEP_COUNT;
  level->page_size = AGS_LEVEL_DEFAULT_PAGE_SIZE;
  
  level->samplerate = AGS_LEVEL_DEFAULT_SAMPLERATE;
  
  level->audio_channel = NULL;
  level->audio_channel_count = 0;
}

void
ags_level_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsLevel *level;

  level = AGS_LEVEL(gobject);

  switch(prop_id){
  case PROP_ORIENTATION:
  {
    GtkOrientation orientation;

    orientation = g_value_get_enum(value);

    if(orientation != level->orientation){
      level->orientation = orientation;
      
      gtk_widget_queue_resize(GTK_WIDGET(level));

      g_object_notify_by_pspec(gobject,
			       param_spec);
    }
  }
  break;
  case PROP_DATA_FORMAT:
  {
    level->data_format = g_value_get_uint(value);
  }
  break;
  case PROP_LOWER:
  {
    level->lower = g_value_get_double(value);

    gtk_widget_queue_draw((GtkWidget *) level);
  }
  break;
  case PROP_UPPER:
  {
    level->upper = g_value_get_double(value);

    gtk_widget_queue_draw((GtkWidget *) level);
  }
  break;
  case PROP_NORMALIZED_VOLUME:
  {
    level->normalized_volume = g_value_get_double(value);

    gtk_widget_queue_draw((GtkWidget *) level);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_level_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsLevel *level;

  level = AGS_LEVEL(gobject);

  switch(prop_id){
  case PROP_ORIENTATION:
  {
    g_value_set_enum(value, level->orientation);
  }
  break;
  case PROP_DATA_FORMAT:
  {
    g_value_set_uint(value,
		     level->data_format);
  }
  break;
  case PROP_LOWER:
  {
    g_value_set_double(value,
		       level->lower);
  }
  break;
  case PROP_UPPER:
  {
    g_value_set_double(value,
		       level->upper);
  }
  break;
  case PROP_NORMALIZED_VOLUME:
  {
    g_value_set_double(value,
		       level->normalized_volume);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_level_finalize(GObject *gobject)
{
  AgsLevel *level;

  level = AGS_LEVEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_level_parent_class)->finalize(gobject);
}

void
ags_level_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_level_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_level_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}

void
ags_level_frame_clock_update_callback(GdkFrameClock *frame_clock,
				      AgsLevel *level)
{
  gtk_widget_queue_draw((GtkWidget *) level);
}

void
ags_level_snapshot(GtkWidget *widget,
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

  ags_level_draw((AgsLevel *) widget,
		 cr,
		 TRUE);
  
  cairo_destroy(cr);
}

gboolean
ags_level_key_pressed_callback(GtkEventControllerKey *event_controller,
			       guint keyval,
			       guint keycode,
			       GdkModifierType state,
			       AgsLevel *level)
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
ags_level_key_released_callback(GtkEventControllerKey *event_controller,
				guint keyval,
				guint keycode,
				GdkModifierType state,
				AgsLevel *level)
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

      c_range = level->upper - level->lower;
      
      step = c_range / level->step_count;

      if(level->normalized_volume + log(step) > level->upper){
	level->normalized_volume = level->upper;
      }else{
	level->normalized_volume += log(step);
      }

      gtk_widget_queue_draw((GtkWidget *) level);

      ags_level_value_changed(level,
			      level->normalized_volume);
    }
    break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
    {
      gdouble c_range;
      gdouble step;

      c_range = level->upper - level->lower;
      
      step = c_range / level->step_count;

      if(level->normalized_volume - log(step) < level->lower){
	level->normalized_volume = level->lower;
      }else{
	level->normalized_volume -= log(step);
      }

      gtk_widget_queue_draw((GtkWidget *) level);

      ags_level_value_changed(level,
			      level->normalized_volume);
    }
    break;
    case GDK_KEY_Page_Up:
    case GDK_KEY_KP_Page_Up:
    {
      gdouble c_range;
      gdouble page;

      c_range = level->upper - level->lower;
      
      page = level->page_size * (c_range / level->step_count);

      if(level->normalized_volume + log(page) > level->upper){
	level->normalized_volume = level->upper;
      }else{
	level->normalized_volume += log(page);
      }

      gtk_widget_queue_draw((GtkWidget *) level);

      ags_level_value_changed(level,
			      level->normalized_volume);
    }
    break;
    case GDK_KEY_Page_Down:
    case GDK_KEY_KP_Page_Down:
    {
      gdouble c_range;
      gdouble page;

      c_range = level->upper - level->lower;
      
      page = level->page_size * (c_range / level->step_count);

      if(level->normalized_volume - log(page) < level->lower){
	level->normalized_volume = level->lower;
      }else{
	level->normalized_volume -= log(page);
      }

      gtk_widget_queue_draw((GtkWidget *) level);

      ags_level_value_changed(level,
			      level->normalized_volume);
    }
    break;
    }
  }

  return(FALSE);
}

gboolean
ags_level_modifiers_callback(GtkEventControllerKey *event_controller,
			     GdkModifierType keyval,
			     AgsLevel *level)
{
  return(FALSE);
}

gboolean
ags_level_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					 gint n_press,
					 gdouble x,
					 gdouble y,
					 AgsLevel *level)
{
  guint width, height;
  guint x_start, y_start;

  width = gtk_widget_get_width((GtkWidget *) level);
  height = gtk_widget_get_height((GtkWidget *) level);

  x_start = 0;
  y_start = 0;

  if(x >= x_start &&
     x < width &&
     y >= y_start &&
     y < height){
    gdouble c_range;
    gdouble normalized_volume;
      
    level->button_state |= AGS_LEVEL_BUTTON_1_PRESSED;

    c_range = level->upper - level->lower;

    normalized_volume = 0.0;
      
    if(level->orientation == GTK_ORIENTATION_VERTICAL){
      normalized_volume = y / c_range;
    }else if(level->orientation == GTK_ORIENTATION_HORIZONTAL){
      normalized_volume = x / c_range;
    }

    level->normalized_volume = normalized_volume;
    gtk_widget_queue_draw((GtkWidget *) level);

    ags_level_value_changed(level,
			    normalized_volume);
  }

  return(FALSE);
}

gboolean
ags_level_gesture_click_released_callback(GtkGestureClick *event_controller,
					  gint n_press,
					  gdouble x,
					  gdouble y,
					  AgsLevel *level)
{
  gtk_widget_grab_focus((GtkWidget *) level);

  if((AGS_LEVEL_BUTTON_1_PRESSED & (level->button_state)) != 0){
    gdouble c_range;
    gdouble normalized_volume;
      
    level->button_state |= AGS_LEVEL_BUTTON_1_PRESSED;

    c_range = level->upper - level->lower;
      
    normalized_volume = 0.0;

    if(level->orientation == GTK_ORIENTATION_VERTICAL){
      normalized_volume = y / c_range;
    }else if(level->orientation == GTK_ORIENTATION_HORIZONTAL){
      normalized_volume = x / c_range;
    }

    level->normalized_volume = normalized_volume;
    gtk_widget_queue_draw((GtkWidget *) level);

    ags_level_value_changed(level,
			    normalized_volume);
  }
    
  level->button_state &= (~AGS_LEVEL_BUTTON_1_PRESSED);

  return(FALSE);
}

gboolean
ags_level_motion_callback(GtkEventControllerMotion *event_controller,
			  gdouble x,
			  gdouble y,
			  AgsLevel *level)
{
  guint width, height;
  guint x_start, y_start;
  
  width = gtk_widget_get_width((GtkWidget *) level);
  height = gtk_widget_get_height((GtkWidget *) level);

  x_start = 0;
  y_start = 0;

  if((AGS_LEVEL_BUTTON_1_PRESSED & (level->button_state)) != 0){
    gdouble c_range;
    gdouble new_normalized_volume;
      
    level->button_state |= AGS_LEVEL_BUTTON_1_PRESSED;

    c_range = level->upper - level->lower;

    new_normalized_volume = 0.0;
            
    if(level->orientation == GTK_ORIENTATION_VERTICAL){
      new_normalized_volume = y / c_range;
    }else if(level->orientation == GTK_ORIENTATION_HORIZONTAL){
      new_normalized_volume = x / c_range;
    }

    if(new_normalized_volume != level->normalized_volume){
      level->normalized_volume = new_normalized_volume;
      gtk_widget_queue_draw((GtkWidget *) level);
      
      ags_level_value_changed(level,
			      new_normalized_volume);
    }
  }
  
  return(FALSE);
}

void
ags_level_draw(AgsLevel *level,
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
  gchar *format_str;

  guint width, height;
  guint x_start, y_start;
  guint i;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  gboolean text_success;
  gboolean shadow_success;

  style_context = gtk_widget_get_style_context((GtkWidget *) level);

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

  width = gtk_widget_get_width((GtkWidget *) level);
  height = gtk_widget_get_height((GtkWidget *) level);
  
  x_start = 0;
  y_start = 0;

  //  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);

  /* background */
  cairo_set_source_rgb(cr,
		       0.0, 0.0, 0.0);
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

  /* draw level */
  cairo_set_line_width(cr,
		       2.0);

  cairo_set_source_rgb(cr,
		       0.5, 0.4, 0.0);

  if(level->orientation == GTK_ORIENTATION_VERTICAL){
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
  
    /* small level */
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
  
    /* small level */
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

  /* format string */
  switch(level->data_format){
  case AGS_LEVEL_PCM_S8:
    {
      format_str = "signed 8 bit";
    }
    break;
  case AGS_LEVEL_PCM_S16:
    {
      format_str = "signed 16 bit";
    }
    break;
  case AGS_LEVEL_PCM_S24:
    {
      format_str = "signed 24 bit";
    }
    break;
  case AGS_LEVEL_PCM_S32:
    {
      format_str = "signed 32 bit";
    }
    break;
  case AGS_LEVEL_PCM_S64:
    {
      format_str = "signed 64 bit";
    }
    break;
  case AGS_LEVEL_PCM_FLOAT:
    {
      format_str = "float";
    }
    break;
  case AGS_LEVEL_PCM_DOUBLE:
    {
      format_str = "double";
    }
    break;
  case AGS_LEVEL_PCM_COMPLEX:
    {
      format_str = "complex";
    }
    break;
  }

  /* show text */
  text = g_strdup_printf("%u [Hz]\n%s",
			 level->samplerate,
			 format_str);

  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout,
			text,
			-1);
  desc = pango_font_description_from_string(font_name);
  pango_font_description_set_size(desc,
				  level->font_size * PANGO_SCALE);
  pango_layout_set_font_description(layout,
				    desc);
  pango_font_description_free(desc);    

  pango_layout_get_extents(layout,
			   &ink_rect,
			   &logical_rect);
  
  cairo_set_source_rgb(cr,
		       1.0, 1.0, 1.0);

  if(level->orientation == GTK_ORIENTATION_VERTICAL){
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
  g_free(text);
  
  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

/**
 * ags_level_set_upper:
 * @level: the #AgsLevel
 * @upper: the upper
 * 
 * Set @upper of @level.
 * 
 * Since: 3.2.2
 */
void
ags_level_set_upper(AgsLevel *level,
		    gdouble upper)
{
  if(!AGS_IS_LEVEL(level)){
    return;
  }

  g_object_set(level,
	       "upper", upper,
	       NULL);
}

/**
 * ags_level_get_upper:
 * @level: the #AgsLevel
 * 
 * Get upper of @level.
 * 
 * Returns: the upper
 * 
 * Since: 3.2.2
 */
gdouble
ags_level_get_upper(AgsLevel *level)
{
  gdouble upper;
  
  if(!AGS_IS_LEVEL(level)){
    return(0.0);
  }

  g_object_get(level,
	       "upper", &upper,
	       NULL);

  return(upper);
}

/**
 * ags_level_set_lower:
 * @level: the #AgsLevel
 * @lower: the lower
 * 
 * Set @lower of @level.
 * 
 * Since: 3.2.2
 */
void
ags_level_set_lower(AgsLevel *level,
		    gdouble lower)
{
  if(!AGS_IS_LEVEL(level)){
    return;
  }

  g_object_set(level,
	       "lower", lower,
	       NULL);
}

/**
 * ags_level_get_lower:
 * @level: the #AgsLevel
 * 
 * Get lower of @level.
 * 
 * Returns: the lower
 * 
 * Since: 3.2.2
 */
gdouble
ags_level_get_lower(AgsLevel *level)
{
  gdouble lower;
  
  if(!AGS_IS_LEVEL(level)){
    return(0.0);
  }

  g_object_get(level,
	       "lower", &lower,
	       NULL);

  return(lower);
}

/**
 * ags_level_set_normalized_volume:
 * @level: the #AgsLevel
 * @normalized_volume: the normalized volume
 * 
 * Set @normalized_volume of @level.
 * 
 * Since: 3.2.2
 */
void
ags_level_set_normalized_volume(AgsLevel *level,
				gdouble normalized_volume)
{
  if(!AGS_IS_LEVEL(level)){
    return;
  }

  g_object_set(level,
	       "normalized-volume", normalized_volume,
	       NULL);
}

/**
 * ags_level_get_normalized_volume:
 * @level: the #AgsLevel
 * 
 * Get normalized volume of @level.
 * 
 * Returns: the normalized volume
 * 
 * Since: 3.2.2
 */
gdouble
ags_level_get_normalized_volume(AgsLevel *level)
{
  gdouble normalized_volume;
  
  if(!AGS_IS_LEVEL(level)){
    return(0.0);
  }

  g_object_get(level,
	       "normalized-volume", &normalized_volume,
	       NULL);

  return(normalized_volume);
}

/**
 * ags_level_value_changed:
 * @level: the #AgsLevel
 * @normalized_volume: the normalized volume
 * 
 * Emits ::value-changed event.
 * 
 * Since: 3.0.0
 */
void
ags_level_value_changed(AgsLevel *level,
			gdouble normalized_volume)
{
  g_return_if_fail(AGS_IS_LEVEL(level));
  
  g_object_ref((GObject *) level);
  g_signal_emit(G_OBJECT(level),
		level_signals[VALUE_CHANGED], 0,
		normalized_volume);
  g_object_unref((GObject *) level);
}

/**
 * ags_level_new:
 * @orientation: the #GtkOrientation
 * @width_request: the level's width
 * @height_request: the level's height
 * 
 * Create a new instance of #AgsLevel.
 * 
 * Returns: the new #AgsLevel instance
 * 
 * Since: 3.0.0
 */
AgsLevel*
ags_level_new(GtkOrientation orientation,
	      guint width_request,
	      guint height_request)
{
  AgsLevel *level;

  level = (AgsLevel *) g_object_new(AGS_TYPE_LEVEL,
				    "orientation", orientation,
				    "width-request", width_request,
				    "height-request", height_request,
				    NULL);
  
  return(level);
}
