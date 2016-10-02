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

#include <ags/plugin/ags_lv2ui_manager.h>

#include <ags/lib/ags_string_util.h>

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
static const gchar *ags_lv2ui_default_path = "/usr/lib/lv2\0";

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
		   g_object_unref);
}

/**
 * ags_lv2ui_manager_get_filenames:
 * @lv2ui_manager: the #AgsLv2uiManager
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 0.4.3
 */
gchar**
ags_lv2ui_manager_get_filenames(AgsLv2uiManager *lv2ui_manager)
{
  GList *lv2ui_plugin;

  gchar **filenames;

  guint length;
  guint i;

  length = g_list_length(lv2ui_manager->lv2ui_plugin);

  lv2ui_plugin = lv2ui_manager->lv2ui_plugin;
  filenames = (gchar **) malloc((length + 1) * sizeof(gchar *));

  for(i = 0; i < length; i++){
    filenames[i] = AGS_BASE_PLUGIN(lv2ui_plugin->data)->filename;
    lv2ui_plugin = lv2ui_plugin->next;
  }

  filenames[i] = NULL;

  return(filenames);
}

/**
 * ags_lv2ui_manager_find_lv2ui_plugin:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @filename: the filename of the plugin
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsLv2uiPlugin-struct
 *
 * Since: 0.4.3
 */
AgsLv2uiPlugin*
ags_lv2ui_manager_find_lv2ui_plugin(AgsLv2uiManager *lv2ui_manager,
				    gchar *filename)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  GList *list;

  list = lv2ui_manager->lv2ui_plugin;

  while(list != NULL){
    lv2ui_plugin = AGS_LV2UI_PLUGIN(list->data);
    if(!g_strcmp0(AGS_BASE_PLUGIN(lv2ui_plugin)->filename,
		  filename)){
      return(lv2ui_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_lv2ui_manager_load_file:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @turtle: the #AgsTurtle
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 0.4.3
 */
void
ags_lv2ui_manager_load_file(AgsLv2uiManager *lv2ui_manager,
			    AgsTurtle *turtle,
			    gchar *filename)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  gchar *turtle_path;

  GError *error;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  if(filename == NULL){
    return;
  }
  
  /* load plugin */
  pthread_mutex_lock(&(mutex));

  lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
						     filename);
  g_message("loading: %s\0", filename);

  if(lv2ui_plugin == NULL){
    //FIXME:JK: do proper setup
    lv2ui_plugin = ags_lv2ui_plugin_new(turtle, filename, NULL, NULL, 0);
    
    lv2ui_manager->lv2ui_plugin = g_list_prepend(lv2ui_manager->lv2ui_plugin,
						 lv2ui_plugin);
  }

  pthread_mutex_unlock(&(mutex));
}

/**
 * ags_lv2ui_manager_load_default_directory:
 * @lv2ui_manager: the #AgsLv2uiManager
 * 
 * Loads all available plugins.
 *
 * Since: 0.4.3
 */
void
ags_lv2ui_manager_load_default_directory(AgsLv2uiManager *lv2ui_manager)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  GDir *dir;

  gchar *path, *plugin_path;
  gchar *str;

  GError *error;

  error = NULL;
  dir = g_dir_open(ags_lv2ui_default_path,
		   0,
		   &error);

  if(error != NULL){
    g_warning("%s\0", error->message);
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
      
      GList *ttl_list, *gtkui_list, *binary_list;

      gchar *manifest_filename;
      gchar *turtle_path, *filename;

      manifest_filename = g_strdup_printf("%s/manifest.ttl\0",
					  plugin_path);
      
      if(!g_file_test(manifest_filename,
		      G_FILE_TEST_EXISTS)){
	continue;
      }
	
      manifest = ags_turtle_new(manifest_filename);
      ags_turtle_load(manifest,
		      NULL);
  
      /* read turtle from manifest */
      ttl_list = ags_turtle_find_xpath(manifest,
				       "//rdf-triple//rdf-verb[//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':binary') + 1) = ':binary']]/following-sibling::rdf-object-list//rdf-iriref[substring(text(), string-length(text()) - string-length('.ttl>') + 1) = '.ttl>']\0");

      /* load */
      while(ttl_list != NULL){
	/* read filename */
	turtle_path = xmlNodeGetContent((xmlNode *) ttl_list->data);

	if(turtle_path == NULL){
	  ttl_list = ttl_list->next;
	  binary_list = binary_list->next;
	  continue;
	}
	
	turtle_path = g_strndup(&(turtle_path[1]),
				strlen(turtle_path) - 2);
	
	if(!g_ascii_strncasecmp(turtle_path,
				"http://\0",
				7)){
	  ttl_list = ttl_list->next;
	  binary_list = binary_list->next;
	  continue;
	}

	/* load turtle doc */
	turtle = ags_turtle_new(g_strdup_printf("%s/%s\0",
						plugin_path,
						turtle_path));
	g_message("%s\0", turtle->filename);
	ags_turtle_load(turtle,
			NULL);

	/* check if works with Gtk+ */
	gtkui_list = ags_turtle_find_xpath(turtle,
					   "//rdf-triple//rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':GtkUI') + 1) = ':GtkUI']\0");

	if(gtkui_list == NULL){
	  ttl_list = ttl_list->next;
	  
	  continue;
	}

	/* read binary from turtle */
	binary_list = ags_turtle_find_xpath(turtle,
					    "//rdf-triple//rdf-verb//rdf-pname-ln[text()='uiext:binary']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref\0");

	if(binary_list == NULL){
	  ttl_list = ttl_list->next;
	  
	  continue;
	}

	str = xmlNodeGetContent(binary_list->data);

	if(strlen(str) < 2){
	  ttl_list = ttl_list->next;
	  
	  continue;
	}
	
	str = g_strndup(&(str[1]),
			strlen(str) - 2);
	filename = g_strdup_printf("%s/%s\0",
				   plugin_path,
				   str);
	free(str);
	
	ags_lv2ui_manager_load_file(lv2ui_manager,
				    turtle,
				    filename);

	ttl_list = ttl_list->next;
      }

      g_object_unref(manifest);
    }
  }
}
  
/**
 * ags_lv2ui_manager_uri_index:
 * @lv2ui_manager: the #AgsLv2uiManager
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
ags_lv2ui_manager_uri_index(AgsLv2uiManager *lv2ui_manager,
			    gchar *filename,
			    gchar *uri)
{
  AgsLv2uiPlugin *lv2ui_plugin;
    
  void *plugin_so;
  LV2UI_DescriptorFunction lv2ui_descriptor;
  LV2UI_Descriptor *plugin_descriptor;

  uint32_t uri_index;
  uint32_t i;

  if(filename == NULL ||
     uri == NULL){
    return(0);
  }
  
  /* load plugin */
  lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
						     filename);

  if(lv2ui_plugin == NULL){
    return(0);
  }
    
  if(AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so == NULL){
    plugin_so =
      AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so = dlopen(AGS_BASE_PLUGIN(lv2ui_plugin)->filename,
							RTLD_NOW);
  }else{
    plugin_so = AGS_BASE_PLUGIN(lv2ui_plugin)->plugin_so;
  }
  
  uri_index = 0;

  if(plugin_so){
    lv2ui_descriptor = (LV2UI_Descriptor *) dlsym(plugin_so,
						  "lv2ui_descriptor\0");
    
    if(dlerror() == NULL && lv2ui_descriptor){
      for(i = 0; (plugin_descriptor = lv2ui_descriptor(i)) != NULL; i++){
	g_message("%s\0", plugin_descriptor->URI);
	g_message("%s\0", uri);
	
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
 * ags_lv2ui_manager_find_uri:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @filename: the plugin.so filename
 * @effect: the uri's effect
 *
 * Retrieve the uri of @filename and @effect
 *
 * Returns: the appropriate uri
 *
 * Since: 0.4.3
 */
gchar*
ags_lv2ui_manager_find_uri(AgsLv2uiManager *lv2ui_manager,
			   gchar *filename,
			   gchar *effect)
{
  AgsLv2uiPlugin *lv2ui_plugin;
  
  GList *uri_node;

  gchar *escaped_effect;
  gchar *uri;
  gchar *str;
  
  /* load plugin */
  lv2ui_plugin = ags_lv2ui_manager_find_lv2ui_plugin(lv2ui_manager,
						     filename);

  if(lv2ui_plugin == NULL){
    return(NULL);
  }

  escaped_effect = ags_string_util_escape_single_quote(effect);
  str = g_strdup_printf("//rdf-triple//rdf-subject[ancestor::*[self::rdf-triple][1]//rdf-verb[@verb='a']/following-sibling::*//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':Plugin') + 1) = ':Plugin'] and ancestor::*[self::rdf-triple][1]//rdf-verb[//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name']]/following-sibling::*//rdf-string[text()='\"%s\"']]/ancestor::*[self::rdf-triple][1]//rdf-pname-ln[text()='uiext:ui']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iri",
			escaped_effect);
  uri_node = ags_turtle_find_xpath(lv2ui_plugin->turtle,
				   str);
  free(str);
  
  uri = NULL;
  
  if(uri_node != NULL){
    xmlNode *child;

    child = ((xmlNode *) uri_node->data)->children;

    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"rdf-iriref\0",
				11)){
	  uri = xmlNodeGetContent(child);

	  if(strlen(uri) > 2){
	    uri = g_strndup(uri + 1,
			    strlen(uri) - 2);
	  }

	  break;
	}else if(!g_ascii_strncasecmp(child->name,
				      "rdf-prefixed-name\0",
				      18)){
	  xmlNode *pname_node;

	  gchar *pname;
	  
	  pname_node = child->children;
	  pname = NULL;
	  
	  while(pname_node != NULL){
	    if(pname_node->type == XML_ELEMENT_NODE){
	      if(!g_ascii_strncasecmp(pname_node->name,
				      "rdf-pname-ln\0",
				      11)){
		pname = xmlNodeGetContent(pname_node);
		
		break;
	      }
	    }

	    pname_node = pname_node->next;
	  }

	  if(pname != NULL){
	    gchar *suffix, *prefix;
	    gchar *offset;

	    offset = index(pname, ':');

	    if(offset != NULL){
	      GList *prefix_node;
	      
	      offset++;
	      suffix = g_strndup(offset,
				 strlen(pname) - (offset - pname));
	      prefix = g_strndup(pname,
				 offset - pname);

	      str = g_strdup_printf("//rdf-pname-ns[text()='%s']/following-sibling::rdf-iriref",
				    prefix);
	      prefix_node = ags_turtle_find_xpath(lv2ui_plugin->turtle,
						  str);
	      free(str);

	      if(prefix_node != NULL){
		gchar *iriref;

		iriref = xmlNodeGetContent(prefix_node->data);

		if(iriref != NULL){
		  if(strlen(iriref) > 2){
		    gchar *tmp;
		    
		    tmp = g_strndup(iriref + 1,
				    strlen(iriref) - 2);
		    uri = g_strdup_printf("%s/%s\0",
					  tmp,
					  suffix);

		    free(tmp);
		  }
		  
		  free(iriref);
		}
	      }
	    }
	  }
	  
	  break;
	}
      }

      child = child->next;
    }
  }

  return(uri);
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

    ags_lv2ui_manager_load_default_directory(ags_lv2ui_manager);
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
