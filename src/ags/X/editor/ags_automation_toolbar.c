/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/editor/ags_automation_toolbar.h>
#include <ags/X/editor/ags_automation_toolbar_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

void ags_automation_toolbar_class_init(AgsAutomationToolbarClass *automation_toolbar);
void ags_automation_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_automation_toolbar_init(AgsAutomationToolbar *automation_toolbar);
void ags_automation_toolbar_connect(AgsConnectable *connectable);
void ags_automation_toolbar_disconnect(AgsConnectable *connectable);
void ags_automation_toolbar_show(GtkWidget *widget);

/**
 * SECTION:ags_automation_toolbar
 * @short_description: edit tool
 * @title: AgsAutomationToolbar
 * @section_id:
 * @include: ags/X/editor/ags_note_edit.h
 *
 * The #AgsAutomationToolbar lets you choose edit tool.
 */

GType
ags_automation_toolbar_get_type(void)
{
  static GType ags_type_automation_toolbar = 0;

  if (!ags_type_automation_toolbar){
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
							 "AgsAutomationToolbar\0", &ags_automation_toolbar_info,
							 0);
    
    g_type_add_interface_static(ags_type_automation_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_automation_toolbar);
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
  GtkMenuToolButton *menu_tool_button;
  GtkMenu *menu;
  GtkLabel *label;

  automation_toolbar->position = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					      "label\0", "position\0",
					      "image\0", gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
										  GTK_ICON_SIZE_LARGE_TOOLBAR),
					      NULL);
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    GTK_WIDGET(automation_toolbar->position));

  automation_toolbar->edit = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					  "label\0", "edit\0",
					  "image\0", gtk_image_new_from_stock(GTK_STOCK_JUMP_TO,
										  GTK_ICON_SIZE_LARGE_TOOLBAR),
					  NULL);
  automation_toolbar->selected_edit_mode = automation_toolbar->edit;
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    GTK_WIDGET(automation_toolbar->edit));

  automation_toolbar->clear = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					   "label\0", "clear\0",
					   "image\0", gtk_image_new_from_stock(GTK_STOCK_CLEAR,
									       GTK_ICON_SIZE_LARGE_TOOLBAR),
					   NULL);
  
  automation_toolbar->select = g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					    "label\0", "select\0",
					    "image\0", gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL,
										GTK_ICON_SIZE_LARGE_TOOLBAR),
					    NULL);
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    GTK_WIDGET(automation_toolbar->select));

  menu_tool_button = gtk_menu_tool_button_new(gtk_image_new_from_stock(GTK_STOCK_EXECUTE,
								       GTK_ICON_SIZE_BUTTON),
					      NULL);
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    GTK_WIDGET(menu_tool_button));

  menu = g_object_new(GTK_TYPE_MENU,
		      NULL);
  gtk_menu_tool_button_set_menu(menu_tool_button,
				menu);

  automation_toolbar->copy = g_object_new(GTK_TYPE_MENU_ITEM,
					  "label\0", "copy\0",
					  NULL);
  gtk_menu_shell_append(menu,
			automation_toolbar->copy);

  automation_toolbar->cut = g_object_new(GTK_TYPE_MENU_ITEM,
					 "label\0", "cut\0",
					 NULL);
  gtk_menu_shell_append(menu,
			automation_toolbar->cut);

  automation_toolbar->paste = g_object_new(GTK_TYPE_MENU_ITEM,
					   "label\0", "paste\0",
					   NULL);
  gtk_menu_shell_append(menu,
			automation_toolbar->paste);

  label = gtk_label_new("zoom\0");
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    label);
  
  automation_toolbar->zoom = gtk_spin_button_new_with_range(0.0,
							    800.0,
							    0.25);
  gtk_container_add(GTK_CONTAINER(automation_toolbar),
		    automation_toolbar->zoom);
}

void
ags_automation_toolbar_connect(AgsConnectable *connectable)
{
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = AGS_AUTOMATION_TOOLBAR(connectable);
}

void
ags_automation_toolbar_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_automation_toolbar_new:
 *
 * Create a new #AgsAutomationToolbar.
 *
 * Since: 0.4
 */
AgsAutomationToolbar*
ags_automation_toolbar_new()
{
  AgsAutomationToolbar *automation_toolbar;

  automation_toolbar = (AgsAutomationToolbar *) g_object_new(AGS_TYPE_AUTOMATION_TOOLBAR, NULL);

  return(automation_toolbar);
}
