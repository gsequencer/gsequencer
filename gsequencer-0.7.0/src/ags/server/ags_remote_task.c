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

#include <ags/server/ags_remote_task.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/server/ags_server.h>
#include <ags/server/ags_registry.h>

#include <ags/thread/ags_task.h>

void ags_remote_task_class_init(AgsRemoteTaskClass *remote_task);
void ags_remote_task_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remote_task_init(AgsRemoteTask *remote_task);
void ags_remote_task_add_to_registry(AgsConnectable *connectable);
void ags_remote_task_remove_from_registry(AgsConnectable *connectable);
void ags_remote_task_connect(AgsConnectable *connectable);
void ags_remote_task_disconnect(AgsConnectable *connectable);
void ags_remote_task_finalize(GObject *gobject);

void* ags_remote_task_launch_timed_thread(void *ptr);

static gpointer ags_remote_task_parent_class = NULL;

GType
ags_remote_task_get_type()
{
  static GType ags_type_remote_task = 0;

  if(!ags_type_remote_task){
    static const GTypeInfo ags_remote_task_info = {
      sizeof (AgsRemoteTaskClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remote_task_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoteTask),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remote_task_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remote_task_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remote_task = g_type_register_static(G_TYPE_OBJECT,
					   "AgsRemoteTask\0",
					   &ags_remote_task_info,
					   0);

    g_type_add_interface_static(ags_type_remote_task,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_remote_task);
}

void
ags_remote_task_class_init(AgsRemoteTaskClass *remote_task)
{
  GObjectClass *gobject;

  ags_remote_task_parent_class = g_type_class_peek_parent(remote_task);

  /* GObjectClass */
  gobject = (GObjectClass *) remote_task;

  gobject->finalize = ags_remote_task_finalize;
}

void
ags_remote_task_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_remote_task_add_to_registry;
  connectable->remove_from_registry = ags_remote_task_remove_from_registry;
  connectable->connect = ags_remote_task_connect;
  connectable->disconnect = ags_remote_task_disconnect;
}

void
ags_remote_task_init(AgsRemoteTask *remote_task)
{
  remote_task->server = NULL;
}

void
ags_remote_task_add_to_registry(AgsConnectable *connectable)
{
  AgsServer *server;
  AgsRegistry *registry;
  AgsRemoteTask *remote_task;
  struct xmlrpc_method_info3 *method_info;

  remote_task = AGS_REMOTE_TASK(connectable);

  server = AGS_SERVER(remote_task->server);
  registry = AGS_REGISTRY(server->registry);

#ifdef AGS_WITH_XMLRPC_C
  /* bulk */
  method_info = (struct xmlrpc_method_info3 *) malloc(sizeof(struct xmlrpc_method_info3));
  method_info->methodName = "ags_remote_task_launch\0";
  method_info->methodFunction = &ags_remote_task_launch;
  method_info->serverInfo = NULL;
  xmlrpc_registry_add_method3(&(AGS_MAIN(server->main)->env),
			      registry->registry,
			      method_info);
#endif /* AGS_WITH_XMLRPC_C */
}

void
ags_remote_task_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_remote_task_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_remote_task_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_remote_task_finalize(GObject *gobject)
{
  AgsRemoteTask *remote_task;

  remote_task = AGS_REMOTE_TASK(gobject);

  G_OBJECT_CLASS(ags_remote_task_parent_class)->finalize(gobject);
}

#ifdef AGS_WITH_XMLRPC_C
xmlrpc_value*
ags_remote_task_launch(xmlrpc_env *env,
		       xmlrpc_value *param_array,
		       void *server_info)
{
  AgsServer *server;
  AgsTask *task;
  AgsRegistryEntry *registry_entry;
  gchar *registry_id;
  xmlrpc_value *item;

  if(xmlrpc_array_size(env, param_array) != 1){
    return(NULL);
  }

  server = ags_server_lookup(server_info);

  /* read registry id */
  xmlrpc_array_read_item(env, param_array, 1, &item);
  xmlrpc_read_string(env, item, &registry_id);
  xmlrpc_DECREF(item);

  registry_entry = ags_registry_entry_find(server->registry,
					   registry_id);
  task = (AgsTask *) g_value_get_object(&(registry_entry->entry));

  /* launch */
  ags_task_launch(task);

  return(NULL);
}

void*
ags_remote_task_launch_timed_thread(void *ptr)
{
  //TODO:JK: implement me
}

xmlrpc_value*
ags_remote_task_launch_timed(xmlrpc_env *env,
			     xmlrpc_value *param_array,
			     void *server_info)
{
  AgsServer *server;
  AgsTask *task;
  AgsRegistryEntry *registry_entry;
  gchar *registry_id;
  xmlrpc_value *item;

  if(xmlrpc_array_size(env, param_array) != 1){
    return(NULL);
  }

  server = ags_server_lookup(server_info);

  /* read registry id */
  xmlrpc_array_read_item(env, param_array, 1, &item);
  xmlrpc_read_string(env, item, &registry_id);
  xmlrpc_DECREF(item);

  registry_entry = ags_registry_entry_find(server->registry,
					   registry_id);
  task = (AgsTask *) g_value_get_object(&(registry_entry->entry));

  /* launch timed */
  //TODO:JK: implement me
}
#endif /* AGS_WITH_XMLRPC_C */

AgsRemoteTask*
ags_remote_task_new()
{
  AgsRemoteTask *remote_task;

  remote_task = (AgsRemoteTask *) g_object_new(AGS_TYPE_REMOTE_TASK,
					       NULL);

  return(remote_task);
}
