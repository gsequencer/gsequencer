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

#include <ags/audio/ags_recall_dssi.h>

#include <ags/libags.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_plugin_port.h>
#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/audio/ags_port.h>

#include <endian.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_recall_dssi_class_init(AgsRecallDssiClass *recall_dssi_class);
void ags_recall_dssi_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_dssi_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_dssi_init(AgsRecallDssi *recall_dssi);
void ags_recall_dssi_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_recall_dssi_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_recall_dssi_finalize(GObject *gobject);

void ags_recall_dssi_set_ports(AgsPlugin *plugin, GList *port);
void ags_recall_dssi_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_recall_dssi_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

/**
 * SECTION:ags_recall_dssi
 * @short_description: The object interfacing with DSSI
 * @title: AgsRecallDssi
 * @section_id:
 * @include: ags/audio/ags_recall_dssi.h
 *
 * #AgsRecallDssi provides DSSI support.
 */

enum{
  PROP_0,
  PROP_BANK,
  PROP_PROGRAM,
};

static gpointer ags_recall_dssi_parent_class = NULL;
static AgsConnectableInterface* ags_recall_dssi_parent_connectable_interface;
static AgsPluginInterface* ags_recall_dssi_parent_plugin_interface;

GType
ags_recall_dssi_get_type (void)
{
  static GType ags_type_recall_dssi = 0;

  if(!ags_type_recall_dssi){
    static const GTypeInfo ags_recall_dssi_info = {
      sizeof (AgsRecallDssiClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_dssi_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallDssi),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_dssi_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_dssi_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_dssi_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_dssi = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						  "AgsRecallDssi",
						  &ags_recall_dssi_info,
						  0);

    g_type_add_interface_static(ags_type_recall_dssi,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_dssi,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_dssi);
}

void
ags_recall_dssi_class_init(AgsRecallDssiClass *recall_dssi)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_recall_dssi_parent_class = g_type_class_peek_parent(recall_dssi);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_dssi;

  gobject->set_property = ags_recall_dssi_set_property;
  gobject->get_property = ags_recall_dssi_get_property;

  gobject->finalize = ags_recall_dssi_finalize;

  /* properties */
  /**
   * AgsRecallDssi:bank:
   *
   * The selected bank.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("bank",
				 i18n_pspec("bank"),
				 i18n_pspec("The selected bank"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsRecallDssi:program:
   *
   * The selected program.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("program",
				 i18n_pspec("program"),
				 i18n_pspec("The selected program"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PROGRAM,
				  param_spec);
}

void
ags_recall_dssi_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_dssi_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_dssi_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_dssi_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_recall_dssi_set_ports;
  plugin->read = ags_recall_dssi_read;
  plugin->write = ags_recall_dssi_write;
}

void
ags_recall_dssi_init(AgsRecallDssi *recall_dssi)
{
  AGS_RECALL(recall_dssi)->name = "ags-dssi";
  AGS_RECALL(recall_dssi)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(recall_dssi)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(recall_dssi)->xml_type = "ags-recall-dssi";
  AGS_RECALL(recall_dssi)->port = NULL;
  
  recall_dssi->bank = 0;
  recall_dssi->program = 0;
  
  recall_dssi->plugin_descriptor = NULL;

  recall_dssi->input_port = NULL;
  recall_dssi->input_lines = 0;

  recall_dssi->output_port = NULL;
  recall_dssi->output_lines = 0;
}

void
ags_recall_dssi_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsRecallDssi *recall_dssi;

  pthread_mutex_t *recall_mutex;

  recall_dssi = AGS_RECALL_DSSI(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_BANK:
    {
      pthread_mutex_lock(recall_mutex);

      recall_dssi->bank = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PROGRAM:
    {
      pthread_mutex_lock(recall_mutex);

      recall_dssi->program = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_dssi_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsRecallDssi *recall_dssi;

  pthread_mutex_t *recall_mutex;

  recall_dssi = AGS_RECALL_DSSI(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_BANK:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall_dssi->bank);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PROGRAM:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall_dssi->program);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_dssi_finalize(GObject *gobject)
{
  AgsRecallDssi *recall_dssi;
  
  recall_dssi = AGS_RECALL_DSSI(gobject);

  if(recall_dssi->input_port != NULL){
    free(recall_dssi->input_port);
  }

  if(recall_dssi->output_port != NULL){
    free(recall_dssi->output_port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_dssi_parent_class)->finalize(gobject);
}

void
ags_recall_dssi_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsRecallDssi *recall_dssi;
  AgsPort *current_port;

  AgsDssiPlugin *dssi_plugin;

  GList *list;
  GList *plugin_port_start, *plugin_port;

  gchar *filename, *effect;
  
  guint port_count;
  guint i;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;
  pthread_mutex_t *port_mutex;

  recall_dssi = AGS_RECALL_DSSI(plugin);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(recall_dssi)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_dssi)->filename);
  effect = g_strdup(AGS_RECALL(recall_dssi)->effect);
  
  pthread_mutex_unlock(recall_mutex);
  
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);

  g_free(filename);
  g_free(effect);
  
  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get plugin port */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_port =
    plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(dssi_plugin)->plugin_port);

  pthread_mutex_unlock(base_plugin_mutex);

  /* match port */
  if(plugin_port != NULL){
    port_count = g_list_length(plugin_port_start);

    for(i = 0; i < port_count; i++){
      AgsPluginPort *current_plugin_port;
      
      pthread_mutex_t *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
      
      plugin_port_mutex = current_plugin_port->obj_mutex;

      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());
      
      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_CONTROL)){
	gchar *specifier;
	
	GValue *default_value;
	
	default_value = g_new0(GValue,
			       1);
	
	pthread_mutex_lock(plugin_port_mutex);

	specifier = g_strdup(current_plugin_port->port_name);
	g_value_copy(current_plugin_port->default_value,
		     default_value);
	
	pthread_mutex_unlock(plugin_port_mutex);

	list = port;

	while(list != NULL){
	  gchar *current_specifier;

	  gboolean success;

	  current_port = list->data;
	  
	  /* get port mutex */
	  pthread_mutex_lock(ags_port_get_class_mutex());

	  port_mutex = current_port->obj_mutex;
      
	  pthread_mutex_unlock(ags_port_get_class_mutex());

	  /* check specifier */
	  pthread_mutex_lock(port_mutex);

	  current_specifier = g_strdup(current_port->specifier);
	  
	  pthread_mutex_unlock(port_mutex);

	  success = (!g_strcmp0(specifier,
				current_specifier)) ? TRUE: FALSE;
	  g_free(current_specifier);
	  
	  if(success){
	    break;
	  }
	  
	  list = list->next;
	}
	
	if(list != NULL){
	  current_port->plugin_port = current_plugin_port;
	  ags_recall_dssi_load_conversion(recall_dssi,
					  (GObject *) current_port,
					  current_plugin_port);

	  //TODO:JK: check non-raw write	  
	  ags_port_safe_write_raw(current_port,
				  default_value);
	    
#ifdef AGS_DEBUG
	  g_message("connecting port: %lu/%lu", i, port_count);
#endif
	}

	g_free(default_value);
	g_free(specifier);
      }else if(ags_plugin_port_test_flags(current_plugin_port,
					  AGS_PLUGIN_PORT_AUDIO)){
	pthread_mutex_lock(recall_mutex);
	
	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_INPUT)){
	  if(recall_dssi->input_port == NULL){
	    recall_dssi->input_port = (guint *) malloc(sizeof(guint));
	    recall_dssi->input_port[0] = i;
	  }else{
	    recall_dssi->input_port = (guint *) realloc(recall_dssi->input_port,
							(recall_dssi->input_lines + 1) * sizeof(guint));
	    recall_dssi->input_port[recall_dssi->input_lines] = i;
	  }

	  recall_dssi->input_lines += 1;
	}else if(ags_plugin_port_test_flags(current_plugin_port,
					    AGS_PLUGIN_PORT_OUTPUT)){
	  if(recall_dssi->output_port == NULL){
	    recall_dssi->output_port = (guint *) malloc(sizeof(guint));
	    recall_dssi->output_port[0] = i;
	  }else{
	    recall_dssi->output_port = (guint *) realloc(recall_dssi->output_port,
							 (recall_dssi->output_lines + 1) * sizeof(guint));
	    recall_dssi->output_port[recall_dssi->output_lines] = i;
	  }

	  recall_dssi->output_lines += 1;
	}

	pthread_mutex_unlock(recall_mutex);
      }

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
    
    /* reverse port */
    pthread_mutex_lock(recall_mutex);
    
    AGS_RECALL(recall_dssi)->port = g_list_reverse(port);

    pthread_mutex_unlock(recall_mutex);
  }

  g_list_free(plugin_port_start);
}

void
ags_recall_dssi_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsRecallDssi *gobject;
  AgsDssiPlugin *dssi_plugin;

  gobject = AGS_RECALL_DSSI(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));

  ags_recall_dssi_load(gobject);
}

xmlNode*
ags_recall_dssi_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsRecallDssi *recall_dssi;

  xmlNode *node;

  gchar *id;

  /*  */
  recall_dssi = AGS_RECALL_DSSI(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-dssi");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", recall_dssi,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_recall_dssi_load:
 * @recall_dssi: the #AgsRecallDssi
 *
 * Set up DSSI handle.
 * 
 * Since: 2.0.0
 */
void
ags_recall_dssi_load(AgsRecallDssi *recall_dssi)
{
  AgsDssiPlugin *dssi_plugin;

  gchar *filename;
  gchar *effect;

  guint effect_index;
  
  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;

  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_RECALL_DSSI(recall_dssi)){
    return;
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(recall_dssi)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_dssi)->filename);
  effect = g_strdup(AGS_RECALL(recall_dssi)->effect);
  
  effect_index = AGS_RECALL(recall_dssi)->effect_index;

  pthread_mutex_unlock(recall_mutex);
  
  /* find dssi plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);
  g_free(filename);
  g_free(effect);

  g_object_get(dssi_plugin,
	       "plugin-so", &plugin_so,
	       NULL);

  if(plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor");

    if(dlerror() == NULL && dssi_descriptor){
      pthread_mutex_lock(recall_mutex);
      
      recall_dssi->plugin_descriptor = 
	plugin_descriptor = dssi_descriptor((unsigned long) effect_index);

      pthread_mutex_unlock(recall_mutex);
    }
  }
}

/**
 * ags_recall_dssi_load_ports:
 * @recall_dssi: the #AgsRecallDssi
 *
 * Set up DSSI ports.
 *
 * Returns: the #GList-struct containing #AgsPort
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_dssi_load_ports(AgsRecallDssi *recall_dssi)
{
  AgsDssiPlugin *dssi_plugin;
  AgsPort *current_port;

  GList *port, *retval;
  GList *plugin_port_start, *plugin_port;

  gchar *filename;
  gchar *effect;

  guint effect_index;
  
  guint port_count;
  guint i;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_RECALL_DSSI(recall_dssi)){
    return(NULL);
  }

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(recall_dssi)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_dssi)->filename);
  effect = g_strdup(AGS_RECALL(recall_dssi)->effect);
  
  effect_index = AGS_RECALL(recall_dssi)->effect_index;

  pthread_mutex_unlock(recall_mutex);

  /* find dssi plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);
  g_free(filename);
  g_free(effect);

  /* get base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());
  
  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get port descriptor */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_port =
    plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(dssi_plugin)->plugin_port);

  pthread_mutex_unlock(base_plugin_mutex);

  port = NULL;
  retval = NULL;
  
  if(plugin_port != NULL){
    port_count = g_list_length(plugin_port_start);
    
    for(i = 0; i < port_count; i++){
      AgsPluginPort *current_plugin_port;
      
      pthread_mutex_t *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      pthread_mutex_lock(ags_plugin_port_get_class_mutex());
      
      plugin_port_mutex = current_plugin_port->obj_mutex;

      pthread_mutex_unlock(ags_plugin_port_get_class_mutex());

      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_CONTROL)){
	gchar *plugin_name;
	gchar *specifier;

	GValue *default_value;
	
	plugin_name = g_strdup_printf("dssi-%lu", dssi_plugin->unique_id);

	default_value = g_new0(GValue,
			       1);
	
	pthread_mutex_lock(plugin_port_mutex);

	specifier = g_strdup(current_plugin_port->port_name);
	g_value_copy(current_plugin_port->default_value,
		     default_value);
	
	pthread_mutex_unlock(plugin_port_mutex);

	current_port = g_object_new(AGS_TYPE_PORT,
				    "plugin-name", plugin_name,
				    "specifier", specifier,
				    "control-port", g_strdup_printf("%lu/%lu",
								    i,
								    port_count),
				    "port-value-is-pointer", FALSE,
				    "port-value-type", G_TYPE_FLOAT,
				    NULL);
	current_port->flags |= AGS_PORT_USE_LADSPA_FLOAT;
	g_object_ref(current_port);
	
	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_OUTPUT)){
	  current_port->flags |= AGS_PORT_IS_OUTPUT;

	  ags_recall_set_flags(recall_dssi,
			       AGS_RECALL_HAS_OUTPUT_PORT);

	}else{
	  if(!ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_TOGGLED)){
	    current_port->flags |= AGS_PORT_INFINITE_RANGE;
	  }
	}
	
	g_object_set(current_port,
		     "plugin-port", current_plugin_port,
		     NULL);

	ags_recall_dssi_load_conversion(recall_dssi,
					(GObject *) current_port,
					current_plugin_port);
	
	ags_port_safe_write_raw(current_port,
				default_value);

	port = g_list_prepend(port,
			      current_port);

	g_free(default_value);
	g_free(plugin_name);
	g_free(specifier);
      }else if(ags_plugin_port_test_flags(current_plugin_port,
					  AGS_PLUGIN_PORT_AUDIO)){
	pthread_mutex_lock(recall_mutex);

	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_INPUT)){
	  if(recall_dssi->input_port == NULL){
	    recall_dssi->input_port = (unsigned long *) malloc(sizeof(unsigned long));
	    recall_dssi->input_port[0] = i;
	  }else{
	    recall_dssi->input_port = (unsigned long *) realloc(recall_dssi->input_port,
								(recall_dssi->input_lines + 1) * sizeof(unsigned long));
	    recall_dssi->input_port[recall_dssi->input_lines] = i;
	  }
	  
	  recall_dssi->input_lines += 1;
	}else if(ags_plugin_port_test_flags(current_plugin_port,
					    AGS_PLUGIN_PORT_OUTPUT)){
	  if(recall_dssi->output_port == NULL){
	    recall_dssi->output_port = (unsigned long *) malloc(sizeof(unsigned long));
	    recall_dssi->output_port[0] = i;
	  }else{
	    recall_dssi->output_port = (unsigned long *) realloc(recall_dssi->output_port,
								 (recall_dssi->output_lines + 1) * sizeof(unsigned long));
	    recall_dssi->output_port[recall_dssi->output_lines] = i;
	  }
	  
	  recall_dssi->output_lines += 1;
	}

	pthread_mutex_unlock(recall_mutex);
      }

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
    
    /* reverse port */
    pthread_mutex_lock(recall_mutex);
    
    AGS_RECALL(recall_dssi)->port = g_list_reverse(port);
    
    retval = g_list_copy(AGS_RECALL(recall_dssi)->port);
    
    pthread_mutex_unlock(recall_mutex);
  }

  g_list_free(plugin_port_start);

  return(retval);
}

/**
 * ags_recall_dssi_load_conversion:
 * @recall_dssi: the #AgsRecallDssi
 * @port: the #AgsPort
 * @plugin_port: the #AgsPluginPort
 * 
 * Loads conversion object by using @plugin_port and sets in on @port.
 * 
 * Since: 2.0.0
 */
void
ags_recall_dssi_load_conversion(AgsRecallDssi *recall_dssi,
				GObject *port,
				gpointer plugin_port)
{
  AgsLadspaConversion *ladspa_conversion;

  if(!AGS_IS_RECALL_DSSI(recall_dssi) ||
     !AGS_IS_PORT(port) ||
     !AGS_IS_PLUGIN_PORT(plugin_port)){
    return;
  }

  ladspa_conversion = NULL;

  if(ags_plugin_port_test_flags(plugin_port,
				AGS_PLUGIN_PORT_BOUNDED_BELOW)){
    if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }

    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
  }

  if(ags_plugin_port_test_flags(plugin_port,
				AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
    if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }

    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
  }
  
  if(ags_plugin_port_test_flags(plugin_port,
				AGS_PLUGIN_PORT_SAMPLERATE)){
    if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }
        
    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
  }

  if(ags_plugin_port_test_flags(plugin_port,
				AGS_PLUGIN_PORT_LOGARITHMIC)){
    if(!AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }
    
    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
  }

  if(ladspa_conversion != NULL){
    g_object_set(port,
		 "conversion", ladspa_conversion,
		 NULL);
  }
}

/**
 * ags_recall_dssi_find:
 * @recall: the #GList-struct containing #AgsRecall
 * @filename: plugin filename
 * @effect: effect's name
 *
 * Retrieve DSSI recall.
 *
 * Returns: Next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_dssi_find(GList *recall,
		     gchar *filename, gchar *effect)
{
  gboolean success;
  
  pthread_mutex_t *recall_mutex;

  while(recall != NULL){
    if(AGS_IS_RECALL_DSSI(recall->data)){
      /* get recall mutex */
      pthread_mutex_lock(ags_recall_get_class_mutex());
  
      recall_mutex = AGS_RECALL(recall->data)->obj_mutex;
  
      pthread_mutex_unlock(ags_recall_get_class_mutex());

      /* check filename and effect */
      pthread_mutex_lock(recall_mutex);
      
      success = (!g_strcmp0(AGS_RECALL(recall->data)->filename,
			    filename) &&
		 !g_strcmp0(AGS_RECALL(recall->data)->effect,
			    effect)) ? TRUE: FALSE;

      pthread_mutex_unlock(recall_mutex);
      
      if(success){
	return(recall);
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_dssi_new:
 * @source: the #AgsChannel as source
 * @filename: the DSSI plugin filename
 * @effect: effect's name
 * @effect_index: effect's index
 *
 * Creates a new instance of #AgsRecallDssi
 *
 * Returns: the new #AgsRecallDssi
 * 
 * Since: 2.0.0
 */
AgsRecallDssi*
ags_recall_dssi_new(AgsChannel *source,
		    gchar *filename,
		    gchar *effect,
		    guint effect_index)
{
  AgsRecallDssi *recall_dssi;

  GObject *output_soundcard;

  output_soundcard = NULL;

  if(source != NULL){
    g_object_get(source,
		 "output-soundcard", &output_soundcard,
		 NULL);
  }

  recall_dssi = (AgsRecallDssi *) g_object_new(AGS_TYPE_RECALL_DSSI,
					       "output-soundcard", output_soundcard,
					       "source", source,
					       "filename", filename,
					       "effect", effect,
					       "effect-index", effect_index,
					       NULL);

  return(recall_dssi);
}
