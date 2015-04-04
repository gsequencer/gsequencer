/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/object/ags_config.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_application_context.h>

#include <gio/gio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

void ags_config_class_init(AgsConfigClass *config_class);
void ags_config_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_config_init(AgsConfig *config);
void ags_config_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_config_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_config_add_to_registry(AgsConnectable *connectable);
void ags_config_remove_from_registry(AgsConnectable *connectable);
gboolean ags_config_is_connected(AgsConnectable *connectable);
void ags_config_connect(AgsConnectable *connectable);
void ags_config_disconnect(AgsConnectable *connectable);
void ags_config_finalize(GObject *gobject);
gchar* ags_config_get_version(AgsConfig *config);
void ags_config_set_version(AgsConfig *config, gchar *version);
gchar* ags_config_get_build_id(AgsConfig *config);
void ags_config_set_build_id(AgsConfig *config, gchar *build_id);

void ags_config_real_load_defaults(AgsConfig *config);
void ags_config_real_set_value(AgsConfig *config, gchar *group, gchar *key, gchar *value);
gchar* ags_config_real_get_value(AgsConfig *config, gchar *group, gchar *key);

/**
 * SECTION:ags_config
 * @short_description: Config Advanced Gtk+ Sequencer
 * @title: AgsConfig
 * @section_id:
 * @include: ags/audio/ags_config.h
 *
 * #AgsConfig provides configuration to Advanced Gtk+ Sequencer.
 */

enum{
  LOAD_DEFAULTS,
  SET_VALUE,
  GET_VALUE,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

AgsConfig *ags_config;
static gpointer ags_config_parent_class = NULL;
static guint config_signals[LAST_SIGNAL];

GType
ags_config_get_type (void)
{
  static GType ags_type_config = 0;

  if(!ags_type_config){
    static const GTypeInfo ags_config_info = {
      sizeof (AgsConfigClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_config_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConfig),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_config_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_config_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };


    ags_type_config = g_type_register_static(G_TYPE_OBJECT,
					     "AgsConfig\0",
					     &ags_config_info,
					     0);

    g_type_add_interface_static(ags_type_config,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_config);
}

void
ags_config_class_init(AgsConfigClass *config)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_config_parent_class = g_type_class_peek_parent(config);

  /* GObjectClass */
  gobject = (GObjectClass *) config;

  gobject->set_property = ags_config_set_property;
  gobject->get_property = ags_config_get_property;

  gobject->finalize = ags_config_finalize;

  /* properties */
  /**
   * AgsConfig:devout:
   *
   * The assigned devout.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("application context\0",
				   "application context of config\0",
				   "The application context which this config is packed into\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /* AgsConfigClass */
  config->load_defaults = ags_config_real_load_defaults;
  config->set_value = ags_config_real_set_value;
  config->get_value = ags_config_real_get_value;

  /* signals */
  /**
   * AgsConfig::load-defaults:
   * @config: the object to resolve
   *
   * The ::load-defaults signal notifies about loading defaults
   */
  config_signals[LOAD_DEFAULTS] =
    g_signal_new("load-defaults\0",
		 G_TYPE_FROM_CLASS (config),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsConfigClass, load_defaults),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsConfig::set-value:
   * @config: the object to resolve
   * @group: the group to apply to
   * @key: the key to set
   * @value: the value to apply
   *
   * The ::set-value signal notifies about value been setting.
   */
  config_signals[SET_VALUE] =
    g_signal_new("set-value\0",
		 G_TYPE_FROM_CLASS (config),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsConfigClass, set_value),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__STRING_STRING_STRING,
		 G_TYPE_NONE, 3,
		 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  /**
   * AgsConfig::get-value:
   * @config: the object to resolve
   * @group: the group to retrieve from
   * @key: the key to get
   * Returns: the value
   *
   * The ::get-value signal notifies about value been getting.
   */
  config_signals[GET_VALUE] =
    g_signal_new("get-value\0",
		 G_TYPE_FROM_CLASS (config),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsConfigClass, get_value),
		 NULL, NULL,
		 g_cclosure_user_marshal_STRING__STRING_STRING,
		 G_TYPE_NONE, 2,
		 G_TYPE_STRING, G_TYPE_STRING);
}

void
ags_config_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->add_to_registry = ags_config_add_to_registry;
  connectable->remove_from_registry = ags_config_remove_from_registry;

  connectable->is_ready = NULL;
  connectable->is_connected = ags_config_is_connected;
  connectable->connect = ags_config_connect;
  connectable->disconnect = ags_config_disconnect;
}

void
ags_config_init(AgsConfig *config)
{
  config->version = AGS_CONFIG_DEFAULT_VERSION;
  config->build_id = AGS_CONFIG_DEFAULT_BUILD_ID;

  config->application_context == NULL;

  config->key_file = g_key_file_new();
}

void
ags_config_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsConfig *config;

  config = AGS_CONFIG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;
      
      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(application_context == ((AgsApplicationContext *) config->application_context))
	return;

      if(config->application_context != NULL)
	g_object_unref(config->application_context);

      if(application_context != NULL)
	g_object_ref(G_OBJECT(application_context));

      config->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_config_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsConfig *config;

  config = AGS_CONFIG(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, config->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_config_add_to_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_remove_from_registry(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gboolean
ags_config_is_connected(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_config_finalize(GObject *gobject)
{

  //TODO:JK: implement me

  G_OBJECT_CLASS(ags_config_parent_class)->finalize(gobject);
}

gchar*
ags_config_get_version(AgsConfig *config)
{
  return(config->version);
}

void
ags_config_set_version(AgsConfig *config, gchar *version)
{
  config->version = version;
}

gchar*
ags_config_get_build_id(AgsConfig *config)
{
  return(config->build_id);
}

void
ags_config_set_build_id(AgsConfig *config, gchar *build_id)
{
  config->build_id = build_id;
}

void
ags_config_real_load_defaults(AgsConfig *config)
{
  ags_config_set(config, AGS_CONFIG_GENERIC, "autosave-thread\0", "false\0");

  ags_config_set(config, AGS_CONFIG_THREAD, "model\0", "multi-threaded\0");
  ags_config_set(config, AGS_CONFIG_THREAD, "lock-global\0", "ags-thread\0");
  ags_config_set(config, AGS_CONFIG_THREAD, "lock-parent\0", "ags-recycling-thread\0");

  ags_config_set(config, AGS_CONFIG_DEVOUT, "samplerate\0", "44100\0");
  ags_config_set(config, AGS_CONFIG_DEVOUT, "buffer-size\0", "940\0");
  ags_config_set(config, AGS_CONFIG_DEVOUT, "pcm-channels\0", "2\0");
  ags_config_set(config, AGS_CONFIG_DEVOUT, "dsp-channels\0", "2\0");
  ags_config_set(config, AGS_CONFIG_DEVOUT, "alsa-handle\0", "hw:0,0\0");

  ags_config_set(config, AGS_CONFIG_RECALL, "auto-sense\0", "true\0");
}

/**
 * ags_config_load_defaults:
 * @config: the #AgsConfig
 *
 * Load configuration from default values.
 *
 * Since: 0.4
 */
void
ags_config_load_defaults(AgsConfig *config)
{
  g_return_if_fail(AGS_IS_CONFIG(config));

  g_object_ref(G_OBJECT(config));
  g_signal_emit(G_OBJECT(config),
		config_signals[LOAD_DEFAULTS], 0);
  g_object_unref(G_OBJECT(config));
}

/**
 * ags_config_load_from_file:
 * @config: the #AgsConfig
 * @filename: the configuration file
 *
 * Load configuration from @filename.
 *
 * Since: 0.4
 */
void
ags_config_load_from_file(AgsConfig *config, gchar *filename)
{
  GFile *file;

  file = g_file_new_for_path(filename);

  g_message("loading preferences for: %s\0", filename);

  if(!g_file_query_exists(file,
			  NULL)){
    ags_config_load_defaults(config);
  }else{
    GKeyFile *key_file;
    gchar **groups, **groups_start;
    gchar **keys, **keys_start;
    gchar *value;

    GError *error;

    error = NULL;
    
    key_file = g_key_file_new();
    g_key_file_load_from_file(key_file,
			      filename,
			      G_KEY_FILE_NONE,
			      &error);


    groups =
      groups_start = g_key_file_get_groups(key_file,
					   NULL);

    while(*groups != NULL){
      keys =
	keys_start = g_key_file_get_keys(key_file,
					 *groups,
					 NULL,
					 NULL);

      while(*keys != NULL){
	value = g_key_file_get_value(key_file,
				     *groups,
				     *keys,
				     NULL);

	ags_config_set(config,
		       *groups,
		       *keys,
		       value);
	
	keys++;
      }

      g_strfreev(keys_start);

      groups++;
    }

    g_strfreev(groups_start);
    g_key_file_unref(key_file);
  }

  g_object_unref(file);
}

/**
 * ags_config_save:
 * @config: the #AgsConfig
 *
 * Save configuration.
 *
 * Since: 0.4
 */
void
ags_config_save(AgsConfig *config)
{
  struct passwd *pw;
  uid_t uid;
  gchar *path, *filename;
  gchar *content;
  gsize length;
  GError *error;

  uid = getuid();
  pw = getpwuid(uid);

  /* open conf dir */
  path = g_strdup_printf("%s/%s\0",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  if(!g_mkdir_with_parents(path,
			   0755)){
    filename = g_strdup_printf("%s/%s",
			       path,
			       AGS_DEFAULT_CONFIG);

    /* get content */
    error = NULL;

    content = g_key_file_to_data(config->key_file,
				 &length,
				 &error);
    
    if(error != NULL){
      //TODO:JK: do recovery
      goto ags_config_save_END;
    }

    /* write content */
    error = NULL;

    g_file_set_contents(filename,
			content,
			length,
			&error);

  ags_config_save_END:
    g_free(filename);
  }

  g_free(path);
}

void
ags_config_real_set_value(AgsConfig *config, gchar *group, gchar *key, gchar *value)
{
  AgsApplicationContext *application_context;

  application_context = config->application_context;

  g_key_file_set_value(config->key_file, group, key, value);
}

/**
 * ags_config_set:
 * @config: the #AgsConfig
 * @group: the config group identifier
 * @key: the key of the property
 * @value: the value to set
 *
 * Set config by @group and @key, applying @value.
 *
 * Since: 0.4
 */
void
ags_config_set_value(AgsConfig *config, gchar *group, gchar *key, gchar *value)
{
  g_return_if_fail(AGS_IS_CONFIG(config));

  g_object_ref(G_OBJECT(config));
  g_signal_emit(G_OBJECT(config),
		config_signals[SET_VALUE], 0,
		group, key, value);
  g_object_unref(G_OBJECT(config));
}

gchar*
ags_config_real_get_value(AgsConfig *config, gchar *group, gchar *key)
{
  gchar *str;
  GError *error;
  
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);
  
  error = NULL;

  str = g_key_file_get_value(config->key_file, group, key, &error);

  pthread_mutex_unlock(&mutex);

  return(str);
}

/**
 * ags_config_get:
 * @config: the #AgsConfig
 * @group: the config group identifier
 * @key: the key of the property
 *
 * Retrieve config by @group and @key.
 *
 * Returns: the property's value
 *
 * Since: 0.4
 */
gchar*
ags_config_get_value(AgsConfig *config, gchar *group, gchar *key)
{
  gchar *value;
  
  g_return_val_if_fail(AGS_IS_CONFIG(config), NULL);

  g_object_ref(G_OBJECT(config));
  g_signal_emit(G_OBJECT(config),
		config_signals[GET_VALUE], 0,
		group, key,
		&value);
  g_object_unref(G_OBJECT(config));

  return(value);
}

/**
 * ags_config_new:
 *
 * Creates an #AgsConfig.
 *
 * Returns: a new #AgsConfig.
 *
 * Since: 0.4
 */
AgsConfig*
ags_config_new(GObject *application_context)
{
  AgsConfig *config;

  config = (AgsConfig *) g_object_new(AGS_TYPE_CONFIG,
				      "application-context\0", application_context,
				      NULL);

  return(config);
}

