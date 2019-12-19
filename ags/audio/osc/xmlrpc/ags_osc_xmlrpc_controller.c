/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/osc/xmlrpc/ags_osc_xmlrpc_controller.h>

#include <string.h>

void ags_osc_xmlrpc_controller_class_init(AgsOscXmlrpcControllerClass *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_plugin_controller_interface_init(AgsPluginControllerInterface *plugin_controller);
void ags_osc_xmlrpc_controller_init(AgsOscXmlrpcController *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_finalize(GObject *gobject);

gpointer ags_osc_xmlrpc_controller_do_request(AgsPluginController *plugin_controller,
					      SoupMessage *msg,
					      GHashTable *query,
					      SoupClientContext *client,
					      GObject *security_context,
					      gchar *context_path,
					      gchar *login,
					      gchar *security_token);

/**
 * SECTION:ags_osc_xmlrpc_controller
 * @short_description: handle OSC XMLRPC requests
 * @title: AgsOscXmlrpcController
 * @section_id:
 * @include: ags/audio/osc/controller/ags_osc_xmlrpc_controller.h
 *
 * The #AgsOscXmlrpcController is an object to handle XMLRPC requests.
 */

static gpointer ags_osc_xmlrpc_controller_parent_class = NULL;
static guint osc_xmlrpc_controller_signals[LAST_SIGNAL];

GType
ags_osc_xmlrpc_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_xmlrpc_controller = 0;

    static const GTypeInfo ags_osc_xmlrpc_controller_info = {
      sizeof (AgsOscXmlrpcControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_xmlrpc_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscXmlrpcController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_xmlrpc_controller_init,
    };

    static const GInterfaceInfo ags_plugin_controller_interface_info = {
      (GInterfaceInitFunc) ags_osc_xmlrpc_controller_plugin_controller_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_osc_xmlrpc_controller = g_type_register_static(AGS_TYPE_CONTROLLER,
							    "AgsOscXmlrpcController",
							    &ags_osc_xmlrpc_controller_info,
							    0);

    g_type_add_interface_static(ags_type_osc_xmlrpc_controller,
				AGS_TYPE_PLUGIN_CONTROLLER,
				&ags_plugin_controller_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_xmlrpc_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_xmlrpc_controller_class_init(AgsOscXmlrpcControllerClass *osc_xmlrpc_controller)
{
  GObjectClass *gobject;
  
  ags_osc_xmlrpc_controller_parent_class = g_type_class_peek_parent(osc_xmlrpc_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_xmlrpc_controller;

  gobject->finalize = ags_osc_xmlrpc_controller_finalize;

  /* AgsOscXmlrpcController */
  osc_xmlrpc_controller->do_request = ags_osc_xmlrpc_controller_real_do_request;

  /* signals */
}

void
ags_osc_xmlrpc_controller_plugin_controller_interface_init(AgsPluginControllerInterface *plugin_controller)
{
  plugin_controller->do_request = ags_osc_xmlrpc_controller_do_request;
}

void
ags_osc_xmlrpc_controller_init(AgsOscXmlrpcController *osc_xmlrpc_controller)
{
  gchar *context_path;

  context_path = g_strdup_printf("%s/ags-osc-srv",
				 AGS_CONTROLLER_BASE_PATH);
  
  g_object_set(osc_xmlrpc_controller,
	       "context-path", context_path,
	       NULL);

  g_free(context_path);

  /* OSC XMLRPC server */
  osc_xmlrpc_controller->osc_xmlrpc_server = NULL;

  osc_xmlrpc_controller->message = NULL;
}

void
ags_osc_xmlrpc_controller_finalize(GObject *gobject)
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  osc_xmlrpc_controller = AGS_OSC_XMLRPC_CONTROLLER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_controller_parent_class)->finalize(gobject);
}

gpointer
ags_osc_xmlrpc_controller_do_request(AgsPluginController *plugin_controller,
				     SoupMessage *msg,
				     GHashTable *query,
				     SoupClientContext *client,
				     GObject *security_context,
				     gchar *path,
				     gchar *login,
				     gchar *security_token)
{
  gpointer start_response;
  
  if(!AGS_IS_SECURITY_CONTEXT(security_context) ||
     path == NULL ||
     login == NULL ||
     security_token == NULL){
    return(NULL);
  }

  //TODO:JK: use certs

  start_response = NULL;
  
  return(start_response);
}

/**
 * ags_osc_xmlrpc_controller_new:
 * 
 * Instantiate new #AgsOscXmlrpcController
 * 
 * Returns: the #AgsOscXmlrpcController
 * 
 * Since: 3.0.0
 */
AgsOscXmlrpcController*
ags_osc_xmlrpc_controller_new()
{
  AgsOscXmlrpcController *osc_xmlrpc_controller;

  osc_xmlrpc_controller = (AgsOscXmlrpcController *) g_object_new(AGS_TYPE_OSC_XMLRPC_CONTROLLER,
								  NULL);

  return(osc_xmlrpc_controller);
}
