/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_vst3_channel.h>

#include <ags/plugin/ags_vst3_manager.h>
#include <ags/plugin/ags_vst3_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_port_util.h>

#include <ags/audio/fx/ags_fx_vst3_audio.h>

#include <ags/i18n.h>

void ags_fx_vst3_channel_class_init(AgsFxVst3ChannelClass *fx_vst3_channel);
void ags_fx_vst3_channel_init(AgsFxVst3Channel *fx_vst3_channel);
void ags_fx_vst3_channel_dispose(GObject *gobject);
void ags_fx_vst3_channel_finalize(GObject *gobject);

void ags_fx_vst3_channel_notify_buffer_size_callback(GObject *gobject,
						     GParamSpec *pspec,
						     gpointer user_data);
void ags_fx_vst3_channel_notify_samplerate_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data);

void ags_fx_vst3_channel_safe_write_callback(AgsPort *port, GValue *value,
					     AgsFxVst3Channel *fx_vst3_channel);

/**
 * SECTION:ags_fx_vst3_channel
 * @short_description: fx vst3 channel
 * @title: AgsFxVst3Channel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_vst3_channel.h
 *
 * The #AgsFxVst3Channel class provides ports to the effect processor.
 */

static gpointer ags_fx_vst3_channel_parent_class = NULL;

const gchar *ags_fx_vst3_channel_plugin_name = "ags-fx-vst3";

GType
ags_fx_vst3_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_vst3_channel = 0;

    static const GTypeInfo ags_fx_vst3_channel_info = {
      sizeof (AgsFxVst3ChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_vst3_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxVst3Channel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_vst3_channel_init,
    };

    ags_type_fx_vst3_channel = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL,
						      "AgsFxVst3Channel",
						      &ags_fx_vst3_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_vst3_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_vst3_channel_class_init(AgsFxVst3ChannelClass *fx_vst3_channel)
{
  GObjectClass *gobject;

  ags_fx_vst3_channel_parent_class = g_type_class_peek_parent(fx_vst3_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_vst3_channel;

  gobject->dispose = ags_fx_vst3_channel_dispose;
  gobject->finalize = ags_fx_vst3_channel_finalize;
}

void
ags_fx_vst3_channel_init(AgsFxVst3Channel *fx_vst3_channel)
{
  guint i;
  
  g_signal_connect(fx_vst3_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_vst3_channel_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_vst3_channel, "notify::samplerate",
		   G_CALLBACK(ags_fx_vst3_channel_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_vst3_channel)->name = "ags-fx-vst3";
  AGS_RECALL(fx_vst3_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_vst3_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_vst3_channel)->xml_type = "ags-fx-vst3-channel";

  fx_vst3_channel->output_port_count = 0;
  fx_vst3_channel->output_port = NULL;
  
  fx_vst3_channel->input_port_count = 0;
  fx_vst3_channel->input_port = NULL;

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_vst3_channel->input_data[i] = ags_fx_vst3_channel_input_data_alloc();
      
    fx_vst3_channel->input_data[i]->parent = fx_vst3_channel;
  }
  
  fx_vst3_channel->vst3_plugin = NULL;

  fx_vst3_channel->vst3_port = NULL;
}

void
ags_fx_vst3_channel_dispose(GObject *gobject)
{
  AgsFxVst3Channel *fx_vst3_channel;
  
  fx_vst3_channel = AGS_FX_VST3_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_channel_parent_class)->dispose(gobject);
}

void
ags_fx_vst3_channel_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxVst3Channel *fx_vst3_channel;
  
  guint i;
  
  fx_vst3_channel = AGS_FX_VST3_CHANNEL(gobject);

  g_free(fx_vst3_channel->output_port);
  g_free(fx_vst3_channel->input_port);
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_vst3_channel_input_data_free(fx_vst3_channel->input_data[i]);
  }
  
  if(fx_vst3_channel->vst3_plugin != NULL){
    g_object_unref(fx_vst3_channel->vst3_plugin);
  }
  
  if(fx_vst3_channel->vst3_port != NULL){
    for(iter = fx_vst3_channel->vst3_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_vst3_channel->vst3_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_channel_parent_class)->finalize(gobject);
}

void
ags_fx_vst3_channel_notify_buffer_size_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFxVst3Channel *fx_vst3_channel;

  guint output_port_count, input_port_count;
  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_vst3_channel = AGS_FX_VST3_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_channel);

  /* get buffer size */
  g_object_get(fx_vst3_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_vst3_channel->output_port_count;
  input_port_count = fx_vst3_channel->input_port_count;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxVst3ChannelInputData *input_data;

    input_data = fx_vst3_channel->input_data[i];

    ags_vst_process_data_set_num_samples(input_data->process_data,
					 buffer_size);
    
    if(output_port_count > 0 &&
       buffer_size > 0){
      if(input_data->output == NULL){
	input_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
      }else{
	input_data->output = (float *) g_realloc(input_data->output,
						 output_port_count * buffer_size * sizeof(float));	    
      }
    }

    if(input_port_count > 0 &&
       buffer_size > 0){
      if(input_data->input == NULL){
	input_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
      }else{
	input_data->input = (float *) g_realloc(input_data->input,
						input_port_count * buffer_size * sizeof(float));
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_vst3_channel_notify_samplerate_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsFxVst3Channel *fx_vst3_channel;

  AgsVst3Plugin *vst3_plugin;

  guint output_port_count;
  guint buffer_size;
  guint samplerate;
  guint i;
  
  GRecMutex *recall_mutex;

  fx_vst3_channel = AGS_FX_VST3_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_channel);

  /* get VST3 plugin */
  g_rec_mutex_lock(recall_mutex);

  vst3_plugin = fx_vst3_channel->vst3_plugin;

  g_rec_mutex_unlock(recall_mutex);

  if(vst3_plugin == NULL){
    return;
  }
  
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_vst3_channel,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply samplerate */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxVst3ChannelInputData *input_data;

    input_data = fx_vst3_channel->input_data[i];

    //TODO:JK: implement me
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_vst3_channel_input_data_alloc:
 * 
 * Allocate #AgsFxVst3ChannelInputData-struct
 * 
 * Returns: the new #AgsFxVst3ChannelInputData-struct
 * 
 * Since: 3.10.5
 */
AgsFxVst3ChannelInputData*
ags_fx_vst3_channel_input_data_alloc()
{
  AgsFxVst3ChannelInputData *input_data;

  AgsVstAudioBusBuffers *output, *input;

  input_data = (AgsFxVst3ChannelInputData *) g_malloc(sizeof(AgsFxVst3ChannelInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->icomponent = NULL;
  input_data->iedit_controller = NULL;
  input_data->iaudio_processor = NULL;

  /* process data */
  input_data->process_data = ags_vst_process_data_alloc();

  ags_vst_process_data_set_process_mode(input_data->process_data,
					AGS_VST_KREALTIME);
    
  ags_vst_process_data_set_symbolic_sample_size(input_data->process_data,
						AGS_VST_KSAMPLE32);  

  ags_vst_process_data_set_num_samples(input_data->process_data,
				       0);

  ags_vst_process_data_set_num_inputs(input_data->process_data,
				      1);

  ags_vst_process_data_set_num_outputs(input_data->process_data,
				       1);

  /* input */
  input = ags_vst_audio_bus_buffers_alloc();

  ags_vst_audio_bus_buffers_set_num_channels(input,
					     1);

  ags_vst_audio_bus_buffers_set_silence_flags(input,
					      0);

  ags_vst_audio_bus_buffers_set_samples32(input,
					  &(input_data->input));

  ags_vst_process_data_set_inputs(input_data->process_data,
				  input);

  /* output */
  output = ags_vst_audio_bus_buffers_alloc();

  ags_vst_audio_bus_buffers_set_num_channels(output,
					     1);

  ags_vst_audio_bus_buffers_set_silence_flags(output,
					      0);

  ags_vst_audio_bus_buffers_set_samples32(output,
					  &(input_data->output));

  ags_vst_process_data_set_outputs(input_data->process_data,
				   output);    

  return(input_data);
}

/**
 * ags_fx_vst3_channel_input_data_free:
 * @input_data: the #AgsFxVst3ChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_channel_input_data_free(AgsFxVst3ChannelInputData *input_data)
{
  AgsVst3Plugin *vst3_plugin;
  
  if(input_data == NULL){
    return;
  }

  g_free(input_data->output);
  g_free(input_data->input);

  if(input_data->icomponent != NULL){
    GRecMutex *base_plugin_mutex;
  
    vst3_plugin = NULL;

    base_plugin_mutex = NULL;
    
    if(input_data->parent != NULL){
      vst3_plugin = AGS_FX_VST3_CHANNEL(input_data->parent)->vst3_plugin;
    }
    
    if(vst3_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(vst3_plugin);
    }

    //TODO:JK: implement me
  }
  
  g_free(input_data);
}

/**
 * ags_fx_vst3_channel_load_plugin:
 * @fx_vst3_channel: the #AgsFxVst3Channel
 * 
 * Load plugin of @fx_vst3_channel.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_channel_load_plugin(AgsFxVst3Channel *fx_vst3_channel)
{
  AgsVst3Manager *vst3_manager;
  AgsVst3Plugin *vst3_plugin;

#if defined(HAVE_GLIB_2_68)
  GStrvBuilder *strv_builder;
#endif
  
  gchar **parameter_name;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_VST3_CHANNEL(fx_vst3_channel)){
    return;
  }

  vst3_manager = ags_vst3_manager_get_instance();

#if defined(HAVE_GLIB_2_68)
  strv_builder = g_strv_builder_new();

  g_strv_builder_add(strv_builder,
		     "buffer-size");
  g_strv_builder_add(strv_builder,
		     "samplerate");
  g_strv_builder_add(strv_builder,
		     "iedit-controller");
  g_strv_builder_add(strv_builder,
		     "iaudio-processor");

  parameter_name = g_strv_builder_end(strv_builder);
#else
  parameter_name = (gchar **) g_malloc(5 * sizeof(gchar *));

  parameter_name[0] = g_strdup("buffer-size");
  parameter_name[1] = g_strdup("samplerate");
  parameter_name[2] = g_strdup("iedit-controller");
  parameter_name[3] = g_strdup("iaudio-processor");
  parameter_name[4] = NULL;
#endif
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_channel);

  /* get filename and effect */
  filename = NULL;
  effect = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_vst3_channel,
	       "filename", &filename,
	       "effect", &effect,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  vst3_plugin = fx_vst3_channel->vst3_plugin;

  if(vst3_plugin == NULL){
    vst3_plugin =
      fx_vst3_channel->vst3_plugin = ags_vst3_manager_find_vst3_plugin(vst3_manager,
								       filename, effect);
  }    
  
  g_rec_mutex_unlock(recall_mutex);
    
  if(vst3_plugin != NULL &&
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    GValue *value;

    guint i;
    guint n_params;
    
    n_params = 4;

    value = g_new0(GValue,
		   4);

    g_value_init(value,
		 G_TYPE_UINT);
    
    g_value_set_uint(value,
		     buffer_size);
    
    g_value_init(value + 1,
		 G_TYPE_UINT);

    g_value_set_uint(value + 1,
		     samplerate);
    
    g_value_init(value + 2,
		 G_TYPE_POINTER);

    g_value_init(value + 3,
		 G_TYPE_POINTER);

    /* set vst3 plugin */    
    g_rec_mutex_lock(recall_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxVst3ChannelInputData *input_data;

      input_data = fx_vst3_channel->input_data[i];

      if(input_data->icomponent == NULL){
	input_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									 &n_params,
									 &parameter_name, &value);
	      
	input_data->iedit_controller = g_value_get_pointer(value + 2);
	input_data->iaudio_processor = g_value_get_pointer(value + 3);
      }
    }
    
    g_rec_mutex_unlock(recall_mutex);
  }  

  g_strfreev(parameter_name);
  
  g_free(filename);
  g_free(effect);
}

/**
 * ags_fx_vst3_channel_load_port:
 * @fx_vst3_channel: the #AgsFxVst3Channel
 * 
 * Load port of @fx_vst3_channel.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_channel_load_port(AgsFxVst3Channel *fx_vst3_channel)
{
  AgsChannel *input;
  AgsFxVst3Audio *fx_vst3_audio;
  AgsPort **vst3_port;
  
  AgsVst3Plugin *vst3_plugin;

  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;
  
  guint audio_channel;
  guint pad;
  guint output_port_count, input_port_count;
  guint control_port_count;
  guint samplerate;
  guint buffer_size;
  guint nth;
  guint i, j;
  gboolean is_live_instrument;
  
  GRecMutex *fx_vst3_audio_mutex;
  GRecMutex *fx_vst3_channel_mutex;

  if(!AGS_IS_FX_VST3_CHANNEL(fx_vst3_channel)){
    return;
  }

  fx_vst3_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_channel);

  g_rec_mutex_lock(fx_vst3_channel_mutex);

  if(fx_vst3_channel->vst3_port != NULL){
    g_rec_mutex_unlock(fx_vst3_channel_mutex);
    
    return;
  }

  g_rec_mutex_unlock(fx_vst3_channel_mutex);
  
  fx_vst3_audio = NULL;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_vst3_channel,
	       "recall-audio", &fx_vst3_audio,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);

  if(fx_vst3_audio == NULL){
    return;
  }

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  if(is_live_instrument){
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }
    
    return;
  }
  
  /* get recall mutex */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  input = NULL;

  g_object_get(fx_vst3_channel,
	       "source", &input,
	       NULL);

  audio_channel = 0;
  pad = 0;

  g_object_get(input,
	       "audio-channel", &audio_channel,
	       "pad", &pad,
	       NULL);
  
  /* get VST3 plugin */
  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  output_port_count = fx_vst3_audio->output_port_count;
  output_port = fx_vst3_audio->output_port;

  input_port_count = fx_vst3_audio->input_port_count;
  input_port = fx_vst3_audio->input_port;

  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  /* get VST3 port */
  g_rec_mutex_lock(fx_vst3_channel_mutex);

  vst3_port = fx_vst3_channel->vst3_port;

  g_rec_mutex_unlock(fx_vst3_channel_mutex);

  start_plugin_port = NULL;
  
  g_object_get(vst3_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    /* get control port count */
    plugin_port = start_plugin_port;

    control_port_count = 0;
  
    while(plugin_port != NULL){
      if(ags_plugin_port_test_flags(plugin_port->data,
				    AGS_PLUGIN_PORT_CONTROL)){
	control_port_count++;
      }

      plugin_port = plugin_port->next;
    }

    /*  */
    if(control_port_count > 0){
      vst3_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

      plugin_port = start_plugin_port;
    
      for(nth = 0; nth < control_port_count && plugin_port != NULL; ){
	if(ags_plugin_port_test_flags(plugin_port->data,
				      AGS_PLUGIN_PORT_CONTROL)){
	  AgsPluginPort *current_plugin_port;

	  gchar *plugin_name;
	  gchar *specifier;
	  gchar *control_port;
      
	  guint port_index;
      
	  GValue default_value = {0,};

	  GRecMutex *plugin_port_mutex;
      
	  current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

	  /* get plugin port mutex */
	  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

	  /* plugin name, specifier and control port */
	  plugin_name = g_strdup_printf("vst3-<%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x>",
					vst3_plugin->cid[0],
					vst3_plugin->cid[1],
					vst3_plugin->cid[2],
					vst3_plugin->cid[3],
					vst3_plugin->cid[4],
					vst3_plugin->cid[5],
					vst3_plugin->cid[6],
					vst3_plugin->cid[7],
					vst3_plugin->cid[8],
					vst3_plugin->cid[9],
					vst3_plugin->cid[10],
					vst3_plugin->cid[11],
					vst3_plugin->cid[12],
					vst3_plugin->cid[13],
					vst3_plugin->cid[14],
					vst3_plugin->cid[15]);

	  specifier = NULL;
      
	  port_index = 0;
      
	  g_object_get(current_plugin_port,
		       "port-name", &specifier,
		       "port-index", &port_index,
		       NULL);

	  control_port = g_strdup_printf("%u/%u",
					 nth,
					 control_port_count);

	  /* default value */
	  g_value_init(&default_value,
		       G_TYPE_FLOAT);
      
	  g_rec_mutex_lock(plugin_port_mutex);
      
	  g_value_copy(current_plugin_port->default_value,
		       &default_value);
      
	  g_rec_mutex_unlock(plugin_port_mutex);

	  /* vst3 port */
	  vst3_port[nth] = g_object_new(AGS_TYPE_PORT,
					"plugin-name", plugin_name,
					"specifier", specifier,
					"control-port", control_port,
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					NULL);
      
	  if(ags_plugin_port_test_flags(current_plugin_port,
					AGS_PLUGIN_PORT_OUTPUT)){
	    ags_port_set_flags(vst3_port[nth], AGS_PORT_IS_OUTPUT);
	  
	    ags_recall_set_flags((AgsRecall *) fx_vst3_channel,
				 AGS_RECALL_HAS_OUTPUT_PORT);
	
	  }else{
	    if(!ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_INTEGER) &&
	       !ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_TOGGLED)){
	      ags_port_set_flags(vst3_port[nth], AGS_PORT_INFINITE_RANGE);
	    }
	  }
	
	  g_object_set(vst3_port[nth],
		       "plugin-port", current_plugin_port,
		       NULL);

	  ags_port_util_load_vst3_conversion(vst3_port[nth],
					     current_plugin_port);
	
	  ags_port_safe_write_raw(vst3_port[nth],
				  &default_value);

	  ags_recall_add_port((AgsRecall *) fx_vst3_channel,
			      vst3_port[nth]);
	
	  g_signal_connect_after(vst3_port[nth], "safe-write",
				 ags_fx_vst3_channel_safe_write_callback, fx_vst3_audio);

	  g_free(plugin_name);
	  g_free(specifier);
	  g_free(control_port);

	  g_value_unset(&default_value);

	  nth++;
	}

	plugin_port = plugin_port->next;
      }

      vst3_port[nth] = NULL;
    }

    /* set VST3 output */
    g_rec_mutex_lock(fx_vst3_channel_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxVst3AudioScopeData *scope_data;

      scope_data = fx_vst3_audio->scope_data[i];

      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	AgsFxVst3AudioChannelData *channel_data;
	
	channel_data = scope_data->channel_data[audio_channel];
	
	if(!is_live_instrument){	  
	  AgsFxVst3AudioInputData *input_data;
	    
	  guint nth;
	  
	  input_data = channel_data->input_data[pad];
	      
	  if(input_data->output == NULL &&
	     output_port_count > 0 &&
	     buffer_size > 0){
	    input_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
	  }
	  
	  if(input_data->input == NULL &&
	     input_port_count > 0 &&
	     buffer_size > 0){
	    input_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
	  }
	}
      }
    }
  
    g_rec_mutex_unlock(fx_vst3_channel_mutex);
  }else{
    /* get control port count */
    plugin_port = start_plugin_port;

    output_port = NULL;
    input_port = NULL;
  
    output_port_count = 0;
    input_port_count = 0;

    control_port_count = 0;
  
    while(plugin_port != NULL){
      if(ags_plugin_port_test_flags(plugin_port->data,
				    AGS_PLUGIN_PORT_CONTROL)){
	control_port_count++;
      }

      plugin_port = plugin_port->next;
    }

    /*  */
    if(control_port_count > 0){
      vst3_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

      plugin_port = start_plugin_port;
    
      for(nth = 0; nth < control_port_count && plugin_port != NULL;){
	if(ags_plugin_port_test_flags(plugin_port->data,
				      AGS_PLUGIN_PORT_CONTROL)){
	  AgsPluginPort *current_plugin_port;

	  gchar *plugin_name;
	  gchar *specifier;
	  gchar *control_port;

	  guint port_index;
      
	  GValue default_value = {0,};

	  GRecMutex *plugin_port_mutex;
      
	  current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

	  /* get plugin port mutex */
	  plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

	  /* plugin name, specifier and control port */
	  plugin_name = g_strdup_printf("vst3-<%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x>",
					vst3_plugin->cid[0],
					vst3_plugin->cid[1],
					vst3_plugin->cid[2],
					vst3_plugin->cid[3],
					vst3_plugin->cid[4],
					vst3_plugin->cid[5],
					vst3_plugin->cid[6],
					vst3_plugin->cid[7],
					vst3_plugin->cid[8],
					vst3_plugin->cid[9],
					vst3_plugin->cid[10],
					vst3_plugin->cid[11],
					vst3_plugin->cid[12],
					vst3_plugin->cid[13],
					vst3_plugin->cid[14],
					vst3_plugin->cid[15]);

	  specifier = NULL;
      
	  port_index = 0;

	  g_object_get(current_plugin_port,
		       "port-name", &specifier,
		       "port-index", &port_index,
		       NULL);

	  control_port = g_strdup_printf("%u/%u",
					 nth,
					 control_port_count);

	  /* default value */
	  g_value_init(&default_value,
		       G_TYPE_FLOAT);
      
	  g_rec_mutex_lock(plugin_port_mutex);
      
	  g_value_copy(current_plugin_port->default_value,
		       &default_value);
      
	  g_rec_mutex_unlock(plugin_port_mutex);

	  /* vst3 port */
	  vst3_port[nth] = g_object_new(AGS_TYPE_PORT,
					"plugin-name", plugin_name,
					"specifier", specifier,
					"control-port", control_port,
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					NULL);
      
	  if(ags_plugin_port_test_flags(current_plugin_port,
					AGS_PLUGIN_PORT_OUTPUT)){
	    ags_port_set_flags(vst3_port[nth], AGS_PORT_IS_OUTPUT);
	  
	    ags_recall_set_flags((AgsRecall *) fx_vst3_channel,
				 AGS_RECALL_HAS_OUTPUT_PORT);
	
	  }else{
	    if(!ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_INTEGER) &&
	       !ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_TOGGLED)){
	      ags_port_set_flags(vst3_port[nth], AGS_PORT_INFINITE_RANGE);
	    }
	  }
	
	  g_object_set(vst3_port[nth],
		       "plugin-port", current_plugin_port,
		       NULL);

	  ags_port_util_load_vst3_conversion(vst3_port[nth],
					     current_plugin_port);
	
	  ags_port_safe_write_raw(vst3_port[nth],
				  &default_value);

	  ags_recall_add_port((AgsRecall *) fx_vst3_channel,
			      vst3_port[nth]);
	
	  g_signal_connect_after(vst3_port[nth], "safe-write",
				 ags_fx_vst3_channel_safe_write_callback, fx_vst3_audio);
      
	  g_free(plugin_name);
	  g_free(specifier);
	  g_free(control_port);

	  g_value_unset(&default_value);

	  nth++;
	}

	plugin_port = plugin_port->next;
      }
      
      vst3_port[nth] = NULL;
    }

    /* set VST3 output */
    g_rec_mutex_lock(fx_vst3_channel_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxVst3ChannelInputData *input_data;
      
      input_data = fx_vst3_channel->input_data[i];
	      
      if(input_data->output == NULL &&
	 output_port_count > 0 &&
	 buffer_size > 0){
	input_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
      }
	  
      if(input_data->input == NULL &&
	 input_port_count > 0 &&
	 buffer_size > 0){
	input_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
      }
    }
  
    fx_vst3_channel->output_port_count = output_port_count;
    fx_vst3_channel->output_port = output_port;

    fx_vst3_channel->input_port_count = input_port_count;
    fx_vst3_channel->input_port = input_port;
     
    g_rec_mutex_unlock(fx_vst3_channel_mutex);
  }
  
  /* set VST3 port */
  g_rec_mutex_lock(fx_vst3_channel_mutex);

  fx_vst3_channel->vst3_port = vst3_port;
  
  g_rec_mutex_unlock(fx_vst3_channel_mutex);

  /* unref */
  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }

  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

void
ags_fx_vst3_channel_safe_write_callback(AgsPort *port, GValue *value,
					AgsFxVst3Channel *fx_vst3_channel)
{
  AgsPluginPort *plugin_port;
  
  AgsVstParameterInfo *info;

  gint sound_scope;
  guint port_index;
  gfloat param_value;

//  g_message("write %s", port->specifier);
  
  plugin_port = NULL;

  port_index = 0;

  g_object_get(port,
	       "plugin-port", &plugin_port,
	       NULL);

  if(plugin_port != NULL){
    g_object_get(plugin_port,
		 "port-index", &port_index,
		 NULL);
  }
  
  param_value = g_value_get_float(value);

  info = ags_vst_parameter_info_alloc();      

  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
    AgsFxVst3ChannelInputData *input_data;

    AgsVstParamID param_id;

    input_data = fx_vst3_channel->input_data[sound_scope];

    if(input_data != NULL &&
       input_data->iedit_controller != NULL){
      ags_vst_iedit_controller_get_parameter_info(input_data->iedit_controller,
						  port_index, info);

      param_id = ags_vst_parameter_info_get_param_id(info);
	  
      ags_vst_iedit_controller_set_param_normalized(input_data->iedit_controller,
						    param_id, param_value);
    }
  }

  ags_vst_parameter_info_free(info);
}

/**
 * ags_fx_vst3_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxVst3Channel
 *
 * Returns: the new #AgsFxVst3Channel
 *
 * Since: 3.10.5
 */
AgsFxVst3Channel*
ags_fx_vst3_channel_new(AgsChannel *channel)
{
  AgsFxVst3Channel *fx_vst3_channel;

  fx_vst3_channel = (AgsFxVst3Channel *) g_object_new(AGS_TYPE_FX_VST3_CHANNEL,
						      "source", channel,
						      NULL);

  return(fx_vst3_channel);
}
