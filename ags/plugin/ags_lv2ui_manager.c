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

#include <ags/config.h>

void ags_lv2ui_manager_class_init(AgsLv2uiManagerClass *lv2ui_manager);
void ags_lv2ui_manager_init (AgsLv2uiManager *lv2ui_manager);
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

AgsLv2uiManager *ags_lv2ui_manager = NULL;
gchar **ags_lv2ui_default_path = NULL;

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

  if(ags_lv2ui_default_path == NULL){
    ags_lv2ui_default_path = (gchar **) malloc(3 * sizeof(gchar *));

    ags_lv2ui_default_path[0] = g_strdup("/usr/lib/lv2\0");
    ags_lv2ui_default_path[1] = g_strdup("/usr/lib64/lv2\0");
    ags_lv2ui_default_path[2] = NULL;
  }
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
 * Since: 0.7.0
 */
gchar**
ags_lv2ui_manager_get_filenames(AgsLv2uiManager *lv2ui_manager)
{
  GList *lv2ui_plugin;
  
  gchar **filenames;
  
  guint i;
  gboolean contains_filename;
  
  lv2ui_plugin = lv2ui_manager->lv2ui_plugin;
  filenames = NULL;
  
  for(i = 0; lv2ui_plugin != NULL;){
    if(filenames == NULL){
      filenames = (gchar **) malloc(2 * sizeof(gchar *));
      filenames[i] = AGS_BASE_PLUGIN(lv2ui_plugin->data)->filename;
      filenames[i + 1] = NULL;

      i++;
    }else{
#ifdef HAVE_GLIB_2_44
      contains_filename = g_strv_contains(filenames,
					  AGS_BASE_PLUGIN(lv2ui_plugin->data)->filename);
#else
      contains_filename = ags_strv_contains(filenames,
					    AGS_BASE_PLUGIN(lv2ui_plugin->data)->filename);
#endif
      
      if(!contains_filename){
	filenames = (gchar **) realloc(filenames,
				       (i + 2) * sizeof(gchar *));
	filenames[i] = AGS_BASE_PLUGIN(lv2ui_plugin->data)->filename;
	filenames[i + 1] = NULL;
	
	i++;
      }
    }
    
    lv2ui_plugin = lv2ui_plugin->next;
  }

  return(filenames);
}

/**
 * ags_lv2ui_manager_find_lv2ui_plugin:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsLv2uiPlugin-struct
 *
 * Since: 0.7.0
 */
AgsLv2uiPlugin*
ags_lv2ui_manager_find_lv2ui_plugin(AgsLv2uiManager *lv2ui_manager,
				    gchar *filename, gchar *effect)
{
  AgsLv2uiPlugin *lv2ui_plugin;
  
  GList *list;

  if(filename == NULL ||
     effect == NULL){
    return(NULL);
  }
  
  list = lv2ui_manager->lv2ui_plugin;
  
  while(list != NULL){
    lv2ui_plugin = AGS_LV2UI_PLUGIN(list->data);
    
    if(!g_ascii_strcasecmp(AGS_BASE_PLUGIN(lv2ui_plugin)->filename,
			   filename) &&
       !g_ascii_strcasecmp(AGS_BASE_PLUGIN(lv2ui_plugin)->effect,
			   effect)){
      return(lv2ui_plugin);
    }

    list = list->next;
  }

  return(NULL);
}

/**
 * ags_lv2ui_manager_load_blacklist:
 * @lv2ui_manager: the #AgsLv2uiManager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 0.7.108
 */
void
ags_lv2ui_manager_load_blacklist(AgsLv2uiManager *lv2ui_manager,
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
      lv2ui_manager->lv2ui_plugin_blacklist = g_list_prepend(lv2ui_manager->lv2ui_plugin_blacklist,
							     str);
    }
  }
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
 * Since: 0.7.0
 */
void
ags_lv2ui_manager_load_file(AgsLv2uiManager *lv2ui_manager,
			    AgsTurtle *manifest,
			    AgsTurtle *turtle,
			    gchar *lv2ui_path,
			    gchar *filename)
{
  AgsLv2uiPlugin *lv2ui_plugin;

  xmlNode *node;

  GList *gtk_uri_list;
  GList *qt4_uri_list;
  GList *qt5_uri_list;
  
  gchar *xpath;
  
  GError *error;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  uint32_t i;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  auto void ags_lv2ui_manager_load_file_ui_plugin(GList *list);

  void ags_lv2ui_manager_load_file_ui_plugin(GList *list){
    GList *uri_list;
    GList *binary_list;

    gchar *gui_filename;
    gchar *str;
    gchar *path;
    gchar *gui_path;
    gchar *xpath;
    gchar *uri;
    
    if(list == NULL){
      return;
    }

    while(list != NULL){
      /* find URI */
      xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple/rdf-subject/rdf-iri\0";    
      uri_list = ags_turtle_find_xpath_with_context_node(turtle,
							 xpath,
							 list->data);
      
      uri = NULL;
  
      if(uri_list != NULL){
	xmlNode *child;

	child = ((xmlNode *) uri_list->data)->children;

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

	      uri = pname;
	    
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

		  str = g_strdup_printf("//rdf-pname-ns[text()='%s']/following-sibling::*[self::rdf-iriref][1]\0",
					prefix);
		  prefix_node = ags_turtle_find_xpath(turtle,
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
			uri = g_strdup_printf("%s%s\0",
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

      /* load plugin */
      if(uri == NULL){
	list = list->next;

	continue;
      }

      xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':binary') + 1) = ':binary']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref[substring(text(), string-length(text()) - string-length('.so>') + 1) = '.so>']\0";
      binary_list = ags_turtle_find_xpath_with_context_node(turtle,
							    xpath,
							    list->data);

      /* load */
      while(binary_list != NULL){
	gchar *tmp;
	
	/* read filename of binary */
	str = xmlNodeGetContent((xmlNode *) binary_list->data);
	
	if(str == NULL){
	  binary_list = binary_list->next;
	  continue;
	}

	str = g_strndup(&(str[1]),
			strlen(str) - 2);

	if((tmp = strstr(filename, "/\0")) != NULL){
	  tmp = g_strndup(filename,
			  tmp - filename);
	}else{
	  binary_list = binary_list->next;
	  continue;
	}

	gui_filename = g_strdup_printf("%s/%s\0",
				       tmp,
				       str);
	free(str);

	break;
      }
      
      path = g_strdup_printf("%s/%s\0",
			     lv2ui_path,
			     filename);
      
      gui_path = g_strdup_printf("%s/%s\0",
				 lv2ui_path,
				 gui_filename);
      
      g_message("lv2ui check - %s\0", gui_path);
      
      list = list->next;
    }    
  }
  
  /* entry point */
  if(turtle == NULL ||
     filename == NULL){
    return;
  }
  
  /* load plugin */
  pthread_mutex_lock(&(mutex));

  /* check if gtk UI */
  xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple//rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':gtkui') + 1) = ':gtkui']//ancestor::*[self::rdf-triple]\0";
  gtk_uri_list = ags_turtle_find_xpath(turtle,
				       xpath);
  ags_lv2ui_manager_load_file_ui_plugin(gtk_uri_list);
  
  /* check if qt4 UI */
  xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple//rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':qt4ui') + 1) = ':qt4ui']//ancestor::*[self::rdf-triple]\0";
  qt4_uri_list = ags_turtle_find_xpath(turtle,
				       xpath);
  ags_lv2ui_manager_load_file_ui_plugin(qt4_uri_list);
  
  /* check if qt5 UI */
  xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple//rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':qt5ui') + 1) = ':qt5ui']//ancestor::*[self::rdf-triple]\0";
  qt5_uri_list = ags_turtle_find_xpath(turtle,
				       xpath);
  ags_lv2ui_manager_load_file_ui_plugin(qt5_uri_list);

  /*  */
  pthread_mutex_unlock(&(mutex));
}

/**
 * ags_lv2ui_manager_load_default_directory:
 * @lv2ui_manager: the #AgsLv2uiManager
 * 
 * Loads all available plugins.
 *
 * Since: 0.7.0
 */
void
ags_lv2ui_manager_load_default_directory(AgsLv2uiManager *lv2ui_manager)
{
  GDir *dir;

  gchar **lv2ui_path;
  gchar *path, *plugin_path;
  gchar *str;

  GError *error;

  lv2ui_path = ags_lv2ui_default_path;
  while(*lv2ui_path != NULL){
    if(!g_file_test(*lv2ui_path,
		    G_FILE_TEST_EXISTS)){
      lv2ui_path++;
      
      continue;
    }

    error = NULL;
    dir = g_dir_open(*lv2ui_path,
		     0,
		     &error);

    if(error != NULL){
      g_warning("%s\0", error->message);

      lv2ui_path++;

      continue;
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
				    *lv2ui_path,
				    path);

      if(g_file_test(plugin_path,
		     G_FILE_TEST_IS_DIR)){
	AgsTurtle *manifest, *turtle;

	xmlDoc *doc;

	FILE *out;

	xmlChar *buffer;
	int size;
      
	GList *ttl_list, *ttl_start, *binary_list;

	gchar *manifest_filename;
	gchar *turtle_path, *filename;

	gboolean turtle_loaded;

	manifest_filename = g_strdup_printf("%s/manifest.ttl\0",
					    plugin_path);

	if(!g_file_test(manifest_filename,
			G_FILE_TEST_EXISTS)){
	  continue;
	}
	
	manifest = ags_turtle_new(manifest_filename);
	ags_turtle_load(manifest,
			NULL);

	/* read binary from turtle */
	binary_list = ags_turtle_find_xpath(manifest,
					    "/rdf-turtle-doc/rdf-statement/rdf-triple//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':binary') + 1) = ':binary']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref[substring(text(), string-length(text()) - string-length('.so>') + 1) = '.so>']\0");

	/* persist XML */
	//NOTE:JK: no need for it
      
	//      xmlDocDumpFormatMemoryEnc(manifest->doc, &buffer, &size, "UTF-8\0", TRUE);

	//      out = fopen(g_strdup_printf("%s/manifest.xml\0", plugin_path), "w+\0");

	//      fwrite(buffer, size, sizeof(xmlChar), out);
	//      fflush(out);
            
	/* load */
	while(binary_list != NULL){
	  /* read filename of binary */
	  str = xmlNodeGetContent((xmlNode *) binary_list->data);

	  if(str == NULL){
	    binary_list = binary_list->next;
	    continue;
	  }
	
	  str = g_strndup(&(str[1]),
			  strlen(str) - 2);
	  filename = g_strdup_printf("%s/%s\0",
				     path,
				     str);
	  free(str);

	  /* read turtle from manifest */
	  ttl_start = 
	    ttl_list = ags_turtle_find_xpath_with_context_node(manifest,
							       "./ancestor::*[self::rdf-triple][1]//rdf-iriref[substring(text(), string-length(text()) - string-length('.ttl>') + 1) = '.ttl>']\0",
							       (xmlNode *) binary_list->data);

	  while(ttl_list != NULL){	
	    /* read filename */
	    turtle_path = xmlNodeGetContent((xmlNode *) ttl_list->data);

	    if(turtle_path == NULL){
	      ttl_list = ttl_list->next;
	      continue;
	    }
	
	    turtle_path = g_strndup(&(turtle_path[1]),
				    strlen(turtle_path) - 2);
	
	    if(!g_ascii_strncasecmp(turtle_path,
				    "http://\0",
				    7)){
	      ttl_list = ttl_list->next;
	  
	      continue;
	    }

	    /* load turtle doc */
	    if((turtle = (AgsTurtle *) ags_turtle_manager_find(ags_turtle_manager_get_instance(),
							       turtle_path)) == NULL){
	      turtle = ags_turtle_new(g_strdup_printf("%s/%s\0",
						      plugin_path,
						      turtle_path));
	      ags_turtle_load(turtle,
			      NULL);
	      ags_turtle_manager_add(ags_turtle_manager_get_instance(),
				     (GObject *) turtle);
	    }
	
	    /* load specified plugin */
	    if(!g_list_find_custom(lv2ui_manager->lv2ui_plugin_blacklist,
				   filename,
				   strcmp)){
	      ags_lv2ui_manager_load_file(lv2ui_manager,
					  manifest,
					  turtle,
					  *lv2ui_path,
					  filename);
	    }

	    /* persist XML */
	    //NOTE:JK: no need for it
	    //	xmlDocDumpFormatMemoryEnc(turtle->doc, &buffer, &size, "UTF-8\0", TRUE);

	    //	out = fopen(g_strdup_printf("%s/%s.xml\0", plugin_path, turtle_path), "w+\0");
	
	    //	fwrite(buffer, size, sizeof(xmlChar), out);
	    //	fflush(out);
	    //	xmlSaveFormatFileEnc("-\0", turtle->doc, "UTF-8\0", 1);
	
	    ttl_list = ttl_list->next;
	  }
	
	  binary_list = binary_list->next;
	}

	g_object_unref(manifest);
      }
    }

    lv2ui_path++;
  }
}

/**
 * ags_lv2ui_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLv2uiManager
 *
 * Since: 0.7.0
 */
AgsLv2uiManager*
ags_lv2ui_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&(mutex));

  if(ags_lv2ui_manager == NULL){
    ags_lv2ui_manager = ags_lv2ui_manager_new();

    pthread_mutex_unlock(&(mutex));
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
 * Since: 0.7.0
 */
AgsLv2uiManager*
ags_lv2ui_manager_new()
{
  AgsLv2uiManager *lv2ui_manager;

  lv2ui_manager = (AgsLv2uiManager *) g_object_new(AGS_TYPE_LV2UI_MANAGER,
						   NULL);

  return(lv2ui_manager);
}
