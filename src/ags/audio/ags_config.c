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

#include <ags/audio/ags_config.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

void ags_config_class_init(AgsConfigClass *config_class);
void ags_config_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_config_init(AgsConfig *config);
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

/**
 * SECTION:ags_config
 * @short_description: Config Advanced Gtk+ Sequencer
 * @title: AgsConfig
 * @section_id:
 * @include: ags/audio/ags_config.h
 *
 * #AgsConfig provides configuration to Advanced Gtk+ Sequencer.
 */

static gpointer ags_config_parent_class = NULL;

static const gchar *ags_config_generic = AGS_CONFIG_GENERIC;
static const gchar *ags_config_thread = AGS_CONFIG_THREAD;
static const gchar *ags_config_devout = AGS_CONFIG_DEVOUT;

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

  ags_config_parent_class = g_type_class_peek_parent(config);

  /* GObjectClass */
  gobject = (GObjectClass *) config;

  gobject->finalize = ags_config_finalize;
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

  config->key_file = g_key_file_new();
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
  ags_config_set(config, ags_config_generic, "autosave-thread\0", "false\0");

  ags_config_set(config, ags_config_thread, "model\0", "multi-threaded\0");
  ags_config_set(config, ags_config_thread, "lock-global\0", "ags-thread\0");
  ags_config_set(config, ags_config_thread, "lock-parent\0", "ags-recycling-thread\0");

  ags_config_set(config, ags_config_devout, "samplerate\0", "44100\0");
  ags_config_set(config, ags_config_devout, "buffer-size\0", "940\0");
  ags_config_set(config, ags_config_devout, "pcm-channels\0", "2\0");
  ags_config_set(config, ags_config_devout, "dsp-channels\0", "2\0");
  ags_config_set(config, ags_config_devout, "alsa-handle\0", "hw:0,0\0");
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
  GError *error;

  error = NULL;

  g_key_file_load_from_file(config->key_file,
			    filename,
			    G_KEY_FILE_NONE,
			    &error);
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
ags_config_set(AgsConfig *config, gchar *group, gchar *key, gchar *value)
{
  if(!strncmp(group,
	      ags_config_generic,
	      8)){
    if(!strncmp(key,
		"autosave-thread\0",
		15)){
      //TODO:JK: implement me
    }
  }else if(!strncmp(group,
		    ags_config_thread,
		    7)){
    if(!strncmp(key,
		"model\0",
		6)){
      //TODO:JK: implement me
    }else if(!strncmp(key,
		      "lock-global\0",
		      11)){
      //TODO:JK: implement me
    }else if(!strncmp(key,
		      "lock-parent\0",
		      11)){
      //TODO:JK: implement me
    }
  }else if(!strncmp(group,
		    ags_config_devout,
		    7)){
    if(!strncmp(key,
		"samplerate\0",
		10)){
      //TODO:JK: implement me
    }else if(!strncmp(key,
		      "buffer-size\0",
		      11)){
      //TODO:JK: implement me
    }else if(!strncmp(key,
		      "pcm-channels\0",
		      12)){
      //TODO:JK: implement me
    }else if(!strncmp(key,
		      "dsp-channels\0",
		      12)){
      //TODO:JK: implement me
    }else if(!strncmp(key,
		      "alsa-handle\0",
		      11)){
      //TODO:JK: implement me
    }
  }

  g_key_file_set_value(config->key_file, group, key, value);
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
ags_config_get(AgsConfig *config, gchar *group, gchar *key)
{
  gchar *str;
  GError *error;

  error = NULL;

  str = g_key_file_get_value(config->key_file, group, key, &error);

  return(str);
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
ags_config_new()
{
  AgsConfig *config;

  config = (AgsConfig *) g_object_new(AGS_TYPE_CONFIG,
				      NULL);

  return(config);
}

