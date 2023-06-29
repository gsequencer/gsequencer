/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_ladspa_channel.h>

#include <ags/plugin/ags_ladspa_manager.h>
#include <ags/plugin/ags_ladspa_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port_util.h>

#include <ags/i18n.h>

void ags_fx_ladspa_channel_class_init(AgsFxLadspaChannelClass *fx_ladspa_channel);
void ags_fx_ladspa_channel_init(AgsFxLadspaChannel *fx_ladspa_channel);
void ags_fx_ladspa_channel_dispose(GObject *gobject);
void ags_fx_ladspa_channel_finalize(GObject *gobject);

void ags_fx_ladspa_channel_notify_filename_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data);
void ags_fx_ladspa_channel_notify_effect_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);
void ags_fx_ladspa_channel_notify_buffer_size_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data);
void ags_fx_ladspa_channel_notify_samplerate_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data);

/**
 * SECTION:ags_fx_ladspa_channel
 * @short_description: fx ladspa channel
 * @title: AgsFxLadspaChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_ladspa_channel.h
 *
 * The #AgsFxLadspaChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_ladspa_channel_parent_class = NULL;

const gchar *ags_fx_ladspa_channel_plugin_name = "ags-fx-ladspa";

GType
ags_fx_ladspa_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_ladspa_channel = 0;

    static const GTypeInfo ags_fx_ladspa_channel_info = {
      sizeof (AgsFxLadspaChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_ladspa_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxLadspaChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_ladspa_channel_init,
    };

    ags_type_fx_ladspa_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							"AgsFxLadspaChannel",
							&ags_fx_ladspa_channel_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_ladspa_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_ladspa_channel_class_init(AgsFxLadspaChannelClass *fx_ladspa_channel)
{
  GObjectClass *gobject;

  ags_fx_ladspa_channel_parent_class = g_type_class_peek_parent(fx_ladspa_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_ladspa_channel;

  gobject->dispose = ags_fx_ladspa_channel_dispose;
  gobject->finalize = ags_fx_ladspa_channel_finalize;
}

void
ags_fx_ladspa_channel_init(AgsFxLadspaChannel *fx_ladspa_channel)
{
  guint i;

  g_signal_connect(fx_ladspa_channel, "notify::filename",
		   G_CALLBACK(ags_fx_ladspa_channel_notify_filename_callback), NULL);

  g_signal_connect(fx_ladspa_channel, "notify::effect",
		   G_CALLBACK(ags_fx_ladspa_channel_notify_effect_callback), NULL);

  g_signal_connect(fx_ladspa_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_ladspa_channel_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_ladspa_channel, "notify::samplerate",
		   G_CALLBACK(ags_fx_ladspa_channel_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_ladspa_channel)->name = "ags-fx-ladspa";
  AGS_RECALL(fx_ladspa_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_ladspa_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_ladspa_channel)->xml_type = "ags-fx-ladspa-channel";

  fx_ladspa_channel->output_port_count = 0;
  fx_ladspa_channel->output_port = NULL;
  
  fx_ladspa_channel->input_port_count = 0;
  fx_ladspa_channel->input_port = NULL;

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_ladspa_channel->input_data[i] = ags_fx_ladspa_channel_input_data_alloc();
      
    fx_ladspa_channel->input_data[i]->parent = fx_ladspa_channel;
  }
  
  fx_ladspa_channel->ladspa_plugin = NULL;

  fx_ladspa_channel->ladspa_port = NULL;
}

void
ags_fx_ladspa_channel_dispose(GObject *gobject)
{
  AgsFxLadspaChannel *fx_ladspa_channel;
  
  fx_ladspa_channel = AGS_FX_LADSPA_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_ladspa_channel_parent_class)->dispose(gobject);
}

void
ags_fx_ladspa_channel_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxLadspaChannel *fx_ladspa_channel;
  
  guint i;
  
  fx_ladspa_channel = AGS_FX_LADSPA_CHANNEL(gobject);

  g_free(fx_ladspa_channel->output_port);
  g_free(fx_ladspa_channel->input_port);
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_ladspa_channel_input_data_free(fx_ladspa_channel->input_data[i]);
  }
  
  if(fx_ladspa_channel->ladspa_plugin != NULL){
    g_object_unref(fx_ladspa_channel->ladspa_plugin);
  }
  
  if(fx_ladspa_channel->ladspa_port != NULL){
    for(iter = fx_ladspa_channel->ladspa_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_ladspa_channel->ladspa_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_ladspa_channel_parent_class)->finalize(gobject);
}

void
ags_fx_ladspa_channel_notify_filename_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsFxLadspaChannel *fx_ladspa_channel;

  AgsLadspaManager *ladspa_manager;

  gchar *filename, *effect;
  
  GRecMutex *recall_mutex;

  fx_ladspa_channel = AGS_FX_LADSPA_CHANNEL(gobject);

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  filename = NULL;
  effect = NULL;

  g_object_get(fx_ladspa_channel,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);

  /* get ladspa plugin */
  g_rec_mutex_lock(recall_mutex);

  if(filename != NULL &&
     effect != NULL){
    fx_ladspa_channel->ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
									     filename, effect);
  }
  
  g_rec_mutex_unlock(recall_mutex);

  g_free(filename);
  g_free(effect);
}

void
ags_fx_ladspa_channel_notify_effect_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsFxLadspaChannel *fx_ladspa_channel;

  AgsLadspaManager *ladspa_manager;

  gchar *filename, *effect;

  GRecMutex *recall_mutex;
  
  fx_ladspa_channel = AGS_FX_LADSPA_CHANNEL(gobject);

  ladspa_manager = ags_ladspa_manager_get_instance();

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  filename = NULL;
  effect = NULL;

  g_object_get(fx_ladspa_channel,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);

  /* get ladspa plugin */
  g_rec_mutex_lock(recall_mutex);

  if(filename != NULL &&
     effect != NULL){
    fx_ladspa_channel->ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
									     filename, effect);
  }
  
  g_rec_mutex_unlock(recall_mutex);

  g_free(filename);
  g_free(effect);
}

void
ags_fx_ladspa_channel_notify_buffer_size_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data)
{
  AgsFxLadspaChannel *fx_ladspa_channel;

  guint output_port_count, input_port_count;
  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_ladspa_channel = AGS_FX_LADSPA_CHANNEL(gobject);

  if(!ags_recall_test_state_flags(AGS_RECALL(fx_ladspa_channel),
				  AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  /* get buffer size */
  g_object_get(fx_ladspa_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_ladspa_channel->output_port_count;
  input_port_count = fx_ladspa_channel->input_port_count;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLadspaChannelInputData *input_data;

    input_data = fx_ladspa_channel->input_data[i];

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
ags_fx_ladspa_channel_notify_samplerate_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsFxLadspaChannel *fx_ladspa_channel;

  AgsLadspaPlugin *ladspa_plugin;

  guint output_port_count;
  guint buffer_size;
  guint samplerate;
  guint i;
  
  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  void (*deactivate)(LADSPA_Handle instance);
  void (*cleanup)(LADSPA_Handle instance);
  
  fx_ladspa_channel = AGS_FX_LADSPA_CHANNEL(gobject);

  if(!ags_recall_test_state_flags(AGS_RECALL(fx_ladspa_channel),
				  AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  /* get LADSPA plugin */
  g_rec_mutex_lock(recall_mutex);

  ladspa_plugin = fx_ladspa_channel->ladspa_plugin;

  g_rec_mutex_unlock(recall_mutex);

  if(ladspa_plugin == NULL){
    return;
  }
  
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_ladspa_channel,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* get deactivate and cleanup */
  base_plugin_mutex = NULL;

  deactivate = NULL;
  cleanup = NULL;
  
  if(ladspa_plugin != NULL){
    gpointer plugin_descriptor;
    
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin);
    
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(ladspa_plugin)->plugin_descriptor;

    deactivate = AGS_LADSPA_PLUGIN_DESCRIPTOR(plugin_descriptor)->deactivate;
    cleanup = AGS_LADSPA_PLUGIN_DESCRIPTOR(plugin_descriptor)->cleanup;

    g_rec_mutex_unlock(base_plugin_mutex);
  }

  /* reallocate buffer - apply samplerate */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLadspaChannelInputData *input_data;

    input_data = fx_ladspa_channel->input_data[i];

    if(deactivate != NULL &&
       input_data->ladspa_handle != NULL){
      deactivate(input_data->ladspa_handle);
    }

    if(cleanup != NULL &&
       input_data->ladspa_handle != NULL){
      cleanup(input_data->ladspa_handle);
    }

    input_data->ladspa_handle = ags_base_plugin_instantiate((AgsBasePlugin *) ladspa_plugin,
							    samplerate, buffer_size);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_ladspa_channel_input_data_alloc:
 * 
 * Allocate #AgsFxLadspaChannelInputData-struct
 * 
 * Returns: the new #AgsFxLadspaChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxLadspaChannelInputData*
ags_fx_ladspa_channel_input_data_alloc()
{
  AgsFxLadspaChannelInputData *input_data;

  input_data = (AgsFxLadspaChannelInputData *) g_malloc(sizeof(AgsFxLadspaChannelInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->ladspa_handle = NULL;

  return(input_data);
}

/**
 * ags_fx_ladspa_channel_input_data_free:
 * @input_data: the #AgsFxLadspaChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_ladspa_channel_input_data_free(AgsFxLadspaChannelInputData *input_data)
{
  AgsLadspaPlugin *ladspa_plugin;
  
  if(input_data == NULL){
    return;
  }

  g_free(input_data->output);
  g_free(input_data->input);

  if(input_data->ladspa_handle != NULL){
    gpointer plugin_descriptor;

    GRecMutex *base_plugin_mutex;
    
    void (*deactivate)(LADSPA_Handle Instance);
    void (*cleanup)(LADSPA_Handle Instance);
  
    ladspa_plugin = NULL;
    
    deactivate = NULL;
    cleanup = NULL;

    base_plugin_mutex = NULL;
    
    if(input_data->parent != NULL){
      ladspa_plugin = AGS_FX_LADSPA_CHANNEL(input_data->parent)->ladspa_plugin;
    }
    
    if(ladspa_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin);
      
      g_rec_mutex_lock(base_plugin_mutex);
  
      plugin_descriptor = AGS_BASE_PLUGIN(ladspa_plugin)->plugin_descriptor;

      deactivate = AGS_LADSPA_PLUGIN_DESCRIPTOR(plugin_descriptor)->deactivate;
      cleanup = AGS_LADSPA_PLUGIN_DESCRIPTOR(plugin_descriptor)->cleanup;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    }

    if(deactivate != NULL &&
       input_data->ladspa_handle != NULL){
      deactivate(input_data->ladspa_handle);
    }

    if(cleanup != NULL &&
       input_data->ladspa_handle != NULL){
      cleanup(input_data->ladspa_handle);
    }
  }
  
  g_free(input_data);
}

/**
 * ags_fx_ladspa_channel_load_plugin:
 * @fx_ladspa_channel: the #AgsFxLadspaChannel
 * 
 * Load plugin of @fx_ladspa_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_ladspa_channel_load_plugin(AgsFxLadspaChannel *fx_ladspa_channel)
{
  AgsLadspaManager *ladspa_manager;
  AgsLadspaPlugin *ladspa_plugin;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  guint i;
    
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_LADSPA_CHANNEL(fx_ladspa_channel) ||
     ags_recall_test_state_flags(AGS_RECALL(fx_ladspa_channel),
				 AGS_SOUND_STATE_PLUGIN_LOADED)){
    return;
  }

  ladspa_manager = ags_ladspa_manager_get_instance();
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  /* get filename and effect */
  filename = NULL;
  effect = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_ladspa_channel,
	       "filename", &filename,
	       "effect", &effect,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  ladspa_plugin = fx_ladspa_channel->ladspa_plugin;

  if(ladspa_plugin == NULL){
    ladspa_plugin =
      fx_ladspa_channel->ladspa_plugin = ags_ladspa_manager_find_ladspa_plugin(ladspa_manager,
									       filename, effect);

    if(ladspa_plugin != NULL){
      g_object_ref(ladspa_plugin);
    }
  }    
  
  g_rec_mutex_unlock(recall_mutex);

  if(ladspa_plugin == NULL ||
     ags_base_plugin_test_flags((AgsBasePlugin *) ladspa_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    g_free(filename);
    g_free(effect);
    
    return;
  }
    
  /* set ladspa plugin */    
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLadspaChannelInputData *input_data;

    input_data = fx_ladspa_channel->input_data[i];
      
    if(input_data->ladspa_handle == NULL){
      input_data->ladspa_handle = ags_base_plugin_instantiate((AgsBasePlugin *) ladspa_plugin,
							      samplerate, buffer_size);
    }
  }
    
  g_rec_mutex_unlock(recall_mutex);

  ags_recall_set_state_flags(AGS_RECALL(fx_ladspa_channel),
			     AGS_SOUND_STATE_PLUGIN_LOADED);
  
  g_free(filename);
  g_free(effect);
}

/**
 * ags_fx_ladspa_channel_load_port:
 * @fx_ladspa_channel: the #AgsFxLadspaChannel
 * 
 * Load port of @fx_ladspa_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_ladspa_channel_load_port(AgsFxLadspaChannel *fx_ladspa_channel)
{
  AgsChannel *input;
  AgsPort **ladspa_port;
  
  AgsLadspaPlugin *ladspa_plugin;

  GType channel_type;

  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;
  
  guint audio_channel;
  guint pad;
  guint line;
  guint output_port_count, input_port_count;
  guint control_port_count;
  gboolean has_midiin_event_port;
  guint midiin_event_port;  
  gboolean has_midiin_atom_port;
  guint midiin_atom_port;
  guint samplerate;
  guint buffer_size;
  guint nth;
  guint i, j;
  
  GRecMutex *fx_ladspa_channel_mutex;

  if(!AGS_IS_FX_LADSPA_CHANNEL(fx_ladspa_channel) ||
     ags_recall_test_state_flags(AGS_RECALL(fx_ladspa_channel),
				 AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }

  fx_ladspa_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  g_rec_mutex_lock(fx_ladspa_channel_mutex);

  if(fx_ladspa_channel->ladspa_port != NULL){
    g_rec_mutex_unlock(fx_ladspa_channel_mutex);
    
    return;
  }

  g_rec_mutex_unlock(fx_ladspa_channel_mutex);
  
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_ladspa_channel,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* get recall mutex */
  input = NULL;

  g_object_get(fx_ladspa_channel,
	       "source", &input,
	       NULL);

  line = ags_channel_get_line(input);

  channel_type = (AGS_IS_OUTPUT(input)) ? AGS_TYPE_OUTPUT: AGS_TYPE_INPUT;

  audio_channel = 0;
  pad = 0;

  g_object_get(input,
	       "audio-channel", &audio_channel,
	       "pad", &pad,
	       NULL);
  
  /* get LADSPA plugin */
  g_rec_mutex_lock(fx_ladspa_channel_mutex);

  ladspa_plugin = fx_ladspa_channel->ladspa_plugin;
  
  output_port_count = fx_ladspa_channel->output_port_count;
  output_port = fx_ladspa_channel->output_port;

  input_port_count = fx_ladspa_channel->input_port_count;
  input_port = fx_ladspa_channel->input_port;

  g_rec_mutex_unlock(fx_ladspa_channel_mutex);

  /* get LADSPA port */
  g_rec_mutex_lock(fx_ladspa_channel_mutex);

  ladspa_port = fx_ladspa_channel->ladspa_port;

  g_rec_mutex_unlock(fx_ladspa_channel_mutex);

  start_plugin_port = NULL;
  
  g_object_get(ladspa_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);
  
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
    }else if(ags_plugin_port_test_flags(plugin_port->data,
					AGS_PLUGIN_PORT_AUDIO)){
      guint port_index;

      g_object_get(plugin_port->data,
		   "port-index", &port_index,
		   NULL);
      
      if(ags_plugin_port_test_flags(plugin_port->data,
				    AGS_PLUGIN_PORT_INPUT)){
	if(input_port == NULL){
	  input_port = (guint *) g_malloc(sizeof(guint));	  
	}else{
	  input_port = (guint *) g_realloc(input_port,
					   (input_port_count + 1) * sizeof(guint));
	}

	input_port[input_port_count] = port_index;
	input_port_count++;
      }else if(ags_plugin_port_test_flags(plugin_port->data,
					  AGS_PLUGIN_PORT_OUTPUT)){
	if(output_port == NULL){
	  output_port = (guint *) g_malloc(sizeof(guint));	  
	}else{
	  output_port = (guint *) g_realloc(output_port,
					    (output_port_count + 1) * sizeof(guint));
	}

	output_port[output_port_count] = port_index;
	output_port_count++;
      }
    }

    plugin_port = plugin_port->next;
  }

  /*  */
  if(control_port_count > 0){
    ladspa_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

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
	plugin_name = g_strdup_printf("ladspa-%u", ladspa_plugin->unique_id);

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

	/* ladspa port */
	ladspa_port[nth] = g_object_new(AGS_TYPE_PORT,
					"line", line,
					"channel-type", channel_type,
					"plugin-name", plugin_name,
					"specifier", specifier,
					"control-port", control_port,
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					NULL);
      
	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_OUTPUT)){
	  ags_port_set_flags(ladspa_port[nth], AGS_PORT_IS_OUTPUT);
	  
	  ags_recall_set_flags((AgsRecall *) fx_ladspa_channel,
			       AGS_RECALL_HAS_OUTPUT_PORT);
	
	}else{
	  if(!ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_TOGGLED)){
	    ags_port_set_flags(ladspa_port[nth], AGS_PORT_INFINITE_RANGE);
	  }
	}
	
	g_object_set(ladspa_port[nth],
		     "plugin-port", current_plugin_port,
		     NULL);

	ags_port_util_load_ladspa_conversion(ladspa_port[nth],
					     current_plugin_port);
	
	ags_port_safe_write_raw(ladspa_port[nth],
				&default_value);

	ags_recall_add_port((AgsRecall *) fx_ladspa_channel,
			    ladspa_port[nth]);

	/* connect port */
	for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	  AgsFxLadspaChannelInputData *input_data;

	  input_data = fx_ladspa_channel->input_data[i];

	  if(input_data->ladspa_handle == NULL){
	    input_data->ladspa_handle = ags_base_plugin_instantiate((AgsBasePlugin *) ladspa_plugin,
								    samplerate, buffer_size);
	  }

	  ags_base_plugin_connect_port((AgsBasePlugin *) ladspa_plugin,
				       input_data->ladspa_handle,
				       port_index,
				       &(ladspa_port[nth]->port_value.ags_port_float));
	}
      
	g_free(plugin_name);
	g_free(specifier);
	g_free(control_port);

	g_value_unset(&default_value);

	nth++;
      }

      plugin_port = plugin_port->next;
    }
      
    ladspa_port[nth] = NULL;
  }

  /* set LADSPA output */
  g_rec_mutex_lock(fx_ladspa_channel_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLadspaChannelInputData *input_data;
      
    input_data = fx_ladspa_channel->input_data[i];
	      
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

    for(nth = 0; nth < output_port_count; nth++){
      ags_base_plugin_connect_port((AgsBasePlugin *) ladspa_plugin,
				   input_data->ladspa_handle,
				   output_port[nth],
				   &(input_data->output[nth]));
    }

    for(nth = 0; nth < input_port_count; nth++){
      ags_base_plugin_connect_port((AgsBasePlugin *) ladspa_plugin,
				   input_data->ladspa_handle,
				   input_port[nth],
				   &(input_data->input[nth]));
    }
      
    ags_base_plugin_activate((AgsBasePlugin *) ladspa_plugin,
			     input_data->ladspa_handle);
  }
  
  fx_ladspa_channel->output_port_count = output_port_count;
  fx_ladspa_channel->output_port = output_port;

  fx_ladspa_channel->input_port_count = input_port_count;
  fx_ladspa_channel->input_port = input_port;
     
  g_rec_mutex_unlock(fx_ladspa_channel_mutex);
  
  /* set LADSPA port */
  g_rec_mutex_lock(fx_ladspa_channel_mutex);

  fx_ladspa_channel->ladspa_port = ladspa_port;
  
  g_rec_mutex_unlock(fx_ladspa_channel_mutex);

  ags_recall_set_state_flags(AGS_RECALL(fx_ladspa_channel),
			     AGS_SOUND_STATE_PORT_LOADED);

  /* unref */
  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_ladspa_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxLadspaChannel
 *
 * Returns: the new #AgsFxLadspaChannel
 *
 * Since: 3.3.0
 */
AgsFxLadspaChannel*
ags_fx_ladspa_channel_new(AgsChannel *channel)
{
  AgsFxLadspaChannel *fx_ladspa_channel;

  fx_ladspa_channel = (AgsFxLadspaChannel *) g_object_new(AGS_TYPE_FX_LADSPA_CHANNEL,
							  "source", channel,
							  NULL);

  return(fx_ladspa_channel);
}
