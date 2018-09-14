/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/plugin/ags_lv2_manager.h>

#include <ags/libags.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <string.h>

#include <ags/config.h>

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
void ags_lv2_manager_dispose(GObject *gobject);
void ags_lv2_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_manager
 * @short_description: Singleton pattern to organize LV2
 * @title: AgsLv2Manager
 * @section_id:
 * @include: ags/plugin/ags_lv2_manager.h
 *
 * The #AgsLv2Manager loads/unloads LV2 plugins.
 */

enum{
  PROP_0,
  PROP_LOCALE,
};

static gpointer ags_lv2_manager_parent_class = NULL;

static pthread_mutex_t ags_lv2_manager_class_mutex = PTHREAD_MUTEX_INITIALIZER;

AgsLv2Manager *ags_lv2_manager = NULL;
gchar **ags_lv2_default_path = NULL;

GType
ags_lv2_manager_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_manager = 0;

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
						  "AgsLv2Manager",
						  &ags_lv2_manager_info,
						  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_manager);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_lv2_manager_dispose;
  gobject->finalize = ags_lv2_manager_finalize;

  /* properties */
  /**
   * AgsLv2Manager:locale:
   *
   * The assigned locale.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("locale",
				   "locale of lv2 manager",
				   "The locale this lv2 manager is assigned to",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOCALE,
				  param_spec);
}

void
ags_lv2_manager_init(AgsLv2Manager *lv2_manager)
{  
  /* lv2 manager mutex */
  lv2_manager->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(lv2_manager->obj_mutexattr);
  pthread_mutexattr_settype(lv2_manager->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(lv2_manager->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  lv2_manager->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(lv2_manager->obj_mutex,
		     lv2_manager->obj_mutexattr);

  /* initialize lv2 plugin blacklist */
  lv2_manager->lv2_plugin_blacklist = NULL;

  /* initialize lv2 plugin GList */
  lv2_manager->lv2_plugin = NULL;

  /* current plugin node */
  lv2_manager->current_plugin_node = NULL;

  /* initiliaze ags_lv2_default_path string vector */
  if(ags_lv2_default_path == NULL){
    gchar *lv2_env;

    if((lv2_env = getenv("LV2_PATH")) != NULL){
      gchar *iter, *next;
      guint i;
      
      ags_lv2_default_path = (gchar **) malloc(sizeof(gchar *));

      iter = lv2_env;
      i = 0;
      
      while((next = index(iter, ':')) != NULL){
	ags_lv2_default_path = (gchar **) realloc(ags_lv2_default_path,
						  (i + 2) * sizeof(gchar *));
	ags_lv2_default_path[i] = g_strndup(iter,
					    next - iter);

	iter = next + 1;
	i++;
      }

      if(*iter != '\0'){
	ags_lv2_default_path = (gchar **) realloc(ags_lv2_default_path,
						  (i + 2) * sizeof(gchar *));
	ags_lv2_default_path[i] = g_strdup(iter);

	i++;	
      }

      ags_lv2_default_path[i] = NULL;
    }else{
      gchar *home_dir;
      guint i;

#ifdef __APPLE__
#ifdef AGS_MAC_BUNDLE
      if((home_dir = getenv("HOME")) != NULL){
	ags_lv2_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }else{
	ags_lv2_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }
#else
      if((home_dir = getenv("HOME")) != NULL){
	ags_lv2_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }else{
	ags_lv2_default_path = (gchar **) malloc(4 * sizeof(gchar *));
      }
#endif
      
      i = 0;

#ifdef AGS_MAC_BUNDLE
      ags_lv2_default_path[i++] = g_strdup_printf("%s/lv2",
						  getenv("GSEQUENCER_PLUGIN_DIR"));
#endif
    
      ags_lv2_default_path[i++] = g_strdup("/Library/Audio/Plug-Ins/LV2");
      ags_lv2_default_path[i++] = g_strdup("/usr/lib/lv2");
      ags_lv2_default_path[i++] = g_strdup("/usr/local/lib/lv2");

      if(home_dir != NULL){
	ags_lv2_default_path[i++] = g_strdup_printf("%s/Library/Audio/Plug-Ins/LV2",
						    home_dir);
      }
    
      ags_lv2_default_path[i++] = NULL;
#else
      if((home_dir = getenv("HOME")) != NULL){
	ags_lv2_default_path = (gchar **) malloc(6 * sizeof(gchar *));
      }else{
	ags_lv2_default_path = (gchar **) malloc(5 * sizeof(gchar *));
      }
    
      i = 0;
    
      ags_lv2_default_path[i++] = g_strdup("/usr/lib64/lv2");
      ags_lv2_default_path[i++] = g_strdup("/usr/local/lib64/lv2");
      ags_lv2_default_path[i++] = g_strdup("/usr/lib/lv2");
      ags_lv2_default_path[i++] = g_strdup("/usr/local/lib/lv2");

      if(home_dir != NULL){
	ags_lv2_default_path[i++] = g_strdup_printf("%s/.lv2",
						    home_dir);
      }
    
      ags_lv2_default_path[i++] = NULL;
#endif
    }
  }
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
    {
      g_value_set_string(value, lv2_manager->locale);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_manager_dispose(GObject *gobject)
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = AGS_LV2_MANAGER(gobject);

  if(lv2_manager->lv2_plugin != NULL){
    g_list_free_full(lv2_manager->lv2_plugin,
		     g_object_unref);

    lv2_manager->lv2_plugin = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_manager_parent_class)->dispose(gobject);
}

void
ags_lv2_manager_finalize(GObject *gobject)
{
  AgsLv2Manager *lv2_manager;

  GList *lv2_plugin;

  lv2_manager = AGS_LV2_MANAGER(gobject);

  pthread_mutex_destroy(lv2_manager->obj_mutex);
  free(lv2_manager->obj_mutex);

  pthread_mutexattr_destroy(lv2_manager->obj_mutexattr);
  free(lv2_manager->obj_mutexattr);

  lv2_plugin = lv2_manager->lv2_plugin;

  g_list_free_full(lv2_plugin,
		   g_object_unref);

  if(lv2_manager == ags_lv2_manager){
    ags_lv2_manager = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_manager_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_manager_get_class_mutex:
 * 
 * Get class mutex.
 * 
 * Returns: the class mutex of #AgsLv2Manager
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_lv2_manager_get_class_mutex()
{
  return(&ags_lv2_manager_class_mutex);
}

/**
 * ags_lv2_manager_get_default_path:
 * 
 * Get lv2 manager default plugin path.
 *
 * Returns: the plugin default search path as a string vector
 * 
 * Since: 2.0.0
 */
gchar**
ags_lv2_manager_get_default_path()
{
  return(ags_lv2_default_path);
}

/**
 * ags_lv2_manager_set_default_path:
 * @default_path: the string vector array to use as default path
 * 
 * Set lv2 manager default plugin path.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_manager_set_default_path(gchar** default_path)
{
  ags_lv2_default_path = default_path;
}

/**
 * ags_lv2_manager_get_filenames:
 * @lv2_manager: the #AgsLv2Manager
 * 
 * Retrieve all filenames
 *
 * Returns: a %NULL-terminated array of filenames
 *
 * Since: 2.0.0
 */
gchar**
ags_lv2_manager_get_filenames(AgsLv2Manager *lv2_manager)
{
  GList *start_lv2_plugin, *lv2_plugin;
  
  gchar **filenames;
  
  guint i;
  gboolean contains_filename;

  pthread_mutex_t *lv2_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2_MANAGER(lv2_manager)){
    return(NULL);
  }
  
  /* get lv2 manager mutex */
  pthread_mutex_lock(ags_lv2_manager_get_class_mutex());
  
  lv2_manager_mutex = lv2_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_manager_get_class_mutex());

  /* collect */
  pthread_mutex_lock(lv2_manager_mutex);

  lv2_plugin = 
    start_lv2_plugin = g_list_copy(lv2_manager->lv2_plugin);

  pthread_mutex_unlock(lv2_manager_mutex);

  filenames = NULL;
  
  for(i = 0; lv2_plugin != NULL;){
    gchar *filename;
    
    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin->data)->obj_mutex;
    
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* duplicate filename */
    pthread_mutex_lock(base_plugin_mutex);

    filename = g_strdup(AGS_BASE_PLUGIN(lv2_plugin->data)->filename);

    pthread_mutex_unlock(base_plugin_mutex);

    if(filenames == NULL){
      filenames = (gchar **) malloc(2 * sizeof(gchar *));
      filenames[i] = filename;
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
    
    lv2_plugin = lv2_plugin->next;
  }

  g_list_free(start_lv2_plugin);

  return(filenames);
}

/**
 * ags_lv2_manager_find_lv2_plugin:
 * @lv2_manager: the #AgsLv2Manager
 * @filename: the filename of the plugin
 * @effect: the effect's name
 *
 * Lookup filename in loaded plugins.
 *
 * Returns: the #AgsLv2Plugin-struct
 *
 * Since: 2.0.0
 */
AgsLv2Plugin*
ags_lv2_manager_find_lv2_plugin(AgsLv2Manager *lv2_manager,
				gchar *filename, gchar *effect)
{
  AgsLv2Plugin *lv2_plugin;
  
  GList *start_list, *list;

  gboolean success;  

  pthread_mutex_t *lv2_manager_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_LV2_MANAGER(lv2_manager) ||
     filename == NULL ||
     effect == NULL){
    return(NULL);
  }
  
  /* get lv2 manager mutex */
  pthread_mutex_lock(ags_lv2_manager_get_class_mutex());
  
  lv2_manager_mutex = lv2_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_manager_get_class_mutex());

  /* collect */
  pthread_mutex_lock(lv2_manager_mutex);

  list = 
    start_list = g_list_copy(lv2_manager->lv2_plugin);

  pthread_mutex_unlock(lv2_manager_mutex);

  success = FALSE;
  
  while(list != NULL){
    lv2_plugin = AGS_LV2_PLUGIN(list->data);

    /* get base plugin mutex */
    pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
    base_plugin_mutex = AGS_BASE_PLUGIN(lv2_plugin)->obj_mutex;
    
    pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

    /* check filename and effect */
    pthread_mutex_lock(base_plugin_mutex);

    success = (!g_strcmp0(AGS_BASE_PLUGIN(lv2_plugin)->filename,
			  filename) &&
	       !g_strcmp0(AGS_BASE_PLUGIN(lv2_plugin)->effect,
			  effect)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(base_plugin_mutex);
    
    if(success){
      break;
    }

    list = list->next;
  }

  g_list_free(start_list);

  if(!success){
    lv2_plugin = NULL;
  }

  return(lv2_plugin);
}

/**
 * ags_lv2_manager_load_blacklist:
 * @lv2_manager: the #AgsLv2Manager
 * @blacklist_filename: the filename as string
 * 
 * Load blacklisted plugin filenames.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_manager_load_blacklist(AgsLv2Manager *lv2_manager,
			       gchar *blacklist_filename)
{
  pthread_mutex_t *lv2_manager_mutex;

  if(!AGS_IS_LV2_MANAGER(lv2_manager) ||
     blacklist_filename == NULL){
    return;
  }
  
  /* get lv2 manager mutex */
  pthread_mutex_lock(ags_lv2_manager_get_class_mutex());
  
  lv2_manager_mutex = lv2_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_manager_get_class_mutex());

  /* fill in */
  pthread_mutex_lock(lv2_manager_mutex);

  if(g_file_test(blacklist_filename,
		 (G_FILE_TEST_EXISTS |
		  G_FILE_TEST_IS_REGULAR))){
    FILE *file;

    gchar *str;
    
    file = fopen(blacklist_filename,
		 "r");

    while(getline(&str, NULL, file) != -1){
      lv2_manager->lv2_plugin_blacklist = g_list_prepend(lv2_manager->lv2_plugin_blacklist,
							 str);
    }
  }

  pthread_mutex_unlock(lv2_manager_mutex);
} 

/**
 * ags_lv2_manager_load_file:
 * @lv2_manager: the #AgsLv2Manager
 * @manifest: the manifest
 * @turtle: the loaded turtle
 * @lv2_path: the lv2 path
 * @filename: the filename of the plugin
 *
 * Load @filename specified plugin.
 *
 * Since: 2.0.0
 */
void
ags_lv2_manager_load_file(AgsLv2Manager *lv2_manager,
			  AgsTurtle *manifest,
			  AgsTurtle *turtle,
			  gchar *lv2_path,
			  gchar *filename)
{
  AgsLv2Plugin *lv2_plugin;

  xmlNode *node;

  GList *pname_list;
  GList *effect_list;
  GList *uri_list;

  gchar *str;
  gchar *path;
  gchar *xpath;
  gchar *turtle_pname;
  gchar *effect;
  gchar *escaped_effect;
  gchar *uri;
  
  guint effect_index;
  
  GError *error;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  uint32_t i;

  pthread_mutex_t *lv2_manager_mutex;

  if(!AGS_IS_LV2_MANAGER(lv2_manager) ||
     !AGS_IS_TURTLE(manifest) ||
     !AGS_IS_TURTLE(turtle) ||
     lv2_path == NULL ||
     filename == NULL){
    return;
  }
  
  /* get lv2 manager mutex */
  pthread_mutex_lock(ags_lv2_manager_get_class_mutex());
  
  lv2_manager_mutex = lv2_manager->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_manager_get_class_mutex());

  /* load */
  pthread_mutex_lock(lv2_manager_mutex);

  path = g_strdup_printf("%s/%s",
			 lv2_path,
			 filename);

  g_message("lv2 check - %s", path);

  plugin_so = dlopen(path,
		     RTLD_NOW);
  
  if(plugin_so == NULL){
    g_warning("ags_lv2_manager.c - failed to load static object file");    
    dlerror();
    
    pthread_mutex_unlock(lv2_manager_mutex);

    return;
  }
  
  if(dlerror() != NULL){
    return;
  }
  
  //  xmlSaveFormatFileEnc("-", turtle->doc, "UTF-8", 1);
  
  /* parse lv2 plugin */
  lv2_manager->current_plugin_node = g_hash_table_new_full(g_direct_hash, g_direct_equal,
							   NULL,
							   NULL);

  xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple/rdf-predicate-object-list/rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length('doap:name') + 1) = 'doap:name']/ancestor::*[self::rdf-verb][1]/following-sibling::rdf-object-list[1]//rdf-string[text()]";
  effect_list = ags_turtle_find_xpath(turtle,
				      xpath);

  while(effect_list != NULL){
    xmlNode *current_triple;
    
    AgsUUID *current_uuid;
    
    /* read effect name */
    node = effect_list->data;
    
    if(node == NULL){
      effect_list = effect_list->next;
	  
      continue;
    }
	  
    str = xmlNodeGetContent(node);

    if(strlen(str) < 2){
      effect_list = effect_list->next;
	  
      continue;
    }

    effect = g_strndup(str + 1,
		       strlen(str) - 2);

    /* fill current plugin node */
    xpath = "./ancestor::*[self::rdf-triple][1]";
    current_triple = ags_turtle_find_xpath_with_context_node(turtle,
							     xpath,
							     node)->data;

    current_uuid = ags_uuid_alloc();
    ags_uuid_generate(current_uuid);
    
    escaped_effect = ags_string_util_escape_single_quote(effect);

    g_hash_table_insert(lv2_manager->current_plugin_node,
			current_uuid, current_triple);
    
    /* find URI */
    xpath = "./rdf-subject/rdf-iri";
    uri_list = ags_turtle_find_xpath_with_context_node(turtle,
						       xpath,
						       current_triple);
    
    uri = NULL;
  
    if(uri_list != NULL){
      xmlNode *child;

      child = ((xmlNode *) uri_list->data)->children;

      while(child != NULL){
	if(child->type == XML_ELEMENT_NODE){
	  if(!g_ascii_strncasecmp(child->name,
				  "rdf-iriref",
				  11)){
	    uri = xmlNodeGetContent(child);

	    if(strlen(uri) > 2){
	      uri = g_strndup(uri + 1,
			      strlen(uri) - 2);
	    }
	    break;
	  }else if(!g_ascii_strncasecmp(child->name,
					"rdf-prefixed-name",
					18)){
	    xmlNode *pname_node;

	    gchar *pname;
	  
	    pname_node = child->children;
	    pname = NULL;
	  
	    while(pname_node != NULL){
	      if(pname_node->type == XML_ELEMENT_NODE){
		if(!g_ascii_strncasecmp(pname_node->name,
					"rdf-pname-ln",
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

		str = g_strdup_printf("//rdf-pname-ns[text()='%s']/following-sibling::*[self::rdf-iriref][1]",
				      prefix);
		prefix_node = ags_turtle_find_xpath_with_context_node(turtle,
								      str,
								      current_triple);
		free(str);

		if(prefix_node != NULL){
		  gchar *iriref;

		  iriref = xmlNodeGetContent(prefix_node->data);

		  if(iriref != NULL){
		    if(strlen(iriref) > 2){
		      gchar *tmp;
		    
		      tmp = g_strndup(iriref + 1,
				      strlen(iriref) - 2);
		      uri = g_strdup_printf("%s%s",
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

    /* turtle pname */
    xpath = "./rdf-pname-ln[1]";
    pname_list = ags_turtle_find_xpath_with_context_node(turtle,
							 xpath,
							 current_triple);

    if(pname_list != NULL){
      turtle_pname = xmlNodeGetContent(pname_list->data);
    
      g_list_free(pname_list);
    }else{
      turtle_pname = NULL;
    }
    
    /* get uri index and append plugin */
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor");
  
    if(lv2_descriptor){
      for(i = 0; (plugin_descriptor = lv2_descriptor(i)) != NULL; i++){
	if(path != NULL &&
	   effect != NULL &&
	   uri != NULL &&
	   !g_ascii_strcasecmp(plugin_descriptor->URI,
			       uri)){
	  /* check if already added */
	  lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
						       path,
						       effect);

	  if(lv2_plugin != NULL){
	    break;
	  }

	  if(ags_base_plugin_find_effect(lv2_manager->lv2_plugin,
					 path,
					 effect) == NULL){
	    g_message("ags_lv2_manager.c loading - %s %s as %s",
		      path,
		      turtle->filename,
		      effect);

	    lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
				      "manifest", manifest,
				      "turtle", turtle,
				      "uuid", current_uuid,
				      "filename", path,
				      "effect", effect,
				      "pname", turtle_pname,
				      "uri", uri,
				      "effect-index", i,
				      NULL);
	    ags_base_plugin_load_plugin((AgsBasePlugin *) lv2_plugin);
	    lv2_manager->lv2_plugin = g_list_prepend(lv2_manager->lv2_plugin,
						     lv2_plugin);
	  }
	  
	  break;
	}
      }  
    }
    
    effect_list = effect_list->next;
  }

  pthread_mutex_unlock(lv2_manager_mutex);

  g_hash_table_destroy(lv2_manager->current_plugin_node);
  
  lv2_manager->current_plugin_node = NULL;
}

/**
 * ags_lv2_manager_load_preset:
 * @lv2_manager: the #AgsLv2Manager
 * @lv2_plugin: the #AgsLv2Plugin
 * @preset: the #AgsTurtle
 * 
 * Load preset.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_manager_load_preset(AgsLv2Manager *lv2_manager,
			    AgsLv2Plugin *lv2_plugin,
			    AgsTurtle *preset)
{
  AgsLv2Preset *lv2_preset;

  GList *preset_list;

  gchar *str;
  gchar *xpath;
  gchar *uri;

  if(!AGS_IS_LV2_MANAGER(lv2_manager) ||
     !AGS_IS_LV2_PLUGIN(lv2_plugin) ||
     lv2_plugin->preset != NULL ||
     !AGS_IS_TURTLE(preset)){
    return;
  }
  
  xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple/rdf-predicate-object-list/rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':preset') + 1) = ':preset']/ancestor::*[self::rdf-triple][1]/rdf-subject/rdf-iri";

  preset_list = ags_turtle_find_xpath(preset,
				      xpath);

  while(preset_list != NULL){
    xmlNode *child;
    
    child = ((xmlNode *) preset_list->data)->children;
    uri = NULL;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"rdf-iriref",
				11)){
	  uri = xmlNodeGetContent(child);

	  if(strlen(uri) > 2){
	    uri = g_strndup(uri + 1,
			    strlen(uri) - 2);
	  }
	  break;
	}else if(!g_ascii_strncasecmp(child->name,
				      "rdf-prefixed-name",
				      18)){
	  xmlNode *pname_node;

	  gchar *pname;
	  
	  pname_node = child->children;
	  pname = NULL;
	  
	  while(pname_node != NULL){
	    if(pname_node->type == XML_ELEMENT_NODE){
	      if(!g_ascii_strncasecmp(pname_node->name,
				      "rdf-pname-ln",
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

	      str = g_strdup_printf("//rdf-pname-ns[text()='%s']/following-sibling::*[self::rdf-iriref][1]",
				    prefix);
	      prefix_node = ags_turtle_find_xpath(preset,
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
		    uri = g_strdup_printf("%s%s",
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

    if(uri == NULL){
      g_message("uri not found %s", AGS_BASE_PLUGIN(lv2_plugin)->filename);
      
      /* iterate */
      preset_list = preset_list->next;

      continue;
    }

    g_message("parse presets for %s", uri);
    
    lv2_preset = g_object_new(AGS_TYPE_LV2_PRESET,
			      "lv2-plugin", lv2_plugin,
			      "turtle", preset,
			      "uri", uri,
			      NULL);
    ags_lv2_preset_parse_turtle(lv2_preset);

    g_object_set(lv2_plugin,
		 "preset", lv2_preset,
		 NULL);
    
    /* iterate */
    preset_list = preset_list->next;
  }
}

/**
 * ags_lv2_manager_load_default_directory:
 * @lv2_manager: the #AgsLv2Manager
 * 
 * Loads all available plugins.
 *
 * Since: 2.0.0
 */
void
ags_lv2_manager_load_default_directory(AgsLv2Manager *lv2_manager)
{
  GDir *dir;

  gchar **lv2_path;
  gchar *path, *plugin_path;
  gchar *xpath;
  gchar *str;

  GError *error;

  if(!AGS_IS_LV2_MANAGER(lv2_manager)){
    return;
  }
  
  lv2_path = ags_lv2_default_path;

  while(*lv2_path != NULL){
    if(!g_file_test(*lv2_path,
		    G_FILE_TEST_EXISTS)){
      lv2_path++;
      
      continue;
    }

    error = NULL;
    dir = g_dir_open(*lv2_path,
		     0,
		     &error);

    if(error != NULL){
      g_warning("%s", error->message);

      lv2_path++;

      continue;
    }

    while((path = g_dir_read_name(dir)) != NULL){
      if(!g_ascii_strncasecmp(path,
			      "..",
			      3) ||
	 !g_ascii_strncasecmp(path,
			      ".",
			      2)){
	continue;
      }
    
      plugin_path = g_strdup_printf("%s/%s",
				    *lv2_path,
				    path);

      if(g_file_test(plugin_path,
		     G_FILE_TEST_IS_DIR)){
	AgsLv2Plugin *lv2_plugin;
	
	AgsTurtle *manifest, *turtle;
	AgsTurtle *preset;
	
	xmlDoc *doc;

	//	FILE *out;

	//	xmlChar *buffer;
	//	int size;
      
	GList *ttl_list, *binary_list;
	GList *preset_list;
	GList *pname_list;
	GList *list;
	
	gchar *manifest_filename;

	gchar *turtle_path, *filename;
	gchar *turtle_pname;
	
	gchar *preset_path;
	
	gboolean turtle_loaded;

	manifest_filename = g_strdup_printf("%s/manifest.ttl",
					    plugin_path);

	if(!g_file_test(manifest_filename,
			G_FILE_TEST_EXISTS)){
	  continue;
	}
	
	manifest = ags_turtle_new(manifest_filename);
	ags_turtle_load(manifest,
			NULL);

	/* read binary from turtle */
	str = g_strdup_printf("/rdf-turtle-doc/rdf-statement/rdf-triple/rdf-predicate-object-list/rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':binary') + 1) = ':binary']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref[substring(text(), string-length(text()) - string-length('%s>') + 1) = '%s>']",
			      AGS_LIBRARY_SUFFIX,
			      AGS_LIBRARY_SUFFIX);
	
	binary_list = ags_turtle_find_xpath(manifest,
					    str);

	/* persist XML */
	//NOTE:JK: no need for it
      
	//xmlDocDumpFormatMemoryEnc(manifest->doc, &buffer, &size, "UTF-8", TRUE);

	//out = fopen(g_strdup_printf("%s/manifest.xml", plugin_path), "w+");

	//fwrite(buffer, size, sizeof(xmlChar), out);
	//fflush(out);
            
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
	  filename = g_strdup_printf("%s/%s",
				     path,
				     str);
	  free(str);

	  /* read turtle from manifest */
	  ttl_list = ags_turtle_find_xpath_with_context_node(manifest,
							     "./ancestor::*[self::rdf-triple][1]//rdf-iriref[substring(text(), string-length(text()) - string-length('.ttl>') + 1) = '.ttl>']",
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
				    "http://",
				    7)){
	      ttl_list = ttl_list->next;
	  
	      continue;
	    }

	    /* load turtle doc */
	    if((turtle = (AgsTurtle *) ags_turtle_manager_find(ags_turtle_manager_get_instance(),
							       turtle_path)) == NULL){
	      turtle = ags_turtle_new(g_strdup_printf("%s/%s",
						      plugin_path,
						      turtle_path));
	      ags_turtle_load(turtle,
			      NULL);
	      ags_turtle_manager_add(ags_turtle_manager_get_instance(),
				     (GObject *) turtle);
	    }
	
	    /* load specified plugin */
	    if(!g_list_find_custom(lv2_manager->lv2_plugin_blacklist,
				   filename,
				   strcmp)){
	      ags_lv2_manager_load_file(lv2_manager,
					manifest,
					turtle,
					*lv2_path,
					filename);
	    }
	    
	    /* persist XML */
	    //NOTE:JK: no need for it
	    //xmlDocDumpFormatMemoryEnc(turtle->doc, &buffer, &size, "UTF-8", TRUE);

	    //out = fopen(g_strdup_printf("%s/%s.xml", plugin_path, turtle_path), "w+");
	
	    //fwrite(buffer, size, sizeof(xmlChar), out);
	    //fflush(out);
	    //	xmlSaveFormatFileEnc("-", turtle->doc, "UTF-8", 1);

	    ttl_list = ttl_list->next;
	  }
	
	  binary_list = binary_list->next;
	}

    	/* read presets from turtle */
	xpath = "/rdf-turtle-doc/rdf-statement/rdf-triple/rdf-predicate-object-list/rdf-verb[@verb='a']/following-sibling::*[self::rdf-object-list]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':preset') + 1) = ':preset']/ancestor::*[self::rdf-triple]";
	preset_list = ags_turtle_find_xpath(manifest,
					    xpath);

	while(preset_list != NULL){
	  xpath = ".//rdf-iriref[substring(text(), string-length(text()) - string-length('.ttl>') + 1) = '.ttl>']";
	  list = ags_turtle_find_xpath_with_context_node(manifest,
							 xpath,
							 preset_list->data);
	  
	  /* read filename of turtle */
	  preset_path = xmlNodeGetContent((xmlNode *) list->data);

	  if(preset_path == NULL){
	    preset_list = preset_list->next;

	    continue;
	  }

	  /* read filename */
	  preset_path = g_strndup(&(preset_path[1]),
				  strlen(preset_path) - 2);
	
	  if(!g_ascii_strncasecmp(preset_path,
				  "http://",
				  7)){
	    preset_list = preset_list->next;
	  
	    continue;
	  }

	  /* turtle pname */
	  xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':appliesto') + 1) = ':appliesto']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-pname-ln";
	  pname_list = ags_turtle_find_xpath_with_context_node(manifest,
							       xpath,
							       preset_list->data);

	  if(pname_list != NULL){
	    turtle_pname = xmlNodeGetContent(pname_list->data);
    
	    g_list_free(pname_list);
	  }else{
	    preset_list = preset_list->next;

	    continue;
	  }

	  /* find lv2 plugin by pname */
	  list = ags_lv2_plugin_find_pname(lv2_manager->lv2_plugin,
					   turtle_pname);	  
	  
	  if(list == NULL){
	    preset_list = preset_list->next;

	    continue;
	  }

	  lv2_plugin = list->data;
	  
	  /* load turtle doc */
	  if((preset = (AgsTurtle *) ags_turtle_manager_find(ags_turtle_manager_get_instance(),
							     preset_path)) == NULL){
	    preset = ags_turtle_new(g_strdup_printf("%s/%s",
						    plugin_path,
						    preset_path));
	    ags_turtle_load(preset,
			    NULL);
	    ags_turtle_manager_add(ags_turtle_manager_get_instance(),
				   (GObject *) preset);
	  }

	  ags_lv2_manager_load_preset(lv2_manager,
				      lv2_plugin,
				      preset);

	  preset_list = preset_list->next;
	}
      }
    }

    lv2_path++;
  }
}

/**
 * ags_lv2_manager_get_instance:
 *
 * Get instance.
 *
 * Returns: the #AgsLv2Manager
 *
 * Since: 2.0.0
 */
AgsLv2Manager*
ags_lv2_manager_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_lv2_manager == NULL){
    ags_lv2_manager = ags_lv2_manager_new(AGS_LV2_MANAGER_DEFAULT_LOCALE);
  }
  
  pthread_mutex_unlock(&mutex);

  return(ags_lv2_manager);
}

/**
 * ags_lv2_manager_new:
 * @locale: the default locale
 *
 * Create a new instance of #AgsLv2Manager
 *
 * Returns: the new #AgsLv2Manager
 *
 * Since: 2.0.0
 */
AgsLv2Manager*
ags_lv2_manager_new(gchar *locale)
{
  AgsLv2Manager *lv2_manager;

  lv2_manager = (AgsLv2Manager *) g_object_new(AGS_TYPE_LV2_MANAGER,
					       "locale", locale,
					       NULL);

  return(lv2_manager);
}
