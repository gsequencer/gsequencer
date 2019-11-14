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

#include <ags/audio/recall/ags_prepare_audio_signal.h>

#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_prepare_channel.h>
#include <ags/audio/recall/ags_prepare_channel_run.h>
#include <ags/audio/recall/ags_prepare_recycling.h>

void ags_prepare_audio_signal_class_init(AgsPrepareAudioSignalClass *prepare_audio_signal);
void ags_prepare_audio_signal_init(AgsPrepareAudioSignal *prepare_audio_signal);
void ags_prepare_audio_signal_finalize(GObject *gobject);

void ags_prepare_audio_signal_run_init_pre(AgsRecall *recall);
void ags_prepare_audio_signal_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_prepare_audio_signal
 * @short_description: prepares audio signal
 * @title: AgsPrepareAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_prepare_audio_signal.h
 *
 * The #AgsPrepareAudioSignal class prepares the audio signal.
 */

static gpointer ags_prepare_audio_signal_parent_class = NULL;

GType
ags_prepare_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_prepare_audio_signal = 0;

    static const GTypeInfo ags_prepare_audio_signal_info = {
      sizeof (AgsPrepareAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_prepare_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPrepareAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_prepare_audio_signal_init,
    };

    ags_type_prepare_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							   "AgsPrepareAudioSignal",
							   &ags_prepare_audio_signal_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_prepare_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_prepare_audio_signal_class_init(AgsPrepareAudioSignalClass *prepare_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_prepare_audio_signal_parent_class = g_type_class_peek_parent(prepare_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) prepare_audio_signal;

  gobject->finalize = ags_prepare_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) prepare_audio_signal;

  recall->run_init_pre = ags_prepare_audio_signal_run_init_pre;
  recall->run_pre = ags_prepare_audio_signal_run_pre;
}

void
ags_prepare_audio_signal_init(AgsPrepareAudioSignal *prepare_audio_signal)
{
  AGS_RECALL(prepare_audio_signal)->name = "ags-prepare";
  AGS_RECALL(prepare_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(prepare_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(prepare_audio_signal)->xml_type = "ags-prepare-audio-signal";
  AGS_RECALL(prepare_audio_signal)->port = NULL;
}

void
ags_prepare_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_prepare_audio_signal_parent_class)->finalize(gobject);
}

void
ags_prepare_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsChannel *channel;
  AgsRecycling *recycling;
  AgsAudioSignal *destination;
  AgsRecallID *recall_id, *parent_recall_id;
  AgsRecyclingContext *recycling_context, *parent_recycling_context;
  AgsPrepareChannelRun *prepare_channel_run;  
  AgsPrepareRecycling *prepare_recycling;
  AgsPrepareAudioSignal *prepare_audio_signal;

  GObject *output_soundcard;

  GList *start_list, *list;
  GList *stream;

  gdouble delay;
  guint attack;
  guint length;

  void (*parent_class_run_init_pre)(AgsRecall *recall);  
  
  prepare_audio_signal = (AgsPrepareAudioSignal *) recall;

  /* get parent class */
  AGS_RECALL_LOCK_CLASS();
  
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_prepare_audio_signal_parent_class)->run_init_pre;

  AGS_RECALL_UNLOCK_CLASS();

  /* get some fields */
  ags_recall_unset_behaviour_flags(recall, AGS_SOUND_BEHAVIOUR_PERSISTENT);

  g_object_get(prepare_audio_signal,
	       "parent", &prepare_recycling,
	       "output-soundcard", &output_soundcard,
	       "recall-id", &recall_id,
	       NULL);
  
  g_object_get(prepare_recycling,
	       "parent", &prepare_channel_run,
	       NULL);
  
  g_object_get(prepare_recycling,
	       "destination", &recycling,
	       NULL);
  
  g_object_get(prepare_channel_run,
	       "destination", &channel,
	       NULL);

  /* recycling context */
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(channel,
	       "recall-id", &start_list,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  parent_recall_id = ags_recall_id_find_recycling_context(start_list,
							  parent_recycling_context);

  //TODO:JK: unclear
  attack = 0;
  delay = 0.0;

  /* create new audio signal */
  destination = ags_audio_signal_new((GObject *) output_soundcard,
				     (GObject *) recycling,
				     (GObject *) parent_recall_id);
  
  g_object_set(prepare_audio_signal,
	       "destination", destination,
	       NULL);  
  ags_recycling_create_audio_signal_with_defaults(recycling,
						  destination,
						  delay, attack);
  length = 1; // (guint) (2.0 * soundcard->delay[soundcard->tic_counter]) + 1;
  ags_audio_signal_stream_resize(destination,
				 length);

  ags_connectable_connect(AGS_CONNECTABLE(destination));
  
  destination->stream_current = destination->stream;

  ags_recycling_add_audio_signal(recycling,
				 destination);

#ifdef AGS_DEBUG
  g_message("prepare %x to %x", destination, parent_recall_id);
  g_message("creating destination");
#endif
  
  /* call parent */
  parent_class_run_init_pre(recall);
}

void
ags_prepare_audio_signal_run_pre(AgsRecall *recall)
{
  AgsAudioSignal *destination, *source;
  
  void (*parent_class_run_pre)(AgsRecall *recall);  
  
  /* get parent class */
  AGS_RECALL_LOCK_CLASS();

  parent_class_run_pre = AGS_RECALL_CLASS(ags_prepare_audio_signal_parent_class)->run_pre;

  AGS_RECALL_UNLOCK_CLASS();

  /* call parent */
  parent_class_run_pre(recall);

  /* get some fields */
  g_object_get(recall,
	       "destination", &destination,
	       "source", &source,
	       NULL);
  
  if(source->stream_current != NULL){
    void *buffer;

    guint buffer_size;
    guint format;

    buffer = destination->stream_current->data;

    g_object_get(destination,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);
    
    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
  }else{
    ags_recall_done(recall);
  }
}

/**
 * ags_prepare_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsPrepareAudioSignal
 *
 * Returns: thde new #AgsPrepareAudioSignal
 *
 * Since: 2.0.0
 */
AgsPrepareAudioSignal*
ags_prepare_audio_signal_new(AgsAudioSignal *source)
{
  AgsPrepareAudioSignal *prepare_audio_signal;

  prepare_audio_signal = (AgsPrepareAudioSignal *) g_object_new(AGS_TYPE_PREPARE_AUDIO_SIGNAL,
								"source", source,
								NULL);

  return(prepare_audio_signal);
}
