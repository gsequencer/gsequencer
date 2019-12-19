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

#include <ags/audio/osc/ags_osc_xmlrpc_server.h>

#include <ags/i18n.h>

void ags_osc_xmlrpc_server_class_init(AgsOscXmlrpcServerClass *osc_xmlrpc_server);
void ags_osc_xmlrpc_server_init(AgsOscXmlrpcServer *osc_xmlrpc_server);
void ags_osc_xmlrpc_server_dispose(GObject *gobject);
void ags_osc_xmlrpc_server_finalize(GObject *gobject);

static gpointer ags_osc_xmlrpc_server_parent_class = NULL;

GType
ags_osc_xmlrpc_server_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_xmlrpc_server = 0;

    static const GTypeInfo ags_osc_xmlrpc_server_info = {
      sizeof (AgsOscXmlrpcServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_xmlrpc_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscXmlrpcServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_xmlrpc_server_init,
    };

    ags_type_osc_xmlrpc_server = g_type_register_static(AGS_TYPE_OSC_SERVER,
							"AgsOscXmlrpcServer", &ags_osc_xmlrpc_server_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_xmlrpc_server);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_xmlrpc_server_class_init(AgsOscXmlrpcServerClass *osc_xmlrpc_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_xmlrpc_server_parent_class = g_type_class_peek_parent(osc_xmlrpc_server);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_xmlrpc_server;

  gobject->dispose = ags_osc_xmlrpc_server_dispose;
  gobject->finalize = ags_osc_xmlrpc_server_finalize;
}

void
ags_osc_xmlrpc_server_init(AgsOscXmlrpcServer *osc_xmlrpc_server)
{
  //TODO:JK: implement me
}

void
ags_osc_xmlrpc_server_dispose(GObject *gobject)
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;
  
  osc_xmlrpc_server = (AgsOscXmlrpcServer *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_server_parent_class)->dispose(gobject);
}

void
ags_osc_xmlrpc_server_finalize(GObject *gobject)
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;
    
  osc_xmlrpc_server = (AgsOscXmlrpcServer *) gobject;
    
  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_server_parent_class)->finalize(gobject);
}

/**
 * ags_osc_xmlrpc_server_new:
 * 
 * Creates a new instance of #AgsOscXmlrpcServer
 *
 * Returns: the new #AgsOscXmlrpcServer
 * 
 * Since: 3.0.0
 */
AgsOscXmlrpcServer*
ags_osc_xmlrpc_server_new()
{
  AgsOscXmlrpcServer *osc_xmlrpc_server;
   
  osc_xmlrpc_server = (AgsOscXmlrpcServer *) g_object_new(AGS_TYPE_OSC_XMLRPC_SERVER,
							  NULL);
  
  return(osc_xmlrpc_server);
}
