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

#include <ags/audio/jack/ags_jack_client.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

void ags_jack_client_class_init(AgsJackClientClass *jack_client);
void ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_client_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_jack_client_init(AgsJackClient *jack_client);
void ags_jack_client_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_connect(AgsConnectable *connectable);
void ags_jack_client_disconnect(AgsConnectable *connectable);
void ags_jack_client_finalize(GObject *gobject);

enum{
  PROP_0,
};

static gpointer ags_jack_client_parent_class = NULL;

GType
ags_jack_client_get_type()
{
  static GType ags_type_jack_client = 0;

  if(!ags_type_jack_client){
    static const GTypeInfo ags_jack_client_info = {
      sizeof (AgsJackClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_jack_client = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackClient\0",
						  &ags_jack_client_info,
						  0);

    g_type_add_interface_static(ags_type_jack_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_jack_client);
}

void
ags_jack_client_class_init(AgsJackClientClass *jack_client)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_jack_client_parent_class = g_type_class_peek_parent(jack_client);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_client;

  gobject->set_property = ags_jack_client_set_property;
  gobject->get_property = ags_jack_client_get_property;

  gobject->finalize = ags_jack_client_finalize;

  /* properties */
}

void
ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_jack_client_connect;
  connectable->disconnect = ags_jack_client_disconnect;
}

void
ags_jack_client_init(AgsJackClient *jack_client)
{
  jack_client->flags = 0;

  jack_client->uuid = NULL;

  jack_client->client = NULL;

  jack_client->port = NULL;
}

void
ags_jack_client_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_client_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_client_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_client_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_client_finalize(GObject *gobject)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);

  G_OBJECT_CLASS(ags_jack_client_parent_class)->finalize(gobject);
}

/**
 * ags_jack_client_new:
 *
 * Instantiate a new #AgsJackClient.
 *
 * Returns: the new #AgsJackClient
 *
 * Since: 0.7.1
 */
AgsJackClient*
ags_jack_client_new(GObject *application_context,
		    gchar *url)
{
  AgsJackClient *jack_client;

  jack_client = (AgsJackClient *) g_object_new(AGS_TYPE_JACK_CLIENT,
					       NULL);

  return(jack_client);
}
