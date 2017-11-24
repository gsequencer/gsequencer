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

#include <ags/X/editor/ags_wave_toolbar.h>
#include <ags/X/editor/ags_wave_toolbar_callbacks.h>

#include <ags/lib/ags_string_util.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_wave_window.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_wave_editor.h>

#include <ags/X/editor/ags_select_audio_data_dialog.h>
#include <ags/X/editor/ags_position_wave_cursor_dialog.h>

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
 * @include: ags/X/editor/ags_wave_toolbar.h
 *
 * The #AgsWaveToolbar lets you choose edit tool.
 */

GType
ags_wave_toolbar_get_type(void)
{
  static GType ags_type_wave_toolbar = 0;

  if (!ags_type_wave_toolbar){
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
  }

  return (ags_type_wave_toolbar);
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
  GtkMenuToolButton *menu_tool_button;
  GtkMenu *menu;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;
  
  wave_toolbar->position = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					"image", gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
									  GTK_ICON_SIZE_LARGE_TOOLBAR),
					"relief", GTK_RELIEF_NONE,
					NULL);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->position,
			    "position cursor",
			    NULL);
  
  wave_toolbar->select = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
				      "image", gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL,
									GTK_ICON_SIZE_LARGE_TOOLBAR),
				      "relief", GTK_RELIEF_NONE,
				      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->select,
			    "select wave",
			    NULL);

  wave_toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						  "image", gtk_image_new_from_stock(GTK_STOCK_COPY,
										    GTK_ICON_SIZE_LARGE_TOOLBAR),
						  "relief", GTK_RELIEF_NONE,
						  NULL);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->copy,
			    "copy wave",
			    NULL);

  wave_toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						 "image", gtk_image_new_from_stock(GTK_STOCK_CUT,
										   GTK_ICON_SIZE_LARGE_TOOLBAR),
						 "relief", GTK_RELIEF_NONE,
						 NULL);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->cut,
			    "cut wave",
			    NULL);

  wave_toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						   "image", gtk_image_new_from_stock(GTK_STOCK_PASTE,
										     GTK_ICON_SIZE_LARGE_TOOLBAR),
						   "relief", GTK_RELIEF_NONE,
						   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->paste,
			    "paste wave",
			    NULL);
  
  /* menu tool */
  wave_toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
							       "label", i18n("tool"),
							       "stock-id", GTK_STOCK_EXECUTE,
							       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar, (GtkWidget *) wave_toolbar->menu_tool, i18n("additional tools"), NULL);

  /* menu tool - tool popup */
  wave_toolbar->tool_popup = ags_wave_toolbar_tool_popup_new(wave_toolbar);
  gtk_menu_tool_button_set_menu(wave_toolbar->menu_tool,
				wave_toolbar->tool_popup);

  /* menu tool - dialogs */
  wave_toolbar->select_acceleration = ags_select_audio_data_dialog_new(NULL);
  wave_toolbar->position_wave_cursor = ags_position_wave_cursor_dialog_new(NULL);

  /*  */
  wave_toolbar->zoom_history = 4;
  
  label = (GtkLabel *) gtk_label_new(i18n("zoom"));
  gtk_container_add(GTK_CONTAINER(wave_toolbar),
		    (GtkWidget *) label);

  wave_toolbar->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(GTK_COMBO_BOX(wave_toolbar->zoom),
			   2);
  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->zoom,
			    NULL,
			    NULL);

  /*  */
  label = (GtkLabel *) gtk_label_new(i18n("port"));
  gtk_container_add(GTK_CONTAINER(wave_toolbar),
		    (GtkWidget *) label);

  wave_toolbar->port = (GtkComboBox *) gtk_combo_box_new();

  cell_renderer = gtk_cell_renderer_toggle_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(wave_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(wave_toolbar->port), cell_renderer,
				 "active", 0,
				 NULL);
  gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(cell_renderer),
					   TRUE);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(wave_toolbar->port),
			     cell_renderer,
			     FALSE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(wave_toolbar->port), cell_renderer,
				 "text", 1,
				 NULL);

  gtk_toolbar_append_widget((GtkToolbar *) wave_toolbar,
			    (GtkWidget *) wave_toolbar->port,
			    NULL,
			    NULL);
}

void
ags_wave_toolbar_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsWaveWindow *wave_window;
  AgsWaveEditor *wave_editor;
  AgsWaveToolbar *wave_toolbar;

  wave_toolbar = AGS_WAVE_TOOLBAR(connectable);

  if((AGS_WAVE_TOOLBAR_CONNECTED & (wave_toolbar->flags)) != 0){
    return;
  }

  wave_toolbar->flags |= AGS_WAVE_TOOLBAR_CONNECTED;
  
  wave_editor = (AgsWaveEditor *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_EDITOR);

  wave_window = (AgsWaveWindow *) gtk_widget_get_ancestor((GtkWidget *) wave_toolbar,
							  AGS_TYPE_WAVE_WINDOW);
  window = wave_window->parent_window;
  
  g_object_set(wave_toolbar->select_acceleration,
	       "main-window", window,
	       NULL);
  g_object_set(wave_toolbar->position_wave_cursor,
	       "main-window", window,
	       NULL);

  /*  */
  g_signal_connect_after(G_OBJECT(wave_editor), "machine-changed",
			 G_CALLBACK(ags_wave_toolbar_machine_changed_callback), wave_toolbar);

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

  g_signal_connect((GObject *) wave_toolbar->paste, "clicked",
		   G_CALLBACK(ags_wave_toolbar_paste_callback), (gpointer) wave_toolbar);

  /* additional tools */
  ags_connectable_connect(AGS_CONNECTABLE(wave_toolbar->select_audio_data));

  ags_connectable_connect(AGS_CONNECTABLE(wave_toolbar->position_wave_cursor));

  /* zoom */
  g_signal_connect_after((GObject *) wave_toolbar->zoom, "changed",
			 G_CALLBACK(ags_wave_toolbar_zoom_callback), (gpointer) wave_toolbar);
}

void
ags_wave_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsWaveToolbar *wave_toolbar;

  wave_toolbar = AGS_WAVE_TOOLBAR(connectable);

  if((AGS_WAVE_TOOLBAR_CONNECTED & (wave_toolbar->flags)) == 0){
    return;
  }

  wave_toolbar->flags &= (~AGS_WAVE_TOOLBAR_CONNECTED);

  /* tool */
  g_object_disconnect(G_OBJECT(wave_toolbar->position),
		      "toggled",
		      G_CALLBACK(ags_wave_toolbar_position_callback),
		      wave_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_toolbar->select),
		      "toggled",
		      G_CALLBACK(ags_wave_toolbar_select_callback),
		      wave_toolbar,
		      NULL);

  /* edit */
  g_object_disconnect(G_OBJECT(wave_toolbar->copy),
		      "clicked",
		      G_CALLBACK(ags_wave_toolbar_copy_or_cut_callback),
		      wave_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_toolbar->cut),
		      "clicked",
		      G_CALLBACK(ags_wave_toolbar_copy_or_cut_callback),
		      wave_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(wave_toolbar->paste),
		      "clicked",
		      G_CALLBACK(ags_wave_toolbar_paste_callback),
		      wave_toolbar,
		      NULL);
  
  /* additional tools */
  ags_connectable_disconnect(AGS_CONNECTABLE(wave_toolbar->select_audio_data));

  ags_connectable_disconnect(AGS_CONNECTABLE(wave_toolbar->position_wave_cursor));

  /* zoom */
  g_object_disconnect(G_OBJECT(wave_toolbar->zoom),
		      "changed",
		      G_CALLBACK(ags_wave_toolbar_zoom_callback),
		      wave_toolbar,
		      NULL);
}

/**
 * ags_wave_toolbar_tool_popup_new:
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 1.2.0
 */
GtkMenu*
ags_wave_toolbar_tool_popup_new(GtkToolbar *wave_toolbar)
{
  GtkMenu *tool_popup;
  GtkMenuItem *item;

  GList *list, *list_start;

  tool_popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select audio data"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  /* connect */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) tool_popup);

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_wave_toolbar_tool_popup_select_acceleration_callback), (gpointer) wave_toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_wave_toolbar_tool_popup_position_cursor_callback), (gpointer) wave_toolbar);

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
 * Since: 1.2.0
 */
AgsWaveToolbar*
ags_wave_toolbar_new()
{
  AgsWaveToolbar *wave_toolbar;

  wave_toolbar = (AgsWaveToolbar *) g_object_new(AGS_TYPE_WAVE_TOOLBAR, NULL);

  return(wave_toolbar);
}
