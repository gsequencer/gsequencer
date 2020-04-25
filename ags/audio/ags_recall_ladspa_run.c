/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_recall_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_recall_ladspa_run_class_init(AgsRecallLadspaRunClass *recall_ladspa_run_class);
void ags_recall_ladspa_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_ladspa_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_ladspa_run_init(AgsRecallLadspaRun *recall_ladspa_run);
void ags_recall_ladspa_run_finalize(GObject *gobject);

void ags_recall_ladspa_run_run_init_pre(AgsRecall *recall);
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
static AgsPluginInterface* ags_recall_ladspa_run_parent_plugin_interface;

GType
ags_recall_ladspa_run_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_ladspa_run = 0;

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
							"AgsRecallLadspaRun",
							&ags_recall_ladspa_run_info,
							0);

    g_type_add_interface_static(ags_type_recall_ladspa_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_ladspa_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_recall_ladspa_run);
  }

  return g_define_type_id__volatile;
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
  recall->run_inter = ags_recall_ladspa_run_run_inter;
}


void
ags_recall_ladspa_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_ladspa_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_recall_ladspa_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_ladspa_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_recall_ladspa_run_init(AgsRecallLadspaRun *recall_ladspa_run)
{
  recall_ladspa_run->ladspa_handle = NULL;

  recall_ladspa_run->audio_channels = 0;

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

  g_free(recall_ladspa_run->ladspa_handle);

  g_free(recall_ladspa_run->output);
  g_free(recall_ladspa_run->input);

  /* call parent */
  G_OBJECT_CLASS(ags_recall_ladspa_run_parent_class)->finalize(gobject);
}

void
ags_recall_ladspa_run_run_init_pre(AgsRecall *recall)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallLadspaRun *recall_ladspa_run;
  AgsAudioSignal *audio_signal;

  LADSPA_Handle *ladspa_handle;

  LADSPA_Data *output, *input;

  guint output_lines, input_lines;
  guint samplerate;
  guint buffer_size;
  guint port_count;
  guint i, i_stop;

  void (*parent_class_run_init_pre)(AgsRecall *recall);

  LADSPA_Handle (*instantiate)(const struct _LADSPA_Descriptor * Descriptor,
                               unsigned long SampleRate);
  void (*activate)(LADSPA_Handle Instance);

  GRecMutex *recall_ladspa_mutex;

  /* get recall mutex */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_init_pre;

  /* call parent */
  parent_class_run_init_pre(recall);

  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);

  g_object_get(recall,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_ladspa,
	       NULL);
  
  /* set up buffer */
  g_object_get(recall_ladspa_run,
	       "source", &audio_signal,
	       NULL);

  /* set up buffer */
  g_object_get(audio_signal,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);

  /* get recall ladspa mutex */
  recall_ladspa_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  /* get some fields */
  g_rec_mutex_lock(recall_ladspa_mutex);

  output_lines = recall_ladspa->output_lines;
  input_lines = recall_ladspa->input_lines;
  
  instantiate = recall_ladspa->plugin_descriptor->instantiate;
  activate = recall_ladspa->plugin_descriptor->activate;
  
  g_rec_mutex_unlock(recall_ladspa_mutex);
  
  /* set up buffer */ 
  output = NULL;
  input = NULL;

  if(input_lines > 0){
    input = (LADSPA_Data *) malloc(input_lines *
				   buffer_size *
				   sizeof(LADSPA_Data));
  }

  output = (LADSPA_Data *) malloc(output_lines *
				  buffer_size *
				  sizeof(LADSPA_Data));

  recall_ladspa_run->output = output;
  recall_ladspa_run->input = input;

  if(input_lines < output_lines){
    i_stop = output_lines;
  }else{
    i_stop = input_lines;
  }
  
  ladspa_handle = NULL;

  if(i_stop > 0){
    ladspa_handle = (LADSPA_Handle *) malloc(i_stop *
					     sizeof(LADSPA_Handle));
  }
  
  recall_ladspa_run->audio_channels = i_stop;
  
  /* instantiate ladspa */
  g_rec_mutex_lock(recall_ladspa_mutex);

  for(i = 0; i < i_stop; i++){
    ladspa_handle[i] = instantiate(recall_ladspa->plugin_descriptor,
				   (unsigned long) samplerate);
    
#ifdef AGS_DEBUG
      g_message("instantiated LADSPA handle");
#endif

  }

  g_rec_mutex_unlock(recall_ladspa_mutex);

  /* load ports */
  recall_ladspa_run->ladspa_handle = ladspa_handle;

  ags_recall_ladspa_run_load_ports(recall_ladspa_run);

  /* activate */
  for(i = 0; i < i_stop; i++){
    if(activate != NULL){
      activate(recall_ladspa_run->ladspa_handle[i]);
    }
    
#ifdef AGS_DEBUG
    g_message("activated LADSPA handle");
#endif
  }

  g_object_unref(recall_recycling);

  g_object_unref(recall_channel_run);

  g_object_unref(recall_ladspa);

  g_object_unref(audio_signal);
}

void
ags_recall_ladspa_run_run_inter(AgsRecall *recall)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsRecallLadspaRun *recall_ladspa_run;
  AgsAudioSignal *audio_signal;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *note_start, *note;

  guint output_lines, input_lines;
  guint copy_mode_in, copy_mode_out;
  guint buffer_size;
  guint i;

  void (*parent_class_run_inter)(AgsRecall *recall);

  void (*run)(LADSPA_Handle Instance,
	      unsigned long SampleCount);
  void (*deactivate)(LADSPA_Handle Instance);
  void (*cleanup)(LADSPA_Handle Instance);
  
  GRecMutex *recall_ladspa_mutex;

  /* get recall mutex */
  parent_class_run_inter = AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_inter;

  /* call parent */
  parent_class_run_inter(recall);

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
    g_object_unref(recall_id);

    g_object_unref(audio_signal);
    
    g_object_unref(recycling_context);

    g_object_unref(parent_recycling_context);
    
    return;
  }

  g_list_free_full(note_start,
		   g_object_unref);

  g_object_get(recall,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_ladspa,
	       NULL);

  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);

  /* get recall ladspa mutex */
  recall_ladspa_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  g_object_get(audio_signal,
	       "buffer-size", &buffer_size,
	       NULL);

  /* get some fields */
  g_rec_mutex_lock(recall_ladspa_mutex);

  output_lines = recall_ladspa->output_lines;
  input_lines = recall_ladspa->input_lines;

  run = recall_ladspa->plugin_descriptor->run;

  deactivate = recall_ladspa->plugin_descriptor->deactivate;
  cleanup = recall_ladspa->plugin_descriptor->cleanup;

  g_rec_mutex_unlock(recall_ladspa_mutex);

  if(audio_signal->stream_current == NULL){
    for(i = 0; i < input_lines; i++){
      /* deactivate */
      //TODO:JK: fix-me
      if(deactivate != NULL){
	deactivate(recall_ladspa_run->ladspa_handle[i]);
      }
      
      cleanup(recall_ladspa_run->ladspa_handle[i]);
    }

    ags_recall_done(recall);

    goto ags_recall_ladspa_run_run_inter_END;
  }
  
  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_ladspa_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_ladspa_run->output, output_lines,
				      buffer_size);
  }

  /* copy data  */
  if(recall_ladspa_run->input != NULL &&
     audio_signal->stream_current != NULL){
    ags_audio_buffer_util_clear_float(recall_ladspa_run->input, input_lines,
				      buffer_size);

    ags_audio_buffer_util_copy_buffer_to_buffer(recall_ladspa_run->input, (guint) input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) buffer_size, copy_mode_in);
  }
  
  /* process data */
  run(recall_ladspa_run->ladspa_handle[0],
      buffer_size);

  /* copy data */
  if(recall_ladspa_run->output != NULL &&
     audio_signal->stream_current != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_ladspa_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }
  
ags_recall_ladspa_run_run_inter_END:
  g_object_unref(recall_id);

  g_object_unref(recycling_context);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  g_object_unref(recall_recycling);
  g_object_unref(audio_signal);

  g_object_unref(recall_channel_run);

  g_object_unref(recall_ladspa);
}

/**
 * ags_recall_ladspa_run_load_ports:
 * @recall_ladspa_run: the #AgsRecallLadspaRun
 *
 * Set up LADSPA ports.
 *
 * Since: 3.0.0
 */
void
ags_recall_ladspa_run_load_ports(AgsRecallLadspaRun *recall_ladspa_run)
{
  AgsRecallLadspa *recall_ladspa;
  AgsRecallChannelRun *recall_channel_run;
  AgsRecallRecycling *recall_recycling;
  AgsPort *current_port;

  GList *list_start, *list;

  gchar *specifier, *current_specifier;
  
  guint output_lines, input_lines;
  guint port_count;
  guint i, j, j_stop;

  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortDescriptor current_port_descriptor;
  
  void (*connect_port)(LADSPA_Handle Instance,
		       unsigned long Port,
		       LADSPA_Data * DataLocation);

  GRecMutex *recall_ladspa_mutex;
  GRecMutex *port_mutex;

  if(!AGS_IS_RECALL_LADSPA_RUN(recall_ladspa_run)){
    return;
  }
  
  /* get recall mutex */

  g_object_get(recall_ladspa_run,
	       "parent", &recall_recycling,
	       NULL);

  g_object_get(recall_recycling,
	       "parent", &recall_channel_run,
	       NULL);

  g_object_get(recall_channel_run,
	       "recall-channel", &recall_ladspa,
	       NULL);
  
  /* get recall ladspa mutex */
  recall_ladspa_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall_ladspa);

  /* get some fields */
  g_rec_mutex_lock(recall_ladspa_mutex);
  
  output_lines = recall_ladspa->output_lines;
  input_lines = recall_ladspa->input_lines;

  list_start = g_list_copy(AGS_RECALL(recall_ladspa)->port);

  plugin_descriptor = recall_ladspa->plugin_descriptor;

  port_count = plugin_descriptor->PortCount;
  
  port_descriptor = plugin_descriptor->PortDescriptors;

  connect_port = recall_ladspa->plugin_descriptor->connect_port;
  
  g_rec_mutex_unlock(recall_ladspa_mutex);

  /* match port */
  if(input_lines < output_lines){
    j_stop = output_lines;
  }else{
    j_stop = input_lines;
  }

  for(i = 0; i < port_count; i++){
    g_rec_mutex_lock(recall_ladspa_mutex);

    current_port_descriptor = port_descriptor[i];
    
    g_rec_mutex_unlock(recall_ladspa_mutex);

    if(LADSPA_IS_PORT_CONTROL(current_port_descriptor)){
      if(LADSPA_IS_PORT_INPUT(current_port_descriptor) ||
	 LADSPA_IS_PORT_OUTPUT(current_port_descriptor)){
	LADSPA_Data *port_pointer;
	
	g_rec_mutex_lock(recall_ladspa_mutex);

	specifier = g_strdup(plugin_descriptor->PortNames[i]);

	g_rec_mutex_unlock(recall_ladspa_mutex);

	list = ags_port_find_specifier(list_start, specifier);
	g_free(specifier);

	if(list != NULL){
	  current_port = list->data;
	  
	  /* get port mutex */
	  port_mutex = AGS_PORT_GET_OBJ_MUTEX(current_port);

	  for(j = 0; j < j_stop; j++){
#ifdef AGS_DEBUG
	    g_message("connecting port[%d]: %d/%d - %f", j, i, port_count, current->port_value.ags_port_ladspa);
#endif
	    g_rec_mutex_lock(port_mutex);
	    
	    port_pointer = (LADSPA_Data *) &(current_port->port_value.ags_port_ladspa);

	    g_rec_mutex_unlock(port_mutex);
	  
	    connect_port(recall_ladspa_run->ladspa_handle[j],
			 (unsigned long) i,
			 port_pointer);
	  }
	}
      }
    }
  }

  g_list_free(list_start);
  
  /* connect audio port */
  for(j = 0; j < input_lines; j++){
    connect_port(recall_ladspa_run->ladspa_handle[j],
		 (unsigned long) (recall_ladspa->input_port[j]),
		 &(recall_ladspa_run->input[j]));
  }
  
  for(j = 0; j < recall_ladspa->output_lines; j++){
    connect_port(recall_ladspa_run->ladspa_handle[j],
		 (unsigned long) (recall_ladspa->output_port[j]),
		 &(recall_ladspa_run->output[j]));
  }

  g_object_unref(recall_recycling);

  g_object_unref(recall_channel_run);

  g_object_unref(recall_ladspa);
}

/**
 * ags_recall_ladspa_run_new:
 * @source: the #AgsAudioSignal as source
 *
 * Creates a new instance of #AgsRecallLadspaRun
 *
 * Returns: the new #AgsRecallLadspaRun
 *
 * Since: 3.0.0
 */
AgsRecallLadspaRun*
ags_recall_ladspa_run_new(AgsAudioSignal *source)
{
  AgsRecallLadspaRun *recall_ladspa_run;

  recall_ladspa_run = (AgsRecallLadspaRun *) g_object_new(AGS_TYPE_RECALL_LADSPA_RUN,
							  "source", source,
							  NULL);

  return(recall_ladspa_run);
}
