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

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/lib/ags_string_util.h>

#include <stdarg.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_lv2_plugin_class_init(AgsLv2PluginClass *lv2_plugin);
void ags_lv2_plugin_init (AgsLv2Plugin *lv2_plugin);
void ags_lv2_plugin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_plugin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_lv2_plugin_finalize(GObject *gobject);

gpointer ags_lv2_plugin_instantiate(AgsBasePlugin *base_plugin,
				    guint samplerate);
void ags_lv2_plugin_connect_port(AgsBasePlugin *base_plugin,
				 gpointer plugin_handle,
				 guint port_index,
				 gpointer data_location);
void ags_lv2_plugin_activate(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle);
void ags_lv2_plugin_deactivate(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle);
void ags_lv2_plugin_run(AgsBasePlugin *base_plugin,
			gpointer plugin_handle,
			snd_seq_event_t *seq_event,
			guint frame_count);
void ags_lv2_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_lv2_plugin
 * @short_description: The lv2 plugin class
 * @title: AgsLv2Plugin
 * @section_id:
 * @include: ags/object/ags_lv2_plugin.h
 *
 * The #AgsLv2Plugin loads/unloads a Lv2 plugin.
 */

enum{
  PROP_0,
  PROP_URI,
  PROP_TURTLE,
};

static gpointer ags_lv2_plugin_parent_class = NULL;

GType
ags_lv2_plugin_get_type (void)
{
  static GType ags_type_lv2_plugin = 0;

  if(!ags_type_lv2_plugin){
    static const GTypeInfo ags_lv2_plugin_info = {
      sizeof (AgsLv2PluginClass),
      NULL, /* lv2_init */
      NULL, /* lv2_finalize */
      (GClassInitFunc) ags_lv2_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2Plugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_plugin_init,
    };

    ags_type_lv2_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						 "AgsLv2Plugin\0",
						 &ags_lv2_plugin_info,
						 0);
  }

  return (ags_type_lv2_plugin);
}

void
ags_lv2_plugin_class_init(AgsLv2PluginClass *lv2_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_lv2_plugin_parent_class = g_type_class_peek_parent(lv2_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_plugin;

  gobject->set_property = ags_lv2_plugin_set_property;
  gobject->get_property = ags_lv2_plugin_get_property;

  gobject->finalize = ags_lv2_plugin_finalize;


  /**
   * AgsLv2Plugin:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_object("turtle\0",
				   "turtle of the plugin\0",
				   "The turtle this plugin is located in\0",
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsLv2Plugin:uri:
   *
   * The assigned uri.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("uri\0",
				   "uri of the plugin\0",
				   "The uri this plugin is located in\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  
  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) lv2_plugin;

  base_plugin->instantiate = ags_lv2_plugin_instantiate;

  base_plugin->connect_port = ags_lv2_plugin_connect_port;

  base_plugin->activate = ags_lv2_plugin_activate;
  base_plugin->deactivate = ags_lv2_plugin_deactivate;

  base_plugin->run = ags_lv2_plugin_run;

  base_plugin->load_plugin = ags_lv2_plugin_load_plugin;
}

void
ags_lv2_plugin_init(AgsLv2Plugin *lv2_plugin)
{
  lv2_plugin->flags = 0;
}

void
ags_lv2_plugin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);

  switch(prop_id){
  case PROP_URI:
    {
      gchar *uri;

      uri = (gchar *) g_value_get_string(value);

      if(lv2_plugin->uri == uri){
	return;
      }
      
      if(lv2_plugin->uri != NULL){
	g_free(lv2_plugin->uri);
      }

      lv2_plugin->uri = g_strdup(uri);
    }
    break;
  case PROP_TURTLE:
    {
      GObject *turtle;

      turtle = g_value_get_object(value);

      if(lv2_plugin->turtle == turtle){
	return;
      }

      if(lv2_plugin->turtle != NULL){
	g_object_unref(lv2_plugin->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }
      
      lv2_plugin->turtle = turtle;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_plugin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);

  switch(prop_id){
  case PROP_URI:
    g_value_set_string(value, lv2_plugin->uri);
    break;
  case PROP_TURTLE:
    g_value_set_object(value, lv2_plugin->turtle);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_lv2_plugin_finalize(GObject *gobject)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = AGS_LV2_PLUGIN(gobject);
}

gpointer
ags_lv2_plugin_instantiate(AgsBasePlugin *base_plugin,
			   guint samplerate)
{
  gpointer lv2_handle;
  
  char *path;
  
  static const LV2_Feature **feature = {
    NULL,
  };

  path = g_strndup(base_plugin->filename,
		   rindex(base_plugin->filename, '/') - base_plugin->filename + 1);
  
  lv2_handle = AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->instantiate(AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor),
										      (double) samplerate,
										      path,
										      feature);

  return(lv2_handle);
}

void
ags_lv2_plugin_connect_port(AgsBasePlugin *base_plugin,
			    gpointer plugin_handle,
			    guint port_index,
			    gpointer data_location)
{
  AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->connect_port((LV2_Handle) plugin_handle,
									  (uint32_t) port_index,
									  (float *) data_location);
}

void
ags_lv2_plugin_activate(AgsBasePlugin *base_plugin,
			gpointer plugin_handle)
{
  if(AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->activate != NULL){
    AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->activate((LV2_Handle) plugin_handle);
  }
}

void
ags_lv2_plugin_deactivate(AgsBasePlugin *base_plugin,
			  gpointer plugin_handle)
{
  if(AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->deactivate != NULL){
    AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->deactivate((LV2_Handle) plugin_handle);
  }
}

void
ags_lv2_plugin_run(AgsBasePlugin *base_plugin,
		   gpointer plugin_handle,
		   snd_seq_event_t *seq_event,
		   guint frame_count)
{
  AGS_LV2_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->run((LV2_Handle) plugin_handle,
								 frame_count);
}

void
ags_lv2_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsLv2Plugin *lv2_plugin;
  
  AgsPortDescriptor *port;

  GList *port_list;
  GList *port_descriptor_list;

  gchar *escaped_effect;
  gchar *xpath;
  gchar *uri;

  guint i;
  
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  uint32_t effect_index;

  lv2_plugin = AGS_LV2_PLUGIN(base_plugin);  
  base_plugin->plugin_so = dlopen(base_plugin->filename,
				  RTLD_NOW);
  
  if(base_plugin->plugin_so == NULL){
    g_warning("ags_lv2_plugin.c - failed to load static object file\0");
    
    dlerror();

    return;
  }

  lv2_descriptor = (LV2_Descriptor_Function) dlsym(base_plugin->plugin_so,
						   "lv2_descriptor\0");
  
  if(dlerror() == NULL && lv2_descriptor){
    xmlNode *port_node;
    xmlNode *current;

    GList *list;
    
    /* load ports */
    escaped_effect = ags_string_util_escape_single_quote(base_plugin->effect);
    xpath = "//rdf-triple//rdf-verb[//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name'] and following-sibling::*//rdf-string[text()='\"%s\"']]/ancestor::*[self::rdf-triple]//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':port') + 1) = ':port']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list]/rdf-object\0";
    xpath = g_strdup_printf(xpath,
			    escaped_effect);
    port_list = ags_turtle_find_xpath(lv2_plugin->turtle,
				      xpath);
    free(xpath);

    port_descriptor_list = NULL;

    while(port_list != NULL){
      port = ags_port_descriptor_alloc();
      port_descriptor_list = g_list_prepend(port_descriptor_list,
					    port);
      
      port_node = (xmlNode *) port_list->data;

      /* load flags - control */
      xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':ControlPort') + 1) = ':ControlPort']\0";

      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	port->flags |= AGS_PORT_DESCRIPTOR_CONTROL;
      }

      /* load flags - output */
      xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':OutputPort') + 1) = ':OutputPort']\0";

      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	port->flags |= AGS_PORT_DESCRIPTOR_OUTPUT;
      }

      /* load flags - input */
      xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':InputPort') + 1) = ':InputPort']\0";

      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	port->flags |= AGS_PORT_DESCRIPTOR_INPUT;
      }

      /* load flags - audio */
      xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':AudioPort') + 1) = ':AudioPort']\0";

      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	port->flags |= AGS_PORT_DESCRIPTOR_AUDIO;
      }

      /* load flags - event */
      xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':EventPort') + 1) = ':EventPort']\0";

      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	port->flags |= AGS_PORT_DESCRIPTOR_EVENT;
      }

      /* load flags - atom */
      xpath = ".//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':AtomPort') + 1) = ':AtomPort']\0";

      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	port->flags |= AGS_PORT_DESCRIPTOR_ATOM;
      }

      /* load index */
      xpath = ".//rdf-object-list//rdf-numeric[ancestor::*[self::rdf-object-list][1]/preceding-sibling::*[self::rdf-verb][1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':index') + 1) = ':index']]\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current = (xmlNode *) list->data;

	port->port_index = g_ascii_strtoull(xmlNodeGetContent(current),
					    NULL,
					    10);
      }

      /* properties */
      xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':portProperty') + 1) = ':portProperty']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-pname-ln\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      while(list != NULL){
	current = (xmlNode *) list->data;
	
	if(g_str_has_suffix(xmlNodeGetContent(current),
			    ":toggled\0")){
	  port->flags |= AGS_PORT_DESCRIPTOR_TOGGLED;
	}else if(g_str_has_suffix(xmlNodeGetContent(current),
				  ":enumeration\0")){
	  port->flags |= AGS_PORT_DESCRIPTOR_ENUMERATION;
	}

	list = list->next;
      }
      
      /* load name */
      xpath = ".//rdf-object-list//rdf-string[ancestor::*[self::rdf-object-list][1]/preceding-sibling::*[self::rdf-verb][1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name']]\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	gchar *str;
	
	current = (xmlNode *) list->data;
	str = xmlNodeGetContent(current);

	if(strlen(str) > 2){
	  port->port_name = g_strndup(str + 1,
				      strlen(str) - 2);
	}
      }
      
      /* load symbol */
      xpath = ".//rdf-object-list//rdf-string[ancestor::*[self::rdf-object-list][1]/preceding-sibling::*[self::rdf-verb][1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':symbol') + 1) = ':symbol']]\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	gchar *str;
	
	current = (xmlNode *) list->data;
	str = xmlNodeGetContent(current);

	if(strlen(str) > 2){
	  port->port_symbol = g_strndup(str + 1,
					strlen(str) - 2);
	}
      }

      /* load scale points and value */
      xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':scalePoint') + 1) = ':scalePoint']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-string\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      port->scale_steps = g_list_length(list);
      
      port->scale_points = malloc(port->scale_steps * sizeof(gchar *));
      port->scale_value = malloc(port->scale_steps * sizeof(float));
      
      for(i = 0; list != NULL; i++){
	gchar *str;
	
	current = (xmlNode *) list->data;
	str = xmlNodeGetContent(current);
	
	if(strlen(str) > 2){
	  port->scale_points[i] = g_strndup(str + 1,
					    strlen(str) - 2);
	}

	list = list->next;
      }

      xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':scalePoint') + 1) = ':scalePoint']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      for(i = 0; list != NULL; i++){
	gchar *str;
	
	current = (xmlNode *) list->data;
	str = xmlNodeGetContent(current);
	
	port->scale_value[i] = g_ascii_strtod(str,
					      NULL);
	
	list = list->next;
      }

      /* minimum */
      xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':minimum') + 1) = ':minimum']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current = (xmlNode *) list->data;

	g_value_init(port->lower_value,
		     G_TYPE_FLOAT);
	g_value_set_float(port->lower_value,
			  g_ascii_strtod(xmlNodeGetContent(current),
					 NULL));
      }

      /* maximum */
      xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':maximum') + 1) = ':maximum']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current = (xmlNode *) list->data;

	g_value_init(port->upper_value,
		     G_TYPE_FLOAT);
	g_value_set_float(port->upper_value,
			  g_ascii_strtod(xmlNodeGetContent(current),
					 NULL));
      }

      /* default */
      xpath = ".//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':default') + 1) = ':default']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric\0";
      list = ags_turtle_find_xpath_with_context_node(lv2_plugin->turtle,
						     xpath,
						     port_node);

      if(list != NULL){
	current = (xmlNode *) list->data;

	g_value_init(port->default_value,
		     G_TYPE_FLOAT);
	g_value_set_float(port->default_value,
			  g_ascii_strtod(xmlNodeGetContent(current),
					 NULL));
      }
      
      port_list = port_list->next;
    }

    base_plugin->port = g_list_reverse(port_descriptor_list);
  }
}

/**
 * ags_lv2_plugin_alloc_event_buffer:
 * @buffer_size: the allocated size
 *
 * Allocates a LV2_Event_Buffer
 * 
 * Returns: the new event buffer
 * 
 * Since: 0.7.7
 */
void*
ags_lv2_plugin_alloc_event_buffer(guint buffer_size)
{
  void *event_buffer, *iter;
  
  guint buffer_count;
  guint padded_buffer_size;
  
  buffer_count = (guint) ceil(buffer_size / G_MAXUINT16);

  if(buffer_count == 1){
    if(buffer_size < 8){
      padded_buffer_size = 8;
    }else{
      padded_buffer_size = buffer_size;
    }
    
    event_buffer = (void *) malloc(padded_buffer_size + sizeof(LV2_Event));

    memset(event_buffer, 0, buffer_size + sizeof(LV2_Event));
    AGS_LV2_EVENT_BUFFER(event_buffer)->size = buffer_size;
  }else{
    guint i;

    if(buffer_size % G_MAXUINT16 < 8){
      padded_buffer_size = 8;
    }else{
      padded_buffer_size = buffer_size % G_MAXUINT16;
    }

    iter = 
      event_buffer = (void *) malloc(((buffer_count - 1) * G_MAXUINT16 * sizeof(LV2_Event))  + (padded_buffer_size) + sizeof(LV2_Event));
    memset(event_buffer, 0, ((buffer_count - 1) * G_MAXUINT16 * sizeof(LV2_Event))  + (buffer_size % G_MAXUINT16) + sizeof(LV2_Event));

    for(i = 1; i < buffer_count; i++){
      AGS_LV2_EVENT_BUFFER(iter)->size = G_MAXUINT16;

      iter += (G_MAXUINT16 + sizeof(LV2_Event));
    }

    AGS_LV2_EVENT_BUFFER(iter)->size = buffer_size % G_MAXUINT16;
  }

  return(event_buffer);
}

/**
 * ags_lv2_plugin_concat_event_buffer:
 * @buffer0: the first buffer
 * @...: %NULL terminated variadict arguments
 *
 * Concats the event buffers.
 * 
 * Returns: The newly allocated event buffer
 * 
 * Since: 0.7.7
 */
void*
ags_lv2_plugin_concat_event_buffer(void *buffer0, ...)
{
  void *buffer;
  void *current;
  
  va_list ap;

  guint buffer_length, prev_length;

  buffer_length = AGS_LV2_EVENT_BUFFER(buffer0)->size + sizeof(LV2_Event);

  buffer = (void *) malloc(buffer_length);
  memcpy(buffer, buffer0, buffer_length);
  
  va_start(ap, buffer0);

  while((current = va_arg(ap, void *)) != NULL){
    prev_length = buffer_length;
    buffer_length += (AGS_LV2_EVENT_BUFFER(current)->size + sizeof(LV2_Event));
    
    buffer = (void *) realloc(buffer,
			      buffer_length);
    memcpy(buffer + prev_length, current, buffer_length - prev_length);
  }

  va_end(ap);

  return(buffer);
}

gboolean
ags_lv2_plugin_event_buffer_append_midi(void *event_buffer,
					guint buffer_size,
					snd_seq_event_t *events,
					guint event_count)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me
  
  return(success);
}

void
ags_lv2_plugin_clear_event_buffer(void *event_buffer,
				  guint buffer_size)
{
  void *offset;

  guint padded_buffer_size;

  while(offset < event_buffer + buffer_size){
    if(AGS_LV2_EVENT_BUFFER(offset)->size < 8){
      padded_buffer_size = 8;
    }else{
      padded_buffer_size = AGS_LV2_EVENT_BUFFER(offset)->size;
    }
    
    memset(offset + sizeof(LV2_Event), 0, padded_buffer_size);

    offset += (padded_buffer_size + sizeof(LV2_Event));
  }
}

/**
 * ags_lv2_plugin_alloc_atom_sequence:
 * @sequence_size: the requested size
 *
 * Allocates a LV2_Atom_Sequence
 * 
 * Returns: the new atom sequence
 * 
 * Since: 0.7.7
 */
void*
ags_lv2_plugin_alloc_atom_sequence(guint sequence_size)
{
  void *atom_sequence, *current;
  
  guint padded_sequence_size;
  guint sequence_count;
  guint i;
  
  sequence_count = ceil(sequence_size / 8.0);
  padded_sequence_size = (guint) 8 * sequence_count;

  current = 
    atom_sequence = (void *) malloc(padded_sequence_size + sequence_count * sizeof(LV2_Atom_Sequence));

  for(i = 0; i < sequence_count; i++){
    AGS_LV2_ATOM_SEQUENCE(current)->atom.size = 0;
    AGS_LV2_ATOM_SEQUENCE(current)->atom.type = 0;
    
    AGS_LV2_ATOM_SEQUENCE(current)->body.unit = 0;
    AGS_LV2_ATOM_SEQUENCE(current)->body.pad = 64;
    
    current += (8 + sizeof(LV2_Atom_Sequence));
  }
  
  return(atom_sequence);
}

/**
 * ags_lv2_plugin_concat_atom_sequence:
 * @sequence0: the fist atom sequence
 * @...: %NULL terminated variadict argument list
 *
 * Concats LV2_Atom_Sequence
 * 
 * Returns: the newly allocated atom sequence
 * 
 * Since: 0.7.7
 */
void*
ags_lv2_plugin_concat_atom_sequence(void *sequence0, ...)
{
  void *sequence;
  void *current;
  
  va_list ap;

  guint sequence_length, prev_length;
  guint i;

  sequence_length = 8 + sizeof(LV2_Atom_Sequence);

  sequence = (void *) malloc(sequence_length);
  memcpy(sequence, sequence0, sequence_length);
  
  va_start(ap, sequence0);
  i = 1;
  
  while((current = va_arg(ap, void *)) != NULL){
    prev_length = sequence_length;
    sequence_length += ((i + 1) * 8 + sizeof(LV2_Event));
    
    sequence = (void *) realloc(sequence,
				sequence_length);
    memcpy(sequence + prev_length, current, sequence_length - prev_length);

    i++;
  }

  va_end(ap);

  return(sequence);
}

gboolean
ags_lv2_plugin_atom_sequence_append_midi(void *atom_sequence,
					 guint sequence_size,
					 snd_seq_event_t *events,
					 guint event_count)
{
  gboolean success;

  success = FALSE;

  //TODO:JK: implement me
  
  return(success);
}

void
ags_lv2_plugin_clear_atom_sequence(void *atom_sequence,
				   guint sequence_size)
{
  //TODO:JK: implement me
}

/**
 * ags_lv2_plugin_new:
 * @turtle: the turtle
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @uri: the effect's uri
 * @effect_index: the effect's index
 *
 * Creates an #AgsLv2Plugin
 *
 * Returns: a new #AgsLv2Plugin
 *
 * Since: 0.7.6
 */
AgsLv2Plugin*
ags_lv2_plugin_new(AgsTurtle *turtle, gchar *filename, gchar *effect, gchar *uri, guint effect_index)
{
  AgsLv2Plugin *lv2_plugin;

  lv2_plugin = (AgsLv2Plugin *) g_object_new(AGS_TYPE_LV2_PLUGIN,
					     "turtle\0", turtle,
					     "filename\0", filename,
					     "effect\0", effect,
					     "uri\0", uri,
					     "effect-index\0", effect_index,
					     NULL);

  return(lv2_plugin);
}
