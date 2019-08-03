/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/plugin/ags_lv2ui_manager.h>

#include <ags/lib/ags_string_util.h>
#include <ags/lib/ags_log.h>
#include <ags/lib/ags_turtle_manager.h>

#include <ags/object/ags_marshal.h>

#include <ags/plugin/ags_base_plugin.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>

#include <ags/config.h>

void ags_lv2ui_manager_class_init(AgsLv2uiManagerClass *lv2ui_manager);
void ags_lv2ui_manager_init (AgsLv2uiManager *lv2ui_manager);
void ags_lv2ui_manager_dispose(GObject *gobject);
void ags_lv2ui_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2ui_manager
 * @short_description: Singleton pattern to organize LV2UI
 * @title: AgsLv2uiManager
 * @section_id:
 * @include: ags/plugin/ags_lv2ui_manager.h
 *
 * The #AgsLv2uiManager loads/unloads LV2UI plugins.
 */

static gpointer ags_lv2ui_manager_parent_class = NULL;

static pthread_mutex_t ags_lv2ui_manager_class_mutex = PTHREAD_MUTEX_INITIALIZER;

AgsLv2uiManager *ags_lv2ui_manager = NULL;
gchar **ags_lv2ui_default_path = NULL;

GType
ags_lv2ui_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2ui_manager = 0;

    static const GTypeInfo ags_lv2ui_manager_info = {
      sizeof (AgsLv2uiManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2ui_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2uiManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2ui_manager_init,
    };

    ags_type_lv2ui_manager = g_type_register_static(G_TYPE_OBJECT,
						    "AgsLv2uiManager",
						    &ags_lv2ui_manager_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2ui_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2ui_manager_class_init(AgsLv2uiManagerClass *lv2ui_manager)
{
  GObjectClass *gobject;
  
  ags_lv2ui_manager_parent_class = g_type_class_peek_parent(lv2ui_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2ui_manager;

  gobject->dispose = ags_lv2ui_manager_dispose;
  gobject->finalize = ags_lv2ui_manager_finalize;
}

void
ags_lv2ui_manager_init(AgsLv2uiManager *lv2ui_manager)
{
  /* lv2ui manager mutex */
  lv2ui_manager->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(lv2ui_manager->obj_mutexattr);
  pthread_mutexattr_settype(lv2ui_manager->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(lv2ui_manager->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  lv2ui_manager->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(lv2ui_manager->obj_mutex,
		     lv2ui_manager->obj_mutexattr);

  lv2ui_manager->lv2ui_plugin = NULL;

  if(ags_lv2ui_default_path == NULL){
    gchar *lv2ui_env;

    if((lv2ui_env = getenv("LV2_PATH")) != NULL){
      gchar *iter, *next;
      guint i;
      
      ags_lv2ui_default_path = (gchar **) malloc(sizeof(gchar *));

      iter = lv2ui_env;
      i = 0;
      
      while((next = index(iter, ':')) != NULL){
	ags_lv2ui_default_path = (gchar **) realloc(ags_lv2ui_default_path,
						    (i + 2) * sizeof(gchar *));
	ags_lv2ui_default_path[i] = g_strndup(iter,
					      next - iter);

	iter = next + 1;
	i++;
      }

      if(*iter != '\0'){
	ags_lv2ui_default_path = (gchar **) realloc(ags_lv2ui_default_path,
						    (i + 2) * sizeof(gchar *));
	ags_lv2ui_default_path[i] = g_strdup(iter);

	i++;	
      }

      ags_lv2ui_default_path[i] = NULL;
    }else{
      gchar *home_dir;
      guint i;

#ifdef __APPLE__
#ifdef AGS_MAC_BUNDLE
      if((home_dir = getenv("HOME")) != NULL){
	ags_lv2ui_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }else{
	ags_lv2ui_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }
#else
      if((home_dir = getenv("HOME")) != NULL){
	ags_lv2ui_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }else{
	ags_lv2ui_default_path = (gchar **) malloc(4 * sizeof(gchar *));
      }
#endif
    
      i = 0;

#ifdef AGS_MAC_BUNDLE
      ags_lv2ui_default_path[i++] = g_strdup_printf("%s/lv2",
						    getenv("GSEQUENCER_PLUGIN_DIR"));
#endif

      ags_lv2ui_default_path[i++] = g_strdup("/Library/Audio/Plug-Ins/LV2");
      ags_lv2ui_default_path[i++] = g_strdup("/usr/lib/lv2");
      ags_lv2ui_default_path[i++] = g_strdup("/usr/local/lib/lv2");

      if(home_dir != NULL){
	ags_lv2ui_default_path[i++] = g_strdup_printf("%s/Library/Audio/Plug-Ins/LV2",
						      home_dir);
      }
    
      ags_lv2ui_default_path[i++] = NULL;
#else
      if((home_dir = getenv("HOME")) != NULL){
	ags_lv2ui_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }else{
	ags_lv2ui_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }
    
      i = 0;
    
      ags_lv2ui_default_path[i++] = g_strdup("/usr/lib64/lv2");
      ags_lv2ui_default_path[i++] = g_strdup("/usr/local/lib64/lv2");
      ags_lv2ui_default_path[i++] = g_strdup("/usr/lib/lv2");
      ags_lv2ui_default_path[i++] = g_strdup("/usr/local/lib/lv2");

      if(home_dir != NULL){
	ags_lv2ui_default_path[i++] = g_strdup_printf("%s/.lv2",
						      home_dir);
      }
    
      ags_lv2ui_default_path[i++] = NULL;
#endif
    }
  }
}

void
ags_lv2ui_manager_dispose(GObject *gobject)
{
  AgsLv2uiManager *lv2ui_manager;

  lv2ui_manager = AGS_LV2UI_MANAGER(gobject);

  if(lv2ui_manager->lv2ui_plugin != NULL){
    g_list_free_full(lv2ui_manager->lv2ui_plugin,
		     g_object_unref);

    lv2ui_manager->lv2ui_plugin = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2ui_manager_parent_class)->dispose(gobject);
}

void
ags_lv2ui_manager_finalize(GObject *gobject)
{
  AgsLv2uiManager *lv2ui_manager;

  GList *lv2ui_plugin;

  lv2ui_manager = AGS_LV2UI_MANAGER(gobject);

  lv2ui_plugin = lv2ui_manager->lv2ui_plugin;

  g_list_free_full(lv2ui_plugin,
		   g_object_unref);

  if(lv2ui_manager == ags_lv2ui_manager){
    ags_lv2ui_manager = NULL;
  }
  
  pthread_mutex_destroy(lv2ui_manager->obj_mutex);
  free(lv2ui_manager->obj_mutex);

  pthread_mutexattr_destroy(lv2ui_manager->obj_mutexattr);
  free(lv2ui_manager->obj_mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_lv2ui_manager_parent_class)->finalize(gobject);
}

/**
 * ags_lv2ui_manager_get_class_mutex:
 * 
 * Get class mutex.
 * 
 * Returns: the class mutex of #AgsLv2uiManager
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_lv2ui_manager_get_class_mutex()
{
  return(&ags_lv2ui_manager_class_mutex);
}

/**
 * ags_lv2ui_manager_get_default_path:
 * 
 * Get lv2ui manager default plugin path.
 *
 * Returns: the plugin default search path as a string vector
 * 
 * Since: 2.0.0
 */
gchar**
ags_lv2ui_manager_get_default_path()
{
  return(ags_lv2ui_default_path);
}

/**
 * ags_lv2ui_manager_set_default_path:
 * @default_path: the string vector array to use as default path
 * 
 * Set lv2ui manager default plugin path.
 * 
 * Since: 2.0.0
 */
void
ags_lv2ui_manager_set_default_path(gchar** default_path)
{
  ags_lv2ui_default_path = default_path;
}

/**
 * ags_lv2ui_manager_get_filenames:
 * @lv2ui_manager: the #AgsLv2uiManager
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 2.0.0
 */
gchar**
ags_lv2ui_manager_get_filenames(AgsLv2uiManager *lv2ui_manager)
{
  GList *start_lv2ui_plugin, *lv2ui_plugin;
  
  gchar **filenames;
  
  guint i;
  gboolean contains_filename;

  pthread_mutex_t *lv2ui_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2UI_MANAGER(lv2ui_manager)){
    return(NULL);
  }
  
  /* get lv2ui manager mutex */
  lv2ui_manager_mutex = AGS_LV2UI_MANAGER_GET_OBJ_MUTEX(lv2ui_manager);

  /* collect */
  pthread_mutex_lock(lv2ui_manager_mutex);

  lv2ui_plugin = 
    start_lv2ui_plugin = g_list_copy(lv2ui_manager->lv2ui_plugin);

  pthread_mutex_unlock(lv2ui_manager_mutex);

  filenames = NULL;
  
  for(i = 0; lv2ui_plugin != NULL;){
    gchar *ui_filename;

    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin->data);

    /* duplicate filename */
    pthread_mutex_lock(base_plugin_mutex);

    ui_filename = g_strdup(AGS_BASE_PLUGIN(lv2ui_plugin->data)->ui_filename);

    pthread_mutex_unlock(base_plugin_mutex);

    if(ui_filename == NULL){
      lv2ui_plugin = lv2ui_plugin->next;

      continue;
    }
    
    if(filenames == NULL){
      filenames = (gchar **) malloc(2 * sizeof(gchar *));
      filenames[i] = ui_filename;
      filenames[i + 1] = NULL;

      i++;
    }else{
#ifdef HAVE_GLIB_2_44
      contains_filename = g_strv_contains(filenames,
					  ui_filename);
#else
      contains_filename = ags_strv_contains(filenames,
					    ui_filename);
#endif
      
      if(!contains_filename){
	filenames = (gchar **) realloc(filenames,
				       (i + 2) * sizeof(gchar *));
	filenames[i] = ui_filename;
	filenames[i + 1] = NULL;
	
	i++;
      }else{
	g_free(ui_filename);
      }
    }
    
    lv2ui_plugin = lv2ui_plugin->next;
  }

  g_list_free(start_lv2ui_plugin);

  return(filenames);
}

/**
 * ags_lv2ui_manager_find_lv2ui_plugin:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @ui_filename: the UI filename of the plugin
 * @ui_effect: the UI effect's name
 *
 * Lookup UI filename in loaded plugins.
 *
 * Returns: the #AgsLv2uiPlugin-struct
 *
 * Since: 2.0.0
 */
AgsLv2uiPlugin*
ags_lv2ui_manager_find_lv2ui_plugin(AgsLv2uiManager *lv2ui_manager,
				    gchar *ui_filename, gchar *ui_effect)
{
  AgsLv2uiPlugin *lv2ui_plugin;
  
  GList *start_list, *list;

  gboolean success;  

  pthread_mutex_t *lv2ui_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2UI_MANAGER(lv2ui_manager) ||
     ui_filename == NULL ||
     ui_effect == NULL){
    return(NULL);
  }
  
  /* get lv2 manager mutex */
  lv2ui_manager_mutex = AGS_LV2UI_MANAGER_GET_OBJ_MUTEX(lv2ui_manager);

  /* collect */
  pthread_mutex_lock(lv2ui_manager_mutex);

  list = 
    start_list = g_list_copy(lv2ui_manager->lv2ui_plugin);

  pthread_mutex_unlock(lv2ui_manager_mutex);
  
  while(list != NULL){
    lv2ui_plugin = AGS_LV2UI_PLUGIN(list->data);
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

    /* check filename and effect */
    pthread_mutex_lock(base_plugin_mutex);

    success = (!g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
			  ui_filename) &&
	       !g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_effect,
			  ui_effect)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(base_plugin_mutex);
    
    if(success){
      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  if(!success){
    lv2ui_plugin = NULL;
  }

  return(lv2ui_plugin);
}

/**
 * ags_lv2ui_manager_find_lv2ui_plugin_with_index:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @ui_filename: the UI filename of the plugin
 * @ui_effect_index: the UI index
 *
 * Lookup @ui_filename with @ui_effect_index in loaded plugins.
 *
 * Returns: the #AgsLv2uiPlugin-struct
 *
 * Since: 2.0.0
 */
AgsLv2uiPlugin*
ags_lv2ui_manager_find_lv2ui_plugin_with_index(AgsLv2uiManager *lv2ui_manager,
					       gchar *ui_filename,
					       guint ui_effect_index)
{
  AgsLv2uiPlugin *lv2ui_plugin;
  
  GList *start_list, *list;

  gboolean success;  

  pthread_mutex_t *lv2ui_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2UI_MANAGER(lv2ui_manager) ||
     ui_filename == NULL){
    return(NULL);
  }
  
  /* get lv2 manager mutex */
  lv2ui_manager_mutex = AGS_LV2UI_MANAGER_GET_OBJ_MUTEX(lv2ui_manager);

  /* collect */
  pthread_mutex_lock(lv2ui_manager_mutex);

  list = 
    start_list = g_list_copy(lv2ui_manager->lv2ui_plugin);

  pthread_mutex_unlock(lv2ui_manager_mutex);

  success = FALSE;
  
  while(list != NULL){
    lv2ui_plugin = AGS_LV2UI_PLUGIN(list->data);
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2ui_plugin);

    /* check filename and ui effect index */
    pthread_mutex_lock(base_plugin_mutex);

    if(!g_ascii_strcasecmp(AGS_BASE_PLUGIN(lv2ui_plugin)->ui_filename,
			   ui_filename) &&
       AGS_BASE_PLUGIN(lv2ui_plugin)->ui_effect_index == ui_effect_index){
      success = TRUE;
    }

    pthread_mutex_unlock(base_plugin_mutex);
    
    if(success){
      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  if(!success){
    lv2ui_plugin = NULL;
  }

  return(lv2ui_plugin);
}

/**
 * ags_lv2ui_manager_load_blacklist:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 2.0.0
 */
void
ags_lv2ui_manager_load_blacklist(AgsLv2uiManager *lv2ui_manager,
				 gchar *blacklist_filename)
{
  pthread_mutex_t *lv2ui_manager_mutex;

  if(!AGS_IS_LV2UI_MANAGER(lv2ui_manager) ||
     blacklist_filename == NULL){
    return;
  }

  /* get lv2 manager mutex */
  lv2ui_manager_mutex = AGS_LV2UI_MANAGER_GET_OBJ_MUTEX(lv2ui_manager);

  /* fill in */
  pthread_mutex_lock(lv2ui_manager_mutex);

  if(g_file_test(blacklist_filename,
		 (G_FILE_TEST_EXISTS |
		  G_FILE_TEST_IS_REGULAR))){
    FILE *file;

    gchar *str;
    
    file = fopen(blacklist_filename,
		 "r");

    while(getline(&str, NULL, file) != -1){
      lv2ui_manager->lv2ui_plugin_blacklist = g_list_prepend(lv2ui_manager->lv2ui_plugin_blacklist,
							     str);
    }
  }

  pthread_mutex_unlock(lv2ui_manager_mutex);
} 

/**
 * ags_lv2ui_manager_load_file:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @manifest: the manifest
 * @turtle: the #AgsTurtle
 * @lv2ui_path: the lv2ui path
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 2.0.0
 */
void
ags_lv2ui_manager_load_file(AgsLv2uiManager *lv2ui_manager,
			    AgsTurtle *manifest,
			    AgsTurtle *turtle,
			    gchar *lv2ui_path,
			    gchar *filename)
{
}

/**
 * ags_lv2ui_manager_load_default_directory:
 * @lv2ui_manager: the #AgsLv2uiManager
 * 
 * Loads all available plugins.
 *
 * Since: 2.0.0
 */
void
ags_lv2ui_manager_load_default_directory(AgsLv2uiManager *lv2ui_manager)
{
}

/**
 * ags_lv2ui_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLv2uiManager
 *
 * Since: 2.0.0
 */
AgsLv2uiManager*
ags_lv2ui_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_lv2ui_manager == NULL){
    ags_lv2ui_manager = ags_lv2ui_manager_new();
  }

  pthread_mutex_unlock(&mutex);

  return(ags_lv2ui_manager);
}

/**
 * ags_lv2ui_manager_new:
 *
 * Creates an #AgsLv2uiManager
 *
 * Returns: a new #AgsLv2uiManager
 *
 * Since: 2.0.0
 */
AgsLv2uiManager*
ags_lv2ui_manager_new()
{
  AgsLv2uiManager *lv2ui_manager;

  lv2ui_manager = (AgsLv2uiManager *) g_object_new(AGS_TYPE_LV2UI_MANAGER,
						   NULL);

  return(lv2ui_manager);
}
