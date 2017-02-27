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

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/lib/ags_string_util.h>
#include <ags/lib/ags_log.h>

#include <ags/object/ags_marshal.h>

#include <ags/plugin/ags_base_plugin.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ladspa.h>

#include <ags/config.h>

void ags_ladspa_manager_class_init(AgsLadspaManagerClass *ladspa_manager);
void ags_ladspa_manager_init (AgsLadspaManager *ladspa_manager);
void ags_ladspa_manager_dispose(GObject *gobject);
void ags_ladspa_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_ladspa_manager
 * @short_description: Singleton pattern to organize LADSPA
 * @title: AgsLadspaManager
 * @section_id:
 * @include: ags/plugin/ags_ladspa_manager.h
 *
 * The #AgsLadspaManager loads/unloads LADSPA plugins.
 */
enum{
  ADD,
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_ladspa_manager_parent_class = NULL;
static guint ladspa_manager_signals[LAST_SIGNAL];

AgsLadspaManager *ags_ladspa_manager = NULL;
gchar **ags_ladspa_default_path = NULL;

GType
ags_ladspa_manager_get_type (void)
{
  static GType ags_type_ladspa_manager = 0;

  if(!ags_type_ladspa_manager){
    static const GTypeInfo ags_ladspa_manager_info = {
      sizeof (AgsLadspaManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_ladspa_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_manager_init,
    };

    ags_type_ladspa_manager = g_type_register_static(G_TYPE_OBJECT,
							"AgsLadspaManager\0",
							&ags_ladspa_manager_info,
							0);
  }

  return (ags_type_ladspa_manager);
}

void
ags_ladspa_manager_class_init(AgsLadspaManagerClass *ladspa_manager)
{
  GObjectClass *gobject;

  ags_ladspa_manager_parent_class = g_type_class_peek_parent(ladspa_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) ladspa_manager;

  gobject->dispose = ags_ladspa_manager_dispose;
  gobject->finalize = ags_ladspa_manager_finalize;
}

void
ags_ladspa_manager_init(AgsLadspaManager *ladspa_manager)
{
  ladspa_manager->ladspa_plugin = NULL;

  if(ags_ladspa_default_path == NULL){
    ags_ladspa_default_path = (gchar **) malloc(3 * sizeof(gchar *));

    ags_ladspa_default_path[0] = g_strdup("/usr/lib/ladspa\0");
    ags_ladspa_default_path[1] = g_strdup("/usr/lib64/ladspa\0");
    ags_ladspa_default_path[2] = NULL;
  }
}

void
ags_ladspa_manager_dispose(GObject *gobject)
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = AGS_LADSPA_MANAGER(gobject);

  if(ladspa_manager->ladspa_plugin != NULL){
    g_list_free_full(ladspa_manager->ladspa_plugin,
		     (GDestroyNotify) g_object_unref);

    ladspa_manager->ladspa_plugin = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_ladspa_manager_parent_class)->dispose(gobject);
}

void
ags_ladspa_manager_finalize(GObject *gobject)
{
  AgsLadspaManager *ladspa_manager;
  
  GList *ladspa_plugin;

  ladspa_manager = AGS_LADSPA_MANAGER(gobject);

  ladspa_plugin = ladspa_manager->ladspa_plugin;

  g_list_free_full(ladspa_plugin,
		   (GDestroyNotify) g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_ladspa_manager_parent_class)->finalize(gobject);
}

/**
 * ags_ladspa_manager_get_filenames:
 * @ladspa_manager: the #AgsLadspaManager
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 0.4
 */
gchar**
ags_ladspa_manager_get_filenames(AgsLadspaManager *ladspa_manager)
{
  GList *ladspa_plugin;

  gchar **filenames;

  guint i;
  gboolean contains_filename;
  
  ladspa_plugin = ladspa_manager->ladspa_plugin;
  filenames = NULL;
  
  for(i = 0; ladspa_plugin != NULL;){
    if(filenames == NULL){
      filenames = (gchar **) malloc(2 * sizeof(gchar *));
      filenames[i] = AGS_BASE_PLUGIN(ladspa_plugin->data)->filename;
      filenames[i + 1] = NULL;

      i++;
    }else{
#ifdef HAVE_GLIB_2_44
      contains_filename = g_strv_contains(filenames,
					  AGS_BASE_PLUGIN(ladspa_plugin->data)->filename);
#else
      contains_filename = ags_strv_contains(filenames,
					    AGS_BASE_PLUGIN(ladspa_plugin->data)->filename);
#endif
      
      if(!contains_filename){
	filenames = (gchar **) realloc(filenames,
				       (i + 2) * sizeof(gchar *));
	filenames[i] = AGS_BASE_PLUGIN(ladspa_plugin->data)->filename;
	filenames[i + 1] = NULL;

	i++;
      }
    }
    
    ladspa_plugin = ladspa_plugin->next;
  }

  return(filenames);
}

/**
 * ags_ladspa_manager_find_ladspa_plugin:
 * @ladspa_manager: the #AgsLadspaManager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsLadspaPlugin-struct
 *
 * Since: 0.4
 */
AgsLadspaPlugin*
ags_ladspa_manager_find_ladspa_plugin(AgsLadspaManager *ladspa_manager,
				      gchar *filename, gchar *effect)
{
  AgsLadspaPlugin *ladspa_plugin;

  GList *list;

  list = ladspa_manager->ladspa_plugin;

  while(list != NULL){
    ladspa_plugin = AGS_LADSPA_PLUGIN(list->data);
    
    if(!g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->filename,
		  filename) &&
       !g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->effect,
		  effect)){
      return(ladspa_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_ladspa_manager_load_blacklist:
 * @ladspa_manager: the #AgsLadspaManager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 0.7.108
 */
void
ags_ladspa_manager_load_blacklist(AgsLadspaManager *ladspa_manager,
				  gchar *blacklist_filename)
{
  if(g_file_test(blacklist_filename,
		 (G_FILE_TEST_EXISTS |
		  G_FILE_TEST_IS_REGULAR))){
    FILE *file;

    gchar *str;
    
    file = fopen(blacklist_filename,
		 "r\0");

    while(getline(&str, NULL, file) != -1){
      ladspa_manager->ladspa_plugin_blacklist = g_list_prepend(ladspa_manager->ladspa_plugin_blacklist,
							       str);
    }
  }
} 

/**
 * ags_ladspa_manager_load_file:
 * @ladspa_manager: the #AgsLadspaManager
 * @ladspa_path: the LADSPA path
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 0.4
 */
void
ags_ladspa_manager_load_file(AgsLadspaManager *ladspa_manager,
			     gchar *ladspa_path,
			     gchar *filename)
{
  AgsLadspaPlugin *ladspa_plugin;
  
  gchar *path;
  gchar *effect;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  unsigned long i;
  
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  path = g_strdup_printf("%s/%s\0",
			 ladspa_path,
			 filename);
  
  g_message("ags_ladspa_manager.c loading - %s\0", path);

  plugin_so = dlopen(path,
		     RTLD_NOW);
	
  if(plugin_so == NULL){
    g_warning("ags_ladspa_manager.c - failed to load static object file\0");
      
    dlerror();
    pthread_mutex_unlock(&(mutex));

    return;
  }

  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							 "ladspa_descriptor\0");
    
  if(dlerror() == NULL && ladspa_descriptor){
    for(i = 0; (plugin_descriptor = ladspa_descriptor(i)) != NULL; i++){
      if(ags_base_plugin_find_effect(ladspa_manager->ladspa_plugin,
				     path,
				     plugin_descriptor->Name) == NULL){
	ladspa_plugin = ags_ladspa_plugin_new(path,
					      plugin_descriptor->Name,
					      i);
	ags_base_plugin_load_plugin((AgsBasePlugin *) ladspa_plugin);
	ladspa_manager->ladspa_plugin = g_list_prepend(ladspa_manager->ladspa_plugin,
						       ladspa_plugin);
      }
    }
  }

  pthread_mutex_unlock(&(mutex));

  g_free(path);
}

/**
 * ags_ladspa_manager_load_default_directory:
 * @ladspa_manager: the #AgsLadspaManager
 * 
 * Loads all available plugins.
 *
 * Since: 0.4
 */
void
ags_ladspa_manager_load_default_directory(AgsLadspaManager *ladspa_manager)
{
  AgsLadspaPlugin *ladspa_plugin;

  GDir *dir;

  gchar **ladspa_path;
  gchar *filename;

  GError *error;

  ladspa_path = ags_ladspa_default_path;

  while(*ladspa_path != NULL){
    if(!g_file_test(*ladspa_path,
		    G_FILE_TEST_EXISTS)){
      ladspa_path++;
      
      continue;
    }
    
    error = NULL;
    dir = g_dir_open(*ladspa_path,
		     0,
		     &error);

    if(error != NULL){
      g_warning("%s\0", error->message);

      ladspa_path++;

      continue;
    }

    while((filename = g_dir_read_name(dir)) != NULL){
      if(g_str_has_suffix(filename,
			  ".so\0") &&
	 !g_list_find_custom(ladspa_manager->ladspa_plugin_blacklist,
			     filename,
			     strcmp)){
	ags_ladspa_manager_load_file(ladspa_manager,
				     *ladspa_path,
				     filename);
      }
    }
    
    ladspa_path++;
  }
}

/**
 * ags_ladspa_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLadspaManager
 *
 * Since: 0.4
 */
AgsLadspaManager*
ags_ladspa_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_ladspa_manager == NULL){
    ags_ladspa_manager = ags_ladspa_manager_new();

    pthread_mutex_unlock(&(mutex));
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_ladspa_manager);
}

/**
 * ags_ladspa_manager_new:
 *
 * Creates an #AgsLadspaManager
 *
 * Returns: a new #AgsLadspaManager
 *
 * Since: 0.7.6
 */
AgsLadspaManager*
ags_ladspa_manager_new()
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = (AgsLadspaManager *) g_object_new(AGS_TYPE_LADSPA_MANAGER,
						     NULL);

  return(ladspa_manager);
}
