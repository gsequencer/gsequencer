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
void ags_osc_xmlrpc_connection_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_osc_xmlrpc_connection_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_osc_xmlrpc_connection_dispose(GObject *gobject);
void ags_osc_xmlrpc_connection_finalize(GObject *gobject);

guchar* ags_osc_xmlrpc_connection_read_bytes(AgsOscXmlrpcConnection *osc_xmlrpc_connection,
					     guint *data_length);

gint64 ags_osc_xmlrpc_connection_write_response(AgsOscXmlrpcConnection *osc_xmlrpc_connection,
						GObject *osc_response);

void ags_osc_xmlrpc_connection_close(AgsOscXmlrpcConnection *osc_xmlrpc_connection);

/**
 * SECTION:ags_osc_xmlrpc_connection
 * @short_description: the OSC server side XMLRPC connection
 * @title: AgsOscXmlrpcConnection
 * @section_id:
 * @include: ags/audio/osc/ags_osc_xmlrpc_connection.h
 *
 * #AgsOscXmlrpcConnection your OSC server side XMLRPC connection.
 */

enum{
  PROP_0,
  PROP_CLIENT,
  PROP_SECURITY_CONTEXT,
  PROP_PATH,
  PROP_LOGIN,
  PROP_SECURITY_TOKEN,
};

static gpointer ags_osc_xmlrpc_connection_parent_class = NULL;

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
  AgsOscConnectionClass *osc_connection;
  
  GParamSpec *param_spec;

  ags_osc_xmlrpc_connection_parent_class = g_type_class_peek_parent(osc_xmlrpc_connection);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_xmlrpc_connection;
  
  gobject->set_property = ags_osc_xmlrpc_connection_set_property;
  gobject->get_property = ags_osc_xmlrpc_connection_get_property;
  
  gobject->dispose = ags_osc_xmlrpc_connection_dispose;
  gobject->finalize = ags_osc_xmlrpc_connection_finalize;

  /* properties */
  /**
   * AgsOscXmlrpcConnection:client:
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
   * AgsOscXmlrpcConnection:security-context:
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
   * AgsOscXmlrpcConnection:path:
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
   * AgsOscXmlrpcConnection:login:
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
   * AgsOscXmlrpcConnection:security-token:
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

  /* AgsOscConnectionClass */
  osc_connection = (AgsOscConnection *) osc_xmlrpc_connection;
  
  osc_connection->read_bytes = ags_osc_xmlrpc_connection_read_bytes;
  osc_connection->write_response = ags_osc_xmlrpc_connection_write_response;
  
  osc_connection->close = ags_osc_xmlrpc_connection_close;

  /* signals */
}

void
ags_osc_xmlrpc_connection_init(AgsOscXmlrpcConnection *osc_xmlrpc_connection)
{
  osc_xmlrpc_connection->client = NULL;

  osc_xmlrpc_connection->security_context = NULL;

  osc_xmlrpc_connection->path = NULL;

  osc_xmlrpc_connection->login = NULL;
  osc_xmlrpc_connection->security_token = NULL;
}

void
ags_osc_xmlrpc_connection_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;

  GRecMutex *osc_connection_mutex;

  osc_xmlrpc_connection = AGS_OSC_XMLRPC_CONNECTION(gobject);

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_xmlrpc_connection);
  
  switch(prop_id){
  case PROP_CLIENT:
    {
      SoupClientContext *client;

      client = g_value_get_boxed(value);

      g_rec_mutex_lock(osc_connection_mutex);
      
      osc_xmlrpc_connection->client = client;

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_SECURITY_CONTEXT:
    {
      GObject *security_context;

      security_context = g_value_get_object(value);

      g_rec_mutex_lock(osc_connection_mutex);

      if(osc_xmlrpc_connection->security_context == security_context){
	g_rec_mutex_unlock(osc_connection_mutex);

	return;
      }

      if(osc_xmlrpc_connection->security_context != NULL){
	g_object_unref(osc_xmlrpc_connection->security_context);
      }
      
      if(security_context != NULL){
	g_object_ref(security_context);
      }
      
      osc_xmlrpc_connection->security_context = security_context;

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_PATH:
    {
      gchar *path;

      path = g_value_get_string(value);

      g_rec_mutex_lock(osc_connection_mutex);

      if(osc_xmlrpc_connection->path == path){
	g_rec_mutex_unlock(osc_connection_mutex);

	return;
      }

      g_free(osc_xmlrpc_connection->path);
      
      osc_xmlrpc_connection->path = g_strdup(path);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_LOGIN:
    {
      gchar *login;

      login = g_value_get_string(value);

      g_rec_mutex_lock(osc_connection_mutex);

      if(osc_xmlrpc_connection->login == login){
	g_rec_mutex_unlock(osc_connection_mutex);

	return;
      }

      g_free(osc_xmlrpc_connection->login);
      
      osc_xmlrpc_connection->login = g_strdup(login);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_SECURITY_TOKEN:
    {
      gchar *security_token;

      security_token = g_value_get_string(value);

      g_rec_mutex_lock(osc_connection_mutex);

      if(osc_xmlrpc_connection->security_token == security_token){
	g_rec_mutex_unlock(osc_connection_mutex);

	return;
      }

      g_free(osc_xmlrpc_connection->security_token);
      
      osc_xmlrpc_connection->security_token = g_strdup(security_token);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_xmlrpc_connection_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;

  GRecMutex *osc_connection_mutex;

  osc_xmlrpc_connection = AGS_OSC_XMLRPC_CONNECTION(gobject);

  /* get osc connection mutex */
  osc_connection_mutex = AGS_OSC_CONNECTION_GET_OBJ_MUTEX(osc_xmlrpc_connection);
  
  switch(prop_id){
  case PROP_CLIENT:
    {
      g_rec_mutex_lock(osc_connection_mutex);

      g_value_set_boxed(value, osc_xmlrpc_connection->client);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_SECURITY_CONTEXT:
    {
      g_rec_mutex_lock(osc_connection_mutex);

      g_value_set_object(value, osc_xmlrpc_connection->security_context);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_PATH:
    {
      g_rec_mutex_lock(osc_connection_mutex);

      g_value_set_string(value, osc_xmlrpc_connection->path);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_LOGIN:
    {
      g_rec_mutex_lock(osc_connection_mutex);

      g_value_set_string(value, osc_xmlrpc_connection->login);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  case PROP_SECURITY_TOKEN:
    {
      g_rec_mutex_lock(osc_connection_mutex);

      g_value_set_string(value, osc_xmlrpc_connection->security_token);

      g_rec_mutex_unlock(osc_connection_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_xmlrpc_connection_dispose(GObject *gobject)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;
    
  osc_xmlrpc_connection = (AgsOscXmlrpcConnection *) gobject;  

  if(osc_xmlrpc_connection->security_context != NULL){
    g_object_unref(osc_xmlrpc_connection->security_context);

    osc_xmlrpc_connection->security_context = NULL;
  }

  g_free(osc_xmlrpc_connection->path);
  g_free(osc_xmlrpc_connection->login);
  g_free(osc_xmlrpc_connection->security_token);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_connection_parent_class)->dispose(gobject);
}

void
ags_osc_xmlrpc_connection_finalize(GObject *gobject)
{
  AgsOscXmlrpcConnection *osc_xmlrpc_connection;
    
  osc_xmlrpc_connection = (AgsOscXmlrpcConnection *) gobject;

  if(osc_xmlrpc_connection->security_context != NULL){
    g_object_unref(osc_xmlrpc_connection->security_context);
  }

  g_free(osc_xmlrpc_connection->path);
  g_free(osc_xmlrpc_connection->login);
  g_free(osc_xmlrpc_connection->security_token);
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_connection_parent_class)->finalize(gobject);
}

guchar*
ags_osc_xmlrpc_connection_read_bytes(AgsOscXmlrpcConnection *osc_xmlrpc_connection,
				     guint *data_length)
{
  //TODO:JK: implement me

  return(NULL);
}

gint64
ags_osc_xmlrpc_connection_write_response(AgsOscXmlrpcConnection *osc_xmlrpc_connection,
					 GObject *osc_response)
{
  //TODO:JK: implement me

  return(0);
}

void
ags_osc_xmlrpc_connection_close(AgsOscXmlrpcConnection *osc_xmlrpc_connection)
{
  //TODO:JK: implement me
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
