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
#include <ags/audio/ags_recall_ladspa_run.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_recall_ladspa_run_class_init(AgsRecallLadspaRunClass *recall_ladspa_run_class);
void ags_recall_ladspa_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_ladspa_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_ladspa_run_init(AgsRecallLadspaRun *recall_ladspa_run);
void ags_recall_ladspa_run_connect(AgsConnectable *connectable);
void ags_recall_ladspa_run_disconnect(AgsConnectable *connectable);
void ags_recall_ladspa_run_finalize(GObject *gobject);

void ags_recall_ladspa_run_run_init_pre(AgsRecall *recall);
void ags_recall_ladspa_run_run_pre(AgsRecall *recall);
void ags_recall_ladspa_run_run_inter(AgsRecall *recall);

void ags_recall_ladspa_run_load_ports(AgsRecallLadspaRun *recall_ladspa_run);

/**
 * SECTION:ags_recall_ladspa_run
 * @Short_description: The object interfacing with LADSPA
 * @Title: AgsRecallLadspaRun
 *
 * #AgsRecallLadspaRun provides LADSPA support.
 */

static gpointer ags_recall_ladspa_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_ladspa_run_parent_connectable_interface;

GType
ags_recall_ladspa_run_get_type (void)
{
  static GType ags_type_recall_ladspa_run = 0;

  if(!ags_type_recall_ladspa_run){
    static const GTypeInfo ags_recall_ladspa_run_info = {
      sizeof (AgsRecallLadspaRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_ladspa_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallLadspaRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_ladspa_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_ladspa_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_ladspa_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsRecallLadspaRun\0",
							&ags_recall_ladspa_run_info,
							0);

    g_type_add_interface_static(ags_type_recall_ladspa_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_ladspa_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_ladspa_run);
}

void
ags_recall_ladspa_run_class_init(AgsRecallLadspaRunClass *recall_ladspa_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_ladspa_run_parent_class = g_type_class_peek_parent(recall_ladspa_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_ladspa_run;

  gobject->finalize = ags_recall_ladspa_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_ladspa_run;

  recall->run_init_pre = ags_recall_ladspa_run_run_init_pre;
  recall->run_pre = ags_recall_ladspa_run_run_pre;
  recall->run_inter = ags_recall_ladspa_run_run_inter;
}


void
ags_recall_ladspa_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_ladspa_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_ladspa_run_connect;
  connectable->disconnect = ags_recall_ladspa_run_disconnect;
}

void
ags_recall_ladspa_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  //TODO:JK: implement me
}

void
ags_recall_ladspa_run_init(AgsRecallLadspaRun *recall_ladspa_run)
{
  recall_ladspa_run->input = NULL;
  recall_ladspa_run->output = NULL;
}

void
ags_recall_ladspa_run_connect(AgsConnectable *connectable)
{
  ags_recall_ladspa_run_parent_connectable_interface->connect(connectable);
}

void
ags_recall_ladspa_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_ladspa_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_ladspa_run_finalize(GObject *gobject)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallLadspaRun *recall_ladspa_run;
  unsigned long i;

  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(gobject);

  free(recall_ladspa_run->output);
  free(recall_ladspa_run->input);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_run_parent_class)->finalize(gobject);
}

void
ags_recall_ladspa_run_run_init_pre(AgsRecall *recall)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallLadspaRun *recall_ladspa_run;
  AgsAudioSignal *audio_signal;
  AgsConfig *config;
  unsigned long samplerate;
  unsigned long buffer_size;
  unsigned long i;

  LADSPA_PortDescriptor *port_descriptor;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_init_pre(recall);

  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);
  recall_ladspa = AGS_RECALL_LADSPA(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  
  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_ladspa_run)->source;
  
  /* set up buffer */ 
  samplerate = audio_signal->samplerate;
  buffer_size = audio_signal->buffer_size;

  recall_ladspa_run->input = (LADSPA_Data *) malloc(recall_ladspa->input_lines *
						    buffer_size *
						    sizeof(LADSPA_Data));
  recall_ladspa_run->output = (LADSPA_Data *) malloc(recall_ladspa->output_lines *
						     buffer_size *
						     sizeof(LADSPA_Data));

  recall_ladspa_run->ladspa_handle = (LADSPA_Handle *) malloc(recall_ladspa->input_lines *
							      sizeof(LADSPA_Handle));

  for(i = 0; i < recall_ladspa->input_lines; i++){
    /* instantiate ladspa */
    recall_ladspa_run->ladspa_handle[i] = (LADSPA_Handle *) recall_ladspa->plugin_descriptor->instantiate(recall_ladspa->plugin_descriptor,
													  samplerate);
    recall_ladspa->plugin_descriptor->activate(recall_ladspa_run->ladspa_handle[i]);

#ifdef AGS_DEBUG
      g_message("instantiate LADSPA handle\0");
#endif

  }

  ags_recall_ladspa_run_load_ports(recall_ladspa_run);

  /* can't be done in ags_recall_ladspa_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  port_descriptor = recall_ladspa->plugin_descriptor->PortDescriptors;

  for(i = 0; i < recall_ladspa->input_lines; i++){
    recall_ladspa->plugin_descriptor->connect_port(recall_ladspa_run->ladspa_handle[i],
						   recall_ladspa->input_port[i],
						   recall_ladspa_run->input);
  }

  for(i = 0; i < recall_ladspa->output_lines; i++){
    recall_ladspa->plugin_descriptor->connect_port(recall_ladspa_run->ladspa_handle[i],
						   recall_ladspa->output_port[i],
						   recall_ladspa_run->output);
  }
}

void
ags_recall_ladspa_run_run_pre(AgsRecall *recall)
{
  //empty
}

void
ags_recall_ladspa_run_run_inter(AgsRecall *recall)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallLadspaRun *recall_ladspa_run;
  AgsAudioSignal *audio_signal;
  unsigned long buffer_size;
  unsigned long i;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_inter(recall);

  recall_ladspa = AGS_RECALL_LADSPA(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);

  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_ladspa_run)->source;

  memset(recall_ladspa_run->output, 0, recall_ladspa->output_lines * buffer_size * sizeof(LADSPA_Data));
  memset(recall_ladspa_run->input, 0, recall_ladspa->input_lines * buffer_size * sizeof(LADSPA_Data));

  if(audio_signal->stream_current == NULL){
    for(i = 0; i < recall_ladspa->input_lines; i++){
      /* deactivate */
      //TODO:JK: fix-me
      //      recall_ladspa->plugin_descriptor->deactivate(recall_ladspa_run->ladspa_handle[i]);
      //      recall_ladspa->plugin_descriptor->cleanup(recall_ladspa_run->ladspa_handle[i]);
    }

    ags_recall_done(recall);
    return;
  }
  
  ags_recall_ladspa_short_to_float(audio_signal->stream_current->data,
				   recall_ladspa_run->input,
				   audio_signal->buffer_size, recall_ladspa->input_lines);

  /* process data */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;

  buffer_size = audio_signal->buffer_size;

  for(i = 0; i < recall_ladspa->input_lines; i++){
    recall_ladspa->plugin_descriptor->run(recall_ladspa_run->ladspa_handle[i],
					  buffer_size);
  }

  /* copy data */
  memset((signed short *) audio_signal->stream_current->data,
	 0,
	 buffer_size * sizeof(signed short));
  ags_recall_ladspa_float_to_short(recall_ladspa_run->output,
				   audio_signal->stream_current->data,
				   audio_signal->buffer_size, recall_ladspa->output_lines);
}

/**
 * ags_recall_ladspa_run_load_ports:
 * @recall_ladspa_run: an #AgsRecallLadspaRun
 *
 * Set up LADSPA ports.
 *
 * Since: 0.4
 */
void
ags_recall_ladspa_run_load_ports(AgsRecallLadspaRun *recall_ladspa_run)
{
  AgsRecallLadspa *recall_ladspa;
  AgsLadspaPlugin *ladspa_plugin;
  AgsPort *current;
  GList *port;
  gchar *path;
  unsigned long port_count;
  unsigned long i, j;

  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHintDescriptor hint_descriptor;

  recall_ladspa = AGS_RECALL_LADSPA(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_ladspa_run)->parent->parent)->recall_channel);

  port = AGS_RECALL(recall_ladspa)->port;
  
  plugin_descriptor = recall_ladspa->plugin_descriptor;

  port_count = plugin_descriptor->PortCount;
  port_descriptor = plugin_descriptor->PortDescriptors;

  for(i = 0; i < port_count; i++){
    if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
      if(LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	 LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	GList *list;
	LADSPA_Data *port_data;
	gchar *plugin_name;
	gchar *specifier;

	hint_descriptor = plugin_descriptor->PortRangeHints[i].HintDescriptor;
	specifier = plugin_descriptor->PortNames[i];

	list = port;

	while(list != NULL){
	  current = port->data;

	  if(!g_strcmp0(specifier,
			current->specifier)){
	    break;
	  }

	  list = list->next;
	}

	for(j = 0; j < recall_ladspa->input_lines; j++){
	  g_message("connecting port[%d]: %d/%d\0", j, i, port_count);
	  port_data = &(current->port_value.ags_port_float);
	  recall_ladspa->plugin_descriptor->connect_port(recall_ladspa_run->ladspa_handle[j],
							 i,
							 port_data);
	}
      }
    }
  }
}

/**
 * ags_recall_ladspa_run_new:
 * @audio_signal: the source
 *
 * Creates a #AgsRecallLadspaRun
 *
 * Returns: a new #AgsRecallLadspaRun
 */
AgsRecallLadspaRun*
ags_recall_ladspa_run_new(AgsAudioSignal *audio_signal)
{
  AgsRecallLadspaRun *recall_ladspa_run;

  recall_ladspa_run = (AgsRecallLadspaRun *) g_object_new(AGS_TYPE_RECALL_LADSPA_RUN,
							  "source\0", audio_signal,
							  NULL);

  return(recall_ladspa_run);
}
