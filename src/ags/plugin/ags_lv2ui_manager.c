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

#include <ags/plugin/ags_lv2ui_manager.h>

#include <ags/object/ags_marshal.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_lv2ui_manager_class_init(AgsLv2uiManagerClass *lv2ui_manager);
void ags_lv2ui_manager_init (AgsLv2uiManager *lv2ui_manager);
void ags_lv2ui_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2ui_manager
 * @short_description: Singleton pattern to organize LV2UI
 * @title: AgsLv2uiManager
 * @section_id:
 * @include: ags/object/ags_lv2ui_manager.h
 *
 * The #AgsLv2uiManager loads/unloads LV2UI plugins.
 */

static gpointer ags_lv2ui_manager_parent_class = NULL;

AgsLv2uiManager *ags_lv2ui_manager = NULL;
static const gchar *ags_lv2ui_default_path = "/usr/lib/lv2ui\0";

GType
ags_lv2ui_manager_get_type (void)
{
  static GType ags_type_lv2ui_manager = 0;

  if(!ags_type_lv2ui_manager){
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
						    "AgsLv2uiManager\0",
						    &ags_lv2ui_manager_info,
						    0);
  }

  return (ags_type_lv2ui_manager);
}

void
ags_lv2ui_manager_class_init(AgsLv2uiManagerClass *lv2ui_manager)
{
  GObjectClass *gobject;
  
  ags_lv2ui_manager_parent_class = g_type_class_peek_parent(lv2ui_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2ui_manager;

  gobject->finalize = ags_lv2ui_manager_finalize;
}

void
ags_lv2ui_manager_init(AgsLv2uiManager *lv2ui_manager)
{
  lv2ui_manager->lv2ui_plugin = NULL;
}

void
ags_lv2ui_manager_finalize(GObject *gobject)
{
  AgsLv2uiManager *lv2ui_manager;
  GList *lv2ui_plugin;

  lv2ui_manager = AGS_LV2UI_MANAGER(gobject);

  lv2ui_plugin = lv2ui_manager->lv2ui_plugin;

  g_list_free_full(lv2ui_plugin,
		   ags_lv2ui_plugin_free);
}

/**
 * ags_lv2ui_plugin_alloc:
 * 
 * Alloc the #AgsLv2uiPlugin-struct
 *
 * Returns: the #AgsLv2uiPlugin-struct
 *
 * Since: 0.4.3
 */
AgsLv2uiPlugin*
ags_lv2ui_plugin_alloc()
{
  AgsLv2uiPlugin *lv2ui_plugin;

  lv2ui_plugin = (AgsLv2uiPlugin *) malloc(sizeof(AgsLv2uiPlugin));

  lv2ui_plugin->flags = 0;

  lv2ui_plugin->turtle = NULL;

  lv2ui_plugin->filename = NULL;
  lv2ui_plugin->plugin_so = NULL;

  return(lv2ui_plugin);
}

/**
 * ags_lv2ui_plugin_free:
 * @lv2ui_plugin: the #AgsLv2uiPlugin-struct
 * 
 * Free the #AgsLv2uiPlugin-struct
 *
 * Since: 0.4.3
 */
void
ags_lv2ui_plugin_free(AgsLv2uiPlugin *lv2ui_plugin)
{
  if(lv2ui_plugin->plugin_so != NULL){
    dlclose(lv2ui_plugin->plugin_so);
  }

  free(lv2ui_plugin->filename);
  g_object_unref(lv2ui_plugin->turtle);
  free(lv2ui_plugin);
}

/**
 * ags_lv2ui_manager_get_filenames:
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 0.4.3
 */
gchar**
ags_lv2ui_manager_get_filenames()
{
  AgsLv2uiManager *lv2ui_manager;
  GList *lv2ui_plugin;
  gchar **filenames;
  guint length;
  guint i;

  lv2ui_manager = ags_lv2ui_manager_get_instance();
  length = g_list_length(lv2ui_manager->lv2ui_plugin);

  lv2ui_plugin = lv2ui_manager->lv2ui_plugin;
  filenames = (gchar **) malloc((length + 1) * sizeof(gchar *));

  for(i = 0; i < length; i++){
    filenames[i] = AGS_LV2UI_PLUGIN(lv2ui_plugin->data)->filename;
    lv2ui_plugin = lv2ui_plugin->next;
  }

  filenames[i] = NULL;

  return(filenames);
}

/**
 * ags_lv2ui_manager_find_lv2ui_plugin:
 * @filename: the filename of the plugin
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsLv2uiPlugin-struct
 *
 * Since: 0.4.3
 */
AgsLv2uiPlugin*
ags_lv2ui_manager_find_lv2ui_plugin(gchar *filename)
{
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2uiPlugin *lv2ui_plugin;
  GList *list;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  list = lv2ui_manager->lv2ui_plugin;

  while(list != NULL){
    lv2ui_plugin = AGS_LV2UI_PLUGIN(list->data);
    if(!g_strcmp0(lv2ui_plugin->filename,
		  filename)){
      return(lv2ui_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_lv2ui_manager_load_file:
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 0.4.3
 */
void
ags_lv2ui_manager_load_file(AgsTurtle *turtle,
			    gchar *filename)
{
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2uiPlugin *lv2ui_plugin;

  gchar *turtle_path;

  GError *error;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  if(filename == NULL){
    return;
  }

  lv2ui_manager = ags_lv2ui_manager_get_instance();
  
  /* load plugin */
  pthread_mutex_lock(&(mutex));

  lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(filename);
  g_message("loading: %s\0", filename);

  if(lv2ui_plugin == NULL){
    lv2ui_plugin = ags_lv2ui_plugin_alloc();

    /* set turtle */
    lv2ui_plugin->turtle = turtle;

    /* set filename and plugin file */
    lv2ui_plugin->filename = g_strdup(filename);
    
    lv2ui_manager->lv2ui_plugin = g_list_prepend(lv2ui_manager->lv2ui_plugin,
						 lv2ui_plugin);
  }

  pthread_mutex_unlock(&(mutex));
}

/**
 * ags_lv2ui_manager_load_default_directory:
 * 
 * Loads all available plugins.
 *
 * Since: 0.4.3
 */
void
ags_lv2ui_manager_load_default_directory()
{
  AgsLv2uiManager *lv2ui_manager;
  AgsLv2uiPlugin *lv2ui_plugin;

  GDir *dir;

  gchar *path, *plugin_path;
  gchar *str;

  GError *error;

  lv2ui_manager = ags_lv2ui_manager_get_instance();

  error = NULL;
  dir = g_dir_open(ags_lv2ui_default_path,
		   0,
		   &error);

  if(error != NULL){
    g_warning(error->message);
  }
  
  while((path = g_dir_read_name(dir)) != NULL){
    if(!g_ascii_strncasecmp(path,
			    "..\0",
			    3) ||
       !g_ascii_strncasecmp(path,
			    ".\0",
			    2)){
      continue;
    }
    
    plugin_path = g_strdup_printf("%s/%s\0",
				  ags_lv2ui_default_path,
				  path);

    if(g_file_test(plugin_path,
		   G_FILE_TEST_IS_DIR)){
      AgsTurtle *manifest, *turtle;
      
      GList *ttl_list, *binary_list;

      gchar *turtle_path, *filename;
      
      manifest = ags_turtle_new(g_strdup_printf("%s/manifest.ttl\0",
						plugin_path));
      ags_turtle_load(manifest);
  
      /* instantiate and load turtle */
      ttl_list = ags_turtle_find_xpath(manifest,
				       "//rdf-triple/rdf-verb[@do=\"rdfs:seeAlso\"]/rdf-list/rdf-value\0");

      /* load */
      if(ttl_list == NULL){
	continue;
      }
      
      while(ttl_list != NULL){
	turtle_path = xmlGetProp((xmlNode *) ttl_list->data,
				 "value\0");
	g_message(turtle_path);
	turtle = ags_turtle_new(g_strdup_printf("%s/%s\0",
						plugin_path,
						g_strndup(&(turtle_path[1]),
							  strlen(turtle_path) - 2)));
	ags_turtle_load(turtle);
	//	xmlSaveFormatFileEnc("-\0", turtle->doc, "UTF-8\0", 1);

	/* read binary from turtle */
	binary_list = ags_turtle_find_xpath(turtle,
					    "//rdf-triple/rdf-verb[@do=\"uiext:binary\"]/rdf-list/rdf-value\0");

	if(binary_list == NULL){
	  g_object_unref(turtle);
	  continue;
	}

	str = xmlGetProp(binary_list->data,
			 "value\0");
	str = g_strndup(&(str[1]),
			strlen(str) - 2);
	filename = g_strdup_printf("%s/%s\0",
				   plugin_path,
				   str);
	free(str);
	
	ags_lv2ui_manager_load_file(turtle,
				    filename);

	ttl_list = ttl_list->next;
      }

      g_object_unref(manifest);
    }
  }
}
  
/**
 * ags_lv2ui_manager_uri_index:
 * @filename: the plugin.so filename
 * @uri: the uri's name within plugin
 *
 * Retrieve the uri's index within @filename
 *
 * Returns: the index, G_MAXULONG if not found
 *
 * Since: 0.4.3
 */
uint32_t
ags_lv2ui_manager_uri_index(gchar *filename,
			    gchar *uri)
{
  AgsLv2uiPlugin *lv2ui_plugin;
    
  void *plugin_so;
  LV2UI_Descriptor* (*lv2ui_descriptor)(uint32_t index);
  LV2UI_Descriptor *plugin_descriptor;

  uint32_t uri_index;
  uint32_t i;

  if(filename == NULL ||
     uri == NULL){
    return(G_MAXULONG);
  }
  
  /* load plugin */
  lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(filename);

  if(lv2ui_plugin->plugin_so == NULL){
    plugin_so =
      lv2ui_plugin->plugin_so = dlopen(lv2ui_plugin->filename,
				       RTLD_NOW);
  }else{
    plugin_so = lv2ui_plugin->plugin_so;
  }
  
  uri_index = G_MAXULONG;

  if(plugin_so){
    lv2ui_descriptor = (LV2UI_Descriptor *) dlsym(plugin_so,
						  "lv2ui_descriptor\0");
    
    if(dlerror() == NULL && lv2ui_descriptor){
      for(i = 0; (plugin_descriptor = lv2ui_descriptor(i)) != NULL; i++){
	if(!strncmp(plugin_descriptor->URI,
		    uri,
		    strlen(uri))){
	  uri_index = i;
	  break;
	}
      }
    }
  }
  
  return(uri_index);
}

/**
 * ags_lv2ui_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLv2uiManager
 *
 * Since: 0.4.3
 */
AgsLv2uiManager*
ags_lv2ui_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_lv2ui_manager == NULL){
    ags_lv2ui_manager = ags_lv2ui_manager_new();

    pthread_mutex_unlock(&(mutex));

    ags_lv2ui_manager_load_default_directory();
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_lv2ui_manager);
}

/**
 * ags_lv2ui_manager_new:
 *
 * Creates an #AgsLv2uiManager
 *
 * Returns: a new #AgsLv2uiManager
 *
 * Since: 0.4.3
 */
AgsLv2uiManager*
ags_lv2ui_manager_new()
{
  AgsLv2uiManager *lv2ui_manager;

  lv2ui_manager = (AgsLv2uiManager *) g_object_new(AGS_TYPE_LV2UI_MANAGER,
						   NULL);

  return(lv2ui_manager);
}
