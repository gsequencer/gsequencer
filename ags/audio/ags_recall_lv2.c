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

#include <ags/audio/ags_recall_lv2.h>

#include <ags/libags.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_plugin_port.h>
#include <ags/plugin/ags_lv2_conversion.h>

#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

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
void ags_recall_lv2_dispose(GObject *gobject);
void ags_recall_lv2_finalize(GObject *gobject);

void ags_recall_lv2_set_ports(AgsPlugin *plugin, GList *port);
void ags_recall_lv2_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_recall_lv2_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

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
  PROP_PLUGIN,
  PROP_URI,
  PROP_EVENT_PORT,
  PROP_ATOM_PORT,
  PROP_BANK,
  PROP_PROGRAM,
};

static gpointer ags_recall_lv2_parent_class = NULL;
static AgsConnectableInterface* ags_recall_lv2_parent_connectable_interface;
static AgsPluginInterface* ags_recall_lv2_parent_plugin_interface;

GType
ags_recall_lv2_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_lv2 = 0;

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
						 "AgsRecallLv2",
						 &ags_recall_lv2_info,
						 0);

    g_type_add_interface_static(ags_type_recall_lv2,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_lv2,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_lv2);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_recall_lv2_dispose;
  gobject->finalize = ags_recall_lv2_finalize;

  /* properties */
  /**
   * AgsRecallLv2:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("turtle",
				   i18n_pspec("turtle of recall lv2"),
				   i18n_pspec("The turtle which this recall lv2 is described by"),
				   AGS_TYPE_TURTLE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TURTLE,
				  param_spec);

  /**
   * AgsRecallLv2:plugin:
   *
   * The assigned plugin.
   * 
   * Since: 2.1.53
   */
  param_spec = g_param_spec_object("plugin",
				   i18n_pspec("plugin of recall lv2"),
				   i18n_pspec("The plugin which this recall lv2 does run"),
				   AGS_TYPE_LV2_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN,
				  param_spec);

  /**
   * AgsRecallLv2:uri:
   *
   * The uri's name.
   * 
   * Since: 2.0.0
   */
  param_spec =  g_param_spec_string("uri",
				    i18n_pspec("the uri"),
				    i18n_pspec("The uri's string representation"),
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URI,
				  param_spec);

  /**
   * AgsRecallLv2:event-port:
   *
   * The event port index.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("event-port",
				 i18n_pspec("event port"),
				 i18n_pspec("The event port index"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EVENT_PORT,
				  param_spec);

  /**
   * AgsRecallLv2:atom-port:
   *
   * The atom port index.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("atom-port",
				 i18n_pspec("atom port"),
				 i18n_pspec("The atom port index"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ATOM_PORT,
				  param_spec);
  
  /**
   * AgsRecallLv2:bank:
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
   * AgsRecallLv2:program:
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
ags_recall_lv2_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_lv2_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_lv2_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_lv2_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_recall_lv2_set_ports;
  plugin->read = ags_recall_lv2_read;
  plugin->write = ags_recall_lv2_write;
}

void
ags_recall_lv2_init(AgsRecallLv2 *recall_lv2)
{
  AGS_RECALL(recall_lv2)->name = "ags-lv2";
  AGS_RECALL(recall_lv2)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(recall_lv2)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(recall_lv2)->xml_type = "ags-recall-lv2";
  AGS_RECALL(recall_lv2)->port = NULL;

  recall_lv2->flags = 0;
  
  recall_lv2->turtle = NULL;

  recall_lv2->uri = NULL;

  recall_lv2->plugin = NULL;
  recall_lv2->plugin_descriptor = NULL;

  recall_lv2->input_port = NULL;
  recall_lv2->input_lines = 0;

  recall_lv2->output_port = NULL;
  recall_lv2->output_lines = 0;

  recall_lv2->event_port = 0;
  recall_lv2->atom_port = 0;

  recall_lv2->bank = 0;
  recall_lv2->program = 0;
}

void
ags_recall_lv2_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsRecallLv2 *recall_lv2;

  pthread_mutex_t *recall_mutex;

  recall_lv2 = AGS_RECALL_LV2(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  switch(prop_id){
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall_lv2->turtle == turtle){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(recall_lv2->turtle != NULL){
	g_object_unref(recall_lv2->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }

      recall_lv2->turtle = turtle;
      
      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PLUGIN:
    {
      AgsLv2Plugin *plugin;

      plugin = (AgsLv2Plugin *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(recall_lv2->plugin == plugin){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(recall_lv2->plugin != NULL){
	g_object_unref(recall_lv2->plugin);
      }

      if(plugin != NULL){
	g_object_ref(plugin);
      }

      recall_lv2->plugin = plugin;
      
      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      pthread_mutex_lock(recall_mutex);

      if(uri == recall_lv2->uri){
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(recall_lv2->uri != NULL){      
	g_free(recall_lv2->uri);
      }

      recall_lv2->uri = g_strdup(uri);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_EVENT_PORT:
    {
      pthread_mutex_lock(recall_mutex);

      recall_lv2->event_port = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_ATOM_PORT:
    {
      pthread_mutex_lock(recall_mutex);

      recall_lv2->atom_port = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_BANK:
    {
      pthread_mutex_lock(recall_mutex);

      recall_lv2->bank = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PROGRAM:
    {
      pthread_mutex_lock(recall_mutex);

      recall_lv2->program = g_value_get_uint(value);
      
      pthread_mutex_unlock(recall_mutex);	
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

  pthread_mutex_t *recall_mutex;

  recall_lv2 = AGS_RECALL_LV2(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  switch(prop_id){
  case PROP_TURTLE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_lv2->turtle);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PLUGIN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_lv2->plugin);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_URI:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_string(value, recall_lv2->uri);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_EVENT_PORT:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall_lv2->event_port);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_ATOM_PORT:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall_lv2->atom_port);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BANK:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall_lv2->bank);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PROGRAM:
    {
      pthread_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, recall_lv2->program);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_lv2_dispose(GObject *gobject)
{
  AgsRecallLv2 *recall_lv2;
  
  recall_lv2 = AGS_RECALL_LV2(gobject);

  /* turtle */
  if(recall_lv2->turtle != NULL){
    g_object_unref(recall_lv2->turtle);

    recall_lv2->turtle = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_lv2_parent_class)->dispose(gobject);
}

void
ags_recall_lv2_finalize(GObject *gobject)
{
  AgsRecallLv2 *recall_lv2;
  
  recall_lv2 = AGS_RECALL_LV2(gobject);

  /* turtle */
  if(recall_lv2->turtle != NULL){
    g_object_unref(recall_lv2->turtle);
  }

  if(recall_lv2->input_port != NULL){
    free(recall_lv2->input_port);
  }

  if(recall_lv2->output_port != NULL){
    free(recall_lv2->output_port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_lv2_parent_class)->finalize(gobject);
}

void
ags_recall_lv2_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsRecallLv2 *recall_lv2;
  AgsPort *current_port;
  
  AgsLv2Plugin *lv2_plugin;
  
  GList *list;  
  GList *plugin_port_start, *plugin_port;
  
  gchar *filename, *effect;
  
  guint port_count;
  guint i;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;
  pthread_mutex_t *port_mutex;

  recall_lv2 = AGS_RECALL_LV2(plugin);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_lv2)->filename);
  effect = g_strdup(AGS_RECALL(recall_lv2)->effect);
  
  pthread_mutex_unlock(recall_mutex);
  
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  g_free(filename);
  g_free(effect);

  /* set port */
  pthread_mutex_lock(recall_mutex);

  recall_lv2->plugin = lv2_plugin;

  pthread_mutex_unlock(recall_mutex);
  
  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

  /* get plugin port */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_port =
    plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

  pthread_mutex_unlock(base_plugin_mutex);

  /* match port */
  if(plugin_port != NULL){
    port_count = g_list_length(plugin_port_start);

    for(i = 0; i < port_count; i++){
      AgsPluginPort *current_plugin_port;
      
      pthread_mutex_t *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_CONTROL)){
	gchar *specifier;
	
	GValue *default_value;

	default_value = g_new0(GValue,
			       1);
	g_value_init(default_value,
		     G_TYPE_FLOAT);
		
	pthread_mutex_lock(plugin_port_mutex);

	specifier = g_strdup(current_plugin_port->port_name);
	g_value_copy(current_plugin_port->default_value,
		     default_value);
	
	pthread_mutex_unlock(plugin_port_mutex);

	list = ags_port_find_specifier(port, specifier);

	if(list != NULL){
	  current_port = list->data;
	  g_object_set(current_port,
		       "plugin-port", current_plugin_port,
		       NULL);
	  ags_recall_lv2_load_conversion(recall_lv2,
					 (GObject *) current_port,
					 current_plugin_port);

	  //TODO:JK: check raw write	  
	  ags_port_safe_write(current_port,
			      default_value);

#ifdef AGS_DEBUG
	  g_message("connecting port: %lu/%lu", i, port_count);
#endif
	}

	g_value_unset(default_value);
	g_free(default_value);
	g_free(specifier);
      }else if(ags_plugin_port_test_flags(current_plugin_port,
					  AGS_PLUGIN_PORT_AUDIO)){
	guint port_index;

	g_object_get(current_plugin_port,
		     "port-index", &port_index,
		     NULL);
	
	pthread_mutex_lock(recall_mutex);

	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_INPUT)){
	  if(recall_lv2->input_port == NULL){
	    recall_lv2->input_port = (guint *) malloc(sizeof(guint));
	    recall_lv2->input_port[0] = port_index;
	  }else{
	    recall_lv2->input_port = (guint *) realloc(recall_lv2->input_port,
						       (recall_lv2->input_lines + 1) * sizeof(guint));
	    recall_lv2->input_port[recall_lv2->input_lines] = port_index;
	  }

	  recall_lv2->input_lines += 1;
	}else if(ags_plugin_port_test_flags(current_plugin_port,
					    AGS_PLUGIN_PORT_OUTPUT)){
	  if(recall_lv2->output_port == NULL){
	    recall_lv2->output_port = (guint *) malloc(sizeof(guint));
	    recall_lv2->output_port[0] = port_index;
	  }else{
	    recall_lv2->output_port = (guint *) realloc(recall_lv2->output_port,
							(recall_lv2->output_lines + 1) * sizeof(guint));
	    recall_lv2->output_port[recall_lv2->output_lines] = port_index;
	  }

	  recall_lv2->output_lines += 1;
	}

	pthread_mutex_unlock(recall_mutex);
      }

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }

    /* reverse port */
    pthread_mutex_lock(recall_mutex);
    
    AGS_RECALL(recall_lv2)->port = g_list_reverse(port);

    pthread_mutex_unlock(recall_mutex);
  }

  g_list_free(plugin_port_start);
}

void
ags_recall_lv2_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsRecallLv2 *gobject;
  AgsLv2Plugin *lv2_plugin;

  gchar *uri;

  gobject = AGS_RECALL_LV2(plugin);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", xmlGetProp(node, AGS_FILE_ID_PROP)),
				   "reference", gobject,
				   NULL));
  uri = xmlGetProp(node,
		   "uri");

  g_object_set(gobject,
	       "uri", uri,
	       NULL);

  ags_recall_lv2_load(gobject);
}

xmlNode*
ags_recall_lv2_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsRecallLv2 *recall_lv2;

  xmlNode *node;

  gchar *id;

  pthread_mutex_t *recall_mutex;

  recall_lv2 = AGS_RECALL_LV2(plugin);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /*  */
  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-lv2");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context", file->application_context,
				   "file", file,
				   "node", node,
				   "xpath", g_strdup_printf("xpath=//*[@id='%s']", id),
				   "reference", recall_lv2,
				   NULL));

  pthread_mutex_lock(recall_mutex);

  xmlNewProp(node,
	     "uri",
	     g_strdup(recall_lv2->uri));

  pthread_mutex_unlock(recall_mutex);

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_recall_lv2_test_flags:
 * @recall_lv2: the #AgsRecallLv2
 * @flags: the flags
 * 
 * Test @flags to be set on @recall.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 * 
 * Since: 2.0.0
 */
gboolean
ags_recall_lv2_test_flags(AgsRecallLv2 *recall_lv2, guint flags)
{
  gboolean retval;
  
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_RECALL_LV2(recall_lv2)){
    return(FALSE);
  }
  
  /* get base plugin mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* test flags */
  pthread_mutex_lock(recall_mutex);

  retval = ((flags & (recall_lv2->flags)) != 0) ? TRUE: FALSE;
  
  pthread_mutex_unlock(recall_mutex);

  return(retval);
}

/**
 * ags_recall_lv2_set_flags:
 * @recall_lv2: the #AgsRecallLv2
 * @flags: the flags
 *
 * Set flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_lv2_set_flags(AgsRecallLv2 *recall_lv2, guint flags)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_RECALL_LV2(recall_lv2)){
    return;
  }
  
  /* get base plugin mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* set flags */
  pthread_mutex_lock(recall_mutex);

  recall_lv2->flags |= flags;
  
  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_lv2_unset_flags:
 * @recall_lv2: the #AgsRecallLv2
 * @flags: the flags
 *
 * Unset flags.
 * 
 * Since: 2.0.0
 */
void
ags_recall_lv2_unset_flags(AgsRecallLv2 *recall_lv2, guint flags)
{
  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_RECALL_LV2(recall_lv2)){
    return;
  }
  
  /* get base plugin mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* unset flags */
  pthread_mutex_lock(recall_mutex);

  recall_lv2->flags &= (~flags);
  
  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_recall_lv2_load:
 * @recall_lv2: the #AgsRecallLv2
 *
 * Set up LV2 handle.
 * 
 * Since: 2.0.0
 */
void
ags_recall_lv2_load(AgsRecallLv2 *recall_lv2)
{
  AgsLv2Plugin *lv2_plugin;

  gchar *filename;
  gchar *effect;

  guint effect_index;
  guint i;
  
  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  pthread_mutex_t *recall_mutex;

  if(!AGS_IS_RECALL_LV2(recall_lv2)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);
  
  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_lv2)->filename);
  effect = g_strdup(AGS_RECALL(recall_lv2)->effect);

  pthread_mutex_unlock(recall_mutex);
  
  /* find lv2 plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);

  g_object_get(lv2_plugin,
	       "plugin-so", &plugin_so,
	       NULL);

  if(plugin_so == NULL){
    g_message("open %s", filename);
    
    plugin_so = dlopen(filename,
		       RTLD_NOW);
    
    g_object_set(lv2_plugin,
		 "plugin-so", plugin_so,
		 NULL);
  }
  
  g_free(filename);
  g_free(effect);

  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor");

    if(dlerror() == NULL && lv2_descriptor){
      effect_index = 0;
  
      for(i = 0; (plugin_descriptor = lv2_descriptor((unsigned long) i)) != NULL; i++){
	 if(!g_ascii_strcasecmp(plugin_descriptor->URI,
				lv2_plugin->uri)){
	   effect_index = i;

	   g_object_set(lv2_plugin,
			"effect-index", effect_index,
			NULL);
	   
	   break;
	 }
      }
      
      pthread_mutex_lock(recall_mutex);
      
      recall_lv2->plugin_descriptor = plugin_descriptor;

      pthread_mutex_unlock(recall_mutex);
      
      if(ags_lv2_plugin_test_flags(lv2_plugin, AGS_LV2_PLUGIN_NEEDS_WORKER)){
	ags_recall_lv2_set_flags(recall_lv2,
				 AGS_RECALL_LV2_HAS_WORKER);
      }
    }
  }
}

/**
 * ags_recall_lv2_load_ports:
 * @recall_lv2: the #AgsRecallLv2
 *
 * Set up LV2 ports.
 *
 * Returns: a #GList-struct containing #AgsPort
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_lv2_load_ports(AgsRecallLv2 *recall_lv2)
{
  AgsPort *current_port;

  AgsLv2Plugin *lv2_plugin;

  GList *port, *retval;
  GList *plugin_port_start, *plugin_port;

  gchar *filename;
  gchar *effect;

  guint effect_index;
  
  guint port_count;
  guint i;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  if(!AGS_IS_RECALL_LV2(recall_lv2)){
    return(NULL);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_lv2);

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_lv2)->filename);
  effect = g_strdup(AGS_RECALL(recall_lv2)->effect);
  
  effect_index = AGS_RECALL(recall_lv2)->effect_index;

  pthread_mutex_unlock(recall_mutex);

  /* find lv2 plugin */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       filename, effect);
  g_free(filename);
  g_free(effect);
  
  /* set lv2 plugin */
  pthread_mutex_lock(recall_mutex);

  recall_lv2->plugin = lv2_plugin;

  pthread_mutex_unlock(recall_mutex);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

  /* get port descriptor */
  pthread_mutex_lock(base_plugin_mutex);

  plugin_port =
    plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(lv2_plugin)->plugin_port);

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
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      if(ags_plugin_port_test_flags(current_plugin_port, AGS_PLUGIN_PORT_INPUT)){
	if(ags_plugin_port_test_flags(current_plugin_port, AGS_PLUGIN_PORT_EVENT)){
	  guint port_index;

	  ags_recall_lv2_set_flags(recall_lv2, AGS_RECALL_LV2_HAS_EVENT_PORT);
	  
	  g_object_get(current_plugin_port,
		       "port-index", &port_index,
		       NULL);
	  
	  g_object_set(recall_lv2,
		       "event-port", port_index,
		       NULL);
	}
      
	if(ags_plugin_port_test_flags(current_plugin_port, AGS_PLUGIN_PORT_ATOM)){
	  guint port_index;
	  
	  ags_recall_lv2_set_flags(recall_lv2, AGS_RECALL_LV2_HAS_ATOM_PORT);

	  g_object_get(current_plugin_port,
		       "port-index", &port_index,
		       NULL);
	  
	  g_object_set(recall_lv2,
		       "atom-port", port_index,
		       NULL);
	}
      }
            
      if(ags_plugin_port_test_flags(current_plugin_port, AGS_PLUGIN_PORT_CONTROL) &&
	 (ags_plugin_port_test_flags(current_plugin_port, AGS_PLUGIN_PORT_INPUT) ||
	  ags_plugin_port_test_flags(current_plugin_port, AGS_PLUGIN_PORT_OUTPUT))){
	gchar *plugin_name;
	gchar *specifier;
	  
	GValue *default_value;

	default_value = g_new0(GValue,
			       1);
	g_value_init(default_value,
		     G_TYPE_FLOAT);
		
	pthread_mutex_lock(plugin_port_mutex);

	specifier = g_strdup(current_plugin_port->port_name);
	g_value_copy(current_plugin_port->default_value,
		     default_value);
	
	pthread_mutex_unlock(plugin_port_mutex);

	if(specifier == NULL){
	  g_value_unset(default_value);
	  g_free(default_value);
	  
	  plugin_port = plugin_port->next;
	  
	  continue;
	}
	
	plugin_name = g_strdup_printf("lv2-<%s>", lv2_plugin->uri);

	current_port = g_object_new(AGS_TYPE_PORT,
				    "plugin-name", plugin_name,
				    "specifier", specifier,
				    "control-port", g_strdup_printf("%u/%u",
								    i,
								    port_count),
				    "port-value-is-pointer", FALSE,
				    "port-value-type", G_TYPE_FLOAT,
				    NULL);
	g_object_ref(current_port);
	
	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_OUTPUT)){
	  current_port->flags |= AGS_PORT_IS_OUTPUT;

	  ags_recall_set_flags((AgsRecall *) recall_lv2,
			       AGS_RECALL_HAS_OUTPUT_PORT);
	}else{
	  gint scale_steps;

	  g_object_get(current_plugin_port,
		       "scale-steps", &scale_steps,
		       NULL);
	  
	  if(!ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_TOGGLED) &&
	     scale_steps == -1){
	    current_port->flags |= AGS_PORT_INFINITE_RANGE;
	  }
	}
	
	g_object_set(current_port,
		     "plugin-port", current_plugin_port,
		     NULL);

	ags_recall_lv2_load_conversion(recall_lv2,
				       (GObject *) current_port,
				       current_plugin_port);

	ags_port_safe_write_raw(current_port,
				default_value);

#ifdef AGS_DEBUG
	g_message("connecting port: %s %d/%d", specifier, i, port_count);
#endif
	
	port = g_list_prepend(port,
			      current_port);

	g_value_unset(default_value);
	g_free(default_value);
	g_free(plugin_name);
	g_free(specifier);
      }else if(ags_plugin_port_test_flags(current_plugin_port,
					  AGS_PLUGIN_PORT_AUDIO)){
	guint port_index;

	g_object_get(current_plugin_port,
		     "port-index", &port_index,
		     NULL);

	pthread_mutex_lock(recall_mutex);

	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_INPUT)){
	  if(recall_lv2->input_port == NULL){
	    recall_lv2->input_port = (guint *) malloc(sizeof(guint));
	    recall_lv2->input_port[0] = port_index;
	  }else{
	    recall_lv2->input_port = (guint *) realloc(recall_lv2->input_port,
						       (recall_lv2->input_lines + 1) * sizeof(guint));
	    recall_lv2->input_port[recall_lv2->input_lines] = port_index;
	  }
	  
	  recall_lv2->input_lines += 1;
	}else if(ags_plugin_port_test_flags(current_plugin_port,
					    AGS_PLUGIN_PORT_OUTPUT)){
	  if(recall_lv2->output_port == NULL){
	    recall_lv2->output_port = (guint *) malloc(sizeof(guint));
	    recall_lv2->output_port[0] = port_index;
	  }else{
	    recall_lv2->output_port = (guint *) realloc(recall_lv2->output_port,
							(recall_lv2->output_lines + 1) * sizeof(guint));
	    recall_lv2->output_port[recall_lv2->output_lines] = port_index;
	  }
	  
	  recall_lv2->output_lines += 1;
	}

	pthread_mutex_unlock(recall_mutex);
      }
      
      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
    
    /* reverse port */
    pthread_mutex_lock(recall_mutex);
    
    AGS_RECALL(recall_lv2)->port = g_list_reverse(port);
    
    retval = g_list_copy(AGS_RECALL(recall_lv2)->port);
    
    pthread_mutex_unlock(recall_mutex);
  }
  
  g_list_free(plugin_port_start);

  return(retval);
}

/**
 * ags_recall_lv2_load_conversion:
 * @recall_lv2: the #AgsRecallLv2
 * @port: the #AgsPort
 * @plugin_port: the #AgsPluginPort
 * 
 * Loads conversion object by using @plugin_port and sets in on @port.
 * 
 * Since: 2.0.0
 */
void
ags_recall_lv2_load_conversion(AgsRecallLv2 *recall_lv2,
			       GObject *port,
			       gpointer plugin_port)
{
  AgsLv2Conversion *lv2_conversion;

  if(!AGS_IS_RECALL_LV2(recall_lv2) ||
     !AGS_IS_PORT(port) ||
     !AGS_IS_PLUGIN_PORT(plugin_port)){
    return;
  }

  lv2_conversion = NULL;
  
  if(ags_plugin_port_test_flags(plugin_port,
				AGS_PLUGIN_PORT_LOGARITHMIC)){
    if(!AGS_IS_LV2_CONVERSION(lv2_conversion)){
      lv2_conversion = ags_lv2_conversion_new();
    }
    
    lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;
  }

  if(lv2_conversion != NULL){
    g_object_set(port,
		 "conversion", lv2_conversion,
		 NULL);
  }
}

/**
 * ags_recall_lv2_find:
 * @recall: the #GList-struct containing #AgsRecall
 * @filename: plugin filename
 * @effect: effect's name
 *
 * Retrieve LV2 recall.
 *
 * Returns: Next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_lv2_find(GList *recall,
		     gchar *filename, gchar *effect)
{
  gboolean success;
  
  pthread_mutex_t *recall_mutex;

  while(recall != NULL){
    if(AGS_IS_RECALL_LV2(recall->data)){
      /* get recall mutex */
      recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall->data);

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
 * ags_recall_lv2_new:
 * @source: the #AgsChannel as source
 * @turtle: the RDF turtle file
 * @filename: the LV2 plugin filename
 * @effect: the LV2 plugin effect
 * @uri: uri's name
 * @effect_index: effect's index
 *
 * Creates a new instance of #AgsRecallLv2
 *
 * Returns: the new #AgsRecallLv2
 * 
 * Since: 2.0.0
 */
AgsRecallLv2*
ags_recall_lv2_new(AgsChannel *source,
		   AgsTurtle *turtle,
		   gchar *filename,
		   gchar *effect,
		   gchar *uri,
		   uint32_t index)
{
  AgsRecallLv2 *recall_lv2;

  GObject *output_soundcard;

  output_soundcard = NULL;

  if(source != NULL){
    g_object_get(source,
		 "output-soundcard", &output_soundcard,
		 NULL);
  }

  recall_lv2 = (AgsRecallLv2 *) g_object_new(AGS_TYPE_RECALL_LV2,
					     "output-soundcard", output_soundcard,
					     "turtle", turtle,
					     "source", source,
					     "filename", filename,
					     "effect", effect,
					     "uri", uri,
					     "effect-index", index,
					     NULL);

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  return(recall_lv2);
}
