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

#include <ags/audio/fx/ags_fx_synth_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_synth_audio.h>
#include <ags/audio/fx/ags_fx_synth_channel.h>
#include <ags/audio/fx/ags_fx_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_synth_recycling.h>

#include <ags/i18n.h>

void ags_fx_synth_audio_signal_class_init(AgsFxSynthAudioSignalClass *fx_synth_audio_signal);
void ags_fx_synth_audio_signal_init(AgsFxSynthAudioSignal *fx_synth_audio_signal);
void ags_fx_synth_audio_signal_dispose(GObject *gobject);
void ags_fx_synth_audio_signal_finalize(GObject *gobject);

void ags_fx_synth_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_synth_audio_signal
 * @short_description: fx synth audio signal
 * @title: AgsFxSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_synth_audio_signal.h
 *
 * The #AgsFxSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_synth_audio_signal_parent_class = NULL;

const gchar *ags_fx_synth_audio_signal_plugin_name = "ags-fx-synth";

GType
ags_fx_synth_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_synth_audio_signal_info = {
      sizeof (AgsFxSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_synth_audio_signal_init,
    };

    ags_type_fx_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
							    "AgsFxSynthAudioSignal",
							    &ags_fx_synth_audio_signal_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_synth_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_synth_audio_signal_class_init(AgsFxSynthAudioSignalClass *fx_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_synth_audio_signal;

  gobject->dispose = ags_fx_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_synth_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_synth_audio_signal;
  
  recall->run_inter = ags_fx_synth_audio_signal_real_run_inter;
}

void
ags_fx_synth_audio_signal_init(AgsFxSynthAudioSignal *fx_synth_audio_signal)
{
  AGS_RECALL(fx_synth_audio_signal)->name = "ags-fx-synth";
  AGS_RECALL(fx_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_synth_audio_signal)->xml_type = "ags-fx-synth-audio-signal";
}

void
ags_fx_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxSynthAudioSignal *fx_synth_audio_signal;
  
  fx_synth_audio_signal = AGS_FX_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxSynthAudioSignal *fx_synth_audio_signal;
  
  fx_synth_audio_signal = AGS_FX_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_synth_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxSynthAudio *fx_synth_audio;
  AgsFxSynthChannel *fx_synth_channel;
  AgsFxSynthChannelProcessor *fx_synth_channel_processor;
  AgsFxSynthRecycling *fx_synth_recycling;
  AgsFxSynthAudioSignal *fx_synth_audio_signal;
  
  guint buffer_size;
  guint format;
  guint samplerate;
  
  GRecMutex *stream_mutex;
  
  source = NULL;

  fx_synth_audio = NULL;
  fx_synth_channel = NULL;
  fx_synth_channel_processor = NULL;
  fx_synth_recycling = NULL;
  fx_synth_audio_signal = (AgsFxSynthAudioSignal *) recall;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(recall,
	       "parent", &fx_synth_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_synth_recycling,
	       "parent", &fx_synth_channel_processor,
	       NULL);

  g_object_get(fx_synth_channel_processor,
	       "recall-audio", &fx_synth_audio,
	       "recall-channel", &fx_synth_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "samplerate", &samplerate,
	       NULL);

  if(fx_synth_audio != NULL){
    AgsPort *port;

    GValue value = {0,};
  }
  
  if(source != NULL &&
     source->stream_current != NULL){
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);

    g_rec_mutex_lock(stream_mutex);

    //TODO:JK: implement me
    
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
  
  if(fx_synth_audio != NULL){
    g_object_unref(fx_synth_audio);
  }
  
  if(fx_synth_channel != NULL){
    g_object_unref(fx_synth_channel);
  }

  if(fx_synth_channel_processor != NULL){
    g_object_unref(fx_synth_channel_processor);
  }

  if(fx_synth_recycling != NULL){
    g_object_unref(fx_synth_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_synth_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxSynthAudioSignal
 *
 * Returns: the new #AgsFxSynthAudioSignal
 *
 * Since: 3.14.0
 */
AgsFxSynthAudioSignal*
ags_fx_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxSynthAudioSignal *fx_synth_audio_signal;

  fx_synth_audio_signal = (AgsFxSynthAudioSignal *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO_SIGNAL,
								 "source", audio_signal,
								 NULL);

  return(fx_synth_audio_signal);
}
