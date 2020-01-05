/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/object/ags_application_context.h>

#include <ags/lib/ags_log.h>

#include <ags/file/ags_file.h>

#include <ags/object/ags_connectable.h>

#include <gio/gio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <ags/i18n.h>

void ags_application_context_class_init(AgsApplicationContextClass *application_context);
void ags_application_context_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_application_context_init(AgsApplicationContext *application_context);
void ags_application_context_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_application_context_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_application_context_dispose(GObject *gobject);
void ags_application_context_finalize(GObject *gobject);

AgsUUID* ags_application_context_get_uuid(AgsConnectable *connectable);
gboolean ags_application_context_has_resource(AgsConnectable *connectable);
gboolean ags_application_context_is_ready(AgsConnectable *connectable);
void ags_application_context_add_to_registry(AgsConnectable *connectable);
void ags_application_context_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_application_context_list_resource(AgsConnectable *connectable);
xmlNode* ags_application_context_xml_compose(AgsConnectable *connectable);
void ags_application_context_xml_parse(AgsConnectable *connectable,
				       xmlNode *node);
gboolean ags_application_context_is_connected(AgsConnectable *connectable);
void ags_application_context_connect(AgsConnectable *connectable);
void ags_application_context_disconnect(AgsConnectable *connectable);

void ags_application_context_real_load_config(AgsApplicationContext *application_context);

void ags_application_context_real_prepare(AgsApplicationContext *application_context);
void ags_application_context_real_setup(AgsApplicationContext *application_context);

void ags_application_context_real_register_types(AgsApplicationContext *application_context);

void ags_application_context_real_quit(AgsApplicationContext *application_context);

/**
 * SECTION:ags_application_context
 * @short_description: The application context
 * @title: AgsApplicationContext
 * @section_id:
 * @include: ags/object/ags_application_context.h
 *
 * #AgsApplicationContext is a context provider is your and libraries entry
 * point to the application. You might subtype it to implement your own contices.
 * Thus you should consider to create a provider interface for reusability.
 */

enum{
  LOAD_CONFIG,
  PREPARE,
  SETUP,
  REGISTER_TYPES,
  QUIT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_MAIN_LOOP,
  PROP_TASK_LAUNCHER,
  PROP_CONFIG,
  PROP_FILE,
};

static gpointer ags_application_context_parent_class = NULL;
static guint application_context_signals[LAST_SIGNAL];

AgsApplicationContext *ags_application_context = NULL;

GType
ags_application_context_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_application_context = 0;

    static const GTypeInfo ags_application_context_info = {
      sizeof (AgsApplicationContextClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_application_context_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplicationContext),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_application_context_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_application_context_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_application_context = g_type_register_static(G_TYPE_OBJECT,
							  "AgsApplicationContext",
							  &ags_application_context_info,
							  0);

    g_type_add_interface_static(ags_type_application_context,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_application_context);
  }

  return g_define_type_id__volatile;
}

void
ags_application_context_class_init(AgsApplicationContextClass *application_context)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_application_context_parent_class = g_type_class_peek_parent(application_context);

  /* GObjectClass */
  gobject = (GObjectClass *) application_context;

  gobject->set_property = ags_application_context_set_property;
  gobject->get_property = ags_application_context_get_property;

  gobject->dispose = ags_application_context_dispose;
  gobject->finalize = ags_application_context_finalize;

  /* properties */
  /**
   * AgsApplicationContext:main-loop:
   *
   * The assigned main-loop.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("main-loop",
				   i18n_pspec("main loop of application context"),
				   i18n_pspec("The main loop of application context running"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_LOOP,
				  param_spec);

  /**
   * AgsApplicationContext:task-launcher:
   *
   * The assigned task launcher.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("task-launcher",
				   i18n_pspec("task launcher"),
				   i18n_pspec("The task launcher"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TASK_LAUNCHER,
				  param_spec);

  /**
   * AgsApplicationContext:config:
   *
   * The assigned config.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("config",
				   i18n_pspec("config of application context"),
				   i18n_pspec("The config what application context is running in"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONFIG,
				  param_spec);

  /**
   * AgsApplicationContext:file:
   *
   * The assigned file.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("file",
				   i18n_pspec("file of application context"),
				   i18n_pspec("The file what application context does persist"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE,
				  param_spec);

  /* AgsApplicationContextClass */
  application_context->load_config = ags_application_context_real_load_config;

  application_context->prepare = ags_application_context_real_prepare;
  application_context->setup = ags_application_context_real_setup;

  application_context->register_types = ags_application_context_real_register_types;
  
  application_context->quit = ags_application_context_real_quit;

  /* signals */
  /**
   * AgsApplicationContext::load-config:
   * @application_context: the #AgsApplicationContext
   *
   * The ::load-config notifies to load configuration.
   *
   * Since: 3.0.0
   */
  application_context_signals[LOAD_CONFIG] =
    g_signal_new("load-config",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, load_config),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsApplicationContext::prepare:
   * @application_context: the  #AgsApplicationContext
   *
   * The ::prepare signal should be implemented to prepare
   * your application context.
   *
   * Since: 3.0.0
   */
  application_context_signals[PREPARE] =
    g_signal_new("prepare",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, prepare),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsApplicationContext::setup:
   * @application_context: the  #AgsApplicationContext
   *
   * The ::setup signal should be implemented to setup
   * your application context.
   *
   * Since: 3.0.0
   */
  application_context_signals[SETUP] =
    g_signal_new("setup",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, setup),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsApplicationContext::register-types:
   * @application_context: the  #AgsApplicationContext
   *
   * The ::register-types signal should be implemented to load
   * your types.
   *
   * Since: 3.0.0
   */
  application_context_signals[REGISTER_TYPES] =
    g_signal_new("register-types",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, register_types),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsApplicationContext::quit:
   * @application_context: the #AgsApplicationContext
   *
   * The ::quit notifies to load configuration.
   *
   * Since: 3.0.0
   */
  application_context_signals[QUIT] =
    g_signal_new("quit",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, quit),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_application_context_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_application_context_get_uuid;
  connectable->has_resource = ags_application_context_has_resource;

  connectable->is_ready = ags_application_context_is_ready;
  connectable->add_to_registry = ags_application_context_add_to_registry;
  connectable->remove_from_registry = ags_application_context_remove_from_registry;

  connectable->list_resource = ags_application_context_list_resource;
  connectable->xml_compose = ags_application_context_xml_compose;
  connectable->xml_parse = ags_application_context_xml_parse;

  connectable->is_connected = ags_application_context_is_connected;
  connectable->connect = ags_application_context_connect;
  connectable->disconnect = ags_application_context_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_application_context_init(AgsApplicationContext *application_context)
{
  GFile *file;

  application_context->flags = 0;
  
  g_rec_mutex_init(&(application_context->obj_mutex));

  /* uuid */
  application_context->uuid = ags_uuid_alloc();
  ags_uuid_generate(application_context->uuid);
  
  application_context->argc = 0;
  application_context->argv = NULL;
  
  application_context->version = g_strdup(AGS_VERSION);
  application_context->build_id = g_strdup(AGS_BUILD_ID);

  application_context->log = NULL;
  
  application_context->domain = NULL;
  
  application_context->config = NULL;

  application_context->main_loop = NULL;
  application_context->task_launcher = NULL;
  
  application_context->file = NULL;  
  application_context->history = NULL;
}

void
ags_application_context_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;
  
  application_context = AGS_APPLICATION_CONTEXT(gobject);

  /* get application context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);
  
  switch(prop_id){
  case PROP_MAIN_LOOP:
    {
      GObject *main_loop;
      
      main_loop = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(application_context_mutex);
      
      if(main_loop == application_context->main_loop){  
	g_rec_mutex_unlock(application_context_mutex);

	return;
      }

      if(application_context->main_loop != NULL){
	g_object_unref(application_context->main_loop);
      }
      
      if(main_loop != NULL){
	g_object_ref(G_OBJECT(main_loop));
      }
      
      application_context->main_loop = main_loop;
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  case PROP_TASK_LAUNCHER:
    {
      GObject *task_launcher;
      
      task_launcher = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(application_context_mutex);
      
      if(task_launcher == application_context->task_launcher){  
	g_rec_mutex_unlock(application_context_mutex);

	return;
      }

      if(application_context->task_launcher != NULL){
	g_object_unref(application_context->task_launcher);
      }
      
      if(task_launcher != NULL){
	g_object_ref(G_OBJECT(task_launcher));
      }
      
      application_context->task_launcher = task_launcher;
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  case PROP_CONFIG:
    {
      AgsConfig *config;
      
      config = (AgsConfig *) g_value_get_object(value);

      g_rec_mutex_lock(application_context_mutex);
      
      if(config == application_context->config){  
	g_rec_mutex_unlock(application_context_mutex);

	return;
      }

      if(application_context->config != NULL){
	g_object_unref(application_context->config);
      }
      
      if(config != NULL){
	g_object_ref(G_OBJECT(config));
      }
      
      application_context->config = config;
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  case PROP_FILE:
    {
      AgsFile *file;
      
      file = (AgsFile *) g_value_get_object(value);

      g_rec_mutex_lock(application_context_mutex);

      if(file == application_context->file){
	g_rec_mutex_unlock(application_context_mutex);
	
	return;
      }

      if(application_context->file != NULL){
	g_object_unref(application_context->file);
      }
      
      if(file != NULL){
	g_object_ref(G_OBJECT(file));
      }
      
      application_context->file = (AgsFile *) file;
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
  
}

void
ags_application_context_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsApplicationContext *application_context;

  GRecMutex *application_context_mutex;
  
  application_context = AGS_APPLICATION_CONTEXT(gobject);

  /* get application context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  switch(prop_id){
  case PROP_MAIN_LOOP:
    {
      g_rec_mutex_lock(application_context_mutex);

      g_value_set_object(value, application_context->main_loop);
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  case PROP_TASK_LAUNCHER:
    {
      g_rec_mutex_lock(application_context_mutex);

      g_value_set_object(value, application_context->task_launcher);
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  case PROP_CONFIG:
    {
      g_rec_mutex_lock(application_context_mutex);

      g_value_set_object(value, application_context->config);
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  case PROP_FILE:
    {
      g_rec_mutex_lock(application_context_mutex);

      g_value_set_object(value, application_context->file);
  
      g_rec_mutex_unlock(application_context_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_application_context_dispose(GObject *gobject)
{
  AgsApplicationContext *application_context;

  application_context = AGS_APPLICATION_CONTEXT(gobject);
  
  /* log */
  if(application_context->log != NULL){
    g_object_unref(application_context->log);

    application_context->log = NULL;
  }

  /* config */
  if(application_context->config != NULL){    
    g_object_unref(application_context->config);

    application_context->config = NULL;
  }
  
  /* main loop */
  if(application_context->main_loop != NULL){
    g_object_unref(application_context->main_loop);

    application_context->main_loop = NULL;
  }

  /* task launcher */
  if(application_context->task_launcher != NULL){
    g_object_unref(application_context->task_launcher);

    application_context->task_launcher = NULL;
  }

  /* file */
  if(application_context->file != NULL){
    g_object_unref(application_context->file);

    application_context->file = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_application_context_parent_class)->dispose(gobject);
}

void
ags_application_context_finalize(GObject *gobject)
{
  AgsApplicationContext *application_context;

  application_context = AGS_APPLICATION_CONTEXT(gobject);

  g_free(application_context->version);
  g_free(application_context->build_id);

  /* log */
  if(application_context->log != NULL){
    g_object_unref(application_context->log);
  }

  /* config */
  if(application_context->config != NULL){
    g_object_unref(application_context->config);
  }
  
  /* main loop */
  if(application_context->main_loop != NULL){
    g_object_unref(application_context->main_loop);
  }

  /* task launcher */
  if(application_context->task_launcher != NULL){
    g_object_unref(application_context->task_launcher);
  }

  /* file */
  if(application_context->file != NULL){
    g_object_unref(application_context->file);
  }

  if(application_context == ags_application_context){
    ags_application_context = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_application_context_parent_class)->finalize(gobject);
}

AgsUUID*
ags_application_context_get_uuid(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;
  
  AgsUUID *ptr;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  /* get application_context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* get UUID */
  g_rec_mutex_lock(application_context_mutex);

  ptr = application_context->uuid;

  g_rec_mutex_unlock(application_context_mutex);
  
  return(ptr);
}

gboolean
ags_application_context_has_resource(AgsConnectable *connectable)
{
  return(TRUE);
}

gboolean
ags_application_context_is_ready(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;
  
  gboolean is_ready;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  /* get application_context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* check is added */
  g_rec_mutex_lock(application_context_mutex);

  is_ready = (((AGS_APPLICATION_CONTEXT_ADDED_TO_REGISTRY & (application_context->flags)) != 0) ? TRUE: FALSE);

  g_rec_mutex_unlock(application_context_mutex);
  
  return(is_ready);
}

void
ags_application_context_add_to_registry(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;
  
  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  application_context = AGS_APPLICATION_CONTEXT(connectable);

  ags_application_context_set_flags(application_context, AGS_APPLICATION_CONTEXT_ADDED_TO_REGISTRY);
}

void
ags_application_context_remove_from_registry(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  ags_application_context_unset_flags(application_context, AGS_APPLICATION_CONTEXT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_application_context_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_application_context_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_application_context_xml_parse(AgsConnectable *connectable,
				  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_application_context_is_connected(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;
  
  gboolean is_connected;

  GRecMutex *application_context_mutex;

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  /* get application_context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* check is connected */
  g_rec_mutex_lock(application_context_mutex);

  is_connected = (((AGS_APPLICATION_CONTEXT_CONNECTED & (application_context->flags)) != 0) ? TRUE: FALSE);
  
  g_rec_mutex_unlock(application_context_mutex);
  
  return(is_connected);
}

void
ags_application_context_connect(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  ags_application_context_set_flags(application_context, AGS_APPLICATION_CONTEXT_CONNECTED);
}

void
ags_application_context_disconnect(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  ags_application_context_unset_flags(application_context, AGS_APPLICATION_CONTEXT_CONNECTED);
}

/**
 * ags_application_context_test_flags:
 * @application_context: the #AgsApplicationContext
 * @flags: the flags
 *
 * Test @flags to be set on @application_context.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_application_context_test_flags(AgsApplicationContext *application_context, guint flags)
{
  gboolean retval;  
  
  GRecMutex *application_context_mutex;

  if(!AGS_IS_APPLICATION_CONTEXT(application_context)){
    return(FALSE);
  }

  /* get application_context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* test */
  g_rec_mutex_lock(application_context_mutex);

  retval = (flags & (application_context->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(application_context_mutex);

  return(retval);
}

/**
 * ags_application_context_set_flags:
 * @application_context: the #AgsApplicationContext
 * @flags: see enum AgsApplicationContextFlags
 *
 * Enable a feature of #AgsApplicationContext.
 *
 * Since: 3.0.0
 */
void
ags_application_context_set_flags(AgsApplicationContext *application_context, guint flags)
{
  guint application_context_flags;
  
  GRecMutex *application_context_mutex;

  if(!AGS_IS_APPLICATION_CONTEXT(application_context)){
    return;
  }

  /* get application_context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* set flags */
  g_rec_mutex_lock(application_context_mutex);

  application_context->flags |= flags;
  
  g_rec_mutex_unlock(application_context_mutex);
}
    
/**
 * ags_application_context_unset_flags:
 * @application_context: the #AgsApplicationContext
 * @flags: see enum AgsApplicationContextFlags
 *
 * Disable a feature of AgsApplicationContext.
 *
 * Since: 3.0.0
 */
void
ags_application_context_unset_flags(AgsApplicationContext *application_context, guint flags)
{
  guint application_context_flags;
  
  GRecMutex *application_context_mutex;

  if(!AGS_IS_APPLICATION_CONTEXT(application_context)){
    return;
  }

  /* get application_context mutex */
  application_context_mutex = AGS_APPLICATION_CONTEXT_GET_OBJ_MUTEX(application_context);

  /* unset flags */
  g_rec_mutex_lock(application_context_mutex);

  application_context->flags &= (~flags);
  
  g_rec_mutex_unlock(application_context_mutex);
}

void
ags_application_context_real_load_config(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}

/**
 * ags_application_context_load_config:
 * @application_context: the #AgsApplicationContext
 *
 * Signal to load and parse configuration.
 *
 * Since: 3.0.0
 */
void
ags_application_context_load_config(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[LOAD_CONFIG], 0);
  g_object_unref(G_OBJECT(application_context));
}

void
ags_application_context_real_prepare(AgsApplicationContext *application_context)
{
  ags_log_add_message(ags_log_get_instance(), "prepare Advanced Gtk+ Sequencer");
}

/**
 * ags_application_context_prepare:
 * @application_context: the #AgsApplicationContext
 *
 * Prepare @application_context.
 *
 * Since: 3.0.0
 */
void
ags_application_context_prepare(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[PREPARE], 0);
  g_object_unref(G_OBJECT(application_context));
}

void
ags_application_context_real_setup(AgsApplicationContext *application_context)
{
  ags_log_add_message(ags_log_get_instance(), "setup Advanced Gtk+ Sequencer");
}

/**
 * ags_application_context_setup:
 * @application_context: the #AgsApplicationContext
 *
 * Setup @application_context.
 *
 * Since: 3.0.0
 */
void
ags_application_context_setup(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[SETUP], 0);
  g_object_unref(G_OBJECT(application_context));
}

void
ags_application_context_real_register_types(AgsApplicationContext *application_context)
{
  //TODO:JK: implement me
}

/**
 * ags_application_context_register_types:
 * @application_context: the #AgsApplicationContext
 *
 * Notification to register your types.
 *
 * Since: 3.0.0
 */
void
ags_application_context_register_types(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[REGISTER_TYPES], 0);
  g_object_unref(G_OBJECT(application_context));
}

void
ags_application_context_real_quit(AgsApplicationContext *application_context)
{
  //TODO:JK: enhance me
  exit(0);
}

/**
 * ags_application_context_quit:
 * @application_context: the #AgsApplicationContext
 *
 * Calls exit()
 *
 * Since: 3.0.0
 */
void
ags_application_context_quit(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[QUIT], 0);
  g_object_unref(G_OBJECT(application_context));
}

/**
 * ags_application_context_get_instance:
 * 
 * Get your application context instance.
 *
 * Returns: (transfer none): the #AgsApplicationContext instance
 *
 * Since: 3.0.0
 */
AgsApplicationContext*
ags_application_context_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(ags_application_context == NULL){
    ags_application_context = ags_application_context_new(NULL,
							  NULL);
  }

  g_mutex_unlock(&mutex);
  
  return(ags_application_context);
}

/**
 * ags_application_context_new:
 * @main_loop: (nullable): the #AgsMainLoop
 * @config: (nullable): the #AgsConfig
 *
 * Create a new instance of #AgsApplicationContext
 * 
 * Returns: the #AgsApplicationContext instance
 *
 * Since: 3.0.0
 */
AgsApplicationContext*
ags_application_context_new(GObject *main_loop,
			    AgsConfig *config)
{
  AgsApplicationContext *application_context;

  application_context = (AgsApplicationContext *) g_object_new(AGS_TYPE_APPLICATION_CONTEXT,
							       "main-loop", main_loop,
							       "config", config,
							       NULL);

  return(application_context);
}
