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

#include <ags/object/ags_config.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_application_context.h>

#include <gio/gio.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef AGS_W32API
#include <pwd.h>
#endif

#include <ags/config.h>
#include <ags/i18n.h>

void ags_config_class_init(AgsConfigClass *config_class);
void ags_config_init(AgsConfig *config);
void ags_config_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_config_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_config_dispose(GObject *gobject);
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
 * @include: ags/object/ags_config.h
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
};

static gpointer ags_config_parent_class = NULL;
static guint config_signals[LAST_SIGNAL];

AgsConfig *ags_config = NULL;

GType
ags_config_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_config = 0;

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

    ags_type_config = g_type_register_static(G_TYPE_OBJECT,
					     "AgsConfig",
					     &ags_config_info,
					     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_config);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_config_dispose;
  gobject->finalize = ags_config_finalize;

  /* properties */

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
   *
   * Since: 3.0.0
   */
  config_signals[LOAD_DEFAULTS] =
    g_signal_new("load-defaults",
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
   *
   * Since: 3.0.0
   */
  config_signals[SET_VALUE] =
    g_signal_new("set-value",
		 G_TYPE_FROM_CLASS (config),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsConfigClass, set_value),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__STRING_STRING_STRING,
		 G_TYPE_NONE, 3,
		 G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  /**
   * AgsConfig::get-value:
   * @config: the object to resolve
   * @group: the group to retrieve from
   * @key: the key to get
   *
   * The ::get-value signal notifies about value been getting.
   *
   * Returns: the value
   *
   * Since: 3.0.0
   */
  config_signals[GET_VALUE] =
    g_signal_new("get-value",
		 G_TYPE_FROM_CLASS (config),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsConfigClass, get_value),
		 NULL, NULL,
		 ags_cclosure_marshal_STRING__STRING_STRING,
		 G_TYPE_STRING, 2,
		 G_TYPE_STRING, G_TYPE_STRING);
}

void
ags_config_init(AgsConfig *config)
{
  config->flags = 0;
  
  g_rec_mutex_init(&(config->obj_mutex));

  /* version and build id */
  config->version = g_strdup(AGS_CONFIG_DEFAULT_VERSION);
  config->build_id = g_strdup(AGS_CONFIG_DEFAULT_BUILD_ID);

  config->key_file = g_key_file_new();
  g_key_file_ref(config->key_file);
}

void
ags_config_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsConfig *config;

  GRecMutex *config_mutex;

  config = AGS_CONFIG(gobject);

  /* get config mutex */
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  switch(prop_id){
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

  GRecMutex *config_mutex;

  config = AGS_CONFIG(gobject);

  /* get config mutex */
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}
void
ags_config_dispose(GObject *gobject)
{
  AgsConfig *config;

  config = (AgsConfig *) gobject;

  /* call parent */
  G_OBJECT_CLASS(ags_config_parent_class)->dispose(gobject);
}

void
ags_config_finalize(GObject *gobject)
{
  AgsConfig *config;

  config = (AgsConfig *) gobject;

  g_free(config->version);
  g_free(config->build_id);

  /* key file */
  if(config->key_file != NULL){
    g_key_file_unref(config->key_file);
  }

  /* global variable */
  if(ags_config == config){
    ags_config = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_config_parent_class)->finalize(gobject);
}

gchar*
ags_config_get_version(AgsConfig *config)
{
  gchar *version;
  
  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return(NULL);
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* get version */
  g_rec_mutex_lock(config_mutex);

  version = config->version;
  
  g_rec_mutex_unlock(config_mutex);

  return(version);
}

void
ags_config_set_version(AgsConfig *config, gchar *version)
{
  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* set version */
  g_rec_mutex_lock(config_mutex);

  config->version = g_strdup(version);
  
  g_rec_mutex_unlock(config_mutex);
}

gchar*
ags_config_get_build_id(AgsConfig *config)
{
  gchar *build_id;
  
  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return(NULL);
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* get build id */
  g_rec_mutex_lock(config_mutex);

  build_id = config->build_id;
  
  g_rec_mutex_unlock(config_mutex);

  return(build_id);
}

void
ags_config_set_build_id(AgsConfig *config, gchar *build_id)
{
  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* set version */
  g_rec_mutex_lock(config_mutex);

  config->build_id = g_strdup(build_id);
  
  g_rec_mutex_unlock(config_mutex);
}

void
ags_config_real_load_defaults(AgsConfig *config)
{

  GRecMutex *config_mutex;
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* load defaults */
  g_rec_mutex_lock(config_mutex);

  ags_config_set_value(config, AGS_CONFIG_GENERIC, "autosave-thread", "false");
  ags_config_set_value(config, AGS_CONFIG_GENERIC, "simple-file", "true");
  ags_config_set_value(config, AGS_CONFIG_GENERIC, "disable-feature", "experimental");
  ags_config_set_value(config, AGS_CONFIG_GENERIC, "engine-mode", "performance");
  ags_config_set_value(config, AGS_CONFIG_GENERIC, "gui-scale", "1.0");

  ags_config_set_value(config, AGS_CONFIG_THREAD, "model", "super-threaded");
  ags_config_set_value(config, AGS_CONFIG_THREAD, "super-threaded-scope", "audio");
  ags_config_set_value(config, AGS_CONFIG_THREAD, "lock-global", "ags-thread");
  ags_config_set_value(config, AGS_CONFIG_THREAD, "lock-parent", "ags-recycling-thread");
  ags_config_set_value(config, AGS_CONFIG_THREAD, "max-precision", "250");

#if defined(AGS_WITH_WASAPI)
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "backend", "wasapi");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "device", NULL);
#elif defined(AGS_WITH_CORE_AUDIO)
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "backend", "core-audio");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "device", "ags-core-audio-devout-0");
#elif defined(AGS_WITH_PULSE)
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "backend", "pulse");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "device", "ags-pulse-devout-0");
#elif defined(AGS_WITH_ALSA)
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "backend", "alsa");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "device", "default");
#elif defined(AGS_WITH_OSS)
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "backend", "oss");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "device", "/dev/dsp");
#endif
  
#if defined(AGS_WITH_WASAPI)
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "pcm-channels", "2");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "samplerate", "44100");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "buffer-size", "512");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "use-cache", "false");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "cache-buffer-size", "4096");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "format", "16");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "wasapi-buffer-size", "2048");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "wasapi-share-mode", "exclusive");
#else
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "pcm-channels", "2");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "samplerate", "48000");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "buffer-size", "512");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "use-cache", "true");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "cache-buffer-size", "4096");
  ags_config_set_value(config, AGS_CONFIG_SOUNDCARD_0, "format", "16");
#endif
  
  //ags_config_set_value(config, AGS_CONFIG_SEQUENCER_0, "backend", "jack");
  //ags_config_set_value(config, AGS_CONFIG_SEQUENCER_0, "device", "ags-jack-midiin-0");

  ags_config_set_value(config, AGS_CONFIG_RECALL, "auto-sense", "true");

  g_rec_mutex_unlock(config_mutex);
}

/**
 * ags_config_load_defaults:
 * @config: the #AgsConfig
 *
 * Load configuration from default values.
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
 */
void
ags_config_load_from_file(AgsConfig *config, gchar *filename)
{
  GFile *file;

  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  file = g_file_new_for_path(filename);

  g_message("loading preferences for: %s", filename);

  if(!g_file_query_exists(file,
			  NULL)){
    ags_config_load_defaults(config);
  }else{
    GKeyFile *key_file;

    gchar **groups, **groups_start;
    gchar **keys, **keys_start;
    gchar *value;

    GError *error;

    g_rec_mutex_lock(config_mutex);

    error = NULL;
    
    key_file = g_key_file_new();
    g_key_file_load_from_file(key_file,
			      filename,
			      G_KEY_FILE_NONE,
			      &error);

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }

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
	ags_config_set_value(config,
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

    g_rec_mutex_unlock(config_mutex);
  }

  g_object_unref(file);
}

/**
 * ags_config_load_from_data:
 * @config: the #AgsConfig
 * @buffer: the data buffer
 * @buffer_length: the size of the buffer
 *
 * Read configuration in memory.
 *
 * Since: 3.0.0
 */
void
ags_config_load_from_data(AgsConfig *config,
			  char *buffer, gsize buffer_length)
{

  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* load from data */
  //#ifdef AGS_DEBUG
  g_message("loading preferences from data[0x%x]", (unsigned int) buffer);
  //#endif
  
  if(buffer == NULL){
    ags_config_load_defaults(config);
  }else{
    GKeyFile *key_file;

    gchar **groups, **groups_start;
    gchar **keys, **keys_start;
    gchar *value;

    GError *error;

    g_rec_mutex_lock(config_mutex);

    error = NULL;

    key_file = g_key_file_new();
    g_key_file_load_from_data(key_file,
			      buffer,
			      buffer_length,
			      G_KEY_FILE_NONE,
			      &error);

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }
    
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
	ags_config_set_value(config,
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

    g_rec_mutex_unlock(config_mutex);
  }
}

/**
 * ags_config_to_data:
 * @config: the #AgsConfig
 * @buffer: the data buffer
 * @buffer_length: the size of the buffer
 *
 * Save configuration.
 *
 * Since: 3.0.0
 */
void
ags_config_to_data(AgsConfig *config,
		   char **buffer, gsize *buffer_length)
{
  gchar *data;
  gsize length;

  GError *error;

  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* to data */
  g_rec_mutex_lock(config_mutex);

  error = NULL;
  data = g_key_file_to_data(config->key_file,
			    &length,
			    &error);

  if(error != NULL){
    g_warning("%s", error->message);

    g_error_free(error);
  }

  if(buffer != NULL){
    *buffer = data;
  }

  if(buffer_length != NULL){
    *buffer_length = length;
  }

  g_rec_mutex_unlock(config_mutex);
}

/**
 * ags_config_save:
 * @config: the #AgsConfig
 *
 * Save configuration.
 *
 * Since: 3.0.0
 */
void
ags_config_save(AgsConfig *config)
{
#ifdef AGS_W32API
  AgsApplicationContext *application_context;
#else
  struct passwd *pw;

  uid_t uid;
#endif
  
  gchar *path, *filename;
  gchar *content;
#if defined(AGS_W32API)
  gchar *app_dir;
#endif

  gsize length;

  GError *error;

  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* save */
  g_rec_mutex_lock(config_mutex);

  /* open conf dir */
#ifdef AGS_W32API
  app_dir = NULL;

  application_context = ags_application_context_get_instance();

  if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
    app_dir = g_strndup(application_context->argv[0],
			strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
  }
  
  path = g_strdup_printf("%s\\etc\\gsequencer",
			 g_get_current_dir());
    
  if(!g_file_test(path,
		  G_FILE_TEST_IS_DIR)){
    g_free(path);

    if(g_path_is_absolute(app_dir)){
      path = g_strdup_printf("%s\\%s",
			     app_dir,
			     "\\etc\\gsequencer");
    }else{
      path = g_strdup_printf("%s\\%s\\%s",
			     g_get_current_dir(),
			     app_dir,
			     "\\etc\\gsequencer");
    }
  }

  g_free(app_dir);

  filename = g_strdup_printf("%s\\%s",
			     path,
			     AGS_DEFAULT_CONFIG);
#else
  uid = getuid();
  pw = getpwuid(uid);

  path = g_strdup_printf("%s/%s",
			 pw->pw_dir,
			 AGS_DEFAULT_DIRECTORY);

  filename = g_strdup_printf("%s/%s",
			     path,
			     AGS_DEFAULT_CONFIG);
#endif    
  if(!g_mkdir_with_parents(path,
			   0755)){
    /* get content */
    error = NULL;

    content = g_key_file_to_data(config->key_file,
				 &length,
				 &error);
    
    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
      
      //TODO:JK: do recovery
      goto ags_config_save_END;
    }

    /* write content */
    error = NULL;

    g_file_set_contents(filename,
			content,
			length,
			&error);

    if(error != NULL){
      g_warning("%s", error->message);

      g_error_free(error);
    }
  }
    
ags_config_save_END:
  g_free(filename);
  g_free(path);

  g_rec_mutex_unlock(config_mutex);
}

void
ags_config_real_set_value(AgsConfig *config, gchar *group, gchar *key, gchar *value)
{
  GRecMutex *config_mutex;
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* set value */
  g_rec_mutex_lock(config_mutex);
  
  g_key_file_set_value(config->key_file, group, key, value);

  g_rec_mutex_unlock(config_mutex);
}

/**
 * ags_config_set_value:
 * @config: the #AgsConfig
 * @group: the config group identifier
 * @key: the key of the property
 * @value: the value to set
 *
 * Set config by @group and @key, applying @value.
 *
 * Since: 3.0.0
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
  
  GRecMutex *config_mutex;
  
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);

  /* get value */
  g_rec_mutex_lock(config_mutex);
  
  error = NULL;

  str = g_key_file_get_value(config->key_file, group, key, &error);

  if(error != NULL){
//    g_warning("%s", error->message);
    
    g_error_free(error);
  }
  
  g_rec_mutex_unlock(config_mutex);

  return(str);
}

/**
 * ags_config_get_value:
 * @config: the #AgsConfig
 * @group: the config group identifier
 * @key: the key of the property
 *
 * Retrieve config by @group and @key.
 *
 * Returns: (transfer full): the property's value
 *
 * Since: 3.0.0
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
 * ags_config_clear:
 * @config: the #AgsConfig
 *
 * Clears configuration.
 *
 * Since: 3.0.0
 */
void
ags_config_clear(AgsConfig *config)
{
  gchar **group;

  gsize n_group;
  guint i;

  GRecMutex *config_mutex;

  if(!AGS_IS_CONFIG(config)){
    return;
  }
    
  config_mutex = AGS_CONFIG_GET_OBJ_MUTEX(config);
  
  /* clear */
  g_rec_mutex_lock(config_mutex);

  group = g_key_file_get_groups(config->key_file,
				&n_group);

  for(i = 0; i < n_group; i++){
    g_key_file_remove_group(config->key_file,
    			    group[i],
    			    NULL);
  }

  g_rec_mutex_unlock(config_mutex);
}

/**
 * ags_config_get_instance:
 *
 * Get config instance.
 *
 * Returns: (transfer none): the config instance
 *
 * Since: 3.0.0
 */
AgsConfig*
ags_config_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_config == NULL){
    ags_config = ags_config_new();
  }

  g_mutex_unlock(&mutex);

  return(ags_config);
}

/**
 * ags_config_new:
 *
 * Create a new instance of #AgsConfig.
 *
 * Returns: the new #AgsConfig.
 *
 * Since: 3.0.0
 */
AgsConfig*
ags_config_new()
{
  AgsConfig *config;

  config = (AgsConfig *) g_object_new(AGS_TYPE_CONFIG,
				      NULL);

  return(config);
}
