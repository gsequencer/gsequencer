/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ags/server/ags_server_status.h>

#include <ags/object/ags_connectable.h>

void ags_server_status_class_init(AgsServerStatusClass *server_status);
void ags_server_status_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_status_init(AgsServerStatus *server_status);
void ags_server_status_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_server_status_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_server_status_connect(AgsConnectable *connectable);
void ags_server_status_disconnect(AgsConnectable *connectable);
void ags_server_status_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_MAXIMUM_CONNECTIONS,
  PROP_CONNECTION_COUNT,
};

static gpointer ags_server_status_parent_class = NULL;

GType
ags_server_status_get_type()
{
  static GType ags_type_server_status = 0;

  if(!ags_type_server_status){
    static const GTypeInfo ags_server_status_info = {
      sizeof (AgsServerStatusClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_status_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServerStatus),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_status_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_status_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_server_status = g_type_register_static(G_TYPE_OBJECT,
						    "AgsServerStatus",
						    &ags_server_status_info,
						    0);

    g_type_add_interface_static(ags_type_server_status,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_server_status);
}

void
ags_server_status_class_init(AgsServerStatusClass *server_status)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_server_status_parent_class = g_type_class_peek_parent(server_status);

  /* GObjectClass */
  gobject = (GObjectClass *) server_status;

  gobject->set_property = ags_server_status_set_property;
  gobject->get_property = ags_server_status_get_property;

  gobject->finalize = ags_server_status_finalize;
}

void
ags_server_status_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_server_status_connect;
  connectable->disconnect = ags_server_status_disconnect;
}

void
ags_server_status_init(AgsServerStatus *server_status)
{
  server_status->flags = 0;
}

void
ags_server_status_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsServerStatus *server_status;

  server_status = AGS_SERVER_STATUS(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_status_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsServerStatus *server_status;

  server_status = AGS_SERVER_STATUS(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_status_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_status_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_status_finalize(GObject *gobject)
{
  AgsServerStatus *server_status;

  server_status = AGS_SERVER_STATUS(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_server_status_parent_class)->finalize(gobject);
}

/**
 * ags_server_status_new:
 *
 * Instantiate #AgsServerStatus.
 * 
 * Returns: a new #AgsServerStatus
 * 
 * Since: 1.0.0
 */
AgsServerStatus*
ags_server_status_new()
{
  AgsServerStatus *server_status;

  server_status = (AgsServerStatus *) g_object_new(AGS_TYPE_SERVER_STATUS,
						   NULL);

  return(server_status);
}
