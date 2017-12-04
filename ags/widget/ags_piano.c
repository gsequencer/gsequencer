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

#include <ags/widget/ags_piano.h>

#include <ags/widget/ags_widget_marshal.h>

#include <atk/atk.h>

#include <gdk/gdkkeysyms.h>

static GType ags_accessible_piano_get_type(void);
void ags_piano_class_init(AgsPianoClass *piano);
void ags_piano_init(AgsPiano *piano);
void ags_accessible_piano_class_init(AtkObject *object);
void ags_accessible_piano_value_interface_init(AtkValueIface *value);
void ags_accessible_piano_action_interface_init(AtkActionIface *action);
void ags_piano_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_piano_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_piano_finalize(GObject *gobject);
AtkObject* ags_piano_get_accessible(GtkWidget *widget);
void ags_piano_show(GtkWidget *widget);

void ags_accessible_piano_get_value_and_text(AtkValue *value,
					     gdouble *current_value,
					     gchar **text);
#ifdef HAVE_ATK_2_12  
AtkRange* ags_accessible_piano_get_range(AtkValue *value);
#endif
gdouble ags_accessible_piano_get_increment(AtkValue *value);
void ags_accessible_piano_set_value(AtkValue *value,
				    gdouble new_value);

gboolean ags_accessible_piano_do_action(AtkAction *action,
					gint i);
gint ags_accessible_piano_get_n_actions(AtkAction *action);
const gchar* ags_accessible_piano_get_description(AtkAction *action,
						  gint i);
const gchar* ags_accessible_piano_get_name(AtkAction *action,
					   gint i);
const gchar* ags_accessible_piano_get_keybinding(AtkAction *action,
						 gint i);
gboolean ags_accessible_piano_set_description(AtkAction *action,
					      gint i);
gchar* ags_accessible_piano_get_localized_name(AtkAction *action,
					       gint i);

void ags_piano_map(GtkWidget *widget);
void ags_piano_realize(GtkWidget *widget);
void ags_piano_size_request(GtkWidget *widget,
			    GtkRequisition   *requisition);
void ags_piano_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation);
gboolean ags_piano_expose(GtkWidget *widget,
			  GdkEventExpose *event);
gboolean ags_piano_button_press(GtkWidget *widget,
				GdkEventButton *event);
gboolean ags_piano_button_release(GtkWidget *widget,
				  GdkEventButton *event);
gboolean ags_piano_key_press(GtkWidget *widget,
			     GdkEventKey *event);
gboolean ags_piano_key_release(GtkWidget *widget,
			       GdkEventKey *event);
gboolean ags_piano_motion_notify(GtkWidget *widget,
				 GdkEventMotion *event);

void ags_piano_draw(AgsPiano *piano);

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
  PROP_BASE_NOTE,
  PROP_KEY_WIDTH,
  PROP_KEY_HEIGHT,
  PROP_KEY_COUNT,
};

static gpointer ags_piano_parent_class = NULL;
static guint piano_signals[LAST_SIGNAL];

static GQuark quark_accessible_object = 0;

GType
ags_piano_get_type(void)
{
  static GType ags_type_piano = 0;

  if(!ags_type_piano){
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

    ags_type_piano = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsPiano", &ags_piano_info,
					    0);
  }

  return(ags_type_piano);
}

static GType
ags_accessible_piano_get_type(void)
{
  static GType ags_type_accessible_piano = 0;

  if(!ags_type_accessible_piano){
    const GTypeInfo ags_accesssible_piano_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_piano_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_value_interface_info = {
      (GInterfaceInitFunc) ags_accessible_piano_value_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_piano_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_accessible_piano = g_type_register_static(GTK_TYPE_ACCESSIBLE,
						       "AgsAccessiblePiano", &ags_accesssible_piano_info,
						       0);

    g_type_add_interface_static(ags_type_accessible_piano,
				ATK_TYPE_VALUE,
				&atk_value_interface_info);

    g_type_add_interface_static(ags_type_accessible_piano,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_piano);
}

void
ags_piano_class_init(AgsPianoClass *piano)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_piano_parent_class = g_type_class_peek_parent(piano);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");

  /* GObjectClass */
  gobject = (GObjectClass *) piano;

  gobject->set_property = ags_piano_set_property;
  gobject->get_property = ags_piano_get_property;

  gobject->finalize = ags_piano_finalize;

  /* properties */
  /**
   * AgsPiano:base-note:
   *
   * The base note to use as lower.
   * 
   * Since: 1.2.0
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
   * AgsPiano:key-width:
   *
   * The key width to use for drawing a key.
   * 
   * Since: 1.2.0
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
   * Since: 1.2.0
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
   * Since: 1.2.0
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

  widget->get_accessible = ags_piano_get_accessible;
  //  widget->map = ags_piano_map;
  widget->realize = ags_piano_realize;
  widget->expose_event = ags_piano_expose;
  widget->size_request = ags_piano_size_request;
  widget->size_allocate = ags_piano_size_allocate;
  widget->button_press_event = ags_piano_button_press;
  widget->button_release_event = ags_piano_button_release;
  widget->key_press_event = ags_piano_key_press;
  widget->key_release_event = ags_piano_key_release;
  widget->motion_notify_event = ags_piano_motion_notify;
  widget->show = ags_piano_show;

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
   * Since: 1.2.0
   */
  piano_signals[KEY_PRESSED] =
    g_signal_new("key-pressed",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_pressed),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 0,
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
   * Since: 1.2.0
   */
  piano_signals[KEY_RELEASED] =
    g_signal_new("key-released",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_released),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 0,
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
   * Since: 1.2.0
   */
  piano_signals[KEY_CLICKED] =
    g_signal_new("key-clicked",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_clicked),
		 NULL, NULL,
		 ags_widget_cclosure_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 0,
		 G_TYPE_STRING,
		 G_TYPE_INT);
}

void
ags_accessible_piano_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_piano_value_interface_init(AtkValueIface *value)
{
  value->get_current_value = NULL;
  value->get_maximum_value = NULL;
  value->get_minimum_value = NULL;
  value->set_current_value = NULL;
  value->get_minimum_increment = NULL;

#ifdef HAVE_ATK_2_12  
  value->get_value_and_text = ags_accessible_piano_get_value_and_text;
  value->get_range = ags_accessible_piano_get_range;
  value->get_increment = ags_accessible_piano_get_increment;
  value->get_sub_ranges = NULL;
  value->set_value = ags_accessible_piano_set_value;
#endif
}

void
ags_accessible_piano_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_piano_do_action;
  action->get_n_actions = ags_accessible_piano_get_n_actions;
  action->get_description = ags_accessible_piano_get_description;
  action->get_name = ags_accessible_piano_get_name;
  action->get_keybinding = ags_accessible_piano_get_keybinding;
  action->set_description = ags_accessible_piano_set_description;
  action->get_localized_name = ags_accessible_piano_get_localized_name;
}

void
ags_piano_init(AgsPiano *piano)
{
  AtkObject *accessible;

  g_object_set(G_OBJECT(piano),
	       "app-paintable", TRUE,
	       "can-focus", TRUE,
	       NULL);

  accessible = gtk_widget_get_accessible((GtkWidget *) piano);

  g_object_set(accessible,
	       "accessible-name", "piano",
	       "accessible-description", "Hit a key at cursor position",
	       NULL);

  piano->flags = 0;

  piano->button_state = 0;
  piano->layout = AGS_PIANO_LAYOUT_VERTICAL;

  piano->font_size = 12;
  
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
  case PROP_BASE_NOTE:
    {
      g_value_set_string(value, piano->base_note);
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
ags_accessible_piano_get_value_and_text(AtkValue *value,
					gdouble *current_value,
					gchar **text)
{
  AgsPiano *piano;
  
  piano = (AgsPiano *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  if(current_value != NULL){
    *current_value = (gdouble) piano->cursor_position;
  }

  if(text != NULL){
    *text = g_strdup_printf("%d",
			    piano->cursor_position);
  }
}

#ifdef HAVE_ATK_2_12
AtkRange*
ags_accessible_piano_get_range(AtkValue *value)
{
  AgsPiano *piano;
  AtkRange *range;
  
  piano = (AgsPiano *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  range = atk_range_new((gdouble) piano->base_key_code,
			(gdouble) (piano->base_key_code + piano->key_count),
			"Valid lower and upper input range of this piano");
}
#endif

gdouble
ags_accessible_piano_get_increment(AtkValue *value)
{
  AgsPiano *piano;

  piano = (AgsPiano *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  return(1.0);
}

void
ags_accessible_piano_set_value(AtkValue *value,
			       gdouble new_value)
{
  AgsPiano *piano;

  piano = (AgsPiano *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));
  piano->cursor_position = (gint) new_value;
  gtk_widget_queue_draw((GtkWidget *) piano);
}

gboolean
ags_accessible_piano_do_action(AtkAction *action,
			       gint i)
{
  AgsPiano *piano;
  
  GdkEventKey *key_press, *key_release;
  
  if(!(i >= 0 && i < 3)){
    return(FALSE);
  }

  piano = (AgsPiano *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = gdk_event_new(GDK_KEY_PRESS);
  key_release = gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_PIANO_MOVE_CURSOR_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) piano,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) piano,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PIANO_MOVE_CURSOR_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) piano,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) piano,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PIANO_HIT_KEY:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) piano,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) piano,
		       (GdkEvent *) key_release);
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_piano_get_n_actions(AtkAction *action)
{
  return(3);
}

const gchar*
ags_accessible_piano_get_description(AtkAction *action,
				     gint i)
{
  static const gchar **actions = {
    "move up one piano key",
    "move down one piano key",
    "hit current piano key",
  };

  if(i >= 0 && i < 3){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_piano_get_name(AtkAction *action,
			      gint i)
{
  static const gchar **actions = {
    "move-up",
    "move-down",
    "hit-key",
  };
  
  if(i >= 0 && i < 3){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_piano_get_keybinding(AtkAction *action,
				    gint i)
{
  static const gchar **actions = {
    "up",
    "down",
    "space"
  };
  
  if(i >= 0 && i < 3){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_piano_set_description(AtkAction *action,
				     gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_piano_get_localized_name(AtkAction *action,
					gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_piano_map(GtkWidget *widget)
{
  if(gtk_widget_get_realized (widget) && !gtk_widget_get_mapped(widget)){
    GTK_WIDGET_CLASS(ags_piano_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    ags_piano_draw((AgsPiano *) widget);
  }
}

void
ags_piano_realize(GtkWidget *widget)
{
  AgsPiano *piano;

  GdkWindowAttr attributes;

  gint attributes_mask;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_PIANO(widget));

  piano = AGS_PIANO(widget);

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
  gdk_window_set_user_data(widget->window, piano);

  widget->style = gtk_style_attach(widget->style,
				   widget->window);
  gtk_style_set_background(widget->style,
			   widget->window,
			   GTK_STATE_NORMAL);

  gtk_widget_queue_resize(widget);
}

AtkObject*
ags_piano_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_piano_get_type(),
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
ags_piano_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_piano_parent_class)->show(widget);
}

void
ags_piano_size_request(GtkWidget *widget,
		       GtkRequisition *requisition)
{
  AgsPiano *piano;

  piano = AGS_PIANO(widget);

  if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    requisition->width = piano->key_width;
    requisition->height = piano->key_count * piano->key_height;
  }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    requisition->width = piano->key_count * piano->key_height;
    requisition->height = piano->key_width;
  }
}

void
ags_piano_size_allocate(GtkWidget *widget,
			GtkAllocation *allocation)
{
  AgsPiano *piano;

  piano = AGS_PIANO(widget);
  
  widget->allocation = *allocation;

  if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    widget->allocation.width = piano->key_width;
    widget->allocation.height = piano->key_count * piano->key_height;
  }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    widget->allocation.width = piano->key_count * piano->key_height;
    widget->allocation.height = piano->key_width;
  }
}

gboolean
ags_piano_expose(GtkWidget *widget,
		 GdkEventExpose *event)
{
  ags_piano_draw(AGS_PIANO(widget));

  return(FALSE);
}

gboolean
ags_piano_button_press(GtkWidget *widget,
		       GdkEventButton *event)
{
  AgsPiano *piano;

  guint width, height;
  guint x_start, y_start;

  piano = AGS_PIANO(widget);

  width = widget->allocation.width;
  height = widget->allocation.height;

  x_start = 0;
  y_start = 0;

  if(event->x >= x_start &&
     event->x < width &&
     event->y >= y_start &&
     event->y < height){
    if(event->button == 1){
      gchar *note;
      
      piano->button_state |= AGS_PIANO_BUTTON_1_PRESSED;

      if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
	piano->current_key = floor(event->y / piano->key_height);
      }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
	piano->current_key = floor(event->x / piano->key_height);
      }

      note = ags_piano_key_code_to_note(piano->current_key);

      ags_piano_key_pressed(piano,
			    note, piano->current_key);

      g_free(note);
    }
  }
  
  return(FALSE);
}

gboolean
ags_piano_button_release(GtkWidget *widget,
			 GdkEventButton *event)
{
  AgsPiano *piano;

  gtk_widget_grab_focus(widget);

  piano = AGS_PIANO(widget);
  
  if(event->button == 1){
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
  }

  return(FALSE);
}

gboolean
ags_piano_key_press(GtkWidget *widget,
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
    return(GTK_WIDGET_CLASS(ags_piano_parent_class)->key_press_event(widget, event));
  }

  return(TRUE);
}

gboolean
ags_piano_key_release(GtkWidget *widget,
		      GdkEventKey *event)
{
  AgsPiano *piano;

  //TODO:JK: implement me
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    return(GTK_WIDGET_CLASS(ags_piano_parent_class)->key_release_event(widget, event));
  }

  piano = AGS_PIANO(widget);
  
  switch(event->keyval){
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

      gtk_widget_queue_draw(widget);
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

      gtk_widget_queue_draw(widget);
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
  
  return(TRUE);
}

gboolean
ags_piano_motion_notify(GtkWidget *widget,
			GdkEventMotion *event)
{
  AgsPiano *piano;

  guint width, height;
  guint x_start, y_start;

  gint new_current_key;
  
  piano = AGS_PIANO(widget);

  width = widget->allocation.width;
  height = widget->allocation.height;

  x_start = 0;
  y_start = 0;

  if((AGS_PIANO_BUTTON_1_PRESSED & (piano->button_state)) != 0){
    if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
      new_current_key = floor(event->y / piano->key_height);
    }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
      new_current_key = floor(event->x / piano->key_height);
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
    if(event->x >= x_start &&
       event->x < width &&
       event->y >= y_start &&
       event->y < height){
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
ags_piano_draw(AgsPiano *piano)
{
  cairo_t *cr;
  
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
  
  static const gdouble white_gc = 65535.0;
  static const guint bitmap = 0x52a52a;

  if(!AGS_IS_PIANO(piano)){
    return;
  }

  cr = gdk_cairo_create(GTK_WIDGET(piano)->window);
  
  if(cr == NULL){
    return;
  }

  if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    width = GTK_WIDGET(piano)->allocation.width;

    if(piano->key_count * piano->key_height + (piano->key_height / 2) < GTK_WIDGET(piano)->allocation.height){
      height = piano->key_count * piano->key_height + (piano->key_height / 2);
    }else{
      height = GTK_WIDGET(piano)->allocation.height;
    }
  }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
    if(piano->key_count * piano->key_height + (piano->key_height / 2) < GTK_WIDGET(piano)->allocation.height){
      width = piano->key_count * piano->key_height + (piano->key_height / 2);
    }else{
      width = GTK_WIDGET(piano)->allocation.width;
    }

    height = GTK_WIDGET(piano)->allocation.height;
  }
  
  x_start = 0;
  y_start = 0;
  
  /* fill bg */
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr,
		  (double) x_start, (double) y_start,
		  (double) width, (double) height);
  cairo_fill(cr);

  /* draw */
  control_x0 = x_start;
  control_y0 = y_start;
  
  if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
    big_control_width = piano->key_width;
    big_control_height = piano->key_height;
    
    small_control_width = (2.0 / 3.0) * piano->key_width;
    small_control_height = piano->key_height;
  }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
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

      cairo_move_to(cr,
		    (double) (control_x0 + small_control_width), (double) (control_y0 + small_control_height / 2));
      cairo_line_to(cr,
		    (double) (control_x0 + big_control_width), (double) (control_y0 + small_control_height / 2));
      cairo_stroke(cr);
    }else{
      /* draw no semi tone key */
      cairo_set_source_rgb(cr,
			   0.68, 0.68, 0.68);

      if(((1 << (j + 1)) & bitmap) == 0){
	cairo_move_to(cr,
		      (double) control_x0, (double) (control_y0 + big_control_height));
	cairo_line_to(cr,
		      (double) (control_x0 + big_control_width), (double) (control_y0 + big_control_height));
	cairo_stroke(cr);
      }

      cairo_move_to(cr,
		    (double) (control_x0 + big_control_width), (double) control_y0);
      cairo_line_to(cr,
		    (double) (control_x0 + big_control_width), (double) (control_y0 + big_control_height));
      cairo_stroke(cr);
    }

    /* iterate - offset */
    if(piano->layout == AGS_PIANO_LAYOUT_VERTICAL){
      control_y0 += piano->key_height;
    }else if(piano->layout == AGS_PIANO_LAYOUT_HORIZONTAL){
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
}

/**
 * ags_piano_key_code_to_note:
 * @key_code: the key code
 * 
 * Get note from key code.
 * 
 * Returns: the note as string
 * 
 * Since: 1.2.0 
 */
gchar*
ags_piano_key_code_to_note(gint key_code)
{
  gchar *note;
  gchar *prefix;
  gchar *suffix;

  guint tic_count;
  guint i;
  
  static const gchar **note_map = {
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
  gtk_widget_queue_draw(piano);
}

/**
 * ags_piano_key_pressed:
 * @piano: the #AgsPiano
 * @note: the string representation of key
 * @key_code: the key code
 * 
 * Emits ::key-pressed event.
 * 
 * Since: 1.2.0
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
  gtk_widget_queue_draw(piano);
}

/**
 * ags_piano_key_released:
 * @piano: the #AgsPiano
 * @note: the string representation of key
 * @key_code: the key code
 * 
 * Emits ::key-released event.
 * 
 * Since: 1.2.0
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
 * Since: 1.2.0
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
 * Since: 1.2.0
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
 * 
 * Create a new instance of #AgsPiano.
 * 
 * Returns: the new #AgsPiano instance
 * 
 * Since: 1.2.0
 */
AgsPiano*
ags_piano_new()
{
  AgsPiano *piano;

  piano = (AgsPiano *) g_object_new(AGS_TYPE_PIANO,
				    NULL);
  
  return(piano);
}
