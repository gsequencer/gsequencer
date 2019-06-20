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

#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_lv2ui_plugin.h>

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
   * Since: 2.1.78
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
   * Since: 2.1.78
   */
  param_spec = g_param_spec_pointer("plugin",
				    i18n_pspec("lv2 plugins"),
				    i18n_pspec("The parsed lv2 plugins"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN,
				  param_spec);

  /**
   * AgsLv2TurtleParser:turtle:
   *
   * The assigned #GList-struct containing #AgsLv2uiPlugin.
   * 
   * Since: 2.1.78
   */
  param_spec = g_param_spec_pointer("ui-plugin",
				    i18n_pspec("lv2 UI plugins"),
				    i18n_pspec("The parsed lv2 UI plugins"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_PLUGIN,
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
 * Since: 2.1.78
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
 * Since: 2.1.78
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

  auto void ags_lv2_turtle_parser_parse_statement(AgsTurtle *current_turtle,
						  xmlNode **node,
						  AgsTurtle **turtle, guint *n_turtle);
  auto void ags_lv2_turtle_parser_parse_triple(AgsTurtle *current_turtle,
					       xmlNode **node,
					       AgsTurtle **turtle, guint *n_turtle);

  auto void ags_lv2_turtle_parser_parse_predicate_object_list(AgsTurtle *current_turtle,
							      gchar *subject_iriref,
							      xmlNode **node,
							      AgsTurtle **turtle, guint *n_turtle);
  auto void ags_lv2_turtle_parser_parse_blank_node_property_list(AgsTurtle *current_turtle,
								 gchar *subject_iriref,
								 xmlNode **node,
								 AgsTurtle **turtle, guint *n_turtle);
  
  void ags_lv2_turtle_parser_parse_statement(AgsTurtle *current_turtle,
					     xmlNode **node,
					     AgsTurtle **turtle, guint *n_turtle)
  {
    xmlNode *child;

    if(node == NULL){
      return;
    }
    
    child = node[0]->children;
    
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
					  xmlNode **node,
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
    
    child = node[0]->children;

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
	  
	  if(xpath_result != NULL){
	    subject_iriref = xmlNodeGetContent((xmlNode *) xpath_result->data);
	  }

	  g_list_free(xpath_result);
	  
	  g_free(xpath);
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
							 xmlNode **node,
							 AgsTurtle **turtle, guint *n_turtle)
  {
    xmlNode *child;

    GList *xpath_result;

    gchar *xpath;

    gboolean is_plugin, is_ui_plugin;
    
    if(node == NULL){
      return;
    }

    child = node[0]->children;

    is_plugin = FALSE;
    is_ui_plugin = FALSE;
    
    /* parse verbs */
    xpath = g_strdup_printf("./rdf-verb[@verb = 'a']");

    xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
							   xpath,
							   (xmlNode *) node[0]);
    
    g_free(xpath);

    while(xpath_result != NULL){
      xmlNode *current;

      GList *current_xpath_result;
      
      gchar *current_xpath;
      
      current = ((xmlNode *) xpath_result->data)->next;

      current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-iriref[text() = '<http://lv2plug.in/ns/lv2core#plugin>']");

      current_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								     current_xpath,
								     (xmlNode *) current);

      g_free(current_xpath);
	  
      if(current_xpath_result != NULL){
	is_plugin = TRUE;
      }else{
	AgsTurtle *turtle_iter;
	
	gchar *prefix_id;

	prefix_id = NULL;

	for(turtle_iter = turtle; turtle_iter[0] != NULL; turtle_iter++){
	  gchar *str;
	  
	  if((str = g_hash_table_lookup(turtle_iter[0]->prefix_id,
					"<http://lv2plug.in/ns/lv2core#>")) != NULL){
	    prefix_id = str;
	  }
	}
	
	current_xpath = g_strdup_printf("./rdf-object/rdf-iri/rdf-prefixed-name/rdf-pname-ln[text() = '<%s%s>']",
					prefix_id,
					"plugin");

	current_xpath_result = ags_turtle_find_xpath_with_context_node(current_turtle,
								       current_xpath,
								       (xmlNode *) current);

	g_free(current_xpath);
	
	if(current_xpath_result != NULL){
	  is_plugin = TRUE;
	}
      }
      
      g_list_free(current_xpath_result);
	  
      /* iterate */
      xpath_result = xpath_result->next;
    }
    
    g_list_free(xpath_result);
  }

  void ags_lv2_turtle_parser_parse_blank_node_property_list(AgsTurtle *current_turtle,
							    gchar *subject_iriref,
							    xmlNode **node,
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
					      &node,
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
 * Since: 2.1.78
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
