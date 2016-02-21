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
#include <ags/audio/ags_recall_lv2_run.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_lv2_manager.h>

#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_recall_lv2_run_class_init(AgsRecallLv2RunClass *recall_lv2_run_class);
void ags_recall_lv2_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_lv2_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_lv2_run_init(AgsRecallLv2Run *recall_lv2_run);
void ags_recall_lv2_run_connect(AgsConnectable *connectable);
void ags_recall_lv2_run_disconnect(AgsConnectable *connectable);
void ags_recall_lv2_run_finalize(GObject *gobject);

void ags_recall_lv2_run_run_init_pre(AgsRecall *recall);
void ags_recall_lv2_run_run_pre(AgsRecall *recall);
void ags_recall_lv2_run_run_inter(AgsRecall *recall);

void ags_recall_lv2_run_load_ports(AgsRecallLv2Run *recall_lv2_run);

/**
 * SECTION:ags_recall_lv2_run
 * @Short_description: The object interfacing with LV2
 * @Title: AgsRecallLv2Run
 *
 * #AgsRecallLv2Run provides LV2 support.
 */

static gpointer ags_recall_lv2_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_lv2_run_parent_connectable_interface;

GType
ags_recall_lv2_run_get_type (void)
{
  static GType ags_type_recall_lv2_run = 0;

  if(!ags_type_recall_lv2_run){
    static const GTypeInfo ags_recall_lv2_run_info = {
      sizeof (AgsRecallLv2RunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_lv2_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLv2Run),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_lv2_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_lv2_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_lv2_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_lv2_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
						     "AgsRecallLv2Run\0",
						     &ags_recall_lv2_run_info,
						     0);

    g_type_add_interface_static(ags_type_recall_lv2_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_lv2_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_lv2_run);
}

void
ags_recall_lv2_run_class_init(AgsRecallLv2RunClass *recall_lv2_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_lv2_run_parent_class = g_type_class_peek_parent(recall_lv2_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_lv2_run;

  gobject->finalize = ags_recall_lv2_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_lv2_run;

  recall->run_init_pre = ags_recall_lv2_run_run_init_pre;
  recall->run_pre = ags_recall_lv2_run_run_pre;
  recall->run_inter = ags_recall_lv2_run_run_inter;
}


void
ags_recall_lv2_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_lv2_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_lv2_run_connect;
  connectable->disconnect = ags_recall_lv2_run_disconnect;
}

void
ags_recall_lv2_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  //TODO:JK: implement me
}

void
ags_recall_lv2_run_init(AgsRecallLv2Run *recall_lv2_run)
{
  recall_lv2_run->input = NULL;
  recall_lv2_run->output = NULL;
}

void
ags_recall_lv2_run_connect(AgsConnectable *connectable)
{
  ags_recall_lv2_run_parent_connectable_interface->connect(connectable);
}

void
ags_recall_lv2_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_lv2_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_lv2_run_finalize(GObject *gobject)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallLv2Run *recall_lv2_run;
  uint32_t i;

  recall_lv2_run = AGS_RECALL_LV2_RUN(gobject);

  free(recall_lv2_run->output);
  free(recall_lv2_run->input);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_lv2_run_parent_class)->finalize(gobject);
}

void
ags_recall_lv2_run_run_init_pre(AgsRecall *recall)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallLv2Run *recall_lv2_run;
  AgsAudioSignal *audio_signal;
  AgsConfig *config;
  gchar *path;
  double samplerate;
  uint32_t buffer_size;
  uint32_t i;
  static const LV2_Feature **feature = {
    NULL,
  };
  
  /* call parent */
  AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_init_pre(recall);

  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);
  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

  path = g_strndup(recall_lv2->filename,
		   rindex(recall_lv2->filename, '/') - recall_lv2->filename + 1);
  
  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_lv2_run)->source;
  
  /* set up buffer */ 
  samplerate = audio_signal->samplerate;
  buffer_size = audio_signal->buffer_size;

  recall_lv2_run->input = (float *) malloc(recall_lv2->input_lines *
					   buffer_size *
					   sizeof(float));
  recall_lv2_run->output = (float *) malloc(recall_lv2->output_lines *
					    buffer_size *
					    sizeof(float));

  recall_lv2_run->lv2_handle = (LV2_Handle *) malloc(recall_lv2->input_lines *
						     sizeof(LV2_Handle));

  for(i = 0; i < recall_lv2->input_lines; i++){
    /* instantiate lv2 */
    recall_lv2_run->lv2_handle[i] = (LV2_Handle *) recall_lv2->plugin_descriptor->instantiate(recall_lv2->plugin_descriptor,
											      samplerate,
											      path,
											      feature);
    recall_lv2->plugin_descriptor->activate(recall_lv2_run->lv2_handle[i]);

#ifdef AGS_DEBUG
    g_message("instantiate LV2 handle\0");
#endif

  }

  ags_recall_lv2_run_load_ports(recall_lv2_run);

  /* can't be done in ags_recall_lv2_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  for(i = 0; i < recall_lv2->input_lines; i++){
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[i],
						recall_lv2->input_port[i],
						recall_lv2_run->input);
  }

  for(i = 0; i < recall_lv2->output_lines; i++){
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[i],
						recall_lv2->output_port[i],
						recall_lv2_run->output);
  }
}

void
ags_recall_lv2_run_run_pre(AgsRecall *recall)
{
  //empty
}

void
ags_recall_lv2_run_run_inter(AgsRecall *recall)
{
  AgsRecallLv2 *recall_lv2;
  AgsRecallLv2Run *recall_lv2_run;
  AgsAudioSignal *audio_signal;
  uint32_t buffer_size;
  uint32_t i;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_inter(recall);

  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);

  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_lv2_run)->source;

  memset(recall_lv2_run->output, 0, recall_lv2->output_lines * buffer_size * sizeof(float));
  memset(recall_lv2_run->input, 0, recall_lv2->input_lines * buffer_size * sizeof(float));

  if(audio_signal->stream_current == NULL){
    for(i = 0; i < recall_lv2->input_lines; i++){
      /* deactivate */
      //TODO:JK: fix-me
      //      recall_lv2->plugin_descriptor->deactivate(recall_lv2_run->lv2_handle[i]);
      //      recall_lv2->plugin_descriptor->cleanup(recall_lv2_run->lv2_handle[i]);
    }

    ags_recall_done(recall);
    return;
  }
  
  ags_recall_lv2_short_to_float(audio_signal->stream_current->data,
				recall_lv2_run->input,
				audio_signal->buffer_size, recall_lv2->input_lines);

  /* process data */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;

  buffer_size = audio_signal->buffer_size;

  for(i = 0; i < recall_lv2->input_lines; i++){
    recall_lv2->plugin_descriptor->run(recall_lv2_run->lv2_handle[i],
				       buffer_size);
  }

  /* copy data */
  memset((signed short *) audio_signal->stream_current->data,
	 0,
	 buffer_size * sizeof(signed short));
  ags_recall_lv2_float_to_short(recall_lv2_run->output,
				audio_signal->stream_current->data,
				audio_signal->buffer_size, recall_lv2->output_lines);
}

/**
 * ags_recall_lv2_run_load_ports:
 * @recall_lv2_run: an #AgsRecallLv2Run
 *
 * Set up LV2 ports.
 *
 * Since: 0.4.3
 */
void
ags_recall_lv2_run_load_ports(AgsRecallLv2Run *recall_lv2_run)
{
  AgsRecallLv2 *recall_lv2;
  AgsPort *current;

  AgsLv2Plugin *lv2_plugin;

  GList *port;
  GList *port_name_node, *port_name_node_start, *port_index_node;

  gchar *specifier;
  gchar *str;
  gchar *port_name;
  uint32_t port_count;
  uint32_t port_index;
  uint32_t i, j;

  LV2_Descriptor *plugin_descriptor;

  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_lv2_run)->parent->parent)->recall_channel);

  str = g_strdup_printf("//rdf-triple//rdf-verb[//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name'] and following-sibling::*//rdf-string[text()='%s']]/following-sibling::*//rdf-object-list//rdf-string[ancestor::*[self::rdf-object-list][1]/preceding-sibling::rdf-verb[1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name'] and ancestor::rdf-object-list/preceding-sibling::rdf-verb[1]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':port') + 1) = ':port']]",
			recall_lv2->effect);
  port_name_node_start = ags_turtle_find_xpath(lv2_plugin->turtle,
					       str);
  free(str);

  /* connect control port */
  port_count = g_list_length(port_name_node);

  /* connect audio ports */
  for(j = 0; j < recall_lv2->input_lines; j++){
    
    port_name_node = port_name_node_start;
    
    for(i = 0; i < port_count; i++){
      float *port_data;

      port_name = xmlNodeGetContent((xmlNode *) port_name_node->data);
      
      /* port index */
      str = g_strdup_printf("//rdf-triple//rdf-verb[//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':name') + 1) = ':name'] and following-sibling::*//rdf-string[text()='%s']]/following-sibling::*//rdf-object-list//rdf-string[text()='%s']/ancestor::*[self::rdf-object][2]//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':index') + 1) = ':index']/ancestor::*[self::rdf-verb][1]/following-sibling::*[self::rdf-object-list][1]//rdf-numeric\0",
			    recall_lv2->effect,
			    port_name);
      port_index_node = ags_turtle_find_xpath(lv2_plugin->turtle,
					      str);
      free(str);

      /* find port */
      current = NULL;
      port = AGS_RECALL(recall_lv2)->port;

      specifier = port_name;
      
      while(port != NULL){
	if(!g_strcmp0(specifier,
		      AGS_PORT(port->data)->specifier)){
	  current = port->data;
	  break;
	}

	port = port->next;
      }

      /* connect port */
      if(current != NULL){
	port_data = &(current->port_value.ags_port_float);
	port_index = g_ascii_strtoull(xmlNodeGetContent((xmlNode *) port_index_node->data),
				      NULL,
				      10);
	recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[j],
						    port_index,
						    port_data);
      }
      
      port_name_node = port_name_node->next;
    }

  }
}

/**
 * ags_recall_lv2_run_new:
 * @audio_signal: the source
 *
 * Creates a #AgsRecallLv2Run
 *
 * Returns: a new #AgsRecallLv2Run
 *
 * Since: 0.4.3
 */
AgsRecallLv2Run*
ags_recall_lv2_run_new(AgsAudioSignal *audio_signal)
{
  AgsRecallLv2Run *recall_lv2_run;

  recall_lv2_run = (AgsRecallLv2Run *) g_object_new(AGS_TYPE_RECALL_LV2_RUN,
						    "source\0", audio_signal,
						    NULL);

  return(recall_lv2_run);
}
