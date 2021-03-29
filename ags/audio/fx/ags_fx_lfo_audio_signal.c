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

#include <ags/audio/fx/ags_fx_lfo_audio_signal.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_enums.h>

#include <ags/audio/fx/ags_fx_lfo_audio.h>
#include <ags/audio/fx/ags_fx_lfo_channel.h>
#include <ags/audio/fx/ags_fx_lfo_channel_processor.h>
#include <ags/audio/fx/ags_fx_lfo_recycling.h>

#include <ags/i18n.h>

void ags_fx_lfo_audio_signal_class_init(AgsFxLfoAudioSignalClass *fx_lfo_audio_signal);
void ags_fx_lfo_audio_signal_init(AgsFxLfoAudioSignal *fx_lfo_audio_signal);
void ags_fx_lfo_audio_signal_dispose(GObject *gobject);
void ags_fx_lfo_audio_signal_finalize(GObject *gobject);

void ags_fx_lfo_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_lfo_audio_signal
 * @short_description: fx lfo audio signal
 * @title: AgsFxLfoAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lfo_audio_signal.h
 *
 * The #AgsFxLfoAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_lfo_audio_signal_parent_class = NULL;

const gchar *ags_fx_lfo_audio_signal_plugin_name = "ags-fx-lfo";

GType
ags_fx_lfo_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lfo_audio_signal = 0;

    static const GTypeInfo ags_fx_lfo_audio_signal_info = {
      sizeof (AgsFxLfoAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lfo_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxLfoAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lfo_audio_signal_init,
    };

    ags_type_fx_lfo_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsFxLfoAudioSignal",
							  &ags_fx_lfo_audio_signal_info,
							  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lfo_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lfo_audio_signal_class_init(AgsFxLfoAudioSignalClass *fx_lfo_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_lfo_audio_signal_parent_class = g_type_class_peek_parent(fx_lfo_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lfo_audio_signal;

  gobject->dispose = ags_fx_lfo_audio_signal_dispose;
  gobject->finalize = ags_fx_lfo_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_lfo_audio_signal;
  
  recall->run_inter = ags_fx_lfo_audio_signal_real_run_inter;
}

void
ags_fx_lfo_audio_signal_init(AgsFxLfoAudioSignal *fx_lfo_audio_signal)
{
  AGS_RECALL(fx_lfo_audio_signal)->name = "ags-fx-lfo";
  AGS_RECALL(fx_lfo_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lfo_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lfo_audio_signal)->xml_type = "ags-fx-lfo-audio-signal";
}

void
ags_fx_lfo_audio_signal_dispose(GObject *gobject)
{
  AgsFxLfoAudioSignal *fx_lfo_audio_signal;
  
  fx_lfo_audio_signal = AGS_FX_LFO_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_lfo_audio_signal_finalize(GObject *gobject)
{
  AgsFxLfoAudioSignal *fx_lfo_audio_signal;

  fx_lfo_audio_signal = AGS_FX_LFO_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lfo_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_lfo_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsFxLfoChannel *fx_lfo_channel;
  AgsFxLfoChannelProcessor *fx_lfo_channel_processor;
  AgsFxLfoRecycling *fx_lfo_recycling;
  AgsAudioSignal *source;
  AgsPort *port;
  
  GObject *output_soundcard;

  GList *start_note, *note;

  guint buffer_size;
  guint samplerate;
  guint format;
  gboolean enabled;
  guint lfo_wave;
  gdouble lfo_freq;
  gdouble lfo_phase;
  gdouble lfo_depth;
  gdouble lfo_tuning;
  gdouble delay;
  guint note_offset, delay_counter;
  guint current_frame;
  
  GRecMutex *stream_mutex;

  output_soundcard = NULL;

  fx_lfo_channel = NULL;

  fx_lfo_channel_processor = NULL;

  fx_lfo_recycling = NULL;

  start_note = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  stream_mutex = NULL;

  g_object_get(recall,
	       "parent", &fx_lfo_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_lfo_recycling,
	       "parent", &fx_lfo_channel_processor,
	       NULL);

  g_object_get(fx_lfo_channel_processor,
	       "recall-channel", &fx_lfo_channel,
	       NULL);

  g_object_get(source,
	       "output-soundcard", &output_soundcard,
	       "note", &start_note,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       "format", &format,
	       NULL);

  enabled = FALSE;

  lfo_wave = 0;
  lfo_freq = 6.0;
  lfo_phase = 0.0;
  lfo_depth = 1.0;
  lfo_tuning = 0.0;
  
  current_frame = 0;
  
  port = NULL;
  g_object_get(fx_lfo_channel,
	       "enabled", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    enabled = (g_value_get_float(&value) != 0.0) ? TRUE: FALSE;

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_lfo_channel,
	       "lfo-wave", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    lfo_wave = (guint) g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_lfo_channel,
	       "lfo-freq", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    lfo_freq = g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_lfo_channel,
	       "lfo-phase", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    lfo_phase = g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_lfo_channel,
	       "lfo-depth", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    lfo_depth = g_value_get_float(&value);

    g_object_unref(port);
  }

  port = NULL;
  g_object_get(fx_lfo_channel,
	       "lfo-tuning", &port,
	       NULL);

  if(port != NULL){
    GValue value = G_VALUE_INIT;

    g_value_init(&value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(port,
		       &value);

    lfo_tuning = g_value_get_float(&value);

    g_object_unref(port);
  }

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
  
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
  
  if(enabled &&
     source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

    if(start_note != NULL){
      note = start_note;
    
      while(note != NULL){
	guint x0, x1;

	g_object_get(note->data,
		     "x0", &x0,
		     "x1", &x1,
		     NULL);

	if(note_offset >= x0){
	  current_frame = (guint) floor(((gdouble) (note_offset - x0) * delay + delay_counter) * buffer_size);
	}else{
	  current_frame = 0;
	}
	
	g_rec_mutex_lock(stream_mutex);

	switch(lfo_wave){
	case AGS_SYNTH_OSCILLATOR_SIN:
	{
	  ags_lfo_synth_util_sin(source->stream_current->data,
				 lfo_freq, lfo_phase,
				 lfo_depth,
				 lfo_tuning,
				 samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				 current_frame, buffer_size);
	}
	break;
	case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
	{
	  ags_lfo_synth_util_sawtooth(source->stream_current->data,
				      lfo_freq, lfo_phase,
				      lfo_depth,
				      lfo_tuning,
				      samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				      current_frame, buffer_size);
	}
	break;
	case AGS_SYNTH_OSCILLATOR_TRIANGLE:
	{
	  ags_lfo_synth_util_triangle(source->stream_current->data,
				      lfo_freq, lfo_phase,
				      lfo_depth,
				      lfo_tuning,
				      samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				      current_frame, buffer_size);
	}
	break;
	case AGS_SYNTH_OSCILLATOR_SQUARE:
	{
	  ags_lfo_synth_util_square(source->stream_current->data,
				    lfo_freq, lfo_phase,
				    lfo_depth,
				    lfo_tuning,
				    samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				    current_frame, buffer_size);
	}
	break;
	case AGS_SYNTH_OSCILLATOR_IMPULSE:
	{
	  ags_lfo_synth_util_impulse(source->stream_current->data,
				     lfo_freq, lfo_phase,
				     lfo_depth,
				     lfo_tuning,
				     samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				     current_frame, buffer_size);
	}
	break;    
	default:
	  g_warning("invalid LFO wave");
	}
    
	g_rec_mutex_unlock(stream_mutex);
	
	note = note->next;
      }
    }
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_lfo_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_lfo_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxLfoAudioSignal
 *
 * Returns: the new #AgsFxLfoAudioSignal
 *
 * Since: 3.8.0
 */
    AgsFxLfoAudioSignal*
      ags_fx_lfo_audio_signal_new(AgsAudioSignal *audio_signal)
    {
      AgsFxLfoAudioSignal *fx_lfo_audio_signal;

  fx_lfo_audio_signal = (AgsFxLfoAudioSignal *) g_object_new(AGS_TYPE_FX_LFO_AUDIO_SIGNAL,
							     "source", audio_signal,
							     NULL);

  return(fx_lfo_audio_signal);
}
