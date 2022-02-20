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

#include <ags/app/editor/ags_wave_toolbar.h>
#include <ags/app/editor/ags_wave_toolbar_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_wave_window.h>
#include <ags/app/ags_menu_bar.h>
#include <ags/app/ags_wave_editor.h>

#include <ags/app/editor/ags_select_buffer_dialog.h>
#include <ags/app/editor/ags_position_wave_cursor_dialog.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_wave_toolbar_class_init(AgsWaveToolbarClass *wave_toolbar);
void ags_wave_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wave_toolbar_init(AgsWaveToolbar *wave_toolbar);
void ags_wave_toolbar_connect(AgsConnectable *connectable);
void ags_wave_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_wave_toolbar
 * @short_description: wave toolbar
 * @title: AgsWaveToolbar
 * @section_id:
 * @include: ags/app/editor/ags_wave_toolbar.h
 *
 * The #AgsWaveToolbar lets you choose edit tool.
 */

GType
ags_wave_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wave_toolbar = 0;

    static const GTypeInfo ags_wave_toolbar_info = {
      sizeof (AgsWaveToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wave_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsWaveToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wave_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wave_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wave_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
						   "AgsWaveToolbar", &ags_wave_toolbar_info,
						   0);
    
    g_type_add_interface_static(ags_type_wave_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wave_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_wave_toolbar_class_init(AgsWaveToolbarClass *wave_toolbar)
{
}

void
ags_wave_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_wave_toolbar_connect;
  connectable->disconnect = ags_wave_toolbar_disconnect;
}

void
ags_wave_toolbar_init(AgsWaveToolbar *wave_toolbar)
{
  GtkToolItem *tool_item;
  GtkMenuToolButton *menu_tool_button;
  GtkMenu *menu;
  GtkMenuItem *item;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;
  GtkHBox *hbox;

  wave_toolbar->flags = 0;

  wave_toolbar->selected_edit_mode = NULL;

  /* position */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  wave_toolbar->position = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
								"label", i18n("Position"),
								"icon-name", "go-jump",
								NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) wave_toolbar->position);
  
  /* select */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  wave_toolbar->select = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
							      "label", i18n("Select"),
							      "icon-name", "edit-select-all",
							      NULL);
  gtk_container_add((GtkToolbar *) wave_toolbar,
		    (GtkWidget *) wave_toolbar->select);

  /* copy */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  wave_toolbar->copy = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
						      "label", i18n("Copy"),							  
						      "icon-name", "edit-copy",
						      NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) wave_toolbar->copy);

  /* cut */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  wave_toolbar->cut = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
						     "label", i18n("Cut"),
						     "icon-name", "edit-cut",
						     NULL);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) wave_toolbar->cut);

  /* paste */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  wave_toolbar->paste_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								"label", i18n("Paste"),
								"icon-name", "edit-paste",
								NULL);

  menu = (GtkMenu *) gtk_menu_new();

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", "match line",
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  gtk_menu_tool_button_set_menu(wave_toolbar->paste_tool,
				(GtkWidget *) menu);
  gtk_widget_show_all((GtkWidget *) menu);

  
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) wave_toolbar->paste_tool);
  
  /* menu tool */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  wave_toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
							       "label", i18n("Tool"),
							       "icon-name", "system-run",
							       NULL);

  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) wave_toolbar->menu_tool);

  /* menu tool - tool popup */
  wave_toolbar->tool_popup = ags_wave_toolbar_tool_popup_new(wave_toolbar);
  gtk_menu_tool_button_set_menu(wave_toolbar->menu_tool,
				(GtkWidget *) wave_toolbar->tool_popup);

  /* menu tool - dialogs */
  wave_toolbar->select_buffer = (GtkDialog *) ags_select_buffer_dialog_new(NULL);
  wave_toolbar->position_wave_cursor = (GtkDialog *) ags_position_wave_cursor_dialog_new(NULL);

  /* zoom */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
		     (GtkWidget *) tool_item,
		     -1);

  hbox = gtk_hbox_new(FALSE,
		      0);
  gtk_container_add((GtkContainer *) tool_item,
		    (GtkWidget *) hbox);

  wave_toolbar->zoom_history = 2;
  
  label = (GtkLabel *) gtk_label_new(i18n("Zoom"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  wave_toolbar->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(GTK_COMBO_BOX(wave_toolbar->zoom),
			   2);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) wave_toolbar->zoom,
		     FALSE, FALSE,
		     0);

  /* opacity */
  tool_item = gtk_tool_item_new();
  gtk_toolbar_insert((GtkToolbar *) wave_toolbar,
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

  wave_toolbar->opacity = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.001);
  gtk_spin_button_set_value(wave_toolbar->opacity, 0.5);
  gtk_box_pack_start(hbox,
		     (GtkWidget *) wave_toolbar->opacity,
		     FALSE, FALSE,
		     0);
}

void
ags_wave_toolbar_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsWaveWindow *wave_window;
  AgsWaveToolbar *wave_toolbar;

  GList *list;

  wave_toolbar = AGS_WAVE_TOOLBAR(connectable);

  if((AGS_WAVE_TOOLBAR_CONNECTED & (wave_toolbar->flags)) != 0){
    return;
  }

  wave_toolbar->flags |= AGS_WAVE_TOOLBAR_CONNECTED;

  wave_window = (AgsWaveWindow *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
								      AGS_TYPE_WAVE_WINDOW);
  window = (AgsWindow *) wave_window->parent_window;
  
  g_object_set(wave_toolbar->select_buffer,
	       "main-window", window,
	       NULL);
  g_object_set(wave_toolbar->position_wave_cursor,
	       "main-window", window,
	       NULL);

  /* tool */
  g_signal_connect_after((GObject *) wave_toolbar->position, "toggled",
			 G_CALLBACK(ags_wave_toolbar_position_callback), (gpointer) wave_toolbar);

  g_signal_connect_after((GObject *) wave_toolbar->select, "toggled",
			 G_CALLBACK(ags_wave_toolbar_select_callback), (gpointer) wave_toolbar);

  /* edit */
  g_signal_connect((GObject *) wave_toolbar->copy, "clicked",
		   G_CALLBACK(ags_wave_toolbar_copy_or_cut_callback), (gpointer) wave_toolbar);

  g_signal_connect((GObject *) wave_toolbar->cut, "clicked",
		   G_CALLBACK(ags_wave_toolbar_copy_or_cut_callback), (gpointer) wave_toolbar);

  g_signal_connect((GObject *) wave_toolbar->paste_tool, "clicked",
		   G_CALLBACK(ags_wave_toolbar_paste_callback), (gpointer) wave_toolbar);

  list = gtk_container_get_children((GtkContainer *) gtk_menu_tool_button_get_menu(wave_toolbar->paste_tool));

  g_signal_connect_after(list->data, "activate",
			 G_CALLBACK(ags_wave_toolbar_match_line_callback), wave_toolbar);

  g_list_free(list);

  /* additional tools */
  ags_connectable_connect(AGS_CONNECTABLE(wave_toolbar->select_buffer));

  ags_connectable_connect(AGS_CONNECTABLE(wave_toolbar->position_wave_cursor));

  /* zoom */
  g_signal_connect_after((GObject *) wave_toolbar->zoom, "changed",
			 G_CALLBACK(ags_wave_toolbar_zoom_callback), (gpointer) wave_toolbar);

  /* opacity */
  g_signal_connect_after((GObject *) wave_toolbar->opacity, "value-changed",
			 G_CALLBACK(ags_wave_toolbar_opacity_callback), (gpointer) wave_toolbar);
}

void
ags_wave_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsWaveToolbar *wave_toolbar;

  GList *list;

  wave_toolbar = AGS_WAVE_TOOLBAR(connectable);

  if((AGS_WAVE_TOOLBAR_CONNECTED & (wave_toolbar->flags)) == 0){
    return;
  }

  wave_toolbar->flags &= (~AGS_WAVE_TOOLBAR_CONNECTED);

  /* tool */
  g_object_disconnect(G_OBJECT(wave_toolbar->position),
		      "any_signal::toggled",
		      G_CALLBACK(ags_wave_toolbar_position_callback),
		      wave_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_toolbar->select),
		      "any_signal::toggled",
		      G_CALLBACK(ags_wave_toolbar_select_callback),
		      wave_toolbar,
		      NULL);

  /* edit */
  g_object_disconnect(G_OBJECT(wave_toolbar->copy),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_toolbar_copy_or_cut_callback),
		      wave_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_toolbar->cut),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_toolbar_copy_or_cut_callback),
		      wave_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_toolbar->paste_tool),
		      "any_signal::clicked",
		      G_CALLBACK(ags_wave_toolbar_paste_callback),
		      wave_toolbar,
		      NULL);
  
  list = gtk_container_get_children((GtkContainer *) gtk_menu_tool_button_get_menu(wave_toolbar->paste_tool));

  g_object_disconnect(G_OBJECT(list->data),
		      "any_signal::activate",
		      G_CALLBACK(ags_wave_toolbar_match_line_callback),
		      wave_toolbar,
		      NULL);

  g_list_free(list);

  /* additional tools */
  ags_connectable_disconnect(AGS_CONNECTABLE(wave_toolbar->select_buffer));

  ags_connectable_disconnect(AGS_CONNECTABLE(wave_toolbar->position_wave_cursor));

  /* zoom */
  g_object_disconnect(G_OBJECT(wave_toolbar->zoom),
		      "any_signal::changed",
		      G_CALLBACK(ags_wave_toolbar_zoom_callback),
		      wave_toolbar,
		      NULL);

  /* opacity */
  g_object_disconnect(G_OBJECT(wave_toolbar->opacity),
		      "any_signal::value-changed",
		      G_CALLBACK(ags_wave_toolbar_opacity_callback),
		      wave_toolbar,
		      NULL);
}

/**
 * ags_wave_toolbar_tool_popup_new:
 * @wave_toolbar: the #AgsWaveToolbar
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.0.0
 */
GtkMenu*
ags_wave_toolbar_tool_popup_new(AgsWaveToolbar *wave_toolbar)
{
  GtkMenu *tool_popup;
  GtkMenuItem *item;

  GList *list, *list_start;

  tool_popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select audio data"));
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
		   G_CALLBACK(ags_wave_toolbar_tool_popup_select_buffer_callback), (gpointer) wave_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_wave_toolbar_tool_popup_position_cursor_callback), (gpointer) wave_toolbar);

  list = list->next->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_wave_toolbar_tool_popup_enable_all_lines_callback), (gpointer) wave_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_wave_toolbar_tool_popup_disable_all_lines_callback), (gpointer) wave_toolbar);

  g_list_free(list_start);

  /* show */
  gtk_widget_show_all((GtkWidget *) tool_popup);
  
  return(tool_popup);
}

/**
 * ags_wave_toolbar_new:
 *
 * Create a new #AgsWaveToolbar.
 *
 * Returns: a new #AgsWaveToolbar
 *
 * Since: 3.0.0
 */
AgsWaveToolbar*
ags_wave_toolbar_new()
{
  AgsWaveToolbar *wave_toolbar;

  wave_toolbar = (AgsWaveToolbar *) g_object_new(AGS_TYPE_WAVE_TOOLBAR, NULL);

  return(wave_toolbar);
}
