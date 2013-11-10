/* AGS - Advanced GTK Sequencer
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

#include <ags/audio/ags_port.h>

#include <ags-lib/object/ags_connectable.h>

void ags_port_class_init(AgsPortClass *port_class);
void ags_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_port_init(AgsPort *port);
void ags_port_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec);
void ags_port_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec);
void ags_port_connect(AgsConnectable *connectable);
void ags_port_disconnect(AgsConnectable *connectable);

static gpointer ags_port_parent_class = NULL;

GType
ags_port_get_type (void)
{
  static GType ags_type_port = 0;

  if(!ags_type_port){
    static const GTypeInfo ags_port_info = {
      sizeof (AgsPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_port = g_type_register_static(G_TYPE_OBJECT,
					   "AgsPort\0",
					   &ags_port_info,
					   0);

    g_type_add_interface_static(ags_type_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_port);
}

void
ags_port_class_init(AgsPortClass *port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_port_parent_class = g_type_class_peek_parent(port);

  /* GObjectClass */
  gobject = (GObjectClass *) port;

  gobject->set_property = ags_port_set_property;
  gobject->get_property = ags_port_get_property;
}

void
ags_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;
  connectable->connect = ags_port_connect;
  connectable->disconnect = ags_port_disconnect;
}

void
ags_port_init(AgsPort *port)
{
  port->port_value_is_pointer = FALSE;
  port->port_value_type = G_TYPE_NONE;
  port->port_value_size = sizeof(guint);
}

void
ags_port_set_property(GObject *gobject,
		      guint prop_id,
		      const GValue *value,
		      GParamSpec *param_spec)
{
  AgsPort *port;

  port = AGS_PORT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_port_get_property(GObject *gobject,
		      guint prop_id,
		      GValue *value,
		      GParamSpec *param_spec)
{
  AgsPort *port;

  port = AGS_PORT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_port_connect(AgsConnectable *connectable)
{
  AgsPort *port;

  port = AGS_PORT(connectable);
}

void
ags_port_disconnect(AgsConnectable *connectable)
{
  AgsPort *port;

  port = AGS_PORT(connectable);
}

AgsPort*
ags_port_new()
{
  AgsPort *port;

  port = (AgsPort *) g_object_new(AGS_TYPE_PORT,
				  NULL);

  return(port);
}
