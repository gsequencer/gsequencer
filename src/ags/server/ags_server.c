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

#include <ags/audio/ags_server.h>

#include <ags/object/ags_connectable.h>

void ags_server_class_init(AgsServerClass *server);
void ags_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_init(AgsServer *server);
void ags_server_connect(AgsConnectable *connectable);
void ags_server_disconnect(AgsConnectable *connectable);
void ags_server_finalize(GObject *gobject);

static gpointer ags_server_parent_class = NULL;

GType
ags_server_get_type()
{
  static GType ags_type_server = 0;

  if(!ags_type_server){
    static const GTypeInfo ags_server_info = {
      sizeof (AgsServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_server = g_type_register_static(G_TYPE_OBJECT,
					   "AgsServer\0",
					   &ags_server_info,
					   0);

    g_type_add_interface_static(ags_type_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_server);
}

void
ags_server_class_init(AgsServerClass *server)
{
  GObjectClass *gobject;

  ags_server_parent_class = g_type_class_peek_parent(server);

  /* GObjectClass */
  gobject = (GObjectClass *) server;

  gobject->finalize = ags_server_finalize;
}

void
ags_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_server_connect;
  connectable->disconnect = ags_server_disconnect;
}

void
ags_server_init(AgsServer *server)
{
  server->flags = 0;

  server->devout = NULL;
}

void
ags_server_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_finalize(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  G_OBJECT_CLASS(ags_server_parent_class)->finalize(gobject);
}

AgsServer*
ags_server_new(GObject *devout)
{
  AgsServer *server;

  server = (AgsServer *) g_object_new(AGS_TYPE_SERVER,
				      NULL);

  server->devout = devout;

  return(server);
}
