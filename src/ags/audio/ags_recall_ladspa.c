/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/ags_recall_ladspa.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/util/ags_id_generator.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/object/ags_plugin.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/audio/ags_config.h>
#include <ags/audio/ags_devout.h>
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

extern AgsConfig *config;

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
  param_spec =  g_param_spec_uint("index\0",
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
  AGS_RECALL(recall_ladspa)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(recall_ladspa)->build_id = AGS_BUILD_ID;
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
      AgsDevout *devout;
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
      guint *index;
      
      index = g_value_get_uint(value);

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
  AgsLadspaPlugin *ladspa_plugin;
  AgsPort *current;
  GList *list;
  gchar *path;
  unsigned long port_count;
  unsigned long i;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHintDescriptor hint_descriptor;

  recall_ladspa = AGS_RECALL_LADSPA(plugin);

  ags_ladspa_manager_load_file(recall_ladspa->filename);
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(recall_ladspa->filename);
  
  plugin_so = ladspa_plugin->plugin_so;

  if(plugin_so){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      recall_ladspa->plugin_descriptor = 
	plugin_descriptor = ladspa_descriptor(recall_ladspa->index);

      port_count = plugin_descriptor->PortCount;
      port_descriptor = plugin_descriptor->PortDescriptors;

      for(i = 0; i < port_count; i++){
	if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	  if(LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	     LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	    gchar *plugin_name;
	    gchar *specifier;

	    hint_descriptor = plugin_descriptor->PortRangeHints[i].HintDescriptor;

	    plugin_name = g_strdup_printf("ladspa-%lu\0", plugin_descriptor->UniqueID);
	    specifier = g_strdup(plugin_descriptor->PortNames[i]);

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
	    
	    current->port_value.ags_port_float = plugin_descriptor->PortRangeHints[i].LowerBound;

	    g_message("connecting port: %d/%d\0", i, port_count);
	  }
	}else if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
	  if(LADSPA_IS_PORT_INPUT(port_descriptor[i])){
	    if(recall_ladspa->input_port == NULL){
	      recall_ladspa->input_port = (guint *) malloc(sizeof(guint));
	      recall_ladspa->input_port[0] = i;
	    }else{
	      recall_ladspa->input_port = (guint *) realloc(recall_ladspa->input_port,
							    (recall_ladspa->input_lines + 1) * sizeof(guint));
	      recall_ladspa->input_port[recall_ladspa->input_lines] = i;
	    }

	    recall_ladspa->input_lines += 1;
	  }else if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	    if(recall_ladspa->output_port == NULL){
	      recall_ladspa->output_port = (guint *) malloc(sizeof(guint));
	      recall_ladspa->output_port[0] = i;
	    }else{
	      recall_ladspa->output_port = (guint *) realloc(recall_ladspa->output_port,
							    (recall_ladspa->output_lines + 1) * sizeof(guint));
	      recall_ladspa->output_port[recall_ladspa->output_lines] = i;
	    }

	    recall_ladspa->output_lines += 1;
	  }
	}
      }

      AGS_RECALL(recall_ladspa)->port = g_list_reverse(port);
    }
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
				   "main\0", file->ags_main,
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
  GList *list;
  gchar *id;
  guint i;

  recall_ladspa = AGS_RECALL_LADSPA(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-ladspa\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "main\0", file->ags_main,
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
  ags_ladspa_manager_load_file(recall_ladspa->filename);
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(recall_ladspa->filename);
  
  plugin_so = ladspa_plugin->plugin_so;

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
  gchar *path;
  unsigned long port_count;
  unsigned long i;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHintDescriptor hint_descriptor;

  ags_ladspa_manager_load_file(recall_ladspa->filename);
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(recall_ladspa->filename);
  port = NULL;
  
  plugin_so = ladspa_plugin->plugin_so;

  if(plugin_so){
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							   "ladspa_descriptor\0");

    if(dlerror() == NULL && ladspa_descriptor){
      recall_ladspa->plugin_descriptor = 
	plugin_descriptor = ladspa_descriptor(recall_ladspa->index);

      port_count = plugin_descriptor->PortCount;
      port_descriptor = plugin_descriptor->PortDescriptors;

      for(i = 0; i < port_count; i++){
	if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	  if(LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	     LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	    gchar *plugin_name;
	    gchar *specifier;

	    hint_descriptor = plugin_descriptor->PortRangeHints[i].HintDescriptor;

	    plugin_name = g_strdup_printf("ladspa-%lu\0", plugin_descriptor->UniqueID);
	    specifier = g_strdup(plugin_descriptor->PortNames[i]);

	    current = g_object_new(AGS_TYPE_PORT,
				   "plugin-name\0", plugin_name,
				   "specifier\0", specifier,
				   "control-port\0", g_strdup_printf("%d/%d\0",
								     i,
								     port_count),
				   "port-value-is-pointer\0", FALSE,
				   "port-value-type\0", G_TYPE_FLOAT,
				   NULL);
	    current->port_value.ags_port_float = plugin_descriptor->PortRangeHints[i].LowerBound;

	    g_message("connecting port: %d/%d\0", i, port_count);

	    port = g_list_prepend(port,
				  current);
	  }
	}else if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
	  if(LADSPA_IS_PORT_INPUT(port_descriptor[i])){
	    if(recall_ladspa->input_port == NULL){
	      recall_ladspa->input_port = (guint *) malloc(sizeof(guint));
	      recall_ladspa->input_port[0] = i;
	    }else{
	      recall_ladspa->input_port = (guint *) realloc(recall_ladspa->input_port,
							    (recall_ladspa->input_lines + 1) * sizeof(guint));
	      recall_ladspa->input_port[recall_ladspa->input_lines] = i;
	    }

	    recall_ladspa->input_lines += 1;
	  }else if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	    if(recall_ladspa->output_port == NULL){
	      recall_ladspa->output_port = (guint *) malloc(sizeof(guint));
	      recall_ladspa->output_port[0] = i;
	    }else{
	      recall_ladspa->output_port = (guint *) realloc(recall_ladspa->output_port,
							    (recall_ladspa->output_lines + 1) * sizeof(guint));
	      recall_ladspa->output_port[recall_ladspa->output_lines] = i;
	    }

	    recall_ladspa->output_lines += 1;
	  }
	}
      }

      AGS_RECALL(recall_ladspa)->port = g_list_reverse(port);
    }
  }

  return(AGS_RECALL(recall_ladspa)->port);
}

/**
 * ags_recall_ladspa_short_to_float:
 * @buffer: source
 * @destination: destination
 *
 * Convert data type.
 * 
 * Since: 0.4
 */
void
ags_recall_ladspa_short_to_float(signed short *buffer,
				 float *destination,
				 guint buffer_size, guint lines)
{
  float *new_buffer;
  guint i;

  new_buffer = destination;

  for(i = 0; i < buffer_size; i++){
    new_buffer[lines * i] += (float) buffer[i] * (1.0f / 32767.5f);
  }
}

/**
 * ags_recall_ladspa_float_to_short:
 * @buffer: source
 * @destination: destination
 * @buffer_size: buffer_size
 *
 * Convert data type.
 * 
 * Since: 0.4
 */
void
ags_recall_ladspa_float_to_short(float *buffer,
				 signed short *destination,
				 guint buffer_size, guint lines)
{
  signed short *new_buffer;
  guint i;

  new_buffer = destination;

  for(i = 0; i < buffer_size; i++){
    new_buffer[i] += (signed short) buffer[lines * i] * 32767.5f;
  }
}

/**
 * ags_recall_ladpsa_find:
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
ags_recall_ladpsa_find(GList *recall,
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
		      guint index)
{
  AgsDevout *devout;
  AgsRecallLadspa *recall_ladspa;

  if(source != NULL){
    devout = AGS_AUDIO(source->audio)->devout;
  }else{
    devout = NULL;
  }

  recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
						   "devout\0", devout,
						   "source\0", source,
						   "filename\0", filename,
						   "effect\0", effect,
						   "index\0", index,
						   NULL);

  return(recall_ladspa);
}
