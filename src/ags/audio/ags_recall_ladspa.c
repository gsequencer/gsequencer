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

#include <ags-lib/object/ags_connectable.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_recall_ladspa_parent_class = NULL;
static AgsConnectableInterface* ags_recall_ladspa_parent_connectable_interface;

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

    ags_type_recall_ladspa = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
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
  param_spec =  g_param_spec_string("filename\0",
				    "the object file\0",
				    "The filename as string of object file\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec =  g_param_spec_string("effect\0",
				    "the effect\0",
				    "The effect's string representation\0",
				    NULL,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

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
  connectable->connect = ags_recall_ladspa_connect;
  connectable->disconnect = ags_recall_ladspa_disconnect;
}

void
ags_recall_ladspa_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_recall_ladspa_set_ports;
}

void
ags_recall_ladspa_init(AgsRecallLadspa *recall_ladspa)
{
  recall_ladspa->filename = NULL;
  recall_ladspa->effect = NULL;
  recall_ladspa->index = 0;

  recall_ladspa->plugin_descriptor = NULL;
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
      gchar *filename;
      
      filename = g_value_get_string(value);

      if(filename == recall_ladspa->filename){
	return;
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
      g_value_set_string(value, recall_ladspa->index);
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
  AgsLadspaPlugin *ladspa_plugin;
  AgsRecallLadspa *recall_ladspa;
  AgsPort *current;
  gchar *path;
  gchar *plugin_name;
  guint port_count;
  guint i;

  void *plugin_so;
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHintDescriptor hint_descriptor;
  LADSPA_Data lower_bound, upper_bound;

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

      while(port != NULL){
	for(i = 0; i < port_count; i++){
	  plugin_name = g_strdup_printf("ladspa-%s\0", plugin_descriptor->UniqueID);

	  if(!g_strcmp0(plugin_name,
			AGS_PORT(port->data)->plugin_name)){
	    if(LADSPA_IS_PORT_INPUT(port_descriptor[i])){
	      //TODO:JK: implement me
	    }else if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	      //TODO:JK: implement me
	    }else if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	      current = AGS_PORT(port->data);

	      plugin_descriptor->connect_port(plugin_descriptor,
					      i,
					      &(current->port_value.ags_port_double));
	    }else if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
	      //TODO:JK: implement me
	    }

	    g_free(plugin_name);

	    break;
	  }else{
	    g_free(plugin_name);
	  }
	}

	port = port->next;
      }
    }
  }

  AGS_RECALL(recall_ladspa)->port = port;
}

void
ags_recall_ladspa_finalize(GObject *gobject)
{
  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_parent_class)->finalize(gobject);
}

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
	    specifier = g_strdelimit(g_strdup(plugin_descriptor->PortNames[i]),
				     NULL,
				     '-');

	    current = g_object_new(AGS_TYPE_PORT,
				   "plugin-name\0", plugin_name,
				   "specifier\0", g_strdup_printf("./%s\0", specifier),
				   "control-port\0", g_strdup_printf("%d/%d\0",
								     i,
								     port_count),
				   "port-value-is-pointer\0", FALSE,
				   "port-value-type\0", G_TYPE_FLOAT,
				   NULL);
	    current->port_value.ags_port_float = plugin_descriptor->PortRangeHints[i].LowerBound;

	    g_message("%d/%d\0", i, port_count);
	    plugin_descriptor->connect_port(plugin_descriptor,
					    i,
					    &(current->port_value.ags_port_float));

	    port = g_list_prepend(port,
				  current);
	  }
	}else if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
	  if(LADSPA_IS_PORT_INPUT(port_descriptor[i])){
	    recall_ladspa->input_port = i;
	  }else if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	    recall_ladspa->output_port = i;
	  }
	}
      }

      AGS_RECALL(recall_ladspa)->port = g_list_reverse(port);
    }
  }

  return(AGS_RECALL(recall_ladspa)->port);
}

void
ags_recall_ladspa_short_to_float(signed short *buffer,
				 float *destination)
{
  float *new_buffer;
  guint i;

  new_buffer = destination;

  for(i = 0; i < AGS_DEVOUT_DEFAULT_BUFFER_SIZE; i++){
    new_buffer[i] += buffer[i] / (G_MAXFLOAT / G_MAXINT16);
  }
}

void
ags_recall_ladspa_float_to_short(float *buffer,
				 signed short *destination)
{
  signed short *new_buffer;
  guint i;

  new_buffer = destination;

  for(i = 0; i < AGS_DEVOUT_DEFAULT_BUFFER_SIZE; i++){
    new_buffer[i] += buffer[i] * (G_MAXFLOAT / G_MAXINT16);
  }

  return(new_buffer);
}

AgsRecallLadspa*
ags_recall_ladspa_new(AgsChannel *source,
		      gchar *filename,
		      gchar *effect,
		      guint index)
{
  AgsRecallLadspa *recall_ladspa;

  recall_ladspa = (AgsRecallLadspa *) g_object_new(AGS_TYPE_RECALL_LADSPA,
						   "source\0", source,
						   "filename\0", filename,
						   "effect\0", effect,
						   "index\0", index,
						   NULL);

  return(recall_ladspa);
}
