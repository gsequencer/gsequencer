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

#include <ags/audio/osc/ags_osc_websocket_connection.h>

#include <ags/audio/osc/ags_osc_server.h>
#include <ags/audio/osc/ags_osc_response.h>
#include <ags/audio/osc/ags_osc_util.h>
#include <ags/audio/osc/ags_osc_buffer_util.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>
#include <libxml/valid.h>
#include <libxml/xmlIO.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include <ags/i18n.h>

void ags_osc_websocket_connection_class_init(AgsOscWebsocketConnectionClass *osc_websocket_connection);
void ags_osc_websocket_connection_init(AgsOscWebsocketConnection *osc_websocket_connection);
void ags_osc_websocket_connection_set_property(GObject *gobject,
					       guint prop_id,
					       const GValue *value,
					       GParamSpec *param_spec);
void ags_osc_websocket_connection_get_property(GObject *gobject,
					       guint prop_id,
					       GValue *value,
					       GParamSpec *param_spec);
void ags_osc_websocket_connection_dispose(GObject *gobject);
void ags_osc_websocket_connection_finalize(GObject *gobject);

gint64 ags_osc_websocket_connection_write_response(AgsOscWebsocketConnection *osc_websocket_connection,
						   GObject *osc_response);

/**
 * SECTION:ags_osc_websocket_connection
 * @short_description: the OSC server side WebSockets connection
 * @title: AgsOscWebsocketConnection
 * @section_id:
 * @include: ags/audio/osc/ags_osc_websocket_connection.h
 *
 * #AgsOscWebsocketConnection your OSC server side websocket_connection.
 */

enum{
  PROP_0,
  PROP_WEBSOCKET_CONNECTION,
  PROP_CLIENT,
  PROP_SECURITY_CONTEXT,
  PROP_PATH,
  PROP_LOGIN,
  PROP_SECURITY_TOKEN,
  PROP_RESOURCE_ID,
};

static gpointer ags_osc_websocket_connection_parent_class = NULL;

GType
ags_osc_websocket_connection_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_websocket_connection = 0;

    static const GTypeInfo ags_osc_websocket_connection_info = {
      sizeof (AgsOscWebsocketConnectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_websocket_connection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscWebsocketConnection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_websocket_connection_init,
    };

    ags_type_osc_websocket_connection = g_type_register_static(AGS_TYPE_OSC_CONNECTION,
							       "AgsOscWebsocketConnection", &ags_osc_websocket_connection_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_websocket_connection);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_websocket_connection_class_init(AgsOscWebsocketConnectionClass *osc_websocket_connection)
{
  GObjectClass *gobject;
  AgsOscConnectionClass *osc_connection;
  
  GParamSpec *param_spec;

  ags_osc_websocket_connection_parent_class = g_type_class_peek_parent(osc_websocket_connection);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_websocket_connection;
  
  gobject->set_property = ags_osc_websocket_connection_set_property;
  gobject->get_property = ags_osc_websocket_connection_get_property;
  
  gobject->dispose = ags_osc_websocket_connection_dispose;
  gobject->finalize = ags_osc_websocket_connection_finalize;

  /* properties */
  /**
   * AgsOscWebsocketConnection:websocket-connection:
   *
   * The assigned #SoupMessage.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("websocket-connection",
				   i18n_pspec("assigned websocket connection"),
				   i18n_pspec("The websocket connection it is assigned with"),
				   SOUP_TYPE_WEBSOCKET_CONNECTION,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WEBSOCKET_CONNECTION,
				  param_spec);

  /**
   * AgsOscWebsocketConnection:client:
   *
   * The assigned #SoupClientContext-struct.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_boxed("client",
				  i18n_pspec("assigned client"),
				  i18n_pspec("The assigned client"),
				  SOUP_TYPE_CLIENT_CONTEXT,
				  G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CLIENT,
				  param_spec);

  /**
   * AgsOscWebsocketConnection:security-context:
   *
   * The assigned #AgsSecurityContext.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("security-context",
				   i18n_pspec("assigned security context"),
				   i18n_pspec("The security context it is assigned with"),
				   AGS_TYPE_SECURITY_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SECURITY_CONTEXT,
				  param_spec);

  /**
   * AgsOscWebsocketConnection:path:
   *
   * The current path.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("path",
				   i18n_pspec("path"),
				   i18n_pspec("The current path"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATH,
				  param_spec);

  /**
   * AgsOscWebsocketConnection:login:
   *
   * The current login.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("login",
				   i18n_pspec("login"),
				   i18n_pspec("The current login"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOGIN,
				  param_spec);

  /**
   * AgsOscWebsocketConnection:security-token:
   *
   * The current security token.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("security-token",
				   i18n_pspec("security token"),
				   i18n_pspec("The current security token"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SECURITY_TOKEN,
				  param_spec);

  /**
   * AgsOscWebsocketConnection:resource-id:
   *
   * The resource ID from a redirect.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("resource-id",
				   i18n_pspec("resource ID"),
				   i18n_pspec("The resource ID from a redirect"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RESOURCE_ID,
				  param_spec);

  /* AgsOscConnectionClass */
  osc_connection = (AgsOscConnection *) osc_websocket_connection;
  
  osc_connection->read_bytes = NULL;
  osc_connection->write_response = ags_osc_websocket_connection_write_response;
  
  osc_connection->close = NULL;

  /* signals */
}

void
ags_osc_websocket_connection_init(AgsOscWebsocketConnection *osc_websocket_connection)
{
  osc_websocket_connection->websocket_connection = NULL;

  osc_websocket_connection->client = NULL;

  osc_websocket_connection->security_context = NULL;

  osc_websocket_connection->path = NULL;

  osc_websocket_connection->login = NULL;
  osc_websocket_connection->security_token = NULL;

  osc_websocket_connection->resource_id = NULL;
}

void
ags_osc_websocket_connection_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec)
{
  AgsOscWebsocketConnection *osc_websocket_connection;

  GRecMutex *osc_connection_mutex;

  osc_websocket_connection = AGS_OSC_WEBSOCKET_CONNECTION(gobject);

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_websocket_connection);
  
  switch(prop_id){
  case PROP_WEBSOCKET_CONNECTION:
  {
    GObject *websocket_connection;

    websocket_connection = g_value_get_object(value);

    g_rec_mutex_lock(osc_connection_mutex);

    if(osc_websocket_connection->websocket_connection == websocket_connection){
      g_rec_mutex_unlock(osc_connection_mutex);

      return;
    }

    if(osc_websocket_connection->websocket_connection != NULL){
      g_object_unref(osc_websocket_connection->websocket_connection);
    }
      
    if(websocket_connection != NULL){
      g_object_ref(websocket_connection);
    }
      
    osc_websocket_connection->websocket_connection = websocket_connection;

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_CLIENT:
  {
    SoupClientContext *client;

    client = g_value_get_boxed(value);

    g_rec_mutex_lock(osc_connection_mutex);
      
    osc_websocket_connection->client = client;

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_SECURITY_CONTEXT:
  {
    GObject *security_context;

    security_context = g_value_get_object(value);

    g_rec_mutex_lock(osc_connection_mutex);

    if(osc_websocket_connection->security_context == security_context){
      g_rec_mutex_unlock(osc_connection_mutex);

      return;
    }

    if(osc_websocket_connection->security_context != NULL){
      g_object_unref(osc_websocket_connection->security_context);
    }
      
    if(security_context != NULL){
      g_object_ref(security_context);
    }
      
    osc_websocket_connection->security_context = security_context;

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_PATH:
  {
    gchar *path;

    path = g_value_get_string(value);

    g_rec_mutex_lock(osc_connection_mutex);

    if(osc_websocket_connection->path == path){
      g_rec_mutex_unlock(osc_connection_mutex);

      return;
    }

    g_free(osc_websocket_connection->path);
      
    osc_websocket_connection->path = g_strdup(path);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_LOGIN:
  {
    gchar *login;

    login = g_value_get_string(value);

    g_rec_mutex_lock(osc_connection_mutex);

    if(osc_websocket_connection->login == login){
      g_rec_mutex_unlock(osc_connection_mutex);

      return;
    }

    g_free(osc_websocket_connection->login);
      
    osc_websocket_connection->login = g_strdup(login);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_SECURITY_TOKEN:
  {
    gchar *security_token;

    security_token = g_value_get_string(value);

    g_rec_mutex_lock(osc_connection_mutex);

    if(osc_websocket_connection->security_token == security_token){
      g_rec_mutex_unlock(osc_connection_mutex);

      return;
    }

    g_free(osc_websocket_connection->security_token);
      
    osc_websocket_connection->security_token = g_strdup(security_token);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_RESOURCE_ID:
  {
    gchar *resource_id;

    resource_id = g_value_get_string(value);

    g_rec_mutex_lock(osc_connection_mutex);

    if(resource_id == osc_websocket_connection->resource_id){
      g_rec_mutex_unlock(osc_connection_mutex);

      return;
    }
    
    g_free(osc_websocket_connection->resource_id);
    
    osc_websocket_connection->resource_id = g_strdup(resource_id);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_websocket_connection_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec)
{
  AgsOscWebsocketConnection *osc_websocket_connection;

  GRecMutex *osc_connection_mutex;

  osc_websocket_connection = AGS_OSC_WEBSOCKET_CONNECTION(gobject);

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_websocket_connection);
  
  switch(prop_id){
  case PROP_WEBSOCKET_CONNECTION:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_object(value, osc_websocket_connection->websocket_connection);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_CLIENT:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_boxed(value, osc_websocket_connection->client);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_SECURITY_CONTEXT:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_object(value, osc_websocket_connection->security_context);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_PATH:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_string(value, osc_websocket_connection->path);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_LOGIN:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_string(value, osc_websocket_connection->login);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_SECURITY_TOKEN:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_string(value, osc_websocket_connection->security_token);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  case PROP_RESOURCE_ID:
  {
    g_rec_mutex_lock(osc_connection_mutex);

    g_value_set_string(value, osc_websocket_connection->resource_id);

    g_rec_mutex_unlock(osc_connection_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_websocket_connection_dispose(GObject *gobject)
{
  AgsOscWebsocketConnection *osc_websocket_connection;
    
  osc_websocket_connection = (AgsOscWebsocketConnection *) gobject;

  if(osc_websocket_connection->websocket_connection != NULL){
    g_object_unref(osc_websocket_connection->websocket_connection);

    osc_websocket_connection->websocket_connection = NULL;
  }

  if(osc_websocket_connection->security_context != NULL){
    g_object_unref(osc_websocket_connection->security_context);

    osc_websocket_connection->security_context = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_websocket_connection_parent_class)->dispose(gobject);
}

void
ags_osc_websocket_connection_finalize(GObject *gobject)
{
  AgsOscWebsocketConnection *osc_websocket_connection;
    
  osc_websocket_connection = (AgsOscWebsocketConnection *) gobject;

  if(osc_websocket_connection->websocket_connection != NULL){
    g_object_unref(osc_websocket_connection->websocket_connection);
  }

  if(osc_websocket_connection->security_context != NULL){
    g_object_unref(osc_websocket_connection->security_context);
  }

  g_free(osc_websocket_connection->path);
  g_free(osc_websocket_connection->login);
  g_free(osc_websocket_connection->security_token);
  
  g_free(osc_websocket_connection->resource_id);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_websocket_connection_parent_class)->finalize(gobject);
}

gint64
ags_osc_websocket_connection_write_response(AgsOscWebsocketConnection *osc_websocket_connection,
					    GObject *osc_response)
{
  SoupWebsocketConnection *websocket_connection;
  GIOStream *stream;
  GOutputStream *output_stream;

  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *osc_packet_node_list;
  xmlNode *osc_packet_node;
  
  gchar *data;
  xmlChar *buffer;

  int buffer_length;
  
  GError *error;

  GRecMutex *osc_response_mutex;

  g_object_get(osc_websocket_connection,
	       "websocket-connection", &websocket_connection,
	       NULL);
  
  if(websocket_connection == NULL){
    return(-1);
  }
  
  /* get osc response mutex */
  osc_response_mutex = AGS_OSC_RESPONSE_GET_OBJ_MUTEX(osc_response);

  /* create XML doc */
  doc = xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION);

  root_node = xmlNewNode(NULL,
			 BAD_CAST "ags-osc-over-xmlrpc");

  xmlDocSetRootElement(doc,
		       root_node);

  osc_packet_node_list = xmlNewNode(NULL,
				    BAD_CAST "ags-osc-packet-list");
  
  xmlAddChild(root_node,
	      osc_packet_node_list);

  osc_packet_node = xmlNewNode(NULL,
				BAD_CAST "ags-osc-packet");

  /* encode OSC packet */
  g_rec_mutex_lock(osc_response_mutex);

  data = g_base64_encode(AGS_OSC_RESPONSE(osc_response)->packet,
			 AGS_OSC_RESPONSE(osc_response)->packet_size);

  g_rec_mutex_unlock(osc_response_mutex);

  xmlNodeSetContent(osc_packet_node,
		    data);
  
  xmlAddChild(osc_packet_node_list,
	      osc_packet_node);

  xmlDocDumpFormatMemoryEnc(doc, &buffer, &buffer_length, "UTF-8", TRUE);
  
  /* write stream */
  stream = soup_websocket_connection_get_io_stream(websocket_connection);
  output_stream = g_io_stream_get_output_stream(stream);
  
  error = NULL;
  g_output_stream_write(output_stream,
			buffer,
			(gsize) buffer_length,
			NULL,
			&error);

  if(error != NULL){
    g_warning("%s", error->message);

    g_error_free(error);
  }

  g_object_unref(websocket_connection);
  
  xmlFree(buffer);
	    
  return((gint64) buffer_length);
}

/**
 * ags_osc_websocket_connection_find_resource_id:
 * @osc_websocket_connection: the #GList-struct containing #AgsOscWebsocketConnection
 * @resource_id: the resource id
 * 
 * Find @resource_id in @osc_websocket_connection.
 *
 * Returns: the next matchine #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_osc_websocket_connection_find_resource_id(GList *osc_websocket_connection,
					      gchar *resource_id)
{
  gchar *current_resource_id;
  
  gboolean success;
  
  if(osc_websocket_connection == NULL ||
     resource_id == NULL){
    return(NULL);
  }
  
  while(osc_websocket_connection != NULL){    
    g_object_get(osc_websocket_connection->data,
		 "resource-id", &current_resource_id,
		 NULL);

    success = (!g_strcmp0(resource_id, current_resource_id)) ? TRUE: FALSE;
    
    g_free(current_resource_id);

    if(success){
      break;
    }

    /* iterate */
    osc_websocket_connection = osc_websocket_connection->next;
  }

  return(osc_websocket_connection);
}

/**
 * ags_osc_websocket_connection_new:
 * @osc_server: the #AgsOscServer
 * 
 * Creates a new instance of #AgsOscWebsocketConnection
 *
 * Returns: the new #AgsOscWebsocketConnection
 * 
 * Since: 3.0.0
 */
AgsOscWebsocketConnection*
ags_osc_websocket_connection_new(GObject *osc_server)
{
  AgsOscWebsocketConnection *osc_websocket_connection;
   
  osc_websocket_connection = (AgsOscWebsocketConnection *) g_object_new(AGS_TYPE_OSC_WEBSOCKET_CONNECTION,
									"osc-server", osc_server,
									NULL);
  
  return(osc_websocket_connection);
}
