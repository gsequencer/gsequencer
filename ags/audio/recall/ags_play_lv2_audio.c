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

#include <ags/audio/recall/ags_play_lv2_audio.h>

#include <ags/util/ags_id_generator.h>

#include <ags/plugin/ags_lv2_manager.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_soundcard.h>

#include <ags/plugin/ags_lv2_conversion.h>

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

void ags_play_lv2_audio_class_init(AgsPlayLv2AudioClass *play_lv2_audio);
void ags_play_lv2_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_lv2_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_play_lv2_audio_init(AgsPlayLv2Audio *play_lv2_audio);
void ags_play_lv2_audio_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_play_lv2_audio_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_play_lv2_audio_connect(AgsConnectable *connectable);
void ags_play_lv2_audio_disconnect(AgsConnectable *connectable);
void ags_play_lv2_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_play_lv2_audio_dispose(GObject *gobject);
void ags_play_lv2_audio_finalize(GObject *gobject);
void ags_play_lv2_audio_set_ports(AgsPlugin *plugin, GList *port);

void ags_play_lv2_audio_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_play_lv2_audio_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

/**
 * SECTION:ags_play_lv2_audio
 * @short_description: play audio lv2
 * @title: AgsPlayLv2Audio
 * @section_id:
 * @include: ags/audio/recall/ags_play_lv2_audio.h
 *
 * The #AgsPlayLv2Audio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_TURTLE,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_URI,
  PROP_INDEX,
};

static gpointer ags_play_lv2_audio_parent_class = NULL;
static AgsConnectableInterface* ags_play_lv2_audio_parent_connectable_interface;
static AgsPluginInterface *ags_play_lv2_parent_plugin_interface;

static const gchar *ags_play_lv2_audio_plugin_name = "ags-play-lv2\0";
static const gchar *ags_play_lv2_audio_specifier[] = {
};
static const gchar *ags_play_lv2_audio_control_port[] = {
};

GType
ags_play_lv2_audio_get_type()
{
  static GType ags_type_play_lv2_audio = 0;

  if(!ags_type_play_lv2_audio){
    static const GTypeInfo ags_play_lv2_audio_info = {
      sizeof (AgsPlayLv2AudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_lv2_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayLv2Audio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_lv2_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_lv2_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_play_lv2_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						     "AgsPlayLv2Audio\0",
						     &ags_play_lv2_audio_info,
						     0);

    g_type_add_interface_static(ags_type_play_lv2_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_play_lv2_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_play_lv2_audio);
}

void
ags_play_lv2_audio_class_init(AgsPlayLv2AudioClass *play_lv2_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_lv2_audio_parent_class = g_type_class_peek_parent(play_lv2_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_lv2_audio;

  gobject->set_property = ags_play_lv2_audio_set_property;
  gobject->get_property = ags_play_lv2_audio_get_property;

  gobject->dispose = ags_play_lv2_audio_dispose;
  gobject->finalize = ags_play_lv2_audio_finalize;

  /* properties */
  /**
   * AgsPlayLv2Audio:turtle:
   *
   * The assigned turtle.
   * 
   * Since: 0.7.134
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
   * AgsPlayLv2Audio:filename:
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
   * AgsPlayLv2Audio:effect:
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
   * AgsPlayLv2Audio:uri:
   *
   * The uri's name.
   * 
   * Since: 0.7.134
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
   * AgsPlayLv2Audio:index:
   *
   * The uri's index.
   * 
   * Since: 0.7.134
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
ags_play_lv2_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_lv2_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_lv2_audio_connect;
  connectable->disconnect = ags_play_lv2_audio_disconnect;
}

void
ags_play_lv2_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_lv2_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->read = ags_play_lv2_audio_read;
  plugin->write = ags_play_lv2_audio_write;
  plugin->set_ports = ags_play_lv2_audio_set_ports;
}

void
ags_play_lv2_audio_init(AgsPlayLv2Audio *play_lv2_audio)
{
  GList *port;

  AGS_RECALL(play_lv2_audio)->name = "ags-play-lv2\0";
  AGS_RECALL(play_lv2_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_lv2_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_lv2_audio)->xml_type = "ags-play-lv2-audio\0";

  play_lv2_audio->turtle = NULL;

  play_lv2_audio->filename = NULL;
  play_lv2_audio->effect = NULL;
  play_lv2_audio->uri = NULL;
  play_lv2_audio->index = 0;

  play_lv2_audio->plugin_descriptor = NULL;

  play_lv2_audio->input_port = NULL;
  play_lv2_audio->input_lines = 0;

  play_lv2_audio->output_port = NULL;
  play_lv2_audio->output_lines = 0;

  play_lv2_audio->event_port = 0;
  play_lv2_audio->atom_port = 0;
}

void
ags_play_lv2_audio_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsPlayLv2Audio *play_lv2_audio;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  switch(prop_id){
  case PROP_TURTLE:
    {
      AgsTurtle *turtle;

      turtle = (AgsTurtle *) g_value_get_object(value);

      if(play_lv2_audio->turtle == turtle){
	return;
      }

      if(play_lv2_audio->turtle != NULL){
	g_object_unref(play_lv2_audio->turtle);
      }

      if(turtle != NULL){
	g_object_ref(turtle);
      }

      play_lv2_audio->turtle = turtle;
    }
    break;
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == play_lv2_audio->filename){
	return;
      }

      if(play_lv2_audio->filename != NULL){
	g_free(play_lv2_audio->filename);
      }

      play_lv2_audio->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == play_lv2_audio->effect){
	return;
      }

      if(play_lv2_audio->effect != NULL){
	g_free(play_lv2_audio->effect);
      }

      play_lv2_audio->effect = g_strdup(effect);
    }
    break;
  case PROP_URI:
    {
      gchar *uri;
      
      uri = g_value_get_string(value);

      if(uri == play_lv2_audio->uri){
	return;
      }

      if(play_lv2_audio->uri != NULL){
	g_free(play_lv2_audio->uri);
      }

      play_lv2_audio->uri = g_strdup(uri);
    }
    break;
  case PROP_INDEX:
    {
      uint32_t index;
      
      index = g_value_get_ulong(value);

      if(index == play_lv2_audio->index){
	return;
      }

      play_lv2_audio->index = index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_lv2_audio_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsPlayLv2Audio *play_lv2_audio;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  switch(prop_id){
  case PROP_TURTLE:
    {
      g_value_set_object(value, play_lv2_audio->turtle);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_string(value, play_lv2_audio->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, play_lv2_audio->effect);
    }
    break;
  case PROP_URI:
    {
      g_value_set_string(value, play_lv2_audio->uri);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_ulong(value, play_lv2_audio->index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_lv2_audio_connect(AgsConnectable *connectable)
{
  AgsRecall *recall;
  
  recall = AGS_RECALL(connectable);
  
  if((AGS_RECALL_CONNECTED & (recall->flags)) != 0){
    return;
  }

  /* load automation */
  ags_recall_load_automation(recall,
			     g_list_copy(recall->port));

  /* call parent */
  ags_play_lv2_audio_parent_connectable_interface->connect(connectable);
}

void
ags_play_lv2_audio_disconnect(AgsConnectable *connectable)
{
  ags_play_lv2_audio_parent_connectable_interface->disconnect(connectable);
}

void
ags_play_lv2_audio_dispose(GObject *gobject)
{
  AgsPlayLv2Audio *play_lv2_audio;
  
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  /* turtle */
  if(play_lv2_audio->turtle != NULL){
    g_object_unref(play_lv2_audio->turtle);

    play_lv2_audio->turtle = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_parent_class)->dispose(gobject);
}

void
ags_play_lv2_audio_finalize(GObject *gobject)
{
  AgsPlayLv2Audio *play_lv2_audio;
  
  play_lv2_audio = AGS_PLAY_LV2_AUDIO(gobject);

  /* turtle */
  if(play_lv2_audio->turtle != NULL){
    g_object_unref(play_lv2_audio->turtle);
  }

  /* filename, effect and uri */
  g_free(play_lv2_audio->filename);
  g_free(play_lv2_audio->effect);
  g_free(play_lv2_audio->uri);
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_lv2_audio_parent_class)->finalize(gobject);
}

void
ags_play_lv2_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  AgsPlayLv2Audio *play_lv2_audio;
  AgsPort *current;
  
  AgsLv2Plugin *lv2_plugin;
  
  GList *list;  
  GList *port_descriptor;
  
  unsigned long port_count;
  unsigned long i;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(plugin);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       play_lv2_audio->filename, play_lv2_audio->effect);

  port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

  if(port_descriptor != NULL){
    port_count = g_list_length(port_descriptor);

    for(i = 0; i < port_count; i++){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	gchar *specifier;
	
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;

	if(specifier == NULL){
	  port_descriptor = port_descriptor->next;
	  
	  continue;
	}
	
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

	if(current != NULL){
	  current->port_descriptor = port_descriptor->data;
	  ags_play_lv2_audio_load_conversion(play_lv2_audio,
					     (GObject *) current,
					     port_descriptor->data);
	
	  current->port_value.ags_port_float = (float) ags_conversion_convert(current->conversion,
									      g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value),
									      FALSE);
	
#ifdef AGS_DEBUG
	  g_message("connecting port: %d/%d\0", i, port_count);      
#endif
	}
      }else if((AGS_PORT_DESCRIPTOR_AUDIO & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(play_lv2_audio->input_port == NULL){
	    play_lv2_audio->input_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->input_port[0] = i;
	  }else{
	    play_lv2_audio->input_port = (uint32_t *) realloc(play_lv2_audio->input_port,
							      (play_lv2_audio->input_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->input_port[play_lv2_audio->input_lines] = i;
	  }

	  play_lv2_audio->input_lines += 1;
	}else if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(play_lv2_audio->output_port == NULL){
	    play_lv2_audio->output_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->output_port[0] = i;
	  }else{
	    play_lv2_audio->output_port = (uint32_t *) realloc(play_lv2_audio->output_port,
							       (play_lv2_audio->output_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->output_port[play_lv2_audio->output_lines] = i;
	  }

	  play_lv2_audio->output_lines += 1;
	}
      }

      port_descriptor = port_descriptor->next;
    }

    AGS_RECALL(play_lv2_audio)->port = g_list_reverse(port);
  }
}

void
ags_play_lv2_audio_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsPlayLv2Audio *gobject;
  AgsLv2Plugin *lv2_plugin;
  gchar *filename, *effect, *uri;
  uint32_t index;

  gobject = AGS_PLAY_LV2_AUDIO(plugin);

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
  uri = xmlGetProp(node,
		   "uri\0");
  index = g_ascii_strtoull(xmlGetProp(node,
				      "index\0"),
			   NULL,
			   10);

  g_object_set(gobject,
	       "filename\0", filename,
	       "effect\0", effect,
	       "uri\0", uri,
	       "index\0", index,
	       NULL);

  ags_play_lv2_audio_load(gobject);
}

xmlNode*
ags_play_lv2_audio_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsPlayLv2Audio *play_lv2_audio;
  xmlNode *node;
  GList *list;
  gchar *id;

  play_lv2_audio = AGS_PLAY_LV2_AUDIO(plugin);

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
				   "reference\0", play_lv2_audio,
				   NULL));

  xmlNewProp(node,
	     "filename\0",
	     g_strdup(play_lv2_audio->filename));
  
  xmlNewProp(node,
	     "effect\0",
	     g_strdup(play_lv2_audio->effect));

  xmlNewProp(node,
	     "uri\0",
	     g_strdup(play_lv2_audio->uri));

  xmlNewProp(node,
	     "index\0",
	     g_strdup_printf("%d\0", play_lv2_audio->index));

  xmlAddChild(parent,
	      node);

  return(node);
}

/**
 * ags_play_lv2_audio_load:
 * @play_lv2_audio: an #AgsPlayLv2Audio
 *
 * Set up LV2 handle.
 * 
 * Since: 0.7.134
 */
void
ags_play_lv2_audio_load(AgsPlayLv2Audio *play_lv2_audio)
{
  AgsLv2Plugin *lv2_plugin;

  void *plugin_so;
  LV2_Descriptor_Function lv2_descriptor;
  LV2_Descriptor *plugin_descriptor;

  /*  */
  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       play_lv2_audio->filename, play_lv2_audio->effect);
  plugin_so = AGS_BASE_PLUGIN(lv2_plugin)->plugin_so;
  
  if(plugin_so != NULL){
    lv2_descriptor = (LV2_Descriptor_Function) dlsym(plugin_so,
						     "lv2_descriptor\0");

    if(dlerror() == NULL && lv2_descriptor){
      play_lv2_audio->plugin_descriptor = 
	plugin_descriptor = lv2_descriptor(play_lv2_audio->index);

      if((AGS_LV2_PLUGIN_NEEDS_WORKER & (lv2_plugin->flags)) != 0){
	play_lv2_audio->flags |= AGS_PLAY_LV2_AUDIO_HAS_WORKER;
      }
    }
  }
}

/**
 * ags_play_lv2_audio_load_ports:
 * @play_lv2_audio: an #AgsPlayLv2Audio
 *
 * Set up LV2 ports.
 *
 * Returns: a #GList containing #AgsPort.
 * 
 * Since: 0.7.134
 */
GList*
ags_play_lv2_audio_load_ports(AgsPlayLv2Audio *play_lv2_audio)
{
  AgsPort *current;

  AgsLv2Plugin *lv2_plugin;

  GList *port;
  GList *port_descriptor;

  uint32_t port_count;
  uint32_t i;

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       play_lv2_audio->filename, play_lv2_audio->effect);
#ifdef AGS_DEBUG
  g_message("ports from ttl: %s\0", lv2_plugin->turtle->filename);
#endif
  
  port = NULL;
  port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;
  
  if(port_descriptor != NULL){
    port_count = g_list_length(port_descriptor);
    
    for(i = 0; i < port_count; i++){
#ifdef AGS_DEBUG
      g_message("Lv2 plugin port-index: %d\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index);
#endif
      
      if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_EVENT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  play_lv2_audio->flags |= AGS_PLAY_LV2_AUDIO_HAS_EVENT_PORT;
	  play_lv2_audio->event_port = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
	}
      
	if((AGS_PORT_DESCRIPTOR_ATOM & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  play_lv2_audio->flags |= AGS_PLAY_LV2_AUDIO_HAS_ATOM_PORT;
	  play_lv2_audio->atom_port = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
	}
      }
      
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0 &&
	 ((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0 ||
	  (AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0)){
	gchar *plugin_name;
	gchar *specifier;

	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;

	if(specifier == NULL){
	  port_descriptor = port_descriptor->next;
	  
	  continue;
	}
	
	plugin_name = g_strdup_printf("lv2-<%s>\0", lv2_plugin->uri);

	current = g_object_new(AGS_TYPE_PORT,
			       "plugin-name\0", plugin_name,
			       "specifier\0", specifier,
			       "control-port\0", g_strdup_printf("%u/%u\0",
								 i,
								 port_count),
			       "port-value-is-pointer\0", FALSE,
			       "port-value-type\0", G_TYPE_FLOAT,
			       NULL);
	g_object_ref(current);
	
	if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  AGS_RECALL(play_lv2_audio)->flags |= AGS_RECALL_HAS_OUTPUT_PORT;
	  
	  current->flags |= AGS_PORT_IS_OUTPUT;
	}else{
	  if((AGS_PORT_DESCRIPTOR_INTEGER & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) == 0 &&
	     (AGS_PORT_DESCRIPTOR_TOGGLED & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) == 0 &&
	     AGS_PORT_DESCRIPTOR(port_descriptor->data)->scale_steps == -1){
	    current->flags |= AGS_PORT_INFINITE_RANGE;
	  }
	}
	
	current->port_descriptor = port_descriptor->data;
	ags_play_lv2_audio_load_conversion(play_lv2_audio,
					   (GObject *) current,
					   port_descriptor->data);

	current->port_value.ags_port_float = (float) ags_conversion_convert(current->conversion,
									    g_value_get_float(AGS_PORT_DESCRIPTOR(port_descriptor->data)->default_value),
									    FALSE);

#ifdef AGS_DEBUG
	g_message("connecting port: %s %d/%d\0", specifier, i, port_count);
#endif
	
	port = g_list_prepend(port,
			      current);
      }else if((AGS_PORT_DESCRIPTOR_AUDIO & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	if((AGS_PORT_DESCRIPTOR_INPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(play_lv2_audio->input_port == NULL){
	    play_lv2_audio->input_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->input_port[0] = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
	  }else{
	    play_lv2_audio->input_port = (uint32_t *) realloc(play_lv2_audio->input_port,
							      (play_lv2_audio->input_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->input_port[play_lv2_audio->input_lines] = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
	  }
	  
	  play_lv2_audio->input_lines += 1;
	}else if((AGS_PORT_DESCRIPTOR_OUTPUT & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	  if(play_lv2_audio->output_port == NULL){
	    play_lv2_audio->output_port = (uint32_t *) malloc(sizeof(uint32_t));
	    play_lv2_audio->output_port[0] = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
	  }else{
	    play_lv2_audio->output_port = (uint32_t *) realloc(play_lv2_audio->output_port,
							       (play_lv2_audio->output_lines + 1) * sizeof(uint32_t));
	    play_lv2_audio->output_port[play_lv2_audio->output_lines] = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index;
	  }
	  
	  play_lv2_audio->output_lines += 1;
	}
      }

      port_descriptor = port_descriptor->next;
    }
    
    AGS_RECALL(play_lv2_audio)->port = g_list_reverse(port);
  }
  
  return(g_list_copy(AGS_RECALL(play_lv2_audio)->port));
}

/**
 * ags_play_lv2_audio_load_conversion:
 * @play_lv2_audio: the #AgsPlayLv2Audio
 * @port: an #AgsPort
 * @port_descriptor: the #AgsPortDescriptor-struct
 * 
 * Loads conversion object by using @port_descriptor and sets in on @port.
 * 
 * Since: 0.7.134
 */
void
ags_play_lv2_audio_load_conversion(AgsPlayLv2Audio *play_lv2_audio,
				   GObject *port,
				   gpointer port_descriptor)
{
  AgsLv2Conversion *lv2_conversion;

  if(play_lv2_audio == NULL ||
     port == NULL ||
     port_descriptor == NULL){
    return;
  }

  lv2_conversion = NULL;
  
  if((AGS_PORT_DESCRIPTOR_LOGARITHMIC & (AGS_PORT_DESCRIPTOR(port_descriptor)->flags)) != 0){
    lv2_conversion = ags_lv2_conversion_new();
    g_object_set(port,
		 "conversion\0", lv2_conversion,
		 NULL);
    
    lv2_conversion->flags |= AGS_LV2_CONVERSION_LOGARITHMIC;
  }
}

/**
 * ags_play_lv2_audio_find:
 * @recall: a #GList containing #AgsRecall
 * @filename: plugin filename
 * @uri: uri's name
 *
 * Retrieve LV2 recall.
 *
 * Returns: Next match.
 * 
 * Since: 0.7.134
 */
GList*
ags_play_lv2_audio_find(GList *recall,
			gchar *filename, gchar *uri)
{
  while(recall != NULL){
    if(AGS_IS_PLAY_LV2_AUDIO(recall->data)){
      if(!g_strcmp0(AGS_PLAY_LV2_AUDIO(recall->data)->filename,
		    filename) &&
	 !g_strcmp0(AGS_PLAY_LV2_AUDIO(recall->data)->uri,
		    uri)){
	return(recall);
      }
    }

    recall = recall->next;
  }

  return(NULL);
}

/**
 * ags_play_lv2_audio_new:
 *
 * Creates an #AgsPlayLv2Audio
 *
 * Returns: a new #AgsPlayLv2Audio
 *
 * Since: 0.7.134
 */
AgsPlayLv2Audio*
ags_play_lv2_audio_new()
{
  AgsPlayLv2Audio *play_lv2_audio;

  play_lv2_audio = (AgsPlayLv2Audio *) g_object_new(AGS_TYPE_PLAY_LV2_AUDIO,
						    NULL);

  return(play_lv2_audio);
}
