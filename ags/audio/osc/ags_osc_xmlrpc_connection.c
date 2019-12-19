/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/osc/ags_osc_xmlrpc_connection.h>

#include <ags/audio/osc/ags_osc_server.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <ags/i18n.h>

void ags_osc_xmlrpc_connection_class_init(AgsOscXmlrpcConnectionClass *osc_xmlrpc_connection);
void ags_osc_xmlrpc_connection_init(AgsOscXmlrpcConnection *osc_xmlrpc_connection);
void ags_osc_xmlrpc_connection_dispose(GObject *gobject);
void ags_osc_xmlrpc_connection_finalize(GObject *gobject);

/**
 * SECTION:ags_osc_xmlrpc_connection
 * @short_description: the OSC server side xmlrpc_connection
 * @title: AgsOscXmlrpcConnection
 * @section_id:
 * @include: ags/audio/osc/ags_osc_xmlrpc_connection.h
 *
 * #AgsOscXmlrpcConnection your OSC server side xmlrpc_connection.
 */

static gpointer ags_osc_xmlrpc_connection_parent_class = NULL;
static guint osc_xmlrpc_connection_signals[LAST_SIGNAL];

GType
ags_osc_xmlrpc_connection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_xmlrpc_connection = 0;

    static const GTypeInfo ags_osc_xmlrpc_connection_info = {
      sizeof (AgsOscXmlrpcConnectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_xmlrpc_connection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscXmlrpcConnection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_xmlrpc_connection_init,
    };

    ags_type_osc_xmlrpc_connection = g_type_register_static(AGS_TYPE_OSC_CONNECTION,
							    "AgsOscXmlrpcConnection", &ags_osc_xmlrpc_connection_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_xmlrpc_connection);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_xmlrpc_connection_class_init(AgsOscXmlrpcConnectionClass *osc_xmlrpc_connection)
{
  GObjectClass *gobject;

  ags_osc_xmlrpc_connection_parent_class = g_type_class_peek_parent(osc_xmlrpc_connection);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_xmlrpc_connection;
  
  gobject->dispose = ags_osc_xmlrpc_connection_dispose;
  gobject->finalize = ags_osc_xmlrpc_connection_finalize;

  /* properties */

  /* AgsOscXmlrpcConnectionClass */  

  /* signals */
}

void
ags_osc_xmlrpc_connection_init(AgsOscXmlrpcConnection *osc_xmlrpc_connection)
{
  //TODO:JK: implement me
}

void
ags_osc_xmlrpc_connection_dispose(GObject *gobject)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;
    
  osc_xmlrpc_connection = (AgsOscXmlrpcConnection *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_connection_parent_class)->dispose(gobject);
}

void
ags_osc_xmlrpc_connection_finalize(GObject *gobject)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;
    
  osc_xmlrpc_connection = (AgsOscXmlrpcConnection *) gobject;
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_connection_parent_class)->finalize(gobject);
}

/**
 * ags_osc_xmlrpc_connection_new:
 * @osc_server: the #AgsOscServer
 * 
 * Creates a new instance of #AgsOscXmlrpcConnection
 *
 * Returns: the new #AgsOscXmlrpcConnection
 * 
 * Since: 3.0.0
 */
AgsOscXmlrpcConnection*
ags_osc_xmlrpc_connection_new(GObject *osc_server)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;
   
  osc_xmlrpc_connection = (AgsOscXmlrpcConnection *) g_object_new(AGS_TYPE_OSC_XMLRPC_CONNECTION,
								  "osc-server", osc_server,
								  NULL);
  
  return(osc_xmlrpc_connection);
}
