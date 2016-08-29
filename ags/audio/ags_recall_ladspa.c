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

#include <ags/audio/ags_recall_ladspa.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

void ags_recall_ladspa_class_init(AgsRecallLadspaClass *recall_ladspa_class);
void ags_recall_ladspa_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_ladspa_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa);
void ags_recall_ladspa_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_recall_ladspa_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_recall_ladspa_connect(AgsConnectable *connectable);
void ags_recall_ladspa_disconnect(AgsConnectable *connectable);
void ags_recall_ladspa_set_ports(AgsPlugin *plugin, GList *port);
void ags_recall_ladspa_finalize(GObject *gobject);

void ags_recall_ladspa_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_recall_ladspa_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_recall_ladspa_load(AgsRecallLadspa *recall_ladspa);

/**
 * SECTION:ags_recall_ladspa
 * @short_description: The object interfacing with LADSPA
 * @title: AgsRecallLadspa
 * @section_id:
 * @include: ags/audio/ags_recall_ladspa.h
 *
 * #AgsRecallLadspa provides LADSPA support.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_recall_ladspa_parent_class = NULL;
static AgsConnectableInterface* ags_recall_ladspa_parent_connectable_interface;
static AgsPluginInterface* ags_recall_ladspa_parent_plugin_interface;

GType
ags_recall_ladspa_get_type (void)
{
  static GType ags_type_recall_ladspa = 0;

  if(!ags_type_recall_ladspa){
    static const GTypeInfo ags_recall_ladspa_info = {
      sizeof (AgsRecallLadspaClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_ladspa_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLadspa),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_ladspa_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_ladspa = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						    "AgsRecallLadspa\0",
						    &ags_recall_ladspa_info,
						    0);

    g_type_add_interface_static(ags_type_recall_ladspa,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_ladspa,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_ladspa);
}

void
ags_recall_ladspa_class_init(AgsRecallLadspaClass *recall_ladspa)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recall_ladspa_parent_class = g_type_class_peek_parent(recall_ladspa);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_ladspa;

  gobject->set_property = ags_recall_ladspa_set_property;
  gobject->get_property = ags_recall_ladspa_get_property;

  gobject->finalize = ags_recall_ladspa_finalize;

  /* properties */
  /**
   * AgsRecallLadspa:filename:
   *
   * The plugins filename.
   * 
   * Since: 0.4.2
   */
  param_spec =  g_param_spec_string("filename\0",
				    "the object file\0",
				    "The filename as string of object file\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsRecallLadspa:recycling:
   *
   * The effect's name.
   * 
   * Since: 0.4.2
   */
  param_spec =  g_param_spec_string("effect\0",
				    "the effect\0",
				    "The effect's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsRecallLadspa:recycling:
   *
   * The effect's index.
   * 
   * Since: 0.4.2
   */
  param_spec =  g_param_spec_ulong("index\0",
				   "index of effect\0",
				   "The numerical index of effect\0",
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);
}


void
ags_recall_ladspa_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_ladspa_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_ladspa_connect;
  connectable->disconnect = ags_recall_ladspa_disconnect;
}

void
ags_recall_ladspa_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_ladspa_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_recall_ladspa_read;
  plugin->write = ags_recall_ladspa_write;
  plugin->set_ports = ags_recall_ladspa_set_ports;
}

void
ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa)
{
  AGS_RECALL(recall_ladspa)->name = "ags-ladspa\0";
  AGS_RECALL(recall_ladspa)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(recall_ladspa)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(recall_ladspa)->xml_type = "ags-recall-ladspa\0";
  AGS_RECALL(recall_ladspa)->port = NULL;

  recall_ladspa->filename = NULL;
  recall_ladspa->effect = NULL;
  recall_ladspa->index = 0;

  recall_ladspa->plugin_descriptor = NULL;

  recall_ladspa->input_port = NULL;
  recall_ladspa->input_lines = 0;

  recall_ladspa->output_port = NULL;
  recall_ladspa->output_lines = 0;
}

void
ags_recall_ladspa_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      GObject *soundcard;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == recall_ladspa->filename){
	return;
      }

      if(recall_ladspa->filename != NULL){
	g_free(recall_ladspa->filename);
      }

      recall_ladspa->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == recall_ladspa->effect){
	return;
      }

      recall_ladspa->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long index;
      
      index = g_value_get_ulong(value);

      if(index == recall_ladspa->index){
	return;
      }

      recall_ladspa->index = index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_ladspa_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, recall_ladspa->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, recall_ladspa->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_uint(value, recall_ladspa->index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_ladspa_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;

  recall = AGS_RECALL(connectable);
  
  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    return;
  }

  ags_recall_load_automation(recall,
			     g_list_copy(recall->port));
  
  ags_recall_ladspa_parent_connectable_interface->connect(connectable);
}

void
ags_recall_ladspa_disconnect(AgsConnectable *connectable)
{
  ags_recall_ladspa_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_ladspa_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsRecallLadspa *recall_ladspa;
  AgsPort *current;
  
  AgsLadspaPlugin *ladspa_plugin;
  
  GList *list;  
  GList *port_descriptor;
  
  unsigned long port_count;
  unsigned long i;

  recall_ladspa = AGS_RECALL_LADSPA(plugin);

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(recall_ladspa->filename, recall_ladspa->effect);

  port_descriptor = AGS_BASE_PLUGIN(ladspa_plugin)->port;

  if(port_descriptor != NULL){
    port_count = g_list_length(port_descriptor);

    for(i = 0; i < port_count; i++){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	gchar *plugin_name;
	gchar *specifier;
	
	plugin_name = g_strdup_printf("ladspa-%lu\0", ladspa_plugin->unique_id);
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;
	
	list = port;
	current = NULL;
	
	while(list != NULL){
	  if(!g_strcmp0(specifier,
			AGS_PORT(list->data)->specifier)){
	    current = list->data;
	    break;
	  }
	  
	  list = list->next;
	}

	current->flags |= AGS_PORT_USE_LADSPA_FLOAT;
	
	current->port_descriptor = port_descriptor->data;
	ags_recall_ladspa_load_conversion(recall_ladspa,
					  current,
					  port_descriptor->data);
	
	current->port_value.ags_port_float = (LADSPA_Data) g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value);
	
	g_message("connecting port: %d/%d\0", i, port_count);      
      }else if((AGS_PORT_DESCRIPTOR_AUDIO & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(recall_ladspa->input_port == NULL){
	    recall_ladspa->input_port = (unsigned long *) malloc(sizeof(unsigned long));
	    recall_ladspa->input_port[0] = i;
	  }else{
	    recall_ladspa->input_port = (unsigned long *) realloc(recall_ladspa->input_port,
								  (recall_ladspa->input_lines + 1) * sizeof(unsigned long));
	    recall_ladspa->input_port[recall_ladspa->input_lines] = i;
	  }

	  recall_ladspa->input_lines += 1;
	}else if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(recall_ladspa->output_port == NULL){
	    recall_ladspa->output_port = (unsigned long *) malloc(sizeof(unsigned long));
	    recall_ladspa->output_port[0] = i;
	  }else{
	    recall_ladspa->output_port = (unsigned long *) realloc(recall_ladspa->output_port,
								   (recall_ladspa->output_lines + 1) * sizeof(unsigned long));
	    recall_ladspa->output_port[recall_ladspa->output_lines] = i;
	  }

	  recall_ladspa->output_lines += 1;
	}
      }

      port_descriptor = port_descriptor->next;
    }

    AGS_RECALL(recall_ladspa)->port = g_list_reverse(port);
  }
}

void
ags_recall_ladspa_finalize(GObject *gobject)
{
  AgsRecallLadspa *recall_ladspa;
  
  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_parent_class)->finalize(gobject);
}

void
ags_recall_ladspa_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsRecallLadspa *gobject;
  AgsLadspaPlugin *ladspa_plugin;
  gchar *filename, *effect;
  unsigned long index;

  gobject = AGS_RECALL_LADSPA(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference\0", gobject,
				   NULL));

  filename = xmlGetProp(node,
			"filename\0");
  effect = xmlGetProp(node,
		      "effect\0");
  index = g_ascii_strtoull(xmlGetProp(node,
				      "index\0"),
			   NULL,
			   10);

  g_object_set(gobject,
	       "filename\0", filename,
	       "effect\0", effect,
	       "index\0", index,
	       NULL);

  ags_recall_ladspa_load(gobject);
}

xmlNode*
ags_recall_ladspa_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsRecallLadspa *recall_ladspa;
  xmlNode *node;
  gchar *id;

  recall_ladspa = AGS_RECALL_LADSPA(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-ladspa\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_ladspa,
				   NULL));

  xmlNewProp(node,
	     "filename\0",
	     g_strdup(recall_ladspa->filename));

  xmlNewProp(node,
	     "effect\0",
	     g_strdup(recall_ladspa->effect));

  xmlNewProp(node,
	     "index\0",
	     g_strdup_printf("%d\0", recall_ladspa->index));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_recall_ladspa_load:
 * @recall_ladspa: an #AgsRecallLadspa
 *
 * Set up LADSPA handle.
 * 
 * Since: 0.4
 */
void
ags_recall_ladspa_load(AgsRecallLadspa *recall_ladspa)
{
  AgsLadspaPlugin *ladspa_plugin;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;

  /*  */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(recall_ladspa->filename, recall_ladspa->effect);
  
  plugin_so = AGS_BASE_PLUGIN(ladspa_plugin)->plugin_so;

  if(plugin_so){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      recall_ladspa->plugin_descriptor = 
	plugin_descriptor = ladspa_descriptor(recall_ladspa->index);
    }
  }
}

/**
 * ags_recall_ladspa_load_ports:
 * @recall_ladspa: an #AgsRecallLadspa
 *
 * Set up LADSPA ports.
 *
 * Returns: a #GList containing #AgsPort.
 * 
 * Since: 0.4
 */
GList*
ags_recall_ladspa_load_ports(AgsRecallLadspa *recall_ladspa)
{
  AgsLadspaPlugin *ladspa_plugin;
  AgsPort *current;

  GList *port;
  GList *port_descriptor;

  unsigned long port_count;
  unsigned long i;

  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(recall_ladspa->filename, recall_ladspa->effect);

  port = NULL;
  port_descriptor = AGS_BASE_PLUGIN(ladspa_plugin)->port;
  
  if(port_descriptor != NULL){
    port_count = g_list_length(port_descriptor);
    
    for(i = 0; i < port_count; i++){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	AgsLadspaConversion *ladspa_conversion;
	
	gchar *plugin_name;
	gchar *specifier;
	
	plugin_name = g_strdup_printf("ladspa-%lu\0", ladspa_plugin->unique_id);
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;

	current = g_object_new(AGS_TYPE_PORT,
			       "plugin-name\0", plugin_name,
			       "specifier\0", specifier,
			       "control-port\0", g_strdup_printf("%d/%d\0",
								 i,
								 port_count),
			       "port-value-is-pointer\0", FALSE,
			       "port-value-type\0", G_TYPE_FLOAT,
			       NULL);
	current->flags |= AGS_PORT_USE_LADSPA_FLOAT;
	
	current->port_descriptor = port_descriptor->data;	
	ags_recall_ladspa_load_conversion(recall_ladspa,
					  current,
					  port_descriptor->data);

	current->port_value.ags_port_ladspa = (LADSPA_Data) g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value);
	
	//	g_message("connecting port: %d/%d\0", i, port_count);

	port = g_list_prepend(port,
			      current);
      }else if((AGS_PORT_DESCRIPTOR_AUDIO & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(recall_ladspa->input_port == NULL){
	    recall_ladspa->input_port = (unsigned long *) malloc(sizeof(unsigned long));
	    recall_ladspa->input_port[0] = i;
	  }else{
	    recall_ladspa->input_port = (unsigned long *) realloc(recall_ladspa->input_port,
								  (recall_ladspa->input_lines + 1) * sizeof(unsigned long));
	    recall_ladspa->input_port[recall_ladspa->input_lines] = i;
	  }
	  
	  recall_ladspa->input_lines += 1;
	}else if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(recall_ladspa->output_port == NULL){
	    recall_ladspa->output_port = (unsigned long *) malloc(sizeof(unsigned long));
	    recall_ladspa->output_port[0] = i;
	  }else{
	    recall_ladspa->output_port = (unsigned long *) realloc(recall_ladspa->output_port,
								   (recall_ladspa->output_lines + 1) * sizeof(unsigned long));
	    recall_ladspa->output_port[recall_ladspa->output_lines] = i;
	  }
	  
	  recall_ladspa->output_lines += 1;
	}
      }

      port_descriptor = port_descriptor->next;
    }
    
    AGS_RECALL(recall_ladspa)->port = g_list_reverse(port);
  }

  return(AGS_RECALL(recall_ladspa)->port);
}

/**
 * ags_recall_ladspa_load_conversion:
 * @recall_ladspa: the #AgsRecallLadspa
 * @port: an #AgsPort
 * @port_descriptor: the #AgsPortDescriptor-struct
 * 
 * Loads conversion object by using @port_descriptor and sets in on @port.
 * 
 * Since: 0.7.9
 */
void
ags_recall_ladspa_load_conversion(AgsRecallLadspa *recall_ladspa,
				  GObject *port,
				  gpointer port_descriptor)
{
  AgsLadspaConversion *ladspa_conversion;

  if(recall_ladspa == NULL ||
     port == NULL ||
     port_descriptor == NULL){
    return;
  }

  ladspa_conversion = NULL;

  if((AGS_PORT_DESCRIPTOR_BOUNDED_BELOW & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }

    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
  }

  if((AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }

    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
  }
  
  if((AGS_PORT_DESCRIPTOR_SAMPLERATE & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }
        
    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
  }

  if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }
    
    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
  }

  if(ladspa_conversion != NULL){
    g_object_set(port,
		 "conversion\0", ladspa_conversion,
		 NULL);
  }
}

/**
 * ags_recall_ladspa_find:
 * @recall: a #GList containing #AgsRecall
 * @filename: plugin filename
 * @effect: effect's name
 *
 * Retrieve LADSPA recall.
 *
 * Returns: Next match.
 * 
 * Since: 0.4
 */
GList*
ags_recall_ladspa_find(GList *recall,
		       gchar *filename, gchar *effect)
{
  while(recall != NULL){
    if(AGS_IS_RECALL_LADSPA(recall->data)){
      if(!g_strcmp0(AGS_RECALL_LADSPA(recall->data)->filename,
		    filename) &&
	 !g_strcmp0(AGS_RECALL_LADSPA(recall->data)->effect,
		    effect)){
	return(recall);
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_ladspa_new:
 * @source: the source
 * @filename: the LADSPA plugin filename
 * @effect: effect's name
 * @index: effect's index
 *
 * Creates a #AgsRecallLadspa
 *
 * Returns: a new #AgsRecallLadspa
 * 
 * Since: 0.4
 */
AgsRecallLadspa*
ags_recall_ladspa_new(AgsChannel *source,
		      gchar *filename,
		      gchar *effect,
		      unsigned long index)
{
  GObject *soundcard;
  AgsRecallLadspa *recall_ladspa;

  if(source != NULL){
    soundcard = (GObject *) source->soundcard;
  }else{
    soundcard = NULL;
  }

  recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
						   "soundcard\0", soundcard,
						   "source\0", source,
						   "filename\0", filename,
						   "effect\0", effect,
						   "index\0", index,
						   NULL);

  return(recall_ladspa);
}
