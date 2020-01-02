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

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_automation_window.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_automation_editor_callbacks.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/editor/ags_select_acceleration_dialog.h>
#include <ags/X/editor/ags_ramp_acceleration_dialog.h>
#include <ags/X/editor/ags_position_automation_cursor_dialog.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_automation_toolbar_class_init(AgsAutomationToolbarClass *automation_toolbar);
void ags_automation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_toolbar_init(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_connect(AgsConnectable *connectable);
void ags_automation_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_automation_toolbar
 * @short_description: automation toolbar
 * @title: AgsAutomationToolbar
 * @section_id:
 * @include: ags/X/editor/ags_automation_toolbar.h
 *
 * The #AgsAutomationToolbar lets you choose edit tool.
 */

GType
ags_automation_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_automation_toolbar = 0;

    static const GTypeInfo ags_automation_toolbar_info = {
      sizeof (AgsAutomationToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_automation_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAutomationToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_automation_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_automation_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_automation_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
							 "AgsAutomationToolbar", &ags_automation_toolbar_info,
							 0);
    
    g_type_add_interface_static(ags_type_automation_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_automation_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_automation_toolbar_class_init(AgsAutomationToolbarClass *automation_toolbar)
{
}

void
ags_automation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_automation_toolbar_connect;
  connectable->disconnect = ags_automation_toolbar_disconnect;
}

void
ags_automation_toolbar_init(AgsAutomationToolbar *automation_toolbar)
{
  GtkToolItem *tool_item;
  GtkMenuToolButton *menu_tool_button;
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;
  GtkHBox *hbox;

  /* position */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->position = g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
					      "label", i18n("Position"),
					      "stock-id", GTK_STOCK_JUMP_TO,
					      NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->position);

  /* edit */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->edit = g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
					  "stock-id", GTK_STOCK_EDIT,
					  NULL);
  automation_toolbar->selected_edit_mode = automation_toolbar->edit;
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->edit);

  /* clear */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->clear = g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
					   "stock-id", GTK_STOCK_CLEAR,
					   NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->clear);
  
  /* select */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->select = g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
					    "label", i18n("Select"),
					    "stock-id", GTK_STOCK_SELECT_ALL,
					    NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->select);

  /* copy */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->copy = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							    "stock-id", GTK_STOCK_COPY,
							    NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->copy);

  /* cut */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->cut = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							   "stock-id", GTK_STOCK_CUT,
							   NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->cut);

  /* paste */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->paste_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								      "stock-id", GTK_STOCK_PASTE,
								      NULL);

  menu = (GtkMenu *) gtk_menu_new();

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", "match line",
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);
  
  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", "no duplicates",
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  gtk_menu_tool_button_set_menu(automation_toolbar->paste_tool,
				(GtkWidget *) menu);
  gtk_widget_show_all((GtkWidget *) menu);

  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->paste_tool);
  
  /* menu tool */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  automation_toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								     "label", i18n("Tool"),
								     "stock-id", GTK_STOCK_EXECUTE,
								     NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) automation_toolbar->menu_tool);

  /* menu tool - tool popup */
  automation_toolbar->tool_popup = ags_automation_toolbar_tool_popup_new(automation_toolbar);
  gtk_menu_tool_button_set_menu(automation_toolbar->menu_tool,
				(GtkWidget *) automation_toolbar->tool_popup);

  /* menu tool - dialogs */
  automation_toolbar->select_acceleration = (GtkDialog *) ags_select_acceleration_dialog_new(NULL);
  automation_toolbar->ramp_acceleration = (GtkDialog *) ags_ramp_acceleration_dialog_new(NULL);
  automation_toolbar->position_automation_cursor = (GtkDialog *) ags_position_automation_cursor_dialog_new(NULL);

  /*  */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);
  
  hbox = gtk_hbox_new(FALSE,
		      0);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) hbox);


  label = (GtkLabel *) gtk_label_new(i18n("Zoom"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  automation_toolbar->zoom_history = 2;
  automation_toolbar->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(GTK_COMBO_BOX(automation_toolbar->zoom),
			   2);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) automation_toolbar->zoom,
		     FALSE, FALSE,
		     0);

  /* port */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  hbox = gtk_hbox_new(FALSE,
		      0);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("Port"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  automation_toolbar->port = (GtkComboBox *) gtk_combo_box_new();

  cell_renderer = gtk_cell_renderer_toggle_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(automation_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(automation_toolbar->port), cell_renderer,
				 "active", 0,
				 NULL);
  gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(cell_renderer),
					   TRUE);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(automation_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(automation_toolbar->port), cell_renderer,
				 "text", 1,
				 NULL);

  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(automation_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(automation_toolbar->port), cell_renderer,
				 "text", 2,
				 NULL);
  
  gtk_box_pack_start(hbox,
		     (GtkWidget *) automation_toolbar->port,
		     FALSE, FALSE,
		     0);

  /* opacity */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) automation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  hbox = gtk_hbox_new(FALSE,
		      0);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("Opacity"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  automation_toolbar->opacity = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.001);
  gtk_spin_button_set_value(automation_toolbar->opacity, 0.3);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) automation_toolbar->opacity,
		     FALSE, FALSE,
		     0);
}


void
ags_automation_toolbar_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsAutomationWindow *automation_window;
  AgsAutomationEditor *automation_editor;
  AgsAutomationToolbar *automation_toolbar;

  GList *list;
  
  automation_toolbar = AGS_AUTOMATION_TOOLBAR(connectable);

  if((AGS_AUTOMATION_TOOLBAR_CONNECTED & (automation_toolbar->flags)) != 0){
    return;
  }

  automation_toolbar->flags |= AGS_AUTOMATION_TOOLBAR_CONNECTED;
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);

  automation_window = (AgsAutomationWindow *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_WINDOW);
  window = (AgsWindow *) automation_window->parent_window;
  
  g_object_set(automation_toolbar->select_acceleration,
	       "main-window", window,
	       NULL);
  g_object_set(automation_toolbar->ramp_acceleration,
	       "main-window", window,
	       NULL);
  g_object_set(automation_toolbar->position_automation_cursor,
	       "main-window", window,
	       NULL);

  /*  */
  g_signal_connect_after(G_OBJECT(automation_editor), "machine-changed",
			 G_CALLBACK(ags_automation_toolbar_machine_changed_callback), automation_toolbar);

  /* tool */
  g_signal_connect_after((GObject *) automation_toolbar->position, "toggled",
			 G_CALLBACK(ags_automation_toolbar_position_callback), (gpointer) automation_toolbar);

  g_signal_connect_after((GObject *) automation_toolbar->edit, "toggled",
			 G_CALLBACK(ags_automation_toolbar_edit_callback), (gpointer) automation_toolbar);

  g_signal_connect_after((GObject *) automation_toolbar->clear, "toggled",
			 G_CALLBACK(ags_automation_toolbar_clear_callback), (gpointer) automation_toolbar);

  g_signal_connect_after((GObject *) automation_toolbar->select, "toggled",
			 G_CALLBACK(ags_automation_toolbar_select_callback), (gpointer) automation_toolbar);

  /* edit */
  g_signal_connect((GObject *) automation_toolbar->copy, "clicked",
		   G_CALLBACK(ags_automation_toolbar_copy_or_cut_callback), (gpointer) automation_toolbar);

  g_signal_connect((GObject *) automation_toolbar->cut, "clicked",
		   G_CALLBACK(ags_automation_toolbar_copy_or_cut_callback), (gpointer) automation_toolbar);

  g_signal_connect((GObject *) automation_toolbar->paste_tool, "clicked",
		   G_CALLBACK(ags_automation_toolbar_paste_callback), (gpointer) automation_toolbar);

  list = gtk_container_get_children((GtkContainer *) gtk_menu_tool_button_get_menu(automation_toolbar->paste_tool));

  g_signal_connect_after(list->data, "activate",
			 G_CALLBACK(ags_automation_toolbar_match_line_callback), automation_toolbar);

  g_signal_connect_after(list->next->data, "activate",
			 G_CALLBACK(ags_automation_toolbar_no_duplicates_callback), automation_toolbar);

  g_list_free(list);
  
  /* additional tools */
  ags_connectable_connect(AGS_CONNECTABLE(automation_toolbar->select_acceleration));

  ags_connectable_connect(AGS_CONNECTABLE(automation_toolbar->ramp_acceleration));

  ags_connectable_connect(AGS_CONNECTABLE(automation_toolbar->position_automation_cursor));

  /* zoom */
  g_signal_connect_after((GObject *) automation_toolbar->zoom, "changed",
			 G_CALLBACK(ags_automation_toolbar_zoom_callback), (gpointer) automation_toolbar);

  /* port */
  g_signal_connect_after(automation_toolbar->port, "changed",
			 G_CALLBACK(ags_automation_toolbar_port_callback), automation_toolbar);

  /* opacity */
  g_signal_connect_after((GObject *) automation_toolbar->opacity, "value-changed",
			 G_CALLBACK(ags_automation_toolbar_opacity_callback), (gpointer) automation_toolbar);
}

void
ags_automation_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsAutomationToolbar *automation_toolbar;

  GList *list;
  
  automation_toolbar = AGS_AUTOMATION_TOOLBAR(connectable);

  if((AGS_AUTOMATION_TOOLBAR_CONNECTED & (automation_toolbar->flags)) == 0){
    return;
  }

  automation_toolbar->flags &= (~AGS_AUTOMATION_TOOLBAR_CONNECTED);

  /* tool */
  g_object_disconnect(G_OBJECT(automation_toolbar->position),
		      "any_signal::toggled",
		      G_CALLBACK(ags_automation_toolbar_position_callback),
		      automation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(automation_toolbar->edit),
		      "any_signal::toggled",
		      G_CALLBACK(ags_automation_toolbar_edit_callback),
		      automation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(automation_toolbar->clear),
		      "any_signal::toggled",
		      G_CALLBACK(ags_automation_toolbar_clear_callback),
		      automation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(automation_toolbar->select),
		      "any_signal::toggled",
		      G_CALLBACK(ags_automation_toolbar_select_callback),
		      automation_toolbar,
		      NULL);

  /* edit */
  g_object_disconnect(G_OBJECT(automation_toolbar->copy),
		      "any_signal::clicked",
		      G_CALLBACK(ags_automation_toolbar_copy_or_cut_callback),
		      automation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(automation_toolbar->cut),
		      "any_signal::clicked",
		      G_CALLBACK(ags_automation_toolbar_copy_or_cut_callback),
		      automation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(automation_toolbar->paste_tool),
		      "any_signal::clicked",
		      G_CALLBACK(ags_automation_toolbar_paste_callback),
		      automation_toolbar,
		      NULL);
  
  list = gtk_container_get_children((GtkContainer *) gtk_menu_tool_button_get_menu(automation_toolbar->paste_tool));

  g_object_disconnect(G_OBJECT(list->data),
		      "any_signal::activate",
		      G_CALLBACK(ags_automation_toolbar_match_line_callback),
		      automation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(list->next->data),
		      "any_signal::activate",
		      G_CALLBACK(ags_automation_toolbar_no_duplicates_callback),
		      automation_toolbar,
		      NULL);

  g_list_free(list);

  /* additional tools */
  ags_connectable_disconnect(AGS_CONNECTABLE(automation_toolbar->select_acceleration));

  ags_connectable_disconnect(AGS_CONNECTABLE(automation_toolbar->ramp_acceleration));

  ags_connectable_disconnect(AGS_CONNECTABLE(automation_toolbar->position_automation_cursor));

  /* zoom */
  g_object_disconnect(G_OBJECT(automation_toolbar->zoom),
		      "any_signal::changed",
		      G_CALLBACK(ags_automation_toolbar_zoom_callback),
		      automation_toolbar,
		      NULL);

  /* port */
  g_object_disconnect(G_OBJECT(automation_toolbar->port),
		      "any_signal::changed",
		      G_CALLBACK(ags_automation_toolbar_port_callback),
		      automation_toolbar,
		      NULL);

  /* opacity */
  g_object_disconnect(G_OBJECT(automation_toolbar->opacity),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_automation_toolbar_opacity_callback),
		      automation_toolbar,
		      NULL);
}

/**
 * ags_automation_toolbar_load_port:
 * @automation_toolbar: an #AgsAutomationToolbar
 *
 * Fill in port field with available ports.
 *
 * Since: 2.0.0
 */
void
ags_automation_toolbar_load_port(AgsAutomationToolbar *automation_toolbar)
{
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;
  
  GtkListStore *list_store;
  GtkTreeIter iter;

  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
  
  GList *start_port, *port;

  gchar **collected_specifier;

  guint length;
  
  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  machine = automation_editor->selected_machine;

  if(machine == NULL){
    gtk_combo_box_set_model(automation_toolbar->port,
			    NULL);
    return;
  }

  /*  */
  list_store = gtk_list_store_new(3,
				  G_TYPE_BOOLEAN,
				  G_TYPE_STRING,
				  G_TYPE_STRING);
  gtk_combo_box_set_model(automation_toolbar->port,
			  GTK_TREE_MODEL(list_store));

  collected_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;
  
  /* audio */
  port =
    start_port = ags_audio_collect_all_audio_ports(machine->audio);

  while(port != NULL){
    AgsPluginPort *plugin_port;

    gchar *specifier;

    gboolean is_enabled;
    gboolean contains_control_name;
    
    g_object_get(port->data,
		 "specifier", &specifier,
		 "plugin-port", &plugin_port,
		 NULL);

#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(collected_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_specifier,
					      specifier);
#endif

    if(plugin_port != NULL &&
       !contains_control_name){
      /* create list store entry */
      is_enabled = (ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
										    G_TYPE_NONE,
										    specifier)) ? TRUE: FALSE;

      gtk_list_store_append(list_store, &iter);
      gtk_list_store_set(list_store, &iter,
			 0, is_enabled,
			 1, g_strdup("audio"),
			 2, g_strdup(specifier),
			 -1);      

      /* add to collected specifier */
      collected_specifier = (gchar **) realloc(collected_specifier,
				     (length + 1) * sizeof(gchar *));
      collected_specifier[length - 1] = g_strdup(specifier);
      collected_specifier[length] = NULL;

      length++;
    }
    
    /* iterate */
    port = port->next;
  }

  g_list_free_full(start_port,
		   g_object_unref);
  
  /* output */
  g_object_get(machine->audio,
	       "output", &start_channel,
	       NULL);

  channel = start_channel;
  g_object_ref(channel);

  next_channel = NULL;
  
  while(channel != NULL){
    /* output */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

#ifdef HAVE_GLIB_2_44
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	/* create list store entry */
	is_enabled = (ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
										      AGS_TYPE_OUTPUT,
										      specifier)) ? TRUE: FALSE;
    
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, is_enabled,
			   1, g_strdup("output"),
			   2, g_strdup(specifier),
			   -1);      

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  /* input */
  g_object_get(machine->audio,
	       "input", &start_channel,
	       NULL);

  channel = start_channel;
  g_object_ref(channel);

  next_channel = NULL;

  while(channel != NULL){
    /* input */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

#ifdef HAVE_GLIB_2_44
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);
#else
      contains_control_name = ags_strv_contains(collected_specifier,
						specifier);
#endif

      if(plugin_port != NULL &&
	 !contains_control_name){
	/* create list store entry */
	is_enabled = (ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
										      AGS_TYPE_INPUT,
										      specifier)) ? TRUE: FALSE;
    
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, is_enabled,
			   1, g_strdup("input"),
			   2, g_strdup(specifier),
			   -1);      

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(next_channel != NULL){
    g_object_unref(next_channel);
  }
  
  g_strfreev(collected_specifier);
  
  gtk_list_store_append(list_store, &iter);
  gtk_list_store_set(list_store, &iter,
		     0, FALSE,
		     1, g_strdup(""),
		     2, g_strdup(""),
		     -1);
  gtk_combo_box_set_active_iter(automation_toolbar->port,
				&iter);
}

/**
 * ags_automation_toolbar_apply_port:
 * @automation_toolbar: an #AgsAutomationToolbar
 * @channel_type: the #GType of channel
 * @control_name: the specifier as string
 * @port: the #AgsPort
 *
 * Applies all port to appropriate #AgsMachine.
 *
 * Since: 2.0.0
 */
void
ags_automation_toolbar_apply_port(AgsAutomationToolbar *automation_toolbar,
				  GType channel_type, gchar *control_name)
{
  AgsAutomationEditor *automation_editor;
  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;

  AgsConfig *config;

  GList *start_automation, *automation;
  GList *start_port, *port;
  
  gchar *str;
  
  gdouble gui_scale_factor;
  guint length;
  gboolean contains_specifier;
  gboolean is_active;

  if(!g_ascii_strncasecmp(control_name,
			  "",
			  1)){
    return;
  }

  automation_editor = (AgsAutomationEditor *) gtk_widget_get_ancestor((GtkWidget *) automation_toolbar,
								      AGS_TYPE_AUTOMATION_EDITOR);
  machine = automation_editor->selected_machine;

  model = gtk_combo_box_get_model(automation_toolbar->port);

  config = ags_config_get_instance();
  
  /* scale factor */
  gui_scale_factor = 1.0;
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    gui_scale_factor = g_ascii_strtod(str,
				      NULL);

    g_free(str);
  }

  /* update port combo box */
  start_port = NULL;
  
  contains_specifier = FALSE;

  if(gtk_combo_box_get_active_iter(automation_toolbar->port, &iter)){
    GType scope;
    gchar *str, *specifier;

    GValue value = {0,};

    gtk_tree_model_get(model,
		       &iter,
		       1, &str,
		       2, &specifier,
		       -1);

    scope = G_TYPE_NONE;

    if(!g_ascii_strcasecmp("audio",
			   str)){
      scope = G_TYPE_NONE;

      port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
									control_name,
									TRUE);
      start_port = port;
      
      port = ags_audio_collect_all_audio_ports_by_specifier_and_context(machine->audio,
									control_name,
									FALSE);

      if(start_port != NULL){
	start_port = g_list_concat(start_port,
				   port);
      }else{
	start_port = port;
      }
    }else if(!g_ascii_strcasecmp("output",
				 str)){
      scope = AGS_TYPE_OUTPUT;

      g_object_get(machine->audio,
		   "output", &start_channel,
		   NULL);

      channel = start_channel;
      g_object_ref(channel);

      next_channel = NULL;
      
      start_port = NULL;
      
      while(channel != NULL){
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      control_name,
									      TRUE);
	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}
      
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      control_name,
									      FALSE);

	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }      
    }else if(!g_ascii_strcasecmp("input",
				 str)){
      scope = AGS_TYPE_INPUT;

      g_object_get(machine->audio,
		   "input", &start_channel,
		   NULL);

      channel = start_channel;
      g_object_ref(channel);

      next_channel = NULL;

      start_port = NULL;
      
      while(channel != NULL){
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      control_name,
									      TRUE);
	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}
      
	port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
									      control_name,
									      FALSE);

	if(start_port != NULL){
	  start_port = g_list_concat(start_port,
				     port);
	}else{
	  start_port = port;
	}

	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      /* unref */
      if(start_channel != NULL){
	g_object_unref(start_channel);
      }

      if(next_channel != NULL){
	g_object_unref(next_channel);
      }
    }

    if(scope == channel_type &&
       !g_ascii_strcasecmp(specifier,
			   control_name)){
      gtk_tree_model_get_value(model,
			       &iter,
			       0, &value);

      if(g_value_get_boolean(&value)){
	g_value_set_boolean(&value, FALSE);
      }else{
	g_value_set_boolean(&value, TRUE);

	contains_specifier = TRUE;
      }
  
      gtk_list_store_set_value(GTK_LIST_STORE(model),
			       &iter,
			       0,
			       &value);
    }
  }
  
  /* add/remove enabled automation port */
  if(contains_specifier){
    machine->enabled_automation_port = g_list_prepend(machine->enabled_automation_port,
						      ags_machine_automation_port_alloc(channel_type, control_name));
  }else{
    machine->enabled_automation_port = g_list_delete_link(machine->enabled_automation_port,
							  ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
															  channel_type, control_name));
  }

  g_object_get(machine->audio,
	       "automation", &start_automation,
	       NULL);
  
  /* apply */
  if(contains_specifier){
    AgsAutomationEdit *automation_edit;
    AgsScale *scale;

    AgsPluginPort *plugin_port;

    gchar *specifier;

    gdouble upper, lower;
    gdouble default_value;

    GValue *upper_value, *lower_value;
    GValue *value;
    
    /* scale */
    scale = ags_scale_new();
    g_object_set(scale,
		 "scale-width", (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_SCALE_WIDTH),
		 "scale-height", (guint) (gui_scale_factor * AGS_SCALE_DEFAULT_SCALE_HEIGHT),
		 NULL);

    /* get some fields */
    plugin_port = NULL;
    
    if(start_port != NULL){
      g_object_get(start_port->data,
		   "plugin-port", &plugin_port,
		   NULL);   
    }

    if(plugin_port != NULL){
      g_object_get(plugin_port,
		   "upper-value", &upper_value,
		   "lower-value", &lower_value,
		   "default-value", &value,
		   NULL);

      upper = g_value_get_float(upper_value);
      lower = g_value_get_float(lower_value);

      default_value = g_value_get_float(value);

      g_object_unref(plugin_port);
    }else{
      upper = 1.0;
      lower = 0.0;
      
      default_value = 1.0;
    }    
      
    specifier = g_strdup(control_name);

    /* apply scale */
    g_object_set(scale,
		 "control-name", specifier,
		 "upper", upper,
		 "lower", lower,
		 "default-value", default_value,
		 NULL);

    if(channel_type == G_TYPE_NONE){
      gtk_box_pack_start((GtkBox *) automation_editor->audio_scrolled_scale_box->scale_box,
			 (GtkWidget *) scale,
			 FALSE, FALSE,
			 AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
    }else if(channel_type == AGS_TYPE_OUTPUT){
      gtk_box_pack_start((GtkBox *) automation_editor->output_scrolled_scale_box->scale_box,
			 (GtkWidget *) scale,
			 FALSE, FALSE,
			 AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
    }else if(channel_type == AGS_TYPE_INPUT){
      gtk_box_pack_start((GtkBox *) automation_editor->input_scrolled_scale_box->scale_box,
			 (GtkWidget *) scale,
			 FALSE, FALSE,
			 AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
    }
      
    gtk_widget_show((GtkWidget *) scale);
	  
    /* automation edit */
    automation_edit = ags_automation_edit_new();

    g_object_set(automation_edit,
		 "channel-type", channel_type,
		 "control-specifier", specifier,
		 "control-name", control_name,
		 "upper", upper,
		 "lower", lower,
		 "default-value", default_value,
		 "upper", upper,
		 "lower", lower,
		 NULL);

    if(plugin_port != NULL &&
       ags_plugin_port_test_flags(plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC)){
      automation_edit->flags |= AGS_AUTOMATION_EDIT_LOGARITHMIC;
    }
    
    if(channel_type == G_TYPE_NONE){
      gtk_box_pack_start((GtkBox *) automation_editor->audio_scrolled_automation_edit_box->automation_edit_box,
			 (GtkWidget *) automation_edit,
			 FALSE, FALSE,
			 AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
    }else if(channel_type == AGS_TYPE_OUTPUT){
      gtk_box_pack_start((GtkBox *) automation_editor->output_scrolled_automation_edit_box->automation_edit_box,
			 (GtkWidget *) automation_edit,
			 FALSE, FALSE,
			 AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
    }else if(channel_type == AGS_TYPE_INPUT){
      gtk_box_pack_start((GtkBox *) automation_editor->input_scrolled_automation_edit_box->automation_edit_box,
			 (GtkWidget *) automation_edit,
			 FALSE, FALSE,
			 AGS_AUTOMATION_EDIT_DEFAULT_PADDING);
    }
      
    ags_connectable_connect(AGS_CONNECTABLE(automation_edit));
    gtk_widget_show((GtkWidget *) automation_edit);    

    if(channel_type == G_TYPE_NONE){
      g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			     G_CALLBACK(ags_automation_editor_audio_automation_edit_hscrollbar_value_changed), (gpointer) automation_editor);
    }else if(channel_type == AGS_TYPE_OUTPUT){
      g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			     G_CALLBACK(ags_automation_editor_output_automation_edit_hscrollbar_value_changed), (gpointer) automation_editor);
    }else if(channel_type == AGS_TYPE_INPUT){
      g_signal_connect_after((GObject *) automation_edit->hscrollbar, "value-changed",
			     G_CALLBACK(ags_automation_editor_input_automation_edit_hscrollbar_value_changed), (gpointer) automation_editor);
    }
    
    /* unset bypass */
    ags_audio_add_automation_port(machine->audio, control_name);
    
    automation = start_automation;
    
    while((automation = ags_automation_find_channel_type_with_control_name(automation,
									   channel_type, control_name)) != NULL){
      ags_automation_unset_flags(automation->data,
				 AGS_AUTOMATION_BYPASS);

      automation = automation->next;
    }
  }else{
    GList *list_start, *list;

    gint nth;
    gboolean success;

    /* audio */
    nth = -1;
    success = FALSE;

    list =
      list_start = NULL;
    
    if(channel_type == G_TYPE_NONE){
      list =
	list_start = gtk_container_get_children((GtkContainer *) automation_editor->audio_scrolled_automation_edit_box->automation_edit_box);
    }else if(channel_type == AGS_TYPE_OUTPUT){
      list =
	list_start = gtk_container_get_children((GtkContainer *) automation_editor->output_scrolled_automation_edit_box->automation_edit_box);
    }else if(channel_type == AGS_TYPE_INPUT){
      list =
	list_start = gtk_container_get_children((GtkContainer *) automation_editor->input_scrolled_automation_edit_box->automation_edit_box);
    }
    

    while(list != NULL){
      nth++;
      
      if(AGS_AUTOMATION_EDIT(list->data)->channel_type == channel_type &&
	 !g_strcmp0(AGS_AUTOMATION_EDIT(list->data)->control_name,
		    control_name)){
	gtk_widget_destroy(list->data);

	success = TRUE;

	break;
      }
	
      list = list->next;
    }

    g_list_free(list_start);

    if(success){
      if(channel_type == G_TYPE_NONE){
	list_start = gtk_container_get_children((GtkContainer *) automation_editor->audio_scrolled_scale_box->scale_box);
      }else if(channel_type == AGS_TYPE_OUTPUT){
	list_start = gtk_container_get_children((GtkContainer *) automation_editor->output_scrolled_scale_box->scale_box);
      }else if(channel_type == AGS_TYPE_INPUT){
	list_start = gtk_container_get_children((GtkContainer *) automation_editor->input_scrolled_scale_box->scale_box);
      }

      list = g_list_nth(list_start,
			nth);

      gtk_widget_destroy(list->data);
      
      g_list_free(list_start);
    }
    
    /* set bypass */
    ags_audio_remove_automation_port(machine->audio, control_name);

    automation = start_automation;
    
    while((automation = ags_automation_find_channel_type_with_control_name(automation,
									   channel_type, control_name)) != NULL){
      ags_automation_set_flags(automation->data,
			       AGS_AUTOMATION_BYPASS);

      automation = automation->next;
    }
  }

  g_list_free_full(start_automation,
		   g_object_unref);

  g_list_free_full(start_port,
		   g_object_unref);
}

/**
 * ags_automation_toolbar_tool_popup_new:
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 2.0.0
 */
GtkMenu*
ags_automation_toolbar_tool_popup_new(AgsAutomationToolbar *automation_toolbar)
{
  GtkMenu *tool_popup;
  GtkMenuItem *item;

  GList *list, *list_start;

  tool_popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select accelerations"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("ramp accelerations"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  gtk_menu_shell_append((GtkMenuShell*) tool_popup,
			(GtkWidget*) gtk_separator_menu_item_new());
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("enable all lines"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("disable all lines"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);
  
  /* connect */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) tool_popup);

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_automation_toolbar_tool_popup_select_acceleration_callback), (gpointer) automation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_automation_toolbar_tool_popup_ramp_acceleration_callback), (gpointer) automation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_automation_toolbar_tool_popup_position_cursor_callback), (gpointer) automation_toolbar);

  list = list->next->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_automation_toolbar_tool_popup_enable_all_lines_callback), (gpointer) automation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_automation_toolbar_tool_popup_disable_all_lines_callback), (gpointer) automation_toolbar);

  g_list_free(list_start);

  /* show */
  gtk_widget_show_all((GtkWidget *) tool_popup);
  
  return(tool_popup);
}

/**
 * ags_automation_toolbar_new:
 *
 * Create a new instance of #AgsAutomationToolbar.
 *
 * Returns: the new #AgsAutomationToolbar
 *
 * Since: 2.0.0
 */
AgsAutomationToolbar*
ags_automation_toolbar_new()
{
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = (AgsAutomationToolbar *) g_object_new(AGS_TYPE_AUTOMATION_TOOLBAR, NULL);

  return(automation_toolbar);
}
