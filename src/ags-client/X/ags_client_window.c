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

#include <ags-client/X/ags_client_window.h>
#include <ags-client/X/ags_client_window_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

void ags_client_window_class_init(AgsClientWindowClass *client_window);
void ags_client_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_client_window_init(AgsClientWindow *client_window);
void ags_client_window_connect(AgsConnectable *connectable);
void ags_client_window_disconnect(AgsConnectable *connectable);
void ags_client_window_finalize(GObject *gobject);

static gpointer ags_client_window_parent_class = NULL;

GType
ags_client_window_get_type()
{
  static GType ags_type_client_window = 0;

  if(!ags_type_client_window){
    static const GTypeInfo ags_client_window_info = {
      sizeof (AgsClientWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_client_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClientWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_client_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_client_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_client_window = g_type_register_static(GTK_TYPE_WINDOW,
						    "AgsClientWindow\0",
						    &ags_client_window_info,
						    0);

    g_type_add_interface_static(ags_type_client_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_client_window);
}

void
ags_client_window_class_init(AgsClientWindowClass *client_window)
{
  GObjectClass *gobject;

  ags_client_window_parent_class = g_type_class_peek_parent(client_window);

  /* GObjectClass */
  gobject = (GObjectClass *) client_window;

  gobject->finalize = ags_client_window_finalize;
}

void
ags_client_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_client_window_connect;
  connectable->disconnect = ags_client_window_disconnect;
}

void
ags_client_window_init(AgsClientWindow *client_window)
{
  GtkVBox *vbox;
  GtkVPaned *vpaned;
  
  client_window->name = g_strdup("unnamed\0");
   
  /* widgets */
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(client_window),
		    GTK_WIDGET(vbox));
  
  client_window->menu_bar = ags_client_menu_bar_new();
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(client_window->menu_bar),
		     FALSE, FALSE,
		     0);

  client_window->toolbar = ags_client_toolbar_new();
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(client_window->toolbar),
		     FALSE, FALSE,
		     0);

  vpaned = (GtkVPaned *) gtk_vpaned_new();
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(vpaned),
		     TRUE, TRUE,
		     0);
  
  client_window->script_editor = ags_script_editor_new();
  gtk_paned_add1(GTK_PANED(vpaned),
		 GTK_WIDGET(client_window->script_editor));

  client_window->log = ags_client_log_new();
  gtk_paned_add2(GTK_PANED(vpaned),
		 GTK_WIDGET(client_window->log));
}

void
ags_client_window_connect(AgsConnectable *connectable)
{
  AgsClientWindow *client_window;

  client_window = AGS_CLIENT_WINDOW(connectable);

  g_signal_connect(G_OBJECT(client_window), "delete_event\0",
		   G_CALLBACK(ags_client_window_delete_event_callback), NULL);

  /*  */
  ags_connectable_connect(AGS_CONNECTABLE(client_window->menu_bar));
}

void
ags_client_window_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_window_finalize(GObject *gobject)
{
  AgsClientWindow *client_window;

  client_window = AGS_CLIENT_WINDOW(gobject);

  G_OBJECT_CLASS(ags_client_window_parent_class)->finalize(gobject);
}

AgsClientWindow*
ags_client_window_new()
{
  AgsClientWindow *client_window;

  client_window = (AgsClientWindow *) g_object_new(AGS_TYPE_CLIENT_WINDOW,
						   NULL);

  return(client_window);
}
