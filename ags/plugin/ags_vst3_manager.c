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

#include <ags/plugin/ags_vst3_manager.h>

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

#include <ags/config.h>

void ags_vst3_manager_class_init(AgsVst3ManagerClass *vst3_manager);
void ags_vst3_manager_init (AgsVst3Manager *vst3_manager);
void ags_vst3_manager_dispose(GObject *gobject);
void ags_vst3_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_vst3_manager
 * @short_description: Singleton pattern to organize VST3
 * @title: AgsVst3Manager
 * @section_id:
 * @include: ags/plugin/ags_vst3_manager.h
 *
 * The #AgsVst3Manager loads/unloads VST3 plugins.
 */

static gpointer ags_vst3_manager_parent_class = NULL;

AgsVst3Manager *ags_vst3_manager = NULL;
gchar **ags_vst3_default_path = NULL;

GType
ags_vst3_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_vst3_manager = 0;

    static const GTypeInfo ags_vst3_manager_info = {
      sizeof (AgsVst3ManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_vst3_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVst3Manager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_vst3_manager_init,
    };

    ags_type_vst3_manager = g_type_register_static(G_TYPE_OBJECT,
						   "AgsVst3Manager",
						   &ags_vst3_manager_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_vst3_manager);
  }

  return g_define_type_id__volatile;
}

void
ags_vst3_manager_class_init(AgsVst3ManagerClass *vst3_manager)
{
  GObjectClass *gobject;

  ags_vst3_manager_parent_class = g_type_class_peek_parent(vst3_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) vst3_manager;

  gobject->dispose = ags_vst3_manager_dispose;
  gobject->finalize = ags_vst3_manager_finalize;
}

void
ags_vst3_manager_init(AgsVst3Manager *vst3_manager)
{
  /* vst3 manager mutex */
  g_rec_mutex_init(&(vst3_manager->obj_mutex));

  /* vst3 plugin and path */
  vst3_manager->vst3_plugin = NULL;

  if(ags_vst3_default_path == NULL){
    gchar *vst3_env;

    if((vst3_env = getenv("VST3_PATH")) != NULL){
      gchar *iter, *next;
      guint i;
      
      ags_vst3_default_path = (gchar **) malloc(sizeof(gchar *));

      iter = vst3_env;
      i = 0;
      
      while((next = strchr(iter, G_SEARCHPATH_SEPARATOR)) != NULL){
	ags_vst3_default_path = (gchar **) realloc(ags_vst3_default_path,
						   (i + 2) * sizeof(gchar *));
	ags_vst3_default_path[i] = g_strndup(iter,
					     next - iter);

	iter = next + 1;
	i++;
      }

      if(*iter != '\0'){
	ags_vst3_default_path = (gchar **) realloc(ags_vst3_default_path,
						   (i + 2) * sizeof(gchar *));
	ags_vst3_default_path[i] = g_strdup(iter);

	i++;	
      }

      ags_vst3_default_path[i] = NULL;
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

      ags_vst3_default_path = (gchar **) malloc(2 * sizeof(gchar *));

      path = g_strdup_printf("%s\\vst3",
			     g_get_current_dir());
      
      if(g_file_test(path,
		     G_FILE_TEST_IS_DIR)){
	ags_vst3_default_path[i++] = path;
      }else{
	g_free(path);
	
	if(g_path_is_absolute(app_dir)){
	  ags_vst3_default_path[i++] = g_strdup_printf("%s\\vst3",
						       app_dir);
	}else{
	  ags_vst3_default_path[i++] = g_strdup_printf("%s\\%s\\vst3",
						       g_get_current_dir(),
						       app_dir);
	}
      }
      
      ags_vst3_default_path[i++] = NULL;

      g_free(app_dir);
#else
      gchar *home_dir;
      guint i;

#ifdef AGS_MAC_BUNDLE
      if((home_dir = getenv("HOME")) != NULL){
	ags_vst3_default_path = (gchar **) malloc(7 * sizeof(gchar *));
      }else{
	ags_vst3_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }
#else
      if((home_dir = getenv("HOME")) != NULL){
	ags_vst3_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }else{
	ags_vst3_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }
#endif
      
      i = 0;

#ifdef AGS_MAC_BUNDLE
      ags_vst3_default_path[i++] = g_strdup_printf("%s/vst3",
						   getenv("GSEQUENCER_PLUGIN_DIR"));
#endif
      
      ags_vst3_default_path[i++] = g_strdup("/usr/lib64/vst3");
      ags_vst3_default_path[i++] = g_strdup("/usr/local/lib64/vst3");
      ags_vst3_default_path[i++] = g_strdup("/usr/lib/vst3");
      ags_vst3_default_path[i++] = g_strdup("/usr/local/lib/vst3");

      if(home_dir != NULL){
	ags_vst3_default_path[i++] = g_strdup_printf("%s/.vst3",
						     home_dir);
      }
    
      ags_vst3_default_path[i++] = NULL;
#endif
    }
  }
}

void
ags_vst3_manager_dispose(GObject *gobject)
{
  AgsVst3Manager *vst3_manager;

  vst3_manager = AGS_VST3_MANAGER(gobject);

  if(vst3_manager->vst3_plugin != NULL){
    g_list_free_full(vst3_manager->vst3_plugin,
		     (GDestroyNotify) g_object_unref);

    vst3_manager->vst3_plugin = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_vst3_manager_parent_class)->dispose(gobject);
}

void
ags_vst3_manager_finalize(GObject *gobject)
{
  AgsVst3Manager *vst3_manager;

  GList *vst3_plugin;

  vst3_manager = AGS_VST3_MANAGER(gobject);

  vst3_plugin = vst3_manager->vst3_plugin;

  g_list_free_full(vst3_plugin,
		   (GDestroyNotify) g_object_unref);

  if(vst3_manager == ags_vst3_manager){
    ags_vst3_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_vst3_manager_parent_class)->finalize(gobject);
}

/**
 * ags_vst3_manager_get_default_path:
 * 
 * Get vst3 manager default plugin path.
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer none): the plugin default search path as a string vector
 * 
 * Since: 3.10.2
 */
gchar**
ags_vst3_manager_get_default_path()
{
  return(ags_vst3_default_path);
}

/**
 * ags_vst3_manager_set_default_path:
 * @default_path: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector array to use as default path
 * 
 * Set vst3 manager default plugin path.
 * 
 * Since: 3.10.2
 */
void
ags_vst3_manager_set_default_path(gchar** default_path)
{
  ags_vst3_default_path = default_path;
}

/**
 * ags_vst3_manager_get_filenames:
 * @vst3_manager: the #AgsVst3Manager
 * 
 * Retrieve all filenames
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): a %NULL-terminated array of filenames
 *
 * Since: 3.10.2
 */
gchar**
ags_vst3_manager_get_filenames(AgsVst3Manager *vst3_manager)
{
  GList *start_vst3_plugin, *vst3_plugin;

  gchar **filenames;

  guint i;
  gboolean contains_filename;

  GRecMutex *vst3_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_VST3_MANAGER(vst3_manager)){
    return(NULL);
  }
  
  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* collect */
  g_rec_mutex_lock(vst3_manager_mutex);

  vst3_plugin = 
    start_vst3_plugin = g_list_copy(vst3_manager->vst3_plugin);

  g_rec_mutex_unlock(vst3_manager_mutex);

  filenames = NULL;
  
  for(i = 0; vst3_plugin != NULL;){
    gchar *filename;
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin->data);

    /* duplicate filename */
    g_rec_mutex_lock(base_plugin_mutex);

    filename = g_strdup(AGS_BASE_PLUGIN(vst3_plugin->data)->filename);

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
					  filename);
#else
      contains_filename = ags_strv_contains(filenames,
					    filename);
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
    
    vst3_plugin = vst3_plugin->next;
  }

  g_list_free(start_vst3_plugin);
  
  return(filenames);
}

/**
 * ags_vst3_manager_find_vst3_plugin:
 * @vst3_manager: the #AgsVst3Manager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: (transfer none): the matching #AgsVst3Plugin
 *
 * Since: 3.10.2
 */
AgsVst3Plugin*
ags_vst3_manager_find_vst3_plugin(AgsVst3Manager *vst3_manager,
				  gchar *filename, gchar *effect)
{
  AgsVst3Plugin *vst3_plugin;
  
  GList *start_list, *list;

  gboolean success;  

  GRecMutex *vst3_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_VST3_MANAGER(vst3_manager)){
    return(NULL);
  }
  
  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* collect */
  g_rec_mutex_lock(vst3_manager_mutex);

  list = 
    start_list = g_list_copy(vst3_manager->vst3_plugin);

  g_rec_mutex_unlock(vst3_manager_mutex);

  success = FALSE;
  
  while(list != NULL){
    vst3_plugin = AGS_VST3_PLUGIN(list->data);

    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

    /* check filename and effect */
    g_rec_mutex_lock(base_plugin_mutex);

    success = (!g_strcmp0(AGS_BASE_PLUGIN(vst3_plugin)->filename,
			  filename) &&
	       !g_strcmp0(AGS_BASE_PLUGIN(vst3_plugin)->effect,
			  effect)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(base_plugin_mutex);
    
    if(success){
      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  if(!success){
    vst3_plugin = NULL;
  }

  return(vst3_plugin);
}

/**
 * ags_vst3_manager_find_vst3_plugin_with_fallback:
 * @vst3_manager: the #AgsVst3Manager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: (transfer none): the matching #AgsVst3Plugin
 *
 * Since: 3.5.10
 */
AgsVst3Plugin*
ags_vst3_manager_find_vst3_plugin_with_fallback(AgsVst3Manager *vst3_manager,
						gchar *filename, gchar *effect)
{
  AgsVst3Plugin *vst3_plugin;
  
  GList *start_list, *list;
  
  gchar *filename_suffix;
  
  gboolean success;  

  GRecMutex *vst3_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_VST3_MANAGER(vst3_manager) ||
     filename == NULL ||
     effect == NULL){
    return(NULL);
  }
  
  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  vst3_plugin = ags_vst3_manager_find_vst3_plugin(vst3_manager,
						  filename, effect);

  if(vst3_plugin == NULL){
    filename_suffix = strrchr(filename, "/");
    
    /* collect */
    g_rec_mutex_lock(vst3_manager_mutex);

    list = 
      start_list = g_list_copy(vst3_manager->vst3_plugin);

    g_rec_mutex_unlock(vst3_manager_mutex);

    success = FALSE;
  
    while(list != NULL){
      vst3_plugin = AGS_VST3_PLUGIN(list->data);

      /* get base plugin mutex */
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);

      /* check filename and effect */
      g_rec_mutex_lock(base_plugin_mutex);

      success = (g_str_has_suffix(AGS_BASE_PLUGIN(vst3_plugin)->filename,
				  filename_suffix) &&
		 !g_strcmp0(AGS_BASE_PLUGIN(vst3_plugin)->effect,
			    effect)) ? TRUE: FALSE;
    
      g_rec_mutex_unlock(base_plugin_mutex);
    
      if(success){
	break;
      }

      list = list->next;
    }

    g_list_free(start_list);

    if(!success){
      vst3_plugin = NULL;
    }
  }
  
  return(vst3_plugin);
}

/**
 * ags_vst3_manager_load_blacklist:
 * @vst3_manager: the #AgsVst3Manager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 3.10.2
 */
void
ags_vst3_manager_load_blacklist(AgsVst3Manager *vst3_manager,
				gchar *blacklist_filename)
{
  GRecMutex *vst3_manager_mutex;

  if(!AGS_VST3_MANAGER(vst3_manager) ||
     blacklist_filename == NULL){
    return;
  } 
  
  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* fill in */
  g_rec_mutex_lock(vst3_manager_mutex);

  if(g_file_test(blacklist_filename,
		 (G_FILE_TEST_EXISTS |
		  G_FILE_TEST_IS_REGULAR))){
    FILE *file;

    gchar *str;
    
    file = fopen(blacklist_filename,
		 "r");

#ifndef AGS_W32API    
    while(getline(&str, NULL, file) != -1){
      vst3_manager->vst3_plugin_blacklist = g_list_prepend(vst3_manager->vst3_plugin_blacklist,
							   str);
    }
#endif
  }

  g_rec_mutex_unlock(vst3_manager_mutex);
} 

/**
 * ags_vst3_manager_load_file:
 * @vst3_manager: the #AgsVst3Manager
 * @vst3_path: the vst3 path
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 3.10.2
 */
void
ags_vst3_manager_load_file(AgsVst3Manager *vst3_manager,
			   gchar *vst3_path,
			   gchar *filename)
{
  AgsVst3Plugin *vst3_plugin;

  gchar *path;
  
  GRecMutex *vst3_manager_mutex;

  if(!AGS_IS_VST3_MANAGER(vst3_manager) ||
     vst3_path == NULL ||
     filename == NULL){
    return;
  }
  
  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);

  /* load */
  g_rec_mutex_lock(vst3_manager_mutex);

  path = g_strdup_printf("%s%c%s",
			 vst3_path,
			 G_DIR_SEPARATOR,
			 filename);
  
  g_message("ags_vst3_manager.c loading - %s", path);

  //TODO:JK: implement me
}

/**
 * ags_vst3_manager_load_default_directory:
 * @vst3_manager: the #AgsVst3Manager
 * 
 * Loads all available plugins.
 *
 * Since: 3.10.2
 */
void
ags_vst3_manager_load_default_directory(AgsVst3Manager *vst3_manager)
{
  AgsVst3Plugin *vst3_plugin;

  GDir *dir;

  gchar **vst3_path;
  gchar *filename;

  GError *error;

  if(!AGS_VST3_MANAGER(vst3_manager)){
    return;
  }

  vst3_path = ags_vst3_default_path;
  
  while(*vst3_path != NULL){
    if(!g_file_test(*vst3_path,
		    G_FILE_TEST_EXISTS)){
      vst3_path++;
      
      continue;
    }

    error = NULL;
    dir = g_dir_open(*vst3_path,
		     0,
		     &error);

    if(error != NULL){
      g_warning("%s", error->message);

      vst3_path++;

      g_error_free(error);
      
      continue;
    }

    while((filename = g_dir_read_name(dir)) != NULL){
      if(g_str_has_suffix(filename,
			  AGS_LIBRARY_SUFFIX) &&
	 !g_list_find_custom(vst3_manager->vst3_plugin_blacklist,
			     filename,
			     strcmp)){
	ags_vst3_manager_load_file(vst3_manager,
				   *vst3_path,
				   filename);
      }
    }
    
    vst3_path++;
  }
}

/**
 * ags_vst3_manager_get_vst3_plugin:
 * @vst3_manager: the #AgsVst3Manager
 *
 * Get vst3 plugin.
 *
 * Returns: (transfer full): the #GList-struct containing #AgsVst3Plugin
 *
 * Since: 3.5.15
 */
GList*
ags_vst3_manager_get_vst3_plugin(AgsVst3Manager *vst3_manager)
{
  GList *vst3_plugin;
  
  GRecMutex *vst3_manager_mutex;

  if(!AGS_IS_VST3_MANAGER(vst3_manager)){
    return(NULL);
  }
  
  /* get vst3 manager mutex */
  vst3_manager_mutex = AGS_VST3_MANAGER_GET_OBJ_MUTEX(vst3_manager);
  
  g_rec_mutex_lock(vst3_manager_mutex);

  vst3_plugin = g_list_copy_deep(vst3_manager->vst3_plugin,
				 (GCopyFunc) g_object_ref,
				 NULL);
  
  g_rec_mutex_unlock(vst3_manager_mutex);

  return(vst3_plugin);
}

/**
 * ags_vst3_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsVst3Manager
 *
 * Since: 3.10.2
 */
AgsVst3Manager*
ags_vst3_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&(mutex));

  if(ags_vst3_manager == NULL){
    ags_vst3_manager = ags_vst3_manager_new();
  }

  g_mutex_unlock(&(mutex));

  return(ags_vst3_manager);
}

/**
 * ags_vst3_manager_new:
 *
 * Create a new instance of #AgsVst3Manager
 *
 * Returns: the new #AgsVst3Manager
 *
 * Since: 3.10.2
 */
AgsVst3Manager*
ags_vst3_manager_new()
{
  AgsVst3Manager *vst3_manager;

  vst3_manager = (AgsVst3Manager *) g_object_new(AGS_TYPE_VST3_MANAGER,
						 NULL);

  return(vst3_manager);
}
