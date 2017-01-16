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
#include <ags/plugin/ags_lv2_urid_manager.h>
#include <ags/plugin/ags_lv2_option_manager.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_route_lv2_audio_run.h>

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
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>

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
  recall_lv2_run->route_lv2_audio_run = NULL;

  recall_lv2_run->worker_handle = NULL;
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

  free(recall_lv2_run->lv2_handle);

  if(recall_lv2_run->feature != NULL){
    guint i;

    for(i = 0; recall_lv2_run->feature[i] != NULL; i++){
      free(recall_lv2_run->feature[i]->data);
      free(recall_lv2_run->feature[i]);
    }
    
    free(recall_lv2_run->feature);
  }

  if(recall_lv2_run->output != NULL){
    free(recall_lv2_run->output);
  }

  if(recall_lv2_run->input != NULL){
    free(recall_lv2_run->input);
  }

  if(recall_lv2_run->route_lv2_audio_run != NULL){
    AGS_ROUTE_LV2_AUDIO_RUN(recall_lv2_run->route_lv2_audio_run)->feed_midi = g_list_remove(AGS_ROUTE_LV2_AUDIO_RUN(recall_lv2_run->route_lv2_audio_run)->feed_midi,
											    recall_lv2_run->note);
  }
  
  if(recall_lv2_run->atom_port != NULL){
    free(recall_lv2_run->atom_port);
  }

  if(recall_lv2_run->event_port != NULL){
    free(recall_lv2_run->event_port);
  }

  if(recall_lv2_run->event_buffer != NULL){
    free(recall_lv2_run->event_buffer);
  }

  if(recall_lv2_run->event_count != NULL){
    free(recall_lv2_run->event_count);
  }

  if(recall_lv2_run->worker_handle != NULL){
    if(AGS_LV2_WORKER(recall_lv2_run->worker_handle)->returnable_thread != NULL){  
      g_atomic_int_and(&(AGS_RETURNABLE_THREAD(AGS_LV2_WORKER(recall_lv2_run->worker_handle)->returnable_thread)->flags),
		       (~AGS_RETURNABLE_THREAD_IN_USE));
      g_atomic_int_and(&(AGS_THREAD(AGS_LV2_WORKER(recall_lv2_run->worker_handle)->returnable_thread)->flags),
		       (~AGS_THREAD_RUNNING));

      g_object_unref(AGS_LV2_WORKER(recall_lv2_run->worker_handle)->returnable_thread);
    }
    
    g_object_unref(recall_lv2_run->worker_handle);
  }
  
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

  LV2_URI_Map_Feature *uri_map_feature;
  
  LV2_Worker_Schedule_Handle worker_handle;
  LV2_Worker_Schedule *worker_schedule;

  LV2_Log_Log *log_feature;

  LV2_Event_Feature *event_feature;

  LV2_URID_Map *urid_map;
  LV2_URID_Unmap *urid_unmap;

  LV2_Options_Interface *options_interface;
  LV2_Options_Option *options;
  
  LV2_Feature **feature;
  
  /* call parent */
  AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_init_pre(recall);

  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);
  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

  path = g_strndup(recall_lv2->filename,
		   rindex(recall_lv2->filename, '/') - recall_lv2->filename + 1);

  /* feature array */
  recall_lv2_run->feature = 
    feature = (LV2_Feature **) malloc(8 * sizeof(LV2_Feature *));

  /* URI map feature */
  uri_map_feature = (LV2_URI_Map_Feature *) malloc(sizeof(LV2_URI_Map_Feature));
  uri_map_feature->callback_data = NULL;
  uri_map_feature->uri_to_id = ags_lv2_uri_map_manager_uri_to_id;
  
  feature[0] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[0]->URI = LV2_URI_MAP_URI;
  feature[0]->data = uri_map_feature;

  /* worker feature */
  recall_lv2_run->worker_handle = 
    worker_handle = ags_lv2_worker_manager_pull_worker(ags_lv2_worker_manager_get_instance());
  
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
  
  /* URID map feature */
  urid_map = (LV2_URID_Map *) malloc(sizeof(LV2_URID_Map));
  urid_map->handle = NULL;
  urid_map->map = ags_lv2_urid_manager_map;
  
  feature[4] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[4]->URI = LV2_URID_MAP_URI;
  feature[4]->data = urid_map;

  /* URID unmap feature */
  urid_unmap = (LV2_URID_Unmap *) malloc(sizeof(LV2_URID_Unmap));
  urid_unmap->handle = NULL;
  urid_unmap->unmap = ags_lv2_urid_manager_unmap;
  
  feature[5] = (LV2_Feature *) malloc(sizeof(LV2_Feature));
  feature[5]->URI = LV2_URID_UNMAP_URI;
  feature[5]->data = urid_unmap;

  /* Options interface */
  options_interface = (LV2_Options_Interface *) malloc(sizeof(LV2_Options_Interface));
  options_interface->set = ags_lv2_option_manager_lv2_options_set;
  options_interface->get = ags_lv2_option_manager_lv2_options_get;
  
  feature[6]->URI = LV2_OPTIONS__interface;
  feature[6]->data = options_interface;

  /* terminate */
  feature[7] = NULL;

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

  recall_lv2_run->lv2_handle = (LV2_Handle *) malloc(sizeof(LV2_Handle));

  
  /* instantiate lv2 */
  recall_lv2_run->lv2_handle[0] = (LV2_Handle) recall_lv2->plugin_descriptor->instantiate(recall_lv2->plugin_descriptor,
											  samplerate,
											  path,
											  feature);
  AGS_LV2_WORKER(worker_handle)->handle = recall_lv2_run->lv2_handle[0];

  /* some options */
  options = (LV2_Options_Option *) malloc(6 * sizeof(LV2_Options_Option));

  /* samplerate */
  options[0].context = LV2_OPTIONS_INSTANCE;
  options[0].subject = 0;
  options[0].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_PARAMETERS__sampleRate);

  options[0].size = sizeof(float);
  options[0].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_ATOM__Float);
  options[0].value = &(audio_signal->samplerate);
  
  /* min-block-length */
  options[1].context = LV2_OPTIONS_INSTANCE;
  options[1].subject = 0;
  options[1].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_BUF_SIZE__minBlockLength);

  options[1].size = sizeof(float);
  options[1].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_ATOM__Int);
  options[1].value = &(audio_signal->buffer_size);

  /* max-block-length */
  options[2].context = LV2_OPTIONS_INSTANCE;
  options[2].subject = 0;
  options[2].key = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_BUF_SIZE__maxBlockLength);

  options[2].size = sizeof(float);
  options[2].type = ags_lv2_urid_manager_lookup(ags_lv2_urid_manager_get_instance(),
					       LV2_ATOM__Int);
  options[2].value = &(audio_signal->buffer_size);

  /* terminate */
  options[3].context = LV2_OPTIONS_INSTANCE;
  options[3].subject = 0;
  options[3].key = 0;

  options[3].size = 0;
  options[3].type = 0;
  options[3].value = NULL;

  /* set options */
  ags_lv2_option_manager_lv2_options_set(recall_lv2_run->lv2_handle[0],
					 options);

  /*  */  
  free(path);
  
#ifdef AGS_DEBUG
  g_message("instantiate LV2 handle\0");
#endif

  ags_recall_lv2_run_load_ports(recall_lv2_run);

  /* can't be done in ags_recall_lv2_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  for(i = 0; i < recall_lv2->input_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect port: %d\0", recall_lv2->input_port[i]);
#endif
    
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->input_port[i],
						recall_lv2_run->input);
  }

  for(i = 0; i < recall_lv2->output_lines; i++){
#ifdef AGS_DEBUG
    g_message("connect port: %d\0", recall_lv2->output_port[i]);
#endif
    
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->output_port[i],
						recall_lv2_run->output);
  }

  /* connect event port */
  if((AGS_RECALL_LV2_HAS_EVENT_PORT & (recall_lv2->flags)) != 0){
    recall_lv2_run->event_port = ags_lv2_plugin_alloc_event_buffer(AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT);
    
    recall_lv2->plugin_descriptor->connect_port(recall_lv2_run->lv2_handle[0],
						recall_lv2->event_port,
						recall_lv2_run->event_port);
  }
  
  /* connect atom port */
  if((AGS_RECALL_LV2_HAS_ATOM_PORT & (recall_lv2->flags)) != 0){
    recall_lv2_run->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_RECALL_LV2_DEFAULT_MIDI_LENGHT);
    
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
  AgsAudioSignal *audio_signal;
  AgsRecallLv2 *recall_lv2;
  AgsRecallLv2Run *recall_lv2_run;

  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsRouteLv2AudioRun *route_lv2_audio_run;

  guint copy_mode_in, copy_mode_out;
  uint32_t buffer_size;
  uint32_t i;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_pre(recall);

  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);
  
  if((AGS_RECALL_LV2_HAS_EVENT_PORT & (recall_lv2->flags)) == 0 &&
     (AGS_RECALL_LV2_HAS_ATOM_PORT & (recall_lv2->flags)) == 0){
    return;
  }
  
  if(recall_lv2_run->route_lv2_audio_run != NULL){
    route_lv2_audio_run = AGS_ROUTE_LV2_AUDIO_RUN(recall_lv2_run->route_lv2_audio_run);
    count_beats_audio_run = route_lv2_audio_run->count_beats_audio_run;
  }else{
    route_lv2_audio_run = NULL;
    count_beats_audio_run = NULL;
  }
  
  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_lv2_run)->source;
  buffer_size = audio_signal->buffer_size;

  if(audio_signal->stream_current == NULL ||
     (count_beats_audio_run == NULL ||
      ((AGS_NOTE(recall_lv2_run->note)->x[1] <= count_beats_audio_run->notation_counter  &&
      (AGS_NOTE_FEED & (AGS_NOTE(recall_lv2_run->note)->flags)) == 0) ||
       AGS_NOTE(recall_lv2_run->note)->x[0] > count_beats_audio_run->notation_counter))){
    //    g_message("done\0");
    /* deactivate */
    if(recall_lv2->plugin_descriptor->deactivate != NULL){
      recall_lv2->plugin_descriptor->deactivate(recall_lv2_run->lv2_handle[0]);
    }

    /* cleanup */
    if(recall_lv2->plugin_descriptor->cleanup != NULL){
      recall_lv2->plugin_descriptor->cleanup(recall_lv2_run->lv2_handle[0]);
    }

    ags_recall_done(recall);
    return;
  }

  /* get copy mode and clear buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;

  buffer_size = audio_signal->buffer_size;


  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_lv2_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->output, recall_lv2->output_lines,
				      buffer_size);
  }

  if(recall_lv2_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->input, recall_lv2->input_lines,
				      buffer_size);
  }

  /* copy data  */
  if(recall_lv2_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_lv2_run->input, (guint) recall_lv2->input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) audio_signal->buffer_size, copy_mode_in);
  }
  
  /* process data */
  recall_lv2->plugin_descriptor->run(recall_lv2_run->lv2_handle[0],
				     buffer_size);

  /* copy data */
  if(recall_lv2_run->output != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_lv2_run->output, (guint) recall_lv2->output_lines, 0,
						(guint) audio_signal->buffer_size, copy_mode_out);
  }
}

void
ags_recall_lv2_run_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *audio_signal;
  AgsRecallLv2 *recall_lv2;
  AgsRecallLv2Run *recall_lv2_run;

  guint copy_mode_in, copy_mode_out;
  uint32_t buffer_size;
  uint32_t i;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_lv2_run_parent_class)->run_inter(recall);

  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  recall_lv2_run = AGS_RECALL_LV2_RUN(recall);
  
  if((AGS_RECALL_LV2_HAS_EVENT_PORT & (recall_lv2->flags)) != 0 ||
     (AGS_RECALL_LV2_HAS_ATOM_PORT & (recall_lv2->flags)) != 0){
    return;
  }
  
  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_lv2_run)->source;
  buffer_size = audio_signal->buffer_size;

  if(audio_signal->stream_current == NULL){
    //    g_message("done\0");
    /* deactivate */
    if(recall_lv2->plugin_descriptor->deactivate != NULL){
      recall_lv2->plugin_descriptor->deactivate(recall_lv2_run->lv2_handle[0]);
    }

    /* cleanup */
    if(recall_lv2->plugin_descriptor->cleanup != NULL){
      recall_lv2->plugin_descriptor->cleanup(recall_lv2_run->lv2_handle[0]);
    }

    ags_recall_done(recall);
    return;
  }
  
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_lv2_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->output, recall_lv2->output_lines,
				      buffer_size);
  }

  if(recall_lv2_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_lv2_run->input, recall_lv2->input_lines,
				      buffer_size);
  }

  if(recall_lv2_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_lv2_run->input, (guint) recall_lv2->input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) audio_signal->buffer_size, copy_mode_in);
  }
  
  /* process data */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;

  buffer_size = audio_signal->buffer_size;

  recall_lv2->plugin_descriptor->run(recall_lv2_run->lv2_handle[0],
				     buffer_size);

  /* copy data */
  if(recall_lv2_run->output != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_lv2_run->output, (guint) recall_lv2->output_lines, 0,
						(guint) audio_signal->buffer_size, copy_mode_out);
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
  
  uint32_t i, j;

  recall_lv2 = AGS_RECALL_LV2(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_lv2_run)->parent->parent)->recall_channel);

  lv2_plugin = ags_lv2_manager_find_lv2_plugin(ags_lv2_manager_get_instance(),
					       recall_lv2->filename, recall_lv2->effect);

  if(AGS_BASE_PLUGIN(lv2_plugin)->port != NULL){
    port_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->port;

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

#ifdef AGS_DEBUG
	g_message("connect port: %d\0", AGS_PORT_DESCRIPTOR(port_descriptor->data)->port_index);
#endif
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

