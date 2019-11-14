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

#include <ags/audio/recall/ags_analyse_audio_signal.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_analyse_channel.h>
#include <ags/audio/recall/ags_analyse_channel_run.h>
#include <ags/audio/recall/ags_analyse_recycling.h>

#include <ags/i18n.h>

void ags_analyse_audio_signal_class_init(AgsAnalyseAudioSignalClass *analyse_audio_signal);
void ags_analyse_audio_signal_init(AgsAnalyseAudioSignal *analyse_audio_signal);
void ags_analyse_audio_signal_finalize(GObject *gobject);

void ags_analyse_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_analyse_audio_signal
 * @short_description: analyses audio signal
 * @title: AgsAnalyseAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_analyse_audio_signal.h
 *
 * The #AgsAnalyseAudioSignal class analyses the audio signal.
 */

static gpointer ags_analyse_audio_signal_parent_class = NULL;

GType
ags_analyse_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_analyse_audio_signal = 0;

    static const GTypeInfo ags_analyse_audio_signal_info = {
      sizeof (AgsAnalyseAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_analyse_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAnalyseAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_analyse_audio_signal_init,
    };

    ags_type_analyse_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							   "AgsAnalyseAudioSignal",
							   &ags_analyse_audio_signal_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_analyse_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_analyse_audio_signal_class_init(AgsAnalyseAudioSignalClass *analyse_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_analyse_audio_signal_parent_class = g_type_class_peek_parent(analyse_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) analyse_audio_signal;

  gobject->finalize = ags_analyse_audio_signal_finalize;

  /* properties */

  /* AgsRecallClass */
  recall = (AgsRecallClass *) analyse_audio_signal;

  recall->run_inter = ags_analyse_audio_signal_run_inter;
}

void
ags_analyse_audio_signal_init(AgsAnalyseAudioSignal *analyse_audio_signal)
{
  AGS_RECALL(analyse_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_analyse_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_analyse_audio_signal_parent_class)->finalize(gobject);
}

void
ags_analyse_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;

  AgsAnalyseAudioSignal *analyse_audio_signal;

  void (*parent_class_run_inter)(AgsRecall *recall);
  
  analyse_audio_signal = AGS_ANALYSE_AUDIO_SIGNAL(recall);

  /* get parent class */
  parent_class_run_inter = AGS_RECALL_CLASS(ags_analyse_audio_signal_parent_class)->run_inter;

  /* call parent */
  parent_class_run_inter(recall);
  
  /* get some fields */
  g_object_get(analyse_audio_signal,
	       "source", &source,
	       NULL);
  
  if(source->stream_current != NULL){
    AgsAnalyseChannel *analyse_channel;
    AgsAnalyseChannelRun *analyse_channel_run;
    AgsAnalyseRecycling *analyse_recycling;

    guint samplerate;
    guint buffer_size;
    guint format;
  
    g_object_get(analyse_audio_signal,
		 "parent", &analyse_recycling,
		 NULL);

    g_object_get(analyse_recycling,
		 "parent", &analyse_channel_run,
		 NULL);

    g_object_get(analyse_channel_run,
		 "recall-channel", &analyse_channel,
		 NULL);

    g_object_get(source,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);
    
    ags_analyse_channel_buffer_add(analyse_channel,
				   source->stream_current->data,
				   samplerate,
				   buffer_size,
				   format);

    g_object_unref(analyse_recycling);

    g_object_unref(analyse_channel);

    g_object_unref(analyse_channel_run);
  }else{
    ags_recall_done(recall);
  }

  g_object_unref(source);
}

/**
 * ags_analyse_audio_signal_new:
 * @source: the source #AgsAudioSignal
 *
 * Create a new instance of #AgsAnalyseAudioSignal
 *
 * Returns: the new #AgsAnalyseAudioSignal
 *
 * Since: 2.0.0
 */
AgsAnalyseAudioSignal*
ags_analyse_audio_signal_new(AgsAudioSignal *source)
{
  AgsAnalyseAudioSignal *analyse_audio_signal;

  analyse_audio_signal = (AgsAnalyseAudioSignal *) g_object_new(AGS_TYPE_ANALYSE_AUDIO_SIGNAL,
								"source", source,
								NULL);

  return(analyse_audio_signal);
}
