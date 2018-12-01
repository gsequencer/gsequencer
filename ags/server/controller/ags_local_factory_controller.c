/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/server/controller/ags_local_factory_controller.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_marshal.h>

#include <ags/server/ags_service_provider.h>
#include <ags/server/ags_server.h>
#include <ags/server/ags_registry.h>

#include <ags/server/security/ags_authentication_manager.h>

void ags_local_factory_controller_class_init(AgsLocalFactoryControllerClass *local_factory_controller);
void ags_local_factory_controller_init(AgsLocalFactoryController *local_factory_controller);
void ags_local_factory_controller_finalize(GObject *gobject);

gpointer ags_local_factory_controller_real_create_instance(AgsLocalFactoryController *local_factory_controller,
							   GType type_name,
							   guint n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_local_factory_controller
 * @short_description: local factory controller
 * @title: AgsLocalFactoryController
 * @section_id:
 * @include: ags/server/controller/ags_local_factory_controller.h
 *
 * The #AgsLocalFactoryController is a controller.
 */

enum{
  CREATE_INSTANCE,
  LAST_SIGNAL,
};

static gpointer ags_local_factory_controller_parent_class = NULL;
static guint local_factory_controller_signals[LAST_SIGNAL];

GType
ags_local_factory_controller_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_local_factory_controller = 0;

    static const GTypeInfo ags_local_factory_controller_info = {
      sizeof (AgsLocalFactoryControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_local_factory_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLocalFactoryController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_local_factory_controller_init,
    };
    
    ags_type_local_factory_controller = g_type_register_static(G_TYPE_OBJECT,
							       "AgsLocalFactoryController",
							       &ags_local_factory_controller_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_local_factory_controller);
  }

  return g_define_type_id__volatile;
}

void
ags_local_factory_controller_class_init(AgsLocalFactoryControllerClass *local_factory_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_local_factory_controller_parent_class = g_type_class_peek_parent(local_factory_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) local_factory_controller;

  gobject->finalize = ags_local_factory_controller_finalize;

  /* AgsLocalFactoryController */
  local_factory_controller->create_instance = ags_local_factory_controller_real_create_instance;

  /* signals */
  /**
   * AgsLocalFactoryController::create-instance:
   * @local_factory_controller: the #AgsLocalFactoryController
   * @type_name: the type name
   * @n_params: the parameter name and value count
   * @parameter_name: the parameter names
   * @value: the #GValue-struct array related to parameter names
   *
   * The ::create-instance signal is used to create an instance.
   *
   * Returns: the response
   * 
   * Since: 2.0.0
   */
  local_factory_controller_signals[CREATE_INSTANCE] =
    g_signal_new("create-instance",
		 G_TYPE_FROM_CLASS(local_factory_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalFactoryControllerClass, create_instance),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__ULONG_UINT_POINTER_POINTER,
		 G_TYPE_POINTER, 4,
		 G_TYPE_ULONG,
		 G_TYPE_UINT,
		 G_TYPE_POINTER,
		 G_TYPE_POINTER);
}

void
ags_local_factory_controller_init(AgsLocalFactoryController *local_factory_controller)
{
  gchar *context_path;

  context_path = g_strdup_printf("%s%s",
				 AGS_CONTROLLER_BASE_PATH,
				 AGS_LOCAL_FACTORY_CONTROLLER_CONTEXT_PATH);
  g_object_set(local_factory_controller,
	       "context-path", context_path,
	       NULL);
  g_free(context_path);
  
  //TODO:JK: implement me
}

void
ags_local_factory_controller_finalize(GObject *gobject)
{
  AgsLocalFactoryController *local_factory_controller;

  local_factory_controller = AGS_LOCAL_FACTORY_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_local_factory_controller_parent_class)->finalize(gobject);
}

gpointer
ags_local_factory_controller_real_create_instance(AgsLocalFactoryController *local_factory_controller,
						  GType gtype,
						  guint n_params, gchar **parameter_name, GValue *value)
{
  AgsServer *server;
  AgsRegistry *registry;

  AgsApplicationContext *application_context;

  GObject *gobject;
  
  AgsRegistryEntry *registry_entry;
  gpointer response;
  
  server = AGS_CONTROLLER(local_factory_controller)->server;

  application_context = server->application_context;

  registry = ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));
  
  response = NULL;
  
#if 0
  /* instantiate object */
  gobject = g_object_newv(gtype,
			  n_params,
			  parameter);

  registry_entry = ags_registry_entry_alloc(registry);
  g_value_init(&(registry_entry->entry),
	       G_TYPE_OBJECT);
  g_value_set_object(&(registry_entry->entry),
		     gobject);
  
  /* create return value */
#ifdef AGS_WITH_XMLRPC_C
  response = xmlrpc_string_new(ags_service_provider_get_env(AGS_SERVICE_PROVIDER(application_context)),
			       registry_entry->id);
#else
  response = NULL;
#endif
#endif
  
  return(response);
}

/**
 * ags_local_factory_controller_create_instance:
 * @local_factory_controller: the #AgsLocalFactoryController
 * @gtype: the type name
 * @n_params: the parameter name and value count
 * @parameter_name: the parameter names
 * @value: the #GValue-struct array related to parameter names
 * 
 * Creates an instance of @gtype and passes @parameter to g_object_newv()
 * 
 * Returns: the response
 * 
 * Since: 2.0.0
 */
gpointer
ags_local_factory_controller_create_instance(AgsLocalFactoryController *local_factory_controller,
					     GType gtype,
					     guint n_params, gchar **parameter_name, GValue *value)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_LOCAL_FACTORY_CONTROLLER(local_factory_controller),
		       NULL);

  g_object_ref((GObject *) local_factory_controller);
  g_signal_emit(G_OBJECT(local_factory_controller),
		local_factory_controller_signals[CREATE_INSTANCE], 0,
		gtype,
		n_params, parameter_name, value,
		&retval);
  g_object_unref((GObject *) local_factory_controller);

  return(retval);
}

/**
 * ags_local_factory_controller_new:
 * 
 * Instantiate new #AgsLocalFactoryController
 * 
 * Returns: the #AgsLocalFactoryController
 * 
 * Since: 2.0.0
 */
AgsLocalFactoryController*
ags_local_factory_controller_new()
{
  AgsLocalFactoryController *local_factory_controller;

  local_factory_controller = (AgsLocalFactoryController *) g_object_new(AGS_TYPE_LOCAL_FACTORY_CONTROLLER,
									NULL);

  return(local_factory_controller);
}
