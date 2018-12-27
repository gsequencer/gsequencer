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

#include <ags/audio/recall/ags_rt_stream_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_rt_stream_recycling.h>

void ags_rt_stream_channel_run_class_init(AgsRtStreamChannelRunClass *rt_stream_channel_run);
void ags_rt_stream_channel_run_init(AgsRtStreamChannelRun *rt_stream_channel_run);

void ags_rt_stream_channel_run_check_rt_data(AgsRecall *recall);
void ags_rt_stream_channel_run_done(AgsRecall *recall);

/**
 * SECTION:ags_rt_stream_channel_run
 * @short_description: rt_streams channel
 * @title: AgsRtStreamChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_rt_stream_channel_run.h
 *
 * The #AgsRtStreamChannelRun class streams the audio signal template.
 */

static gpointer ags_rt_stream_channel_run_parent_class = NULL;

GType
ags_rt_stream_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_rt_stream_channel_run = 0;

    static const GTypeInfo ags_rt_stream_channel_run_info = {
      sizeof (AgsRtStreamChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_rt_stream_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRtStreamChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_rt_stream_channel_run_init,
    };

    ags_type_rt_stream_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							    "AgsRtStreamChannelRun",
							    &ags_rt_stream_channel_run_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_rt_stream_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_rt_stream_channel_run_class_init(AgsRtStreamChannelRunClass *rt_stream_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_rt_stream_channel_run_parent_class = g_type_class_peek_parent(rt_stream_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) rt_stream_channel_run;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) rt_stream_channel_run;

  recall->check_rt_data = ags_rt_stream_channel_run_check_rt_data;
  recall->done = ags_rt_stream_channel_run_done;
}

void
ags_rt_stream_channel_run_init(AgsRtStreamChannelRun *rt_stream_channel_run)
{
  ags_recall_set_ability_flags((AgsRecall *) rt_stream_channel_run, (AGS_SOUND_ABILITY_PLAYBACK |
								     AGS_SOUND_ABILITY_SEQUENCER |
								     AGS_SOUND_ABILITY_NOTATION |
								     AGS_SOUND_ABILITY_WAVE |
								     AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(rt_stream_channel_run)->name = "ags-rt_stream";
  AGS_RECALL(rt_stream_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(rt_stream_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(rt_stream_channel_run)->xml_type = "ags-rt_stream-channel-run";
  AGS_RECALL(rt_stream_channel_run)->port = NULL;

  AGS_RECALL(rt_stream_channel_run)->child_type = AGS_TYPE_RT_STREAM_RECYCLING;
}

void
ags_rt_stream_channel_run_check_rt_data(AgsRecall *recall)
{
  AgsChannel *source;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *end_recycling;
  AgsRtStreamChannelRun *rt_stream_channel_run;
  AgsRecallID *recall_id;
  
  GObject *output_soundcard;
  
  rt_stream_channel_run = (AgsRtStreamChannelRun *) recall;

  g_object_get(rt_stream_channel_run,
	       "source", &source,
	       "output-soundcard", &output_soundcard,
	       "recall-id", &recall_id,
	       NULL);

  /* get first and last recycling */
  g_object_get(source,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);
  
  /* get end */
  g_object_get(last_recycling,
	       "next", &end_recycling,
	       NULL);

  /* */
  recycling = first_recycling;
  
  while(recycling != end_recycling){
    AgsAudioSignal *audio_signal;
    AgsAudioSignal *rt_template, *template;
    
    /* create rt template */
    rt_template = ags_audio_signal_new(output_soundcard,
				       (GObject *) recycling,
				       (GObject *) recall_id);
    rt_template->flags |= AGS_AUDIO_SIGNAL_RT_TEMPLATE;
    ags_recycling_create_audio_signal_with_defaults(recycling,
						    rt_template,
						    0.0, 0);
    ags_recycling_add_audio_signal(recycling,
				   rt_template);
    
    /* create buffer */
    audio_signal = ags_audio_signal_new(output_soundcard,
					(GObject *) recycling,
					(GObject *) recall_id);

    g_object_set(audio_signal,
		 "rt-template", rt_template,
		 NULL);
    ags_audio_signal_stream_resize(audio_signal,
				   3);
    audio_signal->stream_current = audio_signal->stream;
    
    /* add buffer */
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);

    /* iterate */
    g_object_get(recycling,
		 "next", &recycling,
		 NULL);
  }
}

void
ags_rt_stream_channel_run_done(AgsRecall *recall)
{
  AgsChannel *source;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *end_recycling;
  AgsRecallID *recall_id;
  AgsRtStreamChannelRun *rt_stream_channel_run;

  void (*parent_class_done)(AgsRecall *recall);  
  
  rt_stream_channel_run = (AgsRtStreamChannelRun *) recall;

  /* get parent class */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  parent_class_done = AGS_RECALL_CLASS(ags_rt_stream_channel_run_parent_class)->done;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  g_object_get(rt_stream_channel_run,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);

  /* get first and last recycling */
  g_object_get(source,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);
  
  /* get end */
  g_object_get(last_recycling,
	       "next", &end_recycling,
	       NULL);

  /* */
  recycling = first_recycling;
  
  while(recycling != end_recycling){
    GList *start_audio_signal, *audio_signal;
    
    g_object_get(recycling,
		 "audio-signal", &start_audio_signal,
		 NULL);
    
    audio_signal = start_audio_signal;
    
    while((audio_signal = ags_audio_signal_find_by_recall_id(audio_signal,
							     (GObject *) recall_id)) != NULL){
      ags_recycling_remove_audio_signal(recycling,
					audio_signal->data);
      
      audio_signal = audio_signal->next;
    }

    g_list_free(start_audio_signal);
    
    /* iterate */
    g_object_get(recycling,
		 "next", &recycling,
		 NULL);
  }
  
  /* call parent */
  parent_class_done(recall);
}

/**
 * ags_rt_stream_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsRtStreamChannelRun
 *
 * Returns: the new #AgsRtStreamChannelRun
 *
 * Since: 2.0.0
 */
AgsRtStreamChannelRun*
ags_rt_stream_channel_run_new(AgsChannel *source)
{
  AgsRtStreamChannelRun *rt_stream_channel_run;

  rt_stream_channel_run = (AgsRtStreamChannelRun *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL_RUN,
								 "source", source,
								 NULL);
  
  return(rt_stream_channel_run);
}
