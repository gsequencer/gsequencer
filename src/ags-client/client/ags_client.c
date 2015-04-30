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

#include <ags-client/client/ags_client.h>

#include <ags/object/ags_connectable.h>

void ags_client_class_init(AgsClientClass *client);
void ags_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_client_init(AgsClient *client);
void ags_client_connect(AgsConnectable *connectable);
void ags_client_disconnect(AgsConnectable *connectable);
void ags_client_finalize(GObject *gobject);

static gpointer ags_client_parent_class = NULL;

GType
ags_client_get_type()
{
  static GType ags_type_client = 0;

  if(!ags_type_client){
    static const GTypeInfo ags_client_info = {
      sizeof (AgsClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_client = g_type_register_static(G_TYPE_OBJECT,
					   "AgsClient\0",
					   &ags_client_info,
					   0);

    g_type_add_interface_static(ags_type_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_client);
}

void
ags_client_class_init(AgsClientClass *client)
{
  GObjectClass *gobject;

  ags_client_parent_class = g_type_class_peek_parent(client);

  /* GObjectClass */
  gobject = (GObjectClass *) client;

  gobject->finalize = ags_client_finalize;
}

void
ags_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_client_connect;
  connectable->disconnect = ags_client_disconnect;
}

void
ags_client_init(AgsClient *client)
{
  //TODO:JK: implement me
}

void
ags_client_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_finalize(GObject *gobject)
{
  AgsClient *client;

  client = AGS_CLIENT(gobject);

  G_OBJECT_CLASS(ags_client_parent_class)->finalize(gobject);
}

AgsClient*
ags_client_new()
{
  AgsClient *client;

  client = (AgsClient *) g_object_new(AGS_TYPE_CLIENT,
				      NULL);

  return(client);
}

