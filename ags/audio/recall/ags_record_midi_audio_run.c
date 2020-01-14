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

#include <ags/audio/recall/ags_record_midi_audio_run.h>

#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>

#include <ags/audio/recall/ags_record_midi_audio.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/midi/ags_midi_util.h>
#include <ags/audio/midi/ags_midi_file.h>

#include <ags/i18n.h>

void ags_record_midi_audio_run_class_init(AgsRecordMidiAudioRunClass *record_midi_audio_run);
void ags_record_midi_audio_run_init(AgsRecordMidiAudioRun *record_midi_audio_run);
void ags_record_midi_audio_run_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_record_midi_audio_run_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_record_midi_audio_run_dispose(GObject *gobject);
void ags_record_midi_audio_run_finalize(GObject *gobject);

void ags_record_midi_audio_run_resolve_dependency(AgsRecall *recall);
void ags_record_midi_audio_run_run_init_pre(AgsRecall *recall);
void ags_record_midi_audio_run_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_record_midi_audio_run
 * @short_description: record midi
 * @title: AgsRecordMidiAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_record_midi_audio_run.h
 *
 * The #AgsRecordMidiAudioRun does record midi.
 */

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_COUNT_BEATS_AUDIO_RUN,
  PROP_TIMESTAMP,
};

static gpointer ags_record_midi_audio_run_parent_class = NULL;

GType
ags_record_midi_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_record_midi_audio_run = 0;

    static const GTypeInfo ags_record_midi_audio_run_info = {
      sizeof (AgsRecordMidiAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_record_midi_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecordMidiAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_record_midi_audio_run_init,
    };

    ags_type_record_midi_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							    "AgsRecordMidiAudioRun",
							    &ags_record_midi_audio_run_info,
							    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_record_midi_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_record_midi_audio_run_class_init(AgsRecordMidiAudioRunClass *record_midi_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_record_midi_audio_run_parent_class = g_type_class_peek_parent(record_midi_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) record_midi_audio_run;

  gobject->set_property = ags_record_midi_audio_run_set_property;
  gobject->get_property = ags_record_midi_audio_run_get_property;

  gobject->dispose = ags_record_midi_audio_run_dispose;
  gobject->finalize = ags_record_midi_audio_run_finalize;

  /* properties */
  /**
   * AgsRecordMidiAudioRun:delay-audio-run:
   * 
   * The delay audio run dependency.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("the AgsDelayAudioRun which emits midi_alloc_input signal"),
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRecordMidiAudioRun:count-beats-audio-run:
   * 
   * The count beats audio run dependency.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("count-beats-audio-run",
				   i18n_pspec("assigned AgsCountBeatsAudioRun"),
				   i18n_pspec("the AgsCountBeatsAudioRun which just counts"),
				   AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_COUNT_BEATS_AUDIO_RUN,
				  param_spec);

  /**
   * AgsRecordMidiAudioRun:timestamp:
   * 
   * The timestamp.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("assigned timestamp"),
				   i18n_pspec("the timestamp"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) record_midi_audio_run;

  recall->resolve_dependency = ags_record_midi_audio_run_resolve_dependency;
  recall->run_init_pre = ags_record_midi_audio_run_run_init_pre;
  recall->run_pre = ags_record_midi_audio_run_run_pre;
}

void
ags_record_midi_audio_run_init(AgsRecordMidiAudioRun *record_midi_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) record_midi_audio_run, (AGS_SOUND_ABILITY_NOTATION));

  AGS_RECALL(record_midi_audio_run)->name = "ags-record-midi";
  AGS_RECALL(record_midi_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(record_midi_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(record_midi_audio_run)->xml_type = "ags-record-midi-audio-run";
  AGS_RECALL(record_midi_audio_run)->port = NULL;

  record_midi_audio_run->delay_audio_run = NULL;
  record_midi_audio_run->count_beats_audio_run = NULL;

  record_midi_audio_run->note = NULL;

  record_midi_audio_run->timestamp = ags_timestamp_new();

  record_midi_audio_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  record_midi_audio_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  record_midi_audio_run->timestamp->timer.ags_offset.offset = 0;

  record_midi_audio_run->midi_file = NULL;
}

void
ags_record_midi_audio_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsRecordMidiAudioRun *record_midi_audio_run;

  GRecMutex *recall_mutex;
  
  record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(record_midi_audio_run);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *delay_audio_run, *old_delay_audio_run;

      gboolean is_template;

      delay_audio_run = g_value_get_object(value);
      old_delay_audio_run = NULL;
      
      g_rec_mutex_lock(recall_mutex);

      if(delay_audio_run == record_midi_audio_run->delay_audio_run){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(record_midi_audio_run->delay_audio_run != NULL){
	old_delay_audio_run = record_midi_audio_run->delay_audio_run;

	g_object_unref(G_OBJECT(record_midi_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(delay_audio_run);
      }

      g_rec_mutex_unlock(recall_mutex);

      /* check template */
      if(delay_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) record_midi_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      /* old - dependency/connection */
      if(is_template){
	if(old_delay_audio_run != NULL){
	  AgsRecallDependency *recall_dependency;

	  GList *list;
	  
	  recall_dependency = NULL;
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(record_midi_audio_run)->recall_dependency,
						       (GObject *) old_delay_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(record_midi_audio_run),
					      recall_dependency);
	}
      }

      /* new - dependency/connection */
      g_rec_mutex_lock(recall_mutex);

      record_midi_audio_run->delay_audio_run = delay_audio_run;

      g_rec_mutex_unlock(recall_mutex);

      if(delay_audio_run != NULL){
	if(is_template){
	  ags_recall_add_recall_dependency(AGS_RECALL(record_midi_audio_run),
					   ags_recall_dependency_new((GObject *) delay_audio_run));
	}
      }
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      AgsCountBeatsAudioRun *count_beats_audio_run, *old_count_beats_audio_run;

      gboolean is_template;

      count_beats_audio_run = g_value_get_object(value);
      old_count_beats_audio_run = NULL;
      
      g_rec_mutex_lock(recall_mutex);

      if(count_beats_audio_run == record_midi_audio_run->count_beats_audio_run){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if((AGS_RECALL_TEMPLATE & (AGS_RECALL(record_midi_audio_run)->flags)) != 0){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      if(record_midi_audio_run->count_beats_audio_run != NULL){
	old_count_beats_audio_run = record_midi_audio_run->count_beats_audio_run;

	g_object_unref(G_OBJECT(record_midi_audio_run->count_beats_audio_run));
      }

      if(count_beats_audio_run != NULL){
	g_object_ref(count_beats_audio_run);
      }

      record_midi_audio_run->count_beats_audio_run = count_beats_audio_run;

      g_rec_mutex_unlock(recall_mutex);

      /* check template */
      if(count_beats_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) record_midi_audio_run, AGS_RECALL_TEMPLATE)){
	is_template = TRUE;
      }else{
	is_template = FALSE;
      }

      /* dependency - remove */
      if(is_template){
	if(old_count_beats_audio_run != NULL){
	  AgsRecallDependency *recall_dependency;

	  GList *list;
	  
	  recall_dependency = NULL;
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(record_midi_audio_run)->recall_dependency,
						       (GObject *) old_count_beats_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(record_midi_audio_run),
					      recall_dependency);
	}
      }

      /* dependency - add */
      if(is_template &&
	 count_beats_audio_run != NULL){
	ags_recall_add_recall_dependency(AGS_RECALL(record_midi_audio_run),
					 ags_recall_dependency_new((GObject *) count_beats_audio_run));
      }
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = g_value_get_object(value);
      
      g_rec_mutex_lock(recall_mutex);

      if(timestamp == record_midi_audio_run->timestamp){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(record_midi_audio_run->timestamp != NULL){
	g_object_unref(G_OBJECT(record_midi_audio_run->timestamp));
      }

      if(timestamp != NULL){
	g_object_ref(timestamp);
      }

      record_midi_audio_run->timestamp = timestamp;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_record_midi_audio_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsRecordMidiAudioRun *record_midi_audio_run;
  
  GRecMutex *recall_mutex;
  
  record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(record_midi_audio_run);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(record_midi_audio_run->delay_audio_run));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_COUNT_BEATS_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(record_midi_audio_run->count_beats_audio_run));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, G_OBJECT(record_midi_audio_run->timestamp));

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_record_midi_audio_run_dispose(GObject *gobject)
{
  AgsRecordMidiAudioRun *record_midi_audio_run;

  record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(gobject);

  /* delay audio run */
  if(record_midi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(record_midi_audio_run->delay_audio_run));

    record_midi_audio_run->delay_audio_run = NULL;
  }

  /* count beats audio run */
  if(record_midi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(record_midi_audio_run->count_beats_audio_run));

    record_midi_audio_run->count_beats_audio_run = NULL;
  }

  g_list_free_full(record_midi_audio_run->note,
		   g_object_unref);
  record_midi_audio_run->note = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_record_midi_audio_run_parent_class)->dispose(gobject);
}

void
ags_record_midi_audio_run_finalize(GObject *gobject)
{
  AgsRecordMidiAudioRun *record_midi_audio_run;

  record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(gobject);

  /* delay audio run */
  if(record_midi_audio_run->delay_audio_run != NULL){
    g_object_unref(G_OBJECT(record_midi_audio_run->delay_audio_run));
  }

  /* count beats audio run */
  if(record_midi_audio_run->count_beats_audio_run != NULL){
    g_object_unref(G_OBJECT(record_midi_audio_run->count_beats_audio_run));
  }

  /* timestamp */
  if(record_midi_audio_run->timestamp != NULL){
    g_object_unref(G_OBJECT(record_midi_audio_run->timestamp));
  }

  g_list_free_full(record_midi_audio_run->note,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_record_midi_audio_run_parent_class)->finalize(gobject);
}

void
ags_record_midi_audio_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallID *recall_id;
  AgsRecallContainer *recall_container;
  AgsRecallDependency *recall_dependency;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GList *list_start, *list;

  guint i, i_stop;

  g_object_get(recall,
	       "recall-id", &recall_id,
	       "recall-container", &recall_container,
	       NULL);

  g_object_get(recall_container,
	       "recall-audio-run", &list_start,
	       NULL);

  template = NULL;
  list = ags_recall_find_template(list_start);

  if(list != NULL){
    template = AGS_RECALL(list->data);
  }

  g_list_free_full(list_start,
		   g_object_unref);

  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);

  list = list_start;

  delay_audio_run = NULL;
  count_beats_audio_run = NULL;

  i_stop = 2;

  for(i = 0; i < i_stop && list != NULL;){
    GObject *dependency;
    
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);
    
    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);
    
    if(AGS_IS_DELAY_AUDIO_RUN(dependency)){
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency,
									   recall_id);
      
      i++;
    }else if(AGS_IS_COUNT_BEATS_AUDIO_RUN(dependency)){
      count_beats_audio_run = (AgsCountBeatsAudioRun *) ags_recall_dependency_resolve(recall_dependency,
										      recall_id);

      i++;
    }

    g_object_unref(dependency);
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  g_object_set(G_OBJECT(recall),
	       "delay-audio-run", delay_audio_run,
	       "count-beats-audio-run", count_beats_audio_run,
	       NULL);

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(recall_container);
}

void
ags_record_midi_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsPort *port;
  AgsRecordMidiAudio *record_midi_audio;
  AgsRecordMidiAudioRun *record_midi_audio_run;
  
  GObject *input_sequencer;

  gchar *filename;
  
  gboolean playback, record;

  GValue value = {0,};

  void (*parent_class_run_init_pre)(AgsRecall *recall);
  
  record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(recall);

  /* get parent class */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_record_midi_audio_run_parent_class)->run_init_pre;

  /* get some fields */
  g_object_get(record_midi_audio_run,
	       "audio", &audio,
	       "recall-audio", &record_midi_audio,
	       NULL);

  g_object_get(audio,
	       "input-sequencer", &input_sequencer,
	       NULL);
  
  if(input_sequencer == NULL){
    g_object_unref(audio);
    
    g_object_unref(record_midi_audio);

    return;
  }
  
  /* midi file */
  if(record_midi_audio_run->midi_file != NULL){
    g_object_unref(record_midi_audio_run->midi_file);
  }

  /* get filename */
  g_object_get(record_midi_audio,
	       "filename", &port,
	       NULL);

  g_value_init(&value,
	       G_TYPE_POINTER);

  ags_port_safe_read(port,
		     &value);

  filename = g_value_get_pointer(&value);

  g_value_unset(&value);
  g_object_unref(port);  

  /* instantiate midi file and open rw */
  record_midi_audio_run->midi_file = (GObject *) ags_midi_file_new(filename);
  
  ags_midi_file_rw_open((AgsMidiFile *) record_midi_audio_run->midi_file,
			filename,
			TRUE);

  /* call parent */
  parent_class_run_init_pre(recall);

  /* unref */
  g_object_unref(audio);
    
  g_object_unref(record_midi_audio);

  g_free(filename);  
}

void
ags_record_midi_audio_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsNotation *notation;
  AgsPort *port;
  AgsRecordMidiAudio *record_midi_audio;
  AgsRecordMidiAudioRun *record_midi_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;

  AgsTimestamp *timestamp;
  
  GObject *input_sequencer;

  GList *start_list, *list;
  GList *note, *note_next;
  
  unsigned char *midi_buffer;

  gdouble delay_factor;
  glong division, tempo, bpm;
  guint64 notation_counter;
  gboolean reverse_mapping;
  gboolean pattern_mode;
  gboolean playback, record;
  guint midi_channel;
  guint audio_start_mapping;
  guint midi_start_mapping, midi_end_mapping;
  guint input_pads;
  guint audio_channel;
  guint buffer_length;
  guint i;
  
  GValue value = {0,};

  void (*parent_class_run_pre)(AgsRecall *recall);

  GRecMutex *audio_mutex;

  record_midi_audio_run = AGS_RECORD_MIDI_AUDIO_RUN(recall);

  /* get parent class */
  parent_class_run_pre = AGS_RECALL_CLASS(ags_record_midi_audio_run_parent_class)->run_pre;

  /* get some fields */
  g_object_get(record_midi_audio_run,
	       "audio-channel", &audio_channel,
	       "audio", &audio,
	       "recall-audio", &record_midi_audio,
	       "delay-audio-run", &delay_audio_run,
	       "count-beats-audio-run", &count_beats_audio_run,
	       "timestamp", &timestamp,
	       NULL);

  g_object_get(audio,
	       "input-sequencer", &input_sequencer,
	       NULL);
  
  if(input_sequencer == NULL){
    g_object_unref(audio);
    
    g_object_unref(record_midi_audio);

    g_object_unref(delay_audio_run);

    g_object_unref(count_beats_audio_run);

    g_object_unref(timestamp);

    return;
  }

  delay_factor = ags_sequencer_get_delay_factor(AGS_SEQUENCER(input_sequencer));
  
  /* get audio fields */
  pattern_mode = ags_audio_test_behaviour_flags(audio,
						AGS_SOUND_BEHAVIOUR_PATTERN_MODE);

  reverse_mapping = ags_audio_test_behaviour_flags(audio,
						   AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING);

  g_object_get(audio,
	       "input-pads", &input_pads,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       "midi-end-mapping", &midi_end_mapping,
	       "midi-channel", &midi_channel,
	       "notation", &start_list,
	       NULL);

  /* get sequencer specific data */
  bpm = ags_sequencer_get_bpm(AGS_SEQUENCER(input_sequencer));

  /* get notation */
  g_object_get(count_beats_audio_run,
	       "notation-counter", &notation_counter,
	       NULL);

  ags_timestamp_set_ags_offset(timestamp,
			       AGS_NOTATION_DEFAULT_OFFSET * floor(notation_counter / AGS_NOTATION_DEFAULT_OFFSET));
  
  notation = NULL;
  list = ags_notation_find_near_timestamp(start_list, audio_channel,
					  timestamp);

  if(list != NULL){
    notation = list->data;
  }
  
  /* get mode */
  g_object_get(record_midi_audio,
	       "playback", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(port,
		     &value);

  playback = g_value_get_boolean(&value);

  g_value_reset(&value);

  g_object_unref(port);
  
  g_object_get(record_midi_audio,
	       "record", &port,
	       NULL);

  ags_port_safe_read(port,
		     &value);

  record = g_value_get_boolean(&value);

  g_object_unref(port);
  
  /* delta time specific fields */
  g_value_unset(&value);

  g_value_init(&value,
	       G_TYPE_INT64);

  g_object_get(record_midi_audio,
	       "division", &port,
	       NULL);

  ags_port_safe_read(port,
		     &value);

  division = g_value_get_int64(&value);
  
  g_value_reset(&value);

  g_object_unref(port);
    
  ags_port_safe_read(record_midi_audio->tempo,
		     &value);

  tempo = g_value_get_int64(&value);

  g_value_reset(&value);

  g_object_get(record_midi_audio,
	       "bpm", &port,
	       NULL);
  
  ags_port_safe_read(port,
		     &value);

  bpm = g_value_get_int64(&value);

  g_value_unset(&value);
  g_object_unref(port);
  
  /* retrieve buffer */
  midi_buffer = ags_sequencer_get_buffer(AGS_SEQUENCER(input_sequencer),
					 &buffer_length);

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* playback */
  ags_sequencer_lock_buffer(AGS_SEQUENCER(input_sequencer),
			    midi_buffer);
  
  if(midi_buffer != NULL){
    if(playback){
      unsigned char *midi_iter;
      
      /* parse bytes */
      midi_iter = midi_buffer;
      
      while(midi_iter < midi_buffer + buffer_length){

#ifdef AGS_DEBUG
	g_message("0x%x", *midi_iter);
#endif
	
	if(ags_midi_util_is_key_on(midi_iter)){
	  AgsNote *current_note;
	  
	  /* key on - check within mapping */
	  if(midi_channel == (0x0f & midi_iter[0])){
	    if(midi_start_mapping <= (0x7f & midi_iter[1]) &&
	       ((reverse_mapping &&
		 input_pads - ((0x7f & midi_iter[1]) - midi_start_mapping) - 1 > 0) ||
		(!reverse_mapping &&
		 (0x7f & midi_iter[1]) - midi_start_mapping < midi_end_mapping))){
	      current_note = NULL;
	      note = record_midi_audio_run->note;

	      while(note != NULL){
		guint note_y;

		g_object_get(note->data,
			     "y", &note_y,
			     NULL);
		
		/* check current notes */
		if(reverse_mapping){
		  if(note_y == input_pads - ((0x7f & midi_iter[1]) - midi_start_mapping) - 1){
		    current_note = note->data;

		    break;
		  }
		}else{
		  if(note_y == (0x7f & midi_iter[1]) - midi_start_mapping){
		    current_note = note->data;

		    break;
		  }
		}
	    
		note = note->next;
	      }

	      /* add note */
	      if(current_note == NULL){
		if((0x7f & (midi_iter[2])) != 0){
		  current_note = ags_note_new();
	    
		  current_note->x[0] = notation_counter;
		  current_note->x[1] = notation_counter + 1;
	      
		  if(reverse_mapping){
		    current_note->y = input_pads - ((0x7f & midi_iter[1]) - midi_start_mapping) - 1;
		  }else{
		    current_note->y = (0x7f & midi_iter[1]) - midi_start_mapping;
		  }

		  /* velocity */
#if 0
		  current_note->attack.imag = (gdouble) (0x7f & (midi_iter[2])) / 127.0;
#endif
		  
#ifdef AGS_DEBUG
		  g_message("add %d", current_note->y);
#endif
		
		  if(!pattern_mode){
		    record_midi_audio_run->note = g_list_prepend(record_midi_audio_run->note,
								 current_note);
		    g_object_ref(current_note);

		    ags_note_set_flags(current_note,
				       AGS_NOTE_FEED);
		  }

		  if(notation == NULL){
		    notation = ags_notation_new((GObject *) audio,
						audio_channel);

		    ags_timestamp_set_ags_offset(notation->timestamp,
						 ags_timestamp_get_ags_offset(timestamp));


		    ags_audio_add_notation(audio,
					   (GObject *) notation);
		  }
		  
		  ags_notation_add_note(notation,
					current_note,
					FALSE);
		}
	      }else{
		if((0x7f & (midi_iter[2])) == 0){
		  /* note-off */
		  ags_note_unset_flags(current_note,
				       AGS_NOTE_FEED);

		  /* velocity */
#if 0		  
		  current_note->release.imag = (gdouble) (0x7f & (midi_iter[2])) / 127.0;
#endif
		  
		  record_midi_audio_run->note = g_list_remove(record_midi_audio_run->note,
							      current_note);
		  g_object_unref(current_note);

#ifdef AGS_DEBUG
		  g_message("remove %d", current_note->y);
#endif
		}else{
		  g_object_set(current_note,
			       "x1", notation_counter + 1,
			       NULL);

#ifdef AGS_DEBUG
		  g_message("count %d", current_note->y);
#endif
 		}
	      }
	    }
	  }
	  
	  midi_iter += 3;
	}else if(ags_midi_util_is_key_off(midi_iter)){
	  AgsNote *current_note;

	  if(midi_channel == (0x0f & midi_iter[0])){
	    /* key off - find matching note */
	    current_note = NULL;
	    note = record_midi_audio_run->note;
	    
	    while(note != NULL){
	      guint note_y;

	      g_object_get(note->data,
			   "y", &note_y,
			   NULL);

	      /* check current notes */
	      if(reverse_mapping){
		if(note_y == input_pads - ((0x7f & midi_iter[1]) - midi_start_mapping) - 1){
		  current_note = note->data;

		  break;
		}
	      }else{
		if(note_y == (0x7f & midi_iter[1]) - midi_start_mapping){
		  current_note = note->data;

		  break;
		}
	      }
	    
	      note = note->next;
	    }
	    
	    /* remove current note */
	    if(current_note != NULL){
	      /* velocity */
#if 0
	      current_note->release.imag = (gdouble) (0x7f & (midi_iter[2])) / 127.0;
#endif
	      
	      ags_note_unset_flags(current_note,
				   AGS_NOTE_FEED);
	      
	      record_midi_audio_run->note = g_list_remove(record_midi_audio_run->note,
							  current_note);
	      g_object_unref(current_note);

#ifdef AGS_DEBUG
	      g_message("remove %d", current_note->y);
#endif	    
	    }
	  }
	  
	  midi_iter += 3;
	}else if(ags_midi_util_is_key_pressure(midi_iter)){
	  AgsNote *current_note;

	  if(midi_channel == (0x0f & midi_iter[0])){
	    /* key pressure */
	    current_note = NULL;
	    note = record_midi_audio_run->note;

	    while(note != NULL){
	      guint note_y;

	      g_object_get(note->data,
			   "y", &note_y,
			   NULL);

	      /* check current notes */
	      if(reverse_mapping){
		if(note_y == input_pads - ((0x7f & midi_iter[1]) - midi_start_mapping) - 1){
		  current_note = note->data;

		  break;
		}
	      }else{
		if(note_y == (0x7f & midi_iter[1]) - midi_start_mapping){
		  current_note = note->data;

		  break;
		}
	      }
	    
	      note = note->next;
	    }

	    /* feed note */
	    if(current_note != NULL){
	      g_object_set(current_note,
			   "x1", notation_counter + 1,
			   NULL);

#ifdef AGS_DEBUG
	      g_message("count %d", current_note->y);
#endif
	    }
	  }
	  
	  midi_iter += 3;
	}else if(ags_midi_util_is_change_parameter(midi_iter)){
	  /* change parameter */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 3;
	}else if(ags_midi_util_is_pitch_bend(midi_iter)){
	  /* change parameter */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 3;
	}else if(ags_midi_util_is_change_program(midi_iter)){
	  /* change program */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 2;
	}else if(ags_midi_util_is_change_pressure(midi_iter)){
	  /* change pressure */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 2;
	}else if(ags_midi_util_is_sysex(midi_iter)){
	  guint n;
	  
	  /* sysex */
	  n = 0;
	  
	  while(midi_iter[n] != 0xf7){
	    n++;
	  }

	  //TODO:JK: implement me	  
	  
	  midi_iter += (n + 1);
	}else if(ags_midi_util_is_song_position(midi_iter)){
	  /* song position */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 3;
	}else if(ags_midi_util_is_song_select(midi_iter)){
	  /* song select */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 2;
	}else if(ags_midi_util_is_tune_request(midi_iter)){
	  /* tune request */
	  //TODO:JK: implement me	  
	  
	  midi_iter += 1;
	}else if(ags_midi_util_is_meta_event(midi_iter)){
	  /* meta event */
	  //TODO:JK: implement me	  
	  
	  midi_iter += (3 + midi_iter[2]);
	}else{
	  g_warning("ags_record_midi_audio_run.c - unexpected byte %x", midi_iter[0]);
	  
	  midi_iter++;
	}
      }
      
      /* feed notes */
      note = record_midi_audio_run->note;

      while(note != NULL){
	AgsNote *current_note;
	
	current_note = note->data;

	g_object_set(current_note,
		     "x1", notation_counter + 1,
		     NULL);
#ifdef AGS_DEBUG
	g_message("count %d", current_note->y);
#endif
	note = note->next;
      }
    }

    /* record */
    if(record){
      unsigned char *smf_buffer;

      glong delta_time;
      guint smf_buffer_length;

      delta_time = ags_midi_util_offset_to_delta_time(delay_factor,
						      division,
						      tempo,
						      bpm,
						      notation_counter);
      
      smf_buffer = ags_midi_util_to_smf(midi_buffer, buffer_length,
					delta_time,
					&smf_buffer_length);
      
      ags_midi_file_write((AgsMidiFile *) record_midi_audio_run->midi_file,
			  smf_buffer, smf_buffer_length);
    }
  }

  ags_sequencer_unlock_buffer(AGS_SEQUENCER(input_sequencer),
			      midi_buffer);
  
  /* call parent */
  parent_class_run_pre(recall);

  /* unref */
  g_object_unref(audio);
    
  g_object_unref(record_midi_audio);

  g_object_unref(delay_audio_run);

  g_object_unref(count_beats_audio_run);

  g_object_unref(timestamp);

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_record_midi_audio_run_new:
 * @audio: the #AgsAudio
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun dependency
 *
 * Create a new instance of #AgsRecordMidiAudioRun
 *
 * Returns: the new #AgsRecordMidiAudioRun
 *
 * Since: 3.0.0
 */
AgsRecordMidiAudioRun*
ags_record_midi_audio_run_new(AgsAudio *audio,
			      AgsDelayAudioRun *delay_audio_run,
			      AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsRecordMidiAudioRun *record_midi_audio_run;

  record_midi_audio_run = (AgsRecordMidiAudioRun *) g_object_new(AGS_TYPE_RECORD_MIDI_AUDIO_RUN,
								 "audio", audio,
								 "delay-audio-run", delay_audio_run,
								 "count-beats-audio-run", count_beats_audio_run,
								 NULL);

  return(record_midi_audio_run);
}
