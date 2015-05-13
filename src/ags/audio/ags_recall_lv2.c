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

#include <ags/audio/ags_recall_lv2.h>

#include <ags/util/ags_id_generator.h>

#include <ags/plugin/ags_lv2_manager.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

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

void ags_recall_lv2_class_init(AgsRecallLv2Class *recall_lv2_class);
void ags_recall_lv2_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_lv2_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_lv2_init(AgsRecallLv2 *recall_lv2);
void ags_recall_lv2_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_recall_lv2_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_recall_lv2_connect(AgsConnectable *connectable);
void ags_recall_lv2_disconnect(AgsConnectable *connectable);
void ags_recall_lv2_set_ports(AgsPlugin *plugin, GList *port);
void ags_recall_lv2_finalize(GObject *gobject);

void ags_recall_lv2_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_recall_lv2_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_recall_lv2_load(AgsRecallLv2 *recall_lv2);

/**
 * SECTION:ags_recall_lv2
 * @short_description: The object interfacing with LV2
 * @title: AgsRecallLv2
 * @section_id:
 * @include: ags/audio/ags_recall_lv2.h
 *
 * #AgsRecallLv2 provides LV2 support.
 */

enum{
  PROP_0,
  PROP_TURTLE,
  PROP_FILENAME,
  PROP_URI,
  PROP_INDEX,
};

static gpointer ags_recall_lv2_parent_class = NULL;
static AgsConnectableInterface* ags_recall_lv2_parent_connectable_interface;
static AgsPluginInterface* ags_recall_lv2_parent_plugin_interface;

GType
ags_recall_lv2_get_type (void)
{
  static GType ags_type_recall_lv2 = 0;

  if(!ags_type_recall_lv2){
    static const GTypeInfo ags_recall_lv2_info = {
      sizeof (AgsRecallLv2Class),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_lv2_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLv2),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_lv2_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_lv2_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_lv2_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_lv2 = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						 "AgsRecallLv2\0",
						 &ags_recall_lv2_info,
						 0);

    g_type_add_interface_static(ags_type_recall_lv2,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_lv2,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_lv2);
}

void
ags_recall_lv2_class_init(AgsRecallLv2Class *recall_lv2)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_recall_lv2_parent_class = g_type_class_peek_parent(recall_lv2);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_lv2;

  gobject->set_property = ags_recall_lv2_set_property;
  gobject->get_property = ags_recall_lv2_get_property;

  gobject->finalize = ags_recall_lv2_finalize;

  /* properties */
  /**
   * AgsRecallLv2:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 0.4.3
   */
  param_spec = g_param_spec_object("turtle\0",
				   "turtle of recall lv2\0",
				   "The turtle which this recall lv2 is described by\0",
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsRecallLv2:filename:
   *
   * The plugins filename.
   * 
   * Since: 0.4.3
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
   * AgsRecallLv2:uri:
   *
   * The uri's name.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_string("uri\0",
				    "the uri\0",
				    "The uri's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsRecallLv2:index:
   *
   * The uri's index.
   * 
   * Since: 0.4.3
   */
  param_spec =  g_param_spec_ulong("index\0",
				   "index of uri\0",
				   "The numerical index of uri\0",
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX,
				  param_spec);
}


void
ags_recall_lv2_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_lv2_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_lv2_connect;
  connectable->disconnect = ags_recall_lv2_disconnect;
}

void
ags_recall_lv2_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_lv2_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_recall_lv2_read;
  plugin->write = ags_recall_lv2_write;
  plugin->set_ports = ags_recall_lv2_set_ports;
}

void
ags_recall_lv2_init(AgsRecallLv2 *recall_lv2)
{
  AGS_RECALL(recall_lv2)->name = "ags-lv2\0";
  AGS_RECALL(recall_lv2)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(recall_lv2)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(recall_lv2)->xml_type = "ags-recall-lv2\0";
  AGS_RECALL(recall_lv2)->port = NULL;

  recall_lv2->turtle = NULL;

  recall_lv2->filename = NULL;
  recall_lv2->uri = NULL;
  recall_lv2->index = 0;

  recall_lv2->plugin_descriptor = NULL;

  recall_lv2->input_port = NULL;
  recall_lv2->input_lines = 0;

  recall_lv2->output_port = NULL;
  recall_lv2->output_lines = 0;
}

void
ags_recall_lv2_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsRecallLv2 *recall_lv2;

  recall_lv2 = AGS_RECALL_LV2(gobject);

  switch(prop_id){
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      if(recall_lv2->turtle == turtle){
	return;
      }

      if(recall_lv2->turtle != NULL){
	g_object_unref(recall_lv2->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }

      recall_lv2->turtle = turtle;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == recall_lv2->filename){
	return;
      }

      if(recall_lv2->filename != NULL){
	g_free(recall_lv2->filename);
      }

      recall_lv2->filename = g_strdup(filename);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      if(uri == recall_lv2->uri){
	return;
      }

      if(recall_lv2->uri != NULL){
	g_free(recall_lv2->uri);
      }

      recall_lv2->uri = g_strdup(uri);
    }
    break;
  case PROP_INDEX:
    {
      uint32_t *index;
      
      index = g_value_get_ulong(value);

      if(index == recall_lv2->index){
	return;
      }

      recall_lv2->index = index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_lv2_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsRecallLv2 *recall_lv2;

  recall_lv2 = AGS_RECALL_LV2(gobject);

  switch(prop_id){
  case PROP_TURTLE:
    {
      g_value_set_object(value, recall_lv2->turtle);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, recall_lv2->filename);
    }
    break;
  case PROP_URI:
    {
      g_value_set_string(value, recall_lv2->uri);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, recall_lv2->index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_lv2_connect(AgsConnectable *connectable)
{
  ags_recall_lv2_parent_connectable_interface->connect(connectable);
}

void
ags_recall_lv2_disconnect(AgsConnectable *connectable)
{
  ags_recall_lv2_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_lv2_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsRecallLv2 *recall_lv2;
  AgsPort *current;

  AgsLv2Plugin *lv2_plugin;
  
  GList *list;
  GList *port_node, *port_name_node, *port_default_node, *port_index_node;

  uint32_t port_count;
  uint32_t i;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  recall_lv2 = AGS_RECALL_LV2(plugin);

  ags_lv2_manager_load_file(recall_lv2->turtle,
			    recall_lv2->filename);
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(recall_lv2->filename);

  if(lv2_plugin->plugin_so == NULL){
    plugin_so =
      lv2_plugin->plugin_so = dlopen(lv2_plugin->filename,
				     RTLD_NOW);
  }else{
    plugin_so = lv2_plugin->plugin_so;
  }
  
  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor\0");

    if(dlerror() == NULL && lv2_descriptor){
      recall_lv2->plugin_descriptor = 
	plugin_descriptor = lv2_descriptor(recall_lv2->index);

      /* connect control port */
      port_node = ags_turtle_find_xpath(recall_lv2->turtle,
					"//rdf-triple[@subject=\"lv2:port\"]\/rdf-verb[@has_type=\"true\"]/rdf-list/rdf-value\0");
      port_name_node = ags_turtle_find_xpath(recall_lv2->turtle,
					     "//rdf-triple[@subject=\"lv2:port\"]/rdf-verb[do=\"lv2:name\"]/rdf-list/rdf-value\0");
      port_index_node = ags_turtle_find_xpath(recall_lv2->turtle,
					      "//rdf-triple[@subject=\"lv2:port\"]\/rdf-verb[@do=\"lv2:index\"]/rdf-list/rdf-value\0");
      port_default_node = ags_turtle_find_xpath(recall_lv2->turtle,
						"//rdf-triple[@subject=\"lv2:port\"]/rdf-verb[do=\"lv2:default\"]/rdf-list/rdf-value\0");
      
      port_count = g_list_length(port_node);

      for(i = 0; i < port_count; i++){
	gchar *port_type_0, *port_type_1;

	port_type_0 = xmlGetProp(port_node->data,
				 "value\0");
	port_type_1 = xmlGetProp(port_node->next->data,
				 "value\0");
	
	if(!g_ascii_strncasecmp(port_type_0,
				"lv2:AudioPort\0",
				13) ||
	   !g_ascii_strncasecmp(port_type_1,
				"lv2:AudioPort\0\0",
				13)){
	  if(!g_ascii_strncasecmp(port_type_0,
				  "lv2:InputPort\0",
				  13) ||
	     !g_ascii_strncasecmp(port_type_1,
				  "lv2:InpuPort\0\0",
				  13)){
	    if(recall_lv2->input_port == NULL){
	      recall_lv2->input_port = (uint32_t *) malloc(sizeof(uint32_t));
	      recall_lv2->input_port[0] = (uint32_t) g_ascii_strtoull(xmlGetProp(port_index_node->data,
									      "value\0"),
								   10,
								   NULL);
	    }else{
	      recall_lv2->input_port = (uint32_t *) realloc(recall_lv2->input_port,
							 (recall_lv2->input_lines + 1) * sizeof(uint32_t));
	      recall_lv2->input_port[recall_lv2->input_lines] = g_ascii_strtoull(xmlGetProp(port_index_node->data,
											    "value\0"),
										 10,
										 NULL);
	    }

	    recall_lv2->input_lines += 1;
	  }else if(!g_ascii_strncasecmp(port_type_0,
					"lv2:OutputPort\0",
					13) ||
		   !g_ascii_strncasecmp(port_type_1,
					"lv2:OutpuPort\0\0",
					13)){
	    if(recall_lv2->output_port == NULL){
	      recall_lv2->output_port = (uint32_t *) malloc(sizeof(uint32_t));
	      recall_lv2->output_port[0] = g_ascii_strtoull(xmlGetProp(port_index_node->data,
								       "value\0"),
							    10,
							    NULL);
	    }else{
	      recall_lv2->output_port = (uint32_t *) realloc(recall_lv2->output_port,
							  (recall_lv2->output_lines + 1) * sizeof(uint32_t));
	      recall_lv2->output_port[recall_lv2->output_lines] = g_ascii_strtoull(xmlGetProp(port_index_node->data,
											      "value\0"),
										   10,
										   NULL);
	    }

	    recall_lv2->output_lines += 1;
	  }
	}else{
	  gchar *plugin_name;
	  gchar *specifier;

	  plugin_name = g_strdup_printf("lv2-%lu\0", plugin_descriptor->URI);
	  specifier = g_strdup(xmlGetProp(port_name_node->data,
					  "value\0"));

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
	
	  current->port_value.ags_port_float = (float) g_ascii_strtod(xmlGetProp(port_default_node->data,
										 "value\0"),
								      NULL);
	
	  g_message("connecting port: %d/%d\0", i, port_count);
	}
	
	port_node = port_node->next->next;
	port_name_node = port_name_node->next;
	port_index_node = port_index_node->next;
	port_default_node = port_default_node->next;
      }
            
      AGS_RECALL(recall_lv2)->port = g_list_reverse(port);
    }
  }
}

void
ags_recall_lv2_finalize(GObject *gobject)
{
  AgsRecallLv2 *recall_lv2;
  
  recall_lv2 = AGS_RECALL_LV2(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_lv2_parent_class)->finalize(gobject);
}

void
ags_recall_lv2_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsRecallLv2 *gobject;
  AgsLv2Plugin *lv2_plugin;
  gchar *filename, *uri;
  uint32_t index;

  gobject = AGS_RECALL_LV2(plugin);

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
  uri = xmlGetProp(node,
		   "uri\0");
  index = g_ascii_strtoull(xmlGetProp(node,
				      "index\0"),
			   NULL,
			   10);

  g_object_set(gobject,
	       "filename\0", filename,
	       "uri\0", uri,
	       "index\0", index,
	       NULL);

  ags_recall_lv2_load(gobject);
}

xmlNode*
ags_recall_lv2_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsRecallLv2 *recall_lv2;
  xmlNode *node;
  GList *list;
  gchar *id;

  recall_lv2 = AGS_RECALL_LV2(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-lv2\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", recall_lv2,
				   NULL));

  xmlNewProp(node,
	     "filename\0",
	     g_strdup(recall_lv2->filename));

  xmlNewProp(node,
	     "uri\0",
	     g_strdup(recall_lv2->uri));

  xmlNewProp(node,
	     "index\0",
	     g_strdup_printf("%d\0", recall_lv2->index));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_recall_lv2_load:
 * @recall_lv2: an #AgsRecallLv2
 *
 * Set up LV2 handle.
 * 
 * Since: 0.4.3
 */
void
ags_recall_lv2_load(AgsRecallLv2 *recall_lv2)
{
  AgsLv2Plugin *lv2_plugin;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  /*  */
  ags_lv2_manager_load_file(recall_lv2->turtle,
			    recall_lv2->filename);
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(recall_lv2->filename);

  if(lv2_plugin->plugin_so == NULL){
    plugin_so =
      lv2_plugin->plugin_so = dlopen(lv2_plugin->filename,
				     RTLD_NOW);
  }else{
    plugin_so = lv2_plugin->plugin_so;
  }
  
  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor\0");

    if(dlerror() == NULL && lv2_descriptor){
      recall_lv2->plugin_descriptor = 
	plugin_descriptor = lv2_descriptor(recall_lv2->index);
    }
  }
}

/**
 * ags_recall_lv2_load_ports:
 * @recall_lv2: an #AgsRecallLv2
 *
 * Set up LV2 ports.
 *
 * Returns: a #GList containing #AgsPort.
 * 
 * Since: 0.4.3
 */
GList*
ags_recall_lv2_load_ports(AgsRecallLv2 *recall_lv2)
{
  AgsLv2Plugin *lv2_plugin;
  AgsPort *current;
  GList *port;
  GList *port_node, *port_name_node, *port_default_node, *port_index_node;

  uint32_t port_count;
  uint32_t i;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  ags_lv2_manager_load_file(recall_lv2->turtle,
			    recall_lv2->filename);
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(recall_lv2->filename);
  port = NULL;

    if(lv2_plugin->plugin_so == NULL){
    plugin_so =
      lv2_plugin->plugin_so = dlopen(lv2_plugin->filename,
				     RTLD_NOW);
  }else{
    plugin_so = lv2_plugin->plugin_so;
  }
  
  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor\0");

    if(dlerror() == NULL && lv2_descriptor){
      recall_lv2->plugin_descriptor = 
	plugin_descriptor = lv2_descriptor(recall_lv2->index);

      /* connect control port */
      port_node = ags_turtle_find_xpath(recall_lv2->turtle,
					"//rdf-triple[@subject=\"lv2:port\"]\/rdf-verb[@has_type=\"true\"]/rdf-list/rdf-value\0");
      port_name_node = ags_turtle_find_xpath(recall_lv2->turtle,
					     "//rdf-triple[@subject=\"lv2:port\"]/rdf-verb[do=\"lv2:name\"]/rdf-list/rdf-value\0");
      port_index_node = ags_turtle_find_xpath(recall_lv2->turtle,
					      "//rdf-triple[@subject=\"lv2:port\"]\/rdf-verb[@do=\"lv2:index\"]/rdf-list/rdf-value\0");
      port_default_node = ags_turtle_find_xpath(recall_lv2->turtle,
						"//rdf-triple[@subject=\"lv2:port\"]/rdf-verb[do=\"lv2:default\"]/rdf-list/rdf-value\0");
      
      port_count = g_list_length(port_node);

      for(i = 0; i < port_count; i++){
	gchar *port_type_0, *port_type_1;

	port_type_0 = xmlGetProp(port_node->data,
				 "value\0");
	port_type_1 = xmlGetProp(port_node->next->data,
				 "value\0");
	

	if(!g_ascii_strncasecmp(port_type_0,
				"lv2:AudioPort\0",
				13) ||
	   !g_ascii_strncasecmp(port_type_1,
				"lv2:AudioPort\0\0",
				13)){
	  if(!g_ascii_strncasecmp(port_type_0,
				  "lv2:InputPort\0",
				  13) ||
	     !g_ascii_strncasecmp(port_type_1,
				  "lv2:InpuPort\0\0",
				  13)){
	    if(recall_lv2->input_port == NULL){
	      recall_lv2->input_port = (uint32_t *) malloc(sizeof(uint32_t));
	      recall_lv2->input_port[0] = i;
	    }else{
	      recall_lv2->input_port = (uint32_t *) realloc(recall_lv2->input_port,
							    (recall_lv2->input_lines + 1) * sizeof(uint32_t));
	      recall_lv2->input_port[recall_lv2->input_lines] = i;
	    }

	    recall_lv2->input_lines += 1;
	  }else if(!g_ascii_strncasecmp(port_type_0,
					"lv2:OutputPort\0",
					13) ||
		   !g_ascii_strncasecmp(port_type_1,
					"lv2:OutpuPort\0\0",
					13)){
	    if(recall_lv2->output_port == NULL){
	      recall_lv2->output_port = (uint32_t *) malloc(sizeof(uint32_t));
	      recall_lv2->output_port[0] = i;
	    }else{
	      recall_lv2->output_port = (uint32_t *) realloc(recall_lv2->output_port,
							     (recall_lv2->output_lines + 1) * sizeof(uint32_t));
	      recall_lv2->output_port[recall_lv2->output_lines] = i;
	    }

	    recall_lv2->output_lines += 1;
	  }
	}else{
	  gchar *plugin_name;
	  gchar *specifier;

	  plugin_name = g_strdup_printf("lv2-%lu\0", plugin_descriptor->URI);
	  specifier = g_strdup(xmlGetProp(port_name_node->data,
					  "value\0"));

	  current = g_object_new(AGS_TYPE_PORT,
				 "plugin-name\0", plugin_name,
				 "specifier\0", specifier,
				 "control-port\0", g_strdup_printf("%d/%d\0",
								   i,
								   port_count),
				 "port-value-is-pointer\0", FALSE,
				 "port-value-type\0", G_TYPE_FLOAT,
				 NULL);
	  current->port_value.ags_port_float = (float) g_ascii_strtod(xmlGetProp(port_default_node->data,
										 "value\0"),
								      NULL);

	  g_message("connecting port: %d/%d\0", i, port_count);

	  port = g_list_prepend(port,
				current);
	}

	port_node = port_node->next->next;
	port_name_node = port_name_node->next;
	port_index_node = port_index_node->next;
	port_default_node = port_default_node->next;
      }

      AGS_RECALL(recall_lv2)->port = g_list_reverse(port);
    }
  }
  
  return(AGS_RECALL(recall_lv2)->port);
}

/**
 * ags_recall_lv2_short_to_float:
 * @buffer: source
 * @destination: destination
 *
 * Convert data type.
 * 
 * Since: 0.4.3
 */
void
ags_recall_lv2_short_to_float(signed short *buffer,
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
 * ags_recall_lv2_float_to_short:
 * @buffer: source
 * @destination: destination
 * @buffer_size: buffer_size
 *
 * Convert data type.
 * 
 * Since: 0.4.3
 */
void
ags_recall_lv2_float_to_short(float *buffer,
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
 * @uri: uri's name
 *
 * Retrieve LV2 recall.
 *
 * Returns: Next match.
 * 
 * Since: 0.4.3
 */
GList*
ags_recall_ladpsa_find(GList *recall,
		       gchar *filename, gchar *uri)
{
  while(recall != NULL){
    if(AGS_IS_RECALL_LV2(recall->data)){
      if(!g_strcmp0(AGS_RECALL_LV2(recall->data)->filename,
		    filename) &&
	 !g_strcmp0(AGS_RECALL_LV2(recall->data)->uri,
		    uri)){
	return(recall);
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_lv2_new:
 * @source: the source
 * #turtle: the .ttl file
 * @filename: the LV2 plugin filename
 * @uri: uri's name
 * @index: uri's index
 *
 * Creates a #AgsRecallLv2
 *
 * Returns: a new #AgsRecallLv2
 * 
 * Since: 0.4.3
 */
AgsRecallLv2*
ags_recall_lv2_new(AgsChannel *source,
		   AgsTurtle *turtle,
		   gchar *filename,
		   gchar *uri,
		   uint32_t index)
{
  GObject *soundcard;
  AgsRecallLv2 *recall_lv2;

  if(source != NULL){
    soundcard = AGS_AUDIO(source->audio)->soundcard;
  }else{
    soundcard = NULL;
  }

  recall_lv2 = (AgsRecallLv2 *) g_object_new(AGS_TYPE_RECALL_LV2,
					     "soundcard\0", soundcard,
					     "turtle\0", turtle,
					     "source\0", source,
					     "filename\0", filename,
					     "uri\0", uri,
					     "index\0", index,
					     NULL);

  return(recall_lv2);
}
