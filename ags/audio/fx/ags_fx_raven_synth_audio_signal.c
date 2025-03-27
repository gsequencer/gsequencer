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

#include <ags/audio/fx/ags_fx_raven_synth_audio_signal.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_raven_synth_audio.h>
#include <ags/audio/fx/ags_fx_raven_synth_audio_processor.h>
#include <ags/audio/fx/ags_fx_raven_synth_channel.h>
#include <ags/audio/fx/ags_fx_raven_synth_channel_processor.h>
#include <ags/audio/fx/ags_fx_raven_synth_recycling.h>

#include <ags/i18n.h>

void ags_fx_raven_synth_audio_signal_class_init(AgsFxRavenSynthAudioSignalClass *fx_raven_synth_audio_signal);
void ags_fx_raven_synth_audio_signal_init(AgsFxRavenSynthAudioSignal *fx_raven_synth_audio_signal);
void ags_fx_raven_synth_audio_signal_dispose(GObject *gobject);
void ags_fx_raven_synth_audio_signal_finalize(GObject *gobject);

void ags_fx_raven_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						 AgsAudioSignal *source,
						 AgsNote *note,
						 gboolean pattern_mode,
						 guint x0, guint x1,
						 guint y,
						 gdouble delay_counter, guint64 offset_counter,
						 guint frame_count,
						 gdouble delay, guint buffer_size);
void ags_fx_raven_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						   AgsAudioSignal *source,
						   AgsNote *note,
						   guint x0, guint x1,
						   guint y);

/**
 * SECTION:ags_fx_raven_synth_audio_signal
 * @short_description: fx raven synth audio signal
 * @title: AgsFxRavenSynthAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_raven_synth_audio_signal.h
 *
 * The #AgsFxRavenSynthAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_raven_synth_audio_signal_parent_class = NULL;

const gchar *ags_fx_raven_synth_audio_signal_plugin_name = "ags-fx-raven-synth";

GType
ags_fx_raven_synth_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_raven_synth_audio_signal = 0;

    static const GTypeInfo ags_fx_raven_synth_audio_signal_info = {
      sizeof (AgsFxRavenSynthAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_raven_synth_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxRavenSynthAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_raven_synth_audio_signal_init,
    };

    ags_type_fx_raven_synth_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								  "AgsFxRavenSynthAudioSignal",
								  &ags_fx_raven_synth_audio_signal_info,
								  0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_raven_synth_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_raven_synth_audio_signal_class_init(AgsFxRavenSynthAudioSignalClass *fx_raven_synth_audio_signal)
{
  GObjectClass *gobject;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_raven_synth_audio_signal_parent_class = g_type_class_peek_parent(fx_raven_synth_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_raven_synth_audio_signal;

  gobject->dispose = ags_fx_raven_synth_audio_signal_dispose;
  gobject->finalize = ags_fx_raven_synth_audio_signal_finalize;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_raven_synth_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_raven_synth_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_raven_synth_audio_signal_notify_remove;
}

void
ags_fx_raven_synth_audio_signal_init(AgsFxRavenSynthAudioSignal *fx_raven_synth_audio_signal)
{
  AGS_RECALL(fx_raven_synth_audio_signal)->name = "ags-fx-raven-synth";
  AGS_RECALL(fx_raven_synth_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_raven_synth_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_raven_synth_audio_signal)->xml_type = "ags-fx-raven-synth-audio-signal";
}

void
ags_fx_raven_synth_audio_signal_dispose(GObject *gobject)
{
  AgsFxRavenSynthAudioSignal *fx_raven_synth_audio_signal;
  
  fx_raven_synth_audio_signal = AGS_FX_RAVEN_SYNTH_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_raven_synth_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_raven_synth_audio_signal_finalize(GObject *gobject)
{
  AgsFxRavenSynthAudioSignal *fx_raven_synth_audio_signal;
  
  fx_raven_synth_audio_signal = AGS_FX_RAVEN_SYNTH_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_raven_synth_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_raven_synth_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					    AgsAudioSignal *source,
					    AgsNote *note,
					    gboolean pattern_mode,
					    guint x0, guint x1,
					    guint y,
					    gdouble delay_counter, guint64 offset_counter,
					    guint frame_count,
					    gdouble delay, guint buffer_size)
{
  //TODO:JK: implement me
}

void
ags_fx_raven_synth_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					      AgsAudioSignal *source,
					      AgsNote *note,
					      guint x0, guint x1,
					      guint y)
{
  AgsAudio *audio;
  AgsFxRavenSynthAudio *fx_raven_synth_audio;
  AgsFxRavenSynthAudioProcessor *fx_raven_synth_audio_processor;
  AgsFxRavenSynthChannelProcessor *fx_raven_synth_channel_processor;
  AgsFxRavenSynthRecycling *fx_raven_synth_recycling;

  guint sound_scope;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;

  GRecMutex *fx_raven_synth_audio_mutex;

  audio = NULL;
  
  fx_raven_synth_audio = NULL;
  fx_raven_synth_audio_processor = NULL;

  fx_raven_synth_channel_processor = NULL;

  fx_raven_synth_recycling = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_raven_synth_recycling,
	       NULL);

  g_object_get(fx_raven_synth_recycling,
	       "parent", &fx_raven_synth_channel_processor,
	       NULL);
  
  g_object_get(fx_raven_synth_channel_processor,
	       "recall-audio", &fx_raven_synth_audio,
	       "recall-audio-run", &fx_raven_synth_audio_processor,
	       NULL);

  g_object_get(fx_raven_synth_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(fx_raven_synth_audio_processor,
	       "audio-channel", &audio_channel,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  fx_raven_synth_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  midi_note = (y - audio_start_mapping + midi_start_mapping);

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxRavenSynthAudioScopeData *scope_data;
    AgsFxRavenSynthAudioChannelData *channel_data;
    AgsFxRavenSynthAudioInputData *input_data;

    g_rec_mutex_lock(fx_raven_synth_audio_mutex);
      
    scope_data = fx_raven_synth_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[audio_channel];

    input_data = channel_data->input_data[midi_note];

    input_data->key_on -= 1;
      
    g_rec_mutex_unlock(fx_raven_synth_audio_mutex);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_raven_synth_audio != NULL){
    g_object_unref(fx_raven_synth_audio);
  }

  if(fx_raven_synth_audio_processor != NULL){
    g_object_unref(fx_raven_synth_audio_processor);
  }
  
  if(fx_raven_synth_channel_processor != NULL){
    g_object_unref(fx_raven_synth_channel_processor);
  }
  
  if(fx_raven_synth_recycling != NULL){
    g_object_unref(fx_raven_synth_recycling);
  }
}

/**
 * ags_fx_raven_synth_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxRavenSynthAudioSignal
 *
 * Returns: the new #AgsFxRavenSynthAudioSignal
 *
 * Since: 7.5.0
 */
AgsFxRavenSynthAudioSignal*
ags_fx_raven_synth_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxRavenSynthAudioSignal *fx_raven_synth_audio_signal;

  fx_raven_synth_audio_signal = (AgsFxRavenSynthAudioSignal *) g_object_new(AGS_TYPE_FX_RAVEN_SYNTH_AUDIO_SIGNAL,
									    "source", audio_signal,
									    NULL);

  return(fx_raven_synth_audio_signal);
}
