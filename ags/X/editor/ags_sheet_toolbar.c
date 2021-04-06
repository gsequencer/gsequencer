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

#include <ags/X/editor/ags_sheet_toolbar.h>
#include <ags/X/editor/ags_sheet_toolbar_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_menu_bar.h>

#include <ags/i18n.h>

void ags_sheet_toolbar_class_init(AgsSheetToolbarClass *sheet_toolbar);
void ags_sheet_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_sheet_toolbar_init(AgsSheetToolbar *sheet_toolbar);
void ags_sheet_toolbar_connect(AgsConnectable *connectable);
void ags_sheet_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_sheet_toolbar
 * @short_description: sheet_toolbar
 * @title: AgsSheetToolbar
 * @section_id:
 * @include: ags/X/editor/ags_sheet_toolbar.h
 *
 * The #AgsSheetToolbar lets you choose edit tool.
 */

GType
ags_sheet_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_sheet_toolbar = 0;

    static const GTypeInfo ags_sheet_toolbar_info = {
      sizeof (AgsSheetToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_sheet_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSheetToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_sheet_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_sheet_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_sheet_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
						    "AgsSheetToolbar", &ags_sheet_toolbar_info,
						    0);
    
    g_type_add_interface_static(ags_type_sheet_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_sheet_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_sheet_toolbar_class_init(AgsSheetToolbarClass *sheet_toolbar)
{
}

void
ags_sheet_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_sheet_toolbar_connect;
  connectable->disconnect = ags_sheet_toolbar_disconnect;
}

void
ags_sheet_toolbar_init(AgsSheetToolbar *sheet_toolbar)
{
  GtkLabel *label;
  GtkMenu *menu;
  GtkMenuItem *item;

  sheet_toolbar->flags = 0;
  
  /* position */
  sheet_toolbar->position = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
								 "label", i18n("Position"),
								 "icon-name", "go-jump",
								 NULL);
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->position);

  /* edit */
  sheet_toolbar->edit_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
								"icon-name", "format-text-direction-ltr",
								"active", TRUE,
								NULL);
  
  menu = (GtkMenu *) gtk_menu_new();

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("1/1"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);
  
  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("1/2"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("1/4"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("1/8"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("1/16"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);

  item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
		      "label", i18n("1/32"),
		      "active", TRUE,
		      NULL);
  gtk_menu_shell_append((GtkMenuShell *) menu,
			(GtkWidget *) item);
  
  gtk_menu_tool_button_set_menu(sheet_toolbar->edit_tool,
				(GtkWidget *) menu);
  gtk_widget_show_all((GtkWidget *) menu);
  
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->edit_tool);
  
  /* clear */
  sheet_toolbar->clear = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
							      "icon-name", "edit-clear",
							      NULL);
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->clear);

  /* select */
  sheet_toolbar->select = (GtkToggleToolButton *) g_object_new(GTK_TYPE_TOGGLE_TOOL_BUTTON,
							       "label", i18n("Select"),
							       "icon-name", "edit-select-all",
							       NULL);
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->select);

  /* copy */
  sheet_toolbar->copy = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
						       "icon-name", "edit-copy",
						       NULL);
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->copy);

  /* cut */
  sheet_toolbar->cut = (GtkToolButton *) g_object_new(GTK_TYPE_TOOL_BUTTON,
						      "icon-name", "edit-cut",
						      NULL);
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->cut);

  /* paste */
  sheet_toolbar->paste_tool = (GtkMenuToolButton *) g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
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

  gtk_menu_tool_button_set_menu(sheet_toolbar->paste_tool,
				(GtkWidget *) menu);
  gtk_widget_show_all((GtkWidget *) menu);
  
  gtk_container_add((GtkToolbar *) sheet_toolbar,
		    (GtkWidget *) sheet_toolbar->paste_tool);
}

void
ags_sheet_toolbar_connect(AgsConnectable *connectable)
{
  AgsWindow *window;
  AgsSheetToolbar *sheet_toolbar;

  GList *list;
  
  sheet_toolbar = AGS_SHEET_TOOLBAR(connectable);

  if((AGS_SHEET_TOOLBAR_CONNECTED & (sheet_toolbar->flags)) != 0){
    return;
  }

  sheet_toolbar->flags |= AGS_SHEET_TOOLBAR_CONNECTED;

  //TODO:JK: implement me
}

void
ags_sheet_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsSheetToolbar *sheet_toolbar;

  GList *list;
  
  sheet_toolbar = AGS_SHEET_TOOLBAR(connectable);

  if((AGS_SHEET_TOOLBAR_CONNECTED & (sheet_toolbar->flags)) == 0){
    return;
  }

  sheet_toolbar->flags &= (~AGS_SHEET_TOOLBAR_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_sheet_toolbar_new:
 *
 * Create a new #AgsSheetToolbar.
 *
 * Returns: a new #AgsSheetToolbar
 *
 * Since: 3.0.0
 */
AgsSheetToolbar*
ags_sheet_toolbar_new()
{
  AgsSheetToolbar *sheet_toolbar;

  sheet_toolbar = (AgsSheetToolbar *) g_object_new(AGS_TYPE_SHEET_TOOLBAR,
						   NULL);

  return(sheet_toolbar);
}
