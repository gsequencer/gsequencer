/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/plugin/ags_lv2_preset.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_lv2_preset_class_init(AgsLv2PresetClass *lv2_preset);
void ags_lv2_preset_init(AgsLv2Preset *lv2_preset);
void ags_lv2_preset_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_preset_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_preset_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_preset
 * @short_description: LV2 presets
 * @title: AgsLv2Preset
 * @section_id:
 * @include: ags/plugin/ags_lv2_preset.h
 *
 * The #AgsLv2Preset handles LV2 presets mapped to a bank.
 */

enum{
  PROP_0,
  PROP_LV2_PLUGIN,
  PROP_URI,
  PROP_BANK,
  PROP_PRESET_LABEL,
  PROP_MANIFEST,
  PROP_TURTLE,
  PROP_PORT_PRESET,
};

static gpointer ags_lv2_preset_parent_class = NULL;

static pthread_mutex_t ags_lv2_preset_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_lv2_preset_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_preset = 0;

    static const GTypeInfo ags_lv2_preset_info = {
      sizeof (AgsLv2PresetClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_preset_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Preset),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_preset_init,
    };
    
    ags_type_lv2_preset = g_type_register_static(G_TYPE_OBJECT,
						 "AgsLv2Preset",
						 &ags_lv2_preset_info,
						 0);    

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_preset);
  }

  return g_define_type_id__volatile;
}

void
ags_lv2_preset_class_init(AgsLv2PresetClass *lv2_preset)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_preset_parent_class = g_type_class_peek_parent(lv2_preset);

  /* GObject */
  gobject = (GObjectClass *) lv2_preset;

  gobject->set_property = ags_lv2_preset_set_property;
  gobject->get_property = ags_lv2_preset_get_property;

  gobject->finalize = ags_lv2_preset_finalize;

  /* properties */
  /**
   * AgsLv2Preset:lv2-plugin:
   *
   * The assigned lv2 plugin.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("lv2-plugin",
				   i18n_pspec("lv2 plugin of the preset"),
				   i18n_pspec("The lv2 plugin this preset is located in"),
				   AGS_TYPE_LV2_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LV2_PLUGIN,
				  param_spec);

  /**
   * AgsLv2Preset:uri:
   *
   * The assigned uri.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("uri",
				   i18n_pspec("uri of the preset"),
				   i18n_pspec("The uri this preset is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsLv2Preset:bank:
   *
   * The assigned bank.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("bank",
				   i18n_pspec("bank of the preset"),
				   i18n_pspec("The bank this preset is assigned with"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsLv2Preset:preset-label:
   *
   * The preset label.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("preset-label",
				   i18n_pspec("preset label"),
				   i18n_pspec("The preset label"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PRESET_LABEL,
				  param_spec);

  /**
   * AgsLv2Preset:manifest:
   *
   * The assigned manifest.
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_object("manifest",
				   i18n_pspec("manifest of the preset"),
				   i18n_pspec("The manifest this preset is refered by"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MANIFEST,
				  param_spec);

  /**
   * AgsLv2Preset:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("turtle",
				   i18n_pspec("turtle of the preset"),
				   i18n_pspec("The turtle this preset is located in"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);
  
  /**
   * AgsLv2Preset:port-preset:
   *
   * The port preset.
   * 
   * Since: 2.1.55
   */
  param_spec = g_param_spec_pointer("port-preset",
				    i18n_pspec("port-preset of the preset"),
				    i18n_pspec("The port-preset this preset is located in"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_PRESET,
				  param_spec);
}

void
ags_lv2_preset_init(AgsLv2Preset *lv2_preset)
{
  lv2_preset->flags = 0;

  /* add lv2 preset mutex */
  lv2_preset->obj_mutexattr =  (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(lv2_preset->obj_mutexattr);
  pthread_mutexattr_settype(lv2_preset->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(lv2_preset->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  lv2_preset->obj_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(lv2_preset->obj_mutex,
		     lv2_preset->obj_mutexattr);
  
  lv2_preset->lv2_plugin = NULL;

  lv2_preset->uri = NULL;

  lv2_preset->bank = NULL;
  lv2_preset->preset_label = NULL;

  lv2_preset->manifest = NULL;
  lv2_preset->turtle = NULL;

  lv2_preset->port_preset = NULL;
}


void
ags_lv2_preset_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Preset *lv2_preset;

  pthread_mutex_t *lv2_preset_mutex;

  lv2_preset = AGS_LV2_PRESET(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_lv2_preset_get_class_mutex());
  
  lv2_preset_mutex = lv2_preset->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_preset_get_class_mutex());

  switch(prop_id){
  case PROP_LV2_PLUGIN:
    {
      AgsLv2Plugin *lv2_plugin;

      lv2_plugin = (AgsLv2Plugin *) g_value_get_object(value);

      pthread_mutex_lock(lv2_preset_mutex);
      
      if(lv2_preset->lv2_plugin == (GObject *) lv2_plugin){
	pthread_mutex_unlock(lv2_preset_mutex);

	return;
      }

      if(lv2_preset->lv2_plugin != NULL){
	g_object_unref(lv2_preset->lv2_plugin);
      }

      if(lv2_plugin != NULL){
	g_object_ref(lv2_plugin);
      }
      
      lv2_preset->lv2_plugin = lv2_plugin;

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;

      uri = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->uri == uri){
	pthread_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->uri != NULL){
	g_free(lv2_preset->uri);
      }

      lv2_preset->uri = g_strdup(uri);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_BANK:
    {
      gchar *bank;

      bank = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->bank == bank){
	pthread_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->bank != NULL){
	g_free(lv2_preset->bank);
      }

      lv2_preset->bank = g_strdup(bank);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_PRESET_LABEL:
    {
      gchar *preset_label;

      preset_label = (gchar *) g_value_get_string(value);

      pthread_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->preset_label == preset_label){
	pthread_mutex_unlock(lv2_preset_mutex);

	return;
      }
      
      if(lv2_preset->preset_label != NULL){
	g_free(lv2_preset->preset_label);
      }

      lv2_preset->preset_label = g_strdup(preset_label);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      AgsTurtle *manifest;

      manifest = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->manifest == manifest){
	pthread_mutex_unlock(lv2_preset_mutex);

	return;
      }

      if(lv2_preset->manifest != NULL){
	g_object_unref(lv2_preset->manifest);
      }

      if(manifest != NULL){
	g_object_ref(manifest);
      }
      
      lv2_preset->manifest = manifest;

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(lv2_preset_mutex);

      if(lv2_preset->turtle == turtle){
	pthread_mutex_unlock(lv2_preset_mutex);

	return;
      }

      if(lv2_preset->turtle != NULL){
	g_object_unref(lv2_preset->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }
      
      lv2_preset->turtle = turtle;

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Preset *lv2_preset;

  pthread_mutex_t *lv2_preset_mutex;

  lv2_preset = AGS_LV2_PRESET(gobject);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_lv2_preset_get_class_mutex());
  
  lv2_preset_mutex = lv2_preset->obj_mutex;
  
  pthread_mutex_unlock(ags_lv2_preset_get_class_mutex());

  switch(prop_id){
  case PROP_LV2_PLUGIN:
    {
      pthread_mutex_lock(lv2_preset_mutex);

      g_value_set_object(value, lv2_preset->lv2_plugin);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_URI:
    {
      pthread_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->uri);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_BANK:
    {
      pthread_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->bank);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_PRESET_LABEL:
    {
      pthread_mutex_lock(lv2_preset_mutex);

      g_value_set_string(value, lv2_preset->preset_label);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_MANIFEST:
    {
      pthread_mutex_lock(lv2_preset_mutex);

      g_value_set_object(value, lv2_preset->manifest);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_TURTLE:
    {
      pthread_mutex_lock(lv2_preset_mutex);

      g_value_set_object(value, lv2_preset->turtle);

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  case PROP_PORT_PRESET:
    {
      pthread_mutex_lock(lv2_preset_mutex);
      
      g_value_set_pointer(value, g_list_copy(lv2_preset->port_preset));

      pthread_mutex_unlock(lv2_preset_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_preset_finalize(GObject *gobject)
{
  AgsLv2Preset *lv2_preset;

  lv2_preset = AGS_LV2_PRESET(gobject);

  /* lv2 plugin */
  if(lv2_preset->lv2_plugin != NULL){
    g_object_unref(lv2_preset->lv2_plugin);
  }

  /* uri */
  if(lv2_preset->uri != NULL){
    free(lv2_preset->uri);
  }

  /* bank and preset label */
  if(lv2_preset->bank != NULL){
    free(lv2_preset->bank);
  }
  
  if(lv2_preset->preset_label != NULL){
    free(lv2_preset->preset_label);
  }

  /* turtle */
  if(lv2_preset->turtle != NULL){
    g_object_unref(lv2_preset->turtle);
  }

  /* port preset */
  if(lv2_preset->port_preset != NULL){
    g_list_free_full(lv2_preset->port_preset,
		     (GDestroyNotify) ags_lv2_port_preset_free);
  }

  /* destroy object mutex */
  pthread_mutex_destroy(lv2_preset->obj_mutex);
  free(lv2_preset->obj_mutex);

  pthread_mutexattr_destroy(lv2_preset->obj_mutexattr);
  free(lv2_preset->obj_mutexattr);
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_preset_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_preset_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_lv2_preset_get_class_mutex()
{
  return(&ags_lv2_preset_class_mutex);
}

/**
 * ags_lv2_port_preset_alloc:
 * @port_symbol: the port symbol
 * @port_type: the port type
 * 
 * Allocated #AgsLv2PortPreset-struct.
 * 
 * Returns: the new #AgsLv2PortPreset-struct.
 * 
 * Since: 2.0.0
 */
AgsLv2PortPreset*
ags_lv2_port_preset_alloc(gchar *port_symbol,
			  GType port_type)
{
  AgsLv2PortPreset *lv2_port_preset;

  lv2_port_preset = (AgsLv2PortPreset *) malloc(sizeof(AgsLv2PortPreset));

  lv2_port_preset->port_symbol = port_symbol;

  lv2_port_preset->port_value = g_new0(GValue,
				       1);
  
  if(port_type != G_TYPE_NONE){
    g_value_init(lv2_port_preset->port_value,
		 port_type);
  }

  return(lv2_port_preset);
}

/**
 * ags_lv2_port_preset_free:
 * @lv2_port_preset: the #AgsLv2PortPreset-struct
 * 
 * Free @lv2_port_preset.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_port_preset_free(AgsLv2PortPreset *lv2_port_preset)
{
  if(lv2_port_preset == NULL){
    return;
  }
  
  if(lv2_port_preset->port_symbol != NULL){
    free(lv2_port_preset->port_symbol);
  }
  
  if(lv2_port_preset->port_value != NULL){
    g_value_unset(lv2_port_preset->port_value);

    free(lv2_port_preset->port_value);
  }
  
  free(lv2_port_preset);
}

/**
 * ags_lv2_preset_parse_turtle:
 * @lv2_preset: the #AgsLv2Preset
 * 
 * Parse @lv2_preset.
 * 
 * Since: 2.0.0
 */
void
ags_lv2_preset_parse_turtle(AgsLv2Preset *lv2_preset)
{
  AgsLv2PortPreset *lv2_port_preset;

  AgsTurtle *turtle;
  
  xmlNode *triple_node;
  xmlNode *port_node;
  xmlNode *current;
  
  GList *label_list;
  GList *bank_list;
  GList *port_list, *list;

  gchar *uri;
  
  gchar *str;
  gchar *xpath;
  
  if(!AGS_LV2_PRESET(lv2_preset)){
    return;
  }
  
  g_object_get(lv2_preset,
	       "turtle", &turtle,
	       "uri", &uri,
	       NULL);
    

  if(turtle == NULL){
    return;
  }
  
  if(uri == NULL){
    g_object_unref(turtle);
    
    return;
  }
  
  /* retrieve triple by uri */
  xpath = g_strdup_printf("(//rdf-triple/rdf-subject/rdf-iri/rdf-iriref[text() = '%s'])/ancestor::*[self::rdf-triple][1]",
			  uri);
    
  list = ags_turtle_find_xpath(turtle,
			       xpath);

  free(xpath);

  if(list != NULL){
    triple_node = (xmlNode *) list->data;

    g_list_free(list);
  }else{
    xpath = g_strdup_printf("//rdf-triple//rdf-iriref[text() = '<%s>']/ancestor::*[self::rdf-triple][1]",
			    lv2_preset->uri);
    
    list = ags_turtle_find_xpath(turtle,
				 xpath);

    free(xpath);
    
    if(list != NULL){
      triple_node = (xmlNode *) list->data;

      g_list_free(list);
    }else{
      g_warning("rdf-triple not found");
      
      g_object_unref(turtle);
      g_free(uri);
      
      return;
    }
  }

  /* preset label */
  xpath = ".//rdf-pname-ln[text()='rdfs:label']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string";
  label_list = ags_turtle_find_xpath_with_context_node(turtle,
						       xpath,
						       triple_node);

  if(label_list != NULL){
    lv2_preset->preset_label = xmlNodeGetContent((xmlNode *) label_list->data);

    g_list_free(label_list);
  }

  /* bank */
  xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':bank') + 1) = ':bank']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-iriref";
  bank_list = ags_turtle_find_xpath_with_context_node(turtle,
						      xpath,
						      triple_node);

  if(bank_list != NULL){
    str = xmlNodeGetContent((xmlNode *) bank_list->data);

    lv2_preset->bank = g_strndup(&(str[1]),
				 strlen(str) - 2);

    g_list_free(bank_list);
  }

  /* load ports */
  xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':port') + 1) = ':port']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list]/rdf-object";
  port_list = ags_turtle_find_xpath_with_context_node(turtle,
						      xpath,
						      triple_node);

  while(port_list != NULL){
    lv2_port_preset = ags_lv2_port_preset_alloc(NULL,
						G_TYPE_FLOAT);
    lv2_preset->port_preset = g_list_prepend(lv2_preset->port_preset,
					     lv2_port_preset);
    port_node = port_list->data;
    
    /* load symbol */
    xpath = g_ascii_strdown(".//rdf-object-list//rdf-string[ancestor::*[self::rdf-object-list][1]/preceding-sibling::*[self::rdf-verb][1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':symbol') + 1) = ':symbol']]",
			    -1);
    list = ags_turtle_find_xpath_with_context_node(turtle,
						   xpath,
						   port_node);

    if(list != NULL){
      gchar *str;
	
      current = (xmlNode *) list->data;
      str = xmlNodeGetContent(current);

      if(strlen(str) > 2){
	lv2_port_preset->port_symbol = g_strndup(str + 1,
						 strlen(str) - 2);
      }

      g_list_free(list);
    }

    /* port value */
    xpath = g_ascii_strdown(".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':value') + 1) = ':value']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric",
			    -1);
    list = ags_turtle_find_xpath_with_context_node(turtle,
						   xpath,
						   port_node);

    if(list != NULL){
      current = (xmlNode *) list->data;

      g_value_set_float(lv2_port_preset->port_value,
			g_ascii_strtod(xmlNodeGetContent(current),
				       NULL));

      g_list_free(list);
    }

    /* iterate */
    port_list = port_list->next;
  }

  lv2_preset->port_preset = g_list_reverse(lv2_preset->port_preset);
  
  if(port_list != NULL){
    g_list_free(port_list);
  }

  g_object_unref(turtle);
  g_free(uri);
}

/**
 * ags_lv2_preset_find_preset_uri:
 * @lv2_preset: the #GList-struct containing #AgsLv2Preset
 * @preset_uri: the preset URI
 * 
 * Find @preset_uri within @lv2_preset.
 * 
 * Returns: the matching #GList-struct containing #AgsLv2Preset
 * 
 * Since: 2.2.0
 */
GList*
ags_lv2_preset_find_preset_uri(GList *lv2_preset,
			       gchar *preset_uri)
{
  if(preset_uri == NULL){
    return(NULL);
  }

  while(lv2_preset != NULL){
    if(!g_strcmp0(preset_uri,
		  AGS_LV2_PRESET(lv2_preset->data)->uri)){
      return(lv2_preset);
    }

    lv2_preset = lv2_preset->next;
  }
  
  return(NULL);
}

/**
 * ags_lv2_preset_find_preset_label:
 * @lv2_preset: the #GList-struct containing #AgsLv2Preset
 * @preset_label: the preset label
 * 
 * Find @preset_label within @lv2_preset.
 * 
 * Returns: the matching #GList-struct containing #AgsLv2Preset
 * 
 * Since: 2.0.0
 */
GList*
ags_lv2_preset_find_preset_label(GList *lv2_preset,
				 gchar *preset_label)
{
  if(preset_label == NULL){
    return(NULL);
  }

  while(lv2_preset != NULL){
    if(!g_strcmp0(preset_label,
		  AGS_LV2_PRESET(lv2_preset->data)->preset_label)){
      return(lv2_preset);
    }

    lv2_preset = lv2_preset->next;
  }
  
  return(NULL);
}

/**
 * ags_lv2_preset_new:
 * @lv2_plugin: an #AgsLv2Plugin
 * @turtle: the #AgsTurtle
 * @uri: the URI as string
 * 
 * Create a new instance of #AgsLv2Preset.
 *
 * Returns: the new #AgsLv2Preset
 *
 * Since: 2.0.0
 */ 
AgsLv2Preset*
ags_lv2_preset_new(GObject *lv2_plugin,
		   AgsTurtle *turtle,
		   gchar *uri)
{
  AgsLv2Preset *lv2_preset;

  lv2_preset = (AgsLv2Preset *) g_object_new(AGS_TYPE_LV2_PRESET,
					     "lv2-plugin", lv2_plugin,
					     "turtle", turtle,
					     "uri", uri,
					     NULL);

  return(lv2_preset);
}
