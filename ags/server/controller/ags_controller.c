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

#include <ags/server/controller/ags_controller.h>

#include <ags/server/ags_server.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_controller_class_init(AgsControllerClass *controller);
void ags_controller_init(AgsController *controller);
void ags_controller_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_controller_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_controller_dispose(GObject *gobject);
void ags_controller_finalize(GObject *gobject);

/**
 * SECTION:ags_controller
 * @short_description: base controller
 * @title: AgsController
 * @section_id:
 * @include: ags/server/controller/ags_controller.h
 *
 * The #AgsController is a base object to implement controllers.
 */

enum{
  PROP_0,
  PROP_SERVER,
  PROP_CONTEXT_PATH,
};

static gpointer ags_controller_parent_class = NULL;

GType
ags_controller_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_controller = 0;

    static const GTypeInfo ags_controller_info = {
      sizeof (AgsControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_controller_init,
    };
    
    ags_type_controller = g_type_register_static(G_TYPE_OBJECT,
						 "AgsController",
						 &ags_controller_info,
						 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_controller);
  }

  return g_define_type_id__static;
}

void
ags_controller_class_init(AgsControllerClass *controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_controller_parent_class = g_type_class_peek_parent(controller);

  /* GObjectClass */
  gobject = (GObjectClass *) controller;

  gobject->set_property = ags_controller_set_property;
  gobject->get_property = ags_controller_get_property;

  gobject->dispose = ags_controller_dispose;
  gobject->finalize = ags_controller_finalize;

  /* properties */
  /**
   * AgsController:server:
   *
   * The assigned #AgsServer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("server",
				   i18n("assigned server"),
				   i18n("The assigned server"),
				   AGS_TYPE_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER,
				  param_spec);
  
  /**
   * AgsController:context-path:
   *
   * The context path provided.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("context-path",
				   i18n_pspec("context path to provide"),
				   i18n_pspec("The context path provided by this controller"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONTEXT_PATH,
				  param_spec);
}

void
ags_controller_init(AgsController *controller)
{
  g_rec_mutex_init(&(controller->obj_mutex));

  controller->server = NULL;

  controller->context_path = NULL;
  
  controller->resource = g_hash_table_new_full(g_str_hash,
					       g_str_equal,
					       g_free,
					       (GDestroyNotify) ags_controller_resource_unref);
}

void
ags_controller_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsController *controller;

  GRecMutex *controller_mutex;

  controller = AGS_CONTROLLER(gobject);

  /* get controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(controller);
  
  switch(prop_id){
  case PROP_SERVER:
    {
      AgsServer *server;

      server = (AgsServer *) g_value_get_object(value);

      g_rec_mutex_lock(controller_mutex);

      if(controller->server == (GObject *) server){
	g_rec_mutex_unlock(controller_mutex);

	return;
      }

      if(controller->server != NULL){
	g_object_unref(G_OBJECT(controller->server));
      }

      if(server != NULL){
	g_object_ref(G_OBJECT(server));
      }
      
      controller->server = (GObject *) server;

      g_rec_mutex_unlock(controller_mutex);
    }
    break;
  case PROP_CONTEXT_PATH:
    {
      char *context_path;

      context_path = (char *) g_value_get_string(value);

      g_rec_mutex_lock(controller_mutex);

      controller->context_path = g_strdup(context_path);

      g_rec_mutex_unlock(controller_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_controller_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsController *controller;

  GRecMutex *controller_mutex;

  controller = AGS_CONTROLLER(gobject);

  /* get controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(controller);
  
  switch(prop_id){
  case PROP_SERVER:
    {
      g_rec_mutex_lock(controller_mutex);
      
      g_value_set_object(value, controller->server);

      g_rec_mutex_unlock(controller_mutex);
    }
    break;
  case PROP_CONTEXT_PATH:
    {
      g_rec_mutex_lock(controller_mutex);
      
      g_value_set_string(value, controller->context_path);

      g_rec_mutex_unlock(controller_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_controller_dispose(GObject *gobject)
{
  AgsController *controller;

  controller = AGS_CONTROLLER(gobject);

  if(controller->server != NULL){
    g_object_unref(controller->server);

    controller->server = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_controller_parent_class)->dispose(gobject);
}

void
ags_controller_finalize(GObject *gobject)
{
  AgsController *controller;

  controller = AGS_CONTROLLER(gobject);

  if(controller->server != NULL){
    g_object_unref(controller->server);
  }

  g_free(controller->context_path);

  g_hash_table_destroy(controller->resource);
  
  /* call parent */
  G_OBJECT_CLASS(ags_controller_parent_class)->finalize(gobject);
}

/**
 * ags_controller_resource_alloc:
 * @group_id: the group id
 * @user_id: the user id
 * @access_mode: the access mode
 * 
 * Allocated #AgsControllerResource-struct.
 * 
 * Returns: (type gpointer) (transfer none): the newly allocated #AgsControllerResource-struct
 * 
 * Since: 3.0.0
 */
AgsControllerResource*
ags_controller_resource_alloc(gchar *group_id, gchar *user_id,
			      guint access_mode)
{
  AgsControllerResource *controller_resource;

  controller_resource = (AgsControllerResource *) malloc(sizeof(AgsControllerResource));

  controller_resource->ref_count = 1;

  controller_resource->group_id = g_strdup(group_id);
  controller_resource->user_id = g_strdup(user_id);

  controller_resource->access_mode = access_mode;
  
  return(controller_resource);
}

/**
 * ags_controller_resource_free:
 * @controller_resource: (type gpointer): the #AgsControllerResource-struct
 * 
 * Free @controller_resource.
 * 
 * Since: 3.0.0
 */
void
ags_controller_resource_free(AgsControllerResource *controller_resource)
{
  if(controller_resource == NULL){
    return;
  }
  
  g_free(controller_resource->group_id);
  g_free(controller_resource->user_id);

  g_free(controller_resource);
}

/**
 * ags_controller_resource_ref:
 * @controller_resource: (type gpointer): the #AgsControllerResource-struct
 * 
 * Increase ref-count of @controller_resource.
 * 
 * Since: 3.0.0
 */
void
ags_controller_resource_ref(AgsControllerResource *controller_resource)
{
  if(controller_resource == NULL){
    return;
  }

  controller_resource->ref_count += 1;
}

/**
 * ags_controller_resource_unref:
 * @controller_resource: (type gpointer): the #AgsControllerResource-struct
 * 
 * Decrease ref-count of @controller_resource and free it if ref-count drops to 0.
 * 
 * Since: 3.0.0
 */
void
ags_controller_resource_unref(AgsControllerResource *controller_resource)
{
  if(controller_resource == NULL){
    return;
  }

  controller_resource->ref_count -= 1;

  if(controller_resource->ref_count <= 0){
    ags_controller_resource_free(controller_resource);
  }
}

/**
 * ags_controller_add_resource:
 * @controller: the #AgsController
 * @resource_name: the resource name as string
 * @controller_resource: (type gpointer): the #AgsControllerResource-struct
 * 
 * Add @controller_resource with key @resource_name to hash table.
 * 
 * Since: 3.0.0
 */
void
ags_controller_add_resource(AgsController *controller,
			    gchar *resource_name, AgsControllerResource *controller_resource)
{
  GRecMutex *controller_mutex;

  if(!AGS_IS_CONTROLLER(controller) ||
     resource_name == NULL ||
     controller_resource == NULL){
    return;
  }
  
  /* get controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(controller);

  /* add resource */
  g_rec_mutex_lock(controller_mutex);

  ags_controller_resource_ref(controller_resource);
  
  g_hash_table_insert(controller->resource,
		      g_strdup(resource_name), controller_resource);

  g_rec_mutex_unlock(controller_mutex);
}

/**
 * ags_controller_remove_resource:
 * @controller: the #AgsController
 * @resource_name: the resource name as string
 * 
 * Remove key @resource_name from hash table.
 * 
 * Since: 3.0.0
 */
void
ags_controller_remove_resource(AgsController *controller,
			       gchar *resource_name)
{
  GRecMutex *controller_mutex;

  if(!AGS_IS_CONTROLLER(controller) ||
     resource_name == NULL){
    return;
  }
  
  /* get controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(controller);

  /* remove resource */
  g_rec_mutex_lock(controller_mutex);

  g_hash_table_remove(controller->resource,
		      resource_name);

  g_rec_mutex_unlock(controller_mutex);
}

/**
 * ags_controller_lookup_resource:
 * @controller: the #AgsController
 * @resource_name: the resource name as string
 * 
 * Lookup key @resource_name in hash table.
 * 
 * Returns: (type gpointer) (transfer none): the matching #AgsControllerResource-struct
 * 
 * Since: 3.0.0
 */
AgsControllerResource*
ags_controller_lookup_resource(AgsController *controller,
			       gchar *resource_name)
{
  AgsControllerResource *controller_resource;

  GRecMutex *controller_mutex;

  if(!AGS_IS_CONTROLLER(controller) ||
     resource_name == NULL){
    return(NULL);
  }

  /* get controller mutex */
  controller_mutex = AGS_CONTROLLER_GET_OBJ_MUTEX(controller);

  /* lookup resource */
  g_rec_mutex_lock(controller_mutex);

  controller_resource = (AgsControllerResource *) g_hash_table_lookup(controller->resource,
								      resource_name);
  
  ags_controller_resource_ref(controller_resource);
  
  g_rec_mutex_unlock(controller_mutex);

  return(controller_resource);
}

/**
 * ags_controller_query_security_context:
 * @controller: the #AgsController
 * @security_context: the #AgsSecurityContext
 * @login: the login to query
 * 
 * Query @security_context for @login.
 * 
 * Returns: %TRUE if allowed to proceed, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_controller_query_security_context(AgsController *controller,
				      GObject *security_context, gchar *login)
{
  //TODO:JK: implement me

  return(FALSE);
}

/**
 * ags_controller_new:
 * 
 * Instantiate new #AgsController
 * 
 * Returns: the #AgsController
 * 
 * Since: 3.0.0
 */
AgsController*
ags_controller_new()
{
  AgsController *controller;

  controller = (AgsController *) g_object_new(AGS_TYPE_CONTROLLER,
					      NULL);

  return(controller);
}
