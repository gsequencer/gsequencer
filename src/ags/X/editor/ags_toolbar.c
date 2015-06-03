/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_toolbar_callbacks.h>
#include <ags/X/editor/ags_toolbar_mode_stock.h>

#include <ags/object/ags_connectable.h>

#include <ags/X/ags_menu_bar.h>

#include <gtk/gtkhbox.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkstock.h>

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
 * @include: ags/X/editor/ags_note_edit.h
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
					      "AgsToolbar\0", &ags_toolbar_info,
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

  toolbar->position = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						       "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_JUMP_TO, GTK_ICON_SIZE_LARGE_TOOLBAR),
						       "relief\0", GTK_RELIEF_NONE,
						       NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->position, "position cursor\0", NULL);

  toolbar->edit = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						   "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_LARGE_TOOLBAR),
						   "relief\0", GTK_RELIEF_NONE,
						   "active\0", TRUE,
						   NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->edit, "edit notes\0", NULL);
  toolbar->selected_edit_mode = toolbar->edit;

  toolbar->clear = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						    "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_LARGE_TOOLBAR),
						    "relief\0", GTK_RELIEF_NONE,
						    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->clear, "clear notes\0", NULL);

  toolbar->select = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
						     "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_SELECT_ALL, GTK_ICON_SIZE_LARGE_TOOLBAR),
						     "relief\0", GTK_RELIEF_NONE,
						     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->select, "select notes\0", NULL);

  toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					     "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_LARGE_TOOLBAR),
					     "relief\0", GTK_RELIEF_NONE,
					     NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->copy, "copy notes\0", NULL);

  toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					    "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_CUT, GTK_ICON_SIZE_LARGE_TOOLBAR),
					    "relief\0", GTK_RELIEF_NONE,
					    NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->cut, "cut notes\0", NULL);

  toolbar->paste = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
					      "image\0", (GtkWidget *) gtk_image_new_from_stock(GTK_STOCK_PASTE, GTK_ICON_SIZE_LARGE_TOOLBAR),
					      "relief\0", GTK_RELIEF_NONE,
					      NULL);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->paste, "paste notes\0", NULL);

  /* zoom */
  label = (GtkLabel *) gtk_label_new("zoom\0");
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  toolbar->zoom_history = 4;
  toolbar->zoom = ags_zoom_combo_box_new();
  gtk_combo_box_set_active(toolbar->zoom, 4);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->zoom, NULL , NULL);

  /* edit modes */
  label = (GtkLabel *) gtk_label_new("mode\0");
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) label, NULL, NULL);

  //TODO:JK: uncomment me
  toolbar->mode = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(toolbar->mode,
				 AGS_TOOLBAR_MODE_SINGLE_CHANNEL);
  //  gtk_combo_box_text_append_text(toolbar->mode,
  //				 AGS_TOOLBAR_MODE_MULTI_CHANNEL);
  //  gtk_combo_box_text_append_text(toolbar->mode,
  //				 AGS_TOOLBAR_MODE_ALL_CHANNELS);
  gtk_combo_box_set_active(toolbar->mode,
			   0);
  gtk_toolbar_append_widget((GtkToolbar *) toolbar, (GtkWidget *) toolbar->mode, NULL, NULL);
}

void
ags_toolbar_connect(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;

  toolbar = AGS_TOOLBAR(connectable);

  /* tool */
  g_signal_connect_after((GObject *) toolbar->position, "toggled\0",
			 G_CALLBACK(ags_toolbar_position_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->edit, "toggled\0",
			 G_CALLBACK(ags_toolbar_edit_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->clear, "toggled\0",
			 G_CALLBACK(ags_toolbar_clear_callback), (gpointer) toolbar);

  g_signal_connect_after((GObject *) toolbar->select, "toggled\0",
			 G_CALLBACK(ags_toolbar_select_callback), (gpointer) toolbar);

  /* edit */
  g_signal_connect((GObject *) toolbar->copy, "clicked\0",
		   G_CALLBACK(ags_toolbar_copy_or_cut_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->cut, "clicked\0",
		   G_CALLBACK(ags_toolbar_copy_or_cut_callback), (gpointer) toolbar);

  g_signal_connect((GObject *) toolbar->paste, "clicked\0",
		   G_CALLBACK(ags_toolbar_paste_callback), (gpointer) toolbar);

  /* zoom */
  g_signal_connect_after((GObject *) toolbar->zoom, "changed\0",
			 G_CALLBACK(ags_toolbar_zoom_callback), (gpointer) toolbar);

  /* mode */
  g_signal_connect_after((GObject *) toolbar->mode, "changed\0",
			 G_CALLBACK(ags_toolbar_mode_callback), (gpointer) toolbar);
}

void
ags_toolbar_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_toolbar_new:
 *
 * Create a new #AgsToolbar.
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
