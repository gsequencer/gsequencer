/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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
void ags_recall_dssi_run_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_recall_dssi_run_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
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

enum{
  PROP_0,
  PROP_ROUTE_DSSI_AUDIO_RUN,
};

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

  gobject->set_property = ags_recall_dssi_run_set_property;
  gobject->get_property = ags_recall_dssi_run_get_property;

  gobject->finalize = ags_recall_dssi_run_finalize;

  /* properties */
  /**
   * AgsRecallDssiRun:route-dssi-audio-run:
   * 
   * The route dssi audio run dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("route-dssi-audio-run",
				   i18n_pspec("assigned AgsRouteDssiAudioRun"),
				   i18n_pspec("the AgsRouteDssiAudioRun"),
				   AGS_TYPE_ROUTE_DSSI_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ROUTE_DSSI_AUDIO_RUN,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_dssi_run;

  recall->run_init_pre = ags_recall_dssi_run_run_init_pre;
  recall->run_pre = ags_recall_dssi_run_run_pre;
}

void
ags_recall_dssi_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_dssi_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_dssi_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  //TODO:JK: implement me
}

void
ags_recall_dssi_run_init(AgsRecallDssiRun *recall_dssi_run)
{
  recall_dssi_run->ladspa_handle = NULL;

  recall_dssi_run->audio_channels = 0;

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
ags_recall_dssi_run_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsRouteDssiAudioRun *recall_dssi_run;

  pthread_mutex_t *recall_mutex;

  recall_dssi_run = AGS_RECALL_DSSI_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_NOTE:
    {
      GObject *note;

      note = g_value_get_pointer(value);

      pthread_mutex_lock(recall_mutex);

      if(!AGS_IS_NOTE(note) ||
	 g_list_find(recall_dssi_run->note, note) != NULL){
	pthread_mutex_lock(recall_mutex);
	
	return;
      }

      recall_dssi_run->note = g_list_prepend(recall_dssi_run->note,
					     note);
      g_object_ref(note);

      pthread_mutex_lock(recall_mutex);
    }
    break;
  case PROP_ROUTE_DSSI_AUDIO_RUN:
    {
      AgsDelayAudioRun *route_dssi_audio_run;

      route_dssi_audio_run = g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(route_dssi_audio_run == recall_dssi_run->route_dssi_audio_run){
	pthread_mutex_unlock(recall_mutex);
	
	return;
      }

      if(recall_dssi_run->route_dssi_audio_run != NULL){
	g_object_unref(G_OBJECT(recall_dssi_run->route_dssi_audio_run));
      }

      if(route_dssi_audio_run != NULL){
	g_object_ref(route_dssi_audio_run);
      }

      recall_dssi_run->route_dssi_audio_run = route_dssi_audio_run;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_recall_dssi_run_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsRouteDssiAudioRun *recall_dssi_run;
  
  pthread_mutex_t *recall_mutex;

  recall_dssi_run = AGS_RECALL_DSSI_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_ROUTE_DSSI_AUDIO_RUN:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, recall_dssi_run->route_dssi_audio_run);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_pointer(value,
			  g_list_copy(recall_dssi_run->note));

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
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
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallDssiRun *recall_dssi_run;
  AgsAudioSignal *audio_signal;

  guint output_lines, input_lines;
  guint samplerate;
  guint buffer_size;
  unsigned long port_count;
  unsigned long i, i_stop;
  
  /* call parent */
  AGS_RECALL_CLASS(ags_recall_dssi_run_parent_class)->run_init_pre(recall);

  recall_dssi_run = AGS_RECALL_DSSI_RUN(recall);

  g_object_get(recall,
	       "parent", &recal_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recal_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_dssi,
	       NULL);
  
  /* set up buffer */
  g_object_get(recall_dssi_run,
	       "source", &audio_signal,
	       NULL);

  /* set up buffer */
  g_object_get(audio_signal,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);

  /*  */
  if(recall_dssi->input_lines > 0){
    recall_dssi_run->input = (LADSPA_Data *) malloc(recall_dssi->input_lines *
						    buffer_size *
						    sizeof(LADSPA_Data));
  }

  recall_dssi_run->output = (LADSPA_Data *) malloc(recall_dssi->output_lines *
						   buffer_size *
						   sizeof(LADSPA_Data));

  if(recall_dssi->input_lines < recall_dssi->output_lines){
    i_stop = recall_dssi->output_lines;
  }else{
    i_stop = recall_dssi->input_lines;
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
    recall_dssi_run->ladspa_handle[i] = recall_dssi->plugin_descriptor->LADSPA_Plugin->instantiate(recall_dssi->plugin_descriptor->LADSPA_Plugin,
												   samplerate);

#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle %d %d",
	      recall_dssi->bank,
	      recall_dssi->program);
#endif
  }

  ags_recall_dssi_run_load_ports(recall_dssi_run);

  port_count = recall_dssi->plugin_descriptor->LADSPA_Plugin->PortCount;

  if(port_count > 0){
    AgsPort *current;
    
    GList *list;

    gchar *specifier;
    
    recall_dssi_run->port_data = (LADSPA_Data *) malloc(port_count * sizeof(LADSPA_Data));
  }
  
  for(i = 0; i < i_stop; i++){
    if(recall_dssi->plugin_descriptor->LADSPA_Plugin->activate != NULL){
      recall_dssi->plugin_descriptor->LADSPA_Plugin->activate(recall_dssi_run->ladspa_handle[i]);
    }
    
#ifdef AGS_DEBUG
    g_message("instantiate DSSI handle");
#endif

  }
}

void
ags_recall_dssi_run_run_pre(AgsRecall *recall)
{
  AgsRecallDssi *recall_dssi;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallDssiRun *recall_dssi_run;
  AgsAudioSignal *audio_signal;
  AgsPort *current_port;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;
  
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsRouteDssiAudioRun *route_dssi_audio_run;
    
  GList *list_start, *list;
  GList *port;
  
  GList *note_start, *note;

  snd_seq_event_t **event_buffer;
  unsigned long *event_count;
    
  gchar *specifier, *current_specifier;
  
  LADSPA_Data port_data;

  guint notation_counter;
  guint x0, x1;
  unsigned long port_count;
  
  guint copy_mode_in, copy_mode_out;
  unsigned long buffer_size;
  unsigned long i, i_stop;

  void (*parent_class_run_pre)(AgsRecall *recall);

  void (*run_synth)(LADSPA_Handle Instance,
		    unsigned long SampleCount,
		    snd_seq_event_t *Events,
		    unsigned long EventCount);
  void (*run)(LADSPA_Handle Instance,
	      unsigned long SampleCount);
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *recall_dssi_mutex;
  pthread_mutex_t *port_mutex;

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  parent_class_run_pre = AGS_RECALL_CLASS(ags_recall_dssi_run_parent_class)->run_pre;
  
  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_pre(recall);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       "source", &audio_signal,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_object_get(audio_signal,
	       "note", &note_start,
	       NULL);

  if(ags_recall_global_get_rt_safe() &&
     parent_recycling_context != NULL &&
     note_start == NULL){
    return;
  }

  g_list_free(note_start);
  
  g_object_get(recall,
	       "parent", &recal_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recal_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_dssi,
	       NULL);

  recall_dssi_run = AGS_RECALL_DSSI_RUN(recall);

  g_object_get(recall_dssi_run,
	       "route-dssi-audio-run", &route_dssi_audio_run,
	       NULL);
  
  if(route_dssi_audio_run == NULL){
    return;
  }

  /* get recall dssi mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_dssi_mutex = AGS_RECALL(recall_dssi)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  g_object_get(route_dssi_audio_run,
	       "count-beats-audio-run", &count_beats_audio_run,
	       NULL);

  g_object_get(audio_signal,
	       "buffer-size", &buffer_size,
	       NULL);

  if(recall_dssi->input_lines < recall_dssi->output_lines){
    i_stop = recall_dssi->output_lines;
  }else{
    i_stop = recall_dssi->input_lines;
  }

  g_object_get(count_beats_audio_run,
	       "notation-counter", &notation_counter,
	       NULL);
  
  g_object_get(recall_dssi_run,
	       "note", &note_start,
	       NULL);
  
  if(ags_recall_global_get_rt_safe()){
    note = note_start;
    
    while(note != NULL){
      g_object_get(note->data,
		   "x0", &x0,
		   "x1", &x1,
		   NULL);
	
      if((x1 <= notation_counter &&
	  !ags_note_test_flags(note->data, AGS_NOTE_FEED)) ||
	 x0 > notation_counter){
	recall_dssi_run->note = g_list_remove(recall_dssi_run->note,
					      note->data);
	g_object_unref(note->data);
      }
    
      note = note->next;
    }
    
    if(note_start == NULL){
      memset(recall_dssi_run->event_buffer[0], 0, sizeof(snd_seq_event_t));
    }

    g_list_free(note_start);
  }else{
    g_object_get(note_start->data,
		 "x0", &x0,
		 "x1", &x1,
		 NULL);
    
    if(audio_signal->stream_current == NULL ||
       (x1 <= notation_counter &&
	!ags_note_test_flags(note->data, AGS_NOTE_FEED)) ||
       x0 > notation_counter){
      //    g_message("done");

      pthread_mutex_lock(recall_dssi_mutex);
      
      for(i = 0; i < i_stop; i++){
	/* deactivate */
	//TODO:JK: fix-me
	if(recall_dssi->plugin_descriptor->LADSPA_Plugin->deactivate != NULL){
	  recall_dssi->plugin_descriptor->LADSPA_Plugin->deactivate(recall_dssi_run->ladspa_handle[i]);
	}
      
	recall_dssi->plugin_descriptor->LADSPA_Plugin->cleanup(recall_dssi_run->ladspa_handle[i]);
      }
      
      pthread_mutex_unlock(recall_dssi_mutex);
      
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
    ags_audio_buffer_util_clear_float(recall_dssi_run->output, recall_dssi->output_lines,
				      buffer_size);
  }

  if(recall_dssi_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_dssi_run->input, recall_dssi->input_lines,
				      buffer_size);
  }

  /* copy data  */
  if(recall_dssi_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_dssi_run->input, (guint) recall_dssi->input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }


  /* select program */
  pthread_mutex_lock(recall_dssi_mutex);

  port_count = recall_dssi->plugin_descriptor->LADSPA_Plugin->PortCount;

  pthread_mutex_unlock(recall_dssi_mutex);

  /* cache port data */
  g_object_get(recall_dssi,
	       "port", &list_start,
	       NULL);
  
  for(i = 0; i < port_count; i++){
    pthread_mutex_lock(recall_dssi_mutex);

    specifier = g_strdup(recall_dssi->plugin_descriptor->LADSPA_Plugin->PortNames[i]);

    pthread_mutex_unlock(recall_dssi_mutex);

    list = list_start;
    
    while(list != NULL){
      gboolean success;
      
      current_port = list->data;

      /* get port mutex */
      pthread_mutex_lock(ags_port_get_class_mutex());

      port_mutex = current_port->obj_mutex;
      
      pthread_mutex_unlock(ags_port_get_class_mutex());

      /* check specifier */
      pthread_mutex_lock(port_mutex);

      current_specifier = g_strdup(current_port->specifier);
      
      pthread_mutex_unlock(port_mutex);
      
      success = (!g_strcmp0(specifier,
			    current_specifier)) ? TRUE: FALSE;
      g_free(current_specifier);
	
      if(success){
	GValue value = {0,};
	  
	g_value_init(&value,
		     G_TYPE_FLOAT);
	ags_port_safe_read(current,
			   &value);
	
	recall_dssi_run->port_data[i] = g_value_get_float(&value);
	
	break;
      }

      list = list->next;
    }

    g_free(specifier);
  }
  
  pthread_mutex_lock(recall_dssi_mutex);

  if(recall_dssi->plugin_descriptor->select_program != NULL){    
    for(i = 0; i < i_stop; i++){
      recall_dssi->plugin_descriptor->select_program(recall_dssi_run->ladspa_handle[i],
						     recall_dssi->bank,
						     recall_dssi->program);

      //      g_message("b p %u %u", recall_dssi->bank, recall_dssi->program);
    }
  }

  pthread_mutex_unlock(recall_dssi_mutex);

  /* reset port data */    
  for(i = 0; i < port_count; i++){
    pthread_mutex_lock(recall_dssi_mutex);

    specifier = g_strdup(recall_dssi->plugin_descriptor->LADSPA_Plugin->PortNames[i]);

    pthread_mutex_unlock(recall_dssi_mutex);

    list = list_start;
    current_port = NULL;
    
    while(list != NULL){
      gboolean success;
      
      current_port = list->data;

      /* get port mutex */
      pthread_mutex_lock(ags_port_get_class_mutex());

      port_mutex = current_port->obj_mutex;
      
      pthread_mutex_unlock(ags_port_get_class_mutex());

      /* check specifier */
      pthread_mutex_lock(port_mutex);

      current_specifier = g_strdup(current_port->specifier);
      
      pthread_mutex_unlock(port_mutex);
      
      success = (!g_strcmp0(specifier,
			    current_specifier)) ? TRUE: FALSE;
      g_free(current_specifier);

      if(success){
	break;
      }

      list = list->next;
    }

    g_free(specifier);

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

  g_list_free(list_start);
  
  /* process data */
  pthread_mutex_lock(recall_dssi_mutex);

  run_synth = recall_dssi->plugin_descriptor->run_synth;
  run = recall_dssi->plugin_descriptor->LADSPA_Plugin->run;
  
  pthread_mutex_unlock(recall_dssi_mutex);


  g_object_get(recall_dssi_run,
	       "note", &note_start,
	       NULL);
  
  note = note_start;

  while(note != NULL){    
    if(run_synth != NULL){
      if(recall_dssi_run->event_buffer != NULL){
	event_buffer = recall_dssi_run->event_buffer;
	event_count = recall_dssi_run->event_count;
      
	while(*event_buffer != NULL){
	  if(event_buffer[0]->type == SND_SEQ_EVENT_NOTEON){
	    run_synth(recall_dssi_run->ladspa_handle[0],
		      recall_dssi->output_lines * buffer_size,
		      event_buffer[0],
		      event_count[0]);
	  }
	  
	  event_buffer++;
	  event_count++;
	}
      }
    }else if(run != NULL){
      run(recall_dssi_run->ladspa_handle[0],
	  buffer_size);
    }

    note = note->next;
  }

  g_list_free(note_start);
  
  /* copy data */
  if(recall_dssi_run->output != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_dssi_run->output, (guint) recall_dssi->output_lines, 0,
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
  
  unsigned long port_count;
  unsigned long i, j, j_stop;

  DSSI_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;

  recall_dssi = AGS_RECALL_DSSI(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_dssi_run)->parent->parent)->recall_channel);
  port = AGS_RECALL(recall_dssi)->port;
  
  plugin_descriptor = recall_dssi->plugin_descriptor;

  port_count = plugin_descriptor->LADSPA_Plugin->PortCount;
  
  port_descriptor = plugin_descriptor->LADSPA_Plugin->PortDescriptors;

  if(recall_dssi->input_lines < recall_dssi->output_lines){
    j_stop = recall_dssi->output_lines;
  }else{
    j_stop = recall_dssi->input_lines;
  }

  for(i = 0; i < port_count; i++){
    if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
      if(LADSPA_IS_PORT_INPUT(port_descriptor[i]) ||
	 LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	LADSPA_Data *port_pointer;
	
	specifier = plugin_descriptor->LADSPA_Plugin->PortNames[i];

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
	
	for(j = 0; j < j_stop; j++){
#ifdef AGS_DEBUG
	  g_message("connecting port[%d]: %d/%d - %f", j, i, port_count, current->port_value.ags_port_ladspa);
#endif
	  port_pointer = (LADSPA_Data *) &(current->port_value.ags_port_ladspa);
	  
	  recall_dssi->plugin_descriptor->LADSPA_Plugin->connect_port(recall_dssi_run->ladspa_handle[j],
								      i,
								      port_pointer);
	}
      }
    }
  }

  /* connect audio port */
  for(j = 0; j < recall_dssi->input_lines; j++){
    recall_dssi->plugin_descriptor->LADSPA_Plugin->connect_port(recall_dssi_run->ladspa_handle[j],
								recall_dssi->input_port[j],
								&(recall_dssi_run->input[j]));
  }
  
  for(j = 0; j < recall_dssi->output_lines; j++){
    recall_dssi->plugin_descriptor->LADSPA_Plugin->connect_port(recall_dssi_run->ladspa_handle[j],
								recall_dssi->output_port[j],
								&(recall_dssi_run->output[j]));
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
