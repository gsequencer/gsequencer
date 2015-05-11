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

#include <ags/plugin/ags_lv2_manager.h>

#include <ags/object/ags_marshal.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_lv2_manager_class_init(AgsLv2ManagerClass *lv2_manager);
void ags_lv2_manager_init (AgsLv2Manager *lv2_manager);
void ags_lv2_manager_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_lv2_manager_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_lv2_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_manager
 * @short_description: Singleton pattern to organize LV2
 * @title: AgsLv2Manager
 * @section_id:
 * @include: ags/object/ags_lv2_manager.h
 *
 * The #AgsLv2Manager loads/unloads LV2 plugins.
 */

enum{
  PROP_0,
  PROP_LOCALE,
};

enum{
  ADD,
  CREATE,
  LAST_SIGNAL,
};

static gpointer ags_lv2_manager_parent_class = NULL;
static guint lv2_manager_signals[LAST_SIGNAL];

AgsLv2Manager *ags_lv2_manager = NULL;
static const gchar *ags_lv2_default_path = "/usr/lib/lv2\0";

GType
ags_lv2_manager_get_type (void)
{
  static GType ags_type_lv2_manager = 0;

  if(!ags_type_lv2_manager){
    static const GTypeInfo ags_lv2_manager_info = {
      sizeof (AgsLv2ManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Manager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_manager_init,
    };

    ags_type_lv2_manager = g_type_register_static(G_TYPE_OBJECT,
						  "AgsLv2Manager\0",
						  &ags_lv2_manager_info,
						  0);
  }

  return (ags_type_lv2_manager);
}

void
ags_lv2_manager_class_init(AgsLv2ManagerClass *lv2_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_manager_parent_class = g_type_class_peek_parent(lv2_manager);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_manager;

  gobject->set_property = ags_lv2_manager_set_property;
  gobject->get_property = ags_lv2_manager_get_property;

  gobject->finalize = ags_lv2_manager_finalize;

  /* properties */
  /**
   * AgsLv2Manager:locale:
   *
   * The assigned locale.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_string("locale\0",
				   "locale of lv2 manager\0",
				   "The locale this lv2 manager is assigned to\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOCALE,
				  param_spec);

}

void
ags_lv2_manager_init(AgsLv2Manager *lv2_manager)
{
  lv2_manager->lv2_plugin = NULL;
}

void
ags_lv2_manager_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = AGS_LV2_MANAGER(gobject);

  switch(prop_id){
  case PROP_LOCALE:
    {
      gchar *locale;

      locale = (gchar *) g_value_get_string(value);

      if(lv2_manager->locale == locale){
	return;
      }
      
      if(lv2_manager->locale != NULL){
	g_free(lv2_manager->locale);
      }

      lv2_manager->locale = g_strdup(locale);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_manager_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = AGS_LV2_MANAGER(gobject);

  switch(prop_id){
  case PROP_LOCALE:
    g_value_set_string(value, lv2_manager->locale);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_manager_finalize(GObject *gobject)
{
  AgsLv2Manager *lv2_manager;
  GList *lv2_plugin;

  lv2_manager = AGS_LV2_MANAGER(gobject);

  lv2_plugin = lv2_manager->lv2_plugin;

  g_list_free_full(lv2_plugin,
		   ags_lv2_plugin_free);
}

/**
 * ags_lv2_plugin_alloc:
 * 
 * Alloc the #AgsLv2Plugin-struct
 *
 * Returns: the #AgsLv2Plugin-struct
 *
 * Since: 0.4.3
 */
AgsLv2Plugin*
ags_lv2_plugin_alloc()
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = (AgsLv2Plugin *) malloc(sizeof(AgsLv2Plugin));

  lv2_plugin->flags = 0;

  lv2_plugin->manifest = NULL;

  lv2_plugin->filename = NULL;
  lv2_plugin->plugin_so = NULL;

  return(lv2_plugin);
}

/**
 * ags_lv2_plugin_free:
 * @lv2_plugin: the #AgsLv2Plugin-struct
 * 
 * Free the #AgsLv2Plugin-struct
 *
 * Since: 0.4.3
 */
void
ags_lv2_plugin_free(AgsLv2Plugin *lv2_plugin)
{
  if(lv2_plugin->plugin_so != NULL){
    dlclose(lv2_plugin->plugin_so);
  }

  free(lv2_plugin->filename);
  g_object_unref(lv2_plugin->manifest);
  free(lv2_plugin);
}

/**
 * ags_lv2_manager_get_filenames:
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 0.4.3
 */
gchar**
ags_lv2_manager_get_filenames()
{
  AgsLv2Manager *lv2_manager;
  GList *lv2_plugin;
  gchar **filenames;
  guint length;
  guint i;

  lv2_manager = ags_lv2_manager_get_instance();
  length = g_list_length(lv2_manager->lv2_plugin);

  lv2_plugin = lv2_manager->lv2_plugin;
  filenames = (gchar **) malloc((length + 1) * sizeof(gchar *));

  for(i = 0; i < length; i++){
    filenames[i] = AGS_LV2_PLUGIN(lv2_plugin->data)->filename;
    lv2_plugin = lv2_plugin->next;
  }

  filenames[i] = NULL;

  return(filenames);
}

/**
 * ags_lv2_manager_find_lv2_plugin:
 * @filename: the filename of the plugin
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsLv2Plugin-struct
 *
 * Since: 0.4.3
 */
AgsLv2Plugin*
ags_lv2_manager_find_lv2_plugin(gchar *filename)
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;
  GList *list;

  lv2_manager = ags_lv2_manager_get_instance();

  list = lv2_manager->lv2_plugin;

  while(list != NULL){
    lv2_plugin = AGS_LV2_PLUGIN(list->data);
    if(!g_strcmp0(lv2_plugin->filename,
		  filename)){
      return(lv2_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_lv2_manager_load_file:
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 0.4.3
 */
void
ags_lv2_manager_load_file(gchar *filename)
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GDir *dir;
  
  gchar *path;
  gchar *plugin_name;
  gchar *manifest;

  GError *error;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  lv2_manager = ags_lv2_manager_get_instance();

  error = NULL;
  dir = g_dir_open(filename,
		   0,
		   &error);

  if(error != NULL){
    g_warning(error->message);
  }

  plugin_name = NULL;
  manifest = g_strdup_printf("%s/%s/manifest.ttl\0",
			     ags_lv2_default_path,
			     filename);
  
  while((plugin_name = g_dir_read_name(dir)) != NULL){
    if(g_str_has_suffix(plugin_name,
			".so\0")){
      break;
    }
  }
  
  pthread_mutex_lock(&(mutex));

  path = g_strdup_printf("%s/%s\0",
			 filename,
			 plugin_name);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename);
  g_message("loading: %s\0", filename);

  if(lv2_plugin == NULL){
    gchar **turtle_filter;
    
    lv2_plugin = ags_lv2_plugin_alloc();

    /* load manifest */
    turtle_filter = malloc(2 * sizeof(gchar *));
    turtle_filter[0] = g_strdup(lv2_manager->locale);
    turtle_filter[1] = NULL;

    lv2_plugin->manifest = ags_turtle_new(manifest,
					  turtle_filter);
    ags_turtle_load(lv2_plugin->manifest);

    g_strfreev(turtle_filter);

    /* set filename and plugin file */
    lv2_plugin->filename = g_strdup(filename);
    
    lv2_manager->lv2_plugin = g_list_prepend(lv2_manager->lv2_plugin,
					     lv2_plugin);

    //NOTE:JK: only as needed
    //    lv2_plugin->plugin_so = dlopen(path,
    //				   RTLD_NOW);
    lv2_plugin->path = g_strdup(path);

    
    if(lv2_plugin->plugin_so){
      dlerror();
    }
  }

  pthread_mutex_unlock(&(mutex));

  g_free(path);
}

/**
 * ags_lv2_manager_load_default_directory:
 * 
 * Loads all available plugins.
 *
 * Since: 0.4.3
 */
void
ags_lv2_manager_load_default_directory()
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  GDir *dir;

  gchar *filename, *plugin_path;

  GError *error;

  lv2_manager = ags_lv2_manager_get_instance();

  error = NULL;
  dir = g_dir_open(ags_lv2_default_path,
		   0,
		   &error);

  if(error != NULL){
    g_warning(error->message);
  }

  while((filename = g_dir_read_name(dir)) != NULL){
    plugin_path = g_strdup_printf("%s/%s\0",
				  ags_lv2_default_path,
				  filename);

    if(g_file_test(plugin_path,
		   G_FILE_TEST_IS_DIR)){
      ags_lv2_manager_load_file(plugin_path);
    }
  }
}

/**
 * ags_lv2_manager_uri_index:
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
ags_lv2_manager_uri_index(gchar *filename,
			  gchar *uri)
{
  AgsLv2Plugin *lv2_plugin;
    
  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  uint32_t index;
  uint32_t i;

  if(filename == NULL ||
     uri == NULL){
    return(G_MAXULONG);
  }
  
  /* load plugin */
  ags_lv2_manager_load_file(filename);
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename);

  plugin_so =
    lv2_plugin->plugin_so = dlopen(lv2_plugin->path,
				   RTLD_NOW);
  
  index = G_MAXULONG;

  if(plugin_so){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor\0");
    
    if(dlerror() == NULL && lv2_descriptor){
      for(i = 0; (plugin_descriptor = lv2_descriptor(i)) != NULL; i++){
	if(!strncmp(plugin_descriptor->URI,
		    uri,
		    strlen(uri))){
	  index = i;
	  break;
	}
      }
    }
  }
  
  return(index);
}

/**
 * ags_lv2_manager_uri_turtle:
 * @filename: the plugin.so filename
 * @uri: the uri's name within plugin
 *
 * Retrieve the uri's turtle within @filename
 *
 * Returns: the turtle, G_MAXULONG if not found
 *
 * Since: 0.4.3
 */
AgsTurtle*
ags_lv2_manager_uri_turtle(gchar *filename,
			   gchar *uri)
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  AgsTurtle *turtle;

  GList *list;
  
  gchar *turtle_path;
  gchar *str;
  
  gchar **turtle_filter;

  
  if(filename == NULL ||
     uri == NULL){
    return(NULL);
  }

  lv2_manager = ags_lv2_manager_get_instance();
  
  /* load plugin */
  ags_lv2_manager_load_file(filename);
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(filename);
  
  /* instantiate and load turtle */
  list = ags_turtle_find_xpath(lv2_plugin->manifest,
			       g_strdup_printf("/rdf-triple[@subject='%s']/rdf-verb[@do='rdfs:seeAlso]/rdf-value'\0",
					       uri));

  if(list == NULL){
    return(NULL);
  }
  
  str = xmlGetProp((xmlNode *) list->data,
		   "value\0");
  turtle_path = g_strdup_printf("%s/%s\0",
				filename,
				str);

  turtle_filter = malloc(2 * sizeof(gchar *));
  turtle_filter[0] = g_strdup(lv2_manager->locale);
  turtle_filter[1] = NULL;
    
  turtle = ags_turtle_new(turtle_path,
			  turtle_filter);
  ags_turtle_load(turtle);

  g_strfreev(turtle_filter);
  
  return(turtle);
}

/**
 * ags_lv2_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLv2Manager
 *
 * Since: 0.4.3
 */
AgsLv2Manager*
ags_lv2_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_lv2_manager == NULL){
    ags_lv2_manager = ags_lv2_manager_new(AGS_LV2_MANAGER_DEFAULT_LOCALE);

    pthread_mutex_unlock(&(mutex));

    ags_lv2_manager_load_default_directory();
  }else{
    pthread_mutex_unlock(&(mutex));
  }

  return(ags_lv2_manager);
}

/**
 * ags_lv2_manager_new:
 *
 * Creates an #AgsLv2Manager
 *
 * Returns: a new #AgsLv2Manager
 *
 * Since: 0.4.3
 */
AgsLv2Manager*
ags_lv2_manager_new(gchar *locale)
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = (AgsLv2Manager *) g_object_new(AGS_TYPE_LV2_MANAGER,
					       "locale\0", locale,
					       NULL);

  return(lv2_manager);
}
