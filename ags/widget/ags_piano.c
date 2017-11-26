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

void ags_piano_class_init(AgsPianoClass *piano);
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
void ags_piano_show();

void ags_piano_map(GtkWidget *widget);
void ags_piano_realize(GtkWidget *widget);
void ags_piano_size_request(GtkWidget *widget,
			    GtkRequisition   *requisition);
void ags_piano_size_allocate(GtkWidget *widget,
			     GtkAllocation *allocation);
gboolean ags_piano_expose(GtkWidget *widget,
			  GdkEventExpose *event);

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

  //  widget->map = ags_piano_map;
  widget->realize = ags_piano_realize;
  widget->expose_event = ags_piano_expose;
  widget->size_request = ags_piano_size_request;
  widget->size_allocate = ags_piano_size_allocate;
  widget->show = ags_piano_show;

  /* AgsPianoClass */
  piano->key_pressed = ags_piano_real_key_pressed;
  piano->key_released = ags_piano_real_key_released;
  
  piano->key_clicked = ags_piano_real_key_clicked;

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
    g_signal_new("key-pressed\0",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_pressed),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__STRING_INT,
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
    g_signal_new("key-released\0",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_released),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__STRING_INT,
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
    g_signal_new("key-clicked\0",
		 G_TYPE_FROM_CLASS(piano),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsPianoClass, key_clicked),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__STRING_INT,
		 G_TYPE_NONE, 0,
		 G_TYPE_STRING,
		 G_TYPE_INT);
}

void
ags_piano_init(AgsPiano *piano)
{
  g_object_set(G_OBJECT(piano),
	       "app-paintable", TRUE,
	       NULL);

  piano->base_note = g_strdup(AGS_PIANO_DEFAULT_BASE_NOTE);
  
  piano->key_width = AGS_PIANO_DEFAULT_KEY_WIDTH;
  piano->key_height = AGS_PIANO_DEFAULT_KEY_HEIGHT;

  piano->key_count = AGS_PIANO_DEFAULT_KEY_COUNT;

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
  gdk_window_set_user_data(widget->window, cartesian);

  widget->style = gtk_style_attach(widget->style,
				   widget->window);
  gtk_style_set_background(widget->style,
			   widget->window,
			   GTK_STATE_NORMAL);

  gtk_widget_queue_resize(widget);
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
  requisition->width = -1;
  requisition->height = -1;
}

void
ags_piano_size_allocate(GtkWidget *widget,
			GtkAllocation *allocation)
{
  AgsPiano *piano;

  piano = AGS_PIANO(widget);
  
  widget->allocation = *allocation;

  allocation->width = piano->key_width;
  allocation->height = piano->key_height;
}

gboolean
ags_piano_expose(GtkWidget *widget,
		 GdkEventExpose *event)
{
  ags_piano_draw(AGS_PIANO(widget));

  return(FALSE);
}

void
ags_piano_draw(AgsPiano *piano)
{
  //TODO:JK: implement me
}

void
ags_piano_real_key_pressed(AgsPiano *piano,
			   gchar *note, gint key_code)
{
  //TODO:JK: implement me
}

void
ags_piano_key_pressed(AgsPiano *piano,
		      gchar *note, gint key_code)
{
  g_return_if_fail(AGS_IS_PIANO(piano));
  
  g_object_ref((GObject *) piano);
  g_signal_emit(G_OBJECT(piano),
		piano_signals[key_pressed], 0,
		note, key_code);
  g_object_unref((GObject *) piano);
}

void
ags_piano_real_key_released(AgsPiano *piano,
			    gchar *note, gint key_code)
{
  //TODO:JK: implement me
}

void
ags_piano_key_released(AgsPiano *piano,
		       gchar *note, gint key_code)
{
  g_return_if_fail(AGS_IS_PIANO(piano));
  
  g_object_ref((GObject *) piano);
  g_signal_emit(G_OBJECT(piano),
		piano_signals[key_released], 0,
		note, key_code);
  g_object_unref((GObject *) piano);
}

void
ags_piano_real_key_clicked(AgsPiano *piano,
			   gchar *note, gint key_code)
{
  //TODO:JK: implement me
}

void
ags_piano_key_clicked(AgsPiano *piano,
		      gchar *note, gint key_code)
{
  g_return_if_fail(AGS_IS_PIANO(piano));
  
  g_object_ref((GObject *) piano);
  g_signal_emit(G_OBJECT(piano),
		piano_signals[key_clicked], 0,
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
  
  if(count > 0){
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
