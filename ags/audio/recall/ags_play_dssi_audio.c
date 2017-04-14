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

#include <ags/audio/recall/ags_play_dssi_audio.h>

#include <ags/util/ags_id_generator.h>

#include <ags/lib/ags_endian.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_ladspa_conversion.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>

#include <ags/audio/ags_port.h>

#include <endian.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

void ags_play_dssi_audio_class_init(AgsPlayDssiAudioClass *play_dssi_audio);
void ags_play_dssi_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_dssi_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_dssi_audio_init(AgsPlayDssiAudio *play_dssi_audio);
void ags_play_dssi_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_play_dssi_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_play_dssi_audio_finalize(GObject *gobject);
void ags_play_dssi_audio_connect(AgsConnectable *connectable);
void ags_play_dssi_audio_disconnect(AgsConnectable *connectable);
void ags_play_dssi_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_play_dssi_audio
 * @short_description: play audio dssi
 * @title: AgsPlayDssiAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_dssi_audio.h
 *
 * The #AgsPlayDssiAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_play_dssi_audio_parent_class = NULL;
static AgsConnectableInterface* ags_play_dssi_audio_parent_connectable_interface;
static AgsPluginInterface *ags_play_dssi_parent_plugin_interface;

static const gchar *ags_play_dssi_audio_plugin_name = "ags-play-dssi\0";
static const gchar *ags_play_dssi_audio_specifier[] = {
};
static const gchar *ags_play_dssi_audio_control_port[] = {
};

GType
ags_play_dssi_audio_get_type()
{
  static GType ags_type_play_dssi_audio = 0;

  if(!ags_type_play_dssi_audio){
    static const GTypeInfo ags_play_dssi_audio_info = {
      sizeof (AgsPlayDssiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_dssi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayDssiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_dssi_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_dssi_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_dssi_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_dssi_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsPlayDssiAudio\0",
						      &ags_play_dssi_audio_info,
						      0);

    g_type_add_interface_static(ags_type_play_dssi_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_dssi_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_dssi_audio);
}

void
ags_play_dssi_audio_class_init(AgsPlayDssiAudioClass *play_dssi_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_dssi_audio_parent_class = g_type_class_peek_parent(play_dssi_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_dssi_audio;

  gobject->set_property = ags_play_dssi_audio_set_property;
  gobject->get_property = ags_play_dssi_audio_get_property;

  gobject->finalize = ags_play_dssi_audio_finalize;

  /* properties */
  /**
   * AgsPlayDssiAudio:filename:
   *
   * The plugins filename.
   * 
   * Since: 0.7.134
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
   * AgsPlayDssiAudio:effect:
   *
   * The effect's name.
   * 
   * Since: 0.7.134
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
   * AgsPlayDssiAudio:index:
   *
   * The effect's index.
   * 
   * Since: 0.7.134
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
ags_play_dssi_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_dssi_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_dssi_audio_connect;
  connectable->disconnect = ags_play_dssi_audio_disconnect;
}

void
ags_play_dssi_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_dssi_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_play_dssi_audio_set_ports;
}

void
ags_play_dssi_audio_init(AgsPlayDssiAudio *play_dssi_audio)
{
  GList *port;

  AGS_RECALL(play_dssi_audio)->name = "ags-play-dssi\0";
  AGS_RECALL(play_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_dssi_audio)->xml_type = "ags-play-dssi-audio\0";

  play_dssi_audio->filename = NULL;
  play_dssi_audio->effect = NULL;
  play_dssi_audio->index = 0;
  
  play_dssi_audio->bank = 0;
  play_dssi_audio->program = 0;
  
  play_dssi_audio->plugin_descriptor = NULL;

  play_dssi_audio->input_port = NULL;
  play_dssi_audio->input_lines = 0;

  play_dssi_audio->output_port = NULL;
  play_dssi_audio->output_lines = 0;
}

void
ags_play_dssi_audio_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_play_dssi_audio_parent_class)->finalize(gobject);
}

void
ags_play_dssi_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      GObject *soundcard;
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == play_dssi_audio->filename){
	return;
      }

      if(play_dssi_audio->filename != NULL){
	g_free(play_dssi_audio->filename);
      }

      play_dssi_audio->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == play_dssi_audio->effect){
	return;
      }

      play_dssi_audio->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long index;
      
      index = g_value_get_ulong(value);

      if(index == play_dssi_audio->index){
	return;
      }

      play_dssi_audio->index = index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_dssi_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, play_dssi_audio->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, play_dssi_audio->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_uint(value, play_dssi_audio->index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_dssi_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    port = port->next;
  }
}

void
ags_play_dssi_audio_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;

  recall = AGS_RECALL(connectable);
  
  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    return;
  }

  ags_recall_load_automation(recall,
			     g_list_copy(recall->port));
  
  ags_play_dssi_audio_parent_connectable_interface->connect(connectable);
}

void
ags_play_dssi_audio_disconnect(AgsConnectable *connectable)
{
  ags_play_dssi_audio_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_dssi_audio_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsPlayDssiAudio *gobject;
  AgsDssiPlugin *dssi_plugin;
  gchar *filename, *effect;
  unsigned long index;

  gobject = AGS_PLAY_DSSI_AUDIO(plugin);

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

  ags_play_dssi_audio_load(gobject);
}

xmlNode*
ags_play_dssi_audio_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsPlayDssiAudio *play_dssi_audio;
  xmlNode *node;
  gchar *id;

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(plugin);

  id = ags_id_generator_create_uuid();
  
  node = xmlNewNode(NULL,
		    "ags-recall-dssi\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", play_dssi_audio,
				   NULL));

  xmlNewProp(node,
	     "filename\0",
	     g_strdup(play_dssi_audio->filename));

  xmlNewProp(node,
	     "effect\0",
	     g_strdup(play_dssi_audio->effect));

  xmlNewProp(node,
	     "index\0",
	     g_strdup_printf("%lu\0", play_dssi_audio->index));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_play_dssi_audio_load:
 * @play_dssi_audio: an #AgsPlayDssiAudio
 *
 * Set up DSSI handle.
 * 
 * Since: 0.7.134
 */
void
ags_play_dssi_audio_load(AgsPlayDssiAudio *play_dssi_audio)
{
  AgsDssiPlugin *dssi_plugin;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;

  /*  */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  play_dssi_audio->filename, play_dssi_audio->effect);
  
  plugin_so = AGS_BASE_PLUGIN(dssi_plugin)->plugin_so;

  if(plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor\0");

    if(dlerror() == NULL && dssi_descriptor){
      play_dssi_audio->plugin_descriptor = 
	plugin_descriptor = dssi_descriptor(play_dssi_audio->index);
    }
  }
}

/**
 * ags_play_dssi_audio_load_ports:
 * @play_dssi_audio: an #AgsPlayDssiAudio
 *
 * Set up DSSI ports.
 *
 * Returns: a #GList containing #AgsPort.
 * 
 * Since: 0.7.134
 */
GList*
ags_play_dssi_audio_load_ports(AgsPlayDssiAudio *play_dssi_audio)
{
  AgsDssiPlugin *dssi_plugin;
  AgsPort *current;

  GList *port;
  GList *port_descriptor;

  unsigned long port_count;
  unsigned long i;

  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  play_dssi_audio->filename, play_dssi_audio->effect);

  port = NULL;
  port_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->port;
  
  if(port_descriptor != NULL){
    port_count = g_list_length(port_descriptor);
    
    for(i = 0; i < port_count; i++){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	gchar *plugin_name;
	gchar *specifier;
	
	plugin_name = g_strdup_printf("dssi-%lu\0", dssi_plugin->unique_id);
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;

	current = g_object_new(AGS_TYPE_PORT,
			       "plugin-name\0", plugin_name,
			       "specifier\0", specifier,
			       "control-port\0", g_strdup_printf("%lu/%lu\0",
								 i,
								 port_count),
			       "port-value-is-pointer\0", FALSE,
			       "port-value-type\0", G_TYPE_FLOAT,
			       NULL);
	g_object_ref(current);
	current->flags |= AGS_PORT_USE_LADSPA_FLOAT;

	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  AGS_RECALL(play_dssi_audio)->flags |= AGS_RECALL_HAS_OUTPUT_PORT;

	  current->flags |= AGS_PORT_IS_OUTPUT;
	}else{
	  if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) == 0 &&
	     (AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) == 0){
	    current->flags |= AGS_PORT_INFINITE_RANGE;
	  }
	}
	
	current->port_descriptor = port_descriptor->data;
	ags_play_dssi_audio_load_conversion(play_dssi_audio,
					    (GObject *) current,
					    port_descriptor->data);
	
	current->port_value.ags_port_ladspa = g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value);
	//	g_message("connecting port: %d/%d %f\0", i, port_count, current->port_value.ags_port_float);

	port = g_list_prepend(port,
			      current);
      }else if((AGS_PORT_DESCRIPTOR_AUDIO & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(play_dssi_audio->input_port == NULL){
	    play_dssi_audio->input_port = (unsigned long *) malloc(sizeof(unsigned long));
	    play_dssi_audio->input_port[0] = i;
	  }else{
	    play_dssi_audio->input_port = (unsigned long *) realloc(play_dssi_audio->input_port,
								    (play_dssi_audio->input_lines + 1) * sizeof(unsigned long));
	    play_dssi_audio->input_port[play_dssi_audio->input_lines] = i;
	  }
	  
	  play_dssi_audio->input_lines += 1;
	}else if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(play_dssi_audio->output_port == NULL){
	    play_dssi_audio->output_port = (unsigned long *) malloc(sizeof(unsigned long));
	    play_dssi_audio->output_port[0] = i;
	  }else{
	    play_dssi_audio->output_port = (unsigned long *) realloc(play_dssi_audio->output_port,
								     (play_dssi_audio->output_lines + 1) * sizeof(unsigned long));
	    play_dssi_audio->output_port[play_dssi_audio->output_lines] = i;
	  }
	  
	  play_dssi_audio->output_lines += 1;
	}
      }

      port_descriptor = port_descriptor->next;
    }
    
    AGS_RECALL(play_dssi_audio)->port = g_list_reverse(port);
  }

  //  g_message("output lines: %d\0", play_dssi_audio->output_lines);

  return(g_list_copy(AGS_RECALL(play_dssi_audio)->port));
}

/**
 * ags_play_dssi_audio_load_conversion:
 * @play_dssi_audio: the #AgsPlayDssiAudio
 * @port: an #AgsPort
 * @port_descriptor: the #AgsPortDescriptor-struct
 * 
 * Loads conversion object by using @port_descriptor and sets in on @port.
 * 
 * Since: 0.7.134
 */
void
ags_play_dssi_audio_load_conversion(AgsPlayDssiAudio *play_dssi_audio,
				    GObject *port,
				    gpointer port_descriptor)
{
  AgsLadspaConversion *ladspa_conversion;

  if(play_dssi_audio == NULL ||
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
 * ags_play_dssi_audio_find:
 * @recall: a #GList containing #AgsRecall
 * @filename: plugin filename
 * @effect: effect's name
 *
 * Retrieve DSSI recall.
 *
 * Returns: Next match.
 * 
 * Since: 0.7.134
 */
GList*
ags_play_dssi_audio_find(GList *recall,
			 gchar *filename, gchar *effect)
{
  while(recall != NULL){
    if(AGS_IS_PLAY_DSSI_AUDIO(recall->data)){
      if(!g_strcmp0(AGS_PLAY_DSSI_AUDIO(recall->data)->filename,
		    filename) &&
	 !g_strcmp0(AGS_PLAY_DSSI_AUDIO(recall->data)->effect,
		    effect)){
	return(recall);
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_play_dssi_audio_new:
 *
 * Creates an #AgsPlayDssiAudio
 *
 * Returns: a new #AgsPlayDssiAudio
 *
 * Since: 0.7.134
 */
AgsPlayDssiAudio*
ags_play_dssi_audio_new()
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = (AgsPlayDssiAudio *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO,
						      NULL);

  return(play_dssi_audio);
}
