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

#include <ags/X/machine/ags_cell_pattern.h>
#include <ags/X/machine/ags_cell_pattern_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <gdk/gdkkeysyms.h>

#include <atk/atk.h>

static GType ags_accessible_cell_pattern_get_type(void);
void ags_cell_pattern_class_init(AgsCellPatternClass *cell_pattern);
void ags_accessible_cell_pattern_class_init(AtkObject *object);
void ags_accessible_cell_pattern_action_interface_init(AtkActionIface *action);
void ags_cell_pattern_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cell_pattern_init(AgsCellPattern *cell_pattern);
void ags_cell_pattern_finalize(GObject *gobject);
void ags_cell_pattern_connect(AgsConnectable *connectable);
void ags_cell_pattern_disconnect(AgsConnectable *connectable);
AtkObject* ags_cell_pattern_get_accessible(GtkWidget *widget);
void ags_cell_pattern_realize(GtkWidget *widget);
void ags_cell_pattern_show(GtkWidget *widget);
void ags_cell_pattern_show_all(GtkWidget *widget);

gboolean ags_accessible_cell_pattern_do_action(AtkAction *action,
					       gint i);
gint ags_accessible_cell_pattern_get_n_actions(AtkAction *action);
const gchar* ags_accessible_cell_pattern_get_description(AtkAction *action,
							 gint i);
const gchar* ags_accessible_cell_pattern_get_name(AtkAction *action,
						  gint i);
const gchar* ags_accessible_cell_pattern_get_keybinding(AtkAction *action,
							gint i);
gboolean ags_accessible_cell_pattern_set_description(AtkAction *action,
						     gint i);
gchar* ags_accessible_cell_pattern_get_localized_name(AtkAction *action,
						      gint i);

/**
 * SECTION:ags_cell_pattern
 * @short_description: cell pattern sequencer
 * @title: AgsCellPattern
 * @section_id:
 * @include: ags/X/machine/ags_cell_pattern.h
 *
 * The #AgsCellPattern is a composite widget to act as cell pattern sequencer.
 */

static gpointer ags_cell_pattern_parent_class = NULL;
static GQuark quark_accessible_object = 0;

GtkStyle *cell_pattern_style = NULL;
GHashTable *ags_cell_pattern_led_queue_draw = NULL;

GType
ags_cell_pattern_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_cell_pattern = 0;

    static const GTypeInfo ags_cell_pattern_info = {
      sizeof(AgsCellPatternClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cell_pattern_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCellPattern),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cell_pattern_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cell_pattern_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_cell_pattern = g_type_register_static(GTK_TYPE_TABLE,
						   "AgsCellPattern", &ags_cell_pattern_info,
						   0);
    
    g_type_add_interface_static(ags_type_cell_pattern,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_cell_pattern);
  }

  return g_define_type_id__volatile;
}

static GType
ags_accessible_cell_pattern_get_type(void)
{
  static GType ags_type_accessible_cell_pattern = 0;

  if(!ags_type_accessible_cell_pattern){
    const GTypeInfo ags_accesssible_cell_pattern_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_cell_pattern_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_cell_pattern_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_cell_pattern = g_type_register_static(GTK_TYPE_ACCESSIBLE,
							      "AgsAccessibleCellPattern", &ags_accesssible_cell_pattern_info,
							      0);

    g_type_add_interface_static(ags_type_accessible_cell_pattern,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_cell_pattern);
}

void
ags_cell_pattern_class_init(AgsCellPatternClass *cell_pattern)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_cell_pattern_parent_class = g_type_class_peek_parent(cell_pattern);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");

  /* GObjectClass */
  gobject = (GObjectClass *) cell_pattern;

  gobject->finalize = ags_cell_pattern_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) cell_pattern;

  widget->realize = ags_cell_pattern_realize;
  widget->show = ags_cell_pattern_show;
  widget->show_all = ags_cell_pattern_show_all;
}

void
ags_accessible_cell_pattern_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_cell_pattern_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_cell_pattern_do_action;
  action->get_n_actions = ags_accessible_cell_pattern_get_n_actions;
  action->get_description = ags_accessible_cell_pattern_get_description;
  action->get_name = ags_accessible_cell_pattern_get_name;
  action->get_keybinding = ags_accessible_cell_pattern_get_keybinding;
  action->set_description = ags_accessible_cell_pattern_set_description;
  action->get_localized_name = ags_accessible_cell_pattern_get_localized_name;
}

void
ags_cell_pattern_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_cell_pattern_connectable_parent_interface;

  connectable->connect = ags_cell_pattern_connect;
  connectable->disconnect = ags_cell_pattern_disconnect;
}

void
ags_cell_pattern_init(AgsCellPattern *cell_pattern)
{
  GtkAdjustment *adjustment;
  AgsLed *led;

  guint i;

  g_object_set(cell_pattern,
	       "can-focus", TRUE,
	       "n-columns", 2,
	       "n-rows", 2,
	       "homogeneous", FALSE,
	       NULL);

  cell_pattern->flags = 0;

  cell_pattern->key_mask = 0;
  
  cell_pattern->cell_width = AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH;
  cell_pattern->cell_height = AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT;

  cell_pattern->n_cols = AGS_CELL_PATTERN_DEFAULT_CONTROLS_HORIZONTALLY;
  cell_pattern->n_rows = AGS_CELL_PATTERN_DEFAULT_CONTROLS_VERTICALLY;

  cell_pattern->cursor_x = 0;
  cell_pattern->cursor_y = 0;

  cell_pattern->drawing_area = (GtkDrawingArea *) gtk_drawing_area_new();  

  gtk_widget_set_size_request((GtkWidget *) cell_pattern->drawing_area,
			      AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY * cell_pattern->cell_width + 1, AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY * cell_pattern->cell_height + 1);
  gtk_table_attach((GtkTable *) cell_pattern,
		   (GtkWidget *) cell_pattern->drawing_area,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_set_events((GtkWidget *) cell_pattern->drawing_area,
			GDK_EXPOSURE_MASK
			| GDK_LEAVE_NOTIFY_MASK
			| GDK_BUTTON_PRESS_MASK
			| GDK_POINTER_MOTION_MASK
			| GDK_POINTER_MOTION_HINT_MASK
			| GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);
  
  adjustment = (GtkAdjustment *) gtk_adjustment_new(0.0, 0.0, (double) AGS_CELL_PATTERN_DEFAULT_CONTROLS_VERTICALLY - 1.0, 1.0, 1.0, (gdouble) AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY);

  cell_pattern->vscrollbar = (GtkVScrollbar *) gtk_vscrollbar_new(adjustment);
  gtk_table_attach((GtkTable *) cell_pattern,
		   (GtkWidget *) cell_pattern->vscrollbar,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  cell_pattern->hscrollbar = NULL;
  
  /* led */
  cell_pattern->active_led = 0;

  cell_pattern->hled_array = ags_hled_array_new();
  g_object_set(cell_pattern->hled_array,
	       "led-width", cell_pattern->cell_width,
	       "led-height", AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT,
	       "led-count", cell_pattern->n_cols,
	       NULL);
  gtk_table_attach((GtkTable *) cell_pattern,
		   (GtkWidget *) cell_pattern->hled_array,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  gtk_widget_show_all((GtkWidget *) cell_pattern->hled_array);

  if(ags_cell_pattern_led_queue_draw == NULL){
    ags_cell_pattern_led_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							   NULL,
							   NULL);
  }

  g_hash_table_insert(ags_cell_pattern_led_queue_draw,
		      cell_pattern, ags_cell_pattern_led_queue_draw_timeout);
  g_timeout_add(1000 / 30, (GSourceFunc) ags_cell_pattern_led_queue_draw_timeout, (gpointer) cell_pattern);
}

void
ags_cell_pattern_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_cell_pattern_led_queue_draw,
		      gobject);

  G_OBJECT_CLASS(ags_cell_pattern_parent_class)->finalize(gobject);
}

void
ags_cell_pattern_connect(AgsConnectable *connectable)
{
  AgsCellPattern *cell_pattern;

  if((AGS_CELL_PATTERN_CONNECTED & (AGS_CELL_PATTERN(connectable)->flags)) != 0){
    return;
  }

  /* AgsCellPattern */
  cell_pattern = AGS_CELL_PATTERN(connectable);

  cell_pattern->flags |= AGS_CELL_PATTERN_CONNECTED;

  g_signal_connect_after(G_OBJECT(cell_pattern), "focus_in_event",
			 G_CALLBACK(ags_cell_pattern_focus_in_callback), (gpointer) cell_pattern);
  
  g_signal_connect(G_OBJECT(cell_pattern->drawing_area), "key_press_event",
		   G_CALLBACK(ags_cell_pattern_drawing_area_key_press_event), (gpointer) cell_pattern);

  g_signal_connect(G_OBJECT(cell_pattern->drawing_area), "key_release_event",
		   G_CALLBACK(ags_cell_pattern_drawing_area_key_release_event), (gpointer) cell_pattern);

  g_signal_connect_after(G_OBJECT(cell_pattern->drawing_area), "configure_event",
			 G_CALLBACK(ags_cell_pattern_drawing_area_configure_callback), (gpointer) cell_pattern);

  g_signal_connect_after(G_OBJECT(cell_pattern->drawing_area), "expose_event",
			 G_CALLBACK(ags_cell_pattern_drawing_area_expose_callback), (gpointer) cell_pattern);

  g_signal_connect(G_OBJECT(cell_pattern->drawing_area), "button_press_event",
		   G_CALLBACK(ags_cell_pattern_drawing_area_button_press_callback), (gpointer) cell_pattern);

  g_signal_connect(G_OBJECT(GTK_RANGE(cell_pattern->vscrollbar)->adjustment), "value_changed",
		   G_CALLBACK(ags_cell_pattern_adjustment_value_changed_callback), (gpointer) cell_pattern);
}

void
ags_cell_pattern_disconnect(AgsConnectable *connectable)
{
  AgsCellPattern *cell_pattern;

  if((AGS_CELL_PATTERN_CONNECTED & (AGS_CELL_PATTERN(connectable)->flags)) == 0){
    return;
  }

  /* AgsCellPattern */
  cell_pattern = AGS_CELL_PATTERN(connectable);

  cell_pattern->flags &= (~AGS_CELL_PATTERN_CONNECTED);

  g_object_disconnect(G_OBJECT(cell_pattern),
		      "any_signal::focus_in_event",
		      G_CALLBACK(ags_cell_pattern_focus_in_callback),
		      (gpointer) cell_pattern,
		      NULL);

  g_object_disconnect(G_OBJECT(cell_pattern->drawing_area),
		      "any_signal::key_press_event",
		      G_CALLBACK(ags_cell_pattern_drawing_area_key_press_event),
		      (gpointer) cell_pattern,
		      "any_signal::key_release_event",
		      G_CALLBACK(ags_cell_pattern_drawing_area_key_release_event),
		      (gpointer) cell_pattern,
		      "any_signal::configure_event",
		      G_CALLBACK(ags_cell_pattern_drawing_area_configure_callback),
		      (gpointer) cell_pattern,
		      "any_signal::expose_event",
		      G_CALLBACK(ags_cell_pattern_drawing_area_expose_callback),
		      (gpointer) cell_pattern,
		      "any_signal::button_press_event",
		      G_CALLBACK(ags_cell_pattern_drawing_area_button_press_callback),
		      (gpointer) cell_pattern,
		      NULL);

  g_object_disconnect(G_OBJECT(GTK_RANGE(cell_pattern->vscrollbar)->adjustment),
		      "any_signal::value_changed",
		      G_CALLBACK(ags_cell_pattern_adjustment_value_changed_callback),
		      (gpointer) cell_pattern,
		      NULL);
}

AtkObject*
ags_cell_pattern_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_cell_pattern_get_type(),
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
ags_cell_pattern_realize(GtkWidget *widget)
{
  AgsCellPattern *cell_pattern;

  cell_pattern = (AgsCellPattern *) widget;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_cell_pattern_parent_class)->realize(widget);

  if(cell_pattern_style == NULL){
    cell_pattern_style = gtk_style_copy(gtk_widget_get_style((GtkWidget *) cell_pattern));
  }

  gtk_widget_set_style((GtkWidget *) cell_pattern->drawing_area,
		       cell_pattern_style);

  gtk_widget_set_style((GtkWidget *) cell_pattern->vscrollbar,
		       cell_pattern_style);
}

void
ags_cell_pattern_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_cell_pattern_parent_class)->show(widget);

  //  ags_cell_pattern_draw_matrix(AGS_CELL_PATTERN(widget));
}

void
ags_cell_pattern_show_all(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_cell_pattern_parent_class)->show_all(widget);

  //  ags_cell_pattern_draw_matrix(AGS_CELL_PATTERN(widget));
}

gboolean
ags_accessible_cell_pattern_do_action(AtkAction *action,
				      gint i)
{
  AgsCellPattern *cell_pattern;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  
  if(!(i >= 0 && i < 6)){
    return(FALSE);
  }

  cell_pattern = (AgsCellPattern *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
  key_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_CELL_PATTERN_MOVE_LEFT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_CELL_PATTERN_MOVE_RIGHT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_CELL_PATTERN_MOVE_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_CELL_PATTERN_MOVE_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_CELL_PATTERN_TOGGLE_PAD:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_CELL_PATTERN_COPY_PATTERN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* create modifier */
      modifier_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
      modifier_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);

      modifier_press->keyval =
	modifier_release->keyval = GDK_KEY_Control_R;

      /* send event */
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) cell_pattern,
		       (GdkEvent *) modifier_release);      
    }    
    break;
  }

  return(TRUE);
}

gint
ags_accessible_cell_pattern_get_n_actions(AtkAction *action)
{
  return(6);
}

const gchar*
ags_accessible_cell_pattern_get_description(AtkAction *action,
					    gint i)
{
  static const gchar *actions[] = {
    "move cursor left",
    "move cursor right",
    "move cursor up",
    "move cursor down",
    "toggle audio pattern"
    "copy pattern to clipboard",
  };

  if(i >= 0 && i < 6){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_cell_pattern_get_name(AtkAction *action,
				     gint i)
{
  static const gchar *actions[] = {
    "left",
    "right",
    "up",
    "down",
    "toggle",
    "copy",
  };
  
  if(i >= 0 && i < 6){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

const gchar*
ags_accessible_cell_pattern_get_keybinding(AtkAction *action,
					   gint i)
{
  static const gchar *actions[] = {
    "left",
    "right",
    "up",
    "down",
    "space",
    "Ctrl+c",
  };
  
  if(i >= 0 && i < 6){
    return(actions[i]);
  }else{
    return(NULL);
  }
}

gboolean
ags_accessible_cell_pattern_set_description(AtkAction *action,
					    gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_cell_pattern_get_localized_name(AtkAction *action,
					       gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

void
ags_cell_pattern_paint(AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_draw_gutter(cell_pattern);
  ags_cell_pattern_draw_matrix(cell_pattern);

  ags_cell_pattern_draw_cursor(cell_pattern);
}

void
ags_cell_pattern_draw_gutter(AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;
  
  AgsChannel *start_channel;
  AgsChannel *channel, *prev_pad;

  guint input_pads;
  guint gutter;
  guint current_gutter;
  int i, j;

  pthread_mutex_t *audio_mutex;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = machine->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* retrieve some audio fields */
  pthread_mutex_lock(audio_mutex);
  
  input_pads = machine->audio->input_pads;

  start_channel = machine->audio->input;

  if(start_channel != NULL){
    g_object_ref(start_channel);
  }
  
  pthread_mutex_unlock(audio_mutex);

  if(input_pads == 0){
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    return;
  }
  
  if(input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    gutter = AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY;
  }else{
    gutter = input_pads;
  }

  current_gutter = (guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value;
  
  /* clear bg */
  gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
		     GTK_WIDGET(cell_pattern->drawing_area)->style->bg_gc[0],
		     TRUE,
		     0, 0,
		     AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH * AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY, gutter * AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT);

  if(input_pads - ((guint) current_gutter + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY) > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    channel = ags_channel_nth(start_channel,
			      input_pads - (current_gutter + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY));
  }else if(input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    channel = ags_channel_nth(start_channel,
			      AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY);
  }else{
    channel = ags_channel_nth(start_channel,
			      input_pads - 1);
  }
  
  if(channel == NULL){
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }
    
    return;
  }

  prev_pad = NULL;
  
  for (i = 0; channel != NULL && i < gutter; i++){
    for (j = 0; j < 32; j++){
      gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
			 GTK_WIDGET(cell_pattern->drawing_area)->style->fg_gc[0],
			 FALSE,
			 j * AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH, i * AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT,
			 AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH, AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT);

      ags_cell_pattern_redraw_gutter_point(cell_pattern, channel, j, i);
    }

    /* iterate */
    prev_pad = ags_channel_prev_pad(channel);

    g_object_unref(channel);

    channel = prev_pad;
  }

  if(prev_pad != NULL){
    g_object_unref(prev_pad);
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_cell_pattern_draw_matrix(AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;

  AgsChannel *start_channel;
  AgsChannel *channel, *prev_pad;

  guint input_pads;
  guint gutter;
  guint current_gutter;
  int i, j;

  pthread_mutex_t *audio_mutex;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  /* get audio mutex */
  pthread_mutex_lock(ags_audio_get_class_mutex());
  
  audio_mutex = machine->audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get some audio fields */
  pthread_mutex_lock(audio_mutex);

  input_pads = machine->audio->input_pads;

  start_channel = machine->audio->input;

  if(start_channel != NULL){
    g_object_ref(start_channel);
  }
    
  pthread_mutex_unlock(audio_mutex);

  if(input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    gutter = AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY;
  }else{
    gutter = input_pads;
  }

  current_gutter = (guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value;

  channel = ags_channel_nth(start_channel,
			    input_pads - current_gutter - 1);

  if(channel == NULL){
    if(start_channel != NULL){
      g_object_unref(start_channel);
    }

    return;
  }

  prev_pad = NULL;

  for (i = 0; channel != NULL && i < gutter; i++){
    for(j = 0; j < AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_HORIZONTALLY; j++){
      ags_cell_pattern_redraw_gutter_point(cell_pattern, channel, j, i);
    }
    
    /* iterate */
    prev_pad = ags_channel_prev_pad(channel);

    g_object_unref(channel);

    channel = prev_pad;
  }

  if(prev_pad != NULL){
    g_object_unref(prev_pad);
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_cell_pattern_draw_cursor(AgsCellPattern *cell_pattern)
{
  guint i, j;

  if(cell_pattern->cursor_y >= GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value &&
     cell_pattern->cursor_y < GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + cell_pattern->n_rows){
    i = cell_pattern->cursor_y - GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value;
    j = cell_pattern->cursor_x;
    
    if((AGS_CELL_PATTERN_CURSOR_ON & (cell_pattern->flags)) != 0){
      ags_cell_pattern_highlight_gutter_point(cell_pattern, j, i);
    }else{
      ags_cell_pattern_unpaint_gutter_point(cell_pattern, j, i);
    }
  }
}

void
ags_cell_pattern_redraw_gutter_point(AgsCellPattern *cell_pattern, AgsChannel *channel, guint j, guint i)
{
  AgsMachine *machine;

  AgsPattern *pattern;
  
  gboolean do_highlight;
  
  pthread_mutex_t *channel_mutex;

  if(channel == NULL ||
     channel->pattern == NULL){
    return;
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  /* get channel mutex */
  pthread_mutex_lock(ags_channel_get_class_mutex());
  
  channel_mutex = channel->obj_mutex;
  
  pthread_mutex_unlock(ags_channel_get_class_mutex());

  /* redraw */
  pthread_mutex_lock(channel_mutex);

  pattern = channel->pattern->data;
  
  pthread_mutex_unlock(channel_mutex);

  do_highlight = ags_pattern_get_bit(pattern,
				     machine->bank_0,
				     machine->bank_1,
				     j);
  
  if(do_highlight){
    ags_cell_pattern_highlight_gutter_point(cell_pattern, j, i);
  }else{
    ags_cell_pattern_unpaint_gutter_point(cell_pattern, j, i);
  }
}

void
ags_cell_pattern_highlight_gutter_point(AgsCellPattern *cell_pattern, guint j, guint i)
{
  gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
		     GTK_WIDGET(cell_pattern->drawing_area)->style->fg_gc[0],
		     TRUE,
		     j * cell_pattern->cell_width + 1, i * cell_pattern->cell_height + 1,
		     AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH - 1, AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT - 1);
}

void
ags_cell_pattern_unpaint_gutter_point(AgsCellPattern *cell_pattern, guint j, guint i)
{
  gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
		     GTK_WIDGET(cell_pattern->drawing_area)->style->bg_gc[0],
		     TRUE,
		     j * cell_pattern->cell_width + 1, i * cell_pattern->cell_height +1,
		     AGS_CELL_PATTERN_DEFAULT_CELL_WIDTH - 1, AGS_CELL_PATTERN_DEFAULT_CELL_HEIGHT - 1);
}

/**
 * ags_cell_pattern_led_queue_draw_timeout:
 * @cell_pattern: the #AgsCellPattern
 *
 * Queue draw led.
 *
 * Returns: %TRUE if continue timeout, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_cell_pattern_led_queue_draw_timeout(AgsCellPattern *cell_pattern)
{
  if(g_hash_table_lookup(ags_cell_pattern_led_queue_draw,
			 cell_pattern) != NULL){
    AgsMachine *machine;

    AgsAudio *audio;
    AgsRecallID *recall_id;
    AgsRecyclingContext *recycling_context;
    
    AgsCountBeatsAudio *play_count_beats_audio;
    AgsCountBeatsAudioRun *play_count_beats_audio_run;

    GList *list_start, *list;
    GList *start_recall, *recall;
    
    guint offset;
    guint64 active_led_new;
    
    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						     AGS_TYPE_MACHINE);

    if(machine == NULL){      
      return(TRUE);
    }

    audio = machine->audio;
  
    /* get some recalls */
    recall_id = NULL;
    g_object_get(audio,
		 "recall-id", &list_start,
		 NULL);

    list = list_start;
    
    while(list != NULL){
      AgsRecyclingContext *current;

      g_object_get(list->data,
		   "recycling-context", &current,
		   NULL);

      if(current != NULL){
	g_object_unref(current);
	
	g_object_get(current,
		     "parent", &current,
		     NULL);

	if(current != NULL){
	  g_object_unref(current);
	}
	
	if(current == NULL &&
	   ags_recall_id_check_sound_scope(list->data, AGS_SOUND_SCOPE_SEQUENCER)){
	  recall_id = list->data;

	  g_object_get(audio,
		       "play", &start_recall,
		       NULL);

	  play_count_beats_audio = NULL;
	  play_count_beats_audio_run = NULL;
    
	  recall = ags_recall_find_type(start_recall,
					AGS_TYPE_COUNT_BEATS_AUDIO);
    
	  if(recall != NULL){
	    play_count_beats_audio = AGS_COUNT_BEATS_AUDIO(recall->data);
	  }
    
	  recall = ags_recall_find_type_with_recycling_context(start_recall,
							       AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
							       (GObject *) recall_id->recycling_context);
    
	  if(recall != NULL){
	    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall->data);
	  }

	  g_list_free_full(start_recall,
			   g_object_unref);

	  if(play_count_beats_audio == NULL ||
	     play_count_beats_audio_run == NULL){
	    recall_id = NULL;
	  }else{
	    break;
	  }
	}
      }
      
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
    
    if(recall_id == NULL){
      return(TRUE);
    }

    /* active led */
    g_object_get(play_count_beats_audio_run,
		 "sequencer-counter", &active_led_new,
		 NULL);

    cell_pattern->active_led = (guint) (active_led_new % cell_pattern->n_cols);
    ags_led_array_unset_all((AgsLedArray *) cell_pattern->hled_array);
    ags_led_array_set_nth((AgsLedArray *) cell_pattern->hled_array,
			  cell_pattern->active_led);
    
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_cell_pattern_new:
 *
 * Creates an #AgsCellPattern
 *
 * Returns: a new #AgsCellPattern
 *
 * Since: 0.5
 */
AgsCellPattern*
ags_cell_pattern_new()
{
  AgsCellPattern *cell_pattern;

  cell_pattern = (AgsCellPattern *) g_object_new(AGS_TYPE_CELL_PATTERN,
						 NULL);

  return(cell_pattern);
}
