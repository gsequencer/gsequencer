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

#include <ags/X/machine/ags_pattern_box.h>
#include <ags/X/machine/ags_pattern_box_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_pattern.h>

#include <ags/widget/ags_led.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <gdk/gdkkeysyms.h>

#include <atk/atk.h>

static GType ags_accessible_pattern_box_get_type(void);
void ags_pattern_box_class_init(AgsPatternBoxClass *pattern_box);
void ags_accessible_pattern_box_class_init(AtkObject *object);
void ags_accessible_pattern_box_action_interface_init(AtkActionIface *action);
void ags_pattern_box_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pattern_box_init(AgsPatternBox *pattern_box);
void ags_pattern_box_finalize(GObject *gobject);
void ags_pattern_box_connect(AgsConnectable *connectable);
void ags_pattern_box_disconnect(AgsConnectable *connectable);
AtkObject* ags_pattern_box_get_accessible(GtkWidget *widget);
void ags_pattern_box_show(GtkWidget *widget);
void ags_pattern_box_show_all(GtkWidget *widget);

gboolean ags_accessible_pattern_box_do_action(AtkAction *action,
					      gint i);
gint ags_accessible_pattern_box_get_n_actions(AtkAction *action);
const gchar* ags_accessible_pattern_box_get_description(AtkAction *action,
							gint i);
const gchar* ags_accessible_pattern_box_get_name(AtkAction *action,
						 gint i);
const gchar* ags_accessible_pattern_box_get_keybinding(AtkAction *action,
						       gint i);
gboolean ags_accessible_pattern_box_set_description(AtkAction *action,
						    gint i);
gchar* ags_accessible_pattern_box_get_localized_name(AtkAction *action,
						     gint i);

/**
 * SECTION:ags_pattern_box
 * @short_description: pattern_box sequencer
 * @title: AgsPatternBox
 * @section_id:
 * @include: ags/X/machine/ags_pattern_box.h
 *
 * The #AgsPatternBox is a composite widget to act as pattern box sequencer.
 */

static gpointer ags_pattern_box_parent_class = NULL;
static GQuark quark_accessible_object = 0;

extern pthread_mutex_t ags_application_mutex;

GtkStyle *pattern_box_style;

GType
ags_pattern_box_get_type(void)
{
  static GType ags_type_pattern_box = 0;

  if(!ags_type_pattern_box){
    static const GTypeInfo ags_pattern_box_info = {
      sizeof(AgsPatternBoxClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pattern_box_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPatternBox),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pattern_box_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pattern_box_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pattern_box = g_type_register_static(GTK_TYPE_TABLE,
						  "AgsPatternBox\0", &ags_pattern_box_info,
						  0);
    
    g_type_add_interface_static(ags_type_pattern_box,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_pattern_box);
}

static GType
ags_accessible_pattern_box_get_type(void)
{
  static GType ags_type_accessible_pattern_box = 0;

  if(!ags_type_accessible_pattern_box){
    const GTypeInfo ags_accesssible_pattern_box_info = {
      sizeof(GtkAccessibleClass),
      NULL,           /* base_init */
      NULL,           /* base_finalize */
      (GClassInitFunc) ags_accessible_pattern_box_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof(GtkAccessible),
      0,             /* n_preallocs */
      NULL, NULL
    };

    static const GInterfaceInfo atk_action_interface_info = {
      (GInterfaceInitFunc) ags_accessible_pattern_box_action_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_accessible_pattern_box = g_type_register_static(GTK_TYPE_ACCESSIBLE,
							     "AgsAccessiblePatternBox\0", &ags_accesssible_pattern_box_info,
							     0);

    g_type_add_interface_static(ags_type_accessible_pattern_box,
				ATK_TYPE_ACTION,
				&atk_action_interface_info);
  }
  
  return(ags_type_accessible_pattern_box);
}

void
ags_pattern_box_class_init(AgsPatternBoxClass *pattern_box)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_pattern_box_parent_class = g_type_class_peek_parent(pattern_box);

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object\0");

  /* GObjectClass */
  gobject = (GObjectClass *) pattern_box;

  gobject->finalize = ags_pattern_box_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) pattern_box;

  widget->show = ags_pattern_box_show;
  widget->show_all = ags_pattern_box_show_all;
}

void
ags_accessible_pattern_box_class_init(AtkObject *object)
{
  /* empty */
}

void
ags_accessible_pattern_box_action_interface_init(AtkActionIface *action)
{
  action->do_action = ags_accessible_pattern_box_do_action;
  action->get_n_actions = ags_accessible_pattern_box_get_n_actions;
  action->get_description = ags_accessible_pattern_box_get_description;
  action->get_name = ags_accessible_pattern_box_get_name;
  action->get_keybinding = ags_accessible_pattern_box_get_keybinding;
  action->set_description = ags_accessible_pattern_box_set_description;
  action->get_localized_name = ags_accessible_pattern_box_get_localized_name;
}

void
ags_pattern_box_connectable_interface_init(AgsConnectableInterface *connectable)
{
  AgsConnectableInterface *ags_pattern_box_connectable_parent_interface;

  connectable->connect = ags_pattern_box_connect;
  connectable->disconnect = ags_pattern_box_disconnect;
}

void
ags_pattern_box_init(AgsPatternBox *pattern_box)
{
  AgsLed *led;
  GtkToggleButton *toggle_button;
  GtkRadioButton *radio_button;
  
  guint i;

  g_object_set(pattern_box,
	       "can-focus\0", TRUE,
	       "n-columns\0", 2,
	       "n-rows", 2,
	       "homogeneous\0", FALSE,
	       NULL);
  gtk_widget_set_events((GtkWidget *) pattern_box,
			GDK_CONTROL_MASK
			| GDK_KEY_PRESS_MASK
			| GDK_KEY_RELEASE_MASK);

  pattern_box->flags = 0;

  pattern_box->key_mask = 0;

  pattern_box->n_controls = AGS_PATTERN_BOX_N_CONTROLS;
  pattern_box->n_indices = AGS_PATTERN_BOX_N_INDICES;
  
  /* led */
  pattern_box->active_led = 0;

  pattern_box->led = (GtkHBox *) gtk_hbox_new(FALSE, 16);
  gtk_table_attach(pattern_box,
		   (GtkWidget *) pattern_box->led,
		   0, 1,
		   0, 1,
		   0, 0,
		   0, 0);

  for(i = 0; i < pattern_box->n_controls; i++){
    led = (GtkToggleButton *) ags_led_new();
    gtk_widget_set_size_request((GtkWidget *) led,
				AGS_PATTERN_BOX_LED_DEFAULT_WIDTH, AGS_PATTERN_BOX_LED_DEFAULT_HEIGHT);
    gtk_box_pack_start((GtkBox *) pattern_box->led,
		       (GtkWidget *) led,
		       FALSE, FALSE,
		       0);
  }

  /* pattern */
  pattern_box->pattern = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach(pattern_box,
		   (GtkWidget *) pattern_box->pattern,
		   0, 1,
		   1, 2,
		   0, 0,
		   0, 0);

  for(i = 0; i < pattern_box->n_controls; i++){
    toggle_button = (GtkToggleButton *) gtk_toggle_button_new();
    gtk_widget_set_size_request((GtkWidget *) toggle_button,
				AGS_PATTERN_BOX_DEFAULT_PAD_WIDTH, AGS_PATTERN_BOX_DEFAULT_PAD_HEIGHT);
    gtk_box_pack_start((GtkBox *) pattern_box->pattern,
		       (GtkWidget *) toggle_button,
		       FALSE, FALSE,
		       0);
  }

  /* page / offset */
  pattern_box->offset = (GtkVBox*) gtk_vbox_new(FALSE, 0);
  gtk_table_attach_defaults(pattern_box,
			    (GtkWidget *) pattern_box->offset,
			    1, 2,
			    0, 2);
  radio_button = NULL;

  for(i = 0; i < pattern_box->n_indices; i++){
    if(radio_button == NULL){
      radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label(NULL, g_strdup_printf("%d-%d\0",
											      i * pattern_box->n_controls + 1, (i + 1) * pattern_box->n_controls));
      gtk_box_pack_start((GtkBox*) pattern_box->offset,
			 (GtkWidget *) radio_button,
			 FALSE, FALSE,
			 0);
    }else{
      gtk_box_pack_start((GtkBox*) pattern_box->offset,
			 (GtkWidget *) gtk_radio_button_new_with_label(radio_button->group, g_strdup_printf("%d-%d\0",
													    i * pattern_box->n_controls + 1, (i + 1) * pattern_box->n_controls)),
			 FALSE, FALSE,
			 0);
    }
  }
}

void
ags_pattern_box_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_pattern_box_parent_class)->finalize(gobject);
}

void
ags_pattern_box_connect(AgsConnectable *connectable)
{
  AgsPatternBox *pattern_box;

  GList *list, *list_start;

  if((AGS_PATTERN_BOX_CONNECTED & (AGS_PATTERN_BOX(connectable)->flags)) != 0){
    return;
  }

  /* AgsPatternBox */
  pattern_box = AGS_PATTERN_BOX(connectable);

  pattern_box->flags |= AGS_PATTERN_BOX_CONNECTED;

  g_signal_connect_after(G_OBJECT(pattern_box), "focus_in_event\0",
			 G_CALLBACK(ags_pattern_box_focus_in_callback), (gpointer) pattern_box);
  
  g_signal_connect(G_OBJECT(pattern_box), "key_press_event\0",
		   G_CALLBACK(ags_pattern_box_key_press_event), (gpointer) pattern_box);

  g_signal_connect(G_OBJECT(pattern_box), "key_release_event\0",
		   G_CALLBACK(ags_pattern_box_key_release_event), (gpointer) pattern_box);

  /* connect pattern */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "clicked\0",
		     G_CALLBACK(ags_pattern_box_pad_callback), (gpointer) pattern_box);

    list = list->next;
  }

  g_list_free(list_start);

  /* connect pattern offset range */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "clicked\0",
		     G_CALLBACK(ags_pattern_box_offset_callback), (gpointer) pattern_box);
		   
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_pattern_box_disconnect(AgsConnectable *connectable)
{
  AgsPatternBox *pattern_box;

  /* AgsPatternBox */
  pattern_box = AGS_PATTERN_BOX(connectable);

  //TODO:JK: implement me
}

AtkObject*
ags_pattern_box_get_accessible(GtkWidget *widget)
{
  AtkObject* accessible;

  accessible = g_object_get_qdata(G_OBJECT(widget),
				  quark_accessible_object);
  
  if(!accessible){
    accessible = g_object_new(ags_accessible_pattern_box_get_type(),
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
ags_pattern_box_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_pattern_box_parent_class)->show(widget);

  //  ags_pattern_box_draw_matrix(AGS_PATTERN_BOX(widget));
}

void
ags_pattern_box_show_all(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_pattern_box_parent_class)->show_all(widget);

  //  ags_pattern_box_draw_matrix(AGS_PATTERN_BOX(widget));
}

gboolean
ags_accessible_pattern_box_do_action(AtkAction *action,
				     gint i)
{
  AgsPatternBox *pattern_box;
  
  GdkEventKey *key_press, *key_release;
  GdkEventKey *modifier_press, *modifier_release;
  
  if(!(i >= 0 && i < 6)){
    return(FALSE);
  }

  pattern_box = gtk_accessible_get_widget(ATK_OBJECT(action));
  
  key_press = gdk_event_new(GDK_KEY_PRESS);
  key_release = gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_PATTERN_BOX_MOVE_LEFT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event(pattern_box, key_press);
      gtk_widget_event(pattern_box, key_release);
    }
    break;
  case AGS_PATTERN_BOX_MOVE_RIGHT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event(pattern_box, key_press);
      gtk_widget_event(pattern_box, key_release);
    }
    break;
  case AGS_PATTERN_BOX_INDEX_DECREMENT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event(pattern_box, key_press);
      gtk_widget_event(pattern_box, key_release);
    }
    break;
  case AGS_PATTERN_BOX_INDEX_INCREMENT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event(pattern_box, key_press);
      gtk_widget_event(pattern_box, key_release);
    }
    break;
  case AGS_PATTERN_BOX_TOGGLE_PAD:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event(pattern_box, key_press);
      gtk_widget_event(pattern_box, key_release);
    }
    break;
  case AGS_PATTERN_BOX_COPY_PATTERN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* create modifier */
      modifier_press = gdk_event_new(GDK_KEY_PRESS);
      modifier_release = gdk_event_new(GDK_KEY_RELEASE);

      modifier_press->keyval =
	modifier_release->keyval = GDK_KEY_Control_R;

      /* send event */
      gtk_widget_event(pattern_box, modifier_press);
      gtk_widget_event(pattern_box, key_press);
      gtk_widget_event(pattern_box, key_release);
      gtk_widget_event(pattern_box, modifier_release);      
    }    
    break;
  }

  return(TRUE);
}

gint
ags_accessible_pattern_box_get_n_actions(AtkAction *action)
{
  return(6);
}

const gchar*
ags_accessible_pattern_box_get_description(AtkAction *action,
					   gint i)
{
  static const gchar **actions = {
    "move cursor left\0",
    "move cursor right\0",
    "decrement pattern index\0",
    "increment pattern index\0",
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
ags_accessible_pattern_box_get_name(AtkAction *action,
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
ags_accessible_pattern_box_get_keybinding(AtkAction *action,
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
ags_accessible_pattern_box_set_description(AtkAction *action,
					   gint i)
{
  //TODO:JK: implement me

  return(FALSE);
}

gchar*
ags_accessible_pattern_box_get_localized_name(AtkAction *action,
					      gint i)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_pattern_box_set_pattern:
 * @pattern_box: the #AgsPatternBox
 *
 * Resets the pattern on @pattern_box.
 *
 * since: 0.5.0
 */
void
ags_pattern_box_set_pattern(AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  AgsLine *selected_line;
  GList *list, *list_start;
  GList *line, *line_start;
  guint index0, index1, offset;
  gboolean set_active;
  guint i;

  machine = gtk_widget_get_ancestor(pattern_box,
				    AGS_TYPE_MACHINE);

  if(machine->selected_input_pad == NULL){
    return;
  }
    
  index0 = machine->bank_0;
  index1 = machine->bank_1;

  /* read boundaries */
  list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  for(i = 0; i < pattern_box->n_indices && ! GTK_TOGGLE_BUTTON(list->data)->active; i++)
    list = list->next;

  offset = i * pattern_box->n_controls;

  /* get pads */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  /* reset */
  pattern_box->flags |= AGS_PATTERN_BOX_BLOCK_PATTERN;

  for(i = 0; i < pattern_box->n_controls; i++){
    set_active = TRUE;

    line_start = 
      line = gtk_container_get_children(GTK_CONTAINER(AGS_PAD(machine->selected_input_pad)->expander_set));

    while((line = ags_line_find_next_grouped(line)) != NULL){
      selected_line = AGS_LINE(line->data);

      if(!ags_pattern_get_bit((AgsPattern *) selected_line->channel->pattern->data, index0, index1, offset + i)){
	set_active = FALSE;
	break;
      }

      line = line->next;
    }

    g_list_free(line_start);
    
    gtk_toggle_button_set_active((GtkToggleButton *) list->data, set_active);
    
    list = list->next;
  }

  pattern_box->flags &= (~AGS_PATTERN_BOX_BLOCK_PATTERN);

  g_list_free(list_start);
}

/**
 * ags_pattern_box_new:
 *
 * Creates an #AgsPatternBox
 *
 * Returns: a new #AgsPatternBox
 *
 * Since: 0.5
 */
AgsPatternBox*
ags_pattern_box_new()
{
  AgsPatternBox *pattern_box;

  pattern_box = (AgsPatternBox *) g_object_new(AGS_TYPE_PATTERN_BOX,
					       NULL);

  return(pattern_box);
}
