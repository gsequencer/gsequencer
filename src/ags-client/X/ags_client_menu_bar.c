/* AGS Client - Advanced GTK Sequencer Client
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags-client/X/ags_client_menu_bar.h>

#include <ags-lib/object/ags_connectable.h>

void ags_client_menu_bar_class_init(AgsClientMenuBarClass *client_menu_bar);
void ags_client_menu_bar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_client_menu_bar_init(AgsClientMenuBar *client_menu_bar);
void ags_client_menu_bar_connect(AgsConnectable *connectable);
void ags_client_menu_bar_disconnect(AgsConnectable *connectable);
void ags_client_menu_bar_finalize(GObject *gobject);

static gpointer ags_client_menu_bar_parent_class = NULL;

GType
ags_client_menu_bar_get_type()
{
  static GType ags_type_client_menu_bar = 0;

  if(!ags_type_client_menu_bar){
    static const GTypeInfo ags_client_menu_bar_info = {
      sizeof (AgsClientMenuBarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_client_menu_bar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClientMenuBar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_client_menu_bar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_client_menu_bar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_client_menu_bar = g_type_register_static(GTK_TYPE_MENU_BAR,
						      "AgsClientMenuBar\0",
						      &ags_client_menu_bar_info,
						      0);
    
    g_type_add_interface_static(ags_type_client_menu_bar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_client_menu_bar);
}

void
ags_client_menu_bar_class_init(AgsClientMenuBarClass *client_menu_bar)
{
  GObjectClass *gobject;

  ags_client_menu_bar_parent_class = g_type_class_peek_parent(client_menu_bar);

  /* GObjectClass */
  gobject = (GObjectClass *) client_menu_bar;

  gobject->finalize = ags_client_menu_bar_finalize;
}

void
ags_client_menu_bar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_client_menu_bar_connect;
  connectable->disconnect = ags_client_menu_bar_disconnect;
}

void
ags_client_menu_bar_init(AgsClientMenuBar *client_menu_bar)
{
  GtkMenu *menu;
  GtkMenuItem *menu_item;

  /* file menu */
  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_FILE,
								 NULL);
  gtk_menu_bar_append(GTK_MENU_BAR(client_menu_bar),
		      GTK_WIDGET(menu_item));

  menu = 
    client_menu_bar->file = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu(menu_item,
			    GTK_WIDGET(menu));

  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE,
								 NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			GTK_WIDGET(menu_item));

  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS,
								 NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			GTK_WIDGET(menu_item));

  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT,
								 NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			GTK_WIDGET(menu_item));

  /**/
  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_NETWORK,
									       NULL);
  gtk_menu_bar_append(GTK_MENU_BAR(client_menu_bar),
		      GTK_WIDGET(menu_item));

  menu = 
    client_menu_bar->network = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu(menu_item,
			    GTK_WIDGET(menu));

  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_CONNECT,
								 NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			GTK_WIDGET(menu_item));

  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_EXECUTE,
								 NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			GTK_WIDGET(menu_item));

  /*  */
  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP,
								 NULL);
  gtk_menu_bar_append(GTK_MENU_BAR(client_menu_bar),
		      GTK_WIDGET(menu_item));

  menu = 
    client_menu_bar->help = (GtkMenu *) gtk_menu_new();
  gtk_menu_item_set_submenu(menu_item,
			    GTK_WIDGET(menu));

  menu_item = (GtkMenuItem *) gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,
								 NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),
			GTK_WIDGET(menu_item));
}

void
ags_client_menu_bar_connect(AgsConnectable *connectable)
{
  GList *list;

  //TODO:JK: implement me
}

void
ags_client_menu_bar_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_menu_bar_finalize(GObject *gobject)
{
  AgsClientMenuBar *client_menu_bar;

  client_menu_bar = AGS_CLIENT_MENU_BAR(gobject);

  G_OBJECT_CLASS(ags_client_menu_bar_parent_class)->finalize(gobject);
}

AgsClientMenuBar*
ags_client_menu_bar_new()
{
  AgsClientMenuBar *client_menu_bar;

  client_menu_bar = (AgsClientMenuBar *) g_object_new(AGS_TYPE_CLIENT_MENU_BAR,
						      NULL);

  return(client_menu_bar);
}
