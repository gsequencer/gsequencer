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
#include <ags/server/ags_server.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

void ags_server_class_init(AgsServerClass *server);
void ags_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_server_init(AgsServer *server);
void ags_server_add_to_registry(AgsConnectable *connectable);
void ags_server_remove_from_registry(AgsConnectable *connectable);
void ags_server_connect(AgsConnectable *connectable);
void ags_server_disconnect(AgsConnectable *connectable);
void ags_server_finalize(GObject *gobject);

void ags_server_real_start(AgsServer *server);

enum{
  START,
  LAST_SIGNAL,
};

static gpointer ags_server_parent_class = NULL;
static guint server_signals[LAST_SIGNAL];

static GList *ags_server_list = NULL;

GType
ags_server_get_type()
{
  static GType ags_type_server = 0;

  if(!ags_type_server){
    static const GTypeInfo ags_server_info = {
      sizeof (AgsServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_server = g_type_register_static(G_TYPE_OBJECT,
					     "AgsServer\0",
					     &ags_server_info,
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

  ags_server_parent_class = g_type_class_peek_parent(server);

  /* GObjectClass */
  gobject = (GObjectClass *) server;

  gobject->finalize = ags_server_finalize;

  /* AgsServer */
  server->start = ags_server_real_start;

  server_signals[START] =
    g_signal_new("start\0",
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
  connectable->add_to_registry = ags_server_add_to_registry;
  connectable->remove_from_registry = ags_server_remove_from_registry;
  connectable->connect = ags_server_connect;
  connectable->disconnect = ags_server_disconnect;
}

void
ags_server_init(AgsServer *server)
{
  server->flags = 0;

  memset(&(server->address), 0, sizeof(struct sockaddr_in));
  server->address.sin_port = 8080;
  server->address.sin_family = AF_INET;
  inet_aton("127.0.0.1\0", &(server->address.sin_addr.s_addr));

  server->server_info = NULL;

  server->main = NULL;

  server->registry = ags_registry_new();
  server->remote_task = ags_remote_task_new();
}

void
ags_server_add_to_registry(AgsConnectable *connectable)
{
  AgsServer *server;
  AgsRegistry *registry;

#ifdef AGS_WITH_XMLRPC_C
  struct xmlrpc_method_info3 *method_info;
#endif /* AGS_WITH_XMLRPC_C */

  server = AGS_SERVER(connectable);
  
  registry = AGS_REGISTRY(server->registry);


#ifdef AGS_WITH_XMLRPC_C
  /* create object */
  method_info = (struct xmlrpc_method_info3 *) malloc(sizeof(struct xmlrpc_method_info3));
  method_info->methodName = "ags_server_create_object\0";
  method_info->methodFunction = &ags_server_create_object;
  method_info->serverInfo = NULL;
  xmlrpc_registry_add_method3(&(AGS_MAIN(server->main)->env),
			      registry->registry,
			      method_info);

  /* set property */
  method_info = (struct xmlrpc_method_info3 *) malloc(sizeof(struct xmlrpc_method_info3));
  method_info->methodName = "ags_server_object_set_property\0";
  method_info->methodFunction = &ags_server_object_set_property;
  method_info->serverInfo = NULL;
  xmlrpc_registry_add_method3(&(AGS_MAIN(server->main)->env),
			      registry->registry,
			      method_info);
#endif /* AGS_WITH_XMLRPC_C */

  /* children */
  ags_connectable_add_to_registry(AGS_CONNECTABLE(server->registry));
  ags_connectable_add_to_registry(AGS_CONNECTABLE(server->remote_task));
}

void
ags_server_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_server_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_server_finalize(GObject *gobject)
{
  AgsServer *server;

  server = AGS_SERVER(gobject);

  G_OBJECT_CLASS(ags_server_parent_class)->finalize(gobject);
}

void
ags_server_real_start(AgsServer *server)
{
  AgsMain *main;
  AgsRegistry *registry;
  const char *error;

  main = AGS_MAIN(server->main);

  registry = AGS_REGISTRY(server->registry);
#ifdef AGS_WITH_XMLRPC_C
  registry->registry = xmlrpc_registry_new(&(main->env));
#endif /* AGS_WITH_XMLRPC_C */

  ags_connectable_add_to_registry(AGS_CONNECTABLE(main->main_loop));
  ags_connectable_add_to_registry(AGS_CONNECTABLE(server));
  ags_connectable_add_to_registry(AGS_CONNECTABLE(main->devout));
  ags_connectable_add_to_registry(AGS_CONNECTABLE(main->window));

  //  xmlrpc_registry_set_shutdown(registry,
  //			       &requestShutdown, &terminationRequested);
  server->socket_fd = socket(AF_INET, SOCK_RDM, PF_INET);
  bind(server->socket_fd, &(server->address), sizeof(struct sockaddr_in));

#ifdef AGS_WITH_XMLRPC_C
  SocketUnixCreateFd(server->socket_fd, &(server->socket));

  ServerCreateSocket2(&(server->abyss_server), server->socket, &error);
  xmlrpc_server_abyss_set_handlers2(&(server->abyss_server), "/RPC2", registry->registry);
  ServerInit(&(server->abyss_server));
  //  setupSignalHandlers();

  while((AGS_SERVER_RUNNING & (server->flags)) != 0){
    printf("Waiting for next RPC...\n");
    ServerRunOnce(&(server->abyss_server));
    /* This waits for the next connection, accepts it, reads the
       HTTP POST request, executes the indicated RPC, and closes
       the connection.
    */
  } 
#endif /* AGS_WITH_XMLRPC_C */
}

void
ags_server_start(AgsServer *server)
{
  g_return_if_fail(AGS_IS_SERVER(server));

  g_object_ref((GObject *) server);
  g_signal_emit(G_OBJECT(server),
		server_signals[START], 0);
  g_object_unref((GObject *) server);
}

AgsServer*
ags_server_lookup(void *server_info)
{
  GList *current;

  current = ags_server_list;

  while(current != NULL){
    if(server_info == AGS_SERVER(current)->server_info){
      return(AGS_SERVER(current->data));
    }

    current = current->next;
  }

  return(NULL);
}

#ifdef AGS_WITH_XMLRPC_C
xmlrpc_value*
ags_server_create_object(xmlrpc_env *env,
			 xmlrpc_value *param_array,
			 void *server_info)
{
  AgsServer *server;
  GType type;
  GObjectClass *object_class;
  GParameter *parameter;
  GObject *object;
  AgsRegistryEntry *registry_entry;
  gchar *type_name;
  xmlrpc_value *item, *retval;
  guint n_params;
  guint i;

  if(xmlrpc_array_size(env, param_array) % 2 != 1){
    return(NULL);
  }

  server = ags_server_lookup(server_info);

  /* read type */
  xmlrpc_array_read_item(env, param_array, 0, &item);
  xmlrpc_read_string(env, item, &type_name);
  xmlrpc_DECREF(item);

  type = g_type_from_name(type_name);
  object_class = g_type_class_ref(type);

  /* read parameters */
  n_params = (xmlrpc_array_size(env, param_array) - 1) / 2;
  parameter = g_new(GParameter, n_params);

  for(i = 0; i < n_params; i++){
    GParamSpec *pspec;
    AgsRegistryEntry *registry_entry;
    gchar *param_name, *registry_id;
    gchar *error;

    /* read parameter name */
    xmlrpc_array_read_item(env, param_array, 1 + i * 2, &item);
    xmlrpc_read_string(env, item, &param_name);
    xmlrpc_DECREF(item);

    pspec = g_object_class_find_property(object_class,
					 param_name);

    parameter[i].name = param_name;
    parameter[i].value.g_type = 0;

    /* read registry id */
    xmlrpc_array_read_item(env, param_array, 2 + i * 2, &item);
    xmlrpc_read_string(env, item, &registry_id);
    xmlrpc_DECREF(item);

    /* find registry entry */
    registry_entry = ags_registry_entry_find(server->registry,
					     registry_id);

    /* copy GValue from registry entry to parameter array */
    g_value_init(&parameter[i].value, G_PARAM_SPEC_VALUE_TYPE(pspec));
    g_value_copy(&(registry_entry->entry),
		 &parameter[i].value);

    /* free not needed strings */
    g_free(param_name);
    g_free(registry_id);

    if(error){
      g_warning ("%s: %s\0", G_STRFUNC, error);
      g_free (error);
      g_value_unset (&parameter[i].value);
      break;
    }
  }

  /* instantiate object */
  object = g_object_newv(type,
			 n_params,
			 parameter);

  registry_entry = ags_registry_entry_alloc(server->registry);
  g_value_init(&(registry_entry->entry),
	       G_TYPE_OBJECT);
  g_value_set_object(&(registry_entry->entry),
		     object);
  
  /* create return value */
  retval = xmlrpc_string_new(env, registry_entry->id);

  return(retval);
}

xmlrpc_value*
ags_server_object_set_property(xmlrpc_env *env,
			       xmlrpc_value *param_array,
			       void *server_info)
{
  AgsServer *server;
  GObject *object, *property;
  AgsRegistryEntry *registry_entry;
  gchar *param_name, *registry_id;
  xmlrpc_value *item;

  if(xmlrpc_array_size(env, param_array) != 3){
    return(NULL);
  }

  server = ags_server_lookup(server_info);

  /* read registry id */
  xmlrpc_array_read_item(env, param_array, 1, &item);
  xmlrpc_read_string(env, item, &registry_id);
  xmlrpc_DECREF(item);

  registry_entry = ags_registry_entry_find(server->registry,
					   registry_id);
  object = g_value_get_object(&(registry_entry->entry));

  /* read parameter name */
  xmlrpc_array_read_item(env, param_array, 1, &item);
  xmlrpc_read_string(env, item, &param_name);
  xmlrpc_DECREF(item);

  /* read registry id of object to set as property */
  xmlrpc_array_read_item(env, param_array, 1, &item);
  xmlrpc_read_string(env, item, &registry_id);
  xmlrpc_DECREF(item);

  registry_entry = ags_registry_entry_find(server->registry,
					   registry_id);
  property = g_value_get_object(&(registry_entry->entry));
 
  /* set property */
  g_object_set(object,
	       param_name, property,
	       NULL);

  return(NULL);
}
#endif /* AGS_WITH_XMLRPC_C */

AgsServer*
ags_server_new(GObject *main)
{
  AgsServer *server;

  server = (AgsServer *) g_object_new(AGS_TYPE_SERVER,
				      NULL);

  server->main = main;

  return(server);
}
