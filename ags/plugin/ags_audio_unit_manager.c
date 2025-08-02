/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/plugin/ags_audio_unit_manager.h>

#include <ags/plugin/ags_base_plugin.h>

#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>
#include <strings.h>

#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>

#include <ags/config.h>

void ags_audio_unit_manager_class_init(AgsAudioUnitManagerClass *audio_unit_manager);
void ags_audio_unit_manager_init (AgsAudioUnitManager *audio_unit_manager);
void ags_audio_unit_manager_dispose(GObject *gobject);
void ags_audio_unit_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_audio_unit_manager
 * @short_description: Singleton pattern to organize AUDIO_UNIT
 * @title: AgsAudioUnitManager
 * @section_id:
 * @include: ags/plugin/ags_audio_unit_manager.h
 *
 * The #AgsAudioUnitManager loads/unloads AUDIO_UNIT plugins.
 */

static gpointer ags_audio_unit_manager_parent_class = NULL;

AgsAudioUnitManager *ags_audio_unit_manager = NULL;
gchar **ags_audio_unit_default_path = NULL;

GType
ags_audio_unit_manager_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_manager = 0;

    static const GTypeInfo ags_audio_unit_manager_info = {
      sizeof (AgsAudioUnitManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioUnitManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_manager_init,
    };

    ags_type_audio_unit_manager = g_type_register_static(G_TYPE_OBJECT,
							 "AgsAudioUnitManager",
							 &ags_audio_unit_manager_info,
							 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_manager);
  }

  return(g_define_type_id__static);
}

void
ags_audio_unit_manager_class_init(AgsAudioUnitManagerClass *audio_unit_manager)
{
  GObjectClass *gobject;

  ags_audio_unit_manager_parent_class = g_type_class_peek_parent(audio_unit_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_manager;

  gobject->dispose = ags_audio_unit_manager_dispose;
  gobject->finalize = ags_audio_unit_manager_finalize;
}

void
ags_audio_unit_manager_init(AgsAudioUnitManager *audio_unit_manager)
{
  /* audio_unit manager mutex */
  g_rec_mutex_init(&(audio_unit_manager->obj_mutex));

  /* Audio Unit plugin and path */
  audio_unit_manager->audio_unit_plugin_blacklist = NULL;
  audio_unit_manager->audio_unit_plugin = NULL;
}

void
ags_audio_unit_manager_dispose(GObject *gobject)
{
  AgsAudioUnitManager *audio_unit_manager;

  audio_unit_manager = AGS_AUDIO_UNIT_MANAGER(gobject);

  if(audio_unit_manager->audio_unit_plugin != NULL){
    g_list_free_full(audio_unit_manager->audio_unit_plugin,
		     (GDestroyNotify) g_object_unref);

    audio_unit_manager->audio_unit_plugin = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_manager_parent_class)->dispose(gobject);
}

void
ags_audio_unit_manager_finalize(GObject *gobject)
{
  AgsAudioUnitManager *audio_unit_manager;

  GList *audio_unit_plugin;

  audio_unit_manager = AGS_AUDIO_UNIT_MANAGER(gobject);

  audio_unit_plugin = audio_unit_manager->audio_unit_plugin;

  g_list_free_full(audio_unit_plugin,
		   (GDestroyNotify) g_object_unref);

  if(audio_unit_manager == ags_audio_unit_manager){
    ags_audio_unit_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_manager_parent_class)->finalize(gobject);
}

/**
 * ags_audio_unit_manager_get_default_path:
 * 
 * Get audio_unit manager default plugin path.
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer none): the plugin default search path as a string vector
 * 
 * Since: 8.1.2
 */
gchar**
ags_audio_unit_manager_get_default_path()
{
  return(ags_audio_unit_default_path);
}

/**
 * ags_audio_unit_manager_set_default_path:
 * @default_path: (element-type utf8) (array zero-terminated=1) (transfer full): the string vector array to use as default path
 * 
 * Set audio_unit manager default plugin path.
 * 
 * Since: 8.1.2
 */
void
ags_audio_unit_manager_set_default_path(gchar** default_path)
{
  ags_audio_unit_default_path = default_path;
}

/**
 * ags_audio_unit_manager_get_filenames:
 * @audio_unit_manager: the #AgsAudioUnitManager
 * 
 * Retrieve all filenames
 *
 * Returns: (element-type utf8) (array zero-terminated=1) (transfer full): a %NULL-terminated array of filenames
 *
 * Since: 8.1.2
 */
gchar**
ags_audio_unit_manager_get_filenames(AgsAudioUnitManager *audio_unit_manager)
{
  GList *start_audio_unit_plugin, *audio_unit_plugin;

  gchar **filenames;

  guint i;
  gboolean contains_filename;

  GRecMutex *audio_unit_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_AUDIO_UNIT_MANAGER(audio_unit_manager)){
    return(NULL);
  }
  
  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  /* collect */
  g_rec_mutex_lock(audio_unit_manager_mutex);

  audio_unit_plugin = 
    start_audio_unit_plugin = g_list_copy(audio_unit_manager->audio_unit_plugin);

  g_rec_mutex_unlock(audio_unit_manager_mutex);

  filenames = NULL;
  
  for(i = 0; audio_unit_plugin != NULL;){
    gchar *filename;
    
    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin->data);

    /* duplicate filename */
    g_rec_mutex_lock(base_plugin_mutex);

    filename = g_strdup(AGS_BASE_PLUGIN(audio_unit_plugin->data)->filename);

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
      contains_filename = g_strv_contains((const gchar * const *) filenames,
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
    
    audio_unit_plugin = audio_unit_plugin->next;
  }

  g_list_free(start_audio_unit_plugin);
  
  return(filenames);
}

/**
 * ags_audio_unit_manager_find_audio_unit_plugin:
 * @audio_unit_manager: the #AgsAudioUnitManager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: (transfer none): the matching #AgsAudioUnitPlugin
 *
 * Since: 8.1.2
 */
AgsAudioUnitPlugin*
ags_audio_unit_manager_find_audio_unit_plugin(AgsAudioUnitManager *audio_unit_manager,
					      gchar *filename, gchar *effect)
{
  AgsAudioUnitPlugin *audio_unit_plugin;
  
  GList *start_list, *list;

  gboolean success;  

  GRecMutex *audio_unit_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_AUDIO_UNIT_MANAGER(audio_unit_manager)){
    return(NULL);
  }
  
  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  /* collect */
  g_rec_mutex_lock(audio_unit_manager_mutex);

  list = 
    start_list = g_list_copy(audio_unit_manager->audio_unit_plugin);

  g_rec_mutex_unlock(audio_unit_manager_mutex);

  success = FALSE;
  
  while(list != NULL){
    audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(list->data);

    /* get base plugin mutex */
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

    /* check filename and effect */
    g_rec_mutex_lock(base_plugin_mutex);

    success = ((filename == NULL ||
		!g_strcmp0(AGS_BASE_PLUGIN(audio_unit_plugin)->filename,
			   filename)) &&
		!g_strcmp0(AGS_BASE_PLUGIN(audio_unit_plugin)->effect,
			  effect)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(base_plugin_mutex);
    
    if(success){
      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  if(!success){
    audio_unit_plugin = NULL;
  }

  return(audio_unit_plugin);
}

/**
 * ags_audio_unit_manager_find_audio_unit_plugin_with_fallback:
 * @audio_unit_manager: the #AgsAudioUnitManager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: (transfer none): the matching #AgsAudioUnitPlugin
 *
 * Since: 8.1.2
 */
AgsAudioUnitPlugin*
ags_audio_unit_manager_find_audio_unit_plugin_with_fallback(AgsAudioUnitManager *audio_unit_manager,
							    gchar *filename, gchar *effect)
{
  AgsAudioUnitPlugin *audio_unit_plugin;
  
  GList *start_list, *list;
  
  gchar *filename_suffix;
  
  gboolean success;  

  GRecMutex *audio_unit_manager_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_AUDIO_UNIT_MANAGER(audio_unit_manager) ||
     filename == NULL ||
     strlen(filename) <= 0 ||
     strrchr(filename, G_DIR_SEPARATOR) == NULL ||
     effect == NULL ||
     strlen(effect) <= 0){
    return(NULL);
  }
  
  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(audio_unit_manager,
								    filename, effect);

  if(audio_unit_plugin == NULL){
    filename_suffix = strrchr(filename, G_DIR_SEPARATOR);
    
    /* collect */
    g_rec_mutex_lock(audio_unit_manager_mutex);

    list = 
      start_list = g_list_copy(audio_unit_manager->audio_unit_plugin);

    g_rec_mutex_unlock(audio_unit_manager_mutex);

    success = FALSE;
  
    while(list != NULL){
      audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(list->data);

      /* get base plugin mutex */
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

      /* check filename and effect */
      g_rec_mutex_lock(base_plugin_mutex);

      success = ((filename == NULL ||
		  g_str_has_suffix(AGS_BASE_PLUGIN(audio_unit_plugin)->filename,
				   filename_suffix)) &&
		 !g_strcmp0(AGS_BASE_PLUGIN(audio_unit_plugin)->effect,
			    effect)) ? TRUE: FALSE;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    
      if(success){
	break;
      }

      list = list->next;
    }

    g_list_free(start_list);

    if(!success){
      audio_unit_plugin = NULL;
    }
  }
  
  return(audio_unit_plugin);
}

/**
 * ags_audio_unit_manager_load_blacklist:
 * @audio_unit_manager: the #AgsAudioUnitManager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 8.1.2
 */
void
ags_audio_unit_manager_load_blacklist(AgsAudioUnitManager *audio_unit_manager,
				      gchar *blacklist_filename)
{
  GRecMutex *audio_unit_manager_mutex;

  if(!AGS_AUDIO_UNIT_MANAGER(audio_unit_manager) ||
     blacklist_filename == NULL){
    return;
  } 
  
  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  /* fill in */
  g_rec_mutex_lock(audio_unit_manager_mutex);

  if(g_file_test(blacklist_filename,
		 (G_FILE_TEST_EXISTS |
		  G_FILE_TEST_IS_REGULAR))){
    FILE *file;

    gchar *str;
    
    file = fopen(blacklist_filename,
		 "r");

#ifndef AGS_W32API    
    while(getline(&str, NULL, file) != -1){
      audio_unit_manager->audio_unit_plugin_blacklist = g_list_prepend(audio_unit_manager->audio_unit_plugin_blacklist,
								       str);
    }
#endif
  }

  g_rec_mutex_unlock(audio_unit_manager_mutex);
} 

/**
 * ags_audio_unit_manager_load_component:
 * @audio_unit_manager: the #AgsAudioUnitManager
 * @component: the Audio Unit component
 *
 * Load @filename specified plugin.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_manager_load_component(AgsAudioUnitManager *audio_unit_manager,
				      gpointer component)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  gchar *str;
  gchar *plugin_name;
  
  GRecMutex *audio_unit_manager_mutex;

  if(!AGS_IS_AUDIO_UNIT_MANAGER(audio_unit_manager) ||
     component == NULL){
    return;
  }
  
  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);

  /* load */
  g_rec_mutex_lock(audio_unit_manager_mutex);

  str = [((AVAudioUnitComponent *) component)->name UTF8String];
  
  plugin_name = g_strdup(str);
  
  g_message("ags_audio_unit_manager.c loading - %s", plugin_name);

  audio_unit_plugin = ags_audio_unit_plugin_new(NULL, plugin_name, 0);

  audio_unit_plugin->component = component;
  
  audio_unit_manager->audio_unit_plugin = g_list_prepend(audio_unit_manager->audio_unit_plugin,
							 audio_unit_plugin);
  
  g_rec_mutex_unlock(audio_unit_manager_mutex);
}

/**
 * ags_audio_unit_manager_load_shared:
 * @audio_unit_manager: the #AgsAudioUnitManager
 * 
 * Loads all available plugins.
 *
 * Since: 8.1.2
 */
void
ags_audio_unit_manager_load_shared(AgsAudioUnitManager *audio_unit_manager)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  NSObject *audio_unit_component_manager;

  NSArray<AVAudioUnitComponent *> *component_arr;

  AudioComponentDescription description;

  guint i, i_stop;
  
  if(!AGS_AUDIO_UNIT_MANAGER(audio_unit_manager)){
    return;
  }

  audio_unit_component_manager = [(AVAudioUnitComponentManager) sharedAudioUnitComponentManager];

  description = (AudioComponentDescription) {0,};

  description.componentType = kAudioUnitType_Effect;

  component_arr = [componentsMatchingDescription:(AudioComponentDescription) description];

  i_stop = [component_arr:count];
  
  for(i = 0; i < i_stop; i++){
    ags_audio_unit_manager_load_component(audio_unit_manager,
					  (gpointer) component_arr[i]);
  }
}

/**
 * ags_audio_unit_manager_get_audio_unit_plugin:
 * @audio_unit_manager: the #AgsAudioUnitManager
 *
 * Get audio_unit plugin.
 *
 * Returns: (transfer full): the #GList-struct containing #AgsAudioUnitPlugin
 *
 * Since: 8.1.2
 */
GList*
ags_audio_unit_manager_get_audio_unit_plugin(AgsAudioUnitManager *audio_unit_manager)
{
  GList *audio_unit_plugin;
  
  GRecMutex *audio_unit_manager_mutex;

  if(!AGS_IS_AUDIO_UNIT_MANAGER(audio_unit_manager)){
    return(NULL);
  }
  
  /* get audio_unit manager mutex */
  audio_unit_manager_mutex = AGS_AUDIO_UNIT_MANAGER_GET_OBJ_MUTEX(audio_unit_manager);
  
  g_rec_mutex_lock(audio_unit_manager_mutex);

  audio_unit_plugin = g_list_copy_deep(audio_unit_manager->audio_unit_plugin,
				       (GCopyFunc) g_object_ref,
				       NULL);
  
  g_rec_mutex_unlock(audio_unit_manager_mutex);

  return(audio_unit_plugin);
}

/**
 * ags_audio_unit_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsAudioUnitManager
 *
 * Since: 8.1.2
 */
AgsAudioUnitManager*
ags_audio_unit_manager_get_instance()
{
  static GMutex mutex;

  g_mutex_lock(&(mutex));

  if(ags_audio_unit_manager == NULL){
    ags_audio_unit_manager = ags_audio_unit_manager_new();
  }

  g_mutex_unlock(&(mutex));

  return(ags_audio_unit_manager);
}

/**
 * ags_audio_unit_manager_new:
 *
 * Create a new instance of #AgsAudioUnitManager
 *
 * Returns: the new #AgsAudioUnitManager
 *
 * Since: 8.1.2
 */
AgsAudioUnitManager*
ags_audio_unit_manager_new()
{
  AgsAudioUnitManager *audio_unit_manager;

  audio_unit_manager = (AgsAudioUnitManager *) g_object_new(AGS_TYPE_AUDIO_UNIT_MANAGER,
							    NULL);

  return(audio_unit_manager);
}
