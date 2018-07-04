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

#include <ags/audio/ags_recall_dssi.h>
#include <ags/audio/ags_recall_dssi_run.h>

#include <ags/libags.h>

#include <ags/plugin/ags_dssi_manager.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_route_dssi_audio_run.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void ags_recall_dssi_run_class_init(AgsRecallDssiRunClass *recall_dssi_run_class);
void ags_recall_dssi_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_dssi_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_dssi_run_init(AgsRecallDssiRun *recall_dssi_run);
void ags_recall_dssi_run_connect(AgsConnectable *connectable);
void ags_recall_dssi_run_disconnect(AgsConnectable *connectable);
void ags_recall_dssi_run_finalize(GObject *gobject);

void ags_recall_dssi_run_run_init_pre(AgsRecall *recall);
void ags_recall_dssi_run_run_pre(AgsRecall *recall);

void ags_recall_dssi_run_load_ports(AgsRecallDssiRun *recall_dssi_run);

/**
 * SECTION:ags_recall_dssi_run
 * @Short_description: The object interfacing with DSSI
 * @Title: AgsRecallDssiRun
 *
 * #AgsRecallDssiRun provides DSSI support.
 */

static gpointer ags_recall_dssi_run_parent_class = NULL;
static AgsConnectableInterface* ags_recall_dssi_run_parent_connectable_interface;

GType
ags_recall_dssi_run_get_type (void)
{
  static GType ags_type_recall_dssi_run = 0;

  if(!ags_type_recall_dssi_run){
    static const GTypeInfo ags_recall_dssi_run_info = {
      sizeof (AgsRecallDssiRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_dssi_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallDssiRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_dssi_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_dssi_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_dssi_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_dssi_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
						      "AgsRecallDssiRun",
						      &ags_recall_dssi_run_info,
						      0);

    g_type_add_interface_static(ags_type_recall_dssi_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_dssi_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_recall_dssi_run);
}

void
ags_recall_dssi_run_class_init(AgsRecallDssiRunClass *recall_dssi_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_dssi_run_parent_class = g_type_class_peek_parent(recall_dssi_run);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_dssi_run;

  gobject->finalize = ags_recall_dssi_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_dssi_run;

  recall->run_init_pre = ags_recall_dssi_run_run_init_pre;
  recall->run_pre = ags_recall_dssi_run_run_pre;
}


void
ags_recall_dssi_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_dssi_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_dssi_run_connect;
  connectable->disconnect = ags_recall_dssi_run_disconnect;
}

void
ags_recall_dssi_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  //TODO:JK: implement me
}

void
ags_recall_dssi_run_init(AgsRecallDssiRun *recall_dssi_run)
{
  recall_dssi_run->audio_channels = 0;
  
  recall_dssi_run->ladspa_handle = NULL;

  recall_dssi_run->input = NULL;
  recall_dssi_run->output = NULL;

  recall_dssi_run->port_data = NULL;
  
  recall_dssi_run->delta_time = 0;
  
  recall_dssi_run->event_buffer = (snd_seq_event_t **) malloc(2 * sizeof(snd_seq_event_t *));

  recall_dssi_run->event_buffer[0] = (snd_seq_event_t *) malloc(sizeof(snd_seq_event_t));
  memset(recall_dssi_run->event_buffer[0], 0, sizeof(snd_seq_event_t));
  
  recall_dssi_run->event_buffer[1] = NULL;

  recall_dssi_run->event_count = (unsigned long *) malloc(2 * sizeof(unsigned long));

  recall_dssi_run->event_count[0] = 0;
  recall_dssi_run->event_count[1] = 0;

  recall_dssi_run->note = NULL;
  recall_dssi_run->route_dssi_audio_run = NULL;
}

void
ags_recall_dssi_run_connect(AgsConnectable *connectable)
{
  ags_recall_dssi_run_parent_connectable_interface->connect(connectable);
}

void
ags_recall_dssi_run_disconnect(AgsConnectable *connectable)
{
  ags_recall_dssi_run_parent_connectable_interface->disconnect(connectable);
}

void
ags_recall_dssi_run_finalize(GObject *gobject)
{
  AgsRecallDssi *recall_dssi;
  AgsRecallDssiRun *recall_dssi_run;
  
  unsigned long i;

  recall_dssi_run = AGS_RECALL_DSSI_RUN(gobject);

  free(recall_dssi_run->output);
  free(recall_dssi_run->input);

  if(recall_dssi_run->port_data != NULL){
    free(recall_dssi_run->port_data);
  }
  
  if(recall_dssi_run->event_buffer != NULL){
    if(recall_dssi_run->event_buffer[0] != NULL){
      free(recall_dssi_run->event_buffer[0]);
    }
    
    free(recall_dssi_run->event_buffer);
  }

  if(recall_dssi_run->event_count != NULL){
    free(recall_dssi_run->event_count);
  }

  free(recall_dssi_run->ladspa_handle);

  g_list_free_full(recall_dssi_run->note,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_recall_dssi_run_parent_class)->finalize(gobject);
}

void
ags_recall_dssi_run_run_init_pre(AgsRecall *recall)
{
  AgsRecallDssi *recall_dssi;
  AgsRecallDssiRun *recall_dssi_run;
  AgsAudioSignal *audio_signal;

  AgsDssiPlugin *dssi_plugin;

  DSSI_Descriptor *plugin_descriptor;
  
  guint output_lines, input_lines;
  unsigned long port_count;
  unsigned long samplerate;
  unsigned long buffer_size;
  unsigned long i, i_stop;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_dssi_run_parent_class)->run_init_pre(recall);

  recall_dssi_run = AGS_RECALL_DSSI_RUN(recall);
  recall_dssi = AGS_RECALL_DSSI(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  
  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  dssi_plugin = recall_dssi->plugin;

  plugin_descriptor = recall_dssi->plugin_descriptor;
  
  output_lines = recall_dssi->output_lines;
  input_lines = recall_dssi->input_lines;
  
  pthread_mutex_unlock(recall_mutex);

  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_dssi_run)->source;
  
  /* set up buffer */ 
  samplerate = audio_signal->samplerate;
  buffer_size = audio_signal->buffer_size;

  if(input_lines > 0){
    recall_dssi_run->input = (LADSPA_Data *) malloc(input_lines *
						    buffer_size *
						    sizeof(LADSPA_Data));
  }

  recall_dssi_run->output = (LADSPA_Data *) malloc(output_lines *
						   buffer_size *
						   sizeof(LADSPA_Data));

  if(input_lines < output_lines){
    i_stop = output_lines;
  }else{
    i_stop = input_lines;
  }
  
  recall_dssi_run->audio_channels = i_stop;

  if(i_stop > 0){
    recall_dssi_run->ladspa_handle = (LADSPA_Handle *) malloc(i_stop *
							      sizeof(LADSPA_Handle));
  }else{
    recall_dssi_run->ladspa_handle = NULL;
  }
  
  for(i = 0; i < i_stop; i++){
    /* instantiate dssi */
    recall_dssi_run->ladspa_handle[i] = (LADSPA_Handle *) ags_base_plugin_instantiate(AGS_BASE_PLUGIN(dssi_plugin),
										      samplerate);

#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle %d %d",
	      recall_dssi->bank,
	      recall_dssi->program);
#endif
  }

  ags_recall_dssi_run_load_ports(recall_dssi_run);

  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* port count */
  pthread_mutex_lock(base_plugin_mutex);

  port_count = plugin_descriptor->LADSPA_Plugin->PortCount;

  pthread_mutex_unlock(base_plugin_mutex);

  if(port_count > 0){
    AgsPort *current;
    
    GList *list;

    gchar *specifier;
    
    recall_dssi_run->port_data = (LADSPA_Data *) malloc(port_count * sizeof(LADSPA_Data));
  }
  
  for(i = 0; i < i_stop; i++){
    ags_base_plugin_activate(AGS_BASE_PLUGIN(dssi_plugin),
			     recall_dssi_run->ladspa_handle[i]);
    
#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle");
#endif

  }
}

void
ags_recall_dssi_run_run_pre(AgsRecall *recall)
{
  AgsRecallDssi *recall_dssi;
  AgsRecallDssiRun *recall_dssi_run;
  AgsAudioSignal *audio_signal;
  AgsPort *current;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsRouteDssiAudioRun *route_dssi_audio_run;

  AgsDssiPlugin *dssi_plugin;
    
  DSSI_Descriptor *plugin_descriptor;

  GList *list, *port;
  
  GList *note, *note_next;

  snd_seq_event_t **event_buffer;
  unsigned long *event_count;
    
  gchar *specifier;
  
  LADSPA_Data port_data;
  
  guint output_lines, input_lines;
  unsigned long port_count;
  unsigned long bank, program;
  guint copy_mode_in, copy_mode_out;
  unsigned long buffer_size;
  unsigned long i, i_stop;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;
  
  /* call parent */
  AGS_RECALL_CLASS(ags_recall_dssi_run_parent_class)->run_pre(recall);

  if(recall->rt_safe &&
     recall->recall_id->recycling_context->parent != NULL &&
     AGS_RECALL_AUDIO_SIGNAL(recall)->source->note == NULL){
    return;
  }
  
  recall_dssi = AGS_RECALL_DSSI(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  recall_dssi_run = AGS_RECALL_DSSI_RUN(recall);

  route_dssi_audio_run = AGS_ROUTE_DSSI_AUDIO_RUN(recall_dssi_run->route_dssi_audio_run);

  if(route_dssi_audio_run == NULL){
    return;
  }
  
  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(recall_dssi)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  dssi_plugin = recall_dssi->plugin;

  plugin_descriptor = recall_dssi->plugin_descriptor;

  input_lines = recall_dssi->input_lines;
  output_lines = recall_dssi->output_lines;

  bank = recall_dssi->bank;
  program = recall_dssi->program;
  
  pthread_mutex_unlock(recall_mutex);

  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* some fields */
  count_beats_audio_run = route_dssi_audio_run->count_beats_audio_run;

  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  buffer_size = audio_signal->buffer_size;

  if(input_lines < output_lines){
    i_stop = output_lines;
  }else{
    i_stop = input_lines;
  }

  if(AGS_RECALL(recall_dssi_run)->rt_safe){
    note = recall_dssi_run->note;

    while(note != NULL){
      note_next = note->next;
      
      if((AGS_NOTE(note->data)->x[1] <= count_beats_audio_run->notation_counter &&
	  (AGS_NOTE_FEED & (AGS_NOTE(note->data)->flags)) == 0) ||
	 AGS_NOTE(note->data)->x[0] > count_beats_audio_run->notation_counter){
	recall_dssi_run->note = g_list_remove(recall_dssi_run->note,
					      note->data);
      }
    
      note = note_next;
    }

    if(recall_dssi_run->note == NULL){
      memset(recall_dssi_run->event_buffer[0], 0, sizeof(snd_seq_event_t));
    }
  }else{
    if(audio_signal->stream_current == NULL ||
       (recall_dssi_run->note != NULL &&
	((AGS_NOTE(recall_dssi_run->note->data)->x[1] <= count_beats_audio_run->notation_counter &&
	  (AGS_NOTE_FEED & (AGS_NOTE(recall_dssi_run->note->data)->flags)) == 0) ||
	 AGS_NOTE(recall_dssi_run->note->data)->x[0] > count_beats_audio_run->notation_counter))){
      //    g_message("done");
    
      for(i = 0; i < i_stop; i++){
	/* deactivate */
	//TODO:JK: fix-me
	ags_base_plugin_deactivate(dssi_plugin,
				   recall_dssi_run->ladspa_handle[i]);
	
	pthread_mutex_lock(base_plugin_mutex);

	plugin_descriptor->LADSPA_Plugin->cleanup(recall_dssi_run->ladspa_handle[i]);

	pthread_mutex_unlock(base_plugin_mutex);
      }

      ags_recall_done(recall);
      
      return;
    }
  }
  
  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_dssi_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_dssi_run->output, output_lines,
				      buffer_size);
  }

  if(recall_dssi_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_dssi_run->input, input_lines,
				      buffer_size);
  }

  /* copy data  */
  if(recall_dssi_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_dssi_run->input, (guint) input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }

  /* select program */
  pthread_mutex_lock(base_plugin_mutex);
  
  port_count = plugin_descriptor->LADSPA_Plugin->PortCount;

  pthread_mutex_unlock(base_plugin_mutex);

  /* cache port data */    
  for(i = 0; i < port_count; i++){
    pthread_mutex_lock(base_plugin_mutex);
    
    specifier = plugin_descriptor->LADSPA_Plugin->PortNames[i];

    list = AGS_RECALL(recall_dssi)->port;

    while(list != NULL){
      current = list->data;

      if(!g_strcmp0(specifier,
		    current->specifier)){	  
	break;
      }

      list = list->next;
    }

    pthread_mutex_unlock(base_plugin_mutex);

    if(list != NULL){
      GValue value = {0,};
	  
      g_value_init(&value,
		   G_TYPE_FLOAT);
      ags_port_safe_read(current,
			 &value);
	
      recall_dssi_run->port_data[i] = g_value_get_float(&value);
    }
  }
    
  if(plugin_descriptor->select_program != NULL){    
    for(i = 0; i < i_stop; i++){
      ags_dssi_plugin_real_change_program(dssi_plugin,
					  recall_dssi_run->ladspa_handle[i],
					  bank,
					  program)

      //      g_message("b p %u %u", recall_dssi->bank, recall_dssi->program);
    }
  }

  /* reset port data */    
  for(i = 0; i < port_count; i++){
    current = NULL;
    
    pthread_mutex_lock(base_plugin_mutex);

    specifier = plugin_descriptor->LADSPA_Plugin->PortNames[i];

    list = AGS_RECALL(recall_dssi)->port;

    while(list != NULL){
      current = list->data;

      if(!g_strcmp0(specifier,
		    current->specifier)){
	break;
      }

      list = list->next;
    }

    pthread_mutex_unlock(base_plugin_mutex);

    if(list != NULL){
      GValue value = {0,};
      
      g_value_init(&value,
		   G_TYPE_FLOAT);
      port_data = recall_dssi_run->port_data[i];

      g_value_set_float(&value,
			port_data);
      ags_port_safe_write(current,
			  &value);
    }
  }
  
  /* process data */
  note = recall_dssi_run->note;

  while(note != NULL){    
    event_buffer = recall_dssi_run->event_buffer;
    event_count = recall_dssi_run->event_count;
    
    while(*event_buffer != NULL){
      ags_base_plugin_run(dssi_plugin,
			  recall_dssi_run->ladspa_handle[0],
			  seq_event,
			  buffer_size);
      
      event_buffer++;
      event_count++;
    }

    note = note->next;
  }
  
  /* copy data */
  if(recall_dssi_run->output != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_dssi_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }
}

/**
 * ags_recall_dssi_run_load_ports:
 * @recall_dssi_run: an #AgsRecallDssiRun
 *
 * Set up DSSI ports.
 *
 * Since: 1.0.0
 */
void
ags_recall_dssi_run_load_ports(AgsRecallDssiRun *recall_dssi_run)
{
  AgsRecallDssi *recall_dssi;
  AgsDssiPlugin *dssi_plugin;
  AgsPort *current;

  GList *port;
  GList *list;

  gchar *plugin_name;
  gchar *specifier;
  gchar *path;
  
  guint output_lines, input_lines;
  unsigned long port_count;
  unsigned long i, j, j_stop;

  DSSI_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  recall_dssi = AGS_RECALL_DSSI(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_dssi_run)->parent->parent)->recall_channel);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(recall_dssi)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  port = AGS_RECALL(recall_dssi)->port;
  
  plugin_descriptor = recall_dssi->plugin_descriptor;

  input_lines = recall_dssi->input_lines;
  output_lines = recall_dssi->output_lines;

  pthread_mutex_unlock(recall_mutex);
  
  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(dssi_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  port_count = plugin_descriptor->LADSPA_Plugin->PortCount;
  
  port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;

  pthread_mutex_unlock(base_plugin_mutex);

  if(input_lines < output_lines){
    j_stop = output_lines;
  }else{
    j_stop = input_lines;
  }

  for(i = 0; i < port_count; i++){
    int descriptor;
    
    pthread_mutex_lock(base_plugin_mutex);

    descriptor = port_descriptor[i];
    
    pthread_mutex_unlock(base_plugin_mutex);
    
    if(LADSPA_IS_PORT_CONTROL(descriptor)){
      if(LADSPA_IS_PORT_INPUT(descriptor) ||
	 LADSPA_IS_PORT_OUTPUT(descriptor)){
	LADSPA_Data *port_pointer;

	pthread_mutex_lock(base_plugin_mutex);
	
	specifier = g_strdup(plugin_descriptor->LADSPA_Plugin->PortNames[i]);

	pthread_mutex_unlock(base_plugin_mutex);

	/* check port */
	pthread_mutex_lock(recall_mutex);
	
	list = port;
	current = NULL;
	
	while(list != NULL){
	  current = list->data;

	  if(!g_strcmp0(specifier,
			current->specifier)){
	    break;
	  }

	  list = list->next;
	}
	
	pthread_mutex_unlock(recall_mutex);

	g_free(specifier);
	
	/* connect port */
	for(j = 0; j < j_stop; j++){
#ifdef AGS_DEBUG
	  g_message("connecting port[%d]: %d/%d - %f", j, i, port_count, current->port_value.ags_port_ladspa);
#endif
	  pthread_mutex_lock(current->mutex);
	  
	  port_pointer = (LADSPA_Data *) &(current->port_value.ags_port_ladspa);

	  pthread_mutex_unlock(current->mutex);
	  
	  /* connect */
	  pthread_mutex_lock(base_plugin_mutex);
	  
	  plugin_descriptor->LADSPA_Plugin->connect_port(recall_dssi_run->ladspa_handle[j],
							 i,
							 port_pointer);

	  pthread_mutex_unlock(base_plugin_mutex);
	}
      }
    }
  }

  /* connect audio port */
  for(j = 0; j < input_lines; j++){
    unsigned long port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);
    
    port_index = recall_dssi->input_port[j];

    pthread_mutex_unlock(recall_mutex);

    /* connect */
    pthread_mutex_lock(base_plugin_mutex);
    
    plugin_descriptor->LADSPA_Plugin->connect_port(recall_dssi_run->ladspa_handle[j],
								port_index,
								&(recall_dssi_run->input[j]));

    pthread_mutex_unlock(base_plugin_mutex);
  }
  
  for(j = 0; j < output_lines; j++){
    unsigned long port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);

    port_index = recall_dssi->output_port[j];

    pthread_mutex_unlock(recall_mutex);
    
    /* connect */
    pthread_mutex_lock(base_plugin_mutex);

    plugin_descriptor->LADSPA_Plugin->connect_port(recall_dssi_run->ladspa_handle[j],
						   port_index,
						   &(recall_dssi_run->output[j]));

    pthread_mutex_unlock(base_plugin_mutex);
  }
}

/**
 * ags_recall_dssi_run_new:
 * @audio_signal: the source
 *
 * Creates a #AgsRecallDssiRun
 *
 * Returns: a new #AgsRecallDssiRun
 *
 * Since: 1.0.0
 */
AgsRecallDssiRun*
ags_recall_dssi_run_new(AgsAudioSignal *audio_signal)
{
  AgsRecallDssiRun *recall_dssi_run;

  recall_dssi_run = (AgsRecallDssiRun *) g_object_new(AGS_TYPE_RECALL_DSSI_RUN,
						      "source", audio_signal,
						      NULL);

  return(recall_dssi_run);
}
