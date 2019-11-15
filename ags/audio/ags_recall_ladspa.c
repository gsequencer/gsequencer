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

#include <ags/audio/ags_recall_ladspa.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_plugin_port.h>
#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/audio/ags_port.h>

#if defined(AGS_W32API)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_recall_ladspa_class_init(AgsRecallLadspaClass *recall_ladspa_class);
void ags_recall_ladspa_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa);
void ags_recall_ladspa_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_recall_ladspa_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_recall_ladspa_finalize(GObject *gobject);

/**
 * SECTION:ags_recall_ladspa
 * @short_description: The object interfacing with LADSPA
 * @title: AgsRecallLadspa
 * @section_id:
 * @include: ags/audio/ags_recall_ladspa.h
 *
 * #AgsRecallLadspa provides LADSPA support.
 */

static gpointer ags_recall_ladspa_parent_class = NULL;
static AgsConnectableInterface* ags_recall_ladspa_parent_connectable_interface;

enum{
  PROP_0,
  PROP_PLUGIN,
};

GType
ags_recall_ladspa_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_ladspa = 0;

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

    ags_type_recall_ladspa = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						    "AgsRecallLadspa",
						    &ags_recall_ladspa_info,
						    0);

    g_type_add_interface_static(ags_type_recall_ladspa,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_ladspa);
  }

  return g_define_type_id__volatile;
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
   * AgsRecallLadspa:plugin:
   *
   * The assigned plugin.
   * 
   * Since: 2.1.53
   */
  param_spec = g_param_spec_object("plugin",
				   i18n_pspec("plugin of recall ladspa"),
				   i18n_pspec("The plugin which this recall ladspa does run"),
				   AGS_TYPE_LADSPA_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN,
				  param_spec);
}

void
ags_recall_ladspa_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_ladspa_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa)
{
  AGS_RECALL(recall_ladspa)->name = "ags-ladspa";
  AGS_RECALL(recall_ladspa)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(recall_ladspa)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(recall_ladspa)->xml_type = "ags-recall-ladspa";
  AGS_RECALL(recall_ladspa)->port = NULL;

  recall_ladspa->plugin = NULL;
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

  GRecMutex *recall_mutex;

  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  switch(prop_id){
  case PROP_PLUGIN:
    {
      AgsLadspaPlugin *plugin;

      plugin = (AgsLadspaPlugin *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(recall_ladspa->plugin == plugin){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(recall_ladspa->plugin != NULL){
	g_object_unref(recall_ladspa->plugin);
      }

      if(plugin != NULL){
	g_object_ref(plugin);
      }

      recall_ladspa->plugin = plugin;
      
      g_rec_mutex_unlock(recall_mutex);
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

  GRecMutex *recall_mutex;

  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  switch(prop_id){
  case PROP_PLUGIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_ladspa->plugin);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_recall_ladspa_finalize(GObject *gobject)
{
  AgsRecallLadspa *recall_ladspa;
  
  recall_ladspa = AGS_RECALL_LADSPA(gobject);

  if(recall_ladspa->input_port != NULL){
    free(recall_ladspa->input_port);
  }

  if(recall_ladspa->output_port != NULL){
    free(recall_ladspa->output_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_parent_class)->finalize(gobject);
}

/**
 * ags_recall_ladspa_load:
 * @recall_ladspa: the #AgsRecallLadspa
 *
 * Set up LADSPA handle.
 * 
 * Since: 2.0.0
 */
void
ags_recall_ladspa_load(AgsRecallLadspa *recall_ladspa)
{
  AgsLadspaPlugin *ladspa_plugin;

  gchar *filename;
  gchar *effect;

  guint effect_index;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;

  GRecMutex *recall_mutex;

  if(!AGS_IS_RECALL_LADSPA(recall_ladspa)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_ladspa)->filename);
  effect = g_strdup(AGS_RECALL(recall_ladspa)->effect);
  
  effect_index = AGS_RECALL(recall_ladspa)->effect_index;

  g_rec_mutex_unlock(recall_mutex);
  
  /* find ladspa plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  g_free(filename);
  g_free(effect);

  g_object_get(ladspa_plugin,
	       "plugin-so", &plugin_so,
	       NULL);

  if(plugin_so){
    gboolean success;
    
    success = FALSE;
    
#ifdef AGS_W32API
    ladspa_descriptor = (LADSPA_Descriptor_Function) GetProcAddress((void *) plugin_so,
								    "ladspa_descriptor");

    success = (!ladspa_descriptor) ? FALSE: TRUE;
#else
    ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym((void *) plugin_so,
							   "ladspa_descriptor");
  
    success = (dlerror() == NULL) ? TRUE: FALSE;
#endif

    if(success && ladspa_descriptor){
      g_rec_mutex_lock(recall_mutex);

      recall_ladspa->plugin_descriptor = 
	plugin_descriptor = ladspa_descriptor(effect_index);

      g_rec_mutex_unlock(recall_mutex);
    }
  }
}

/**
 * ags_recall_ladspa_load_ports:
 * @recall_ladspa: the #AgsRecallLadspa
 *
 * Set up LADSPA ports.
 *
 * Returns: a #GList-struct containing #AgsPort
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_ladspa_load_ports(AgsRecallLadspa *recall_ladspa)
{
  AgsPort *current_port;

  AgsLadspaPlugin *ladspa_plugin;

  GList *port, *retval;
  GList *plugin_port_start, *plugin_port;

  gchar *filename;
  gchar *effect;

  guint effect_index;
  
  guint port_count;
  guint i;

  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_RECALL_LADSPA(recall_ladspa)){
    return(NULL);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(recall_ladspa)->filename);
  effect = g_strdup(AGS_RECALL(recall_ladspa)->effect);
  
  effect_index = AGS_RECALL(recall_ladspa)->effect_index;

  g_rec_mutex_unlock(recall_mutex);

  /* find ladspa plugin */
  ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ags_ladspa_manager_get_instance(),
							filename, effect);
  g_free(filename);
  g_free(effect);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin);

  /* get port descriptor */
  g_rec_mutex_lock(base_plugin_mutex);

  plugin_port =
    plugin_port_start = g_list_copy(AGS_BASE_PLUGIN(ladspa_plugin)->plugin_port);

  g_rec_mutex_unlock(base_plugin_mutex);

  port = NULL;
  retval = NULL;

  if(plugin_port != NULL){
    port_count = g_list_length(plugin_port_start);
    
    for(i = 0; i < port_count; i++){
      AgsPluginPort *current_plugin_port;
      
      GRecMutex *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_CONTROL)){
	gchar *plugin_name;
	gchar *specifier;

	GValue *default_value;
	
	plugin_name = g_strdup_printf("ladspa-%u", ladspa_plugin->unique_id);

	default_value = g_new0(GValue,
			       1);
	g_value_init(default_value,
		     G_TYPE_FLOAT);
	
	g_rec_mutex_lock(plugin_port_mutex);

	specifier = g_strdup(current_plugin_port->port_name);
	g_value_copy(current_plugin_port->default_value,
		     default_value);
	
	g_rec_mutex_unlock(plugin_port_mutex);

	current_port = g_object_new(AGS_TYPE_PORT,
				    "plugin-name", plugin_name,
				    "specifier", specifier,
				    "control-port", g_strdup_printf("%u/%u",
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

	  ags_recall_set_flags((AgsRecall *) recall_ladspa,
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
	
	ags_recall_ladspa_load_conversion(recall_ladspa,
					  (GObject *) current_port,
					  current_plugin_port);
	
	ags_port_safe_write_raw(current_port,
				default_value);

	port = g_list_prepend(port,
			      current_port);

	g_value_unset(default_value);
	g_free(default_value);
	g_free(plugin_name);
	g_free(specifier);
      }else if(ags_plugin_port_test_flags(current_plugin_port,
					  AGS_PLUGIN_PORT_AUDIO)){
	g_rec_mutex_lock(recall_mutex);

	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_INPUT)){
	  if(recall_ladspa->input_port == NULL){
	    recall_ladspa->input_port = (guint *) malloc(sizeof(guint));
	    recall_ladspa->input_port[0] = i;
	  }else{
	    recall_ladspa->input_port = (guint *) realloc(recall_ladspa->input_port,
							  (recall_ladspa->input_lines + 1) * sizeof(guint));
	    recall_ladspa->input_port[recall_ladspa->input_lines] = i;
	  }
	  
	  recall_ladspa->input_lines += 1;
	}else if(ags_plugin_port_test_flags(current_plugin_port,
					    AGS_PLUGIN_PORT_OUTPUT)){
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

	g_rec_mutex_unlock(recall_mutex);
      }

      /* iterate plugin port */
      plugin_port = plugin_port->next;
    }
    
    /* reverse port */
    g_rec_mutex_lock(recall_mutex);
    
    AGS_RECALL(recall_ladspa)->port = g_list_reverse(port);
    
    retval = g_list_copy(AGS_RECALL(recall_ladspa)->port);
    
    g_rec_mutex_unlock(recall_mutex);
  }

  g_list_free(plugin_port_start);

  return(retval);
}

/**
 * ags_recall_ladspa_load_conversion:
 * @recall_ladspa: the #AgsRecallLadspa
 * @port: the #AgsPort
 * @plugin_port: the #AgsPluginPort
 * 
 * Loads conversion object by using @plugin_port and sets in on @port.
 * 
 * Since: 2.0.0
 */
void
ags_recall_ladspa_load_conversion(AgsRecallLadspa *recall_ladspa,
				  GObject *port,
				  gpointer plugin_port)
{
  AgsLadspaConversion *ladspa_conversion;

  if(!AGS_IS_RECALL_LADSPA(recall_ladspa) ||
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
 * ags_recall_ladspa_find:
 * @recall: the #GList-struct containing #AgsRecall
 * @filename: plugin filename
 * @effect: effect's name
 *
 * Retrieve LADSPA recall.
 *
 * Returns: Next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_recall_ladspa_find(GList *recall,
		       gchar *filename, gchar *effect)
{
  gboolean success;
  
  GRecMutex *recall_mutex;

  while(recall != NULL){
    if(AGS_IS_RECALL_LADSPA(recall->data)){
      /* get recall mutex */
      recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall->data);

      /* check filename and effect */
      g_rec_mutex_lock(recall_mutex);
      
      success = (!g_strcmp0(AGS_RECALL(recall->data)->filename,
			    filename) &&
		 !g_strcmp0(AGS_RECALL(recall->data)->effect,
			    effect)) ? TRUE: FALSE;

      g_rec_mutex_unlock(recall_mutex);
      
      if(success){
	return(recall);
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_recall_ladspa_new:
 * @source: the #AgsChannel as source
 * @filename: the LADSPA plugin filename
 * @effect: effect's name
 * @effect_index: effect's index
 *
 * Creates a new instance of #AgsRecallLadspa
 *
 * Returns: the new #AgsRecallLadspa
 * 
 * Since: 2.0.0
 */
AgsRecallLadspa*
ags_recall_ladspa_new(AgsChannel *source,
		      gchar *filename,
		      gchar *effect,
		      guint effect_index)
{
  AgsRecallLadspa *recall_ladspa;

  GObject *output_soundcard;

  output_soundcard = NULL;

  if(source != NULL){
    g_object_get(source,
		 "output-soundcard", &output_soundcard,
		 NULL);
  }

  recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
						   "output-soundcard", output_soundcard,
						   "source", source,
						   "filename", filename,
						   "effect", effect,
						   "effect-index", effect_index,
						   NULL);

  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  return(recall_ladspa);
}
