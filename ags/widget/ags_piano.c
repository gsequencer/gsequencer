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

#include <ags/widget/ags_piano.h>

#include <ags/widget/ags_widget_marshal.h>

#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

void ags_piano_class_init(AgsPianoClass *piano);
void ags_piano_orientable_interface_init(GtkOrientableIface *orientable);
void ags_piano_init(AgsPiano *piano);
void ags_piano_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_piano_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_piano_finalize(GObject *gobject);

void ags_piano_realize(GtkWidget *widget);

void ags_piano_measure(GtkWidget *widget,
		       GtkOrientation orientation,
		       int for_size,
		       int *minimum,
		       int *natural,
		       int *minimum_baseline,
		       int *natural_baseline);
void ags_piano_size_allocate(GtkWidget *widget,
			     int width,
			     int height,
			     int baseline);

void ags_piano_frame_clock_update_callback(GdkFrameClock *frame_clock,
					   AgsPiano *piano);

void ags_piano_snapshot(GtkWidget *widget,
			GtkSnapshot *snapshot);

gboolean ags_piano_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						  gint n_press,
						  gdouble x,
						  gdouble y,
						  AgsPiano *piano);
gboolean ags_piano_gesture_click_released_callback(GtkGestureClick *event_controller,
						   gint n_press,
						   gdouble x,
						   gdouble y,
						   AgsPiano *piano);

gboolean ags_piano_key_pressed_callback(GtkEventControllerKey *event_controller,
					guint keyval,
					guint keycode,
					GdkModifierType state,
					AgsPiano *piano);
gboolean ags_piano_key_released_callback(GtkEventControllerKey *event_controller,
					 guint keyval,
					 guint keycode,
					 GdkModifierType state,
					 AgsPiano *piano);
gboolean ags_piano_modifiers_callback(GtkEventControllerKey *event_controller,
				      GdkModifierType keyval,
				      AgsPiano *piano);

gboolean ags_piano_motion_callback(GtkEventControllerMotion *event_controller,
				   gdouble x,
				   gdouble y,
				   AgsPiano *piano);

void ags_piano_draw(AgsPiano *piano,
		    cairo_t *cr,
		    gboolean is_animation);


void ags_piano_real_key_pressed(AgsPiano *piano,
				gchar *note, gint key_code);
void ags_piano_real_key_released(AgsPiano *piano,
				 gchar *note, gint key_code);

void ags_piano_real_key_clicked(AgsPiano *piano,
				gchar *note, gint key_code);

/**
 * SECTION:ags_piano
 * @short_description: A piano widget
 * @title: AgsPiano
 * @section_id:
 * @include: ags/widget/ags_piano.h
 *
 * #AgsPiano is a widget representing a clavier.
 */

enum{
  KEY_PRESSED,
  KEY_RELEASED,
  KEY_CLICKED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_ORIENTATION,
  PROP_BASE_NOTE,
  PROP_BASE_KEY_CODE,
  PROP_KEY_WIDTH,
  PROP_KEY_HEIGHT,
  PROP_KEY_COUNT,
};

static gpointer ags_piano_parent_class = NULL;
static guint piano_signals[LAST_SIGNAL];

GType
ags_piano_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_piano = 0;

    static const GTypeInfo ags_piano_info = {
      sizeof(AgsPianoClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_piano_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPiano),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_piano_init,
    };

    static const GInterfaceInfo ags_orientable_interface_info = {
      (GInterfaceInitFunc) ags_piano_orientable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_piano = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsPiano", &ags_piano_info,
					    0);
    
    g_type_add_interface_static(ags_type_piano,
				GTK_TYPE_ORIENTABLE,
				&ags_orientable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_piano);
  }

  return g_define_type_id__volatile;
}

void
ags_piano_orientable_interface_init(GtkOrientableIface *orientable)
{
  //empty
}

void
ags_piano_class_init(AgsPianoClass *piano)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  GParamSpec *param_spec;

  ags_piano_parent_class = g_type_class_peek_parent(piano);

  /* GObjectClass */
  gobject = (GObjectClass *) piano;

  gobject->set_property = ags_piano_set_property;
  gobject->get_property = ags_piano_get_property;

  gobject->finalize = ags_piano_finalize;

  g_object_class_override_property(gobject, PROP_ORIENTATION, "orientation");

  /* properties */
  /**
   * AgsPiano:base-note:
   *
   * The base note to use as lower.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("base-note",
				   "base note",
				   "The base note to use as lower",
				   AGS_PIANO_DEFAULT_BASE_NOTE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BASE_NOTE,
				  param_spec);

  /**
   * AgsPiano:base-key-code:
   *
   * The base key code.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("base-key-code",
				 "base key code",
				 "The base key code",
				 0,
				 G_MAXUINT,
				 AGS_PIANO_DEFAULT_BASE_KEY_CODE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BASE_KEY_CODE,
				  param_spec);

  /**
   * AgsPiano:key-width:
   *
   * The key width to use for drawing a key.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("key-width",
				 "key width",
				 "The key width to use for drawing",
				 0,
				 G_MAXUINT,
				 AGS_PIANO_DEFAULT_KEY_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_KEY_WIDTH,
				  param_spec);

  /**
   * AgsPiano:key-height:
   *
   * The key height to use for drawing a key.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("key-height",
				 "key height",
				 "The key height to use for drawing",
				 0,
				 G_MAXUINT,
				 AGS_PIANO_DEFAULT_KEY_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_KEY_HEIGHT,
				  param_spec);

  /**
   * AgsPiano:key-count:
   *
   * The count of keys to be drawn.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("key-count",
				 "key count",
				 "The count of keys to draw",
				 0,
				 G_MAXUINT,
				 AGS_PIANO_DEFAULT_KEY_COUNT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_KEY_COUNT,
				  param_spec);

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) piano;

  widget->realize = ags_piano_realize;
  
  widget->measure = ags_piano_measure;
  widget->size_allocate = ags_piano_size_allocate;
  
  widget->snapshot = ags_piano_snapshot;

  /* AgsPianoClass */
  piano->key_pressed = ags_piano_real_key_pressed;
  piano->key_released = ags_piano_real_key_released;
  
  piano->key_clicked = NULL;

  /* signals */
  /**
   * AgsPiano::key-pressed:
   * @piano: the #AgsPiano
   * @note: the note as string
   * @key_code: the numeric representation of the note
   *
   * The ::key-pressed signal notifies about key pressed.
   *
   * Since: 3.0.0
   */
  piano_signals[KEY_PRESSED] =
    g_signal_new("key-pressed",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_pressed),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_STRING,
		 G_TYPE_INT);

  /**
   * AgsPiano::key-released:
   * @piano: the #AgsPiano
   * @note: the note as string
   * @key_code: the numeric representation of the note
   *
   * The ::key-released signal notifies about key released.
   *
   * Since: 3.0.0
   */
  piano_signals[KEY_RELEASED] =
    g_signal_new("key-released",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_released),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_STRING,
		 G_TYPE_INT);

  /**
   * AgsPiano::key-clicked:
   * @piano: the #AgsPiano
   * @note: the note as string
   * @key_code: the numeric representation of the note
   *
   * The ::key-clicked signal notifies about key clicked.
   *
   * Since: 3.0.0
   */
  piano_signals[KEY_CLICKED] =
    g_signal_new("key-clicked",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_clicked),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 2,
		 G_TYPE_STRING,
		 G_TYPE_INT);
}

void
ags_piano_init(AgsPiano *piano)
{
  GtkEventController *event_controller;
  
  gtk_widget_set_can_focus((GtkWidget *) piano,
			   TRUE);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) piano,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_piano_key_pressed_callback), piano);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_piano_key_released_callback), piano);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_piano_modifiers_callback), piano);

  event_controller = gtk_gesture_click_new();
  gtk_widget_add_controller((GtkWidget *) piano,
			    event_controller);

  g_signal_connect(event_controller, "pressed",
		   G_CALLBACK(ags_piano_gesture_click_pressed_callback), piano);

  g_signal_connect(event_controller, "released",
		   G_CALLBACK(ags_piano_gesture_click_released_callback), piano);

  event_controller = gtk_event_controller_motion_new();
  gtk_widget_add_controller((GtkWidget *) piano,
			    event_controller);

  g_signal_connect(event_controller, "motion",
		   G_CALLBACK(ags_piano_motion_callback), piano);

  piano->flags = 0;

  piano->orientation = GTK_ORIENTATION_VERTICAL;

  piano->button_state = 0;

  piano->font_size = AGS_PIANO_DEFAULT_FONT_SIZE;
  
  piano->base_note = g_strdup(AGS_PIANO_DEFAULT_BASE_NOTE);
  piano->base_key_code = AGS_PIANO_DEFAULT_BASE_KEY_CODE;
  
  piano->key_width = AGS_PIANO_DEFAULT_KEY_WIDTH;
  piano->key_height = AGS_PIANO_DEFAULT_KEY_HEIGHT;

  piano->key_count = AGS_PIANO_DEFAULT_KEY_COUNT;

  piano->cursor_position = -1;
  piano->current_key = 0;
  
  piano->active_key = NULL;
  piano->active_key_count = 0;
}

void
ags_piano_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsPiano *piano;

  piano = AGS_PIANO(gobject);

  switch(prop_id){
  case PROP_ORIENTATION:
  {
    GtkOrientation orientation;

    orientation = g_value_get_enum(value);

    if(orientation != piano->orientation){
      piano->orientation = orientation;
      
      gtk_widget_queue_resize(GTK_WIDGET(piano));

      g_object_notify_by_pspec(gobject,
			       param_spec);
    }
  }
  break;
  case PROP_BASE_NOTE:
  {
    gchar *base_note;

    base_note = g_value_get_string(value);

    if(base_note == piano->base_note){
      return;
    }

    g_free(piano->base_note);
      
    piano->base_note = g_strdup(base_note);
  }
  break;
  case PROP_BASE_KEY_CODE:
  {
    piano->base_key_code = g_value_get_uint(value);
  }
  break;
  case PROP_KEY_WIDTH:
  {
    piano->key_width = g_value_get_uint(value);
  }
  break;
  case PROP_KEY_HEIGHT:
  {
    piano->key_height = g_value_get_uint(value);
  }
  break;
  case PROP_KEY_COUNT:
  {
    piano->key_count = g_value_get_uint(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_piano_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsPiano *piano;

  piano = AGS_PIANO(gobject);

  switch(prop_id){
  case PROP_ORIENTATION:
  {
    g_value_set_enum(value, piano->orientation);
  }
  break;
  case PROP_BASE_NOTE:
  {
    g_value_set_string(value, piano->base_note);
  }
  break;
  case PROP_BASE_KEY_CODE:
  {
    g_value_set_uint(value, piano->base_key_code);
  }
  break;
  case PROP_KEY_WIDTH:
  {
    g_value_set_uint(value, piano->key_width);
  }
  break;
  case PROP_KEY_HEIGHT:
  {
    g_value_set_uint(value, piano->key_height);
  }
  break;
  case PROP_KEY_COUNT:
  {
    g_value_set_uint(value, piano->key_count);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_piano_finalize(GObject *gobject)
{
  AgsPiano *piano;

  piano = AGS_PIANO(gobject);
  
  g_free(piano->base_note);

  /* call parent */
  G_OBJECT_CLASS(ags_piano_parent_class)->finalize(gobject);
}

void
ags_piano_realize(GtkWidget *widget)
{
  GdkFrameClock *frame_clock;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_piano_parent_class)->realize(widget);

  frame_clock = gtk_widget_get_frame_clock(widget);
  
  g_signal_connect(frame_clock, "update", 
		   G_CALLBACK(ags_piano_frame_clock_update_callback), widget);

  gdk_frame_clock_begin_updating(frame_clock);
}


void
ags_piano_measure(GtkWidget *widget,
		  GtkOrientation orientation,
		  int for_size,
		  int *minimum,
		  int *natural,
		  int *minimum_baseline,
		  int *natural_baseline)
{
  AgsPiano *piano;

  piano = (AgsPiano *) widget;
  
  if(orientation == GTK_ORIENTATION_VERTICAL){
    if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_VERTICAL){
      minimum[0] =
	natural[0] = piano->key_count * piano->key_height;
    }else{
      minimum[0] = 
	natural[0] = piano->key_width;
    }
  }else{
    if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_VERTICAL){
      minimum[0] = 
	natural[0] = piano->key_width;
    }else{
      minimum[0] = 
	natural[0] = piano->key_count * piano->key_height;
    }
  }
}

void
ags_piano_size_allocate(GtkWidget *widget,
			int width,
			int height,
			int baseline)
{
  AgsPiano *piano;

  piano = (AgsPiano *) widget;

  if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_VERTICAL){
    width = piano->key_height;
    height = piano->key_count * piano->key_width;
  }else{
    width = piano->key_count * piano->key_width;
    height = piano->key_height;
  }
  
  GTK_WIDGET_CLASS(ags_piano_parent_class)->size_allocate(widget,
							  width,
							  height,
							  baseline);
}

void
ags_piano_frame_clock_update_callback(GdkFrameClock *frame_clock,
				      AgsPiano *piano)
{
  gtk_widget_queue_draw((GtkWidget *) piano);
}

void
ags_piano_snapshot(GtkWidget *widget,
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

  ags_piano_draw((AgsPiano *) widget,
		 cr,
		 TRUE);
  
  cairo_destroy(cr);
}

gboolean
ags_piano_gesture_click_pressed_callback(GtkGestureClick *event_controller,
					 gint n_press,
					 gdouble x,
					 gdouble y,
					 AgsPiano *piano)
{
  guint width, height;
  guint x_start, y_start;

  width = gtk_widget_get_width((AgsPiano *) piano);
  height = gtk_widget_get_height((AgsPiano *) piano);

  x_start = 0;
  y_start = 0;

  if(x >= x_start &&
     x < width &&
     y >= y_start &&
     y < height){
    gchar *note;
    
    piano->button_state |= AGS_PIANO_BUTTON_1_PRESSED;

    if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_VERTICAL){
      piano->current_key = floor(y / piano->key_height);
    }else if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_HORIZONTAL){
      piano->current_key = floor(x / piano->key_height);
    }

    note = ags_piano_key_code_to_note(piano->current_key);

    ags_piano_key_pressed(piano,
			  note, piano->current_key);

    g_free(note);
  }

  return(FALSE);
}

gboolean
ags_piano_gesture_click_released_callback(GtkGestureClick *event_controller,
					  gint n_press,
					  gdouble x,
					  gdouble y,
					  AgsPiano *piano)
{
  gtk_widget_grab_focus((GtkWidget *) piano);

  if((AGS_PIANO_BUTTON_1_PRESSED & (piano->button_state)) != 0){
    gchar *note;

    note = ags_piano_key_code_to_note(piano->current_key);
      
    ags_piano_key_released(piano,
			   note, piano->current_key);
    ags_piano_key_clicked(piano,
			  note, piano->current_key);
      
    g_free(note);
  }
    
  piano->button_state &= (~AGS_PIANO_BUTTON_1_PRESSED);

  return(FALSE);
}

gboolean
ags_piano_key_pressed_callback(GtkEventControllerKey *event_controller,
			       guint keyval,
			       guint keycode,
			       GdkModifierType state,
			       AgsPiano *piano)
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
ags_piano_key_released_callback(GtkEventControllerKey *event_controller,
				guint keyval,
				guint keycode,
				GdkModifierType state,
				AgsPiano *piano)
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
      guint value, step, upper;

      value = piano->cursor_position;
      step = 1;
      upper = piano->base_key_code + piano->key_count;
      
      if(value + step > upper){
	piano->cursor_position = upper;
      }else{
	piano->cursor_position = value + step;
      }

      gtk_widget_queue_draw((GtkWidget *) piano);
    }
    break;
    case GDK_KEY_Down:
    case GDK_KEY_downarrow:
    {
      guint value, step, lower;

      value = piano->cursor_position;
      step = 1;
      lower = piano->base_key_code;
      
      if(value - step < lower){
	piano->cursor_position = lower;
      }else{
	piano->cursor_position = value - step;
      }

      gtk_widget_queue_draw((GtkWidget *) piano);
    }
    break;
    case GDK_KEY_space:
    {
      gchar *note;
      guint key_code;

      note = ags_piano_key_code_to_note(piano->cursor_position);
      key_code = piano->cursor_position;

      ags_piano_key_pressed(piano,
			    note, key_code);
      ags_piano_key_released(piano,
			     note, key_code);

      ags_piano_key_clicked(piano,
			    note, key_code);
    }
    break;
    }
  }
  
  return(key_handled);
}

gboolean
ags_piano_modifiers_callback(GtkEventControllerKey *event_controller,
			     GdkModifierType keyval,
			     AgsPiano *piano)
{
  return(FALSE);
}

gboolean
ags_piano_motion_callback(GtkEventControllerMotion *event_controller,
			  gdouble x,
			  gdouble y,
			  AgsPiano *piano)
{
  guint width, height;
  guint x_start, y_start;

  gint new_current_key;

  width = gtk_widget_get_width((AgsPiano *) piano);
  height = gtk_widget_get_height((AgsPiano *) piano);

  x_start = 0;
  y_start = 0;

  if((AGS_PIANO_BUTTON_1_PRESSED & (piano->button_state)) != 0){
    new_current_key = 0;
    
    if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_VERTICAL){
      new_current_key = floor(y / piano->key_height);
    }else if(gtk_orientable_get_orientation(GTK_ORIENTABLE(piano)) == GTK_ORIENTATION_HORIZONTAL){
      new_current_key = floor(x / piano->key_height);
    }

    /* emit released */
    if(new_current_key != piano->current_key){
      gchar *note;

      note = ags_piano_key_code_to_note(piano->current_key);
      
      ags_piano_key_released(piano,
			     note, piano->current_key);
      ags_piano_key_clicked(piano,
			    note, piano->current_key);
      
      g_free(note);
    }

    /* emit pressed */
    if(x >= x_start &&
       x < width &&
       y >= y_start &&
       y < height){
      gchar *note;

      piano->current_key = new_current_key;
      
      note = ags_piano_key_code_to_note(new_current_key);

      ags_piano_key_pressed(piano,
			    note, new_current_key);

      g_free(note);      
    }
  }

  return(FALSE);
}

void
ags_piano_draw(AgsPiano *piano,
	       cairo_t *cr,
	       gboolean is_animation)
{  
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GtkOrientation orientation;

  guint widget_width, widget_height;
  guint width, height;
  guint x_start, y_start;

  guint big_control_width, big_control_height;
  guint small_control_width, small_control_height;
  
  guint control_x0, control_y0;
  
  gint *active_key;
  guint active_key_count;
  gint active_position;
  gboolean current_is_active;
  guint i, j;  
  gboolean dark_theme;

  static const guint bitmap = 0x52a52a;

  style_context = gtk_widget_get_style_context((GtkWidget *) piano);
  
  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  widget_width = gtk_widget_get_width((GtkWidget *) piano);
  widget_height = gtk_widget_get_height((GtkWidget *) piano);

  orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(piano));

  width  = 0;
  height = 0;

  if(orientation == GTK_ORIENTATION_VERTICAL){
    width = widget_width;

    if(piano->key_count * piano->key_height < widget_height){
      height = piano->key_count * piano->key_height;
    }else{
      height = widget_height;
    }
  }else if(orientation == GTK_ORIENTATION_HORIZONTAL){
    if(piano->key_count * piano->key_height < widget_width){
      width = piano->key_count * piano->key_height;
    }else{
      width = widget_width;
    }

    height = widget_height;
  }
  
  x_start = 0;
  y_start = 0;

  //  cairo_surface_flush(cairo_get_target(cr));
  cairo_push_group(cr);
  
  /* clear bg */
  cairo_set_source_rgb(cr,
		       1.0,
		       1.0,
		       1.0);
  cairo_rectangle(cr,
		  (double) x_start, (double) y_start,
		  (double) width, (double) height);
  cairo_fill(cr);
  
  /* draw */
  control_x0 = x_start;
  control_y0 = y_start;

  big_control_width = 0;
  big_control_height = 0;
    
  small_control_width = 0;
  small_control_height = 0;
  
  if(orientation == GTK_ORIENTATION_VERTICAL){
    big_control_width = piano->key_width;
    big_control_height = piano->key_height;
    
    small_control_width = (2.0 / 3.0) * piano->key_width;
    small_control_height = piano->key_height;
  }else if(orientation == GTK_ORIENTATION_HORIZONTAL){
    big_control_width = piano->key_height;
    big_control_height = piano->key_width;
    
    small_control_width = piano->key_height;
    small_control_height = (2.0 / 3.0) * piano->key_width;
  }
  
  active_key = ags_piano_get_active_key(piano,
					&active_key_count);

  active_position = -1;

  cairo_set_line_width(cr,
		       1.0);

  /* apply base note */
  j = piano->base_key_code % 12;
  
  for(i = 0; i < piano->key_count; i++){
    /* check active */
    current_is_active = FALSE;
    
    if(active_key != NULL &&
       active_position + 1 < active_key_count &&
       active_key[active_position] == piano->base_key_code + i){
      current_is_active = TRUE;
      
      active_position++;
    }
    
    if(((1 << j) & bitmap) != 0){
      /* draw semi tone key */
      cairo_set_source_rgb(cr,
			   0.2, 0.2, 0.2);

      cairo_rectangle(cr,
		      (double) control_x0, (double) control_y0,
		      small_control_width, (double) small_control_height);
      cairo_fill(cr); 	

      cairo_set_source_rgb(cr,
			   0.68, 0.68, 0.68);

      if(orientation == GTK_ORIENTATION_VERTICAL){
	cairo_move_to(cr,
		      (double) (control_x0 + small_control_width), (double) (control_y0 + small_control_height / 2));
	cairo_line_to(cr,
		      (double) (control_x0 + big_control_width), (double) (control_y0 + small_control_height / 2));
	cairo_stroke(cr);
      }else{
	cairo_move_to(cr,
		      (double) control_x0 + small_control_width / 2, (double) small_control_height);
	cairo_line_to(cr,
		      (double) control_x0 + small_control_width / 2, (double) big_control_height);
	cairo_stroke(cr);
      }
    }else{
      /* draw no semi tone key */
      cairo_set_source_rgb(cr,
			   0.68, 0.68, 0.68);

      if(orientation == GTK_ORIENTATION_VERTICAL){
	if(((1 << (j + 1)) & bitmap) == 0){
	  cairo_move_to(cr,
			(double) control_x0, (double) (control_y0 + big_control_height));
	  cairo_line_to(cr,
			(double) (control_x0 + big_control_width), (double) (control_y0 + big_control_height));
	  cairo_stroke(cr);
	}
      }else{
	if(((1 << (j + 1)) & bitmap) == 0){
	  cairo_move_to(cr,
			(double) control_x0 + big_control_width, (double) 0.0);
	  cairo_line_to(cr,
			(double) control_x0 + big_control_width, (double) big_control_height);
	  cairo_stroke(cr);
	}
      }
    }

    /* iterate - offset */
    if(orientation == GTK_ORIENTATION_VERTICAL){
      control_y0 += piano->key_height;
    }else if(orientation == GTK_ORIENTATION_HORIZONTAL){
      control_x0 += piano->key_height;
    }

    /* iterate - bitmap */
    if(j == 11){
      j = 0;
    }else{
      j++;
    }
  }

  g_free(active_key);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);
}

/**
 * ags_piano_set_button_state:
 * @piano: the #AgsPiano
 * @button_state: the #AgsPianoButtonState-enum
 * 
 * Set @button_state of @piano.
 * 
 * Since: 3.6.6
 */
void
ags_piano_set_button_state(AgsPiano *piano,
			   guint button_state)
{
  if(!AGS_IS_PIANO(piano)){
    return;
  }

  piano->button_state = button_state;
}

/**
 * ags_piano_get_button_state:
 * @piano: the #AgsPiano
 * 
 * Get mouse button state of @piano.
 * 
 * Returns: the button state
 * 
 * Since: 3.6.6
 */
guint
ags_piano_get_button_state(AgsPiano *piano)
{
  if(!AGS_IS_PIANO(piano)){
    return(0);
  }

  return(piano->button_state);
}

/**
 * ags_piano_set_base_note:
 * @piano: the #AgsPiano
 * @base_note: the base note
 * 
 * Set base note of @piano.
 * 
 * Since: 3.2.0
 */
void
ags_piano_set_base_note(AgsPiano *piano,
			gchar *base_note)
{
  if(!AGS_IS_PIANO(piano)){
    return;
  }

  g_object_set(piano,
	       "base-note", base_note,
	       NULL);
}

/**
 * ags_piano_get_base_note:
 * @piano: the #AgsPiano
 * 
 * Get base note of @piano.
 * 
 * Returns: the base note
 * 
 * Since: 3.2.0
 */
gchar*
ags_piano_get_base_note(AgsPiano *piano)
{
  gchar *base_note;
  
  if(!AGS_IS_PIANO(piano)){
    return(NULL);
  }
  
  g_object_get(piano,
	       "base-note", &base_note,
	       NULL);

  return(base_note);
}

/**
 * ags_piano_set_base_key_code:
 * @piano: the #AgsPiano
 * @base_key_code: the base key code
 * 
 * Set base key code of @piano.
 * 
 * Since: 3.2.0
 */
void
ags_piano_set_base_key_code(AgsPiano *piano,
			    gint base_key_code)
{
  if(!AGS_IS_PIANO(piano)){
    return;
  }
  
  g_object_set(piano,
	       "base-key-code", base_key_code,
	       NULL);
}

/**
 * ags_piano_get_base_key_code:
 * @piano: the #AgsPiano
 * 
 * Get base key code of @piano.
 * 
 * Returns: the base key code
 * 
 * Since: 3.2.0
 */
gint
ags_piano_get_base_key_code(AgsPiano *piano)
{
  gint base_key_code;
  
  if(!AGS_IS_PIANO(piano)){
    return(0);
  }
  
  g_object_get(piano,
	       "base-key-code", &base_key_code,
	       NULL);

  return(base_key_code);
}

/**
 * ags_piano_set_key_width:
 * @piano: the #AgsPiano
 * @key_width: the base note
 * 
 * Set key width of @piano.
 * 
 * Since: 3.2.0
 */
void
ags_piano_set_key_width(AgsPiano *piano,
			guint key_width)
{
  if(!AGS_IS_PIANO(piano)){
    return;
  }
  
  g_object_set(piano,
	       "key-width", key_width,
	       NULL);
}

/**
 * ags_piano_get_key_width:
 * @piano: the #AgsPiano
 * 
 * Get key width of @piano.
 * 
 * Returns: the key width
 * 
 * Since: 3.2.0
 */
guint
ags_piano_get_key_width(AgsPiano *piano)
{
  guint key_width;
  
  if(!AGS_IS_PIANO(piano)){
    return(0);
  }
  
  g_object_get(piano,
	       "key-width", &key_width,
	       NULL);

  return(key_width);
}

/**
 * ags_piano_set_key_height:
 * @piano: the #AgsPiano
 * @key_height: the key width
 * 
 * Set key width of @piano.
 * 
 * Since: 3.2.0
 */
void
ags_piano_set_key_height(AgsPiano *piano,
			 guint key_height)
{
  if(!AGS_IS_PIANO(piano)){
    return;
  }
  
  g_object_set(piano,
	       "key-height", key_height,
	       NULL);
}

/**
 * ags_piano_get_key_height:
 * @piano: the #AgsPiano
 * 
 * Get key height of @piano.
 * 
 * Returns: the key height
 * 
 * Since: 3.2.0
 */
guint
ags_piano_get_key_height(AgsPiano *piano)
{
  guint key_height;
  
  if(!AGS_IS_PIANO(piano)){
    return(0);
  }
  
  g_object_get(piano,
	       "key-height", &key_height,
	       NULL);

  return(key_height);
}

/**
 * ags_piano_set_key_count:
 * @piano: the #AgsPiano
 * @key_count: the key count
 * 
 * Set key count of @piano.
 * 
 * Since: 3.2.0
 */
void
ags_piano_set_key_count(AgsPiano *piano,
			guint key_count)
{
  if(!AGS_IS_PIANO(piano)){
    return;
  }
  
  g_object_set(piano,
	       "key-count", key_count,
	       NULL);
}

/**
 * ags_piano_get_key_count:
 * @piano: the #AgsPiano
 * 
 * Get key count of @piano.
 * 
 * Returns: the key count
 * 
 * Since: 3.2.0
 */
guint
ags_piano_get_key_count(AgsPiano *piano)
{
  guint key_count;
  
  if(!AGS_IS_PIANO(piano)){
    return(0);
  }
  
  g_object_get(piano,
	       "key-count", &key_count,
	       NULL);

  return(key_count);
}

/**
 * ags_piano_key_code_to_note:
 * @key_code: the key code
 * 
 * Get note from key code.
 * 
 * Returns: the note as string
 * 
 * Since: 3.0.0 
 */
gchar*
ags_piano_key_code_to_note(gint key_code)
{
  gchar *note;
  gchar *prefix;
  gchar *suffix;

  guint tic_count;
  guint i;
  
  static const gchar* note_map[] = {
    AGS_PIANO_KEYS_OCTAVE_2_C,
    AGS_PIANO_KEYS_OCTAVE_2_CIS,
    AGS_PIANO_KEYS_OCTAVE_2_D,
    AGS_PIANO_KEYS_OCTAVE_2_DIS,
    AGS_PIANO_KEYS_OCTAVE_2_E,
    AGS_PIANO_KEYS_OCTAVE_2_F,
    AGS_PIANO_KEYS_OCTAVE_2_FIS,
    AGS_PIANO_KEYS_OCTAVE_2_G,
    AGS_PIANO_KEYS_OCTAVE_2_GIS,
    AGS_PIANO_KEYS_OCTAVE_2_A,
    AGS_PIANO_KEYS_OCTAVE_2_AIS,
    AGS_PIANO_KEYS_OCTAVE_2_H,
    AGS_PIANO_KEYS_OCTAVE_3_C,
    AGS_PIANO_KEYS_OCTAVE_3_CIS,
    AGS_PIANO_KEYS_OCTAVE_3_D,
    AGS_PIANO_KEYS_OCTAVE_3_DIS,
    AGS_PIANO_KEYS_OCTAVE_3_E,
    AGS_PIANO_KEYS_OCTAVE_3_F,
    AGS_PIANO_KEYS_OCTAVE_3_FIS,
    AGS_PIANO_KEYS_OCTAVE_3_G,
    AGS_PIANO_KEYS_OCTAVE_3_GIS,
    AGS_PIANO_KEYS_OCTAVE_3_A,
    AGS_PIANO_KEYS_OCTAVE_3_AIS,
    AGS_PIANO_KEYS_OCTAVE_3_H,
  };

  note = NULL;
  
  if(key_code < 2 * 12){
    /* prefix */
    if(key_code >= 0){
      prefix = note_map[key_code % 12];
    }else{
      prefix = note_map[(12 + (key_code % 12)) % 12];
    }
    
    /* suffix */
    tic_count = floor((24.0 - key_code) / 12.0);
    suffix = (gchar *) malloc((tic_count + 1) * sizeof(gchar));
    
    for(i = 0; i < tic_count; i++){
      suffix[i] = ',';
    }
    
    suffix[i] = '\0';

    /* note */
    note = g_strdup_printf("%s%s",
			   prefix,
			   suffix);
  }else if(key_code >= 2 * 12 &&
	   key_code < 3 * 12){
    note = g_strdup(note_map[key_code - 2 * 12]);
  }else if(key_code >= 3 * 12 &&
	   key_code < 4 * 12){
    note = g_strdup(note_map[key_code - 3 * 12]);
  }else{
    /* prefix */
    prefix = note_map[(key_code % 12) + 12];

    /* suffix */
    tic_count = floor((key_code - 48.0) / 12.0);
    suffix = (gchar *) malloc((tic_count + 1) * sizeof(gchar));
    
    for(i = 0; i < tic_count; i++){
      suffix[i] = '\'';
    }

    suffix[i] = '\0';

    /* note */
    note = g_strdup_printf("%s%s",
			   prefix,
			   suffix);
  }

  return(note);
}

void
ags_piano_real_key_pressed(AgsPiano *piano,
			   gchar *note, gint key_code)
{
  guint i;

  i = piano->active_key_count;
  
  if(piano->active_key == NULL){
    piano->active_key = (gint *) malloc(sizeof(gint));
  }else{
    piano->active_key = (gint *) realloc(piano->active_key,
					 (i + 1) * sizeof(gint));
  }

  piano->active_key[i] = key_code;
  piano->active_key_count += 1;

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) piano);
}

/**
 * ags_piano_key_pressed:
 * @piano: the #AgsPiano
 * @note: the string representation of key
 * @key_code: the key code
 * 
 * Emits ::key-pressed event.
 * 
 * Since: 3.0.0
 */
void
ags_piano_key_pressed(AgsPiano *piano,
		      gchar *note, gint key_code)
{
  g_return_if_fail(AGS_IS_PIANO(piano));
  
  g_object_ref((GObject *) piano);
  g_signal_emit(G_OBJECT(piano),
		piano_signals[KEY_PRESSED], 0,
		note, key_code);
  g_object_unref((GObject *) piano);
}

void
ags_piano_real_key_released(AgsPiano *piano,
			    gchar *note, gint key_code)
{
  gint position;

  position = -1;

  while(position < piano->active_key_count){
    position++;

    if(piano->active_key[position] == key_code){
      break;
    }
  }

  if(position != -1 &&
     position < piano->active_key_count){

    if(piano->active_key_count > 1){
      gint *old_active_key;
      
      old_active_key = ags_piano_get_active_key(piano,
						NULL);

      piano->active_key = (gint *) realloc(piano->active_key,
					   (piano->active_key_count - 1) * sizeof(gint));

      if(position < piano->active_key_count - 1){
	memcpy(&(piano->active_key[position]), &(old_active_key[position + 1]), (piano->active_key_count - position - 1) * sizeof(gint));
      }
      
      free(old_active_key);
    }else{
      free(piano->active_key);

      piano->active_key = NULL;
    }

    piano->active_key_count -= 1;
  }

  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) piano);
}

/**
 * ags_piano_key_released:
 * @piano: the #AgsPiano
 * @note: the string representation of key
 * @key_code: the key code
 * 
 * Emits ::key-released event.
 * 
 * Since: 3.0.0
 */
void
ags_piano_key_released(AgsPiano *piano,
		       gchar *note, gint key_code)
{
  g_return_if_fail(AGS_IS_PIANO(piano));
  
  g_object_ref((GObject *) piano);
  g_signal_emit(G_OBJECT(piano),
		piano_signals[KEY_RELEASED], 0,
		note, key_code);
  g_object_unref((GObject *) piano);
}

/**
 * ags_piano_key_clicked:
 * @piano: the #AgsPiano
 * @note: the string representation of key
 * @key_code: the key code
 * 
 * Emits ::key-clicked event.
 * 
 * Since: 3.0.0
 */
void
ags_piano_key_clicked(AgsPiano *piano,
		      gchar *note, gint key_code)
{
  g_return_if_fail(AGS_IS_PIANO(piano));
  
  g_object_ref((GObject *) piano);
  g_signal_emit(G_OBJECT(piano),
		piano_signals[KEY_CLICKED], 0,
		note, key_code);
  g_object_unref((GObject *) piano);
}

/**
 * ags_piano_get_active_key:
 * @piano: the #AgsPiano
 * @active_key_count: return location of count of active keys
 * 
 * Get active keys.
 * 
 * Returns: the active keys as gint array
 * 
 * Since: 3.0.0
 */
gint*
ags_piano_get_active_key(AgsPiano *piano,
			 guint *active_key_count)
{
  gint *active_key;
  
  if(!AGS_IS_PIANO(piano)){
    if(active_key_count != NULL){
      *active_key_count = 0;
    }
    
    return(NULL);
  }
  
  if(piano->active_key_count > 0){
    active_key = (gint *) malloc(piano->active_key_count * sizeof(gint));
    memcpy(active_key, piano->active_key, piano->active_key_count * sizeof(gint));
  }else{
    active_key = NULL;
  }

  if(active_key_count != NULL){
    *active_key_count = piano->active_key_count;
  }
  
  return(active_key);
}

/**
 * ags_piano_new:
 * @orientation: the #GtkOrientation
 * @key_width: the width of one key
 * @key_height: the height of one key
 * 
 * Create a new instance of #AgsPiano.
 * 
 * Returns: the new #AgsPiano instance
 * 
 * Since: 3.0.0
 */
AgsPiano*
ags_piano_new(GtkOrientation orientation,
	      guint key_width,
	      guint key_height)
{
  AgsPiano *piano;

  piano = (AgsPiano *) g_object_new(AGS_TYPE_PIANO,
				    "orientation", orientation,
				    "key-width", key_width,
				    "key-height", key_height,
				    NULL);
  
  return(piano);
}
