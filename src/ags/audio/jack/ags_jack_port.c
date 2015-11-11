/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/jack/ags_jack_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

void ags_jack_port_class_init(AgsJackPortClass *jack_port);
void ags_jack_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_port_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_jack_port_init(AgsJackPort *jack_port);
void ags_jack_port_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_jack_port_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_jack_port_connect(AgsConnectable *connectable);
void ags_jack_port_disconnect(AgsConnectable *connectable);
void ags_jack_port_finalize(GObject *gobject);

enum{
  PROP_0,
};

static gpointer ags_jack_port_parent_class = NULL;

GType
ags_jack_port_get_type()
{
  static GType ags_type_jack_port = 0;

  if(!ags_type_jack_port){
    static const GTypeInfo ags_jack_port_info = {
      sizeof (AgsJackPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_jack_port = g_type_register_static(G_TYPE_OBJECT,
						"AgsJackPort\0",
						&ags_jack_port_info,
						0);

    g_type_add_interface_static(ags_type_jack_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_jack_port);
}

void
ags_jack_port_class_init(AgsJackPortClass *jack_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_jack_port_parent_class = g_type_class_peek_parent(jack_port);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_port;

  gobject->set_property = ags_jack_port_set_property;
  gobject->get_property = ags_jack_port_get_property;

  gobject->finalize = ags_jack_port_finalize;

  /* properties */
}

void
ags_jack_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_jack_port_connect;
  connectable->disconnect = ags_jack_port_disconnect;
}

void
ags_jack_port_init(AgsJackPort *jack_port)
{
  jack_port->flags = 0;

  jack_port->uuid = NULL;
  jack_port->name = NULL;
  jack_port->uri = NULL;

  jack_port->port = NULL;

  jack_port->device = NULL;
}

void
ags_jack_port_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_port_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_port_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_port_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_port_finalize(GObject *gobject)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);

  G_OBJECT_CLASS(ags_jack_port_parent_class)->finalize(gobject);
}

/**
 * ags_jack_port_new:
 *
 * Instantiate a new #AgsJackPort.
 *
 * Returns: the new #AgsJackPort
 *
 * Since: 0.7.1
 */
AgsJackPort*
ags_jack_port_new(GObject *application_context,
		  gchar *url)
{
  AgsJackPort *jack_port;

  jack_port = (AgsJackPort *) g_object_new(AGS_TYPE_JACK_PORT,
					   NULL);

  return(jack_port);
}
