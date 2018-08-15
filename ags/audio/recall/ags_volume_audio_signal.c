/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_volume_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_volume_channel.h>
#include <ags/audio/recall/ags_volume_channel_run.h>
#include <ags/audio/recall/ags_volume_recycling.h>

void ags_volume_audio_signal_class_init(AgsVolumeAudioSignalClass *volume_audio_signal);
void ags_volume_audio_signal_init(AgsVolumeAudioSignal *volume_audio_signal);
void ags_volume_audio_signal_finalize(GObject *gobject);

void ags_volume_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_volume_audio_signal
 * @short_description: volumes audio signal
 * @title: AgsVolumeAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_volume_audio_signal.h
 *
 * The #AgsVolumeAudioSignal class volumes the audio signal.
 */

static gpointer ags_volume_audio_signal_parent_class = NULL;

GType
ags_volume_audio_signal_get_type()
{
  static GType ags_type_volume_audio_signal = 0;

  if(!ags_type_volume_audio_signal){
    static const GTypeInfo ags_volume_audio_signal_info = {
      sizeof (AgsVolumeAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_audio_signal_init,
    };

    ags_type_volume_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsVolumeAudioSignal",
							  &ags_volume_audio_signal_info,
							  0);
  }

  return (ags_type_volume_audio_signal);
}

void
ags_volume_audio_signal_class_init(AgsVolumeAudioSignalClass *volume_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_volume_audio_signal_parent_class = g_type_class_peek_parent(volume_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_audio_signal;

  gobject->finalize = ags_volume_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) volume_audio_signal;

  recall->run_inter = ags_volume_audio_signal_run_inter;
}

void
ags_volume_audio_signal_init(AgsVolumeAudioSignal *volume_audio_signal)
{
  AGS_RECALL(volume_audio_signal)->name = "ags-volume";
  AGS_RECALL(volume_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(volume_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(volume_audio_signal)->xml_type = "ags-volume-audio-signal";
  AGS_RECALL(volume_audio_signal)->port = NULL;
}

void
ags_volume_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_volume_audio_signal_parent_class)->finalize(gobject);
}

void
ags_volume_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsVolumeAudioSignal *volume_audio_signal;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context, *parent_recycling_context;
  
  GList *start_note;
  
  void (*parent_class_run_inter)(AgsRecall *recall);

  volume_audio_signal = (AgsVolumeAudioSignal *) recall;
    
  /* get parent class */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  parent_class_run_inter = AGS_RECALL_CLASS(ags_volume_audio_signal_parent_class)->run_inter;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_inter(recall);

  g_object_get(volume_audio_signal,
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
    return;
  }
  
  if(source->stream_current != NULL){
    AgsPort *port;
    AgsVolumeChannel *volume_channel;
    AgsVolumeChannelRun *volume_channel_run;
    AgsVolumeRecycling *volume_recycling;

    void *buffer;

    gdouble volume;
    guint buffer_size;
    guint format;
    guint limit;
    guint i;

    GValue value = {0,};

    g_object_get(volume_audio_signal,
		 "parent", &volume_recycling,
		 NULL);

    g_object_get(volume_recycling,
		 "parent", &volume_channel_run,
		 NULL);
    
    g_object_get(volume_channel_run,
		 "recall-channel", &volume_channel,
		 NULL);

    g_object_get(source,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);

    buffer = source->stream_current->data;

    g_object_get(volume_channel,
		 "volume", &port,
		 NULL);
    
    g_value_init(&value,
		 G_TYPE_FLOAT);

    ags_port_safe_read(port,
		       &value);

    volume = g_value_get_float(&value);
    
    g_value_unset(&value);

    ags_audio_buffer_util_volume(buffer, 1,
				 ags_audio_buffer_util_format_from_soundcard(format),
				 buffer_size,
				 volume);

  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_volume_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsVolumeAudioSignal
 *
 * Returns: the new #AgsVolumeAudioSignal
 *
 * Since: 2.0.0
 */
AgsVolumeAudioSignal*
ags_volume_audio_signal_new(AgsAudioSignal *source)
{
  AgsVolumeAudioSignal *volume_audio_signal;

  volume_audio_signal = (AgsVolumeAudioSignal *) g_object_new(AGS_TYPE_VOLUME_AUDIO_SIGNAL,
							      "source", source,
							      NULL);

  return(volume_audio_signal);
}
