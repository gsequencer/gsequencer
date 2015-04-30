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

#include <ags-client/X/ags_client_log.h>

#include <ags/object/ags_connectable.h>

void ags_client_log_class_init(AgsClientLogClass *client_log);
void ags_client_log_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_client_log_init(AgsClientLog *client_log);
void ags_client_log_connect(AgsConnectable *connectable);
void ags_client_log_disconnect(AgsConnectable *connectable);
void ags_client_log_finalize(GObject *gobject);

static gpointer ags_client_log_parent_class = NULL;

GType
ags_client_log_get_type()
{
  static GType ags_type_client_log = 0;

  if(!ags_type_client_log){
    static const GTypeInfo ags_client_log_info = {
      sizeof (AgsClientLogClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_client_log_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsClientLog),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_client_log_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_client_log_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_client_log = g_type_register_static(GTK_TYPE_TEXT_VIEW,
						 "AgsClientLog\0",
						 &ags_client_log_info,
						 0);
    
    g_type_add_interface_static(ags_type_client_log,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_client_log);
}

void
ags_client_log_class_init(AgsClientLogClass *client_log)
{
  GObjectClass *gobject;

  ags_client_log_parent_class = g_type_class_peek_parent(client_log);

  /* GObjectClass */
  gobject = (GObjectClass *) client_log;

  gobject->finalize = ags_client_log_finalize;
}

void
ags_client_log_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_client_log_connect;
  connectable->disconnect = ags_client_log_disconnect;
}

void
ags_client_log_init(AgsClientLog *client_log)
{
  //TODO:JK: implement me
}

void
ags_client_log_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_log_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_client_log_finalize(GObject *gobject)
{
  AgsClientLog *client_log;

  client_log = AGS_CLIENT_LOG(gobject);

  G_OBJECT_CLASS(ags_client_log_parent_class)->finalize(gobject);
}

AgsClientLog*
ags_client_log_new()
{
  AgsClientLog *client_log;

  client_log = (AgsClientLog *) g_object_new(AGS_TYPE_CLIENT_LOG,
					     NULL);

  return(client_log);
}
