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

#include <ags-client/X/ags_client_toolbar.h>

#include <ags-lib/object/ags_connectable.h>

void ags_client_toolbar_class_init(AgsClientToolbarClass *client_toolbar);
void ags_client_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_client_toolbar_init(AgsClientToolbar *client_toolbar);
void ags_client_toolbar_connect(AgsConnectable *connectable);
void ags_client_toolbar_disconnect(AgsConnectable *connectable);
void ags_client_toolbar_finalize(GObject *gobject);

static gpointer ags_client_toolbar_parent_class = NULL;

GType
ags_client_toolbar_get_type()
{
  static GType ags_type_client_toolbar = 0;

  if(!ags_type_client_toolbar){
    static const GTypeInfo ags_client_toolbar_info = {
      sizeof (AgsClientToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_client_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClientToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_client_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_client_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_client_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
						     "AgsClientToolbar\0",
						     &ags_client_toolbar_info,
						     0);
    
    g_type_add_interface_static(ags_type_client_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_client_toolbar);
}

void
ags_client_toolbar_class_init(AgsClientToolbarClass *client_toolbar)
{
  GObjectClass *gobject;

  ags_client_toolbar_parent_class = g_type_class_peek_parent(client_toolbar);

  /* GObjectClass */
  gobject = (GObjectClass *) client_toolbar;

  gobject->finalize = ags_client_toolbar_finalize;
}

void
ags_client_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_client_toolbar_connect;
  connectable->disconnect = ags_client_toolbar_disconnect;
}

void
ags_client_toolbar_init(AgsClientToolbar *client_toolbar)
{
  //TODO:JK: implement me
}

void
ags_client_toolbar_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_toolbar_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_toolbar_finalize(GObject *gobject)
{
  AgsClientToolbar *client_toolbar;

  client_toolbar = AGS_CLIENT_TOOLBAR(gobject);

  G_OBJECT_CLASS(ags_client_toolbar_parent_class)->finalize(gobject);
}

AgsClientToolbar*
ags_client_toolbar_new()
{
  AgsClientToolbar *client_toolbar;

  client_toolbar = (AgsClientToolbar *) g_object_new(AGS_TYPE_CLIENT_TOOLBAR,
						     NULL);

  return(client_toolbar);
}
