/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_mute_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>

#include <ags/audio/recall/ags_mute_audio.h>
#include <ags/audio/recall/ags_mute_channel.h>
#include <ags/audio/recall/ags_mute_channel_run.h>
#include <ags/audio/recall/ags_mute_recycling.h>

#include <stdlib.h>

void ags_mute_audio_signal_class_init(AgsMuteAudioSignalClass *mute_audio_signal);
void ags_mute_audio_signal_init(AgsMuteAudioSignal *mute_audio_signal);
void ags_mute_audio_signal_finalize(GObject *gobject);

void ags_mute_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_mute_audio_signal
 * @short_description: mutes audio signal
 * @title: AgsMuteAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_mute_audio_signal.h
 *
 * The #AgsMuteAudioSignal class mutes the audio signal.
 */

static gpointer ags_mute_audio_signal_parent_class = NULL;

GType
ags_mute_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mute_audio_signal = 0;

    static const GTypeInfo ags_mute_audio_signal_info = {
      sizeof (AgsMuteAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMuteAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_audio_signal_init,
    };

    ags_type_mute_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsMuteAudioSignal",
							&ags_mute_audio_signal_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_mute_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_mute_audio_signal_class_init(AgsMuteAudioSignalClass *mute_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_mute_audio_signal_parent_class = g_type_class_peek_parent(mute_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_audio_signal;

  gobject->finalize = ags_mute_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) mute_audio_signal;

  recall->run_inter = ags_mute_audio_signal_run_inter;  
}

void
ags_mute_audio_signal_init(AgsMuteAudioSignal *mute_audio_signal)
{
  AGS_RECALL(mute_audio_signal)->name = "ags-mute";
  AGS_RECALL(mute_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_audio_signal)->xml_type = "ags-mute-audio-signal";
  AGS_RECALL(mute_audio_signal)->port = NULL;

  AGS_RECALL(mute_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_mute_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_mute_audio_signal_parent_class)->finalize(gobject);
}

void
ags_mute_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsPort *port;
  AgsMuteAudio *mute_audio;
  AgsMuteChannel *mute_channel;
  AgsMuteChannelRun *mute_channel_run;
  AgsMuteRecycling *mute_recycling;
  AgsMuteAudioSignal *mute_audio_signal;
  
  GList *stream_source;

  gboolean audio_muted, channel_muted;
  guint buffer_size;
  guint i;

  void (*parent_class_run_inter)(AgsRecall *recall);

  GValue audio_value = {0,};
  GValue channel_value = {0,};

  mute_audio_signal = AGS_MUTE_AUDIO_SIGNAL(recall);

  /* get parent class and mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  parent_class_run_inter = AGS_RECALL_CLASS(ags_mute_audio_signal_parent_class)->run_inter;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_inter(recall);

  g_object_get(mute_audio_signal,
	       "source", &source,
	       NULL);

  stream_source = source->stream_current;

  if(stream_source == NULL){
    ags_recall_done(recall);

    g_object_unref(source);
    
    return;
  }

  g_object_get(source,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* get some fields */
  g_object_get(mute_audio_signal,
	       "parent", &mute_recycling,
	       NULL);

  g_object_get(mute_recycling,
	       "parent", &mute_channel_run,
	       NULL);

  g_object_get(mute_channel_run,
	       "recall-channel", &mute_channel,
	       NULL);

  g_object_get(mute_channel,
	       "recall-audio", &mute_audio,
	       NULL);

  /* check muted */
  g_object_get(mute_channel,
	       "muted", &port,
	       NULL);
  
  g_value_init(&channel_value,
	       G_TYPE_FLOAT);
  
  ags_port_safe_read(port,
		     &channel_value);

  channel_muted = (gboolean) g_value_get_float(&channel_value);
  
  g_value_unset(&channel_value);

  g_object_unref(port);

  /* check audio */
  g_object_get(mute_audio,
	       "muted", &port,
	       NULL);
  
  g_value_init(&audio_value,
	       G_TYPE_FLOAT);
  
  ags_port_safe_read(port,
		     &audio_value);

  audio_muted = (gboolean) g_value_get_float(&audio_value);

  g_value_unset(&audio_value);

  g_object_unref(port);
  
  /* if not muted return */
  if(!channel_muted && !audio_muted){
    goto ags_mute_audio_signal_run_inter_END;
  }
  
  /* mute */
  memset((signed short *) stream_source->data, 0, buffer_size * sizeof(signed short));

ags_mute_audio_signal_run_inter_END:

  /* unref */
  g_object_unref(source);

  g_object_unref(mute_recycling);

  g_object_unref(mute_channel_run);
  g_object_unref(mute_channel);
  g_object_unref(mute_audio);
}

/**
 * ags_mute_audio_signal_new:
 * @source: the source #AgsAudioSignal
 *
 * Create a new instance of #AgsMuteAudioSignal
 *
 * Returns: the new #AgsMuteAudioSignal
 *
 * Since: 2.0.0
 */
AgsMuteAudioSignal*
ags_mute_audio_signal_new(AgsAudioSignal *source)
{
  AgsMuteAudioSignal *mute_audio_signal;

  mute_audio_signal = (AgsMuteAudioSignal *) g_object_new(AGS_TYPE_MUTE_AUDIO_SIGNAL,
							  "source", source,
							  NULL);

  return(mute_audio_signal);
}
