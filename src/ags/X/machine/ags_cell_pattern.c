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

#include <<ags/object/ags_application_context.h>>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_pattern.h>

#include <ags/audio/task/ags_blink_cell_pattern_cursor.h>

#include <ags/widget/ags_led.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

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
 * @short_description: cell_pattern sequencer
 * @title: AgsCellPattern
 * @section_id:
 * @include: ags/X/machine/ags_cell_pattern.h
 *
 * The #AgsCellPattern is a composite widget to act as cell_pattern sequencer.
 */

static gpointer ags_cell_pattern_parent_class = NULL;
static GQuark quark_accessible_object = 0;

extern pthread_mutex_t ags_application_mutex;

GtkStyle *cell_pattern_style;

GType
ags_cell_pattern_get_type(void)
{
  static GType ags_type_cell_pattern = 0;

  if(!ags_type_cell_pattern){
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
						   "AgsCellPattern\0", &ags_cell_pattern_info,
						   0);
    
    g_type_add_interface_static(ags_type_cell_pattern,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_cell_pattern);
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
							      "AgsAccessibleCellPattern\0", &ags_accesssible_cell_pattern_info,
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

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object\0");

  /* GObjectClass */
  gobject = (GObjectClass *) cell_pattern;

  gobject->finalize = ags_cell_pattern_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) cell_pattern;

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
	       "can-focus\0", TRUE,
	       "n-columns\0", 2,
	       "n-rows", 2,
	       "homogeneous\0", FALSE,
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
  gtk_widget_set_style((GtkWidget *) cell_pattern->drawing_area,
		       cell_pattern_style);
  gtk_table_attach(cell_pattern,
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
  gtk_widget_set_style((GtkWidget *) cell_pattern->vscrollbar,
		       cell_pattern_style);
  gtk_table_attach(cell_pattern,
		   (GtkWidget *) cell_pattern->vscrollbar,
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  cell_pattern->hscrollbar = NULL;
  
  /* led */
  cell_pattern->active_led = 0;

  cell_pattern->led = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(cell_pattern,
		   (GtkWidget *) cell_pattern->led,
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  for(i = 0; i < 32; i++){
    led = ags_led_new();
    gtk_widget_set_size_request((GtkWidget *) led,
				cell_pattern->cell_width, cell_pattern->cell_height);
    gtk_box_pack_start((GtkBox *) cell_pattern->led,
		       (GtkWidget *) led,
		       FALSE, FALSE,
		       0);
  }
}

void
ags_cell_pattern_finalize(GObject *gobject)
{
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

  g_signal_connect_after(G_OBJECT(cell_pattern), "focus_in_event\0",
			 G_CALLBACK(ags_cell_pattern_focus_in_callback), (gpointer) cell_pattern);
  
  g_signal_connect(G_OBJECT(cell_pattern), "key_press_event\0",
		   G_CALLBACK(ags_cell_pattern_drawing_area_key_press_event), (gpointer) cell_pattern);

  g_signal_connect(G_OBJECT(cell_pattern), "key_release_event\0",
		   G_CALLBACK(ags_cell_pattern_drawing_area_key_release_event), (gpointer) cell_pattern);

  g_signal_connect_after(G_OBJECT(cell_pattern->drawing_area), "configure_event\0",
			 G_CALLBACK(ags_cell_pattern_drawing_area_configure_callback), (gpointer) cell_pattern);

  g_signal_connect_after(G_OBJECT(cell_pattern->drawing_area), "expose_event\0",
			 G_CALLBACK(ags_cell_pattern_drawing_area_expose_callback), (gpointer) cell_pattern);

  g_signal_connect(G_OBJECT(cell_pattern->drawing_area), "button_press_event\0",
		   G_CALLBACK(ags_cell_pattern_drawing_area_button_press_callback), (gpointer) cell_pattern);

  g_signal_connect(G_OBJECT(GTK_RANGE(cell_pattern->vscrollbar)->adjustment), "value_changed\0",
		   G_CALLBACK(ags_cell_pattern_adjustment_value_changed_callback), (gpointer) cell_pattern);
}

void
ags_cell_pattern_disconnect(AgsConnectable *connectable)
{
  AgsCellPattern *cell_pattern;

  /* AgsCellPattern */
  cell_pattern = AGS_CELL_PATTERN(connectable);

  //TODO:JK: implement me
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
    gtk_accessible_set_widget(accessible,
			      widget);
  }
  
  return(accessible);
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

  cell_pattern = gtk_accessible_get_widget(ATK_OBJECT(action));
  
  key_press = gdk_event_new(GDK_KEY_PRESS);
  key_release = gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_CELL_PATTERN_MOVE_LEFT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event(cell_pattern, key_press);
      gtk_widget_event(cell_pattern, key_release);
    }
    break;
  case AGS_CELL_PATTERN_MOVE_RIGHT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event(cell_pattern, key_press);
      gtk_widget_event(cell_pattern, key_release);
    }
    break;
  case AGS_CELL_PATTERN_MOVE_UP:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event(cell_pattern, key_press);
      gtk_widget_event(cell_pattern, key_release);
    }
    break;
  case AGS_CELL_PATTERN_MOVE_DOWN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event(cell_pattern, key_press);
      gtk_widget_event(cell_pattern, key_release);
    }
    break;
  case AGS_CELL_PATTERN_TOGGLE_PAD:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event(cell_pattern, key_press);
      gtk_widget_event(cell_pattern, key_release);
    }
    break;
  case AGS_CELL_PATTERN_COPY_PATTERN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* create modifier */
      modifier_press = gdk_event_new(GDK_KEY_PRESS);
      modifier_release = gdk_event_new(GDK_KEY_RELEASE);

      modifier_press->keyval =
	modifier_release->keyval = GDK_KEY_Control_R;

      /* send event */
      gtk_widget_event(cell_pattern, modifier_press);
      gtk_widget_event(cell_pattern, key_press);
      gtk_widget_event(cell_pattern, key_release);
      gtk_widget_event(cell_pattern, modifier_release);      
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
  static const gchar **actions = {
    "move cursor left\0",
    "move cursor right\0",
    "move cursor up\0",
    "move cursor down\0",
    "toggle audio pattern\0"
    "copy pattern to clipboard\0",
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
  static const gchar **actions = {
    "left\0",
    "right\0",
    "up\0",
    "down\0",
    "toggle\0",
    "copy\0",
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
  static const gchar **actions = {
    "left\0",
    "right\0",
    "up\0",
    "down\0",
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
  
  AgsChannel *channel;

  AgsMutexManager *mutex_manager;

  guint gutter;
  int i, j;

  pthread_mutex_t *audio_mutex;

  machine = gtk_widget_get_ancestor(cell_pattern,
				    AGS_TYPE_MACHINE);
  
  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  pthread_mutex_lock(audio_mutex);

  gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
		     GTK_WIDGET(cell_pattern->drawing_area)->style->bg_gc[0],
		     TRUE,
		     0, 0,
		     288, 80);

  channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - ((guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY) - 1);

  if(machine->audio->input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    gutter = AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY;
  }else{
    gutter = machine->audio->input_pads;
  }

  for (i = 0; i < gutter; i++){
    for (j = 0; j < 32; j++){
      gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
			 GTK_WIDGET(cell_pattern->drawing_area)->style->fg_gc[0],
			 FALSE,
			 j * 12, i * 10,
			 12, 10);

      ags_cell_pattern_redraw_gutter_point (cell_pattern, channel, j, i);
    }

    channel = channel->prev;
  }

  pthread_mutex_unlock(audio_mutex);
}

void
ags_cell_pattern_draw_matrix(AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;

  AgsChannel *channel;

  AgsMutexManager *mutex_manager;

  guint gutter;
  int i, j;

  pthread_mutex_t *audio_mutex;

  machine = gtk_widget_get_ancestor(cell_pattern,
				    AGS_TYPE_MACHINE);

  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) machine->audio);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  pthread_mutex_lock(audio_mutex);

  if(machine->audio->input_pads > AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
    gutter = AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY;
  }else{
    gutter = machine->audio->input_pads;
  }

  channel = ags_channel_nth(machine->audio->input, machine->audio->input_pads - (guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value - 1);

  if(channel == NULL){
    pthread_mutex_unlock(audio_mutex);
    
    return;
  }

  for (i = 0; i < gutter; i++){
    for (j = 0; j < 32; j++)
      ags_cell_pattern_redraw_gutter_point(cell_pattern, channel, j, i);

    channel = channel->prev;
  }

  pthread_mutex_unlock(audio_mutex);
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

  if(channel->pattern == NULL)
    return;

  machine = gtk_widget_get_ancestor(cell_pattern,
				    AGS_TYPE_MACHINE);

  if(ags_pattern_get_bit((AgsPattern *) channel->pattern->data, machine->bank_0, machine->bank_1, j)){
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
		     11, 9);
}

void
ags_cell_pattern_unpaint_gutter_point(AgsCellPattern *cell_pattern, guint j, guint i)
{
  gdk_draw_rectangle(GTK_WIDGET(cell_pattern->drawing_area)->window,
		     GTK_WIDGET(cell_pattern->drawing_area)->style->bg_gc[0],
		     TRUE,
		     j * cell_pattern->cell_width + 1, i * cell_pattern->cell_height +1,
		     11, 9);
}

void*
ags_cell_pattern_blink_worker(void *data)
{
  AgsWindow *window;
  AgsCellPattern *cell_pattern;

  AgsBlinkCellPatternCursor *blink_cell_pattern_cursor;

  AgsThread *audio_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;
  
  static const guint blink_delay = 1000000; // blink every second
  
  cell_pattern = AGS_CELL_PATTERN(data);
  window = gtk_widget_get_ancestor(cell_pattern,
				   AGS_TYPE_WINDOW);

  application_context = window->application_context;
  
  /* get audio loop */
  pthread_mutex_lock(&(ags_application_mutex));

  audio_loop = application_context->main_loop;

  pthread_mutex_unlock(&(ags_application_mutex));
  
  /* find task thread */
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

  while(gtk_widget_has_focus(cell_pattern)){
    /* blink cursor */
    blink_cell_pattern_cursor = ags_blink_cell_pattern_cursor_new(cell_pattern,
								  !(AGS_CELL_PATTERN_CURSOR_ON & (cell_pattern->flags)));
    ags_task_thread_append_task(task_thread,
				blink_cell_pattern_cursor);

    /* delay */
    usleep(blink_delay);
  }

  /* unset cursor */
  blink_cell_pattern_cursor = ags_blink_cell_pattern_cursor_new(cell_pattern,
								FALSE);
  ags_task_thread_append_task(task_thread,
			      blink_cell_pattern_cursor);

  return(NULL);
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
