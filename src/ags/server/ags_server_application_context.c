/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/server/ags_server_application_context.h>

#include <ags/object/ags_connectable.h>

void ags_server_application_context_class_init(AgsServerApplicationContextClass *server_application_context);
void ags_server_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_application_context_init(AgsServerApplicationContext *server_application_context);
void ags_server_application_context_set_property(GObject *gobject,
						 guint prop_id,
						 const GValue *value,
						 GParamSpec *param_spec);
void ags_server_application_context_get_property(GObject *gobject,
						 guint prop_id,
						 GValue *value,
						 GParamSpec *param_spec);
void ags_server_application_context_connect(AgsConnectable *connectable);
void ags_server_application_context_disconnect(AgsConnectable *connectable);
void ags_server_application_context_finalize(GObject *gobject);

static gpointer ags_server_application_context_parent_class = NULL;
static AgsConnectableInterface* ags_server_application_context_parent_connectable_interface;

AgsServerApplicationContext *ags_server_application_context = NULL;

GType
ags_server_application_context_get_type()
{
  static GType ags_type_server_application_context = 0;

  if(!ags_type_server_application_context){
    static const GTypeInfo ags_server_application_context_info = {
      sizeof (AgsServerApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServerApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_server_application_context = g_type_register_static(AGS_TYPE_APPLICATION_CONTEXT,
								 "AgsServerApplicationContext\0",
								 &ags_server_application_context_info,
								 0);

    g_type_add_interface_static(ags_type_server_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_server_application_context);
}

void
ags_server_application_context_class_init(AgsServerApplicationContextClass *server_application_context)
{
  GObjectClass *gobject;
  AgsApplicationContextClass *application_context;

  ags_server_application_context_parent_class = g_type_class_peek_parent(ags_server_application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) server_application_context;

  gobject->set_property = ags_server_application_context_set_property;
  gobject->get_property = ags_server_application_context_get_property;

  gobject->finalize = ags_server_application_context_finalize;

  /* AgsServerApplicationContextClass */
  application_context = (AgsApplicationContextClass *) server_application_context;
  
  application_context->load_config = NULL;
  application_context->register_types = NULL;
}

void
ags_server_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_server_application_context_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_server_application_context_connect;
  connectable->disconnect = ags_server_application_context_disconnect;
}

void
ags_server_application_context_init(AgsServerApplicationContext *server_application_context)
{
  server_application_context->flags = 0;
}

void
ags_server_application_context_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_application_context_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_application_context_connect(AgsConnectable *connectable)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(server_application_context)->flags)) != 0){
    return;
  }

  ags_server_application_context_parent_connectable_interface->connect(connectable);
}

void
ags_server_application_context_disconnect(AgsConnectable *connectable)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (AGS_APPLICATION_CONTEXT(server_application_context)->flags)) == 0){
    return;
  }

  ags_server_application_context_parent_connectable_interface->disconnect(connectable);
}

void
ags_server_application_context_finalize(GObject *gobject)
{
  AgsServerApplicationContext *server_application_context;

  G_OBJECT_CLASS(ags_server_application_context_parent_class)->finalize(gobject);

  server_application_context = AGS_SERVER_APPLICATION_CONTEXT(gobject);
}

AgsServerApplicationContext*
ags_server_application_context_new(GObject *main_loop,
				   AgsConfig *config)
{
  AgsServerApplicationContext *server_application_context;

  server_application_context = (AgsServerApplicationContext *) g_object_new(AGS_TYPE_SERVER_APPLICATION_CONTEXT,
									    "main-loop\0", main_loop,
									    "config\0", config,
									    NULL);

  return(server_application_context);
}
