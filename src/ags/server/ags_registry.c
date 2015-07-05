/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/server/ags_registry.h>

#include <stdlib.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/server/ags_server.h>

void ags_registry_class_init(AgsRegistryClass *registry);
void ags_registry_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_registry_init(AgsRegistry *registry);
void ags_registry_add_to_registry(AgsConnectable *connectable);
void ags_registry_remove_from_registry(AgsConnectable *connectable);
void ags_registry_connect(AgsConnectable *connectable);
void ags_registry_disconnect(AgsConnectable *connectable);
void ags_registry_finalize(GObject *gobject);

static gpointer ags_registry_parent_class = NULL;

GType
ags_registry_get_type()
{
  static GType ags_type_registry = 0;

  if(!ags_type_registry){
    static const GTypeInfo ags_registry_info = {
      sizeof (AgsRegistryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_registry_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRegistry),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_registry_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_registry_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_registry = g_type_register_static(G_TYPE_OBJECT,
					       "AgsRegistry\0",
					       &ags_registry_info,
					       0);

    g_type_add_interface_static(ags_type_registry,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_registry);
}

void
ags_registry_class_init(AgsRegistryClass *registry)
{
  GObjectClass *gobject;

  ags_registry_parent_class = g_type_class_peek_parent(registry);

  /* GObjectClass */
  gobject = (GObjectClass *) registry;

  gobject->finalize = ags_registry_finalize;
}

void
ags_registry_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_registry_add_to_registry;
  connectable->remove_from_registry = ags_registry_remove_from_registry;
  connectable->connect = ags_registry_connect;
  connectable->disconnect = ags_registry_disconnect;
}

void
ags_registry_init(AgsRegistry *registry)
{
  registry->flags = 0;

  pthread_mutex_init(&(registry->mutex),
		     NULL);

  registry->previous = NULL;
  registry->current = NULL;

#ifdef AGS_WITH_XMLRPC_C
  registry->registry = NULL;
#endif
  
  registry->id_length = AGS_REGISTRY_DEFAULT_ID_LENGTH;
  registry->counter = 0;
  
  registry->entry = NULL;
}

void
ags_registry_add_to_registry(AgsConnectable *connectable)
{
  AgsServer *server;
  AgsRegistry *registry;
  struct xmlrpc_method_info3 *method_info;

  registry = AGS_REGISTRY(connectable);
  server = AGS_SERVER(registry->server);

#ifdef AGS_WITH_XMLRPC_C
  /* bulk */
  method_info = (struct xmlrpc_method_info3 *) malloc(sizeof(struct xmlrpc_method_info3));
  method_info->methodName = "ags_registry_entry_bulk\0";
  method_info->methodFunction = &ags_registry_entry_bulk;
  method_info->serverInfo = NULL;
  xmlrpc_registry_add_method3(&(AGS_MAIN(server->main)->env),
			      registry->registry,
			      method_info);
#endif /* AGS_WITH_XMLRPC_C */
}

void
ags_registry_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_registry_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_registry_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_registry_finalize(GObject *gobject)
{
  AgsRegistry *registry;

  registry = AGS_REGISTRY(gobject);

  G_OBJECT_CLASS(ags_registry_parent_class)->finalize(gobject);
}

AgsRegistryEntry*
ags_registry_entry_alloc(AgsRegistry *registry)
{
  AgsRegistryEntry *registry_entry;

  registry_entry = (AgsRegistryEntry *) malloc(sizeof(AgsRegistryEntry));

  registry_entry->id = ags_id_generator_create_uuid();

  return(registry_entry);
}

void
ags_registry_add(AgsRegistry *registry,
		 AgsRegistryEntry *registry_entry)
{
  pthread_mutex_lock(&(registry->mutex));

  registry->entry = g_list_prepend(registry->entry,
				   registry_entry);

  pthread_mutex_unlock(&(registry->mutex));
}

AgsRegistryEntry*
ags_registry_entry_find(AgsRegistry *registry,
			gchar *id)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  GList *current;
  AgsRegistryEntry *entry;

  pthread_mutex_lock(&(registry->mutex));

  current = registry->entry;
  
  while(current != NULL){
    entry = (AgsRegistryEntry *) current->data;

    if(!strncmp(entry->id,
		id,
		registry->id_length)){
      return(entry);
    }

    current = current->next;
  }

  pthread_mutex_unlock(&(registry->mutex));

  return(entry);
}

#ifdef AGS_WITH_XMLRPC_C
xmlrpc_value*
ags_registry_entry_bulk(xmlrpc_env *env,
			xmlrpc_value *param_array,
			void *server_info)
{
  AgsServer *server;
  AgsRegistry *registry;
  AgsRegistryEntry *entry;
  GList *current;
  xmlrpc_value *bulk;
  xmlrpc_value *item;

  server = ags_server_lookup(server_info);
  registry = server->registry;

  bulk = xmlrpc_array_new(env);

  pthread_mutex_lock(&(registry->mutex));

  current = registry->entry;

  while(current != NULL){
    entry = (AgsRegistryEntry *) current->data;
    item = xmlrpc_string_new(env, entry->id);

    xmlrpc_array_append_item(env, bulk, item);

    current = current->next;
  }

  pthread_mutex_unlock(&(registry->mutex));

  return(bulk);
}
#endif /* AGS_WITH_XMLRPC_C */

AgsRegistry*
ags_registry_new()
{
  AgsRegistry *registry;

  registry = (AgsRegistry *) g_object_new(AGS_TYPE_REGISTRY,
					  NULL);

  return(registry);
}
