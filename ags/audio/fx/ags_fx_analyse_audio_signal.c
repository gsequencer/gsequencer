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

#include <ags/audio/fx/ags_fx_analyse_audio_signal.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_analyse_channel.h>
#include <ags/audio/fx/ags_fx_analyse_channel_processor.h>
#include <ags/audio/fx/ags_fx_analyse_recycling.h>

#include <ags/i18n.h>

void ags_fx_analyse_audio_signal_class_init(AgsFxAnalyseAudioSignalClass *fx_analyse_audio_signal);
void ags_fx_analyse_audio_signal_init(AgsFxAnalyseAudioSignal *fx_analyse_audio_signal);
void ags_fx_analyse_audio_signal_dispose(GObject *gobject);
void ags_fx_analyse_audio_signal_finalize(GObject *gobject);

void ags_fx_analyse_audio_signal_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_analyse_audio_signal
 * @short_description: fx analyse audio signal
 * @title: AgsFxAnalyseAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_analyse_audio_signal.h
 *
 * The #AgsFxAnalyseAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_analyse_audio_signal_parent_class = NULL;

static const gchar *ags_fx_analyse_audio_signal_plugin_name = "ags-fx-analyse";

GType
ags_fx_analyse_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_analyse_audio_signal = 0;

    static const GTypeInfo ags_fx_analyse_audio_signal_info = {
      sizeof (AgsFxAnalyseAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_analyse_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxAnalyseAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_analyse_audio_signal_init,
    };

    ags_type_fx_analyse_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							      "AgsFxAnalyseAudioSignal",
							      &ags_fx_analyse_audio_signal_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_analyse_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_analyse_audio_signal_class_init(AgsFxAnalyseAudioSignalClass *fx_analyse_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_analyse_audio_signal_parent_class = g_type_class_peek_parent(fx_analyse_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_analyse_audio_signal;

  gobject->dispose = ags_fx_analyse_audio_signal_dispose;
  gobject->finalize = ags_fx_analyse_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_analyse_audio_signal;
  
  recall->run_inter = ags_fx_analyse_audio_signal_real_run_inter;
}

void
ags_fx_analyse_audio_signal_init(AgsFxAnalyseAudioSignal *fx_analyse_audio_signal)
{
  AGS_RECALL(fx_analyse_audio_signal)->name = "ags-fx-analyse";
  AGS_RECALL(fx_analyse_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_analyse_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_analyse_audio_signal)->xml_type = "ags-fx-analyse-audio-signal";
}

void
ags_fx_analyse_audio_signal_dispose(GObject *gobject)
{
  AgsFxAnalyseAudioSignal *fx_analyse_audio_signal;
  
  fx_analyse_audio_signal = AGS_FX_ANALYSE_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_analyse_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_analyse_audio_signal_finalize(GObject *gobject)
{
  AgsFxAnalyseAudioSignal *fx_analyse_audio_signal;
  
  fx_analyse_audio_signal = AGS_FX_ANALYSE_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_analyse_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_analyse_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsFxAnalyseChannel *fx_analyse_channel;
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;
  AgsFxAnalyseRecycling *fx_analyse_recycling;
  AgsAudioSignal *source;

  gint sound_scope;
  guint buffer_size;
  guint format;
  guint copy_mode;
  
  GRecMutex *fx_analyse_channel_mutex;
  GRecMutex *stream_mutex;

  sound_scope = ags_recall_get_sound_scope(recall);
  
  fx_analyse_channel = NULL;
  fx_analyse_channel_mutex = NULL;

  fx_analyse_channel_processor = NULL;

  fx_analyse_recycling = NULL;

  source = NULL;

  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(recall,
	       "parent", &fx_analyse_recycling,
	       "source", &source,
	       NULL);

  g_object_get(fx_analyse_recycling,
	       "parent", &fx_analyse_channel_processor,
	       NULL);

  g_object_get(fx_analyse_channel_processor,
	       "recall-channel", &fx_analyse_channel,
	       NULL);

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_DOUBLE,
						  ags_audio_buffer_util_format_from_soundcard(format));

  if(fx_analyse_channel != NULL &&
     source != NULL &&
     source->stream_current != NULL &&
     sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST){
    fx_analyse_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_analyse_channel);
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(source);
  
    g_rec_mutex_lock(fx_analyse_channel_mutex);
    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_copy_buffer_to_buffer(fx_analyse_channel->input_data[sound_scope]->in, 1, 0,
						source->stream_current->data, 1, 0,
						buffer_size, copy_mode);    
    
    g_rec_mutex_unlock(stream_mutex);
    g_rec_mutex_unlock(fx_analyse_channel_mutex);
  }
    
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_analyse_audio_signal_parent_class)->run_inter(recall);
}

/**
 * ags_fx_analyse_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxAnalyseAudioSignal
 *
 * Returns: the new #AgsFxAnalyseAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxAnalyseAudioSignal*
ags_fx_analyse_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxAnalyseAudioSignal *fx_analyse_audio_signal;

  fx_analyse_audio_signal = (AgsFxAnalyseAudioSignal *) g_object_new(AGS_TYPE_FX_ANALYSE_AUDIO_SIGNAL,
								     "source", audio_signal,
								     NULL);

  return(fx_analyse_audio_signal);
}
