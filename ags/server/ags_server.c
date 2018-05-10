/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/thread/ags_mutex_manager.h>

#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_registry.h>

#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
  PROP_APPLICATION_CONTEXT,
};

enum{
  START,
  LAST_SIGNAL,
};

static gpointer ags_server_parent_class = NULL;
static guint server_signals[LAST_SIGNAL];

static GList *ags_server_list = NULL;

static pthread_mutex_t ags_server_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_server_get_type()
{
  static GType ags_type_server = 0;

  if(!ags_type_server){
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
  }

  return (ags_type_server);
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
   * AgsServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n("application context object"),
				   i18n("The application context object"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsServer */
  server->start = ags_server_real_start;

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

  server->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(server->obj_mutexattr);
  pthread_mutexattr_settype(server->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(server->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif
  
  server->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(server->obj_mutex,
		     server->obj_mutexattr);

  /* uuid */
  server->uuid = ags_uuid_alloc();
  ags_uuid_generate(server->uuid);

  /*  */
  server->server_info = ags_server_info_alloc("localhost", ags_uuid_to_string(server->uuid));

#ifdef AGS_WITH_XMLRPC_C
  server->abyss_server = (TServer *) malloc(sizeof(TServer));
  server->socket = NULL;
#else
  server->abyss_server = NULL;
  server->socket = NULL;
#endif

  server->address = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
  memset(server->address, 0, sizeof(struct sockaddr_in));
  
  server->address->sin_port = 8080;
  server->address->sin_family = AF_INET;

  inet_aton("127.0.0.1", &(server->address->sin_addr.s_addr));

  server->auth_module = AGS_SERVER_DEFAULT_AUTH_MODULE;
  
  server->controller = NULL;
  
  server->application_context = NULL;
}

void
ags_server_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsServer *server;

  pthread_mutex_t *server_mutex;

  server = AGS_SERVER(gobject);

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(server_mutex);

      if(server->application_context == (GObject *) application_context){
	pthread_mutex_unlock(server_mutex);

	return;
      }

      if(server->application_context != NULL){
	g_object_unref(G_OBJECT(server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      server->application_context = application_context;

      pthread_mutex_unlock(server_mutex);
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

  pthread_mutex_t *server_mutex;

  server = AGS_SERVER(gobject);

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(server_mutex);

      g_value_set_object(value, server->application_context);

      pthread_mutex_unlock(server_mutex);
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

  if(server->application_context != NULL){
    g_object_unref(server->application_context);

    server->application_context = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->dispose(gobject);
}

void
ags_server_finalize(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  /* mutex */
  pthread_mutex_destroy(server->obj_mutex);
  free(server->obj_mutex);

  pthread_mutexattr_destroy(server->obj_mutexattr);
  free(server->obj_mutexattr);

  if(server->application_context != NULL){
    g_object_unref(server->application_context);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_server_get_uuid(AgsConnectable *connectable)
{
  AgsServer *server;
  
  AgsUUID *ptr;

  pthread_mutex_t *server_mutex;

  server = AGS_SERVER(connectable);

  /* get server signal mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(server_mutex);

  ptr = server->uuid;

  pthread_mutex_unlock(server_mutex);
  
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

  pthread_mutex_t *server_mutex;

  server = AGS_SERVER(connectable);

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(server_mutex);

  is_ready = (((AGS_SERVER_ADDED_TO_REGISTRY & (server->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(server_mutex);
  
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

  pthread_mutex_t *server_mutex;

  server = AGS_SERVER(connectable);

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(server_mutex);

  is_connected = (((AGS_SERVER_CONNECTED & (server->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(server_mutex);
  
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
 * ags_server_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_server_get_class_mutex()
{
  return(&ags_server_class_mutex);
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
  
  pthread_mutex_t *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return(FALSE);
  }

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());

  /* test */
  pthread_mutex_lock(server_mutex);

  retval = (flags & (server->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(server_mutex);

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
  pthread_mutex_t *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return;
  }

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(server_mutex);

  server->flags |= flags;
  
  pthread_mutex_unlock(server_mutex);
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
  pthread_mutex_t *server_mutex;

  if(!AGS_IS_SERVER(server)){
    return;
  }

  /* get server mutex */
  pthread_mutex_lock(ags_server_get_class_mutex());
  
  server_mutex = server->obj_mutex;
  
  pthread_mutex_unlock(ags_server_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(server_mutex);

  server->flags &= (~flags);
  
  pthread_mutex_unlock(server_mutex);
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
  const char *error;

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(server->application_context));
  
  ags_connectable_add_to_registry(AGS_CONNECTABLE(server->application_context));

  //  xmlrpc_registry_set_shutdown(registry,
  //			       &requestShutdown, &terminationRequested);
  server->socket_fd = socket(AF_INET, SOCK_RDM, PF_INET);
  bind(server->socket_fd, server->address, sizeof(struct sockaddr_in));

#ifdef AGS_WITH_XMLRPC_C
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
}

/**
 * ags_server_start:
 * @server: the #AgsServer
 * 
 * Start the XMLRPC-C abyss server.
 * 
 * Since: 1.0.0
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

/**
 * ags_server_lookup:
 * @server_info: the #AgsServerInfo-struct
 *
 * Lookup #AgsServer by @server_info.
 *
 * Returns: the associated #AgsServer if found, else %NULL
 * 
 * Since: 1.0.0
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
 * Since: 1.0.0
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
