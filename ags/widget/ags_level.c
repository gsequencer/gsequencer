/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <atk/atk.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

static GType ags_accessible_level_get_type(void);
void ags_level_class_init(AgsLevelClass *level);
void ags_level_init(AgsLevel *level);
void ags_accessible_level_class_init(AtkObject *object);
void ags_accessible_level_value_interface_init(AtkValueIface *value);
void ags_accessible_level_action_interface_init(AtkActionIface *action);
void ags_level_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_level_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_level_finalize(GObject *gobject);
AtkObject* ags_level_get_accessible(GtkWidget *widget);
void ags_level_show(GtkWidget *widget);

void ags_accessible_level_get_value_and_text(AtkValue *value,
					     gdouble *current_value,
					     gchar **text);
#ifdef HAVE_ATK_2_12  
AtkRange* ags_accessible_level_get_range(AtkValue *value);
#endif
gdouble ags_accessible_level_get_increment(AtkValue *value);
void ags_accessible_level_set_value(AtkValue *value,
				    gdouble new_value);

gboolean ags_accessible_level_do_action(AtkAction *action,
					gint i);
gint ags_accessible_level_get_n_actions(AtkAction *action);
const gchar* ags_accessible_level_get_description(AtkAction *action,
						  gint i);
const gchar* ags_accessible_level_get_name(AtkAction *action,
					   gint i);
const gchar* ags_accessible_level_get_keybinding(AtkAction *action,
						 gint i);
gboolean ags_accessible_level_set_description(AtkAction *action,
					      gint i);
gchar* ags_accessible_level_get_localized_name(AtkAction *action,
					       gint i);

void ags_level_map(GtkWidget *widget);
void ags_level_realize(GtkWidget *widget);
void ags_level_size_request(GtkWidget *widget,
			    GtkRequisition   *requisition);
void ags_level_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation);
gboolean ags_level_expose(GtkWidget *widget,
			  GdkEventExpose *event);
gboolean ags_level_button_press(GtkWidget *widget,
				GdkEventButton *event);
gboolean ags_level_button_release(GtkWidget *widget,
				  GdkEventButton *event);
gboolean ags_level_key_press(GtkWidget *widget,
			     GdkEventKey *event);
gboolean ags_level_key_release(GtkWidget *widget,
			       GdkEventKey *event);
gboolean ags_level_motion_notify(GtkWidget *widget,
				 GdkEventMotion *event);

void ags_level_draw(AgsLevel *level);

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
  PROP_LEVEL_WIDTH,
  PROP_LEVEL_HEIGHT,
  PROP_LOWER,
  PROP_UPPER,
  PROP_NORMALIZED_VOLUME,
};

static gpointer ags_level_parent_class = NULL;
static guint level_signals[LAST_SIGNAL];

static GQuark quark_accessible_object = 0;

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

    ags_type_level = g_type_register_static(GTK_TYPE_WIDGET,
					    "AgsLevel", &ags_level_info,
					    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_level);
  }

  return g_define_type_id__volatile;
}

static GType
ags_accessible_level_get_type(void)
{
  static GType ags_type_accessible_level = 0;

  if(!ags_type_accessible_level){
    const GTypeInfo ags_accesssible_level_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_level_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_value_interface_info = {
      (GInterfaceInitFunc) ags_accessible_level_value_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_level_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_accessible_level = g_type_register_static(GTK_TYPE_ACCESSIBLE,
						       "AgsAccessibleLevel", &ags_accesssible_level_info,
						       0);

    g_type_add_interface_static(ags_type_accessible_level,
				ATK_TYPE_VALUE,
				&atk_value_interface_info);

    g_type_add_interface_static(ags_type_accessible_level,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_level);
}

void
ags_level_class_init(AgsLevelClass *level)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_level_parent_class = g_type_class_peek_parent(level);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");

  /* GObjectClass */
  gobject = (GObjectClass *) level;

  gobject->set_property = ags_level_set_property;
  gobject->get_property = ags_level_get_property;

  gobject->finalize = ags_level_finalize;

  /* properties */
  /**
   * AgsLevel:level-width:
   *
   * The level width to use for drawing a level.
   * 
   * Since: 2.2.22
   */
  param_spec = g_param_spec_uint("level-width",
				 "level width",
				 "The level width to use for drawing",
				 0,
				 G_MAXUINT,
				 AGS_LEVEL_DEFAULT_LEVEL_WIDTH,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LEVEL_WIDTH,
				  param_spec);

  /**
   * AgsLevel:level-height:
   *
   * The level height to use for drawing a level.
   * 
   * Since: 2.2.22
   */
  param_spec = g_param_spec_uint("level-height",
				 "level height",
				 "The level height to use for drawing",
				 0,
				 G_MAXUINT,
				 AGS_LEVEL_DEFAULT_LEVEL_HEIGHT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LEVEL_HEIGHT,
				  param_spec);

  /**
   * AgsLevel:lower:
   *
   * The level's lower range.
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
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
   * Since: 2.0.0
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

  widget->get_accessible = ags_level_get_accessible;
  //  widget->map = ags_level_map;
  widget->realize = ags_level_realize;
  widget->expose_event = ags_level_expose;
  widget->size_request = ags_level_size_request;
  widget->size_allocate = ags_level_size_allocate;
  widget->button_press_event = ags_level_button_press;
  widget->button_release_event = ags_level_button_release;
  widget->key_press_event = ags_level_key_press;
  widget->key_release_event = ags_level_key_release;
  widget->motion_notify_event = ags_level_motion_notify;
  widget->show = ags_level_show;

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
   * Since: 2.0.0
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
ags_accessible_level_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_level_value_interface_init(AtkValueIface *value)
{
  value->get_current_value = NULL;
  value->get_maximum_value = NULL;
  value->get_minimum_value = NULL;
  value->set_current_value = NULL;
  value->get_minimum_increment = NULL;

#ifdef HAVE_ATK_2_12  
  value->get_value_and_text = ags_accessible_level_get_value_and_text;
  value->get_range = ags_accessible_level_get_range;
  value->get_increment = ags_accessible_level_get_increment;
  value->get_sub_ranges = NULL;
  value->set_value = ags_accessible_level_set_value;
#endif
}

void
ags_accessible_level_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_level_do_action;
  action->get_n_actions = ags_accessible_level_get_n_actions;
  action->get_description = ags_accessible_level_get_description;
  action->get_name = ags_accessible_level_get_name;
  action->get_keybinding = ags_accessible_level_get_keybinding;
  action->set_description = ags_accessible_level_set_description;
  action->get_localized_name = ags_accessible_level_get_localized_name;
}

void
ags_level_init(AgsLevel *level)
{
  AtkObject *accessible;

  g_object_set(G_OBJECT(level),
	       "app-paintable", TRUE,
	       "can-focus", TRUE,
	       NULL);

  accessible = gtk_widget_get_accessible((GtkWidget *) level);

  g_object_set(accessible,
	       "accessible-name", "level",
	       "accessible-description", "Specify a default value",
	       NULL);

  level->flags = 0;

  level->key_mask = 0;
  level->button_state = 0;
  level->layout = AGS_LEVEL_LAYOUT_VERTICAL;

  level->font_size = 11;

  level->level_width = AGS_LEVEL_DEFAULT_LEVEL_WIDTH;
  level->level_height = AGS_LEVEL_DEFAULT_LEVEL_HEIGHT;

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
  case PROP_LEVEL_WIDTH:
    {
      level->level_width = g_value_get_uint(value);
    }
    break;
  case PROP_LEVEL_HEIGHT:
    {
      level->level_height = g_value_get_uint(value);
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
  case PROP_LEVEL_WIDTH:
  {
    g_value_set_uint(value,
		     level->level_width);
  }
  break;
  case PROP_LEVEL_HEIGHT:
  {
    g_value_set_uint(value,
		     level->level_height);
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
ags_accessible_level_get_value_and_text(AtkValue *value,
					gdouble *current_value,
					gchar **text)
{
  AgsLevel *level;
  
  level = (AgsLevel *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  if(current_value != NULL){
    *current_value = level->normalized_volume;
  }

  if(text != NULL){
    *text = g_strdup_printf("%f",
			    level->normalized_volume);
  }
}

#ifdef HAVE_ATK_2_12
AtkRange*
ags_accessible_level_get_range(AtkValue *value)
{
  AgsLevel *level;
  AtkRange *range;
  
  level = (AgsLevel *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  range = atk_range_new(level->lower,
			level->upper,
			"Valid lower and upper input range of this level");
}
#endif

gdouble
ags_accessible_level_get_increment(AtkValue *value)
{
  AgsLevel *level;

  level = (AgsLevel *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));

  return((level->upper - level->lower) / level->step_count);
}

void
ags_accessible_level_set_value(AtkValue *value,
			       gdouble new_value)
{
  AgsLevel *level;

  level = (AgsLevel *) gtk_accessible_get_widget(GTK_ACCESSIBLE(value));
  level->normalized_volume = new_value;

  gtk_widget_queue_draw((GtkWidget *) level);
}

gboolean
ags_accessible_level_do_action(AtkAction *action,
			       gint i)
{
  AgsLevel *level;
  
  GdkEventKey *key_press, *key_release;
  
  if(!(i >= 0 && i < 4)){
    return(FALSE);
  }

  level = (AgsLevel *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = gdk_event_new(GDK_KEY_PRESS);
  key_release = gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_LEVEL_STEP_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_LEVEL_STEP_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_LEVEL_PAGE_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Page_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_LEVEL_PAGE_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Page_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) level,
		       (GdkEvent *) key_release);
    }
    break;
  }

  return(TRUE);
}

gint
ags_accessible_level_get_n_actions(AtkAction *action)
{
  return(4);
}

const gchar*
ags_accessible_level_get_description(AtkAction *action,
				     gint i)
{
  static const gchar *actions[] = {
    "step up level default value",
    "step down level default value",
    "page up level default value",
    "page down level default value",
  };

  if(i >= 0 && i < 4){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_level_get_name(AtkAction *action,
			      gint i)
{
  static const gchar *actions[] = {
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
ags_accessible_level_get_keybinding(AtkAction *action,
				    gint i)
{
  static const gchar *actions[] = {
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
ags_accessible_level_set_description(AtkAction *action,
				     gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_level_get_localized_name(AtkAction *action,
					gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_level_map(GtkWidget *widget)
{
  if(gtk_widget_get_realized (widget) && !gtk_widget_get_mapped(widget)){
    GTK_WIDGET_CLASS(ags_level_parent_class)->map(widget);
    
    gdk_window_show(widget->window);
    ags_level_draw((AgsLevel *) widget);
  }
}

void
ags_level_realize(GtkWidget *widget)
{
  AgsLevel *level;

  GdkWindowAttr attributes;

  gint attributes_mask;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(AGS_IS_LEVEL(widget));

  level = AGS_LEVEL(widget);

  gtk_widget_set_realized(widget, TRUE);

  /*  */
  attributes.window_type = GDK_WINDOW_CHILD;
  
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = level->level_width;
  attributes.height = level->level_height;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= (GDK_EXPOSURE_MASK);

  widget->window = gdk_window_new(gtk_widget_get_parent_window (widget),
				  &attributes, attributes_mask);
  gdk_window_set_user_data(widget->window, level);

  widget->style = gtk_style_attach(widget->style,
				   widget->window);
  gtk_style_set_background(widget->style,
			   widget->window,
			   GTK_STATE_NORMAL);

  gtk_widget_queue_resize(widget);
}

AtkObject*
ags_level_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_level_get_type(),
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
ags_level_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_level_parent_class)->show(widget);
}

void
ags_level_size_request(GtkWidget *widget,
		       GtkRequisition *requisition)
{
  AgsLevel *level;

  level = AGS_LEVEL(widget);

  if(level->layout == AGS_LEVEL_LAYOUT_VERTICAL){
    requisition->width = level->level_width;
    requisition->height = level->level_height;
  }else if(level->layout == AGS_LEVEL_LAYOUT_HORIZONTAL){
    requisition->width = level->level_height;
    requisition->height = level->level_width;
  }
}

void
ags_level_size_allocate(GtkWidget *widget,
			GtkAllocation *allocation)
{
  AgsLevel *level;

  GdkWindow *window;
    
  level = AGS_LEVEL(widget);
  
  widget->allocation = *allocation;
  
  window = gtk_widget_get_window(widget);
  gdk_window_move(window,
		  allocation->x, allocation->y);

  if(level->layout == AGS_LEVEL_LAYOUT_VERTICAL){
    widget->allocation.width = level->level_width;
    widget->allocation.height = level->level_height;

    allocation->height = level->level_height;
  }else if(level->layout == AGS_LEVEL_LAYOUT_HORIZONTAL){
    widget->allocation.width = level->level_height;
    widget->allocation.height = level->level_width;

    allocation->width = level->level_height;
  }
}

gboolean
ags_level_expose(GtkWidget *widget,
		 GdkEventExpose *event)
{
  ags_level_draw(AGS_LEVEL(widget));

  return(FALSE);
}

gboolean
ags_level_button_press(GtkWidget *widget,
		       GdkEventButton *event)
{
  AgsLevel *level;

  guint width, height;
  guint x_start, y_start;

  level = AGS_LEVEL(widget);

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
      gdouble normalized_volume;
      
      level->button_state |= AGS_LEVEL_BUTTON_1_PRESSED;

      c_range = level->upper - level->lower;
      
      if(level->layout == AGS_LEVEL_LAYOUT_VERTICAL){
	normalized_volume = event->y / c_range;
      }else if(level->layout == AGS_LEVEL_LAYOUT_HORIZONTAL){
	normalized_volume = event->x / c_range;
      }

      level->normalized_volume = normalized_volume;
      gtk_widget_queue_draw((GtkWidget *) level);

      ags_level_value_changed(level,
			      normalized_volume);
    }
  }
  
  return(FALSE);
}

gboolean
ags_level_button_release(GtkWidget *widget,
			 GdkEventButton *event)
{
  AgsLevel *level;

  gtk_widget_grab_focus(widget);

  level = AGS_LEVEL(widget);
  
  if(event->button == 1){
    if((AGS_LEVEL_BUTTON_1_PRESSED & (level->button_state)) != 0){
      gdouble c_range;
      gdouble normalized_volume;
      
      level->button_state |= AGS_LEVEL_BUTTON_1_PRESSED;

      c_range = level->upper - level->lower;
      
      if(level->layout == AGS_LEVEL_LAYOUT_VERTICAL){
	normalized_volume = event->y / c_range;
      }else if(level->layout == AGS_LEVEL_LAYOUT_HORIZONTAL){
	normalized_volume = event->x / c_range;
      }

      level->normalized_volume = normalized_volume;
      gtk_widget_queue_draw((GtkWidget *) level);

      ags_level_value_changed(level,
			      normalized_volume);
    }
    
    level->button_state &= (~AGS_LEVEL_BUTTON_1_PRESSED);
  }

  return(FALSE);
}

gboolean
ags_level_key_press(GtkWidget *widget,
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
    return(GTK_WIDGET_CLASS(ags_level_parent_class)->key_press_event(widget, event));
  }

  return(TRUE);
}

gboolean
ags_level_key_release(GtkWidget *widget,
		      GdkEventKey *event)
{
  AgsLevel *level;

  //TODO:JK: implement me
  
  if(event->keyval == GDK_KEY_Tab ||
     event->keyval == GDK_ISO_Left_Tab ||
     event->keyval == GDK_KEY_Shift_L ||
     event->keyval == GDK_KEY_Shift_R ||
     event->keyval == GDK_KEY_Alt_L ||
     event->keyval == GDK_KEY_Alt_R ||
     event->keyval == GDK_KEY_Control_L ||
     event->keyval == GDK_KEY_Control_R ){
    return(GTK_WIDGET_CLASS(ags_level_parent_class)->key_release_event(widget, event));
  }

  level = AGS_LEVEL(widget);
  
  switch(event->keyval){
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

      gtk_widget_queue_draw(widget);

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

      gtk_widget_queue_draw(widget);

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

      gtk_widget_queue_draw(widget);

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

      gtk_widget_queue_draw(widget);

      ags_level_value_changed(level,
			      level->normalized_volume);
    }
    break;
  }
  
  return(TRUE);
}

gboolean
ags_level_motion_notify(GtkWidget *widget,
			GdkEventMotion *event)
{
  AgsLevel *level;

  guint width, height;
  guint x_start, y_start;
  
  level = AGS_LEVEL(widget);

  width = widget->allocation.width;
  height = widget->allocation.height;

  x_start = 0;
  y_start = 0;

  if((AGS_LEVEL_BUTTON_1_PRESSED & (level->button_state)) != 0){
    gdouble c_range;
    gdouble new_normalized_volume;
      
    level->button_state |= AGS_LEVEL_BUTTON_1_PRESSED;

    c_range = level->upper - level->lower;
      
    if(level->layout == AGS_LEVEL_LAYOUT_VERTICAL){
      new_normalized_volume = event->y / c_range;
    }else if(level->layout == AGS_LEVEL_LAYOUT_HORIZONTAL){
      new_normalized_volume = event->x / c_range;
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
ags_level_draw(AgsLevel *level)
{
  cairo_t *cr;

  gchar *str;
  
  guint width, height;
  guint x_start, y_start;

  auto void ags_level_draw_string(cairo_t *cr, gchar *str);
  
  void ags_level_draw_string(cairo_t *cr, gchar *str){
    PangoLayout *layout;
    PangoFontDescription *desc;

    layout = pango_cairo_create_layout(cr);
    pango_layout_set_text(layout, str, -1);
    desc = pango_font_description_from_string("Sans Slant"); //pango_font_description_copy_static("Georgia Bold 11");
    pango_font_description_set_size(desc,
				    level->font_size * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);

    pango_cairo_update_layout(cr, layout);
    pango_cairo_show_layout(cr, layout);

#ifndef __APPLE__
    //    pango_fc_font_map_cache_clear(pango_cairo_font_map_get_default());
#endif
    g_object_unref(layout);
  }
  
  static const gdouble white_gc = 65535.0;

  if(!AGS_IS_LEVEL(level)){
    return;
  }

  cr = gdk_cairo_create(GTK_WIDGET(level)->window);
  
  if(cr == NULL){
    return;
  }
  
  width = GTK_WIDGET(level)->allocation.width;
  height = GTK_WIDGET(level)->allocation.height;
  
  x_start = 0;
  y_start = 0;

  cairo_surface_flush(cairo_get_target(cr));
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
  //TODO:JK: implement me

  /* show samplerate */
  str = g_strdup_printf("%u [Hz]", level->samplerate);
  
  cairo_set_source_rgb(cr,
		       1.0, 1.0, 1.0);

  if(level->layout == AGS_LEVEL_LAYOUT_VERTICAL){
    cairo_move_to(cr,
		  x_start + level->font_size, y_start + height - 1.0);
    cairo_rotate(cr,
		 2 * M_PI * 0.75);
  }else{
    cairo_move_to(cr,
		  x_start + level->font_size, y_start + 1.0);
  }
  
  ags_level_draw_string(cr,
			str);

  cairo_pop_group_to_source(cr);
  cairo_paint(cr);

  cairo_surface_mark_dirty(cairo_get_target(cr));
  cairo_destroy(cr);
}

/**
 * ags_level_value_changed:
 * @level: the #AgsLevel
 * @normalized_volume: the normalized volume
 * 
 * Emits ::value-changed event.
 * 
 * Since: 2.0.0
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
 * 
 * Create a new instance of #AgsLevel.
 * 
 * Returns: the new #AgsLevel instance
 * 
 * Since: 2.0.0
 */
AgsLevel*
ags_level_new()
{
  AgsLevel *level;

  level = (AgsLevel *) g_object_new(AGS_TYPE_LEVEL,
				    NULL);
  
  return(level);
}
