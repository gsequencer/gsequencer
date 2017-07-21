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

#include <ags/object/ags_application_context.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>

#include <gio/gio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

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
void ags_application_context_connect(AgsConnectable *connectable);
void ags_application_context_disconnect(AgsConnectable *connectable);
void ags_application_context_dispose(GObject *gobject);
void ags_application_context_finalize(GObject *gobject);

void ags_application_context_real_load_config(AgsApplicationContext *application_context);
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
  REGISTER_TYPES,
  QUIT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_APPLICATION_MUTEX,
  PROP_MAIN_LOOP,
  PROP_CONFIG,
  PROP_FILE,
};

static gpointer ags_application_context_parent_class = NULL;
static guint application_context_signals[LAST_SIGNAL];

AgsApplicationContext *ags_application_context = NULL;

GType
ags_application_context_get_type()
{
  static GType ags_type_application_context = 0;

  if(!ags_type_application_context){
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
  }

  return (ags_type_application_context);
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
   * Since: 0.5.0
   */
  param_spec = g_param_spec_object("main-loop",
				   i18n_pspec("main-loop of application context"),
				   i18n_pspec("The main-loop what application context is running in"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_LOOP,
				  param_spec);

  /**
   * AgsApplicationContext:config:
   *
   * The assigned config.
   * 
   * Since: 0.5.0
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
   * Since: 0.5.0
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

  application_context->register_types = ags_application_context_real_register_types;
  
  application_context->quit = ags_application_context_real_quit;

  /* signals */
  /**
   * AgsApplicationContext::load-config:
   * @application_context: the #AgsApplicationContext
   *
   * The ::load-config notifies to load configuration.
   *
   * Since: 0.7.68
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
   * AgsApplicationContext::register-types:
   * @application_context: the  #AgsApplicationContext
   *
   * The ::register-types signal should be implemented to load
   * your types.
   *
   * Since: 0.7.68
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
   * Since: 0.7.68
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
  connectable->connect = ags_application_context_connect;
  connectable->disconnect = ags_application_context_disconnect;
}

void
ags_application_context_init(AgsApplicationContext *application_context)
{
  GFile *file;
  struct passwd *pw;

  application_context->flags = 0;

  application_context->argc = 0;
  application_context->argv = NULL;
  
  application_context->version = AGS_VERSION;
  application_context->build_id = AGS_BUILD_ID;

  application_context->log = NULL;  
  application_context->domain = NULL;
  application_context->config = NULL;

  application_context->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));

  pthread_mutexattr_init(application_context->mutexattr);
  pthread_mutexattr_settype(application_context->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(application_context->mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  
  application_context->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(application_context->mutex, application_context->mutexattr);

  application_context->main_loop = NULL;
  application_context->task_thread = NULL;
  application_context->autosave_thread = NULL;
  
  application_context->file = NULL;
  
  application_context->history = NULL;
  
  // ags_log_message(ags_default_log, "starting Advanced Gtk+ Sequencer\n");
}

void
ags_application_context_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsApplicationContext *application_context;

  application_context = AGS_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_CONFIG:
    {
      AgsConfig *config;
      
      config = (AgsConfig *) g_value_get_object(value);

      if(config == application_context->config)
	return;

      if(application_context->config != NULL)
	g_object_unref(application_context->config);

      if(config != NULL)
	g_object_ref(G_OBJECT(config));

      application_context->config = config;
    }
    break;
  case PROP_MAIN_LOOP:
    {
      GObject *main_loop;
      
      main_loop = (GObject *) g_value_get_object(value);

      if(main_loop == application_context->main_loop)
	return;

      if(application_context->main_loop != NULL)
	g_object_unref(application_context->main_loop);

      if(main_loop != NULL)
	g_object_ref(G_OBJECT(main_loop));

      application_context->main_loop = main_loop;
    }
    break;
  case PROP_FILE:
    {
      AgsFile *file;
      
      file = (AgsFile *) g_value_get_object(value);

      if(file == application_context->file)
	return;

      if(application_context->file != NULL)
	g_object_unref(application_context->file);

      if(file != NULL)
	g_object_ref(G_OBJECT(file));

      application_context->file = (AgsFile *) file;
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

  application_context = AGS_APPLICATION_CONTEXT(gobject);

  switch(prop_id){
  case PROP_CONFIG:
    {
      g_value_set_object(value, application_context->config);
    }
    break;
  case PROP_MAIN_LOOP:
    {
      g_value_set_object(value, application_context->main_loop);
    }
    break;
  case PROP_FILE:
    {
      g_value_set_object(value, application_context->file);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_application_context_connect(AgsConnectable *connectable)
{
  AgsApplicationContext *application_context;

  application_context = AGS_APPLICATION_CONTEXT(connectable);

  if((AGS_APPLICATION_CONTEXT_CONNECTED & (application_context->flags)) != 0)
    return;

  application_context->flags |= AGS_APPLICATION_CONTEXT_CONNECTED;

  if((AGS_APPLICATION_CONTEXT_DEFAULT & (application_context->flags)) != 0){
    GList *list;

    list = application_context->sibling;

    while(list != NULL){
      if(application_context != list->data){
	ags_connectable_connect(AGS_CONNECTABLE(list->data));
      }

      list = list->next;
    }
  }

  /* note main loop won't connect here */
}

void
ags_application_context_disconnect(AgsConnectable *connectable)
{
  /* empty */
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
    g_object_set(application_context->config,
		 "application-context", NULL,
		 NULL);
    
    g_object_unref(application_context->config);

    application_context->config = NULL;
  }
  
  /* main loop */
  if(application_context->main_loop != NULL){
    g_object_set(application_context->main_loop,
		 "application-context", NULL,
		 NULL);

    g_object_unref(application_context->main_loop);

    application_context->main_loop = NULL;
  }

  /* autosave thread */
  if(application_context->autosave_thread != NULL){
    g_object_set(application_context->autosave_thread,
		 "application-context", NULL,
		 NULL);

    g_object_unref(application_context->autosave_thread);

    application_context->autosave_thread = NULL;
  }

  /* task thread */
  if(application_context->task_thread != NULL){
    g_object_unref(application_context->task_thread);

    application_context->task_thread = NULL;
  }

  /* file */
  if(application_context->file != NULL){
    g_object_set(application_context->file,
		 "application-context", NULL,
		 NULL);

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

  /* application mutex */
  pthread_mutexattr_destroy(application_context->mutexattr);
  free(application_context->mutexattr);

  pthread_mutex_destroy(application_context->mutex);
  free(application_context->mutex);

  /* log */
  if(application_context->log != NULL){
    g_object_unref(application_context->log);
  }

  /* config */
  if(application_context->config != NULL){
    g_object_set(application_context->config,
		 "application-context", NULL,
		 NULL);

    g_object_unref(application_context->config);
  }
  
  /* main loop */
  if(application_context->main_loop != NULL){
    g_object_set(application_context->main_loop,
		 "application-context", NULL,
		 NULL);

    g_object_unref(application_context->main_loop);
  }

  /* autosave thread */
  if(application_context->autosave_thread != NULL){
    g_object_set(application_context->autosave_thread,
		 "application-context", NULL,
		 NULL);

    g_object_unref(application_context->autosave_thread);
  }

  /* task thread */
  if(application_context->task_thread != NULL){
    g_object_unref(application_context->task_thread);
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

void
ags_application_context_real_load_config(AgsApplicationContext *application_context)
{
  AgsConfig *config;

  if(application_context == NULL){
    return;
  }

  config = application_context->config;

  if(config == NULL){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_application_context_load_config:
 * @application_context: the #AgsApplicationContext
 *
 * Signal to load and parse configuration.
 *
 * Since 0.7.0
 */
void
ags_application_context_load_config(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[LOAD_CONFIG], 0);
  g_object_unref(G_OBJECT(application_context));
  g_object_unref(application_context);
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
 * Since 0.7.0
 */
void
ags_application_context_register_types(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[REGISTER_TYPES], 0);
  g_object_unref(G_OBJECT(application_context));
  g_object_unref(application_context);
}

void
ags_application_context_add_sibling(AgsApplicationContext *application_context,
				    AgsApplicationContext *sibling)
{
  //TODO:JK: implement me
}

void
ags_application_context_remove_sibling(AgsApplicationContext *application_context,
				       AgsApplicationContext *sibling)
{
  //TODO:JK: implement me
}

/**
 * ags_application_context_find_default:
 * @application_context: the #GList-struct containing #AgsAppilcationContext
 *
 * Find default context in @application_context #GList-struct containing
 * #AgsApplicationContext.
 *
 * Since: 0.7.111
 */
AgsApplicationContext*
ags_application_context_find_default(GList *application_context)
{
  while(application_context != NULL){
    if((AGS_APPLICATION_CONTEXT_DEFAULT & (AGS_APPLICATION_CONTEXT(application_context->data)->flags)) != 0){
      return(application_context->data);
    }
    
    application_context = application_context->next;
  }

  return(NULL);
}

/**
 * ags_application_context_find_main_loop:
 * @application_context: the #GList-struct containing #AgsAppilcationContext
 *
 * Find :main-loop in @application_context #GList-struct containing
 * #AgsApplicationContext.
 *
 * Since: 0.7.111
 */
GList*
ags_application_context_find_main_loop(GList *application_context)
{
  while(application_context != NULL){
    if(AGS_APPLICATION_CONTEXT(application_context->data)->main_loop != NULL){
      break;
    }
    
    application_context = application_context->next;
  }
  
  return(application_context);
}

void
ags_application_context_real_quit(AgsApplicationContext *application_context)
{
  //TODO:JK: enhance me
  exit(0);
}

/**
 * ags_application_context_quit:
 * @application_context: the context to quit
 *
 * Calls exit()
 *
 * Since: 0.7.111
 */
void
ags_application_context_quit(AgsApplicationContext *application_context)
{
  g_return_if_fail(AGS_IS_APPLICATION_CONTEXT(application_context));

  g_object_ref(G_OBJECT(application_context));
  g_signal_emit(G_OBJECT(application_context),
		application_context_signals[QUIT], 0);
  g_object_unref(G_OBJECT(application_context));
  g_object_unref(application_context);
}

/**
 * ags_application_context_get_instance:
 * 
 * Get your application context instance.
 *
 * Returns: the #AgsApplicationContext instance
 *
 * Since: 0.7.0
 */
AgsApplicationContext*
ags_application_context_get_instance()
{
  if(ags_application_context == NULL){
    ags_application_context = ags_application_context_new(NULL,
							  NULL);
  }

  return(ags_application_context);
}

/**
 * ags_application_context_new:
 * @main_loop: the #AgsMainLoop
 * @config: the #AgsConfig
 *
 * Create a new instance of #AgsApplicationContext
 * 
 * Returns: the #AgsApplicationContext instance
 *
 * Since: 0.7.0
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


