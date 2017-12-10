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

#include <ags/X/editor/ags_notation_toolbar.h>
#include <ags/X/editor/ags_notation_toolbar_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_menu_bar.h>

#include <ags/X/editor/ags_move_note_dialog.h>
#include <ags/X/editor/ags_crop_note_dialog.h>
#include <ags/X/editor/ags_select_note_dialog.h>
#include <ags/X/editor/ags_position_notation_cursor_dialog.h>

#include <gtk/gtkstock.h>

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
  static GType ags_type_notation_toolbar = 0;

  if (!ags_type_notation_toolbar){
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
  }

  return (ags_type_notation_toolbar);
}

void
ags_notation_toolbar_class_init(AgsNotationToolbarClass *notation_toolbar)
{
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
  GtkLabel *label;
  GtkMenu *menu;
  GtkMenuItem *item;

  notation_toolbar->flags = 0;
  
  /* position */
  notation_toolbar->position = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
								"image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
														GTK_ICON_SIZE_LARGE_TOOLBAR),
								"relief", GTK_RELIEF_NONE,
								NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->position, i18n("position cursor"), NULL);

  /* edit */
  notation_toolbar->edit = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							    "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_EDIT,
													    GTK_ICON_SIZE_LARGE_TOOLBAR),
							    "relief", GTK_RELIEF_NONE,
							    "active", TRUE,
							    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->edit, i18n("edit notes"), NULL);
  notation_toolbar->selected_edit_mode = notation_toolbar->edit;

  /* clear */
  notation_toolbar->clear = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							     "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CLEAR,
													     GTK_ICON_SIZE_LARGE_TOOLBAR),
							     "relief", GTK_RELIEF_NONE,
							     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->clear, i18n("clear notes"), NULL);

  /* select */
  notation_toolbar->select = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
							      "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL,
													      GTK_ICON_SIZE_LARGE_TOOLBAR),
							      "relief", GTK_RELIEF_NONE,
							      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar,
			    (GtkWidget *) notation_toolbar->select,
			    i18n("select notes"),
			    NULL);

  /* copy */
  notation_toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						      "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_COPY,
												      GTK_ICON_SIZE_LARGE_TOOLBAR),
						      "relief", GTK_RELIEF_NONE,
						      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar,
			    (GtkWidget *) notation_toolbar->copy,
			    i18n("copy notes"),
			    NULL);

  /* cut */
  notation_toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						     "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CUT,
												     GTK_ICON_SIZE_LARGE_TOOLBAR),
						     "relief", GTK_RELIEF_NONE,
						     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->cut, i18n("cut notes"), NULL);

  /* paste */
  notation_toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
						       "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_PASTE,
												       GTK_ICON_SIZE_LARGE_TOOLBAR),
						       "relief", GTK_RELIEF_NONE,
						       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->paste, i18n("paste notes"), NULL);

  /* invert */
  notation_toolbar->invert = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							"image", (GtkWidget *) gtk_image_new_from_icon_name("object-flip-vertical",
													    GTK_ICON_SIZE_LARGE_TOOLBAR),
							"relief", GTK_RELIEF_NONE,
							NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->invert, i18n("invert notes"), NULL);

  /* menu tool */
  notation_toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								   "label", i18n("tool"),
								   "stock-id", GTK_STOCK_EXECUTE,
								   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->menu_tool, i18n("additional tools"), NULL);

  /* menu tool - tool popup */
  notation_toolbar->tool_popup = ags_notation_toolbar_tool_popup_new(notation_toolbar);
  gtk_menu_tool_button_set_menu(notation_toolbar->menu_tool,
				notation_toolbar->tool_popup);

  /* menu tool - dialogs */
  notation_toolbar->move_note = ags_move_note_dialog_new(NULL);
  notation_toolbar->crop_note = ags_crop_note_dialog_new(NULL);
  notation_toolbar->select_note = ags_select_note_dialog_new(NULL);
  notation_toolbar->position_notation_cursor = ags_position_notation_cursor_dialog_new(NULL);
  
  /* zoom */
  label = (GtkLabel *) gtk_label_new(i18n("zoom"));
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) label, NULL, NULL);

  notation_toolbar->zoom_history = 2;
  notation_toolbar->zoom = (GtkComboBoxText *) ags_zoom_combo_box_new();
  gtk_combo_box_set_active((GtkComboBox *) notation_toolbar->zoom, 2);
  gtk_toolbar_append_widget((GtkToolbar *) notation_toolbar, (GtkWidget *) notation_toolbar->zoom, NULL , NULL);
}

void
ags_notation_toolbar_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsNotationToolbar *notation_toolbar;

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

  g_signal_connect((GObject *) notation_toolbar->paste, "clicked",
		   G_CALLBACK(ags_notation_toolbar_paste_callback), (gpointer) notation_toolbar);

  g_signal_connect((GObject *) notation_toolbar->invert, "clicked",
		   G_CALLBACK(ags_notation_toolbar_invert_callback), (gpointer) notation_toolbar);

  /* additional tools */
  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->position_notation_cursor));

  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->crop_note));

  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->move_note));

  ags_connectable_connect(AGS_CONNECTABLE(notation_toolbar->select_note));

  /* zoom */
  g_signal_connect_after((GObject *) notation_toolbar->zoom, "changed",
			 G_CALLBACK(ags_notation_toolbar_zoom_callback), (gpointer) notation_toolbar);
}

void
ags_notation_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsNotationToolbar *notation_toolbar;

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

  g_object_disconnect(G_OBJECT(notation_toolbar->paste),
		      "any_signal::clicked",
		      G_CALLBACK(ags_notation_toolbar_paste_callback),
		      notation_toolbar,
		      NULL);

  g_object_disconnect(G_OBJECT(notation_toolbar->invert),
		      "any_signal::clicked",
		      G_CALLBACK(ags_notation_toolbar_invert_callback),
		      notation_toolbar,
		      NULL);

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
}

/**
 * ags_notation_toolbar_tool_popup_new:
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 1.0.0
 */
GtkMenu*
ags_notation_toolbar_tool_popup_new(GtkToolbar *notation_toolbar)
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
 * Since: 1.0.0
 */
AgsNotationToolbar*
ags_notation_toolbar_new()
{
  AgsNotationToolbar *notation_toolbar;

  notation_toolbar = (AgsNotationToolbar *) g_object_new(AGS_TYPE_NOTATION_TOOLBAR,
							 NULL);

  return(notation_toolbar);
}
