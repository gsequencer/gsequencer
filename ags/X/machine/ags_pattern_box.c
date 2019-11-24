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

#include <ags/X/machine/ags_pattern_box.h>
#include <ags/X/machine/ags_pattern_box_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>

#include <gdk/gdkkeysyms.h>

#include <atk/atk.h>

#include <ags/i18n.h>

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
void ags_pattern_box_realize(GtkWidget *widget);
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
 * @short_description: pattern box sequencer
 * @title: AgsPatternBox
 * @section_id:
 * @include: ags/X/machine/ags_pattern_box.h
 *
 * The #AgsPatternBox is a composite widget to act as pattern box sequencer.
 */

static gpointer ags_pattern_box_parent_class = NULL;
static GQuark quark_accessible_object = 0;

GtkStyle *pattern_box_style = NULL;
GHashTable *ags_pattern_box_led_queue_draw = NULL;

GType
ags_pattern_box_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pattern_box = 0;

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
						  "AgsPatternBox", &ags_pattern_box_info,
						  0);
    
    g_type_add_interface_static(ags_type_pattern_box,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pattern_box);
  }

  return g_define_type_id__volatile;
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
							     "AgsAccessiblePatternBox", &ags_accesssible_pattern_box_info,
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

  quark_accessible_object = g_quark_from_static_string("ags-accessible-object");

  /* GObjectClass */
  gobject = (GObjectClass *) pattern_box;

  gobject->finalize = ags_pattern_box_finalize;

  /* GtkWidget */
  widget = (GtkWidgetClass *) pattern_box;

  widget->realize = ags_pattern_box_realize;
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

  AgsApplicationContext *application_context;

  gchar *str;
  
  gdouble gui_scale_factor;
  guint i;

  application_context = ags_application_context_get_instance();
  
  g_object_set(pattern_box,
	       "can-focus", TRUE,
	       "n-columns", 2,
	       "n-rows", 2,
	       "homogeneous", FALSE,
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
  gui_scale_factor = ags_ui_provider_get_gui_scale_factor(AGS_UI_PROVIDER(application_context));
    
  pattern_box->active_led = 0;
  pattern_box->hled_array = (AgsHLedArray *) ags_hled_array_new();
  g_object_set(pattern_box->hled_array,
	       "led-count", pattern_box->n_controls,
	       "led-width", (guint) (gui_scale_factor * AGS_PATTERN_BOX_LED_DEFAULT_WIDTH),
	       "led-height", (guint) (gui_scale_factor * AGS_PATTERN_BOX_LED_DEFAULT_HEIGHT),
	       NULL);
  gtk_widget_set_size_request((GtkWidget *) pattern_box->hled_array,
			      (guint) (gui_scale_factor * pattern_box->n_controls * AGS_PATTERN_BOX_DEFAULT_PAD_WIDTH), (guint) (gui_scale_factor * AGS_PATTERN_BOX_LED_DEFAULT_HEIGHT));
  gtk_table_attach((GtkTable *) pattern_box,
		   (GtkWidget *) pattern_box->hled_array,
		   0, 1,
		   0, 1,
		   GTK_FILL|GTK_EXPAND, 0,
		   0, 0);
  gtk_widget_show_all((GtkWidget *) pattern_box->hled_array);
  
  if(ags_pattern_box_led_queue_draw == NULL){
    ags_pattern_box_led_queue_draw = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							   NULL,
							   NULL);
  }

  g_hash_table_insert(ags_pattern_box_led_queue_draw,
		      pattern_box, ags_pattern_box_led_queue_draw_timeout);
  g_timeout_add((guint) floor(AGS_UI_PROVIDER_DEFAULT_TIMEOUT * 1000.0), (GSourceFunc) ags_pattern_box_led_queue_draw_timeout, (gpointer) pattern_box);
  
  /* pattern */
  pattern_box->pattern = (GtkHBox *) gtk_hbox_new(FALSE, 0);
  gtk_table_attach((GtkTable *) pattern_box,
		   (GtkWidget *) pattern_box->pattern,
		   0, 1,
		   1, 2,
		   0, 0,
		   0, 0);

  for(i = 0; i < pattern_box->n_controls; i++){
    toggle_button = (GtkToggleButton *) gtk_toggle_button_new();
    gtk_widget_set_size_request((GtkWidget *) toggle_button,
				gui_scale_factor * AGS_PATTERN_BOX_DEFAULT_PAD_WIDTH, gui_scale_factor * AGS_PATTERN_BOX_DEFAULT_PAD_HEIGHT);
    gtk_box_pack_start((GtkBox *) pattern_box->pattern,
		       (GtkWidget *) toggle_button,
		       FALSE, FALSE,
		       0);
  }

  /* page / offset */
  pattern_box->offset = (GtkVBox*) gtk_vbox_new(FALSE, 0);
  gtk_table_attach_defaults((GtkTable *) pattern_box,
			    (GtkWidget *) pattern_box->offset,
			    1, 2,
			    0, 2);
  radio_button = NULL;

  for(i = 0; i < pattern_box->n_indices; i++){
    if(radio_button == NULL){
      str = g_strdup_printf("%d-%d",
			    i * pattern_box->n_controls + 1, (i + 1) * pattern_box->n_controls);
      radio_button = (GtkRadioButton *) gtk_radio_button_new_with_label(NULL,
									str);
      gtk_box_pack_start((GtkBox*) pattern_box->offset,
			 (GtkWidget *) radio_button,
			 FALSE, FALSE,
			 0);

      g_free(str);
    }else{
      str = g_strdup_printf("%d-%d",
			    i * pattern_box->n_controls + 1, (i + 1) * pattern_box->n_controls);
      gtk_box_pack_start((GtkBox*) pattern_box->offset,
			 (GtkWidget *) gtk_radio_button_new_with_label(gtk_radio_button_get_group(radio_button),
								       str),
			 FALSE, FALSE,
			 0);

      g_free(str);
    }
  }
}

void
ags_pattern_box_finalize(GObject *gobject)
{
  g_hash_table_remove(ags_pattern_box_led_queue_draw,
		      gobject);

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

  g_signal_connect_after(G_OBJECT(pattern_box), "focus_in_event",
			 G_CALLBACK(ags_pattern_box_focus_in_callback), (gpointer) pattern_box);

  g_signal_connect_after(G_OBJECT(pattern_box), "focus_out_event",
			 G_CALLBACK(ags_pattern_box_focus_out_callback), (gpointer) pattern_box);
  
  g_signal_connect(G_OBJECT(pattern_box), "key_press_event",
		   G_CALLBACK(ags_pattern_box_key_press_event), (gpointer) pattern_box);

  g_signal_connect(G_OBJECT(pattern_box), "key_release_event",
		   G_CALLBACK(ags_pattern_box_key_release_event), (gpointer) pattern_box);

  /* connect pattern */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  while(list != NULL){
    g_signal_connect(G_OBJECT(list->data), "clicked",
		     G_CALLBACK(ags_pattern_box_pad_callback), (gpointer) pattern_box);

    list = list->next;
  }

  g_list_free(list_start);

  /* connect pattern offset range */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  while(list != NULL){
    g_signal_connect_after(G_OBJECT(list->data), "clicked",
			   G_CALLBACK(ags_pattern_box_offset_callback), (gpointer) pattern_box);
		   
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_pattern_box_disconnect(AgsConnectable *connectable)
{
  AgsPatternBox *pattern_box;

  GList *list, *list_start;

  if((AGS_PATTERN_BOX_CONNECTED & (AGS_PATTERN_BOX(connectable)->flags)) == 0){
    return;
  }

  /* AgsPatternBox */
  pattern_box = AGS_PATTERN_BOX(connectable);

  pattern_box->flags &= (~AGS_PATTERN_BOX_CONNECTED);

  g_object_disconnect(G_OBJECT(pattern_box),
		      "any_signal::focus_in_event",
		      G_CALLBACK(ags_pattern_box_focus_in_callback),
		      (gpointer) pattern_box,
		      "any_signal::focus_out_event",
		      G_CALLBACK(ags_pattern_box_focus_out_callback),
		      (gpointer) pattern_box,
		      "any_signal::key_press_event",
		      G_CALLBACK(ags_pattern_box_key_press_event),
		      (gpointer) pattern_box,
		      "any_signal::key_release_event",
		      G_CALLBACK(ags_pattern_box_key_release_event),
		      (gpointer) pattern_box,
		      NULL);

  /* connect pattern */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->pattern);

  while(list != NULL){
    g_object_disconnect(G_OBJECT(list->data),
			"any_signal::clicked",
			G_CALLBACK(ags_pattern_box_pad_callback),
			(gpointer) pattern_box,
			NULL);

    list = list->next;
  }

  g_list_free(list_start);

  /* connect pattern offset range */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  while(list != NULL){
    g_object_disconnect(G_OBJECT(list->data),
			"any_signal::clicked",
			G_CALLBACK(ags_pattern_box_offset_callback),
			(gpointer) pattern_box,
			NULL);
		   
    list = list->next;
  }

  g_list_free(list_start);
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
    gtk_accessible_set_widget(GTK_ACCESSIBLE(accessible),
			      widget);
  }
  
  return(accessible);
}

void
ags_pattern_box_realize(GtkWidget *widget)
{
  AgsPatternBox *pattern_box;

  pattern_box = (AgsPatternBox *) widget;
  
  /* call parent */
  GTK_WIDGET_CLASS(ags_pattern_box_parent_class)->realize(widget);

  if(pattern_box_style == NULL){
    pattern_box_style = gtk_style_copy(gtk_widget_get_style((GtkWidget *) pattern_box));
  }
  
  gtk_widget_set_style((GtkWidget *) pattern_box,
		       pattern_box_style);
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

  pattern_box = (AgsPatternBox *) gtk_accessible_get_widget(GTK_ACCESSIBLE(action));
  
  key_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
  key_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);

  switch(i){
  case AGS_PATTERN_BOX_MOVE_LEFT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Left;
      
      /* send event */
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PATTERN_BOX_MOVE_RIGHT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Right;
      
      /* send event */
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PATTERN_BOX_INDEX_DECREMENT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Up;
    
      /* send event */
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PATTERN_BOX_INDEX_INCREMENT:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_Down;
      
      /* send event */
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PATTERN_BOX_TOGGLE_PAD:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_space;
      
      /* send event */
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_release);
    }
    break;
  case AGS_PATTERN_BOX_COPY_PATTERN:
    {
      key_press->keyval =
	key_release->keyval = GDK_KEY_c;

      /* create modifier */
      modifier_press = (GdkEventKey *) gdk_event_new(GDK_KEY_PRESS);
      modifier_release = (GdkEventKey *) gdk_event_new(GDK_KEY_RELEASE);

      modifier_press->keyval =
	modifier_release->keyval = GDK_KEY_Control_R;

      /* send event */
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) modifier_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_press);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) key_release);
      gtk_widget_event((GtkWidget *) pattern_box,
		       (GdkEvent *) modifier_release);      
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
  static const gchar *actions[] = {
    "move cursor left",
    "move cursor right",
    "decrement pattern index",
    "increment pattern index",
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
ags_accessible_pattern_box_get_name(AtkAction *action,
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
ags_accessible_pattern_box_get_keybinding(AtkAction *action,
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
 * since: 2.0.0
 */
void
ags_pattern_box_set_pattern(AgsPatternBox *pattern_box)
{
  AgsMachine *machine;
  AgsLine *selected_line;

  GList *list, *list_start;
  GList *line, *line_start;

  guint index0, index1, offset;
  gboolean is_active;
  gboolean set_active;
  guint i;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						   AGS_TYPE_MACHINE);

  if(machine->selected_input_pad == NULL){
    return;
  }
  
  index0 = machine->bank_0;
  index1 = machine->bank_1;

  /* read boundaries */
  list = gtk_container_get_children((GtkContainer *) pattern_box->offset);

  for(i = 0; i < pattern_box->n_indices && !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data)); i++){
    list = list->next;
  }

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
      GList *start_pattern, *pattern;
      
      selected_line = AGS_LINE(line->data);

      g_object_get(selected_line->channel,
		   "pattern", &start_pattern,
		   NULL);

      /* check active */
      pattern = start_pattern;
      
      is_active = ags_pattern_get_bit((AgsPattern *) pattern->data, index0, index1, offset + i);

      g_list_free_full(start_pattern,
		       g_object_unref);
      
      if(!is_active){	
	set_active = FALSE;

	break;
      }

      /* iterate */
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
 * ags_pattern_box_led_queue_draw_timeout:
 * @pattern_box: the #AgsPatternBox
 *
 * Queue draw led.
 *
 * Returns: %TRUE if continue timeout, otherwise %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_pattern_box_led_queue_draw_timeout(AgsPatternBox *pattern_box)
{
  if(g_hash_table_lookup(ags_pattern_box_led_queue_draw,
			 pattern_box) != NULL){
    AgsMachine *machine;

    AgsAudio *audio;
    AgsRecallID *recall_id;
    AgsRecyclingContext *recycling_context;
    
    AgsCountBeatsAudio *play_count_beats_audio;
    AgsCountBeatsAudioRun *play_count_beats_audio_run;

    GList *start_list, *list;
    GList *start_recall, *recall;
    
    guint64 active_led_new;

    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) pattern_box,
						     AGS_TYPE_MACHINE);

    if(machine == NULL){
      return(TRUE);
    }
    
    audio = machine->audio;
    
    /* get some recalls */
    recall_id = NULL;
    g_object_get(audio,
		 "recall-id", &start_list,
		 NULL);

    list = start_list;
    
    while(list != NULL){
      AgsRecyclingContext *current;

      g_object_get(list->data,
		   "recycling-context", &current,
		   NULL);

      g_object_unref(current);

      if(current != NULL){
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

    g_list_free_full(start_list,
		     g_object_unref);
    
    if(recall_id == NULL){      
      return(TRUE);
    }
    
    /* active led */
    g_object_get(play_count_beats_audio_run,
		 "sequencer-counter", &active_led_new,
		 NULL);

    pattern_box->active_led = (guint) (active_led_new % pattern_box->n_controls);

    ags_led_array_unset_all((AgsLedArray *) pattern_box->hled_array);
    ags_led_array_set_nth((AgsLedArray *) pattern_box->hled_array,
			  pattern_box->active_led);
        
    return(TRUE);
  }else{
    return(FALSE);
  }
}

/**
 * ags_pattern_box_new:
 *
 * Create a new instance of #AgsPatternBox
 *
 * Returns: the new #AgsPatternBox
 *
 * Since: 2.0.0
 */
AgsPatternBox*
ags_pattern_box_new()
{
  AgsPatternBox *pattern_box;

  pattern_box = (AgsPatternBox *) g_object_new(AGS_TYPE_PATTERN_BOX,
					       NULL);

  return(pattern_box);
}
