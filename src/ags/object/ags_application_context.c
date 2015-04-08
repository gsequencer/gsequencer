/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/object/ags_application_context.h>

#include <ags/object/ags_marshal.h>
#include <ags-lib/object/ags_connectable.h>

#include <gio/gio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

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
void ags_application_context_finalize(GObject *gobject);

void ags_application_context_real_load_config(AgsApplicationContext *application_context);
void ags_application_context_real_register_types(AgsApplicationContext *application_context);

enum{
  LOAD_CONFIG,
  REGISTER_TYPES,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_APPLICATION_MUTEX,
  PROP_MAIN_LOOP,
  PROP_CONFIG,
};

static gpointer ags_application_context_parent_class = NULL;
static guint application_context_signals[LAST_SIGNAL];

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
							  "AgsApplicationContext\0",
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

  gobject->finalize = ags_application_context_finalize;

  /* properties */
  /**
   * AgsApplicationContext:main-loop:
   *
   * The assigned main-loop.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("main-loop\0",
				   "main-loop of application context\0",
				   "The main-loop what application context is running in\0",
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
   * Since: 0.4
   */
  param_spec = g_param_spec_object("config\0",
				   "config of application context\0",
				   "The config what application context is running in\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CONFIG,
				  param_spec);

  /* AgsApplicationContextClass */
  application_context->load_config = ags_application_context_real_load_config;
  application_context->register_types = ags_application_context_real_register_types;
  
  /* signals */
  /**
   * AgsApplicationContext::load-config:
   * @application_context: the object to play
   *
   * The ::load-config signal notifies about running
   * stage 2.
   */
  application_context_signals[LOAD_CONFIG] =
    g_signal_new("load-config\0",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, load_config),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsApplicationContext::register-types:
   * @application_context: the object to play
   *
   * The ::register-types signal notifies about running
   * stage 2.
   */
  application_context_signals[REGISTER_TYPES] =
    g_signal_new("register-types\0",
		 G_TYPE_FROM_CLASS (application_context),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsApplicationContextClass, register_types),
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
  //  uid_t uid;
  //  gchar *wdir, *filename;
  pthread_mutexattr_t mutexattr;
  /*
  uid = getuid();
  pw = getpwuid(uid);
  
  wdir = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  file = g_file_new_for_path(wdir);

  g_file_make_directory(file,
			NULL,
			NULL);
  */
  application_context->flags = 0;

  application_context->version = AGS_VERSION;
  application_context->build_id = AGS_BUILD_ID;

  application_context->log = (AgsLog *) g_object_new(AGS_TYPE_LOG,
						     "file\0", stdout,
						     NULL);
  
  application_context->domain = NULL;
  application_context->config = NULL;

  pthread_mutexattr_init(&(mutexattr));
  pthread_mutexattr_settype(&(mutexattr), PTHREAD_MUTEX_RECURSIVE);

  pthread_mutex_init(&(application_context->mutex), &(mutexattr));

  application_context->main_loop = NULL;
  application_context->autosave_thread = NULL;
  
  application_context->file = NULL;

  // ags_log_message(ags_default_log, "starting Advanced Gtk+ Sequencer\n\0");
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
      
      config = (AgsApplicationContext *) g_value_get_object(value);

      if(config == application_context->config)
	return;

      if(application_context->config != NULL)
	g_object_unref(application_context->config);

      if(config != NULL)
	g_object_ref(G_OBJECT(config));

      application_context->config = (GObject *) config;
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
ags_application_context_finalize(GObject *gobject)
{
  AgsApplicationContext *application_context;

  G_OBJECT_CLASS(ags_application_context_parent_class)->finalize(gobject);

  application_context = AGS_APPLICATION_CONTEXT(gobject);

  //TODO:JK: implement me
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

AgsApplicationContext*
ags_application_context_new(GObject *main_loop,
			    AgsConfig *config)
{
  AgsApplicationContext *application_context;

  application_context = (AgsApplicationContext *) g_object_new(AGS_TYPE_APPLICATION_CONTEXT,
							       "main-loop\0", main_loop,
							       "config\0", config,
							       NULL);

  return(application_context);
}


