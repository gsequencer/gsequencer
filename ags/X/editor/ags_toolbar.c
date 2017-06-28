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

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_toolbar_callbacks.h>
#include <ags/X/editor/ags_toolbar_mode_stock.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_menu_bar.h>

#include <ags/X/editor/ags_move_note_dialog.h>
#include <ags/X/editor/ags_crop_note_dialog.h>

#include <gtk/gtkstock.h>

#include <ags/i18n.h>

void ags_toolbar_class_init(AgsToolbarClass *toolbar);
void ags_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_toolbar_init(AgsToolbar *toolbar);
void ags_toolbar_connect(AgsConnectable *connectable);
void ags_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_toolbar
 * @short_description: edit tool
 * @title: AgsToolbar
 * @section_id:
 * @include: ags/X/editor/ags_toolbar.h
 *
 * The #AgsToolbar lets you choose edit tool.
 */

GType
ags_toolbar_get_type(void)
{
  static GType ags_type_toolbar = 0;

  if (!ags_type_toolbar){
    static const GTypeInfo ags_toolbar_info = {
      sizeof (AgsToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
					      "AgsToolbar", &ags_toolbar_info,
					      0);
    
    g_type_add_interface_static(ags_type_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_toolbar);
}

void
ags_toolbar_class_init(AgsToolbarClass *toolbar)
{
}

void
ags_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_toolbar_connect;
  connectable->disconnect = ags_toolbar_disconnect;
}

void
ags_toolbar_init(AgsToolbar *toolbar)
{
  GtkLabel *label;
  GtkMenu *menu;
  GtkMenuItem *item;

  toolbar->flags = 0;
  
  /* position */
  toolbar->position = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						       "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
												       GTK_ICON_SIZE_LARGE_TOOLBAR),
						       "relief", GTK_RELIEF_NONE,
						       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->position, i18n("position cursor"), NULL);

  /* edit */
  toolbar->edit = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						   "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_EDIT,
												   GTK_ICON_SIZE_LARGE_TOOLBAR),
						   "relief", GTK_RELIEF_NONE,
						   "active", TRUE,
						   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->edit, i18n("edit notes"), NULL);
  toolbar->selected_edit_mode = toolbar->edit;

  /* clear */
  toolbar->clear = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						    "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CLEAR,
												    GTK_ICON_SIZE_LARGE_TOOLBAR),
						    "relief", GTK_RELIEF_NONE,
						    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->clear, i18n("clear notes"), NULL);

  /* select */
  toolbar->select = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						     "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL,
												     GTK_ICON_SIZE_LARGE_TOOLBAR),
						     "relief", GTK_RELIEF_NONE,
						     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar,
			    (GtkWidget *) toolbar->select,
			    i18n("select notes"),
			    NULL);

  /* copy */
  toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					     "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_COPY,
											     GTK_ICON_SIZE_LARGE_TOOLBAR),
					     "relief", GTK_RELIEF_NONE,
					     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar,
			    (GtkWidget *) toolbar->copy,
			    i18n("copy notes"),
			    NULL);

  /* cut */
  toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					    "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CUT,
											    GTK_ICON_SIZE_LARGE_TOOLBAR),
					    "relief", GTK_RELIEF_NONE,
					    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->cut, i18n("cut notes"), NULL);

  /* paste */
  toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					      "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_PASTE,
											      GTK_ICON_SIZE_LARGE_TOOLBAR),
					      "relief", GTK_RELIEF_NONE,
					      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->paste, i18n("paste notes"), NULL);

  /* invert */
  toolbar->invert = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					       "image", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CONVERT,
											       GTK_ICON_SIZE_LARGE_TOOLBAR),
					       "relief", GTK_RELIEF_NONE,
					       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->invert, i18n("invert notes"), NULL);

  /* menu tool */
  toolbar->menu_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
							  "label", i18n("tool"),
							  "stock-id", GTK_STOCK_EXECUTE,
							  NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->menu_tool, i18n("additional tools"), NULL);

  /* menu tool - tool popup */
  toolbar->tool_popup = ags_toolbar_tool_popup_new(toolbar);
  gtk_menu_tool_button_set_menu(toolbar->menu_tool,
				toolbar->tool_popup);

  /* menu tool - dialogs */
  toolbar->move_note = ags_move_note_dialog_new(NULL);
  toolbar->crop_note = ags_crop_note_dialog_new(NULL);
  
  /* zoom */
  label = (GtkLabel *) gtk_label_new(i18n("zoom"));
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  toolbar->zoom_history = 2;
  toolbar->zoom = (GtkComboBoxText *) ags_zoom_combo_box_new();
  gtk_combo_box_set_active((GtkComboBox *) toolbar->zoom, 2);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->zoom, NULL , NULL);

  /* edit modes */
  label = (GtkLabel *) gtk_label_new(i18n("mode"));
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  //TODO:JK: uncomment me
  toolbar->mode = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(toolbar->mode,
				 AGS_TOOLBAR_MODE_SINGLE_CHANNEL);
  //  gtk_combo_box_text_append_text(toolbar->mode,
  //				 AGS_TOOLBAR_MODE_MULTI_CHANNEL);
  //  gtk_combo_box_text_append_text(toolbar->mode,
  //				 AGS_TOOLBAR_MODE_ALL_CHANNELS);
  gtk_combo_box_set_active((GtkComboBox *) toolbar->mode,
			   0);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->mode, NULL, NULL);
}

void
ags_toolbar_connect(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;

  toolbar = AGS_TOOLBAR(connectable);

  if((AGS_TOOLBAR_CONNECTED & (toolbar->flags)) != 0){
    return;
  }

  toolbar->flags |= AGS_TOOLBAR_CONNECTED;
  
  /* tool */
  g_signal_connect_after((GObject *) toolbar->position, "toggled",
			 G_CALLBACK(ags_toolbar_position_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->edit, "toggled",
			 G_CALLBACK(ags_toolbar_edit_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->clear, "toggled",
			 G_CALLBACK(ags_toolbar_clear_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->select, "toggled",
			 G_CALLBACK(ags_toolbar_select_callback), (gpointer) toolbar);

  /* edit */
  g_signal_connect((GObject *) toolbar->copy, "clicked",
		   G_CALLBACK(ags_toolbar_copy_or_cut_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->cut, "clicked",
		   G_CALLBACK(ags_toolbar_copy_or_cut_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->paste, "clicked",
		   G_CALLBACK(ags_toolbar_paste_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->invert, "clicked",
		   G_CALLBACK(ags_toolbar_invert_callback), (gpointer) toolbar);

  /* zoom */
  g_signal_connect_after((GObject *) toolbar->zoom, "changed",
			 G_CALLBACK(ags_toolbar_zoom_callback), (gpointer) toolbar);

  /* mode */
  g_signal_connect_after((GObject *) toolbar->mode, "changed",
			 G_CALLBACK(ags_toolbar_mode_callback), (gpointer) toolbar);
}

void
ags_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;

  toolbar = AGS_TOOLBAR(connectable);

  if((AGS_TOOLBAR_CONNECTED & (toolbar->flags)) == 0){
    return;
  }

  toolbar->flags &= (~AGS_TOOLBAR_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_toolbar_tool_popup_new:
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 0.8.8
 */
GtkMenu*
ags_toolbar_tool_popup_new(GtkToolbar *toolbar)
{
  GtkMenu *tool_popup;
  GtkMenuItem *item;

  GList *list, *list_start;

  tool_popup = (GtkMenu *) gtk_menu_new();

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("move notes"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("crop notes"));
  gtk_menu_shell_append((GtkMenuShell *) tool_popup, (GtkWidget *) item);

  /* connect */
  list_start = 
    list = gtk_container_get_children((GtkContainer *) tool_popup);

  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_toolbar_tool_popup_move_note_callback), (gpointer) toolbar);

  list = list->next;
  g_signal_connect(G_OBJECT(list->data), "activate",
		   G_CALLBACK(ags_toolbar_tool_popup_crop_note_callback), (gpointer) toolbar);

  g_list_free(list_start);

  /* show */
  gtk_widget_show_all((GtkWidget *) tool_popup);
  
  return(tool_popup);
}

/**
 * ags_toolbar_new:
 *
 * Create a new #AgsToolbar.
 *
 * Returns: a new #AgsToolbar
 *
 * Since: 0.3
 */
AgsToolbar*
ags_toolbar_new()
{
  AgsToolbar *toolbar;

  toolbar = (AgsToolbar *) g_object_new(AGS_TYPE_TOOLBAR, NULL);

  return(toolbar);
}
