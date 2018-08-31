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

#include <ags/audio/recall/ags_feed_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_feed_channel.h>
#include <ags/audio/recall/ags_feed_channel_run.h>
#include <ags/audio/recall/ags_feed_recycling.h>

void ags_feed_audio_signal_class_init(AgsFeedAudioSignalClass *feed_audio_signal);
void ags_feed_audio_signal_init(AgsFeedAudioSignal *feed_audio_signal);
void ags_feed_audio_signal_finalize(GObject *gobject);

void ags_feed_audio_signal_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_feed_audio_signal
 * @short_description: feeds audio signal
 * @title: AgsFeedAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_feed_audio_signal.h
 *
 * The #AgsFeedAudioSignal class feeds the audio signal.
 */

static gpointer ags_feed_audio_signal_parent_class = NULL;

GType
ags_feed_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_feed_audio_signal;

    static const GTypeInfo ags_feed_audio_signal_info = {
      sizeof (AgsFeedAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_audio_signal_init,
    };

    ags_type_feed_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsFeedAudioSignal",
							&ags_feed_audio_signal_info,
							0);
  }

  return(ags_type_feed_audio_signal);
}

void
ags_feed_audio_signal_class_init(AgsFeedAudioSignalClass *feed_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_feed_audio_signal_parent_class = g_type_class_peek_parent(feed_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_audio_signal;

  gobject->finalize = ags_feed_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) feed_audio_signal;

  recall->run_pre = ags_feed_audio_signal_run_pre;
}

void
ags_feed_audio_signal_init(AgsFeedAudioSignal *feed_audio_signal)
{
  AGS_RECALL(feed_audio_signal)->name = "ags-feed";
  AGS_RECALL(feed_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_audio_signal)->xml_type = "ags-feed-audio-signal";
  AGS_RECALL(feed_audio_signal)->port = NULL;
}

void
ags_feed_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_feed_audio_signal_parent_class)->finalize(gobject);
}

void
ags_feed_audio_signal_run_pre(AgsRecall *recall)
{
  AgsAudioSignal *template, *audio_signal;
  AgsFeedAudioSignal *feed_audio_signal;

  GObject *output_soundcard;
  
  GList *note_start, *note;
  
  void (*parent_class_run_pre)(AgsRecall *recall);

  pthread_mutex_t *recall_mutex;
  
  feed_audio_signal = (AgsFeedAudioSignal *) recall;

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;

  parent_class_run_pre = AGS_RECALL_CLASS(ags_feed_audio_signal_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_pre(recall);

  if(ags_recall_global_get_rt_safe()){
    return;
  }

  g_object_get(feed_audio_signal,
	       "output-soundcard", &output_soundcard,
	       "source", &audio_signal,
	       NULL);

  g_object_get(audio_signal,
	       "note", &note_start,
	       NULL);

  if(note_start != NULL){
    note = note_start;

    while(note != NULL){      
      if(ags_note_test_flags(note->data, AGS_NOTE_FEED)){
	AgsRecycling *recycling;

	guint x0, x1;
	guint samplerate;
	gdouble notation_delay;
	guint frame_count;

	template = NULL;

	g_object_get(note->data,
		     "x0", &x0,
		     "x1", &x1,
		     NULL);
	
	g_object_get(audio_signal,
		     "recycling", &recycling,
		     "samplerate", &samplerate,
		     NULL);
      
	if(recycling != NULL){
	  GList *list_start;

	  g_object_get(recycling,
		       "audio-signal", &list_start,
		       NULL);
	
	  template = ags_audio_signal_get_template(list_start);
	}
    
	/* get notation delay */
	notation_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));

	/* feed audio signal */
	frame_count = (guint) (((gdouble) samplerate / notation_delay) * (gdouble) (x1 - x0));

	ags_audio_signal_feed(audio_signal,
			      template,
			      frame_count);
      }

      note = note->next;
    }
  }else{
    ags_recall_done(recall);
  }

  g_list_free(note_start);
}

/**
 * ags_feed_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFeedAudioSignal
 *
 * Returns: the new #AgsFeedAudioSignal
 *
 * Since: 2.0.0
 */
AgsFeedAudioSignal*
ags_feed_audio_signal_new(AgsAudioSignal *source)
{
  AgsFeedAudioSignal *feed_audio_signal;

  feed_audio_signal = (AgsFeedAudioSignal *) g_object_new(AGS_TYPE_FEED_AUDIO_SIGNAL,
							  "source", source,
							  NULL);

  return(feed_audio_signal);
}
