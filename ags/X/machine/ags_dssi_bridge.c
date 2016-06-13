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

#include <ags/X/machine/ags_dssi_bridge.h>
#include <ags/X/machine/ags_dssi_bridge_callbacks.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_marshal.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_plugin.h>
#include <ags/object/ags_seekable.h>

#include <ags/file/ags_file.h>
#include <ags/file/ags_file_stock.h>
#include <ags/file/ags_file_id_ref.h>
#include <ags/file/ags_file_launch.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_thread-posix.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_factory.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_play_notation_audio.h>
#include <ags/audio/recall/ags_play_notation_audio_run.h>
#include <ags/audio/recall/ags_route_dssi_audio.h>
#include <ags/audio/recall/ags_route_dssi_audio_run.h>

#include <ags/widget/ags_dial.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bulk.h>
#include <ags/X/ags_bulk_member.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_dssi_bridge_class_init(AgsDssiBridgeClass *dssi_bridge);
void ags_dssi_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_dssi_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_dssi_bridge_init(AgsDssiBridge *dssi_bridge);
void ags_dssi_bridge_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_dssi_bridge_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_dssi_bridge_connect(AgsConnectable *connectable);
void ags_dssi_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_dssi_bridge_get_version(AgsPlugin *plugin);
void ags_dssi_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_dssi_bridge_get_build_id(AgsPlugin *plugin);
void ags_dssi_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);
gchar* ags_dssi_bridge_get_xml_type(AgsPlugin *plugin);
void ags_dssi_bridge_set_xml_type(AgsPlugin *plugin, gchar *xml_type);
void ags_dssi_bridge_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
void ags_dssi_bridge_launch_task(AgsFileLaunch *file_launch, AgsDssiBridge *dssi_bridge);
xmlNode* ags_dssi_bridge_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_dssi_bridge_set_audio_channels(AgsAudio *audio,
					guint audio_channels, guint audio_channels_old,
					gpointer data);
void ags_dssi_bridge_set_pads(AgsAudio *audio, GType type,
			      guint pads, guint pads_old,
			      gpointer data);

void ags_dssi_bridge_map_recall(AgsMachine *machine);

/**
 * SECTION:ags_dssi_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsDssiBridge
 * @section_id:
 * @include: ags/X/ags_dssi_bridge.h
 *
 * #AgsDssiBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_INDEX,
};

static gpointer ags_dssi_bridge_parent_class = NULL;
static AgsConnectableInterface* ags_dssi_bridge_parent_connectable_interface;
static AgsPluginInterface* ags_dssi_bridge_parent_plugin_interface;

GType
ags_dssi_bridge_get_type(void)
{
  static GType ags_type_dssi_bridge = 0;

  if(!ags_type_dssi_bridge){
    static const GTypeInfo ags_dssi_bridge_info = {
      sizeof(AgsDssiBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_dssi_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDssiBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dssi_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_dssi_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_dssi_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_dssi_bridge = g_type_register_static(AGS_TYPE_MACHINE,
						  "AgsDssiBridge\0", &ags_dssi_bridge_info,
						  0);

    g_type_add_interface_static(ags_type_dssi_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_dssi_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_dssi_bridge);
}

void
ags_dssi_bridge_class_init(AgsDssiBridgeClass *dssi_bridge)
{
  AgsMachineClass *machine;
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_dssi_bridge_parent_class = g_type_class_peek_parent(dssi_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(dssi_bridge);

  gobject->set_property = ags_dssi_bridge_set_property;
  gobject->get_property = ags_dssi_bridge_get_property;

  /* properties */
  /**
   * AgsRecallDssi:filename:
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
   * AgsRecallDssi:effect:
   *
   * The effect's name.
   * 
   * Since: 0.4.3
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
   * AgsRecallDssi:index:
   *
   * The effect's index.
   * 
   * Since: 0.4.3
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

  /* AgsMachine */
  machine = (AgsMachineClass *) dssi_bridge;

  machine->map_recall = ags_dssi_bridge_map_recall;
}

void
ags_dssi_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_dssi_bridge_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_dssi_bridge_connect;
  connectable->disconnect = ags_dssi_bridge_disconnect;
}

void
ags_dssi_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_dssi_bridge_get_version;
  plugin->set_version = ags_dssi_bridge_set_version;
  plugin->get_build_id = ags_dssi_bridge_get_build_id;
  plugin->set_build_id = ags_dssi_bridge_set_build_id;
  plugin->get_xml_type = ags_dssi_bridge_get_xml_type;
  plugin->set_xml_type = ags_dssi_bridge_set_xml_type;
  plugin->read = ags_dssi_bridge_read;
  plugin->write = ags_dssi_bridge_write;
  plugin->get_ports = NULL;
  plugin->set_ports = NULL;
}

void
ags_dssi_bridge_init(AgsDssiBridge *dssi_bridge)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkLabel *label;
  GtkTable *table;

  AgsAudio *audio;

  audio = AGS_MACHINE(dssi_bridge)->audio;
  audio->flags |= (AGS_AUDIO_OUTPUT_HAS_RECYCLING |
		   AGS_AUDIO_INPUT_HAS_RECYCLING |
		   AGS_AUDIO_SYNC |
		   AGS_AUDIO_ASYNC |
		   AGS_AUDIO_HAS_NOTATION | 
		   AGS_AUDIO_NOTATION_DEFAULT);

  AGS_MACHINE(dssi_bridge)->flags |= (AGS_MACHINE_IS_SYNTHESIZER |
				      AGS_MACHINE_REVERSE_NOTATION);

  g_signal_connect_after(G_OBJECT(audio), "set_audio_channels\0",
			 G_CALLBACK(ags_dssi_bridge_set_audio_channels), NULL);

  g_signal_connect_after(G_OBJECT(audio), "set_pads\0",
			 G_CALLBACK(ags_dssi_bridge_set_pads), NULL);
    
  dssi_bridge->flags = 0;

  dssi_bridge->name = NULL;

  dssi_bridge->version = AGS_DSSI_BRIDGE_DEFAULT_VERSION;
  dssi_bridge->build_id = AGS_DSSI_BRIDGE_DEFAULT_BUILD_ID;

  dssi_bridge->xml_type = "ags-dssi-bridge\0";
  
  dssi_bridge->mapped_output_pad = 0;
  dssi_bridge->mapped_input_pad = 0;
  
  dssi_bridge->filename = NULL;
  dssi_bridge->effect = NULL;
  dssi_bridge->effect_index = 0;

  dssi_bridge->port_values = NULL;
  dssi_bridge->dssi_descriptor = NULL;
  
  vbox = (GtkVBox *) gtk_vbox_new(FALSE, 0);
  gtk_container_add(gtk_bin_get_child(dssi_bridge),
		    (GtkWidget *) vbox);

  hbox = (GtkVBox *) gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) hbox,
		     FALSE, FALSE,
		     0);

  label = gtk_label_new("program\0");
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  dssi_bridge->program = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_box_pack_start((GtkBox *) hbox,
		     (GtkWidget *) dssi_bridge->program,
		     FALSE, FALSE,
		     0);

  AGS_MACHINE(dssi_bridge)->bridge = ags_effect_bridge_new(audio);
  gtk_box_pack_start((GtkBox *) vbox,
		     (GtkWidget *) AGS_MACHINE(dssi_bridge)->bridge,
		     FALSE, FALSE,
		     0);

  
  table = (GtkTable *) gtk_table_new(1, 2, FALSE);
  gtk_box_pack_start(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge),
		     table,
		     FALSE, FALSE,
		     0);

  AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input = (GtkWidget *) ags_effect_bulk_new(audio,
												      AGS_TYPE_INPUT);
  AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input)->flags |= (AGS_EFFECT_BULK_HIDE_BUTTONS |
											      AGS_EFFECT_BULK_HIDE_ENTRIES |
											      AGS_EFFECT_BULK_SHOW_LABELS);
  gtk_table_attach(table,
		   AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input,
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_dssi_bridge_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = g_value_get_string(value);

      if(filename == dssi_bridge->filename){
	return;
      }

      if(dssi_bridge->filename != NULL){
	g_free(dssi_bridge->filename);
      }

      dssi_bridge->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;
      
      effect = g_value_get_string(value);

      if(effect == dssi_bridge->effect){
	return;
      }

      if(dssi_bridge->effect != NULL){
	g_free(dssi_bridge->effect);
      }

      dssi_bridge->effect = g_strdup(effect);
    }
    break;
  case PROP_INDEX:
    {
      unsigned long *effect_index;
      
      effect_index = (unsigned long) g_value_get_uint(value);

      if(effect_index == dssi_bridge->effect_index){
	return;
      }

      dssi_bridge->effect_index = effect_index;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dssi_bridge_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      g_value_set_string(value, dssi_bridge->filename);
    }
    break;
  case PROP_EFFECT:
    {
      g_value_set_string(value, dssi_bridge->effect);
    }
    break;
  case PROP_INDEX:
    {
      g_value_set_uint(value, (guint) dssi_bridge->effect_index);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dssi_bridge_connect(AgsConnectable *connectable)
{
  AgsDssiBridge *dssi_bridge;

  if((AGS_MACHINE_CONNECTED & (AGS_MACHINE(connectable)->flags)) != 0){
    return;
  }

  ags_dssi_bridge_parent_connectable_interface->connect(connectable);

  dssi_bridge = AGS_DSSI_BRIDGE(connectable);

  g_signal_connect_after(G_OBJECT(dssi_bridge->program), "changed\0",
			 G_CALLBACK(ags_dssi_bridge_program_changed_callback), dssi_bridge);
}

void
ags_dssi_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_dssi_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_DSSI_BRIDGE(plugin)->version);
}

void
ags_dssi_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(plugin);

  dssi_bridge->version = version;
}

gchar*
ags_dssi_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_DSSI_BRIDGE(plugin)->build_id);
}

void
ags_dssi_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = AGS_DSSI_BRIDGE(plugin);

  dssi_bridge->build_id = build_id;
}

gchar*
ags_dssi_bridge_get_xml_type(AgsPlugin *plugin)
{
  return(AGS_DSSI_BRIDGE(plugin)->xml_type);
}

void
ags_dssi_bridge_set_xml_type(AgsPlugin *plugin, gchar *xml_type)
{
  AGS_DSSI_BRIDGE(plugin)->xml_type = xml_type;
}

void
ags_dssi_bridge_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  AgsDssiBridge *gobject;
  AgsFileLaunch *file_launch;

  gobject = AGS_DSSI_BRIDGE(plugin);

  g_object_set(gobject,
	       "filename\0", xmlGetProp(node,
					"filename\0"),
	       "effect\0", xmlGetProp(node,
				      "effect\0"),
	       NULL);

  /* launch */
  file_launch = (AgsFileLaunch *) g_object_new(AGS_TYPE_FILE_LAUNCH,
					       "node\0", node,
					       NULL);
  g_signal_connect(G_OBJECT(file_launch), "start\0",
		   G_CALLBACK(ags_dssi_bridge_launch_task), gobject);
  ags_file_add_launch(file,
		      G_OBJECT(file_launch));
}

void
ags_dssi_bridge_launch_task(AgsFileLaunch *file_launch, AgsDssiBridge *dssi_bridge)
{
  GtkTreeModel *model;

  GtkTreeIter iter;

  GList *list, *list_start;
  GList *recall;

  gchar *str;
  
  ags_dssi_bridge_load(dssi_bridge);

  /* block update bulk port */
  list_start = 
    list = gtk_container_get_children(AGS_EFFECT_BULK(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input)->table);

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      AGS_BULK_MEMBER(list->data)->flags |= AGS_BULK_MEMBER_NO_UPDATE;
    }

    list = list->next;
  }
  
  /* update program */
  str = xmlGetProp(file_launch->node,
		   "program\0");

  model = gtk_combo_box_get_model((GtkComboBox *) dssi_bridge->program);

  if(gtk_tree_model_get_iter_first(model, &iter)){
    gchar *value;
    
    do{
      gtk_tree_model_get(model, &iter,
			 0, &value,
			 -1);

      if(!g_strcmp0(str,
		    value)){
	break;
      }
    }while(gtk_tree_model_iter_next(model,
				    &iter));

    gtk_combo_box_set_active_iter((GtkComboBox *) dssi_bridge->program,
				  &iter);
  }

  /* update value and unblock update bulk port */
  recall = NULL;
  
  if(AGS_MACHINE(dssi_bridge)->audio->input != NULL){
    recall = AGS_MACHINE(dssi_bridge)->audio->input->recall;
    
    while((recall = ags_recall_template_find_type(recall, AGS_TYPE_RECALL_DSSI)) != NULL){
      if(!g_strcmp0(AGS_RECALL_DSSI(recall->data)->filename,
		  dssi_bridge->filename) &&
	 !g_strcmp0(AGS_RECALL_DSSI(recall->data)->effect,
		    dssi_bridge->effect)){
	break;
      }

      recall = recall->next;
    }
  }

  while(list != NULL){
    if(AGS_IS_BULK_MEMBER(list->data)){
      GtkWidget *child_widget;
      
      GList *port;

      child_widget = gtk_bin_get_child(list->data);
      
      if(recall != NULL){
	port = AGS_RECALL(recall->data)->port;

	while(port != port->next){
	  if(!g_strcmp0(AGS_BULK_MEMBER(list->data)->specifier,
			AGS_PORT(port->data)->specifier)){
	    if(AGS_IS_DIAL(child_widget)){
	      gtk_adjustment_set_value(AGS_DIAL(child_widget)->adjustment,
				       AGS_PORT(port->data)->port_value.ags_port_ladspa);
	      ags_dial_draw(child_widget);
	    }else if(GTK_IS_TOGGLE_BUTTON(child_widget)){
	      gtk_toggle_button_set_active(child_widget,
					   ((AGS_PORT(port->data)->port_value.ags_port_ladspa != 0.0) ? TRUE: FALSE));
	    }

	    break;
	  }

	  port = port->next;
	}
      }
     
      AGS_BULK_MEMBER(list->data)->flags &= (~AGS_BULK_MEMBER_NO_UPDATE);
    }
    
    list = list->next;
  }

  g_list_free(list_start);
}

xmlNode*
ags_dssi_bridge_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  AgsDssiBridge *dssi_bridge;

  GtkTreeIter iter;
  xmlNode *node;
  
  gchar *id;
  gchar *program;
  
  dssi_bridge = AGS_DSSI_BRIDGE(plugin);

  id = ags_id_generator_create_uuid();
    
  node = xmlNewNode(NULL,
		    "ags-dssi-bridge\0");
  xmlNewProp(node,
	     AGS_FILE_ID_PROP,
	     id);

  xmlNewProp(node,
	     "filename\0",
	     g_strdup(dssi_bridge->filename));

  xmlNewProp(node,
	     "effect\0",
	     g_strdup(dssi_bridge->effect));

  if((program = gtk_combo_box_text_get_active_text(dssi_bridge->program)) != NULL){
    xmlNewProp(node,
	       "program\0",
	       g_strdup(program));
  }

  ags_file_add_id_ref(file,
		      g_object_new(AGS_TYPE_FILE_ID_REF,
				   "application-context\0", file->application_context,
				   "file\0", file,
				   "node\0", node,
				   "xpath\0", g_strdup_printf("xpath=//*[@id='%s']\0", id),
				   "reference\0", dssi_bridge,
				   NULL));

  xmlAddChild(parent,
	      node);

  return(node);
}

void
ags_dssi_bridge_set_audio_channels(AgsAudio *audio,
				   guint audio_channels, guint audio_channels_old,
				   gpointer data)
{
  AgsMachine *machine;
  AgsDssiBridge *dssi_bridge;

  AgsChannel *channel, *next_pad;
  AgsAudioSignal *audio_signal;  

  /* get machine */
  dssi_bridge = (AgsDssiBridge *) audio->machine;
  machine = AGS_MACHINE(dssi_bridge);

  if(audio->input_pads == 0 &&
     audio->output_pads == 0){
    return;
  }
  
  machine = AGS_MACHINE(dssi_bridge);

  if(audio_channels > audio_channels_old){
    /* AgsInput */
    channel = audio->input;
    
    while(channel != NULL){
      next_pad = channel->next_pad;
      channel = ags_channel_nth(channel,
				audio_channels_old);
      
      while(channel != next_pad){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	audio_signal->loop_start = 0;
	audio_signal->loop_end = audio_signal->buffer_size;
	ags_audio_signal_stream_resize(audio_signal,
				       1);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);

	channel = channel->next;
      }
    }

    /* AgsOutput */
    channel = audio->output;

    while(channel != NULL){
      next_pad = channel->next_pad;
      channel = ags_channel_pad_nth(channel,
				    audio_channels_old);

      while(channel != next_pad){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }
    }

    /* recall */
    if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
      ags_dssi_bridge_input_map_recall(dssi_bridge,
				       audio_channels_old,
				       0);

      ags_dssi_bridge_output_map_recall(dssi_bridge,
					audio_channels_old,
					0);
    }
  }
}

void
ags_dssi_bridge_set_pads(AgsAudio *audio, GType type,
			 guint pads, guint pads_old,
			 gpointer data)
{
  AgsMachine *machine;
  AgsDssiBridge *dssi_bridge;

  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  
  gboolean grow;

  if(pads == pads_old ||
     audio->audio_channels == 0){
    return;
  }

  /* get machine */
  dssi_bridge = (AgsDssiBridge *) audio->machine;
  machine = AGS_MACHINE(dssi_bridge);

  if(pads_old < pads){
    grow = TRUE;
  }else{
    grow = FALSE;
  }
  
  if(g_type_is_a(type, AGS_TYPE_INPUT)){
    if(grow){
      /* AgsInput */
      channel = ags_channel_pad_nth(audio->input,
				    pads_old);

      while(channel != NULL){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	audio_signal->loop_start = 0;
	audio_signal->loop_end = audio_signal->buffer_size;
	ags_audio_signal_stream_resize(audio_signal,
				       1);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }

      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_dssi_bridge_input_map_recall(dssi_bridge,
					 0,
					 pads_old);
      }
    }else{
      dssi_bridge->mapped_input_pad = pads;
    }
  }else{
    if(grow){
      /* AgsOutput */
      channel = ags_channel_pad_nth(audio->output,
				    pads_old);

      while(channel != NULL){
	audio_signal = ags_audio_signal_new(audio->soundcard,
					    channel->first_recycling,
					    NULL);
	audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	ags_audio_signal_stream_resize(audio_signal,
				       3);
	ags_recycling_add_audio_signal(channel->first_recycling,
				       audio_signal);
	
	channel = channel->next;
      }

      /* recall */
      if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0){
	ags_dssi_bridge_output_map_recall(dssi_bridge,
					  0,
					  pads_old);
      }
    }else{
      dssi_bridge->mapped_output_pad = pads;
    }
  }
}

void
ags_dssi_bridge_map_recall(AgsMachine *machine)
{  
  AgsWindow *window;
  AgsDssiBridge *dssi_bridge;
  
  AgsAudio *audio;

  AgsDelayAudio *play_delay_audio;
  AgsDelayAudioRun *play_delay_audio_run;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsPlayNotationAudio *recall_notation_audio;
  AgsPlayNotationAudioRun *recall_notation_audio_run;
  AgsRouteDssiAudio *recall_route_dssi_audio;
  AgsRouteDssiAudioRun *recall_route_dssi_audio_run;

  GList *list;
  
  if((AGS_MACHINE_MAPPED_RECALL & (machine->flags)) != 0 ||
     (AGS_MACHINE_PREMAPPED_RECALL & (machine->flags)) != 0){
    return;
  }

  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  dssi_bridge = machine;
  audio = machine->audio;

  /* ags-delay */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-delay\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->play, AGS_TYPE_DELAY_AUDIO_RUN);

  if(list != NULL){
    play_delay_audio_run = AGS_DELAY_AUDIO_RUN(list->data);
    //    AGS_RECALL(play_delay_audio_run)->flags |= AGS_RECALL_PERSISTENT;
  }
  
  /* ags-count-beats */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-count-beats\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_OUTPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_PLAY |
			     AGS_RECALL_FACTORY_RECALL),
			    0);
  
  list = ags_recall_find_type(audio->play, AGS_TYPE_COUNT_BEATS_AUDIO_RUN);

  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);

    /* set dependency */  
    g_object_set(G_OBJECT(play_count_beats_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);
    ags_seekable_seek(AGS_SEEKABLE(play_count_beats_audio_run),
		      window->navigation->position_tact->adjustment->value * ags_soundcard_get_delay(AGS_SOUNDCARD(audio->soundcard)),
		      TRUE);
  }

  /* ags-route-dssi */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-route-dssi\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall, AGS_TYPE_ROUTE_DSSI_AUDIO_RUN);

  if(list != NULL){
    recall_notation_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);
  }

  /* ags-play-notation */
  ags_recall_factory_create(audio,
			    NULL, NULL,
			    "ags-play-notation\0",
			    0, 0,
			    0, 0,
			    (AGS_RECALL_FACTORY_INPUT |
			     AGS_RECALL_FACTORY_ADD |
			     AGS_RECALL_FACTORY_RECALL),
			    0);

  list = ags_recall_find_type(audio->recall, AGS_TYPE_PLAY_NOTATION_AUDIO_RUN);

  if(list != NULL){
    recall_notation_audio_run = AGS_PLAY_NOTATION_AUDIO_RUN(list->data);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "delay-audio-run\0", play_delay_audio_run,
		 NULL);

    /* set dependency */
    g_object_set(G_OBJECT(recall_notation_audio_run),
		 "count-beats-audio-run\0", play_count_beats_audio_run,
		 NULL);
  }

  /* depending on destination */
  ags_dssi_bridge_input_map_recall(dssi_bridge, 0, 0);

  /* add to effect bridge */
  ags_effect_bulk_add_effect(AGS_EFFECT_BRIDGE(AGS_MACHINE(dssi_bridge)->bridge)->bulk_input,
			     NULL,
			     dssi_bridge->filename,
			     dssi_bridge->effect);

  /* depending on destination */
  ags_dssi_bridge_output_map_recall(dssi_bridge, 0, 0);

  /* call parent */
  AGS_MACHINE_CLASS(ags_dssi_bridge_parent_class)->map_recall(machine);
}

void
ags_dssi_bridge_input_map_recall(AgsDssiBridge *dssi_bridge, guint audio_channel_start, guint input_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source, *current;
  
  GList *list;

  audio = AGS_MACHINE(dssi_bridge)->audio;

  if(dssi_bridge->mapped_input_pad > input_pad_start){
    return;
  }

  source = ags_channel_nth(audio->input,
			   audio_channel_start + input_pad_start * audio->audio_channels);

  /* map dependending on output */
  current = source;

  while(current != NULL){
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer\0",
			      audio_channel_start, audio->audio_channels, 
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_ADD),
			      0);
    
    current = current->next_pad;
  }
  
  /*  */
  current = source;

  while(current != NULL){
    /* ags-play */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-play\0",
			      audio_channel_start, audio->audio_channels, 
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_ADD),
			      0);

    current = current->next_pad;
  }

  /*  */
  current = source;

  while(current != NULL){
    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream\0",
			      audio_channel_start, audio->audio_channels, 
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);

    current = current->next_pad;
  }

  dssi_bridge->mapped_input_pad = audio->input_pads;
}

void
ags_dssi_bridge_output_map_recall(AgsDssiBridge *dssi_bridge, guint audio_channel_start, guint output_pad_start)
{
  AgsAudio *audio;
  AgsChannel *source, *input, *current;

  AgsDelayAudio *recall_delay_audio;
  AgsCountBeatsAudioRun *recall_count_beats_audio_run;

  GList *list;

  audio = AGS_MACHINE(dssi_bridge)->audio;

  if(dssi_bridge->mapped_output_pad > output_pad_start){
    return;
  }

  source = ags_channel_nth(audio->output,
			   audio_channel_start + output_pad_start * audio->audio_channels);

  /* remap for input */
  input = audio->input;

  while(input != NULL){
    /* ags-buffer */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-buffer\0",
			      audio_channel_start, audio->audio_channels, 
			      input->pad, input->pad + 1,
			      (AGS_RECALL_FACTORY_INPUT |
			       AGS_RECALL_FACTORY_RECALL |
			       AGS_RECALL_FACTORY_REMAP),
			      0);
    
    input = input->next_pad;
  }

  current = ags_channel_nth(audio->output,
			    audio_channel_start + output_pad_start * audio->audio_channels);

  while(current != NULL){
    /* ags-stream */
    ags_recall_factory_create(audio,
			      NULL, NULL,
			      "ags-stream\0",
			      audio_channel_start, audio->audio_channels,
			      current->pad, current->pad + 1,
			      (AGS_RECALL_FACTORY_OUTPUT |
			       AGS_RECALL_FACTORY_PLAY |
			       AGS_RECALL_FACTORY_RECALL | 
			       AGS_RECALL_FACTORY_ADD),
			      0);

    current = current->next_pad;
  }

  dssi_bridge->mapped_output_pad = audio->output_pads;
}

void
ags_dssi_bridge_load(AgsDssiBridge *dssi_bridge)
{
  GtkListStore *model;
  GtkTreeIter iter;

  AgsDssiPlugin *dssi_plugin;

  AgsConfig *config;

  gchar *str;

  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  DSSI_Program_Descriptor *program_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHintDescriptor hint_descriptor;

  unsigned long samplerate;
  unsigned long effect_index;
  unsigned long port_count;
  unsigned long i;

  config = ags_config_get_instance();
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }else{
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }

  g_message("ags_dssi_bridge.c - load %s %s\0",dssi_bridge->filename, dssi_bridge->effect);
 
  /* load plugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_bridge->filename,
						  dssi_bridge->effect);

  plugin_so = AGS_BASE_PLUGIN(dssi_plugin)->plugin_so;

  /*  */
  gtk_list_store_clear(gtk_combo_box_get_model(dssi_bridge->program));
  
  /*  */
  effect_index = AGS_BASE_PLUGIN(dssi_plugin)->effect_index;

  /* load ports */
  model = gtk_list_store_new(3,
			     G_TYPE_STRING,
			     G_TYPE_ULONG,
			     G_TYPE_ULONG);

  if(effect_index != -1 &&
     plugin_so){
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor\0");

    if(dlerror() == NULL && dssi_descriptor){
      dssi_bridge->dssi_descriptor = 
	plugin_descriptor = dssi_descriptor(effect_index);

      dssi_bridge->ladspa_handle = plugin_descriptor->LADSPA_Plugin->instantiate(plugin_descriptor->LADSPA_Plugin,
										 samplerate);
      port_count = plugin_descriptor->LADSPA_Plugin->PortCount;
      port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;

      dssi_bridge->port_values = (LADSPA_Data *) malloc(plugin_descriptor->LADSPA_Plugin->PortCount * sizeof(LADSPA_Data));
      
      for(i = 0; i < port_count; i++){
	if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	  if(LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	     LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	    AgsDssiPlugin *dssi_plugin;

	    GList *list;

	    gchar *specifier;
	    
	    dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_bridge->filename, dssi_bridge->effect);

	    list = AGS_BASE_PLUGIN(dssi_plugin)->port;
 	    specifier = plugin_descriptor->LADSPA_Plugin->PortNames[i];

	    while(list != NULL){
	      if(!g_strcmp0(specifier,
			    AGS_PORT_DESCRIPTOR(list->data)->port_name)){
		dssi_bridge->port_values[i] = g_value_get_float(AGS_PORT_DESCRIPTOR(list->data)->default_value);
		break;
	      }

	      list = list->next;
	    }
	    
	    plugin_descriptor->LADSPA_Plugin->connect_port(dssi_bridge->ladspa_handle,
							   i,
							   &(dssi_bridge->port_values[i]));
	  }
	}
      }
      
      if(plugin_descriptor->get_program != NULL){
	for(i = 0; (program_descriptor = plugin_descriptor->get_program(dssi_bridge->ladspa_handle, i)) != NULL; i++){
	  gtk_list_store_append(model, &iter);
	  gtk_list_store_set(model, &iter,
			     0, program_descriptor->Name,
			     1, program_descriptor->Bank,
			     2, program_descriptor->Program,
			     -1);
	}
      }
    }
  }
  
  gtk_combo_box_set_model(dssi_bridge->program,
			  model);
}

/**
 * ags_dssi_bridge_new:
 * @soundcard: the assigned soundcard.
 * @filename: the plugin.so
 * @effect: the effect
 *
 * Creates an #AgsDssiBridge
 *
 * Returns: a new #AgsDssiBridge
 *
 * Since: 0.4.3
 */
AgsDssiBridge*
ags_dssi_bridge_new(GObject *soundcard,
		    gchar *filename,
		    gchar *effect)
{
  AgsDssiBridge *dssi_bridge;

  dssi_bridge = (AgsDssiBridge *) g_object_new(AGS_TYPE_DSSI_BRIDGE,
					       NULL);

  if(soundcard != NULL){
    g_object_set(G_OBJECT(AGS_MACHINE(dssi_bridge)->audio),
		 "soundcard\0", soundcard,
		 NULL);
  }

  g_object_set(dssi_bridge,
	       "filename\0", filename,
	       "effect\0", effect,
	       NULL);

  return(dssi_bridge);
}
