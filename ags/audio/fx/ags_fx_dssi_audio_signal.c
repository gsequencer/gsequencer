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

#include <ags/audio/fx/ags_fx_dssi_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_dssi_audio.h>
#include <ags/audio/fx/ags_fx_dssi_audio_processor.h>
#include <ags/audio/fx/ags_fx_dssi_channel_processor.h>
#include <ags/audio/fx/ags_fx_dssi_recycling.h>

#include <ags/plugin/ags_base_plugin.h>

#include <ags/i18n.h>

void ags_fx_dssi_audio_signal_class_init(AgsFxDssiAudioSignalClass *fx_dssi_audio_signal);
void ags_fx_dssi_audio_signal_init(AgsFxDssiAudioSignal *fx_dssi_audio_signal);
void ags_fx_dssi_audio_signal_dispose(GObject *gobject);
void ags_fx_dssi_audio_signal_finalize(GObject *gobject);

void ags_fx_dssi_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					  AgsAudioSignal *source,
					  AgsNote *note,
					  gboolean pattern_mode,
					  guint x0, guint x1,
					  guint y,
					  gdouble delay_counter, guint64 offset_counter,
					  guint frame_count,
					  gdouble delay, guint buffer_size);
void ags_fx_dssi_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					    AgsAudioSignal *source,
					    AgsNote *note,
					    guint x0, guint x1,
					    guint y);

/**
 * SECTION:ags_fx_dssi_audio_signal
 * @short_description: fx dssi audio signal
 * @title: AgsFxDssiAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_audio_signal.h
 *
 * The #AgsFxDssiAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_audio_signal_parent_class = NULL;

const gchar *ags_fx_dssi_audio_signal_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_audio_signal = 0;

    static const GTypeInfo ags_fx_dssi_audio_signal_info = {
      sizeof (AgsFxDssiAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxDssiAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_audio_signal_init,
    };

    ags_type_fx_dssi_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
							   "AgsFxDssiAudioSignal",
							   &ags_fx_dssi_audio_signal_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_audio_signal_class_init(AgsFxDssiAudioSignalClass *fx_dssi_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_dssi_audio_signal_parent_class = g_type_class_peek_parent(fx_dssi_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_audio_signal;

  gobject->dispose = ags_fx_dssi_audio_signal_dispose;
  gobject->finalize = ags_fx_dssi_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_dssi_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_dssi_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_dssi_audio_signal_notify_remove;
}

void
ags_fx_dssi_audio_signal_init(AgsFxDssiAudioSignal *fx_dssi_audio_signal)
{
  AGS_RECALL(fx_dssi_audio_signal)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_audio_signal)->xml_type = "ags-fx-dssi-audio-signal";
}

void
ags_fx_dssi_audio_signal_dispose(GObject *gobject)
{
  AgsFxDssiAudioSignal *fx_dssi_audio_signal;
  
  fx_dssi_audio_signal = AGS_FX_DSSI_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_audio_signal_finalize(GObject *gobject)
{
  AgsFxDssiAudioSignal *fx_dssi_audio_signal;
  
  fx_dssi_audio_signal = AGS_FX_DSSI_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_dssi_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
				     AgsAudioSignal *source,
				     AgsNote *note,
				     gboolean pattern_mode,
				     guint x0, guint x1,
				     guint y,
				     gdouble delay_counter, guint64 offset_counter,
				     guint frame_count,
				     gdouble delay, guint buffer_size)
{
  AgsAudio *audio;
  AgsFxDssiAudio *fx_dssi_audio;
  AgsFxDssiAudioProcessor *fx_dssi_audio_processor;
  AgsFxDssiChannelProcessor *fx_dssi_channel_processor;
  AgsFxDssiRecycling *fx_dssi_recycling;
  AgsFxDssiAudioSignal *fx_dssi_audio_signal;
  
  AgsDssiPlugin *dssi_plugin;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint format;
  guint copy_mode_out;
  
  void (*run_synth)(LADSPA_Handle Instance,
		    unsigned long SampleCount,
		    snd_seq_event_t *Events,
		    unsigned long EventCount);
  void (*run)(LADSPA_Handle Instance,
	      unsigned long SampleCount);
  
  GRecMutex *source_stream_mutex;
  GRecMutex *fx_dssi_audio_mutex;
  GRecMutex *base_plugin_mutex;

  audio = NULL;
  
  fx_dssi_audio = NULL;
  fx_dssi_audio_processor = NULL;
  
  fx_dssi_channel_processor = NULL;

  fx_dssi_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_signal);

  audio_channel = 0;
  
  audio_start_mapping = 0;
  midi_start_mapping = 0;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_dssi_recycling,
	       NULL);

  g_object_get(fx_dssi_recycling,
	       "parent", &fx_dssi_channel_processor,
	       NULL);
  
  g_object_get(fx_dssi_channel_processor,
	       "recall-audio", &fx_dssi_audio,
	       "recall-audio-run", &fx_dssi_audio_processor,
	       NULL);

  g_object_get(fx_dssi_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_dssi_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(source,
	       "format", &format,
	       NULL);

  /* get DSSI plugin */
  fx_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  g_rec_mutex_lock(fx_dssi_audio_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  
  g_rec_mutex_unlock(fx_dssi_audio_mutex);

  /* process data */
  source_stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

  g_rec_mutex_lock(base_plugin_mutex);

  run_synth = AGS_DSSI_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor)->run_synth;
  run = AGS_DSSI_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor)->LADSPA_Plugin->run;
  
  g_rec_mutex_unlock(base_plugin_mutex);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    midi_note = (128 - y - 1 - audio_start_mapping + midi_start_mapping);
  }else{
    midi_note = (y - audio_start_mapping + midi_start_mapping);
  }

  copy_mode_out = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxDssiAudioScopeData *scope_data;
    AgsFxDssiAudioChannelData *channel_data;
    AgsFxDssiAudioInputData *input_data;

    g_rec_mutex_lock(fx_dssi_audio_mutex);

    scope_data = fx_dssi_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->event_buffer->data.note.note = midi_note;
    
    g_rec_mutex_unlock(fx_dssi_audio_mutex);

    if(delay_counter == 0.0 &&
       x0 == offset_counter){
      g_rec_mutex_lock(fx_dssi_audio_mutex);

      input_data->key_on += 1;

      g_rec_mutex_unlock(fx_dssi_audio_mutex);
    }    
    
    if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      g_rec_mutex_lock(fx_dssi_audio_mutex);
      
      if(channel_data->output != NULL){
	ags_audio_buffer_util_clear_float(channel_data->output, 1,
					  fx_dssi_audio->output_port_count * buffer_size);
      }

      if(run_synth != NULL){
	run_synth(channel_data->ladspa_handle,
		  (unsigned long) (fx_dssi_audio->output_port_count * buffer_size),
		  input_data->event_buffer,
		  1);
      }else if(run != NULL){
	run(channel_data->ladspa_handle,
	    (unsigned long) (fx_dssi_audio->output_port_count * buffer_size));
      }

      g_rec_mutex_lock(source_stream_mutex);

      if(channel_data->output != NULL &&
	 fx_dssi_audio->output_port_count >= 1 &&
	 source->stream_current != NULL){
	//NOTE:JK: only mono input, additional channels discarded
	ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						    channel_data->output, fx_dssi_audio->output_port_count, 0,
						    buffer_size, copy_mode_out);
      }
	  
      g_rec_mutex_unlock(source_stream_mutex);

      g_rec_mutex_unlock(fx_dssi_audio_mutex);
    }else{
      g_rec_mutex_lock(fx_dssi_audio_mutex);
      
      if(input_data->output != NULL){
	ags_audio_buffer_util_clear_float(input_data->output, 1,
					  fx_dssi_audio->output_port_count * buffer_size);
      }

      if(run_synth != NULL){
	run_synth(input_data->ladspa_handle,
		  (unsigned long) (fx_dssi_audio->output_port_count * buffer_size),
		  input_data->event_buffer,
		  1);
      }else if(run != NULL){
	run(input_data->ladspa_handle,
	    (unsigned long) (fx_dssi_audio->output_port_count * buffer_size));
      }
      
      g_rec_mutex_lock(source_stream_mutex);

      if(input_data->output != NULL &&
	 fx_dssi_audio->output_port_count >= 1 &&
	 source->stream_current != NULL){
	//NOTE:JK: only mono input, additional channels discarded
	ags_audio_buffer_util_copy_buffer_to_buffer(source->stream_current->data, 1, 0,
						    input_data->output, fx_dssi_audio->output_port_count, 0,
						    buffer_size, copy_mode_out);
      }
	  
      g_rec_mutex_unlock(source_stream_mutex);

      g_rec_mutex_unlock(fx_dssi_audio_mutex);
    }
  }
  
  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_dssi_audio != NULL){
    g_object_unref(fx_dssi_audio);
  }

  if(fx_dssi_audio_processor != NULL){
    g_object_unref(fx_dssi_audio_processor);
  }
  
  if(fx_dssi_channel_processor != NULL){
    g_object_unref(fx_dssi_channel_processor);
  }
  
  if(fx_dssi_recycling != NULL){
    g_object_unref(fx_dssi_recycling);
  }
}

void
ags_fx_dssi_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
				       AgsAudioSignal *source,
				       AgsNote *note,
				       guint x0, guint x1,
				       guint y)
{
  AgsAudio *audio;
  AgsFxDssiAudio *fx_dssi_audio;
  AgsFxDssiAudioProcessor *fx_dssi_audio_processor;
  AgsFxDssiChannelProcessor *fx_dssi_channel_processor;
  AgsFxDssiRecycling *fx_dssi_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_dssi_audio_mutex;

  audio = NULL;
  
  fx_dssi_audio = NULL;
  fx_dssi_audio_processor = NULL;

  fx_dssi_channel_processor = NULL;

  fx_dssi_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope(fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_dssi_recycling,
	       NULL);

  g_object_get(fx_dssi_recycling,
	       "parent", &fx_dssi_channel_processor,
	       NULL);
  
  g_object_get(fx_dssi_channel_processor,
	       "recall-audio", &fx_dssi_audio,
	       "recall-audio-run", &fx_dssi_audio_processor,
	       NULL);

  g_object_get(fx_dssi_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_dssi_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxDssiAudioScopeData *scope_data;
    AgsFxDssiAudioChannelData *channel_data;
    AgsFxDssiAudioInputData *input_data;

    g_rec_mutex_lock(fx_dssi_audio_mutex);
      
    scope_data = fx_dssi_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_dssi_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_dssi_audio != NULL){
    g_object_unref(fx_dssi_audio);
  }

  if(fx_dssi_audio_processor != NULL){
    g_object_unref(fx_dssi_audio_processor);
  }
  
  if(fx_dssi_channel_processor != NULL){
    g_object_unref(fx_dssi_channel_processor);
  }
  
  if(fx_dssi_recycling != NULL){
    g_object_unref(fx_dssi_recycling);
  }
}

/**
 * ags_fx_dssi_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxDssiAudioSignal
 *
 * Returns: the new #AgsFxDssiAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxDssiAudioSignal*
ags_fx_dssi_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxDssiAudioSignal *fx_dssi_audio_signal;

  fx_dssi_audio_signal = (AgsFxDssiAudioSignal *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO_SIGNAL,
							       "source", audio_signal,
							       NULL);

  return(fx_dssi_audio_signal);
}
