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

#include <ags/server/ags_registry.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/server/ags_server.h>

#include <ags/i18n.h>

void ags_registry_class_init(AgsRegistryClass *registry);
void ags_registry_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_registry_init(AgsRegistry *registry);
void ags_registry_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec);
void ags_registry_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec);
void ags_registry_add_to_registry(AgsConnectable *connectable);
void ags_registry_remove_from_registry(AgsConnectable *connectable);
void ags_registry_connect(AgsConnectable *connectable);
void ags_registry_disconnect(AgsConnectable *connectable);
void ags_registry_dispose(GObject *gobject);
void ags_registry_finalize(GObject *gobject);

/**
 * SECTION:ags_registry
 * @short_description: remote control registry
 * @title: AgsRegistry
 * @section_id:
 * @include: ags/server/ags_registry.h
 *
 * The #AgsRegistry is a registry where you are able to lookup objects.
 */

enum{
  PROP_0,
  PROP_SERVER,
};

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
					       "AgsRegistry",
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
  GParamSpec *param_spec;

  ags_registry_parent_class = g_type_class_peek_parent(registry);

  /* GObjectClass */
  gobject = (GObjectClass *) registry;

  gobject->set_property = ags_registry_set_property;
  gobject->get_property = ags_registry_get_property;

  gobject->dispose = ags_registry_dispose;
  gobject->finalize = ags_registry_finalize;

  /* properties */
  /**
   * AgsRegistry:server:
   *
   * The assigned #AgsServer
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("server",
				   i18n("assigned server"),
				   i18n("The assigned server"),
				   AGS_TYPE_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER,
				  param_spec);
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
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  registry->flags = 0;

  registry->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(registry->mutexattr);
  pthread_mutexattr_settype(registry->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(registry->mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif
  
  registry->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(registry->mutex,
		     registry->mutexattr);

  registry->previous = NULL;
  registry->current = NULL;

#ifdef AGS_WITH_XMLRPC_C
  registry->registry = xmlrpc_registry_new(ags_service_provider_get_env(AGS_SERVICE_PROVIDER(application_context)));
#else
  registry->registry = NULL;
#endif
  
  registry->id_length = AGS_REGISTRY_DEFAULT_ID_LENGTH;
  registry->counter = 0;
  
  registry->entry = NULL;
}

void
ags_registry_set_property(GObject *gobject,
			  guint prop_id,
			  const GValue *value,
			  GParamSpec *param_spec)
{
  AgsRegistry *registry;

  registry = AGS_REGISTRY(gobject);
  
  switch(prop_id){
  case PROP_SERVER:
    {
      AgsServer *server;

      server = (AgsServer *) g_value_get_object(value);

      if(registry->server == (GObject *) server){
	return;
      }

      if(registry->server != NULL){
	g_object_unref(G_OBJECT(registry->server));
      }

      if(server != NULL){
	g_object_ref(G_OBJECT(server));
      }
      
      registry->server = server;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_registry_get_property(GObject *gobject,
			  guint prop_id,
			  GValue *value,
			  GParamSpec *param_spec)
{
  AgsRegistry *registry;

  registry = AGS_REGISTRY(gobject);
  
  switch(prop_id){
  case PROP_SERVER:
    {
      g_value_set_object(value, registry->server);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_registry_add_to_registry(AgsConnectable *connectable)
{
  AgsRegistry *registry;

  registry = AGS_REGISTRY(connectable);

  //TODO:JK: implement me
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
ags_registry_dispose(GObject *gobject)
{
  AgsRegistry *registry;

  registry = AGS_REGISTRY(gobject);

  if(registry->previous != NULL){
    g_object_unref(registry->previous);
    
    registry->previous = NULL;
  }

  if(registry->current != NULL){
    g_object_unref(registry->current);

    registry->current = NULL;
  }
    
  if(registry->server != NULL){
    g_object_unref(registry->server);

    registry->server = NULL;
  }

  g_list_free_full(registry->entry,
		   ags_registry_entry_free);

  registry->entry = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_registry_parent_class)->dispose(gobject);
}

void
ags_registry_finalize(GObject *gobject)
{
  AgsRegistry *registry;

  registry = AGS_REGISTRY(gobject);

  if(registry->previous != NULL){
    g_object_unref(registry->previous);
  }

  if(registry->current != NULL){
    g_object_unref(registry->current);
  }
    
  if(registry->server != NULL){
    g_object_unref(registry->server);
  }

  g_list_free_full(registry->entry,
		   ags_registry_entry_free);
  
  /* mutex */
  pthread_mutex_destroy(registry->mutex);
  free(registry->mutex);

  pthread_mutexattr_destroy(registry->mutexattr);
  free(registry->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_registry_parent_class)->finalize(gobject);
}

AgsRegistryEntry*
ags_registry_entry_alloc(AgsRegistry *registry)
{
  AgsRegistryEntry *registry_entry;

  registry_entry = (AgsRegistryEntry *) malloc(sizeof(AgsRegistryEntry));

  registry_entry->id = ags_id_generator_create_uuid();
  memset(&(registry_entry->entry), 0, sizeof(GValue));
  g_value_init(&(registry_entry->entry),
	       G_TYPE_OBJECT);

  return(registry_entry);
}

void
ags_registry_entry_free(AgsRegistryEntry *registry_entry)
{
  GObject *gobject;

  gobject = g_value_get_object(&(registry_entry->entry));

  if(gobject != NULL){
    g_object_unref(gobject);
  }
  
  g_value_unset(&(registry_entry->entry));

  free(registry_entry);
}

void
ags_registry_add(AgsRegistry *registry,
		 AgsRegistryEntry *registry_entry)
{
  pthread_mutex_lock(registry->mutex);

  registry->entry = g_list_prepend(registry->entry,
				   registry_entry);

  pthread_mutex_unlock(registry->mutex);
}

AgsRegistryEntry*
ags_registry_entry_find(AgsRegistry *registry,
			gchar *id)
{
  GList *current;
  AgsRegistryEntry *entry;

  pthread_mutex_lock(registry->mutex);

  current = registry->entry;
  
  while(current != NULL){
    entry = (AgsRegistryEntry *) current->data;

    if(!g_ascii_strncasecmp(entry->id,
			    id,
			    registry->id_length)){
      return(entry);
    }

    current = current->next;
  }

  pthread_mutex_unlock(registry->mutex);

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
