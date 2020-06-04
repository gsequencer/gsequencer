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

#include <ags/audio/fx/ags_fx_ladspa_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_ladspa_channel.h>
#include <ags/audio/fx/ags_fx_ladspa_channel_processor.h>
#include <ags/audio/fx/ags_fx_ladspa_recycling.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/i18n.h>

void ags_fx_ladspa_audio_signal_class_init(AgsFxLadspaAudioSignalClass *fx_ladspa_audio_signal);
void ags_fx_ladspa_audio_signal_init(AgsFxLadspaAudioSignal *fx_ladspa_audio_signal);
void ags_fx_ladspa_audio_signal_dispose(GObject *gobject);
void ags_fx_ladspa_audio_signal_finalize(GObject *gobject);

void ags_fx_ladspa_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_ladspa_audio_signal
 * @short_description: fx ladspa audio signal
 * @title: AgsFxLadspaAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_ladspa_audio_signal.h
 *
 * The #AgsFxLadspaAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_ladspa_audio_signal_parent_class = NULL;

const gchar *ags_fx_ladspa_audio_signal_plugin_name = "ags-fx-ladspa";

GType
ags_fx_ladspa_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_ladspa_audio_signal = 0;

    static const GTypeInfo ags_fx_ladspa_audio_signal_info = {
      sizeof (AgsFxLadspaAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_ladspa_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxLadspaAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_ladspa_audio_signal_init,
    };

    ags_type_fx_ladspa_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							     "AgsFxLadspaAudioSignal",
							     &ags_fx_ladspa_audio_signal_info,
							     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_ladspa_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_ladspa_audio_signal_class_init(AgsFxLadspaAudioSignalClass *fx_ladspa_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_ladspa_audio_signal_parent_class = g_type_class_peek_parent(fx_ladspa_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_ladspa_audio_signal;

  gobject->dispose = ags_fx_ladspa_audio_signal_dispose;
  gobject->finalize = ags_fx_ladspa_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_ladspa_audio_signal;
  
  recall->run_inter = ags_fx_ladspa_audio_signal_real_run_inter;
}

void
ags_fx_ladspa_audio_signal_init(AgsFxLadspaAudioSignal *fx_ladspa_audio_signal)
{
  AGS_RECALL(fx_ladspa_audio_signal)->name = "ags-fx-ladspa";
  AGS_RECALL(fx_ladspa_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_ladspa_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_ladspa_audio_signal)->xml_type = "ags-fx-ladspa-audio-signal";
}

void
ags_fx_ladspa_audio_signal_dispose(GObject *gobject)
{
  AgsFxLadspaAudioSignal *fx_ladspa_audio_signal;
  
  fx_ladspa_audio_signal = AGS_FX_LADSPA_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_ladspa_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_ladspa_audio_signal_finalize(GObject *gobject)
{
  AgsFxLadspaAudioSignal *fx_ladspa_audio_signal;
  
  fx_ladspa_audio_signal = AGS_FX_LADSPA_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_ladspa_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_ladspa_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxLadspaChannel *fx_ladspa_channel;
  AgsFxLadspaChannelProcessor *fx_ladspa_channel_processor;
  AgsFxLadspaRecycling *fx_ladspa_recycling;
  AgsLadspaPlugin *ladspa_plugin;

  guint sound_scope;
  guint buffer_size;
  guint format;
  guint copy_mode_out, copy_mode_in;
  
  void (*run)(LADSPA_Handle Instance,
	      unsigned long SampleCount);

  GRecMutex *source_stream_mutex;
  GRecMutex *fx_ladspa_channel_mutex;
  GRecMutex *base_plugin_mutex;

  fx_ladspa_channel = NULL;
  fx_ladspa_channel_processor = NULL;

  fx_ladspa_recycling = NULL;

  source = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "parent", &fx_ladspa_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_ladspa_recycling,
	       "parent", &fx_ladspa_channel_processor,
	       NULL);

  g_object_get(fx_ladspa_channel_processor,
	       "recall-channel", &fx_ladspa_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* get LADSPA plugin */
  fx_ladspa_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_ladspa_channel);

  g_rec_mutex_lock(fx_ladspa_channel_mutex);

  ladspa_plugin = fx_ladspa_channel->ladspa_plugin;
  
  g_rec_mutex_unlock(fx_ladspa_channel_mutex);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(ladspa_plugin);

  g_rec_mutex_lock(base_plugin_mutex);

  run = AGS_LADSPA_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(ladspa_plugin)->plugin_descriptor)->run;
  
  g_rec_mutex_unlock(base_plugin_mutex);

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);
  
  copy_mode_in = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						     ags_audio_buffer_util_format_from_soundcard(format));

  if(run != NULL){
    AgsFxLadspaChannelInputData *input_data;

    g_rec_mutex_lock(fx_ladspa_channel_mutex);
    
    input_data = fx_ladspa_channel->input_data[sound_scope];

    if(input_data->output != NULL){
      ags_audio_buffer_util_clear_float(input_data->output, fx_ladspa_channel->output_port_count,
					buffer_size);
    }

    if(input_data->input != NULL){
      ags_audio_buffer_util_clear_float(input_data->input, fx_ladspa_channel->input_port_count,
					buffer_size);
    }

    g_rec_mutex_lock(source_stream_mutex);
    
    if(input_data->output != NULL &&
       fx_ladspa_channel->output_port_count >= 1 &&
       source->stream_current != NULL){
      ags_audio_buffer_util_copy_buffer_to_buffer(input_data->input, fx_ladspa_channel->input_port_count, 0,
						  source->stream_current->data, 1, 0,
						  buffer_size, copy_mode_out);
    }

    run(input_data->ladspa_handle,
	(unsigned long) (fx_ladspa_channel->output_port_count * buffer_size));

    if(input_data->output != NULL &&
       fx_ladspa_channel->output_port_count >= 1 &&
       source->stream_current != NULL){
      //NOTE:JK: only mono input, additional channels discarded
      ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						  input_data->output, fx_ladspa_channel->output_port_count, 0,
						  buffer_size, copy_mode_out);
    }
    
    g_rec_mutex_unlock(source_stream_mutex);
    
    g_rec_mutex_unlock(fx_ladspa_channel_mutex);
  }

  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }
  
  /* unref */
  if(source != NULL){
    g_object_unref(source);
  }
  
  if(fx_ladspa_channel != NULL){
    g_object_unref(fx_ladspa_channel);
  }
  
  if(fx_ladspa_channel_processor != NULL){
    g_object_unref(fx_ladspa_channel_processor);
  }
  
  if(fx_ladspa_recycling != NULL){
    g_object_unref(fx_ladspa_recycling);
  }

  /* call parent */
  AGS_RECALL_CLASS(ags_fx_ladspa_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_ladspa_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxLadspaAudioSignal
 *
 * Returns: the new #AgsFxLadspaAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxLadspaAudioSignal*
ags_fx_ladspa_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxLadspaAudioSignal *fx_ladspa_audio_signal;

  fx_ladspa_audio_signal = (AgsFxLadspaAudioSignal *) g_object_new(AGS_TYPE_FX_LADSPA_AUDIO_SIGNAL,
								   "source", audio_signal,
								   NULL);

  return(fx_ladspa_audio_signal);
}
