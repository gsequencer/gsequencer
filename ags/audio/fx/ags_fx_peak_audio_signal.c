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

#include <ags/audio/fx/ags_fx_peak_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_peak_channel.h>
#include <ags/audio/fx/ags_fx_peak_channel_processor.h>
#include <ags/audio/fx/ags_fx_peak_recycling.h>

#include <ags/i18n.h>

void ags_fx_peak_audio_signal_class_init(AgsFxPeakAudioSignalClass *fx_peak_audio_signal);
void ags_fx_peak_audio_signal_init(AgsFxPeakAudioSignal *fx_peak_audio_signal);
void ags_fx_peak_audio_signal_dispose(GObject *gobject);
void ags_fx_peak_audio_signal_finalize(GObject *gobject);

void ags_fx_peak_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_peak_audio_signal
 * @short_description: fx peak audio signal
 * @title: AgsFxPeakAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_peak_audio_signal.h
 *
 * The #AgsFxPeakAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_peak_audio_signal_parent_class = NULL;

static const gchar *ags_fx_peak_audio_signal_plugin_name = "ags-fx-peak";

GType
ags_fx_peak_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_peak_audio_signal = 0;

    static const GTypeInfo ags_fx_peak_audio_signal_info = {
      sizeof (AgsFxPeakAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_peak_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxPeakAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_peak_audio_signal_init,
    };

    ags_type_fx_peak_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							   "AgsFxPeakAudioSignal",
							   &ags_fx_peak_audio_signal_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_peak_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_peak_audio_signal_class_init(AgsFxPeakAudioSignalClass *fx_peak_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_peak_audio_signal_parent_class = g_type_class_peek_parent(fx_peak_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_peak_audio_signal;

  gobject->dispose = ags_fx_peak_audio_signal_dispose;
  gobject->finalize = ags_fx_peak_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_peak_audio_signal;
  
  recall->run_inter = ags_fx_peak_audio_signal_run_inter;
}

void
ags_fx_peak_audio_signal_init(AgsFxPeakAudioSignal *fx_peak_audio_signal)
{
  AGS_RECALL(fx_peak_audio_signal)->name = "ags-fx-peak";
  AGS_RECALL(fx_peak_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_peak_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_peak_audio_signal)->xml_type = "ags-fx-peak-audio-signal";
}

void
ags_fx_peak_audio_signal_dispose(GObject *gobject)
{
  AgsFxPeakAudioSignal *fx_peak_audio_signal;
  
  fx_peak_audio_signal = AGS_FX_PEAK_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_peak_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_peak_audio_signal_finalize(GObject *gobject)
{
  AgsFxPeakAudioSignal *fx_peak_audio_signal;
  
  fx_peak_audio_signal = AGS_FX_PEAK_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_peak_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_peak_audio_signal_run_inter(AgsRecall *recall)
{
  AgsFxPeakChannel *fx_peak_channel;
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;
  AgsFxPeakRecycling *fx_peak_recycling;
  AgsAudioSignal *source;

  gint sound_scope;
  guint buffer_size;
  guint format;
  guint copy_mode;
  
  GRecMutex *fx_peak_channel_mutex;
  GRecMutex *stream_mutex;

  sound_scope = ags_recall_get_sound_scope(recall);
  
  fx_peak_channel = NULL;
  fx_peak_channel_mutex = NULL;

  fx_peak_channel_processor = NULL;

  fx_peak_recycling = NULL;

  source = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(recall,
	       "parent", &fx_peak_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_peak_recycling,
	       "parent", &fx_peak_channel_processor,
	       NULL);

  g_object_get(fx_peak_channel_processor,
	       "recall-channel", &fx_peak_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						  ags_audio_buffer_util_format_from_soundcard(format));

  if(fx_peak_channel != NULL &&
     source != NULL &&
     source->stream_current != NULL &&
     sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST){
    fx_peak_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_peak_channel);
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  
    g_rec_mutex_lock(fx_peak_channel_mutex);
    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_copy_buffer_to_buffer(fx_peak_channel->input_data[sound_scope]->buffer, 1, 0,
						source->stream_current->data, 1, 0,
						buffer_size, copy_mode);    
    
    g_rec_mutex_unlock(stream_mutex);
    g_rec_mutex_unlock(fx_peak_channel_mutex);
  }

  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_peak_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_peak_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxPeakAudioSignal
 *
 * Returns: the new #AgsFxPeakAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxPeakAudioSignal*
ags_fx_peak_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxPeakAudioSignal *fx_peak_audio_signal;

  fx_peak_audio_signal = (AgsFxPeakAudioSignal *) g_object_new(AGS_TYPE_FX_PEAK_AUDIO_SIGNAL,
							       "source", audio_signal,
							       NULL);

  return(fx_peak_audio_signal);
}
