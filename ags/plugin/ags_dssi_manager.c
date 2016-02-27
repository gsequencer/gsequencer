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

#include <ags/plugin/ags_dssi_manager.h>

#include <ags/object/ags_marshal.h>

#include <ags/plugin/ags_base_plugin.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <dssi.h>

void ags_dssi_manager_class_init(AgsDssiManagerClass *dssi_manager);
void ags_dssi_manager_init (AgsDssiManager *dssi_manager);
void ags_dssi_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_dssi_manager
 * @short_description: Singleton pattern to organize DSSI
 * @title: AgsDssiManager
 * @section_id:
 * @include: ags/object/ags_dssi_manager.h
 *
 * The #AgsDssiManager loads/unloads DSSI plugins.
 */
enum{
  ADD,
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_dssi_manager_parent_class = NULL;
static guint dssi_manager_signals[LAST_SIGNAL];

AgsDssiManager *ags_dssi_manager = NULL;
static const gchar *ags_dssi_default_path = "/usr/lib/dssi\0";

GType
ags_dssi_manager_get_type (void)
{
  static GType ags_type_dssi_manager = 0;

  if(!ags_type_dssi_manager){
    static const GTypeInfo ags_dssi_manager_info = {
      sizeof (AgsDssiManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_dssi_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDssiManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dssi_manager_init,
    };

    ags_type_dssi_manager = g_type_register_static(G_TYPE_OBJECT,
						   "AgsDssiManager\0",
						   &ags_dssi_manager_info,
						   0);
  }

  return (ags_type_dssi_manager);
}

void
ags_dssi_manager_class_init(AgsDssiManagerClass *dssi_manager)
{
  GObjectClass *gobject;

  ags_dssi_manager_parent_class = g_type_class_peek_parent(dssi_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) dssi_manager;

  gobject->finalize = ags_dssi_manager_finalize;
}

void
ags_dssi_manager_init(AgsDssiManager *dssi_manager)
{
  dssi_manager->dssi_plugin = NULL;
}

void
ags_dssi_manager_finalize(GObject *gobject)
{
  AgsDssiManager *dssi_manager;
  GList *dssi_plugin;

  dssi_manager = AGS_DSSI_MANAGER(gobject);

  dssi_plugin = dssi_manager->dssi_plugin;

  g_list_free_full(dssi_plugin,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_dssi_manager_get_filenames:
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 0.7.0
 */
gchar**
ags_dssi_manager_get_filenames()
{
  AgsDssiManager *dssi_manager;
  GList *dssi_plugin;
  gchar **filenames;
  guint i;

  dssi_manager = ags_dssi_manager_get_instance();

  dssi_plugin = dssi_manager->dssi_plugin;
  filenames = NULL;
  
  for(i = 0; dssi_plugin != NULL;){
    if(filenames == NULL){
      filenames = (gchar **) malloc(2 * sizeof(gchar *));
      filenames[i] = AGS_BASE_PLUGIN(dssi_plugin->data)->filename;
      filenames[i + 1] = NULL;

      i++;
    }else{
      if(!g_strv_contains(filenames,
			  AGS_BASE_PLUGIN(dssi_plugin->data)->filename)){
	filenames = (gchar **) realloc(filenames,
				       (i + 2) * sizeof(gchar *));
	filenames[i] = AGS_BASE_PLUGIN(dssi_plugin->data)->filename;
	filenames[i + 1] = NULL;

	i++;
      }
    }
    
    dssi_plugin = dssi_plugin->next;
  }

  return(filenames);
}

/**
 * ags_dssi_manager_find_dssi_plugin:
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsDssiPlugin-struct
 *
 * Since: 0.7.0
 */
AgsDssiPlugin*
ags_dssi_manager_find_dssi_plugin(gchar *filename, gchar *effect)
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;
  GList *list;

  dssi_manager = ags_dssi_manager_get_instance();

  list = dssi_manager->dssi_plugin;

  while(list != NULL){
    dssi_plugin = AGS_DSSI_PLUGIN(list->data);
    
    if(!g_strcmp0(AGS_BASE_PLUGIN(dssi_plugin)->filename,
		  filename) &&
       !g_strcmp0(AGS_BASE_PLUGIN(dssi_plugin)->effect,
		  effect)){
      return(dssi_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_dssi_manager_load_file:
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 0.7.0
 */
void
ags_dssi_manager_load_file(gchar *filename)
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  gchar *path;
  gchar *effect;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  unsigned long i;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  dssi_manager = ags_dssi_manager_get_instance();

  pthread_mutex_lock(&(mutex));

  path = g_strdup_printf("%s/%s\0",
			 ags_dssi_default_path,
			 filename);

  g_message("ags_dssi_manager.c loading - %s\0", path);

  plugin_so = dlopen(path,
		     RTLD_NOW);
	
  if(plugin_so == NULL){
    g_warning("ags_dssi_manager.c - failed to load static object file\0");
      
    dlerror();
  }

  dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						     "dssi_descriptor\0");
    
  if(dlerror() == NULL && dssi_descriptor){
    for(i = 0; (plugin_descriptor = dssi_descriptor(i)) != NULL; i++){
      dssi_plugin = ags_dssi_plugin_new(path,
					plugin_descriptor->LADSPA_Plugin->Name,
					i);
      ags_base_plugin_load_plugin(dssi_plugin);
      dssi_manager->dssi_plugin = g_list_prepend(dssi_manager->dssi_plugin,
						 dssi_plugin);
    }
  }

  pthread_mutex_unlock(&(mutex));

  g_free(path);
}

/**
 * ags_dssi_manager_load_default_directory:
 * 
 * Loads all available plugins.
 *
 * Since: 0.7.0
 */
void
ags_dssi_manager_load_default_directory()
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;
  GDir *dir;
  gchar *filename;
  GError *error;

  dssi_manager = ags_dssi_manager_get_instance();

  error = NULL;
  dir = g_dir_open(ags_dssi_default_path,
		   0,
		   &error);

  if(error != NULL){
    g_warning("%s\0", error->message);
  }

  while((filename = g_dir_read_name(dir)) != NULL){
    if(g_str_has_suffix(filename,
			".so\0")){
      ags_dssi_manager_load_file(filename);
    }
  }
}

/**
 * ags_dssi_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsDssiManager
 *
 * Since: 0.7.0
 */
AgsDssiManager*
ags_dssi_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_dssi_manager == NULL){
    ags_dssi_manager = ags_dssi_manager_new();

    pthread_mutex_unlock(&(mutex));

    ags_dssi_manager_load_default_directory();
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_dssi_manager);
}

/**
 * ags_dssi_manager_new:
 * @devout: an #AgsDevout
 *
 * Creates an #AgsDssiManager
 *
 * Returns: a new #AgsDssiManager
 *
 * Since: 0.7.0
 */
AgsDssiManager*
ags_dssi_manager_new()
{
  AgsDssiManager *dssi_manager;

  dssi_manager = (AgsDssiManager *) g_object_new(AGS_TYPE_DSSI_MANAGER,
						 NULL);

  return(dssi_manager);
}
