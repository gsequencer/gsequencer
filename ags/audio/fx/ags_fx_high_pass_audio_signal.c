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

#include <ags/audio/fx/ags_fx_high_pass_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_high_pass_audio.h>
#include <ags/audio/fx/ags_fx_high_pass_channel.h>
#include <ags/audio/fx/ags_fx_high_pass_channel_processor.h>
#include <ags/audio/fx/ags_fx_high_pass_recycling.h>

#include <ags/i18n.h>

void ags_fx_high_pass_audio_signal_class_init(AgsFxHighPassAudioSignalClass *fx_high_pass_audio_signal);
void ags_fx_high_pass_audio_signal_init(AgsFxHighPassAudioSignal *fx_high_pass_audio_signal);
void ags_fx_high_pass_audio_signal_dispose(GObject *gobject);
void ags_fx_high_pass_audio_signal_finalize(GObject *gobject);

void ags_fx_high_pass_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_high_pass_audio_signal
 * @short_description: fx high pass audio signal
 * @title: AgsFxHighPassAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_high_pass_audio_signal.h
 *
 * The #AgsFxHighPassAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_high_pass_audio_signal_parent_class = NULL;

const gchar *ags_fx_high_pass_audio_signal_plugin_name = "ags-fx-high-pass";

GType
ags_fx_high_pass_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_high_pass_audio_signal = 0;

    static const GTypeInfo ags_fx_high_pass_audio_signal_info = {
      sizeof (AgsFxHighPassAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_high_pass_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxHighPassAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_high_pass_audio_signal_init,
    };

    ags_type_fx_high_pass_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
								"AgsFxHighPassAudioSignal",
								&ags_fx_high_pass_audio_signal_info,
								0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_high_pass_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_high_pass_audio_signal_class_init(AgsFxHighPassAudioSignalClass *fx_high_pass_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_high_pass_audio_signal_parent_class = g_type_class_peek_parent(fx_high_pass_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_high_pass_audio_signal;

  gobject->dispose = ags_fx_high_pass_audio_signal_dispose;
  gobject->finalize = ags_fx_high_pass_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_high_pass_audio_signal;
  
  recall->run_inter = ags_fx_high_pass_audio_signal_real_run_inter;
}

void
ags_fx_high_pass_audio_signal_init(AgsFxHighPassAudioSignal *fx_high_pass_audio_signal)
{
  AGS_RECALL(fx_high_pass_audio_signal)->name = "ags-fx-high-pass";
  AGS_RECALL(fx_high_pass_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_high_pass_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_high_pass_audio_signal)->xml_type = "ags-fx-high-pass-audio-signal";
}

void
ags_fx_high_pass_audio_signal_dispose(GObject *gobject)
{
  AgsFxHighPassAudioSignal *fx_high_pass_audio_signal;
  
  fx_high_pass_audio_signal = AGS_FX_HIGH_PASS_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_high_pass_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_high_pass_audio_signal_finalize(GObject *gobject)
{
  AgsFxHighPassAudioSignal *fx_high_pass_audio_signal;
  
  fx_high_pass_audio_signal = AGS_FX_HIGH_PASS_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_high_pass_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_high_pass_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxHighPassAudio *fx_high_pass_audio;
  AgsFxHighPassChannel *fx_high_pass_channel;
  AgsFxHighPassChannelProcessor *fx_high_pass_channel_processor;
  AgsFxHighPassRecycling *fx_high_pass_recycling;
  AgsFxHighPassAudioSignal *fx_high_pass_audio_signal;
  
  guint buffer_size;
  guint format;
  guint samplerate;
  gboolean enabled;
  gdouble q_lin;
  gdouble filter_gain;

  GRecMutex *stream_mutex;
  
  source = NULL;

  fx_high_pass_audio = NULL;
  fx_high_pass_channel = NULL;
  fx_high_pass_channel_processor = NULL;
  fx_high_pass_recycling = NULL;
  fx_high_pass_audio_signal = (AgsFxHighPassAudioSignal *) recall;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  q_lin = 0.0;
  filter_gain = 1.0;

  enabled = FALSE;

  g_object_get(recall,
	       "parent", &fx_high_pass_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_high_pass_recycling,
	       "parent", &fx_high_pass_channel_processor,
	       NULL);

  g_object_get(fx_high_pass_channel_processor,
	       "recall-audio", &fx_high_pass_audio,
	       "recall-channel", &fx_high_pass_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "samplerate", &samplerate,
	       NULL);
  
  if(fx_high_pass_channel != NULL){
    AgsPort *port;

    GValue value = {0,};
        
    /* enabled */    
    g_object_get(fx_high_pass_channel,
		 "enabled", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      enabled = (g_value_get_float(&value) == 0.0) ? FALSE: TRUE;

      g_object_unref(port);
    }

    g_value_unset(&value);
        
    /* q-lin */    
    g_object_get(fx_high_pass_channel,
		 "q-lin", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);
      
      q_lin = g_value_get_float(&value);

      g_object_unref(port);
    }

    g_value_unset(&value);

    /* filter gain */
    g_object_get(fx_high_pass_channel,
		 "filter-gain", &port,
		 NULL);

    g_value_init(&value, G_TYPE_FLOAT);
    
    if(port != NULL){      
      ags_port_safe_read(port,
			 &value);

      filter_gain = g_value_get_float(&value);
      
      g_object_unref(port);
    }

    g_value_unset(&value);
  }

  if(enabled &&
     source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
    
    fx_high_pass_audio_signal->fluid_iir_filter_util.destination = source->stream_current->data;
    fx_high_pass_audio_signal->fluid_iir_filter_util.destination_stride = 1;
	
    fx_high_pass_audio_signal->fluid_iir_filter_util.source = source->stream_current->data;
    fx_high_pass_audio_signal->fluid_iir_filter_util.source_stride = 1;
	
    fx_high_pass_audio_signal->fluid_iir_filter_util.buffer_length = buffer_size;
    fx_high_pass_audio_signal->fluid_iir_filter_util.format = format;
    fx_high_pass_audio_signal->fluid_iir_filter_util.samplerate = samplerate;
	
    fx_high_pass_audio_signal->fluid_iir_filter_util.filter_type = AGS_FLUID_IIR_HIGHPASS;

    fx_high_pass_audio_signal->fluid_iir_filter_util.q_lin = q_lin;      
    fx_high_pass_audio_signal->fluid_iir_filter_util.filter_gain = filter_gain;
      
    g_rec_mutex_lock(stream_mutex);

    ags_fluid_iir_filter_util_process(&(fx_high_pass_audio_signal->fluid_iir_filter_util));
    
    g_rec_mutex_unlock(stream_mutex);
  }
  
  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }

  /* unref */
  if(source != NULL){
    g_object_unref(source);
  }
  
  if(fx_high_pass_audio != NULL){
    g_object_unref(fx_high_pass_audio);
  }
  
  if(fx_high_pass_channel != NULL){
    g_object_unref(fx_high_pass_channel);
  }

  if(fx_high_pass_channel_processor != NULL){
    g_object_unref(fx_high_pass_channel_processor);
  }

  if(fx_high_pass_recycling != NULL){
    g_object_unref(fx_high_pass_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_high_pass_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_high_pass_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxHighPassAudioSignal
 *
 * Returns: the new #AgsFxHighPassAudioSignal
 *
 * Since: 3.13.0
 */
AgsFxHighPassAudioSignal*
ags_fx_high_pass_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxHighPassAudioSignal *fx_high_pass_audio_signal;

  fx_high_pass_audio_signal = (AgsFxHighPassAudioSignal *) g_object_new(AGS_TYPE_FX_HIGH_PASS_AUDIO_SIGNAL,
									"source", audio_signal,
									NULL);

  return(fx_high_pass_audio_signal);
}
