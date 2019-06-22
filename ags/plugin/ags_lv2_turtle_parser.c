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

#include <ags/plugin/ags_lv2_turtle_parser.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2ui_manager.h>
#include <ags/plugin/ags_lv2_preset_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2ui_plugin.h>
#include <ags/plugin/ags_lv2_preset.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_lv2_turtle_parser_class_init(AgsLv2TurtleParserClass *lv2_turtle_parser);
void ags_lv2_turtle_parser_init (AgsLv2TurtleParser *lv2_turtle_parser);
void ags_lv2_turtle_parser_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_lv2_turtle_parser_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_lv2_turtle_parser_dispose(GObject *gobject);
void ags_lv2_turtle_parser_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_turtle_parser
 * @short_description: The lv2 turtle parser class
 * @title: AgsLv2TurtleParser
 * @section_id:
 * @include: ags/plugin/ags_lv2_turtle_parser.h
 *
 * The #AgsLv2TurtleParser parses RDF Turtle files.
 */

static gpointer ags_lv2_turtle_parser_parent_class = NULL;

static pthread_mutex_t ags_lv2_turtle_parser_class_mutex = PTHREAD_MUTEX_INITIALIZER;

enum{
  PROP_0,
  PROP_TURTLE,
  PROP_PLUGIN,
  PROP_UI_PLUGIN,
  PROP_PRESET,
};

GType
ags_lv2_turtle_parser_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_turtle_parser = 0;

    static const GTypeInfo ags_lv2_turtle_parser_info = {
      sizeof(AgsLv2TurtleParserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_turtle_parser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLv2TurtleParser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_turtle_parser_init,
    };

    ags_type_lv2_turtle_parser = g_type_register_static(G_TYPE_OBJECT,
							"AgsLv2TurtleParser",
							&ags_lv2_turtle_parser_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_turtle_parser);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_turtle_parser_class_init(AgsLv2TurtleParserClass *lv2_turtle_parser)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_lv2_turtle_parser_parent_class = g_type_class_peek_parent(lv2_turtle_parser);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_turtle_parser;

  gobject->set_property = ags_lv2_turtle_parser_set_property;
  gobject->get_property = ags_lv2_turtle_parser_get_property;

  gobject->dispose = ags_lv2_turtle_parser_dispose;
  gobject->finalize = ags_lv2_turtle_parser_finalize;

  /* properties */
  /**
   * AgsLv2TurtleParser:turtle:
   *
   * The assigned #GList-struct containing #AgsTurtle.
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_pointer("turtle",
				    i18n_pspec("related turtles"),
				    i18n_pspec("The related turtles"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsLv2TurtleParser:plugin:
   *
   * The assigned #GList-struct containing #AgsLv2Plugin.
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_pointer("plugin",
				    i18n_pspec("lv2 plugins"),
				    i18n_pspec("The parsed lv2 plugins"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN,
				  param_spec);

  /**
   * AgsLv2TurtleParser:ui-pluin:
   *
   * The assigned #GList-struct containing #AgsLv2uiPlugin.
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_pointer("ui-plugin",
				    i18n_pspec("lv2 UI plugins"),
				    i18n_pspec("The parsed lv2 UI plugins"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_PLUGIN,
				  param_spec);

  /**
   * AgsLv2TurtleParser:preset:
   *
   * The assigned #GList-struct containing #AgsLv2Preset.
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_pointer("preset",
				    i18n_pspec("lv2 presets"),
				    i18n_pspec("The parsed lv2 presets"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET,
				  param_spec);
}

void
ags_lv2_turtle_parser_init(AgsLv2TurtleParser *lv2_turtle_parser)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  lv2_turtle_parser->flags = 0;

  /* add base plugin mutex */
  lv2_turtle_parser->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  lv2_turtle_parser->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /*  */
  lv2_turtle_parser->turtle = NULL;
  
  lv2_turtle_parser->plugin = NULL;
  lv2_turtle_parser->ui_plugin = NULL;
  lv2_turtle_parser->preset = NULL;
}

void
ags_lv2_turtle_parser_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsLv2TurtleParser *lv2_turtle_parser;

  pthread_mutex_t *lv2_turtle_parser_mutex;

  lv2_turtle_parser = AGS_LV2_TURTLE_PARSER(gobject);

  /* get lv2 turtle parser mutex */
  pthread_mutex_lock(ags_lv2_turtle_parser_get_class_mutex());
  
  lv2_turtle_parser_mutex = lv2_turtle_parser->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_turtle_parser_get_class_mutex());

  switch(prop_id){
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_pointer(value);

      pthread_mutex_lock(lv2_turtle_parser_mutex);

      if(!AGS_IS_TURTLE(turtle) ||
	 g_list_find(lv2_turtle_parser->turtle, turtle) != NULL){
	pthread_mutex_unlock(lv2_turtle_parser_mutex);
	
	return;
      }

      g_object_ref(turtle);
      lv2_turtle_parser->turtle = g_list_prepend(lv2_turtle_parser->turtle,
						 turtle);

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  case PROP_PLUGIN:
    {
      AgsLv2Plugin *lv2_plugin;

      lv2_plugin = (AgsLv2Plugin *) g_value_get_pointer(value);

      pthread_mutex_lock(lv2_turtle_parser_mutex);

      if(!AGS_IS_LV2_PLUGIN(lv2_plugin) ||
	 g_list_find(lv2_turtle_parser->plugin, lv2_plugin) != NULL){
	pthread_mutex_unlock(lv2_turtle_parser_mutex);
	
	return;
      }

      g_object_ref(lv2_plugin);
      lv2_turtle_parser->plugin = g_list_prepend(lv2_turtle_parser->plugin,
						 lv2_plugin);

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  case PROP_UI_PLUGIN:
    {
      AgsLv2uiPlugin *lv2ui_plugin;

      lv2ui_plugin = (AgsLv2uiPlugin *) g_value_get_pointer(value);

      pthread_mutex_lock(lv2_turtle_parser_mutex);

      if(!AGS_IS_LV2UI_PLUGIN(lv2ui_plugin) ||
	 g_list_find(lv2_turtle_parser->ui_plugin, lv2ui_plugin) != NULL){
	pthread_mutex_unlock(lv2_turtle_parser_mutex);
	
	return;
      }

      g_object_ref(lv2ui_plugin);
      lv2_turtle_parser->ui_plugin = g_list_prepend(lv2_turtle_parser->ui_plugin,
						    lv2ui_plugin);

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  case PROP_PRESET:
    {
      AgsLv2Preset *lv2_preset;

      lv2_preset = (AgsLv2Preset *) g_value_get_pointer(value);

      pthread_mutex_lock(lv2_turtle_parser_mutex);

      if(!AGS_IS_LV2_PRESET(lv2_preset) ||
	 g_list_find(lv2_turtle_parser->preset, lv2_preset) != NULL){
	pthread_mutex_unlock(lv2_turtle_parser_mutex);
	
	return;
      }

      g_object_ref(lv2_preset);
      lv2_turtle_parser->preset = g_list_prepend(lv2_turtle_parser->preset,
						 lv2_preset);

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_turtle_parser_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsLv2TurtleParser *lv2_turtle_parser;

  pthread_mutex_t *lv2_turtle_parser_mutex;

  lv2_turtle_parser = AGS_LV2_TURTLE_PARSER(gobject);

  /* get lv2 turtle parser mutex */
  pthread_mutex_lock(ags_lv2_turtle_parser_get_class_mutex());
  
  lv2_turtle_parser_mutex = lv2_turtle_parser->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_turtle_parser_get_class_mutex());

  switch(prop_id){
  case PROP_TURTLE:
    {
      pthread_mutex_lock(lv2_turtle_parser_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(lv2_turtle_parser->turtle,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  case PROP_PLUGIN:
    {
      pthread_mutex_lock(lv2_turtle_parser_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(lv2_turtle_parser->plugin,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  case PROP_UI_PLUGIN:
    {
      pthread_mutex_lock(lv2_turtle_parser_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(lv2_turtle_parser->ui_plugin,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  case PROP_PRESET:
    {
      pthread_mutex_lock(lv2_turtle_parser_mutex);
      
      g_value_set_pointer(value, g_list_copy_deep(lv2_turtle_parser->preset,
						  (GCopyFunc) g_object_ref,
						  NULL));

      pthread_mutex_unlock(lv2_turtle_parser_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_turtle_parser_dispose(GObject *gobject)
{
  AgsLv2TurtleParser *lv2_turtle_parser;

  lv2_turtle_parser = AGS_LV2_TURTLE_PARSER(gobject);

  if(lv2_turtle_parser->turtle != NULL){
    g_list_free_full(lv2_turtle_parser->turtle,
		     g_object_unref);

    lv2_turtle_parser->turtle = NULL;
  }

  if(lv2_turtle_parser->plugin != NULL){
    g_list_free_full(lv2_turtle_parser->plugin,
		     g_object_unref);

    lv2_turtle_parser->plugin = NULL;
  }

  if(lv2_turtle_parser->ui_plugin != NULL){
    g_list_free_full(lv2_turtle_parser->ui_plugin,
		     g_object_unref);

    lv2_turtle_parser->ui_plugin = NULL;
  }
  
  if(lv2_turtle_parser->preset != NULL){
    g_list_free_full(lv2_turtle_parser->preset,
		     g_object_unref);

    lv2_turtle_parser->preset = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_turtle_parser_parent_class)->dispose(gobject);
}

void
ags_lv2_turtle_parser_finalize(GObject *gobject)
{
  AgsLv2TurtleParser *lv2_turtle_parser;

  lv2_turtle_parser = AGS_LV2_TURTLE_PARSER(gobject);

  /* destroy object mutex */
  pthread_mutex_destroy(lv2_turtle_parser->obj_mutex);
  free(lv2_turtle_parser->obj_mutex);

  pthread_mutexattr_destroy(lv2_turtle_parser->obj_mutexattr);
  free(lv2_turtle_parser->obj_mutexattr);

  if(lv2_turtle_parser->turtle != NULL){
    g_list_free_full(lv2_turtle_parser->turtle,
		     g_object_unref);
  }

  if(lv2_turtle_parser->plugin != NULL){
    g_list_free_full(lv2_turtle_parser->plugin,
		     g_object_unref);
  }

  if(lv2_turtle_parser->ui_plugin != NULL){
    g_list_free_full(lv2_turtle_parser->ui_plugin,
		     g_object_unref);
  }

  if(lv2_turtle_parser->preset != NULL){
    g_list_free_full(lv2_turtle_parser->preset,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_turtle_parser_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_turtle_parser_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.2.0
 */
pthread_mutex_t*
ags_lv2_turtle_parser_get_class_mutex()
{
  return(&ags_lv2_turtle_parser_class_mutex);
}

/**
 * ags_lv2_turtle_parser_parse:
 * @lv2_turtle_parser: the #AgsLv2TurtleParser
 * @turtle: the %NULL terminated array of #AgsTurtle
 * @n_turtle: the turtle count
 * 
 * Parse manifest and refered turtles. 
 * 
 * Since: 2.2.0
 */
void
ags_lv2_turtle_parser_parse(AgsLv2TurtleParser *lv2_turtle_parser,
			    AgsTurtle **turtle, guint *n_turtle)
{
  AgsTurtle *manifest;
  AgsTurtle *current_turtle;

  xmlNode *root_node;
  xmlNode *node;
  
  pthread_mutex_t *lv2_turtle_parser_mutex;

  auto gboolean ags_lv2_turtle_parser_parse_find_pname(gpointer key,
						       gpointer value,
						       gpointer user_data);
  
  auto void ags_lv2_turtle_parser_parse_statement(AgsTurtle *current_turtle,
						  xmlNode *node,
						  AgsTurtle **turtle, guint *n_turtle);
  auto void ags_lv2_turtle_parser_parse_triple(AgsTurtle *current_turtle,
					       xmlNode *node,
					       AgsTurtle **turtle, guint *n_turtle);

  auto void ags_lv2_turtle_parser_parse_predicate_object_list(AgsTurtle *current_turtle,
							      gchar *subject_iriref,
							      xmlNode *node,
							      AgsTurtle **turtle, guint *n_turtle);
  auto void ags_lv2_turtle_parser_parse_blank_node_property_list(AgsTurtle *current_turtle,
								 gchar *subject_iriref,
								 xmlNode *node,
								 AgsTurtle **turtle, guint *n_turtle);

  gboolean ags_lv2_turtle_parser_parse_find_pname(gpointer key,
						  gpointer value,
						  gpointer user_data)
  {
    if(value != NULL &&
       user_data != NULL &&
       !g_strcmp0(value,
		  user_data)){
      return(TRUE);
    }

    return(FALSE);
  }
  
  void ags_lv2_turtle_parser_parse_statement(AgsTurtle *current_turtle,
					     xmlNode *node,
					     AgsTurtle **turtle, guint *n_turtle)
  {
    xmlNode *child;

    if(node == NULL){
      return;
    }
    
    child = node->children;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"rdf-triple",
				11)){
	  ags_lv2_turtle_parser_parse_triple(current_turtle,
					     &child,
					     turtle, n_turtle);
	}
      }

      child = child->next;
    }
  }

  void ags_lv2_turtle_parser_parse_triple(AgsTurtle *current_turtle,
					  xmlNode *node,
					  AgsTurtle **turtle, guint *n_turtle)
  {
    xmlNode *child;
    xmlNode *subject;
    xmlNode *predicate_object_list;
    xmlNode *blank_node_property_list;

    gchar *subject_iriref;
    
    if(node == NULL){
      return;
    }
    
    child = node->children;

    subject = NULL;
    predicate_object_list = NULL;
    blank_node_property_list = NULL;
    
    subject_iriref = NULL;
    
    while(child != NULL){
      if(child->type == XML_ELEMENT_NODE){
	if(!g_ascii_strncasecmp(child->name,
				"rdf-subject",
				12)){
	  GList *xpath_result;
	  
	  gchar *xpath;

	  subject = child;
	  
	  xpath = g_strdup_printf("./rdf-iri/rdf-iriref");

	  xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								 xpath,
								 (xmlNode *) child);
	  
	  g_free(xpath);
	  
	  if(xpath_result != NULL){
	    subject_iriref = xmlNodeGetContent((xmlNode *) xpath_result->data);
	  }else{
	    AgsTurtle **turtle_iter;
	    
	    gchar *pname;
	    
	    xpath = g_strdup_printf("./rdf-iri/rdf-prefixed-name/rdf-pname-ln");
	    
	    xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								   xpath,
								   (xmlNode *) child);
	    
	    pname = xmlNodeGetContent((xmlNode *) xpath_result->data);

	    for(turtle_iter = turtle; turtle_iter[0] != NULL; turtle_iter++){
	      gchar *str;
	    
	      str = g_hash_table_find(turtle_iter[0],
				      (GHRFunc) ags_lv2_turtle_parser_parse_find_pname,
				      pname);

	      if(str != NULL){
		subject_iriref = str;
	      }
	    }
	  }

	  g_list_free(xpath_result);
	}else if(!g_ascii_strncasecmp(child->name,
				      "rdf-predicate-object-list",
				      27)){
	  ags_lv2_turtle_parser_parse_predicate_object_list(current_turtle,
							    subject_iriref,
							    &child,
							    turtle, n_turtle);
	}else if(!g_ascii_strncasecmp(child->name,
				      "rdf-blank-node-property-list",
				      29)){
	  ags_lv2_turtle_parser_parse_blank_node_property_list(current_turtle,
							       subject_iriref,
							       &child,
							       turtle, n_turtle);
	}
      }

      child = child->next;
    }    
  }

  void ags_lv2_turtle_parser_parse_predicate_object_list(AgsTurtle *current_turtle,
							 gchar *subject_iriref,
							 xmlNode *node,
							 AgsTurtle **turtle, guint *n_turtle)
  {
    AgsLv2Manager *lv2_manager;
    AgsLv2uiManager *lv2ui_manager;
    AgsLv2PresetManager *lv2_preset_manager;
    AgsLv2Plugin *lv2_plugin;
    AgsLv2uiPlugin *lv2ui_plugin;
    AgsLv2Preset *lv2_preset;
    
    AgsTurtle **turtle_iter;
    
    xmlNode *child;

    GList *start_xpath_result, *xpath_result;

    gchar *prefix_id_rdfs;
    gchar *prefix_id_rdf;
    gchar *prefix_id_lv2_core;
    gchar *prefix_id_lv2ui;
    gchar *prefix_id_lv2p;
    gchar *xpath;

    gboolean is_plugin, is_ui_plugin;
    gboolean is_preset;
    
    if(node == NULL){
      return;
    }

    prefix_id_lv2_core = NULL;
    prefix_id_lv2ui = NULL;
    
    for(turtle_iter = turtle; turtle_iter[0] != NULL; turtle_iter++){
      gchar *str;
	  
      if((str = g_hash_table_lookup(turtle_iter[0]->prefix_id,
				    "<http://www.w3.org/2000/01/rdf-schema#>")) != NULL){
	prefix_id_rdfs = str;
      }

      if((str = g_hash_table_lookup(turtle_iter[0]->prefix_id,
				    "<http://www.w3.org/1999/02/22-rdf-syntax-ns#>")) != NULL){
	prefix_id_rdf = str;
      }

      if((str = g_hash_table_lookup(turtle_iter[0]->prefix_id,
				    "<http://lv2plug.in/ns/lv2core#>")) != NULL){
	prefix_id_lv2_core = str;
      }

      if((str = g_hash_table_lookup(turtle_iter[0]->prefix_id,
				    "<http://lv2plug.in/ns/extensions/ui#>")) != NULL){
	prefix_id_lv2ui = str;
      }

      if((str = g_hash_table_lookup(turtle_iter[0]->prefix_id,
				    "<http://lv2plug.in/ns/ext/presets#>")) != NULL){
	prefix_id_lv2p = str;
      }
    }
    
    lv2_manager = ags_lv2_manager_get_instance();
    lv2ui_manager = ags_lv2ui_manager_get_instance();
    lv2_preset_manager = ags_lv2_preset_manager_get_instance();

    lv2_plugin = NULL;
    
    child = node->children;

    is_plugin = FALSE;
    is_ui_plugin = FALSE;

    is_preset = FALSE;
    
    /* parse verbs */
    xpath = g_strdup_printf("./rdf-verb[@verb = 'a']");

    xpath_result =
      start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								   xpath,
								   (xmlNode *) node);
    
    g_free(xpath);

    while(start_xpath_result != NULL){
      xmlNode *current;

      GList *current_start_xpath_result, *current_xpath_result;
      
      gchar *current_xpath;
      
      current = ((xmlNode *) xpath_result->data)->next;

      /* check plugin */
      current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#plugin>']");

      current_xpath_result =
	current_start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
									     current_xpath,
									     (xmlNode *) current);

      g_free(current_xpath);
	  
      if(current_start_xpath_result != NULL){
	is_plugin = TRUE;
      }else{
	gchar *prefix_id;

	prefix_id = prefix_id_lv2_core;
	
	current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
					prefix_id,
					"plugin");

	current_xpath_result =
	  current_start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
									       current_xpath,
									       (xmlNode *) current);

	g_free(current_xpath);
	
	if(current_start_xpath_result != NULL){
	  is_plugin = TRUE;
	}
      }
      
      g_list_free(current_start_xpath_result);
      
      /* check UI plugin */
      current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/extensions/ui#gtkui>']");

      current_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								     current_xpath,
								     (xmlNode *) current);

      g_free(current_xpath);
	  
      if(current_xpath_result != NULL){
	is_ui_plugin = TRUE;
      }else{
	gchar *prefix_id;

	prefix_id = prefix_id_lv2ui;
	
	current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
					prefix_id,
					"gtkui");

	current_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								       current_xpath,
								       (xmlNode *) current);

	g_free(current_xpath);
	
	if(current_xpath_result != NULL){
	  is_ui_plugin = TRUE;
	}
      }
      
      g_list_free(current_start_xpath_result);

      /* check preset */
      current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/ext/presets#preset>']");

      current_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								     current_xpath,
								     (xmlNode *) current);

      g_free(current_xpath);
	  
      if(current_xpath_result != NULL){
	is_preset = TRUE;
      }else{
	gchar *prefix_id;

	prefix_id = prefix_id_lv2p;
	
	current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
					prefix_id,
					"preset");

	current_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								       current_xpath,
								       (xmlNode *) current);

	g_free(current_xpath);
	
	if(current_xpath_result != NULL){
	  is_preset = TRUE;
	}
      }
      
      g_list_free(current_start_xpath_result);
      
      /* iterate */
      xpath_result = xpath_result->next;
    }
    
    g_list_free(start_xpath_result);

    /* plugin create instance */
    if(is_plugin){
      GList *list;
      
      list = ags_lv2_plugin_find_uri(lv2_manager->lv2_plugin,
				     subject_iriref);

      if(list != NULL){
	lv2_plugin = list->data;
      }
      
      if(lv2_plugin == NULL){
	AgsUUID *current_uuid;

	xmlNode *node_binary;
	
	gchar *path;
	gchar *so_filename;
	gchar *filename;

	current_uuid = ags_uuid_alloc();
	ags_uuid_generate(current_uuid);

	/* so filename */
	node_binary = NULL;
	
	xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#binary>']");

	xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
							       xpath,
							       (xmlNode *) node);
	  
	g_free(xpath);
	  
	if(xpath_result != NULL){
	  node_binary = ((xmlNode *) xpath_result->data)->parent->parent->parent->next;
	}else{
	  gchar *prefix_id;

	  prefix_id = prefix_id_lv2_core;
	  
	  xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
				  prefix_id,
				  "binary");

	  xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								 xpath,
								 (xmlNode *) node);
	  
	  g_free(xpath);

	  if(xpath_result != NULL){
	    node_binary = ((xmlNode *) xpath_result->data)->parent->parent->parent->parent->next;
	  }
	}
	
	g_list_free(start_xpath_result);
	
	path = dirname(turtle[0]->filename);
	so_filename = NULL;

	if(node_binary != NULL){
	  xmlNode *current;

	  GList *current_start_xpath_result, *current_xpath_result;
      
	  gchar *current_xpath;

	  current_xpath = g_strdup_printf("./rdf-object-list/rdf-object/rdf-iri/rdf-iriref");

	  current_xpath_result =
	    current_start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										 current_xpath,
										 (xmlNode *) node_binary);

	  g_free(current_xpath);

	  if(current_start_xpath_result != NULL){
	    so_filename = xmlNodeGetContent(current_start_xpath_result->data);
	  }
	  
	  g_list_free(current_start_xpath_result);
	}
	
	filename = g_strdup_printf("%s/%s",
				   path,
				   so_filename);
	
	lv2_plugin = g_object_new(AGS_TYPE_LV2_PLUGIN,
				  "manifest", turtle[0],
				  "turtle", turtle[n_turtle[0] - 1],
				  "uuid", current_uuid,
				  "filename", filename,
				  "uri", subject_iriref,
				  NULL);
	lv2_manager->lv2_plugin = g_list_prepend(lv2_manager->lv2_plugin,
						 lv2_plugin);
	
	g_free(filename);
      }
    }

    if(is_ui_plugin){
      GList *list;
      
      list = ags_lv2ui_plugin_find_gui_uri(lv2ui_manager->lv2ui_plugin,
					   subject_iriref);

      if(list != NULL){
	lv2ui_plugin = list->data;
      }
      
      if(lv2ui_plugin == NULL){
	AgsUUID *current_uuid;

	xmlNode *node_binary;
	
	gchar *path;
	gchar *so_filename;
	gchar *filename;

	current_uuid = ags_uuid_alloc();
	ags_uuid_generate(current_uuid);

	/* so filename */
	node_binary = NULL;
	
	xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/extensions/ui#binary>']");

	xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
							       xpath,
							       (xmlNode *) node);
	  
	g_free(xpath);
	  
	if(xpath_result != NULL){
	  node_binary = ((xmlNode *) xpath_result->data)->parent->parent->parent->next;
	}else{
	  gchar *prefix_id;

	  prefix_id = prefix_id_lv2ui;
	  
	  xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
				  prefix_id,
				  "binary");

	  xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								 xpath,
								 (xmlNode *) node);
	  
	  g_free(xpath);

	  if(xpath_result != NULL){
	    node_binary = ((xmlNode *) xpath_result->data)->parent->parent->parent->parent->next;
	  }
	}
	
	g_list_free(start_xpath_result);
	
	path = dirname(turtle[0]->filename);
	so_filename = NULL;

	if(node_binary != NULL){
	  xmlNode *current;

	  GList *current_start_xpath_result, *current_xpath_result;
      
	  gchar *current_xpath;

	  current_xpath = g_strdup_printf("./rdf-object-list/rdf-object/rdf-iri/rdf-iriref");

	  current_xpath_result =
	    current_start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										 current_xpath,
										 (xmlNode *) node_binary);

	  g_free(current_xpath);

	  if(current_start_xpath_result != NULL){
	    so_filename = xmlNodeGetContent(current_start_xpath_result->data);
	  }
	  
	  g_list_free(current_start_xpath_result);
	}
	
	filename = g_strdup_printf("%s/%s",
				   path,
				   so_filename);
	lv2ui_plugin = g_object_new(AGS_TYPE_LV2UI_PLUGIN,
				    "manifest", turtle[0],
				    "gui-turtle", turtle[n_turtle[0] - 1],
				    "uuid", current_uuid,
				    "ui-filename", filename,
				    "gui-uri", subject_iriref,
				    NULL);
	lv2ui_manager->lv2ui_plugin = g_list_prepend(lv2ui_manager->lv2ui_plugin,
						     lv2ui_plugin);

	g_free(filename);
      }
    }
    
    if(is_preset){
      GList *list;
      
      list = ags_lv2_preset_find_uri(lv2_preset_manager->lv2_preset,
				     subject_iriref);

      if(list != NULL){
	lv2_preset = list->data;
      }
      
      if(lv2_preset == NULL){
	lv2_preset = g_object_new(AGS_TYPE_LV2_PRESET,
				  "manifest", turtle[0],
				  "turtle", turtle[n_turtle[0] - 1],
				  "uri", subject_iriref,
				  NULL);

	lv2_preset_manager->lv2_preset = g_list_prepend(lv2_preset_manager->lv2_preset,
							lv2_preset);
      }
    }
    
    /* plugin - read metadata */
    //TODO:JK: implement me
    
    /* plugin - read ports */
    if(is_plugin){
      xmlNode *current;
      
      current = NULL;
      
      xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#port>']");
      
      xpath_result =
	start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								     xpath,
								     (xmlNode *) node);
      
      g_free(xpath);

      if(start_xpath_result != NULL){
	current = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
      }else{
	gchar *prefix_id;

	prefix_id = prefix_id_lv2_core;

	xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
				prefix_id,
				"port");

	xpath_result =
	  start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								       xpath,
								       (xmlNode *) node);

	g_free(xpath);	

	if(start_xpath_result != NULL){
	  current = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
	}
      }

      g_list_free(start_xpath_result);
      
      if(current != NULL &&
	 !g_ascii_strncasecmp(current->name,
			      "rdf-object-list",
			      16)){
	xmlNode *current_child;

	current_child = NULL;

	if(current != NULL){
	  current_child = current->children;
	}
	
	while(current_child != NULL){
	  if(current_child->type == XML_ELEMENT_NODE){
	    if(!g_ascii_strncasecmp(current_child->name,
				    "rdf-object",
				    12)){
	      AgsPluginPort *plugin_port;

	      xmlNode *current_predicate;
	      
	      GList *current_child_start_xpath_result, *current_child_xpath_result;
	      GList *start_port_predicate_xpath_result, *port_predicate_xpath_result;
      
	      gchar *current_child_xpath;
	      gchar *port_predicate_xpath;
	      
	      gboolean is_audio_port, is_atom_port, is_event_port, is_control_port;
	      gboolean is_output_port, is_input_port;
	      guint scale_point_count;
	      gboolean scale_point_has_pname;		

	      plugin_port = ags_plugin_port_new();
	      g_object_set(lv2_plugin,
			   plugin_port);
	      
	      /* init range value */
	      g_value_init(plugin_port->upper_value,
			   G_TYPE_FLOAT);
	      g_value_init(plugin_port->lower_value,
			   G_TYPE_FLOAT);
	      g_value_init(plugin_port->default_value,
			   G_TYPE_FLOAT);

	      /* init range */
	      g_value_set_float(plugin_port->upper_value,
				0.0);
	      g_value_set_float(plugin_port->lower_value,
				0.0);
	      g_value_set_float(plugin_port->default_value,
				0.0);

	      /* port type */	      
	      current_child_xpath = g_strdup_printf("./rdf-verb[@verb = 'a']");

	      current_child_xpath_result =
		current_child_start_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   xpath,
											   (xmlNode *) current_child);
    
	      g_free(current_child_xpath);

	      if(current_child_start_xpath_result != NULL){
		xmlNode *node_port_type;
		
		GList *start_port_type_xpath_result, *port_type_xpath_result;

		gchar *port_type_xpath;
		
		node_port_type = ((xmlNode *) current_child_xpath_result->data)->parent->next;

		/* check audio port */
		port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#audioport>']");
		
		port_type_xpath_result =
		  start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											 port_type_xpath,
											 (xmlNode *) current);

		g_free(port_type_xpath);
		
		if(start_port_type_xpath_result != NULL){
		  is_audio_port = TRUE;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;

		  port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						    prefix_id,
						    "audioport");

		  port_type_xpath_result =
		    start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   port_type_xpath,
											   (xmlNode *) current);

		  g_free(port_type_xpath);

		  if(start_port_type_xpath_result != NULL){
		    is_audio_port = TRUE;
		  }
		}		

		g_list_free(start_port_type_xpath_result);

		/* check atom port */
		port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#atomport>']");
		
		port_type_xpath_result =
		  start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											 port_type_xpath,
											 (xmlNode *) current);

		g_free(port_type_xpath);
		
		if(start_port_type_xpath_result != NULL){
		  is_atom_port = TRUE;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						    prefix_id,
						    "atomport");

		  port_type_xpath_result =
		    start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   port_type_xpath,
											   (xmlNode *) current);

		  g_free(port_type_xpath);

		  if(start_port_type_xpath_result != NULL){
		    is_atom_port = TRUE;
		  }
		}		

		g_list_free(start_port_type_xpath_result);

		/* check event port */
		port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#eventport>']");
		
		port_type_xpath_result =
		  start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											 port_type_xpath,
											 (xmlNode *) current);

		g_free(port_type_xpath);
		
		if(start_port_type_xpath_result != NULL){
		  is_event_port = TRUE;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						    prefix_id,
						    "eventport");

		  port_type_xpath_result =
		    start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   port_type_xpath,
											   (xmlNode *) current);

		  g_free(port_type_xpath);

		  if(start_port_type_xpath_result != NULL){
		    is_event_port = TRUE;
		  }
		}		

		g_list_free(start_port_type_xpath_result);

		/* check control port */
		port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#controlport>']");
		
		port_type_xpath_result =
		  start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											 port_type_xpath,
											 (xmlNode *) current);

		g_free(port_type_xpath);
		
		if(start_port_type_xpath_result != NULL){
		  is_control_port = TRUE;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						    prefix_id,
						    "controlport");

		  port_type_xpath_result =
		    start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   port_type_xpath,
											   (xmlNode *) current);

		  g_free(port_type_xpath);

		  if(start_port_type_xpath_result != NULL){
		    is_control_port = TRUE;
		  }
		}		

		g_list_free(start_port_type_xpath_result);

		/* check output port */
		port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#outputport>']");
		
		port_type_xpath_result =
		  start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											 port_type_xpath,
											 (xmlNode *) current);

		g_free(port_type_xpath);
		
		if(start_port_type_xpath_result != NULL){
		  is_output_port = TRUE;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						    prefix_id,
						    "outputport");

		  port_type_xpath_result =
		    start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   port_type_xpath,
											   (xmlNode *) current);

		  g_free(port_type_xpath);

		  if(start_port_type_xpath_result != NULL){
		    is_output_port = TRUE;
		  }
		}		

		g_list_free(start_port_type_xpath_result);

		/* check input port */
		port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#inputport>']");
		
		port_type_xpath_result =
		  start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											 port_type_xpath,
											 (xmlNode *) current);

		g_free(port_type_xpath);
		
		if(start_port_type_xpath_result != NULL){
		  is_input_port = TRUE;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_type_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						    prefix_id,
						    "inputport");

		  port_type_xpath_result =
		    start_port_type_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   port_type_xpath,
											   (xmlNode *) current);

		  g_free(port_type_xpath);

		  if(start_port_type_xpath_result != NULL){
		    is_input_port = TRUE;
		  }
		}		

		g_list_free(start_port_type_xpath_result);
	      }
	      
	      g_list_free(current_child_start_xpath_result);

	      /* set flags */
	      if(is_audio_port){
		ags_plugin_port_set_flags(plugin_port,
					  AGS_PLUGIN_PORT_AUDIO);
	      }

	      if(is_atom_port){
		ags_plugin_port_set_flags(plugin_port,
					  AGS_PLUGIN_PORT_ATOM);
	      }

	      if(is_event_port){
		ags_plugin_port_set_flags(plugin_port,
					  AGS_PLUGIN_PORT_EVENT);
	      }

	      if(is_control_port){
		ags_plugin_port_set_flags(plugin_port,
					  AGS_PLUGIN_PORT_CONTROL);
	      }

	      if(is_output_port){
		ags_plugin_port_set_flags(plugin_port,
					  AGS_PLUGIN_PORT_OUTPUT);
	      }

	      if(is_input_port){
		ags_plugin_port_set_flags(plugin_port,
					  AGS_PLUGIN_PORT_INPUT);
	      }

              /* port index */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#portindex>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "portindex");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *value_xpath;

		value_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-numeric");

		value_xpath_result =
		  start_value_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										     value_xpath,
										     (xmlNode *) current);

		g_free(value_xpath);

		if(start_value_xpath_result != NULL){
		  gchar *str;

		  str = xmlNodeGetContent((xmlNode *) start_value_xpath_result->data);

		  if(str != NULL){
		    guint port_index;

		    port_index = g_ascii_strtoull(str,
						  NULL,
						  10);

		    g_object_set(plugin_port,
				 "port-index", port_index,
				 NULL);
		  }
		}

		g_list_free(start_value_xpath_result);
	      }

              /* port name */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#portname>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "portname");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *value_xpath;

		value_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-string");

		value_xpath_result =
		  start_value_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										     value_xpath,
										     (xmlNode *) current);

		g_free(value_xpath);

		if(start_value_xpath_result != NULL){
		  gchar *str;

		  str = xmlNodeGetContent((xmlNode *) start_value_xpath_result->data);

		  if(str != NULL){
		    g_object_set(plugin_port,
				 "port-name", str,
				 NULL);
		  }
		}

		g_list_free(start_value_xpath_result);
	      }

              /* port symbol */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#portsymbol>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "portsymbol");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *value_xpath;

		value_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-numeric");

		value_xpath_result =
		  start_value_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										     value_xpath,
										     (xmlNode *) current);

		g_free(value_xpath);

		if(start_value_xpath_result != NULL){
		  gchar *str;

		  str = xmlNodeGetContent((xmlNode *) start_value_xpath_result->data);

		  if(str != NULL){
		    g_object_set(plugin_port,
				 "port-symbol", str,
				 NULL);
		  }
		}

		g_list_free(start_value_xpath_result);
	      }
	      
              /* port property */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#portproperty>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "portproperty");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_toggled_xpath_result, *toggled_xpath_result;
		GList *start_enumeration_xpath_result, *enumeration_xpath_result;
		GList *start_logarithmic_xpath_result, *logarithmic_xpath_result;
		GList *start_integer_xpath_result, *integer_xpath_result;

		gchar *toggled_xpath;
		gchar *enumeration_xpath;
		gchar *logarithmic_xpath;
		gchar *integer_xpath;

		/* toggled */
		toggled_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#toggled>']");

		toggled_xpath_result =
		  start_toggled_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										       toggled_xpath,
										       (xmlNode *) current);

		g_free(toggled_xpath);

		if(start_toggled_xpath_result != NULL){
		  ags_plugin_port_set_flags(plugin_port,
					    AGS_PLUGIN_PORT_TOGGLED);
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_predicate_xpath = g_strdup_printf("./rdf-object-list/rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
							 prefix_id,
							 "toggled");

		  port_predicate_xpath_result =
		    start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
												port_predicate_xpath,
												(xmlNode *) current);

		  g_free(port_predicate_xpath);

		  if(start_port_predicate_xpath_result != NULL){
		    ags_plugin_port_set_flags(plugin_port,
					      AGS_PLUGIN_PORT_TOGGLED);
		  }
		}

		/* enumeration */
		enumeration_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#enumeration>']");

		enumeration_xpath_result =
		  start_enumeration_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   enumeration_xpath,
											   (xmlNode *) current);

		g_free(enumeration_xpath);

		if(start_enumeration_xpath_result != NULL){
		  ags_plugin_port_set_flags(plugin_port,
					    AGS_PLUGIN_PORT_ENUMERATION);
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_predicate_xpath = g_strdup_printf("./rdf-object-list/rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
							 prefix_id,
							 "enumeration");

		  port_predicate_xpath_result =
		    start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
												port_predicate_xpath,
												(xmlNode *) current);

		  g_free(port_predicate_xpath);

		  if(start_port_predicate_xpath_result != NULL){
		    ags_plugin_port_set_flags(plugin_port,
					      AGS_PLUGIN_PORT_ENUMERATION);
		  }
		}

		/* logarithmic */
		logarithmic_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#logarithmic>']");

		logarithmic_xpath_result =
		  start_logarithmic_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											   logarithmic_xpath,
											   (xmlNode *) current);

		g_free(logarithmic_xpath);

		if(start_logarithmic_xpath_result != NULL){
		  ags_plugin_port_set_flags(plugin_port,
					    AGS_PLUGIN_PORT_LOGARITHMIC);
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_predicate_xpath = g_strdup_printf("./rdf-object-list/rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
							 prefix_id,
							 "logarithmic");

		  port_predicate_xpath_result =
		    start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
												port_predicate_xpath,
												(xmlNode *) current);

		  g_free(port_predicate_xpath);

		  if(start_port_predicate_xpath_result != NULL){
		    ags_plugin_port_set_flags(plugin_port,
					      AGS_PLUGIN_PORT_LOGARITHMIC);
		  }
		}

		/* integer */
		integer_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#integer>']");

		integer_xpath_result =
		  start_integer_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										       integer_xpath,
										       (xmlNode *) current);

		g_free(integer_xpath);

		if(start_integer_xpath_result != NULL){
		  ags_plugin_port_set_flags(plugin_port,
					    AGS_PLUGIN_PORT_INTEGER);
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_lv2_core;
	
		  port_predicate_xpath = g_strdup_printf("./rdf-object-list/rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
							 prefix_id,
							 "integer");

		  port_predicate_xpath_result =
		    start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
												port_predicate_xpath,
												(xmlNode *) current);

		  g_free(port_predicate_xpath);

		  if(start_port_predicate_xpath_result != NULL){
		    ags_plugin_port_set_flags(plugin_port,
					      AGS_PLUGIN_PORT_INTEGER);
		  }
		}
		
		g_list_free(start_integer_xpath_result);
	      }

              /* default */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#default>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "default");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *value_xpath;

		value_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-numeric");

		value_xpath_result =
		  start_value_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										     value_xpath,
										     (xmlNode *) current);

		g_free(value_xpath);

		if(start_value_xpath_result != NULL){
		  gchar *str;

		  str = xmlNodeGetContent((xmlNode *) start_value_xpath_result->data);

		  if(str != NULL){
		    gfloat default_value;

		    default_value = g_ascii_strtod(str,
						   NULL);
		    g_value_set_float(plugin_port->default_value,
				      default_value);
		  }
		}

		g_list_free(start_value_xpath_result);
	      }

              /* minimum */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#minimum>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "minimum");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *value_xpath;

		value_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-numeric");

		value_xpath_result =
		  start_value_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										     value_xpath,
										     (xmlNode *) current);

		g_free(value_xpath);

		if(start_value_xpath_result != NULL){
		  gchar *str;

		  str = xmlNodeGetContent((xmlNode *) start_value_xpath_result->data);

		  if(str != NULL){
		    gfloat minimum_value;

		    minimum_value = g_ascii_strtod(str,
						   NULL);
		    g_value_set_float(plugin_port->lower_value,
				      minimum_value);
		  }
		}

		g_list_free(start_value_xpath_result);
	      }

              /* maximum */
	      current_predicate = NULL;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#maximum>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "maximum");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  current_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		}
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      if(current_predicate != NULL){
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *value_xpath;

		value_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-numeric");

		value_xpath_result =
		  start_value_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										     value_xpath,
										     (xmlNode *) current);

		g_free(value_xpath);

		if(start_value_xpath_result != NULL){
		  gchar *str;

		  str = xmlNodeGetContent((xmlNode *) start_value_xpath_result->data);

		  if(str != NULL){
		    gfloat maximum_value;

		    maximum_value = g_ascii_strtod(str,
						   NULL);
		    g_value_set_float(plugin_port->upper_value,
				      maximum_value);
		  }
		}

		g_list_free(start_value_xpath_result);
	      }

	      /* scale point */
	      scale_point_has_pname = FALSE;
	      
	      port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#scalepoint>']");
		
	      port_predicate_xpath_result =
		start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											    port_predicate_xpath,
											    (xmlNode *) current);

	      g_free(port_predicate_xpath);
		
	      if(start_port_predicate_xpath_result != NULL){
		//empty
	      }else{
		gchar *prefix_id;

		prefix_id = prefix_id_lv2_core;
	
		port_predicate_xpath = g_strdup_printf("./rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
						       prefix_id,
						       "scalepoint");

		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current);

		g_free(port_predicate_xpath);

		if(start_port_predicate_xpath_result != NULL){
		  scale_point_has_pname = TRUE;
		}
	      }

	      for(scale_point_count = 0; port_predicate_xpath_result != NULL; scale_point_count++){
		xmlNode *scale_point_predicate;
		
		GList *start_label_xpath_result, *label_xpath_result;
		GList *start_value_xpath_result, *value_xpath_result;

		gchar *label_xpath;
		gchar *value_xpath;

		plugin_port->scale_steps = scale_point_count + 1;
		
		if(plugin_port->scale_point == NULL){
		  plugin_port->scale_point = (gchar **) malloc(2 * sizeof(gchar *));

		  plugin_port->scale_point[0] = NULL;
		  plugin_port->scale_point[1] = NULL;

		  plugin_port->scale_value = (gdouble *) malloc(1 * sizeof(gdouble));

		  plugin_port->scale_value[0] = 0.0;
		}else{
		  plugin_port->scale_point = (gchar **) realloc(plugin_port->scale_point,
								(scale_point_count + 2) * sizeof(gchar *));

		  plugin_port->scale_point[scale_point_count] = NULL;
		  plugin_port->scale_point[scale_point_count + 1] = NULL;

		  plugin_port->scale_value = (gdouble *) realloc(plugin_port->scale_value,
								 (scale_point_count + 1) * sizeof(gdouble));

		  plugin_port->scale_value[scale_point_count] = 0.0;
		}
		
		current_predicate = NULL;

		if(!scale_point_has_pname){
		  current_predicate = ((xmlNode *) port_predicate_xpath_result->data)->parent->parent->parent->next;
		}else{
		  current_predicate = ((xmlNode *) port_predicate_xpath_result->data)->parent->parent->parent->parent->next;
		}
		
		/* label */
		scale_point_predicate = NULL;
	      
		port_predicate_xpath = g_strdup_printf("./rdf-object/rdf-blank-node-property-list/rdf-predicate-object-list/rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://www.w3.org/2000/01/rdf-schema#label>']");
		
		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current_predicate);

		g_free(port_predicate_xpath);
		
		if(start_port_predicate_xpath_result != NULL){
		  scale_point_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_rdfs;
	
		  port_predicate_xpath = g_strdup_printf("./rdf-object/rdf-blank-node-property-list/rdf-predicate-object-list/rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
							 prefix_id,
							 "label");

		  port_predicate_xpath_result =
		    start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
												port_predicate_xpath,
												(xmlNode *) current_predicate);

		  g_free(port_predicate_xpath);

		  if(start_port_predicate_xpath_result != NULL){
		    scale_point_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		  }
		}

		g_list_free(start_port_predicate_xpath_result);

		if(scale_point_predicate != NULL){
		  GList *start_label_xpath_result, *label_xpath_result;

		  gchar *label_xpath;

		  label_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-string");

		  label_xpath_result =
		    start_label_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										       label_xpath,
										       (xmlNode *) current);

		  g_free(label_xpath);

		  if(start_label_xpath_result != NULL){
		    gchar *str;

		    str = xmlNodeGetContent((xmlNode *) start_label_xpath_result->data);

		    if(str != NULL){
		      plugin_port->scale_point[scale_point_count] = g_strdup(str);
		    }
		  }

		  g_list_free(start_label_xpath_result);
		}
		
		/* value */
		scale_point_predicate = NULL;
	      
		port_predicate_xpath = g_strdup_printf("./rdf-object/rdf-blank-node-property-list/rdf-predicate-object-list/rdf-verb/rdf-predicate/rdf-iri/rdf-iriref[text() = '<http://www.w3.org/1999/02/22-rdf-syntax-ns#value>']");
		
		port_predicate_xpath_result =
		  start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
											      port_predicate_xpath,
											      (xmlNode *) current_predicate);

		g_free(port_predicate_xpath);
		
		if(start_port_predicate_xpath_result != NULL){
		  scale_point_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->next;
		}else{
		  gchar *prefix_id;

		  prefix_id = prefix_id_rdf;
	
		  port_predicate_xpath = g_strdup_printf("./rdf-object/rdf-blank-node-property-list/rdf-predicate-object-list/rdf-verb/rdf-predicate/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
							 prefix_id,
							 "value");

		  port_predicate_xpath_result =
		    start_port_predicate_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
												port_predicate_xpath,
												(xmlNode *) current_predicate);

		  g_free(port_predicate_xpath);

		  if(start_port_predicate_xpath_result != NULL){
		    scale_point_predicate = ((xmlNode *) start_xpath_result->data)->parent->parent->parent->parent->next;
		  }
		}

		g_list_free(start_port_predicate_xpath_result);

		if(scale_point_predicate != NULL){
		  GList *start_label_xpath_result, *label_xpath_result;

		  gchar *label_xpath;

		  label_xpath = g_strdup("./rdf-object-list/rdf-object/rdf-literal/rdf-string");

		  label_xpath_result =
		    start_label_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
										       label_xpath,
										       (xmlNode *) current);

		  g_free(label_xpath);

		  if(start_label_xpath_result != NULL){
		    gchar *str;

		    str = xmlNodeGetContent((xmlNode *) start_label_xpath_result->data);

		    if(str != NULL){
		      plugin_port->scale_value[scale_point_count] = g_ascii_strtod(str,
										   NULL);
		    }
		  }

		  g_list_free(start_label_xpath_result);
		}

		/* iterate */
		port_predicate_xpath_result = port_predicate_xpath_result->next;
	      }

	      g_list_free(start_port_predicate_xpath_result);

	      
	      g_object_unref(plugin_port);
	    }
	  }
	  
	  current_child = current_child->next;
	}
      }
    }
  }

  void ags_lv2_turtle_parser_parse_blank_node_property_list(AgsTurtle *current_turtle,
							    gchar *subject_iriref,
							    xmlNode *node,
							    AgsTurtle **turtle, guint *n_turtle)
  {
  }
  
  if(!AGS_IS_LV2_TURTLE_PARSER(lv2_turtle_parser)){
    return;
  }

  /* get lv2 turtle parser mutex */
  pthread_mutex_lock(ags_lv2_turtle_parser_get_class_mutex());
  
  lv2_turtle_parser_mutex = lv2_turtle_parser->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_turtle_parser_get_class_mutex());

  /* get manifest */
  pthread_mutex_lock(lv2_turtle_parser_mutex);
      
  manifest = g_list_last(lv2_turtle_parser->turtle);

  pthread_mutex_unlock(lv2_turtle_parser_mutex);

  if(turtle == NULL){
    if(manifest == NULL){
      return;
    }else{
      guint turtle_count;

      turtle_count = 1;

      turtle = (AgsTurtle **) malloc(2 * sizeof(AgsTurtle *));
      turtle[0] = manifest;
      turtle[1] = NULL;
        
      n_turtle = &turtle_count;
    }
  }

  if(n_turtle == NULL ||
     n_turtle[0] == 0){
    g_warning("missing argument");
    
    return;
  }

  current_turtle = turtle[n_turtle[0] - 1];  

  /* get root node */
  root_node = xmlDocGetRootElement(current_turtle->doc);

  /* start parse */
  node = root_node->children;

  while(node != NULL){
    if(node->type == XML_ELEMENT_NODE){
      if(!g_ascii_strncasecmp(node->name,
			      "rdf-statement",
			      14)){
	ags_lv2_turtle_parser_parse_statement(current_turtle,
					      node,
					      turtle, n_turtle);
      }
    }

    node = node->next;
  }
}

/**
 * ags_lv2_turtle_parser_new:
 * @manifest: the manifest as #AgsTurtle
 *
 * Creates an #AgsLv2TurtleParser
 *
 * Returns: a new #AgsLv2TurtleParser
 *
 * Since: 2.2.0
 */
AgsLv2TurtleParser*
ags_lv2_turtle_parser_new(AgsTurtle *manifest)
{
  AgsLv2TurtleParser *lv2_turtle_parser;

  lv2_turtle_parser = (AgsLv2TurtleParser *) g_object_new(AGS_TYPE_LV2_TURTLE_PARSER,
							  "turtle", manifest,
							  NULL);

  return(lv2_turtle_parser);
}
