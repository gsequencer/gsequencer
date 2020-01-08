/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_lfo_audio_signal.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_synth_enums.h>
#include <ags/audio/ags_lfo_synth_util.h>

#include <ags/audio/recall/ags_lfo_channel.h>
#include <ags/audio/recall/ags_lfo_channel_run.h>
#include <ags/audio/recall/ags_lfo_recycling.h>

void ags_lfo_audio_signal_class_init(AgsLfoAudioSignalClass *lfo_audio_signal);
void ags_lfo_audio_signal_init(AgsLfoAudioSignal *lfo_audio_signal);
void ags_lfo_audio_signal_finalize(GObject *gobject);

void ags_lfo_audio_signal_run_init_pre(AgsRecall *recall);
void ags_lfo_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_lfo_audio_signal
 * @short_description: lfos audio signal
 * @title: AgsLfoAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_lfo_audio_signal.h
 *
 * The #AgsLfoAudioSignal class lfos the audio signal.
 */

static gpointer ags_lfo_audio_signal_parent_class = NULL;

GType
ags_lfo_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lfo_audio_signal = 0;

    static const GTypeInfo ags_lfo_audio_signal_info = {
      sizeof (AgsLfoAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lfo_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLfoAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lfo_audio_signal_init,
    };

    ags_type_lfo_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
						       "AgsLfoAudioSignal",
						       &ags_lfo_audio_signal_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lfo_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_lfo_audio_signal_class_init(AgsLfoAudioSignalClass *lfo_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_lfo_audio_signal_parent_class = g_type_class_peek_parent(lfo_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) lfo_audio_signal;

  gobject->finalize = ags_lfo_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) lfo_audio_signal;

  recall->run_inter = ags_lfo_audio_signal_run_inter;
}

void
ags_lfo_audio_signal_init(AgsLfoAudioSignal *lfo_audio_signal)
{
  AGS_RECALL(lfo_audio_signal)->name = "ags-lfo";
  AGS_RECALL(lfo_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(lfo_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(lfo_audio_signal)->xml_type = "ags-lfo-audio-signal";
  AGS_RECALL(lfo_audio_signal)->port = NULL;

  lfo_audio_signal->current_lfo_phase = 0.0;
}

void
ags_lfo_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_lfo_audio_signal_parent_class)->finalize(gobject);
}

void
ags_lfo_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsLfoChannel *lfo_channel;
  AgsLfoChannelRun *lfo_channel_run;
  AgsLfoRecycling *lfo_recycling;
  AgsLfoAudioSignal *lfo_audio_signal;
  AgsPort *port;
  
  GValue value = {0,};

  void (*parent_class_run_init_pre)(AgsRecall *recall);

  lfo_audio_signal = (AgsLfoAudioSignal *) recall;
    
  /* get parent class */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_lfo_audio_signal_parent_class)->run_init_pre;

  g_object_get(lfo_audio_signal,
	       "parent", &lfo_recycling,
	       NULL);

  g_object_get(lfo_recycling,
	       "parent", &lfo_channel_run,
	       NULL);
    
  g_object_get(lfo_channel_run,
	       "recall-channel", &lfo_channel,
	       NULL);

  g_object_get(lfo_channel,
	       "lfo-phase", &port,
	       NULL);
    
  g_value_init(&value,
	       G_TYPE_FLOAT);

  ags_port_safe_read(port,
		     &value);

  lfo_audio_signal->current_lfo_phase = g_value_get_float(&value);
  
  g_value_unset(&value);

  g_object_unref(port);

  g_object_unref(lfo_recycling);

  g_object_unref(lfo_channel_run);
  g_object_unref(lfo_channel);
}

void
ags_lfo_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsLfoAudioSignal *lfo_audio_signal;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context, *parent_recycling_context;
  
  GList *start_note;
  
  void (*parent_class_run_inter)(AgsRecall *recall);

  lfo_audio_signal = (AgsLfoAudioSignal *) recall;
    
  /* get parent class */
  parent_class_run_inter = AGS_RECALL_CLASS(ags_lfo_audio_signal_parent_class)->run_inter;

  /* call parent */
  parent_class_run_inter(recall);

  g_object_get(lfo_audio_signal,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  g_object_get(source,
	       "note", &start_note,
	       NULL);
  
  if(ags_recall_global_get_rt_safe() &&
     parent_recycling_context != NULL &&
     start_note == NULL){
    g_object_unref(source);

    g_object_unref(recall_id);
    
    g_object_unref(recycling_context);
    g_object_unref(parent_recycling_context);

    return;
  }
  
  if(source->stream_current != NULL){
    AgsLfoChannel *lfo_channel;
    AgsLfoChannelRun *lfo_channel_run;
    AgsLfoRecycling *lfo_recycling;
    AgsPort *port;

    void *buffer;

    gboolean enabled;
    guint lfo_wave;
    gdouble lfo_freq;
    gdouble lfo_phase;
    gdouble lfo_depth;
    gdouble lfo_tuning;
    guint samplerate;
    guint buffer_size;
    guint format;
    guint limit;
    guint i;
 
    GValue value = {0,};

    g_object_get(lfo_audio_signal,
		 "parent", &lfo_recycling,
		 NULL);

    g_object_get(lfo_recycling,
		 "parent", &lfo_channel_run,
		 NULL);
    
    g_object_get(lfo_channel_run,
		 "recall-channel", &lfo_channel,
		 NULL);

    g_object_get(source,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);

    buffer = source->stream_current->data;

    /* ports */
    g_object_get(lfo_channel,
		 "enabled", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_FLOAT);

    ags_port_safe_read(port,
		       &value);

    enabled = (g_value_get_float(&value) == 1.0) ? TRUE: FALSE;
    
    g_value_unset(&value);

    g_object_unref(port);

    if(!enabled){
      g_object_unref(lfo_recycling);

      g_object_unref(lfo_channel_run);
      g_object_unref(lfo_channel);

      return;
    }

    /* wave */
    g_object_get(lfo_channel,
		 "lfo-wave", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_FLOAT);

    ags_port_safe_read(port,
		       &value);

    lfo_wave = (guint) g_value_get_float(&value);
    
    g_value_unset(&value);

    g_object_unref(port);

    /* freq */
    g_object_get(lfo_channel,
		 "lfo-freq", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_FLOAT);

    ags_port_safe_read(port,
		       &value);

    lfo_freq = g_value_get_float(&value);
    
    g_value_unset(&value);

    g_object_unref(port);

    /* phase */
    lfo_phase = lfo_audio_signal->current_lfo_phase;

    /* depth */
    g_object_get(lfo_channel,
		 "lfo-depth", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_FLOAT);

    ags_port_safe_read(port,
		       &value);

    lfo_depth = g_value_get_float(&value);
    
    g_value_unset(&value);

    g_object_unref(port);

    /* tuning */
    g_object_get(lfo_channel,
		 "lfo-tuning", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_FLOAT);

    ags_port_safe_read(port,
		       &value);

    lfo_tuning = g_value_get_float(&value);
    
    g_value_unset(&value);

    g_object_unref(port);

    /* LFO */
    switch(lfo_wave){
    case AGS_SYNTH_OSCILLATOR_SIN:
    {
      ags_lfo_synth_util_sin(buffer,
			     lfo_freq, lfo_phase,
			     lfo_depth,
			     lfo_tuning,
			     samplerate, ags_audio_buffer_util_format_from_soundcard(format),
			     0, buffer_size);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SAWTOOTH:
    {
      ags_lfo_synth_util_sawtooth(buffer,
				  lfo_freq, lfo_phase,
				  lfo_depth,
				  lfo_tuning,
				  samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				  0, buffer_size);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_TRIANGLE:
    {
      ags_lfo_synth_util_triangle(buffer,
				  lfo_freq, lfo_phase,
				  lfo_depth,
				  lfo_tuning,
				  samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				  0, buffer_size);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_SQUARE:
    {
      ags_lfo_synth_util_square(buffer,
				lfo_freq, lfo_phase,
				lfo_depth,
				lfo_tuning,
				samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				0, buffer_size);
    }
    break;
    case AGS_SYNTH_OSCILLATOR_IMPULSE:
    {
      ags_lfo_synth_util_impulse(buffer,
				 lfo_freq, lfo_phase,
				 lfo_depth,
				 lfo_tuning,
				 samplerate, ags_audio_buffer_util_format_from_soundcard(format),
				 0, buffer_size);
    }
    break;
    };

    lfo_audio_signal->current_lfo_phase += buffer_size;

    if(lfo_audio_signal->current_lfo_phase >= samplerate / lfo_freq){
      lfo_audio_signal->current_lfo_phase = (guint) lfo_audio_signal->current_lfo_phase % (guint) (samplerate / lfo_freq);
    }
    
    g_object_unref(lfo_recycling);

    g_object_unref(lfo_channel_run);
    g_object_unref(lfo_channel);
  }else{
    ags_recall_done(recall);
  }

  /* unref */
  g_object_unref(source);

  g_object_unref(recall_id);
    
  g_object_unref(recycling_context);

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }
  
  g_list_free_full(start_note,
		   g_object_unref);
}

/**
 * ags_lfo_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsLfoAudioSignal
 *
 * Returns: the new #AgsLfoAudioSignal
 *
 * Since: 3.0.0
 */
AgsLfoAudioSignal*
ags_lfo_audio_signal_new(AgsAudioSignal *source)
{
  AgsLfoAudioSignal *lfo_audio_signal;

  lfo_audio_signal = (AgsLfoAudioSignal *) g_object_new(AGS_TYPE_LFO_AUDIO_SIGNAL,
							"source", source,
							NULL);

  return(lfo_audio_signal);
}
