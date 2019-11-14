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

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/plugin/ags_base_plugin.h>

#if defined(AGS_W32API)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <strings.h>

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

AgsLadspaManager *ags_ladspa_manager = NULL;
gchar **ags_ladspa_default_path = NULL;

GType
ags_ladspa_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_ladspa_manager = 0;

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
						     "AgsLadspaManager",
						     &ags_ladspa_manager_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_ladspa_manager);
  }

  return g_define_type_id__volatile;
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
  /* ladspa manager mutex */
  g_rec_mutex_init(&(ladspa_manager->obj_mutex));

  /* ladspa plugin and path */
  ladspa_manager->ladspa_plugin = NULL;

  if(ags_ladspa_default_path == NULL){
    gchar *ladspa_env;

    if((ladspa_env = getenv("LADSPA_PATH")) != NULL){
      gchar *iter, *next;
      guint i;
      
      ags_ladspa_default_path = (gchar **) malloc(sizeof(gchar *));

      iter = ladspa_env;
      i = 0;
      
      while((next = strchr(iter, G_SEARCHPATH_SEPARATOR)) != NULL){
	ags_ladspa_default_path = (gchar **) realloc(ags_ladspa_default_path,
						     (i + 2) * sizeof(gchar *));
	ags_ladspa_default_path[i] = g_strndup(iter,
					       next - iter);

	iter = next + 1;
	i++;
      }

      if(*iter != '\0'){
	ags_ladspa_default_path = (gchar **) realloc(ags_ladspa_default_path,
						     (i + 2) * sizeof(gchar *));
	ags_ladspa_default_path[i] = g_strdup(iter);

	i++;	
      }

      ags_ladspa_default_path[i] = NULL;
    }else{
#if defined(AGS_W32API)
      AgsApplicationContext *application_context;
      
      gchar *app_dir;
      gchar *path;
      
      guint i;

      i = 0;
      
      application_context = ags_application_context_get_instance();

      app_dir = NULL;
          
      if(strlen(application_context->argv[0]) > strlen("\\gsequencer.exe")){
	app_dir = g_strndup(application_context->argv[0],
			    strlen(application_context->argv[0]) - strlen("\\gsequencer.exe"));
      }

      ags_ladspa_default_path = (gchar **) malloc(2 * sizeof(gchar *));

      path = g_strdup_printf("%s\\ladspa",
			     g_get_current_dir());
      
      if(g_file_test(path,
		     G_FILE_TEST_IS_DIR)){
	ags_ladspa_default_path[i++] = path;
      }else{
	g_free(path);

	if(g_path_is_absolute(app_dir)){
	  ags_ladspa_default_path[i++] = g_strdup_printf("%s\\ladspa",
							 app_dir);
	}else{
	  ags_ladspa_default_path[i++] = g_strdup_printf("%s\\%s\\ladspa",
							 g_get_current_dir(),
							 app_dir);
	}
      }
      
      ags_ladspa_default_path[i++] = NULL;

      g_free(app_dir);
#else
      gchar *home_dir;
      guint i;

#ifdef AGS_MAC_BUNDLE
      if((home_dir = getenv("HOME")) != NULL){
	ags_ladspa_default_path = (gchar **) malloc(7 * sizeof(gchar *));
      }else{
	ags_ladspa_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }
#else
      if((home_dir = getenv("HOME")) != NULL){
	ags_ladspa_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }else{
	ags_ladspa_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }
#endif
      
      i = 0;

#ifdef AGS_MAC_BUNDLE
      ags_ladspa_default_path[i++] = g_strdup_printf("%s/ladspa",
						     getenv("GSEQUENCER_PLUGIN_DIR"));
#endif
      
      ags_ladspa_default_path[i++] = g_strdup("/usr/lib64/ladspa");
      ags_ladspa_default_path[i++] = g_strdup("/usr/local/lib64/ladspa");
      ags_ladspa_default_path[i++] = g_strdup("/usr/lib/ladspa");
      ags_ladspa_default_path[i++] = g_strdup("/usr/local/lib/ladspa");

      if(home_dir != NULL){
	ags_ladspa_default_path[i++] = g_strdup_printf("%s/.ladspa",
						       home_dir);
      }
    
      ags_ladspa_default_path[i++] = NULL;
#endif
    }
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

  if(ladspa_manager == ags_ladspa_manager){
    ags_ladspa_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_ladspa_manager_parent_class)->finalize(gobject);
}

/**
 * ags_ladspa_manager_get_default_path:
 * 
 * Get ladspa manager default plugin path.
 *
 * Returns: the plugin default search path as a string vector
 * 
 * Since: 2.0.0
 */
gchar**
ags_ladspa_manager_get_default_path()
{
  return(ags_ladspa_default_path);
}

/**
 * ags_ladspa_manager_set_default_path:
 * @default_path: the string vector array to use as default path
 * 
 * Set ladspa manager default plugin path.
 * 
 * Since: 2.0.0
 */
void
ags_ladspa_manager_set_default_path(gchar** default_path)
{
  ags_ladspa_default_path = default_path;
}

/**
 * ags_ladspa_manager_get_filenames:
 * @ladspa_manager: the #AgsLadspaManager
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 2.0.0
 */
gchar**
ags_ladspa_manager_get_filenames(AgsLadspaManager *ladspa_manager)
{
  GList *start_ladspa_plugin, *ladspa_plugin;

  gchar **filenames;

  guint i;
  gboolean contains_filename;

  GRecMutex *ladspa_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_LADSPA_MANAGER(ladspa_manager)){
    return(NULL);
  }
  
  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* collect */
  g_rec_mutex_lock(ladspa_manager_mutex);

  ladspa_plugin = 
    start_ladspa_plugin = g_list_copy(ladspa_manager->ladspa_plugin);

  g_rec_mutex_unlock(ladspa_manager_mutex);

  filenames = NULL;
  
  for(i = 0; ladspa_plugin != NULL;){
    gchar *filename;
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin->data);

    /* duplicate filename */
    g_rec_mutex_lock(base_plugin_mutex);

    filename = g_strdup(AGS_BASE_PLUGIN(ladspa_plugin->data)->filename);

    g_rec_mutex_unlock(base_plugin_mutex);

    if(filenames == NULL){
      filenames = (gchar **) malloc(2 * sizeof(gchar *));

      /* set filename */
      filenames[i] = filename;

      /* terminate */
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
	filenames[i] = filename;
	filenames[i + 1] = NULL;

	i++;
      }else{
	g_free(filename);
      }
    }
    
    ladspa_plugin = ladspa_plugin->next;
  }

  g_list_free(start_ladspa_plugin);

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
 * Since: 2.0.0
 */
AgsLadspaPlugin*
ags_ladspa_manager_find_ladspa_plugin(AgsLadspaManager *ladspa_manager,
				      gchar *filename, gchar *effect)
{
  AgsLadspaPlugin *ladspa_plugin;

  GList *start_list, *list;

  gboolean success;  

  GRecMutex *ladspa_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_LADSPA_MANAGER(ladspa_manager)){
    return(NULL);
  }
  
  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* collect */
  g_rec_mutex_lock(ladspa_manager_mutex);

  list = 
    start_list = g_list_copy(ladspa_manager->ladspa_plugin);

  g_rec_mutex_unlock(ladspa_manager_mutex);

  success = FALSE;
  
  while(list != NULL){
    ladspa_plugin = AGS_LADSPA_PLUGIN(list->data);

    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin);

    /* check filename and effect */
    g_rec_mutex_lock(base_plugin_mutex);

    success = (!g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->filename,
			  filename) &&
	       !g_strcmp0(AGS_BASE_PLUGIN(ladspa_plugin)->effect,
			  effect)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(base_plugin_mutex);
    
    if(success){
      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  if(!success){
    ladspa_plugin = NULL;
  }

  return(ladspa_plugin);
}

/**
 * ags_ladspa_manager_load_blacklist:
 * @ladspa_manager: the #AgsLadspaManager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 2.0.0
 */
void
ags_ladspa_manager_load_blacklist(AgsLadspaManager *ladspa_manager,
				  gchar *blacklist_filename)
{
  GRecMutex *ladspa_manager_mutex;

  if(!AGS_IS_LADSPA_MANAGER(ladspa_manager) ||
     blacklist_filename == NULL){
    return;
  }
  
  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* fill in */
  g_rec_mutex_lock(ladspa_manager_mutex);

  if(g_file_test(blacklist_filename,
		 (G_FILE_TEST_EXISTS |
		  G_FILE_TEST_IS_REGULAR))){
    FILE *file;

    gchar *str;
    
    file = fopen(blacklist_filename,
		 "r");

#ifndef AGS_W32API    
    while(getline(&str, NULL, file) != -1){
      ladspa_manager->ladspa_plugin_blacklist = g_list_prepend(ladspa_manager->ladspa_plugin_blacklist,
							       str);
    }
#endif
  }

  g_rec_mutex_unlock(ladspa_manager_mutex);
} 

/**
 * ags_ladspa_manager_load_file:
 * @ladspa_manager: the #AgsLadspaManager
 * @ladspa_path: the LADSPA path
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 2.0.0
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
  gboolean success;
  
  GRecMutex *ladspa_manager_mutex;

  if(!AGS_IS_LADSPA_MANAGER(ladspa_manager) ||
     ladspa_path == NULL ||
     filename == NULL){
    return;
  }
  
  /* get ladspa manager mutex */
  ladspa_manager_mutex = AGS_LADSPA_MANAGER_GET_OBJ_MUTEX(ladspa_manager);

  /* load */
  g_rec_mutex_lock(ladspa_manager_mutex);

  path = g_strdup_printf("%s%c%s",
			 ladspa_path,
			 G_DIR_SEPARATOR,
			 filename);
  
  g_message("ags_ladspa_manager.c loading - %s", path);

#ifdef AGS_W32API
  plugin_so = LoadLibrary(path);
#else
  plugin_so = dlopen(path,
		     RTLD_NOW);
#endif
	
  if(plugin_so == NULL){
    g_warning("ags_ladspa_manager.c - failed to load static object file");
      
#ifndef AGS_W32API
    dlerror();
#endif
    
    g_rec_mutex_unlock(ladspa_manager_mutex);

    return;
  }

  success = FALSE;
    
#ifdef AGS_W32API
  ladspa_descriptor = (LADSPA_Descriptor_Function) GetProcAddress((void *) plugin_so,
								  "ladspa_descriptor");

  success = (!ladspa_descriptor) ? FALSE: TRUE;
#else
  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym((void *) plugin_so,
							 "ladspa_descriptor");
  
  success = (dlerror() == NULL) ? TRUE: FALSE;
#endif
    
  if(success && ladspa_descriptor){
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

  g_rec_mutex_unlock(ladspa_manager_mutex);

  g_free(path);
}

/**
 * ags_ladspa_manager_load_default_directory:
 * @ladspa_manager: the #AgsLadspaManager
 * 
 * Loads all available plugins.
 *
 * Since: 2.0.0
 */
void
ags_ladspa_manager_load_default_directory(AgsLadspaManager *ladspa_manager)
{
  AgsLadspaPlugin *ladspa_plugin;

  GDir *dir;

  gchar **ladspa_path;
  gchar *filename;

  GError *error;

  if(!AGS_IS_LADSPA_MANAGER(ladspa_manager)){
    return;
  }
  
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
      g_warning("%s", error->message);

      ladspa_path++;

      g_error_free(error);
      
      continue;
    }

    while((filename = g_dir_read_name(dir)) != NULL){
      if(g_str_has_suffix(filename,
			  AGS_LIBRARY_SUFFIX) &&
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
 * Since: 2.0.0
 */
AgsLadspaManager*
ags_ladspa_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&mutex);

  if(ags_ladspa_manager == NULL){
    ags_ladspa_manager = ags_ladspa_manager_new();
  }

  g_mutex_unlock(&mutex);

  return(ags_ladspa_manager);
}

/**
 * ags_ladspa_manager_new:
 *
 * Create a new instance of #AgsLadspaManager
 *
 * Returns: the new #AgsLadspaManager
 *
 * Since: 2.0.0
 */
AgsLadspaManager*
ags_ladspa_manager_new()
{
  AgsLadspaManager *ladspa_manager;

  ladspa_manager = (AgsLadspaManager *) g_object_new(AGS_TYPE_LADSPA_MANAGER,
						     NULL);

  return(ladspa_manager);
}
