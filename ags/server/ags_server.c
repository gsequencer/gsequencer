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

#include <ags/server/ags_server.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/server/ags_service_provider.h>

#include <stdlib.h>
#include <string.h>

#ifndef AGS_W32API
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <unistd.h>

#include <ags/i18n.h>

void ags_server_class_init(AgsServerClass *server);
void ags_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_init(AgsServer *server);
void ags_server_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_server_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_server_dispose(GObject *gobject);
void ags_server_finalize(GObject *gobject);

AgsUUID* ags_server_get_uuid(AgsConnectable *connectable);
gboolean ags_server_has_resource(AgsConnectable *connectable);
gboolean ags_server_is_ready(AgsConnectable *connectable);
void ags_server_add_to_registry(AgsConnectable *connectable);
void ags_server_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_server_list_resource(AgsConnectable *connectable);
xmlNode* ags_server_xml_compose(AgsConnectable *connectable);
void ags_server_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_server_is_connected(AgsConnectable *connectable);
void ags_server_connect(AgsConnectable *connectable);
void ags_server_disconnect(AgsConnectable *connectable);

void ags_server_real_start(AgsServer *server);
void ags_server_real_stop(AgsServer *server);

/**
 * SECTION:ags_server
 * @short_description: remote control server
 * @title: AgsServer
 * @section_id:
 * @include: ags/server/ags_server.h
 *
 * The #AgsServer is a XML-RPC server.
 */

enum{
  PROP_0,
  PROP_DOMAIN,
  PROP_SERVER_PORT,
  PROP_IP4,
  PROP_IP6,
};

enum{
  START,
  STOP,
  LAST_SIGNAL,
};

static gpointer ags_server_parent_class = NULL;
static guint server_signals[LAST_SIGNAL];

static GList *ags_server_list = NULL;

GType
ags_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_server = 0;

    static const GTypeInfo ags_server = {
      sizeof(AgsServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_server = g_type_register_static(G_TYPE_OBJECT,
					     "AgsServer",
					     &ags_server,
					     0);

    g_type_add_interface_static(ags_type_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_server);
  }

  return g_define_type_id__volatile;
}

void
ags_server_class_init(AgsServerClass *server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_server_parent_class = g_type_class_peek_parent(server);

  /* GObjectClass */
  gobject = (GObjectClass *) server;

  gobject->set_property = ags_server_set_property;
  gobject->get_property = ags_server_get_property;

  gobject->dispose = ags_server_dispose;
  gobject->finalize = ags_server_finalize;

  /* properties */
  /**
   * AgsServer:domain:
   *
   * The domain to use.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("domain",
				   i18n_pspec("domain"),
				   i18n_pspec("The domain to use"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DOMAIN,
				  param_spec);
  
  /**
   * AgsServer:server-port:
   *
   * The server port to use.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_uint("server-port",
				 i18n_pspec("server port"),
				 i18n_pspec("The server port to use"),
				 0,
				 G_MAXUINT32,
				 AGS_SERVER_DEFAULT_SERVER_PORT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER_PORT,
				  param_spec);

  /**
   * AgsServer:ip4:
   *
   * The IPv4 address as string of the server.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("ip4",
				   i18n_pspec("ip4"),
				   i18n_pspec("The IPv4 address of the server"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IP4,
				  param_spec);

  /**
   * AgsServer:ip6:
   *
   * The IPv6 address as string of the server.
   * 
   * Since: 2.1.0
   */
  param_spec = g_param_spec_string("ip6",
				   i18n_pspec("ip6"),
				   i18n_pspec("The IPv6 address of the server"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_IP6,
				  param_spec);

  /* AgsServer */
  server->start = ags_server_real_start;
  server->stop = ags_server_real_stop;

  /* signals */
  /**
   * AgsServer::start:
   * @server: the #AgsServer
   *
   * The ::start signal is emitted as the server starts.
   *
   * Since: 2.0.0
   */
  server_signals[START] =
    g_signal_new("start",
		 G_TYPE_FROM_CLASS(server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsServerClass, start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsServer::stop:
   * @server: the #AgsServer
   *
   * The ::stop signal is emitted as the server stops.
   *
   * Since: 2.1.0
   */
  server_signals[STOP] =
    g_signal_new("stop",
		 G_TYPE_FROM_CLASS(server),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsServerClass, stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_server_get_uuid;
  connectable->has_resource = ags_server_has_resource;

  connectable->is_ready = ags_server_is_ready;
  connectable->add_to_registry = ags_server_add_to_registry;
  connectable->remove_from_registry = ags_server_remove_from_registry;

  connectable->list_resource = ags_server_list_resource;
  connectable->xml_compose = ags_server_xml_compose;
  connectable->xml_parse = ags_server_xml_parse;

  connectable->is_connected = ags_server_is_connected;  
  connectable->connect = ags_server_connect;
  connectable->disconnect = ags_server_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_server_init(AgsServer *server)
{
  server->flags = 0;

  g_rec_mutex_init(&(server->obj_mutex));

  /* uuid */
  server->uuid = ags_uuid_alloc();
  ags_uuid_generate(server->uuid);

  /*  */
  server->server_info = ags_server_info_alloc("localhost", ags_uuid_to_string(server->uuid));

#if defined AGS_WITH_XMLRPC_C && !AGS_W32API
  server->abyss_server = (TServer *) malloc(sizeof(TServer));
  server->socket = NULL;
#else
  server->abyss_server = NULL;
  server->socket = NULL;
#endif

  server->ip4 = g_strdup(AGS_SERVER_DEFAULT_INET4_ADDRESS);
  server->ip6 = g_strdup(AGS_SERVER_DEFAULT_INET6_ADDRESS);

  server->domain = g_strdup(AGS_SERVER_DEFAULT_DOMAIN);
  server->server_port = AGS_SERVER_DEFAULT_SERVER_PORT;
  
  server->ip4_fd = -1;
  server->ip6_fd = -1;
  
#if defined AGS_W32API
  server->ip4_address = NULL;
  server->ip6_address = NULL;
#else
  server->ip4_address = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
  memset(server->ip4_address, 0, sizeof(struct sockaddr_in));
  
  server->ip6_address = (struct sockaddr_in6 *) malloc(sizeof(struct sockaddr_in6));
  memset(server->ip6_address, 0, sizeof(struct sockaddr_in6));
#endif
  
  server->auth_module = AGS_SERVER_DEFAULT_AUTH_MODULE;
  
  server->controller = NULL;
}

void
ags_server_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsServer *server;

  GRecMutex *server_mutex;

  server = AGS_SERVER(gobject);

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      gchar *domain;

      domain = g_value_get_string(value);

      g_rec_mutex_lock(server_mutex);
      
      if(server->domain == domain){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_free(server->domain);

      server->domain = g_strdup(domain);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      guint server_port;

      server_port = g_value_get_uint(value);

      g_rec_mutex_lock(server_mutex);

      server->server_port = server_port;
      
      g_rec_mutex_unlock(server_mutex);      
    }
    break;
  case PROP_IP4:
    {
      gchar *ip4;

      ip4 = g_value_get_string(value);

      g_rec_mutex_lock(server_mutex);
      
      if(server->ip4 == ip4){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_free(server->ip4);

      server->ip4 = g_strdup(ip4);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_IP6:
    {
      gchar *ip6;

      ip6 = g_value_get_string(value);

      g_rec_mutex_lock(server_mutex);
      
      if(server->ip6 == ip6){
	g_rec_mutex_unlock(server_mutex);
	
	return;
      }

      g_free(server->ip6);

      server->ip6 = g_strdup(ip6);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsServer *server;

  GRecMutex *server_mutex;

  server = AGS_SERVER(gobject);

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);
  
  switch(prop_id){
  case PROP_DOMAIN:
    {
      g_rec_mutex_lock(server_mutex);

      g_value_set_string(value,
			 server->domain);
      
      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_SERVER_PORT:
    {
      g_rec_mutex_lock(server_mutex);
      
      g_value_set_uint(value,
		       server->server_port);

      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_IP4:
    {
      g_rec_mutex_lock(server_mutex);
      
      g_value_set_string(value,
			 server->ip4);
      
      g_rec_mutex_unlock(server_mutex);
    }
    break;
  case PROP_IP6:
    {
      g_rec_mutex_lock(server_mutex);
      
      g_value_set_string(value,
			 server->ip6);

      g_rec_mutex_unlock(server_mutex);
    }
    break;    
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_server_dispose(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->dispose(gobject);
}

void
ags_server_finalize(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  g_free(server->domain);
  
  g_free(server->ip4);
  g_free(server->ip6);
  
  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_server_get_uuid(AgsConnectable *connectable)
{
  AgsServer *server;
  
  AgsUUID *ptr;

  GRecMutex *server_mutex;

  server = AGS_SERVER(connectable);

  /* get server signal mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  /* get UUID */
  g_rec_mutex_lock(server_mutex);

  ptr = server->uuid;

  g_rec_mutex_unlock(server_mutex);
  
  return(ptr);
}

gboolean
ags_server_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_server_is_ready(AgsConnectable *connectable)
{
  AgsServer *server;
  
  gboolean is_ready;

  server = AGS_SERVER(connectable);

  /* check is added */
  is_ready = ags_server_test_flags(server, AGS_SERVER_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_server_add_to_registry(AgsConnectable *connectable)
{
  AgsServer *server;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  server = AGS_SERVER(connectable);

  ags_server_set_flags(server, AGS_SERVER_ADDED_TO_REGISTRY);
}

void
ags_server_remove_from_registry(AgsConnectable *connectable)
{
  AgsServer *server;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  server = AGS_SERVER(connectable);

  ags_server_unset_flags(server, AGS_SERVER_ADDED_TO_REGISTRY);
}

xmlNode*
ags_server_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_server_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_server_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_server_is_connected(AgsConnectable *connectable)
{
  AgsServer *server;
  
  gboolean is_connected;

  server = AGS_SERVER(connectable);

  /* check is connected */
  is_connected = ags_server_test_flags(server, AGS_SERVER_CONNECTED);
  
  return(is_connected);
}

void
ags_server_connect(AgsConnectable *connectable)
{
  AgsServer *server;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  server = AGS_SERVER(connectable);

  ags_server_set_flags(server, AGS_SERVER_CONNECTED);
}

void
ags_server_disconnect(AgsConnectable *connectable)
{

  AgsServer *server;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  server = AGS_SERVER(connectable);
  
  ags_server_unset_flags(server, AGS_SERVER_CONNECTED);
}

/**
 * ags_server_test_flags:
 * @server: the #AgsServer
 * @flags: the flags
 *
 * Test @flags to be set on @server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_server_test_flags(AgsServer *server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return(FALSE);
  }

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  /* test */
  g_rec_mutex_lock(server_mutex);

  retval = (flags & (server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(server_mutex);

  return(retval);
}

/**
 * ags_server_set_flags:
 * @server: the #AgsServer
 * @flags: see #AgsServerFlags-enum
 *
 * Enable a feature of @server.
 *
 * Since: 2.0.0
 */
void
ags_server_set_flags(AgsServer *server, guint flags)
{
  GRecMutex *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return;
  }

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(server_mutex);

  server->flags |= flags;
  
  g_rec_mutex_unlock(server_mutex);
}
    
/**
 * ags_server_unset_flags:
 * @server: the #AgsServer
 * @flags: see #AgsServerFlags-enum
 *
 * Disable a feature of @server.
 *
 * Since: 2.0.0
 */
void
ags_server_unset_flags(AgsServer *server, guint flags)
{  
  GRecMutex *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return;
  }

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(server_mutex);

  server->flags &= (~flags);
  
  g_rec_mutex_unlock(server_mutex);
}

/**
 * ags_server_info_alloc:
 * @server_name: the server name
 * @uuid: the uuid
 * 
 * Allocate server info.
 * 
 * Returns: the allocated #AgsServerInfo-struct
 * 
 * Since: 2.0.0
 */
AgsServerInfo*
ags_server_info_alloc(gchar *server_name, gchar *uuid)
{
  AgsServerInfo *server_info;

  server_info = (AgsServerInfo *) malloc(sizeof(AgsServerInfo));

  server_info->uuid = uuid;
  server_info->server_name = server_name;

  return(server_info);
}

void
ags_server_real_start(AgsServer *server)
{
  AgsRegistry *registry;

  AgsApplicationContext *application_context;

  const char *error;

  application_context = ags_application_context_get_instance();

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));
  
  ags_connectable_add_to_registry(AGS_CONNECTABLE(registry));

#ifndef AGS_W32API
#if 0
  //  xmlrpc_registry_set_shutdown(registry,
  //			       &requestShutdown, &terminationRequested);
  server->socket_fd = socket(AF_INET, SOCK_RDM, PF_INET);
  bind(server->socket_fd, server->address, sizeof(struct sockaddr_in));

#if defined AGS_WITH_XMLRPC_C
  SocketUnixCreateFd(server->socket_fd, &(server->socket));

  ServerCreateSocket2(server->abyss_server, server->socket, &error);
  xmlrpc_server_abyss_set_handlers2(server->abyss_server, "/RPC2", registry->registry);
  ServerInit(server->abyss_server);
  //  setupSignalHandlers();

  while((AGS_SERVER_RUNNING & (server->flags)) != 0){
    printf("Waiting for next RPC...\n");
    ServerRunOnce(server->abyss_server);
    /* This waits for the next connection, accepts it, reads the
       HTTP POST request, executes the indicated RPC, and closes
       the connection.
    */
  } 
#endif /* AGS_WITH_XMLRPC_C */
#endif
#endif /* AGS_W32API */
}

/**
 * ags_server_start:
 * @server: the #AgsServer
 * 
 * Start the XMLRPC-C abyss server.
 * 
 * Since: 2.0.0
 */
void
ags_server_start(AgsServer *server)
{
  g_return_if_fail(AGS_IS_SERVER(server));

  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[START], 0);
  g_object_unref((GObject *) server);
}

void
ags_server_real_stop(AgsServer *server)
{
  GRecMutex *server_mutex;

  /* get server mutex */
  server_mutex = AGS_SERVER_GET_OBJ_MUTEX(server);

  /* close fd */
  g_rec_mutex_lock(server_mutex);

  if(server->ip4_fd != -1){
    close(server->ip4_fd);
  }

  if(server->ip6_fd != -1){
    close(server->ip6_fd);
  }

  g_rec_mutex_lock(server_mutex);
}

/**
 * ags_server_stop:
 * @server: the #AgsServer
 * 
 * Stop the XMLRPC-C abyss server.
 * 
 * Since: 2.1.0
 */
void
ags_server_stop(AgsServer *server)
{
  g_return_if_fail(AGS_IS_SERVER(server));

  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[STOP], 0);
  g_object_unref((GObject *) server);
}

/**
 * ags_server_lookup:
 * @server_info: the #AgsServerInfo-struct
 *
 * Lookup #AgsServer by @server_info.
 *
 * Returns: the associated #AgsServer if found, else %NULL
 * 
 * Since: 2.0.0
 */
AgsServer*
ags_server_lookup(AgsServerInfo *server_info)
{
  GList *current;

  if(server_info == NULL){
    return(NULL);
  }
  
  current = ags_server_list;

  while(current != NULL){
    if(AGS_SERVER(current->data)->server_info != NULL &&
       !g_ascii_strcasecmp(server_info->uuid,
			   AGS_SERVER(current->data)->server_info->uuid) &&
       !g_strcmp0(server_info->server_name,
		  AGS_SERVER(current->data)->server_info->server_name)){
      return(AGS_SERVER(current->data));
    }

    current = current->next;
  }

  return(NULL);
}

/**
 * ags_server_new:
 * @application_context: the #AgsApplicationContext
 *
 * Instantiate #AgsServer.
 * 
 * Returns: a new #AgsServer
 * 
 * Since: 2.0.0
 */
AgsServer*
ags_server_new(GObject *application_context)
{
  AgsServer *server;

  server = (AgsServer *) g_object_new(AGS_TYPE_SERVER,
				      "application-context", application_context,
				      NULL);

  return(server);
}
