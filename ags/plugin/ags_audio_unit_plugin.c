/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/plugin/ags_audio_unit_plugin.h>

#include <ags/plugin/ags_audio_unit_manager.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <string.h>
#include <math.h>

#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>

void ags_audio_unit_plugin_class_init(AgsAudioUnitPluginClass *audio_unit_plugin);
void ags_audio_unit_plugin_init (AgsAudioUnitPlugin *audio_unit_plugin);
void ags_audio_unit_plugin_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_plugin_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_plugin_finalize(GObject *gobject);

gpointer ags_audio_unit_plugin_instantiate(AgsBasePlugin *base_plugin,
					   guint samplerate,
					   guint buffer_size);

void ags_audio_unit_plugin_connect_port(AgsBasePlugin *base_plugin,
					gpointer plugin_handle,
					guint port_index,
					gpointer data_location);
void ags_audio_unit_plugin_activate(AgsBasePlugin *base_plugin,
				    gpointer plugin_handle);
void ags_audio_unit_plugin_deactivate(AgsBasePlugin *base_plugin,
				      gpointer plugin_handle);
void ags_audio_unit_plugin_run(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle,
			       snd_seq_event_t *seq_event,
			       guint frame_count);
void ags_audio_unit_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_audio_unit_plugin
 * @short_description: The audio_unit plugin class
 * @title: AgsAudioUnitPlugin
 * @section_id:
 * @include: ags/plugin/ags_audio_unit_plugin.h
 *
 * The #AgsAudioUnitPlugin loads/unloads a AudioUnit plugin.
 */

enum{
  PROP_0,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_audio_unit_plugin_parent_class = NULL;
static guint audio_unit_plugin_signals[LAST_SIGNAL];

GHashTable *ags_audio_unit_plugin_process_data = NULL;

GType
ags_audio_unit_plugin_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_plugin = 0;

    static const GTypeInfo ags_audio_unit_plugin_info = {
      sizeof (AgsAudioUnitPluginClass),
      NULL, /* audio_unit_init */
      NULL, /* audio_unit_finalize */
      (GClassInitFunc) ags_audio_unit_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioUnitPlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_plugin_init,
    };

    ags_type_audio_unit_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
							"AgsAudioUnitPlugin",
							&ags_audio_unit_plugin_info,
							0);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_plugin);
  }

  return(g_define_type_id__static);
}

void
ags_audio_unit_plugin_class_init(AgsAudioUnitPluginClass *audio_unit_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_audio_unit_plugin_parent_class = g_type_class_peek_parent(audio_unit_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_plugin;

  gobject->set_property = ags_audio_unit_plugin_set_property;
  gobject->get_property = ags_audio_unit_plugin_get_property;

  gobject->finalize = ags_audio_unit_plugin_finalize;

  /* properties */

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) audio_unit_plugin;

  base_plugin->instantiate = ags_audio_unit_plugin_instantiate;

  base_plugin->connect_port = ags_audio_unit_plugin_connect_port;

  base_plugin->activate = ags_audio_unit_plugin_activate;
  base_plugin->deactivate = ags_audio_unit_plugin_deactivate;

  base_plugin->run = ags_audio_unit_plugin_run;

  base_plugin->load_plugin = ags_audio_unit_plugin_load_plugin;

  /* AgsAudioUnitPluginClass */
}

void
ags_audio_unit_plugin_init(AgsAudioUnitPlugin *audio_unit_plugin)
{
  audio_unit_plugin->component = NULL;
  audio_unit_plugin->av_audio_unit = NULL;
}

void
ags_audio_unit_plugin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  GRecMutex *base_plugin_mutex;

  audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_plugin_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  GRecMutex *base_plugin_mutex;

  audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(gobject);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(audio_unit_plugin);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_plugin_finalize(GObject *gobject)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  audio_unit_plugin = AGS_AUDIO_UNIT_PLUGIN(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_plugin_parent_class)->finalize(gobject);
}

gpointer
ags_audio_unit_plugin_instantiate(AgsBasePlugin *base_plugin,
				  guint samplerate,
				  guint buffer_size)
{
  AVAudioUnitEffect *av_audio_unit_effect;
  AVAudioUnitMIDIInstrument *av_audio_unit_midi_instrument;
  AVAudioUnit *av_audio_unit;
  
  AudioComponentDescription theDescription;

  gchar *plugin_name;

  NSError *ns_error;
  
  av_audio_unit = NULL;

  memset(&theDescription, 0, sizeof(theDescription));

  AudioComponentGetDescription((AudioComponent) AGS_AUDIO_UNIT_PLUGIN(base_plugin)->component, &theDescription);

  if(!ags_base_plugin_test_flags(base_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    av_audio_unit_effect = [[AVAudioUnitEffect alloc] initWithAudioComponentDescription:theDescription];

    av_audio_unit = (AVAudioUnit *) av_audio_unit_effect;
  }else{
    av_audio_unit_midi_instrument = [[AVAudioUnitMIDIInstrument alloc] initWithAudioComponentDescription:theDescription];

    av_audio_unit = (AVAudioUnit *) av_audio_unit_midi_instrument;
  }

  plugin_name = [[av_audio_unit name] UTF8String];

  /* audio unit */
  if(AGS_AUDIO_UNIT_PLUGIN(base_plugin)->av_audio_unit == NULL){
    AudioComponentDescription component_description;
	
    AGS_AUDIO_UNIT_PLUGIN(base_plugin)->av_audio_unit = (gpointer) av_audio_unit;

    component_description = [[av_audio_unit AUAudioUnit] componentDescription];
    
    ags_base_plugin_load_plugin(base_plugin);
  }
 
  return(av_audio_unit);
}

void
ags_audio_unit_plugin_connect_port(AgsBasePlugin *base_plugin,
				   gpointer plugin_handle,
				   guint port_index,
				   gpointer data_location)
{  
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_activate(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_deactivate(AgsBasePlugin *base_plugin,
				 gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_run(AgsBasePlugin *base_plugin,
			  gpointer plugin_handle,
			  snd_seq_event_t *seq_event,
			  guint frame_count)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsPluginPort *current_plugin_port;

  AVAudioUnit *av_audio_unit;
  AUAudioUnit *au_audio_unit;
  AUAudioUnitBusArray *output_bus_arr, *input_bus_arr;
  AUAudioUnitBus *output_bus, *input_bus;
  AUParameterTree *parameter_tree;
  NSArray<AUParameter *> *parameter_arr;

  GList *start_plugin_port;
  
  gchar *str;

  guint output_count, input_count;
  guint i, i_stop;
  
  GRecMutex *base_plugin_mutex;

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(base_plugin);

  g_rec_mutex_lock(base_plugin_mutex);

  start_plugin_port = base_plugin->plugin_port;
  
  av_audio_unit = (AVAudioUnit *) AGS_AUDIO_UNIT_PLUGIN(base_plugin)->av_audio_unit;

  g_rec_mutex_unlock(base_plugin_mutex);

  if(start_plugin_port != NULL ||
     av_audio_unit == NULL){
    return;
  }

  au_audio_unit = [av_audio_unit AUAudioUnit];

  /* output */
  output_bus_arr = [au_audio_unit outputBusses];

  output_count = 
    i_stop = [output_bus_arr count];

  for(i = 0; i < i_stop; i++){
    output_bus = [output_bus_arr objectAtIndexedSubscript:i];
    
    current_plugin_port = ags_plugin_port_new();
    g_object_ref(current_plugin_port);
	
    start_plugin_port = g_list_prepend(start_plugin_port,
				       current_plugin_port);

    current_plugin_port->port_index = i;

    ags_plugin_port_set_flags(current_plugin_port,
			      (AGS_PLUGIN_PORT_AUDIO |
			       AGS_PLUGIN_PORT_OUTPUT));
  }
  
  /* input */
  input_bus_arr = [au_audio_unit inputBusses];

  input_count = 
    i_stop = [input_bus_arr count];

  for(i = 0; i < i_stop; i++){
    input_bus = [input_bus_arr objectAtIndexedSubscript:i];
    
    current_plugin_port = ags_plugin_port_new();
    g_object_ref(current_plugin_port);
	
    start_plugin_port = g_list_prepend(start_plugin_port,
				       current_plugin_port);

    current_plugin_port->port_index = output_count + i;

    ags_plugin_port_set_flags(current_plugin_port,
			      (AGS_PLUGIN_PORT_AUDIO |
			       AGS_PLUGIN_PORT_INPUT));
  }

  /* parameter */
  parameter_tree = [au_audio_unit parameterTree];

  parameter_arr = [parameter_tree allParameters];

  start_plugin_port = NULL;
  
  i_stop = [parameter_arr count];
  
  for(i = 0; i < i_stop; i++){
    UInt32 flags;
    
    current_plugin_port = ags_plugin_port_new();
    g_object_ref(current_plugin_port);
	
    start_plugin_port = g_list_prepend(start_plugin_port,
				       current_plugin_port);

    current_plugin_port->port_index = output_count + input_count + i;

    str = [[parameter_arr[i] displayName] UTF8String];
    
    current_plugin_port->port_name = g_strdup(str);

    flags = [parameter_arr[i] flags];

    if((kAudioUnitParameterFlag_IsReadable & flags) != 0 &&
       (kAudioUnitParameterFlag_IsWritable & flags) != 0){
      ags_plugin_port_set_flags(current_plugin_port,
				(AGS_PLUGIN_PORT_CONTROL |
				 AGS_PLUGIN_PORT_INPUT));
    }
    
    if((kAudioUnitParameterFlag_IsReadable & flags) != 0 &&
       (kAudioUnitParameterFlag_MeterReadOnly & flags) != 0){
      ags_plugin_port_set_flags(current_plugin_port,
				(AGS_PLUGIN_PORT_CONTROL |
				 AGS_PLUGIN_PORT_OUTPUT));
    }
    
    /* default, lower and upper value */
    g_value_init(current_plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(current_plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(current_plugin_port->upper_value,
		 G_TYPE_FLOAT);
	
    g_value_set_float(current_plugin_port->default_value,
		      0.0);
    
    g_value_set_float(current_plugin_port->default_value,
		      (float) [parameter_arr[i] value]);
    g_value_set_float(current_plugin_port->lower_value,
		      (float) [parameter_arr[i] minValue]);
    g_value_set_float(current_plugin_port->upper_value,
		      (float) [parameter_arr[i] maxValue]);
  }
  
  g_rec_mutex_lock(base_plugin_mutex);

  base_plugin->plugin_port = g_list_reverse(start_plugin_port);

  g_rec_mutex_unlock(base_plugin_mutex);
}

/**
 * ags_audio_unit_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Create a new instance of #AgsAudioUnitPlugin
 *
 * Returns: the new #AgsAudioUnitPlugin
 *
 * Since: 8.1.2
 */
AgsAudioUnitPlugin*
ags_audio_unit_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  audio_unit_plugin = (AgsAudioUnitPlugin *) g_object_new(AGS_TYPE_AUDIO_UNIT_PLUGIN,
							  "filename", filename,
							  "effect", effect,
							  "effect-index", effect_index,
							  NULL);

  return(audio_unit_plugin);
}
