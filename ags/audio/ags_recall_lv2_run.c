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
#include <ags/plugin/ags_lv2_event_manager.h>
#include <ags/plugin/ags_lv2_uri_map_manager.h>
#include <ags/plugin/ags_lv2_log_manager.h>
#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2_worker.h>

#include <ags/audio/ags_port.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>

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
  recall_lv2_run->feature = NULL;
  
  recall_lv2_run->input = NULL;
  recall_lv2_run->output = NULL;

  recall_lv2_run->event_port = NULL;
  recall_lv2_run->atom_port = NULL;
  
  recall_lv2_run->delta_time = 0;
  
  recall_lv2_run->event_buffer = NULL;
  recall_lv2_run->event_count = NULL;
  
  recall_lv2_run->note = NULL;
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

  if(recall_lv2_run->feature != NULL){
    guint i;

    for(i = 0; recall_lv2_run->feature[i] != NULL; i++){
      free(recall_lv2_run->feature[i]);
    }
    
    free(recall_lv2_run->feature);
  }
  
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
  uint32_t i, i_stop;

  LV2_URI_Map_Feature *uri_map_feature;
  
  LV2_Worker_Schedule_Handle worker_handle = ags_lv2_worker_manager_pull_worker(ags_lv2_worker_manager_get_instance());
  LV2_Worker_Schedule *worker_schedule;

  LV2_Log_Log *log_feature;

  LV2_Event_Feature *event_feature;

  LV2_Feature **feature;
  
  /* call parent */
  AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_init_pre(recall);

  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);
  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

  path = g_strndup(recall_lv2->filename,
		   rindex(recall_lv2->filename, '/') - recall_lv2->filename + 1);

  /**/
  recall_lv2_run->feature = 
    feature = (LV2_Feature **) malloc(5 * sizeof(LV2_Feature *));

  /* URI map feature */
  uri_map_feature = (LV2_URI_Map_Feature *) malloc(sizeof(LV2_URI_Map_Feature));
  uri_map_feature->callback_data = NULL;
  uri_map_feature->uri_to_id = ags_lv2_uri_map_manager_uri_to_id;
  
  feature[0] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[0]->URI = LV2_URI_MAP_URI;
  feature[0]->data = uri_map_feature;

  /* worker feature */
  worker_schedule = (LV2_Worker_Schedule *) malloc(sizeof(LV2_Worker_Schedule));
  worker_schedule->handle = worker_handle;
  worker_schedule->schedule_work = ags_lv2_worker_schedule_work;
  
  feature[1] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[1]->URI = LV2_WORKER__schedule;
  feature[1]->data = worker_schedule;

  /* log feature */
  log_feature = (LV2_Log_Log *) malloc(sizeof(LV2_Log_Log));
  
  log_feature->handle = NULL;
  log_feature->printf = ags_lv2_log_manager_printf;
  log_feature->vprintf = ags_lv2_log_manager_vprintf;

  feature[2] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[2]->URI = LV2_LOG__log;
  feature[2]->data = log_feature;

  /* event feature */
  event_feature = (LV2_Event_Feature *) malloc(sizeof(LV2_Event_Feature));
  
  event_feature->callback_data = NULL;
  event_feature->lv2_event_ref = ags_lv2_event_manager_lv2_event_ref;
  event_feature->lv2_event_unref = ags_lv2_event_manager_lv2_event_unref;
  
  feature[3] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[3]->URI = LV2_EVENT_URI;
  feature[3]->data = event_feature;

  feature[4] = NULL;

  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_lv2_run)->source;
  
  /* set up buffer */ 
  samplerate = audio_signal->samplerate;
  buffer_size = audio_signal->buffer_size;

  if(recall_lv2->input_lines < recall_lv2->output_lines){
    i_stop = recall_lv2->output_lines;
  }else{
    i_stop = recall_lv2->input_lines;
  }
  
  recall_lv2_run->input = (float *) malloc(recall_lv2->input_lines *
					   buffer_size *
					   sizeof(float));
  recall_lv2_run->output = (float *) malloc(recall_lv2->output_lines *
					    buffer_size *
					    sizeof(float));

  recall_lv2_run->lv2_handle = (LV2_Handle *) malloc(sizeof(LV2_Handle));

  
  /* instantiate lv2 */
  recall_lv2_run->lv2_handle[0] = (LV2_Handle) recall_lv2->plugin_descriptor->instantiate(recall_lv2->plugin_descriptor,
											  samplerate,
											  path,
											  feature);
  AGS_LV2_WORKER(worker_handle)->handle = recall_lv2_run->lv2_handle[0];
  
#ifdef AGS_DEBUG
  g_message("instantiate LV2 handle\0");
#endif

  ags_recall_lv2_run_load_ports(recall_lv2_run);

  /* can't be done in ags_recall_lv2_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  for(i = 0; i < recall_lv2->input_lines; i++){
    g_message("connect port: %d\0", recall_lv2->input_port[i]);
    
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->input_port[i],
						recall_lv2_run->input);
  }

  for(i = 0; i < recall_lv2->output_lines; i++){
    g_message("connect port: %d\0", recall_lv2->output_port[i]);

    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->output_port[i],
						recall_lv2_run->output);
  }

  /* connect event port */
  if((AGS_RECALL_LV2_HAS_EVENT_PORT & (recall_lv2->flags)) != 0){
    recall_lv2_run->event_port = ags_lv2_plugin_alloc_event_buffer(AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT);

    g_message("connect port: %d\0", recall_lv2->event_port);
    
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->event_port,
						recall_lv2_run->event_port);
  }
  
  /* connect atom port */
  if((AGS_RECALL_LV2_HAS_ATOM_PORT & (recall_lv2->flags)) != 0){
    recall_lv2_run->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT);

    g_message("connect port: %d\0", recall_lv2->atom_port);
    
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->atom_port,
						recall_lv2_run->atom_port);   
  }

  /* activate */
  if(recall_lv2->plugin_descriptor->activate != NULL){
    recall_lv2->plugin_descriptor->activate(recall_lv2_run->lv2_handle[0]);
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

  if(recall_lv2_run->output != NULL){
    memset(recall_lv2_run->output, 0, recall_lv2->output_lines * buffer_size * sizeof(float));
  }

  if(recall_lv2_run->input != NULL){
    memset(recall_lv2_run->input, 0, recall_lv2->input_lines * buffer_size * sizeof(float));
  }
  
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

  if(recall_lv2_run->input != NULL){
    ags_recall_lv2_short_to_float(audio_signal->stream_current->data,
				  recall_lv2_run->input,
				  audio_signal->buffer_size, recall_lv2->input_lines);
  }
  
  /* process data */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;

  buffer_size = audio_signal->buffer_size;

  recall_lv2->plugin_descriptor->run(recall_lv2_run->lv2_handle[0],
				     buffer_size);

  /* copy data */
  if(recall_lv2_run->output != NULL){
    memset((signed short *) audio_signal->stream_current->data,
	   0,
	   buffer_size * sizeof(signed short));
    
    ags_recall_lv2_float_to_short(recall_lv2_run->output,
				  audio_signal->stream_current->data,
				  audio_signal->buffer_size, recall_lv2->output_lines);
  }
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
  GList *port_descriptor;

  gchar *specifier;
  
  uint32_t port_count;
  uint32_t i, j, j_stop;

  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_lv2_run)->parent->parent)->recall_channel);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(recall_lv2->filename, recall_lv2->effect);

  if(recall_lv2->input_lines < recall_lv2->output_lines){
    j_stop = recall_lv2->output_lines;
  }else{
    j_stop = recall_lv2->input_lines;
  }

  if(AGS_BASE_PLUGIN(lv2_plugin)->port != NULL){
    port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;
    port_count = g_list_length(port_descriptor);

    for(i = 0; port_descriptor != NULL; i++){
      if((AGS_PORT_DESCRIPTOR_CONTROL & (AGS_PORT_DESCRIPTOR(port_descriptor->data)->flags)) != 0){
	port = AGS_RECALL(recall_lv2)->port;
	current = NULL;
	  
	specifier = AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_name;
	    
	while(port != NULL){
	  if(!g_strcmp0(specifier,
			AGS_PORT(port->data)->specifier)){
	    current = port->data;
	    break;
	  }

	  port = port->next;
	}

	g_message("connect port: %d\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index);
	recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						    (uint32_t) AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index,
						    (float *) &(current->port_value.ags_port_float));
      }

      port_descriptor = port_descriptor->next;
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

