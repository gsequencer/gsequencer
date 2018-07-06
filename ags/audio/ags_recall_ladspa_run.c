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

#include <ags/audio/ags_recall_ladspa.h>
#include <ags/audio/ags_recall_ladspa_run.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_manager.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <dlfcn.h>
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_recall_ladspa_run;

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

    g_once_init_leave (&g_define_type_id__volatile, ags_type_recall_ladspa_run);
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

  free(recall_ladspa_run->ladspa_handle);

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

  AgsLadspaPlugin *ladspa_plugin;

  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;

  guint output_lines, input_lines;
  unsigned long samplerate;
  unsigned long buffer_size;
  unsigned long i, i_stop;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_init_pre(recall);

  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);
  recall_ladspa = AGS_RECALL_LADSPA(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(recall_ladspa)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  ladspa_plugin = recall_ladspa->plugin;

  output_lines = recall_ladspa->output_lines;
  input_lines = recall_ladspa->input_lines;
  
  pthread_mutex_unlock(recall_mutex);

  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(ladspa_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());
  
  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);
  
  port_descriptor = plugin_descriptor->PortDescriptors;

  pthread_mutex_unlock(base_plugin_mutex);

  /* set up buffer */
  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall_ladspa_run)->source;
  
  /* set up buffer */ 
  samplerate = audio_signal->samplerate;
  buffer_size = audio_signal->buffer_size;

  recall_ladspa_run->input = (LADSPA_Data *) malloc(input_lines *
						    buffer_size *
						    sizeof(LADSPA_Data));
  recall_ladspa_run->output = (LADSPA_Data *) malloc(output_lines *
						     buffer_size *
						     sizeof(LADSPA_Data));

  recall_ladspa_run->ladspa_handle = (LADSPA_Handle *) malloc(input_lines *
							      sizeof(LADSPA_Handle));

  if(input_lines < output_lines){
    i_stop = output_lines;
  }else{
    i_stop = input_lines;
  }

  recall_ladspa_run->audio_channels = i_stop;
  
  for(i = 0; i < i_stop; i++){
    /* instantiate ladspa */
    recall_ladspa_run->ladspa_handle[i] = (LADSPA_Handle *) ags_base_plugin_instantiate(AGS_BASE_PLUGIN(ladspa_plugin),
											samplerate);

#ifdef AGS_DEBUG
      g_message("instantiate LADSPA handle");
#endif

  }

  ags_recall_ladspa_run_load_ports(recall_ladspa_run);

  /* can't be done in ags_recall_ladspa_run_run_init_inter since possebility of overlapping buffers */
  /* connect audio port */
  for(i = 0; i < input_lines; i++){
    unsigned long port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);
    
    port_index = recall_ladspa->input_port[i];

    pthread_mutex_unlock(recall_mutex);

    /* connect */
    pthread_mutex_lock(base_plugin_mutex);

    plugin_descriptor->connect_port(recall_ladspa_run->ladspa_handle[i],
				    port_index,
				    &(recall_ladspa_run->input[i]));

    pthread_mutex_unlock(base_plugin_mutex);
  }

  for(i = 0; i < output_lines; i++){
    unsigned long port_index;

    /* port index */
    pthread_mutex_lock(recall_mutex);
    
    port_index = recall_ladspa->output_port[i];

    pthread_mutex_unlock(recall_mutex);

    /* connect */
    pthread_mutex_lock(base_plugin_mutex);

    recall_ladspa->plugin_descriptor->connect_port(recall_ladspa_run->ladspa_handle[i],
						   port_index,
						   &(recall_ladspa_run->output[i]));

    pthread_mutex_unlock(base_plugin_mutex);
  }

  for(i = 0; i < i_stop; i++){
    ags_base_plugin_activate(AGS_BASE_PLUGIN(ladspa_plugin),
			     recall_ladspa_run->ladspa_handle[i]);
    
#ifdef AGS_DEBUG
      g_message("instantiate LADSPA handle");
#endif

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
  AgsAudioSignal *audio_signal;
  AgsRecallLadspa *recall_ladspa;
  AgsRecallLadspaRun *recall_ladspa_run;

  AgsLadspaPlugin *ladspa_plugin;
    
  LADSPA_Descriptor *plugin_descriptor;
  
  guint output_lines, input_lines;
  guint copy_mode_in, copy_mode_out;
  unsigned long buffer_size;
  unsigned long i;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;  

  /* call parent */
  AGS_RECALL_CLASS(ags_recall_ladspa_run_parent_class)->run_inter(recall);

  if(recall->rt_safe &&
     recall->recall_id->recycling_context->parent != NULL &&
     AGS_RECALL_AUDIO_SIGNAL(recall)->source->note == NULL){
    return;
  }

  recall_ladspa = AGS_RECALL_LADSPA(AGS_RECALL_CHANNEL_RUN(recall->parent->parent)->recall_channel);
  recall_ladspa_run = AGS_RECALL_LADSPA_RUN(recall);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(recall_ladspa)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  ladspa_plugin = recall_ladspa->plugin;

  plugin_descriptor = recall_ladspa->plugin_descriptor;

  input_lines = recall_ladspa->input_lines;
  output_lines = recall_ladspa->output_lines;

  pthread_mutex_unlock(recall_mutex);

  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(ladspa_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  audio_signal = AGS_RECALL_AUDIO_SIGNAL(recall)->source;
  buffer_size = audio_signal->buffer_size;

  if(audio_signal->stream_current == NULL){
    for(i = 0; i < input_lines; i++){
      /* deactivate */
      //TODO:JK: fix-me
      ags_base_plugin_deactivate(ladspa_plugin,
				 recall_ladspa_run->ladspa_handle[i]);
	
      pthread_mutex_lock(base_plugin_mutex);

      plugin_descriptor->cleanup(recall_ladspa_run->ladspa_handle[i]);

      pthread_mutex_unlock(base_plugin_mutex);
    }

    ags_recall_done(recall);

    return;
  }
  
  /* get copy mode and clear buffer */
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(audio_signal->format));

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(audio_signal->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  if(recall_ladspa_run->output != NULL){
    ags_audio_buffer_util_clear_float(recall_ladspa_run->output, recall_ladspa->output_lines,
				      buffer_size);
  }

  if(recall_ladspa_run->input != NULL){
    ags_audio_buffer_util_clear_float(recall_ladspa_run->input, recall_ladspa->input_lines,
				      buffer_size);
  }

  /* copy data  */
  if(recall_ladspa_run->input != NULL){
    ags_audio_buffer_util_copy_buffer_to_buffer(recall_ladspa_run->input, (guint) recall_ladspa->input_lines, 0,
						audio_signal->stream_current->data, 1, 0,
						(guint) audio_signal->buffer_size, copy_mode_in);
  }
  
  /* process data */
  ags_base_plugin_run(ladspa_plugin,
		      recall_ladspa_run->ladspa_handle[0],
		      NULL,
		      buffer_size);

  /* copy data */
  if(recall_ladspa_run->output != NULL){
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(audio_signal->format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(audio_signal->stream_current->data, 1, 0,
						recall_ladspa_run->output, (guint) output_lines, 0,
						(guint) buffer_size, copy_mode_out);
  }
}

/**
 * ags_recall_ladspa_run_load_ports:
 * @recall_ladspa_run: an #AgsRecallLadspaRun
 *
 * Set up LADSPA ports.
 *
 * Since: 1.0.0
 */
void
ags_recall_ladspa_run_load_ports(AgsRecallLadspaRun *recall_ladspa_run)
{
  AgsRecallLadspa *recall_ladspa;
  AgsLadspaPlugin *ladspa_plugin;
  AgsPort *current;

  LADSPA_Descriptor *plugin_descriptor;
  LADSPA_PortDescriptor *port_descriptor;

  GList *port;

  gchar *path;

  guint output_lines, input_lines;
  unsigned long port_count;
  unsigned long i, j, j_stop;

  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *base_plugin_mutex;

  recall_ladspa = AGS_RECALL_LADSPA(AGS_RECALL_CHANNEL_RUN(AGS_RECALL(recall_ladspa_run)->parent->parent)->recall_channel);

  /* recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = AGS_RECALL(recall_ladspa)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());
  
  /* get some fields */
  pthread_mutex_lock(recall_mutex);

  port = AGS_RECALL(recall_ladspa)->port;
  
  plugin_descriptor = recall_ladspa->plugin_descriptor;

  input_lines = recall_ladspa->input_lines;
  output_lines = recall_ladspa->output_lines;

  pthread_mutex_unlock(recall_mutex);

  /* base plugin mutex */
  pthread_mutex_lock(ags_base_plugin_get_class_mutex());

  base_plugin_mutex = AGS_BASE_PLUGIN(ladspa_plugin)->obj_mutex;
  
  pthread_mutex_unlock(ags_base_plugin_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(base_plugin_mutex);

  port_count = plugin_descriptor->PortCount;
  port_descriptor = plugin_descriptor->PortDescriptors;

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
	GList *list;
	
	LADSPA_Data *port_data;
	gchar *plugin_name;
	gchar *specifier;

	pthread_mutex_lock(base_plugin_mutex);
	
	specifier = g_strdup(plugin_descriptor->PortNames[i]);

	pthread_mutex_unlock(base_plugin_mutex);

	/* check port */
	pthread_mutex_lock(recall_mutex);
	
	current = NULL;
	list = port;

	while(list != NULL){
	  current = AGS_PORT(port->data);

	  if(!g_strcmp0(specifier,
			current->specifier)){
	    break;
	  }

	  list = list->next;
	}

	pthread_mutex_unlock(recall_mutex);

	g_free(specifier);
	
	/* connect port */
	if(current != NULL){
	  for(j = 0; j < j_stop; j++){
	    port_data = (LADSPA_Data *) &(current->port_value.ags_port_ladspa);
	    recall_ladspa->plugin_descriptor->connect_port(recall_ladspa_run->ladspa_handle[j],
							   i,
							   port_data);
	  
#ifdef AGS_DEBUG
	    g_message("connecting port[%lu]: %lu/%lu", j, i, port_count);
#endif	  
	  }
	}
      }else{
	g_critical("LADSPA port not found");
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
 *
 * Since: 1.0.0
 */
AgsRecallLadspaRun*
ags_recall_ladspa_run_new(AgsAudioSignal *audio_signal)
{
  AgsRecallLadspaRun *recall_ladspa_run;

  recall_ladspa_run = (AgsRecallLadspaRun *) g_object_new(AGS_TYPE_RECALL_LADSPA_RUN,
							  "source", audio_signal,
							  NULL);

  return(recall_ladspa_run);
}
