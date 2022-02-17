/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/GSequencer/editor/ags_notation_toolbar.h>
#include <ags/GSequencer/editor/ags_notation_toolbar_callbacks.h>

#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_menu_bar.h>

#include <ags/GSequencer/editor/ags_move_note_dialog.h>
#include <ags/GSequencer/editor/ags_crop_note_dialog.h>
#include <ags/GSequencer/editor/ags_select_note_dialog.h>
#include <ags/GSequencer/editor/ags_position_notation_cursor_dialog.h>

#include <ags/i18n.h>

void ags_notation_toolbar_class_init(AgsNotationToolbarClass *notation_toolbar);
void ags_notation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_notation_toolbar_init(AgsNotationToolbar *notation_toolbar);

void ags_notation_toolbar_connect(AgsConnectable *connectable);
void ags_notation_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_notation_toolbar
 * @short_description: notation_toolbar
 * @title: AgsNotationToolbar
 * @section_id:
 * @include: ags/X/editor/ags_notation_toolbar.h
 *
 * The #AgsNotationToolbar lets you choose edit tool.
 */

GType
ags_notation_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_notation_toolbar = 0;

    static const GTypeInfo ags_notation_toolbar_info = {
      sizeof (AgsNotationToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_notation_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsNotationToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_notation_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_notation_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_notation_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
						       "AgsNotationToolbar", &ags_notation_toolbar_info,
						       0);
    
    g_type_add_interface_static(ags_type_notation_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_notation_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_notation_toolbar_class_init(AgsNotationToolbarClass *notation_toolbar)
{
  /* empty */
}

void
ags_notation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_notation_toolbar_connect;
  connectable->disconnect = ags_notation_toolbar_disconnect;
}

void
ags_notation_toolbar_init(AgsNotationToolbar *notation_toolbar)
{
  GtkToolItem *tool_item;
  GtkLabel *label;
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkHBox *hbox;
  
  notation_toolbar->flags = 0;
  
  /* position */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);
  
  notation_toolbar->position = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
								    "label", i18n("Position"),
								    "icon-name", "go-jump",
								    NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->position);
  
  /* edit */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->edit = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
								"label", i18n("Edit"),
								"icon-name", "format-text-direction-ltr",
								"active", TRUE,
								NULL);
  notation_toolbar->selected_edit_mode = notation_toolbar->edit;
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->edit);

  /* clear */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->clear = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
								 "label", i18n("Clear"),
								 "icon-name", "edit-clear",
								 NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->clear);

  /* select */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->select = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
								  "label", i18n("Select"),
								  "icon-name", "edit-select-all",
								  NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->select);

  /* copy */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->copy = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							  "label", i18n("Copy"),							  
							  "icon-name", "edit-copy",
							  NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->copy);

  /* cut */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->cut = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							 "label", i18n("Cut"),
							 "icon-name", "edit-cut",
							 NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->cut);

  /* paste */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->paste_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								    "label", i18n("Paste"),
								    "icon-name", "edit-paste",
								    NULL);
  
  menu = (GtkMenu *) gtk_menu_new();

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("match audio channel"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);
  
  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("no duplicates"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  gtk_menu_tool_button_set_menu(notation_toolbar->paste_tool,
				(GtkWidget *) menu);
  gtk_widget_show_all((GtkWidget *) menu);
  
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->paste_tool);

  /* invert */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->invert = (GtkButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
							"label", i18n("Invert"),
							"icon-name", "object-flip-vertical",
							"label", i18n("Invert"),
							NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->invert);

  /* menu tool */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  notation_toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								   "label", i18n("Tool"),
								   "icon-name", "system-run",
								   NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) notation_toolbar->menu_tool);

  /* menu tool - tool popup */
  notation_toolbar->tool_popup = ags_notation_toolbar_tool_popup_new(notation_toolbar);
  gtk_menu_tool_button_set_menu(notation_toolbar->menu_tool,
				(GtkWidget *) notation_toolbar->tool_popup);

  /* menu tool - dialogs */
  notation_toolbar->move_note = (GtkDialog *) ags_move_note_dialog_new(NULL);
  notation_toolbar->crop_note = (GtkDialog *) ags_crop_note_dialog_new(NULL);
  notation_toolbar->select_note = (GtkDialog *) ags_select_note_dialog_new(NULL);
  notation_toolbar->position_notation_cursor = (GtkDialog *) ags_position_notation_cursor_dialog_new(NULL);
  
  /* zoom */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
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

  notation_toolbar->zoom_history = 2;
  notation_toolbar->zoom = (GtkComboBoxText *) ags_zoom_combo_box_new();
  gtk_combo_box_set_active((GtkComboBox *) notation_toolbar->zoom, 2);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) notation_toolbar->zoom,
		     FALSE, FALSE,
		     0);

  /* opacity */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) notation_toolbar,
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

  notation_toolbar->opacity = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.001);
  gtk_spin_button_set_value(notation_toolbar->opacity, 0.8);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) notation_toolbar->opacity,
		     FALSE, FALSE,
		     0);
}

void
ags_notation_toolbar_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsNotationToolbar *notation_toolbar;

  GList *list;
  
  notation_toolbar = AGS_NOTATION_TOOLBAR(connectable);

  if((AGS_NOTATION_TOOLBAR_CONNECTED & (notation_toolbar->flags)) != 0){
    return;
  }

  notation_toolbar->flags |= AGS_NOTATION_TOOLBAR_CONNECTED;

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) notation_toolbar, AGS_TYPE_WINDOW));

  g_object_set(notation_toolbar->move_note,
	       "main-window", window,
	       NULL);
  g_object_set(notation_toolbar->crop_note,
	       "main-window", window,
	       NULL);
  g_object_set(notation_toolbar->select_note,
	       "main-window", window,
	       NULL);
  g_object_set(notation_toolbar->position_notation_cursor,
	       "main-window", window,
	       NULL);

  /* tool */
  g_signal_connect_after((GObject *) notation_toolbar->position, "toggled",
			 G_CALLBACK(ags_notation_toolbar_position_callback), (gpointer) notation_toolbar);

  g_signal_connect_after((GObject *) notation_toolbar->edit, "toggled",
			 G_CALLBACK(ags_notation_toolbar_edit_callback), (gpointer) notation_toolbar);

  g_signal_connect_after((GObject *) notation_toolbar->clear, "toggled",
			 G_CALLBACK(ags_notation_toolbar_clear_callback), (gpointer) notation_toolbar);

  g_signal_connect_after((GObject *) notation_toolbar->select, "toggled",
			 G_CALLBACK(ags_notation_toolbar_select_callback), (gpointer) notation_toolbar);

  /* edit */
  g_signal_connect((GObject *) notation_toolbar->copy, "clicked",
		   G_CALLBACK(ags_notation_toolbar_copy_or_cut_callback), (gpointer) notation_toolbar);

  g_signal_connect((GObject *) notation_toolbar->cut, "clicked",
		   G_CALLBACK(ags_notation_toolbar_copy_or_cut_callback), (gpointer) notation_toolbar);

  g_signal_connect((GObject *) notation_toolbar->paste_tool, "clicked",
		   G_CALLBACK(ags_notation_toolbar_paste_callback), (gpointer) notation_toolbar);

  g_signal_connect((GObject *) notation_toolbar->invert, "clicked",
		   G_CALLBACK(ags_notation_toolbar_invert_callback), (gpointer) notation_toolbar);

  list = gtk_container_get_children((GtkContainer *) gtk_menu_tool_button_get_menu(notation_toolbar->paste_tool));

  g_signal_connect_after(list->data, "activate",
			 G_CALLBACK(ags_notation_toolbar_match_audio_channel_callback), notation_toolbar);

  g_signal_connect_after(list->next->data, "activate",
			 G_CALLBACK(ags_notation_toolbar_no_duplicates_callback), notation_toolbar);

  g_list_free(list);

  /* additional tools */
  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->position_notation_cursor));

  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->crop_note));

  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->move_note));

  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->select_note));

  /* zoom */
  g_signal_connect_after((GObject *) notation_toolbar->zoom, "changed",
			 G_CALLBACK(ags_notation_toolbar_zoom_callback), (gpointer) notation_toolbar);

  /* opacity */
  g_signal_connect_after((GObject *) notation_toolbar->opacity, "value-changed",
			 G_CALLBACK(ags_notation_toolbar_opacity_callback), (gpointer) notation_toolbar);
}

void
ags_notation_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsNotationToolbar *notation_toolbar;

  GList *list;
  
  notation_toolbar = AGS_NOTATION_TOOLBAR(connectable);

  if((AGS_NOTATION_TOOLBAR_CONNECTED & (notation_toolbar->flags)) == 0){
    return;
  }

  notation_toolbar->flags &= (~AGS_NOTATION_TOOLBAR_CONNECTED);

  /* tool */
  g_object_disconnect(G_OBJECT(notation_toolbar->position),
		      "any_signal::toggled",
		      G_CALLBACK(ags_notation_toolbar_position_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->edit),
		      "any_signal::toggled",
		      G_CALLBACK(ags_notation_toolbar_edit_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->clear),
		      "any_signal::toggled",
		      G_CALLBACK(ags_notation_toolbar_clear_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->select),
		      "any_signal::toggled",
		      G_CALLBACK(ags_notation_toolbar_select_callback),
		      notation_toolbar,
		      NULL);

  /* edit */
  g_object_disconnect(G_OBJECT(notation_toolbar->copy),
		      "any_signal::clicked",
		      G_CALLBACK(ags_notation_toolbar_copy_or_cut_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->cut),
		      "any_signal::clicked",
		      G_CALLBACK(ags_notation_toolbar_copy_or_cut_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->paste_tool),
		      "any_signal::clicked",
		      G_CALLBACK(ags_notation_toolbar_paste_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->invert),
		      "any_signal::clicked",
		      G_CALLBACK(ags_notation_toolbar_invert_callback),
		      notation_toolbar,
		      NULL);

  list = gtk_container_get_children((GtkContainer *) gtk_menu_tool_button_get_menu(notation_toolbar->paste_tool));

  g_object_disconnect(G_OBJECT(list->data),
		      "any_signal::activate",
		      G_CALLBACK(ags_notation_toolbar_match_audio_channel_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(list->next->data),
		      "any_signal::activate",
		      G_CALLBACK(ags_notation_toolbar_no_duplicates_callback),
		      notation_toolbar,
		      NULL);

  g_list_free(list);

  /* additional tools */
  ags_connectable_disconnect(AGS_CONNECTABLE(notation_toolbar->position_notation_cursor));

  ags_connectable_disconnect(AGS_CONNECTABLE(notation_toolbar->crop_note));

  ags_connectable_disconnect(AGS_CONNECTABLE(notation_toolbar->move_note));

  ags_connectable_disconnect(AGS_CONNECTABLE(notation_toolbar->select_note));

  /* zoom */
  g_object_disconnect(G_OBJECT(notation_toolbar->zoom),
		      "any_signal::changed",
		      G_CALLBACK(ags_notation_toolbar_zoom_callback),
		      notation_toolbar,
		      NULL);


  /* opacity */
  g_object_disconnect(G_OBJECT(notation_toolbar->opacity),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_notation_toolbar_opacity_callback),
		      notation_toolbar,
		      NULL);
}

/**
 * ags_notation_toolbar_tool_popup_new:
 * @notation_toolbar: the #AgsNotationToolbar
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.0.0
 */
GtkMenu*
ags_notation_toolbar_tool_popup_new(AgsNotationToolbar *notation_toolbar)
{
  GtkMenu *tool_popup;
  GtkMenuItem *item;

  GList *list, *list_start;

  tool_popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("move notes"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("crop notes"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select notes"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  gtk_menu_shell_append((GtkMenuShell*) tool_popup,
			(GtkWidget*) gtk_separator_menu_item_new());
  
  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("enable all audio channels"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("disable all audio channels"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  /* connect */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) tool_popup);

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_notation_toolbar_tool_popup_move_note_callback), (gpointer) notation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_notation_toolbar_tool_popup_crop_note_callback), (gpointer) notation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_notation_toolbar_tool_popup_select_note_callback), (gpointer) notation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_notation_toolbar_tool_popup_position_cursor_callback), (gpointer) notation_toolbar);
  
  list = list->next->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_notation_toolbar_tool_popup_enable_all_lines_callback), (gpointer) notation_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_notation_toolbar_tool_popup_disable_all_lines_callback), (gpointer) notation_toolbar);
  
  g_list_free(list_start);

  /* show */
  gtk_widget_show_all((GtkWidget *) tool_popup);
  
  return(tool_popup);
}

/**
 * ags_notation_toolbar_new:
 *
 * Create a new #AgsNotationToolbar.
 *
 * Returns: a new #AgsNotationToolbar
 *
 * Since: 3.0.0
 */
AgsNotationToolbar*
ags_notation_toolbar_new()
{
  AgsNotationToolbar *notation_toolbar;

  notation_toolbar = (AgsNotationToolbar *) g_object_new(AGS_TYPE_NOTATION_TOOLBAR,
							 NULL);

  return(notation_toolbar);
}
