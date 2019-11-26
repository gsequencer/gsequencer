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

#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_stream_channel_run.h>

#include <ags/audio/task/ags_cancel_audio.h>

#include <math.h>

#include <ags/i18n.h>

void ags_count_beats_audio_run_class_init(AgsCountBeatsAudioRunClass *count_beats_audio_run);
void ags_count_beats_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_count_beats_audio_run_seekable_interface_init(AgsSeekableInterface *seekable);
void ags_count_beats_audio_run_countable_interface_init(AgsCountableInterface *countable);
void ags_count_beats_audio_run_tactable_interface_init(AgsTactableInterface *tactable);
void ags_count_beats_audio_run_init(AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_set_property(GObject *gobject,
					    guint prop_id,
					    const GValue *value,
					    GParamSpec *param_spec);
void ags_count_beats_audio_run_get_property(GObject *gobject,
					    guint prop_id,
					    GValue *value,
					    GParamSpec *param_spec);
void ags_count_beats_audio_run_dispose(GObject *gobject);
void ags_count_beats_audio_run_finalize(GObject *gobject);

void ags_count_beats_audio_run_connect(AgsConnectable *connectable);
void ags_count_beats_audio_run_disconnect(AgsConnectable *connectable);
void ags_count_beats_audio_run_connect_connection(AgsConnectable *connectable,
						  GObject *connection);
void ags_count_beats_audio_run_disconnect_connection(AgsConnectable *connectable,
						     GObject *connection);

void ags_count_beats_audio_run_seek(AgsSeekable *seekable,
				    gint64 offset,
				    guint whence);

guint64 ags_count_beats_audio_run_get_sequencer_counter(AgsCountable *countable);
guint64 ags_count_beats_audio_run_get_notation_counter(AgsCountable *countable);
guint64 ags_count_beats_audio_run_get_wave_counter(AgsCountable *countable);
guint64 ags_count_beats_audio_run_get_midi_counter(AgsCountable *countable);

void ags_count_beats_audio_run_notify_output_soundcard_callback(GObject *gobject,
								GParamSpec *pspec,
								gpointer user_data);

void ags_count_beats_audio_run_resolve_dependency(AgsRecall *recall);
void ags_count_beats_audio_run_notify_dependency(AgsRecall *recall,
						 guint dependency, gboolean increase);
void ags_count_beats_audio_run_run_init_pre(AgsRecall *recall);
void ags_count_beats_audio_run_done(AgsRecall *recall);

gdouble ags_count_beats_audio_run_get_bpm(AgsTactable *tactable);
gdouble ags_count_beats_audio_run_get_tact(AgsTactable *tactable);
void ags_count_beats_audio_run_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);
void ags_count_beats_audio_run_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact);

void ags_count_beats_audio_run_notation_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							      guint nth_run, gdouble delay, guint attack,
							      AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_notation_count_callback(AgsDelayAudioRun *delay_audio_run,
						       guint nth_run, gdouble delay, guint attack,
						       AgsCountBeatsAudioRun *count_beats_audio_run);

void ags_count_beats_audio_run_sequencer_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							       guint nth_run, gdouble delay, guint attack,
							       AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run,
							guint nth_run, gdouble delay, guint attack,
							AgsCountBeatsAudioRun *count_beats_audio_run);

void ags_count_beats_audio_run_wave_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							  guint nth_run, gdouble delay, guint attack,
							  AgsCountBeatsAudioRun *count_beats_audio_run);
void ags_count_beats_audio_run_wave_count_callback(AgsDelayAudioRun *delay_audio_run,
						   guint nth_run, gdouble delay, guint attack,
						   AgsCountBeatsAudioRun *count_beats_audio_run);

/**
 * SECTION:ags_count_beats_audio_run
 * @short_description: count beats
 * @title: AgsCountBeatsAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_count_beats_audio_run.h
 *
 * The #AgsCountBeatsAudioRun class count beats.
 */

enum{
  SEQUENCER_LOOP,
  SEQUENCER_START,
  SEQUENCER_STOP,
  NOTATION_LOOP,
  NOTATION_START,
  NOTATION_STOP,
  WAVE_LOOP,
  WAVE_START,
  WAVE_STOP,
  MIDI_LOOP,
  MIDI_START,
  MIDI_STOP,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_DELAY_AUDIO_RUN,
  PROP_NOTATION_COUNTER,
  PROP_SEQUENCER_COUNTER,
  PROP_WAVE_COUNTER,
  PROP_MIDI_COUNTER,
};

static gpointer ags_count_beats_audio_run_parent_class = NULL;
static AgsConnectableInterface* ags_count_beats_audio_run_parent_connectable_interface;

static guint count_beats_audio_run_signals[LAST_SIGNAL];

GType
ags_count_beats_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_count_beats_audio_run = 0;

    static const GTypeInfo ags_count_beats_audio_run_info = {
      sizeof (AgsCountBeatsAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_count_beats_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCountBeatsAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_count_beats_audio_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_seekable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_seekable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_countable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_countable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_run_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_count_beats_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							    "AgsCountBeatsAudioRun",
							    &ags_count_beats_audio_run_info,
							    0);
    
    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_COUNTABLE,
				&ags_countable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_SEEKABLE,
				&ags_seekable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio_run,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_count_beats_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_count_beats_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_count_beats_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_count_beats_audio_run_connect;
  connectable->disconnect = ags_count_beats_audio_run_disconnect;

  connectable->connect_connection = ags_count_beats_audio_run_connect_connection;
  connectable->disconnect_connection = ags_count_beats_audio_run_disconnect_connection;
}

void
ags_count_beats_audio_run_seekable_interface_init(AgsSeekableInterface *seekable)
{
  seekable->seek = ags_count_beats_audio_run_seek;
}

void
ags_count_beats_audio_run_countable_interface_init(AgsCountableInterface *countable)
{
  countable->get_sequencer_counter = ags_count_beats_audio_run_get_sequencer_counter;
  countable->get_notation_counter = ags_count_beats_audio_run_get_notation_counter;
  countable->get_wave_counter = ags_count_beats_audio_run_get_wave_counter;
  countable->get_midi_counter = ags_count_beats_audio_run_get_midi_counter;
}

void
ags_count_beats_audio_run_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->get_bpm = ags_count_beats_audio_run_get_bpm;
  tactable->get_tact = ags_count_beats_audio_run_get_tact;

  tactable->get_sequencer_duration = NULL;
  tactable->get_notation_duration = NULL;
  tactable->get_wave_duration = NULL;
  tactable->get_midi_duration = NULL;

  tactable->change_sequencer_duration = NULL;
  tactable->change_notation_duration = NULL;
  tactable->change_wave_duration = NULL;
  tactable->change_midi_duration = NULL;

  tactable->change_bpm = ags_count_beats_audio_run_change_bpm;
  tactable->change_tact = ags_count_beats_audio_run_change_tact;
}

void
ags_count_beats_audio_run_class_init(AgsCountBeatsAudioRunClass *count_beats_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_count_beats_audio_run_parent_class = g_type_class_peek_parent(count_beats_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) count_beats_audio_run;

  gobject->set_property = ags_count_beats_audio_run_set_property;
  gobject->get_property = ags_count_beats_audio_run_get_property;

  gobject->dispose = ags_count_beats_audio_run_dispose;
  gobject->finalize = ags_count_beats_audio_run_finalize;

  /* properties */
  /**
   * AgsCountBeatsAudioRun:delay-audio-run:
   *
   * The assigned #AgsDelayAudioRun dependency.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("delay-audio-run",
				   i18n_pspec("assigned AgsDelayAudioRun"),
				   i18n_pspec("The AgsDelayAudioRun which emits sequencer_alloc_output sequencer_count signal"),
				   AGS_TYPE_DELAY_AUDIO_RUN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO_RUN,
				  param_spec);

  /**
   * AgsCountBeatsAudioRun:sequencer-counter:
   *
   * The sequencer counter.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint64("sequencer-counter",
				   i18n_pspec("sequencer counter indicates offset"),
				   i18n_pspec("The sequenecer counter indicates the offset in the sequencer"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_COUNTER,
				  param_spec);
  
  /**
   * AgsCountBeatsAudioRun:notation-counter:
   *
   * The notation counter.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint64("notation-counter",
				   i18n_pspec("notation counter indicates offset"),
				   i18n_pspec("The notation counter indicates the offset in the notation"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_COUNTER,
				  param_spec);

  /**
   * AgsCountBeatsAudioRun:wave-counter:
   *
   * The wave counter.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint64("wave-counter",
				   i18n_pspec("wave counter indicates offset"),
				   i18n_pspec("The sequenecer counter indicates the offset in the wave"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_COUNTER,
				  param_spec);

  /**
   * AgsCountBeatsAudioRun:midi-counter:
   *
   * The midi counter.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint64("midi-counter",
				   i18n_pspec("midi counter indicates offset"),
				   i18n_pspec("The sequenecer counter indicates the offset in the midi"),
				   0,
				   G_MAXUINT64,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_COUNTER,
				  param_spec);
  
  /* AgsRecallClass */
  recall = (AgsRecallClass *) count_beats_audio_run;

  recall->resolve_dependency = ags_count_beats_audio_run_resolve_dependency;
  recall->notify_dependency = ags_count_beats_audio_run_notify_dependency;
  recall->run_init_pre = ags_count_beats_audio_run_run_init_pre;
  recall->done = ags_count_beats_audio_run_done;

  /* AgsCountBeatsAudioRunClass */
  count_beats_audio_run->notation_start = NULL;
  count_beats_audio_run->notation_loop = NULL;
  count_beats_audio_run->notation_stop = NULL;
  count_beats_audio_run->sequencer_start = NULL;
  count_beats_audio_run->sequencer_loop = NULL;
  count_beats_audio_run->sequencer_stop = NULL;

  /* signals */
  /**
   * AgsCountBeatsAudioRun::sequencer-start:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::sequencer-start signal is emited while starting sequencer playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[SEQUENCER_START] =
    g_signal_new("sequencer-start",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, sequencer_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::sequencer-loop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::sequencer-loop signal is emited while looping sequencer playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[SEQUENCER_LOOP] =
    g_signal_new("sequencer-loop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, sequencer_loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::sequencer-stop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::sequencer-loop signal is emited while stoping sequencer playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[SEQUENCER_STOP] =
    g_signal_new("sequencer-stop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, sequencer_stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::notation-start:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::notation-start signal is emited while starting notation playback.
   * 
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[NOTATION_START] =
    g_signal_new("notation-start",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, notation_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::notation-loop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::notation-loop signal is emited while looping notation playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[NOTATION_LOOP] =
    g_signal_new("notation-loop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, notation_loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::notation-stop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::notation-loop signal is emited while stoping notation playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[NOTATION_STOP] =
    g_signal_new("notation-stop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, notation_stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
  
  /**
   * AgsCountBeatsAudioRun::wave-start:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::wave-start signal is emited while starting wave playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[WAVE_START] =
    g_signal_new("wave-start",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, wave_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::wave-loop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::wave-loop signal is emited while looping wave playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[WAVE_LOOP] =
    g_signal_new("wave-loop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, wave_loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::wave-stop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::wave-loop signal is emited while stoping wave playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[WAVE_STOP] =
    g_signal_new("wave-stop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, wave_stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  
  /**
   * AgsCountBeatsAudioRun::midi-start:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::midi-start signal is emited while starting midi playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[MIDI_START] =
    g_signal_new("midi-start",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, midi_start),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::midi-loop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::midi-loop signal is emited while looping midi playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[MIDI_LOOP] =
    g_signal_new("midi-loop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, midi_loop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);

  /**
   * AgsCountBeatsAudioRun::midi-stop:
   * @count_beats_audio_run: the object
   * @nth_run: the nth run
   *
   * The ::midi-loop signal is emited while stoping midi playback.
   *
   * Since: 2.0.0
   */
  count_beats_audio_run_signals[MIDI_STOP] =
    g_signal_new("midi-stop",
		 G_TYPE_FROM_CLASS(count_beats_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCountBeatsAudioRunClass, midi_stop),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__UINT,
		 G_TYPE_NONE, 1,
		 G_TYPE_UINT);
}

void
ags_count_beats_audio_run_init(AgsCountBeatsAudioRun *count_beats_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) count_beats_audio_run, (AGS_SOUND_ABILITY_SEQUENCER |
								     AGS_SOUND_ABILITY_NOTATION |
								     AGS_SOUND_ABILITY_WAVE |
								     AGS_SOUND_ABILITY_MIDI));

  g_signal_connect_after(count_beats_audio_run, "notify::output-soundcard",
			 G_CALLBACK(ags_count_beats_audio_run_notify_output_soundcard_callback), NULL);

  AGS_RECALL(count_beats_audio_run)->name = "ags-count-beats";
  AGS_RECALL(count_beats_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(count_beats_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(count_beats_audio_run)->xml_type = "ags-count-beats-audio-run";
  AGS_RECALL(count_beats_audio_run)->port = NULL;

  count_beats_audio_run->sequencer_counter = 0;
  count_beats_audio_run->notation_counter = 0;
  count_beats_audio_run->wave_counter = 0;
  count_beats_audio_run->midi_counter = 0;

  count_beats_audio_run->recall_ref = 0;

  count_beats_audio_run->hide_ref = 0;
  count_beats_audio_run->sequencer_hide_ref_counter = 0;
  count_beats_audio_run->notation_hide_ref_counter = 0;
  count_beats_audio_run->wave_hide_ref_counter = 0;
  count_beats_audio_run->midi_hide_ref_counter = 0;

  count_beats_audio_run->delay_audio_run = NULL;
}

void
ags_count_beats_audio_run_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GRecMutex *recall_mutex;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      AgsDelayAudioRun *old_delay_audio_run, *delay_audio_run;

      gboolean is_template;

      delay_audio_run = (AgsDelayAudioRun *) g_value_get_object(value);
      old_delay_audio_run = NULL;
      
      g_rec_mutex_lock(recall_mutex);

      if(count_beats_audio_run->delay_audio_run == delay_audio_run){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio_run->delay_audio_run != NULL){
	old_delay_audio_run = count_beats_audio_run->delay_audio_run;
	
	g_object_unref(G_OBJECT(count_beats_audio_run->delay_audio_run));
      }

      if(delay_audio_run != NULL){
	g_object_ref(G_OBJECT(delay_audio_run));
      }

      g_rec_mutex_unlock(recall_mutex);

      /* check template */
      if(delay_audio_run != NULL &&
	 ags_recall_test_flags((AgsRecall *) delay_audio_run, AGS_RECALL_TEMPLATE)){
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
	  list = ags_recall_dependency_find_dependency(AGS_RECALL(count_beats_audio_run)->recall_dependency,
						       (GObject *) old_delay_audio_run);

	  if(list != NULL){
	    recall_dependency = list->data;
	  }
	  
	  ags_recall_remove_recall_dependency(AGS_RECALL(count_beats_audio_run),
					      recall_dependency);
	}
      }else{
	if(ags_connectable_is_connected(AGS_CONNECTABLE(count_beats_audio_run))){
	  ags_connectable_disconnect_connection(AGS_CONNECTABLE(count_beats_audio_run),
						(GObject *) old_delay_audio_run);
	}
      }

      /* new - dependency/connection */
      g_rec_mutex_lock(recall_mutex);

      count_beats_audio_run->delay_audio_run = delay_audio_run;

      g_rec_mutex_unlock(recall_mutex);

      if(delay_audio_run != NULL){
	if(is_template){
	  ags_recall_add_recall_dependency(AGS_RECALL(count_beats_audio_run),
					   ags_recall_dependency_new((GObject *) delay_audio_run));
	}else{
	  if(ags_connectable_is_connected(AGS_CONNECTABLE(count_beats_audio_run))){
	    ags_connectable_connect_connection(AGS_CONNECTABLE(count_beats_audio_run),
					       (GObject *) delay_audio_run);
	  }
	}
      }
    }
    break;
  case PROP_SEQUENCER_COUNTER:
    {
      guint64 counter;

      counter = g_value_get_uint64(value);

      g_rec_mutex_lock(recall_mutex);

      count_beats_audio_run->sequencer_counter = counter;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_COUNTER:
    {
      guint64 counter;

      counter = g_value_get_uint64(value);

      g_rec_mutex_lock(recall_mutex);

      count_beats_audio_run->notation_counter = counter;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_COUNTER:
    {
      guint64 counter;

      counter = g_value_get_uint64(value);

      g_rec_mutex_lock(recall_mutex);

      count_beats_audio_run->wave_counter = counter;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_COUNTER:
    {
      guint64 counter;

      counter = g_value_get_uint64(value);

      g_rec_mutex_lock(recall_mutex);

      count_beats_audio_run->midi_counter = counter;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_run_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  
  GRecMutex *recall_mutex;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  switch(prop_id){
  case PROP_DELAY_AUDIO_RUN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats_audio_run->delay_audio_run);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_COUNTER:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint64(value, count_beats_audio_run->sequencer_counter);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_COUNTER:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint64(value, count_beats_audio_run->notation_counter);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_COUNTER:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint64(value, count_beats_audio_run->wave_counter);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_COUNTER:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_uint64(value, count_beats_audio_run->midi_counter);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_run_dispose(GObject *gobject)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  /* delay audio run */
  if(count_beats_audio_run->delay_audio_run != NULL){
    g_object_unref(count_beats_audio_run->delay_audio_run);

    count_beats_audio_run->delay_audio_run = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_count_beats_audio_run_parent_class)->dispose(gobject);
}

void
ags_count_beats_audio_run_finalize(GObject *gobject)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  /* delay audio run */
  if(count_beats_audio_run->delay_audio_run != NULL){
    g_object_unref(count_beats_audio_run->delay_audio_run);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_count_beats_audio_run_parent_class)->finalize(gobject);
}

void
ags_count_beats_audio_run_connect(AgsConnectable *connectable)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  ags_count_beats_audio_run_parent_connectable_interface->connect(connectable);
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(connectable);

  g_object_get(count_beats_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_connect_connection(connectable,
				     (GObject *) delay_audio_run);  

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_count_beats_audio_run_disconnect(AgsConnectable *connectable)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsDelayAudioRun *delay_audio_run;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  ags_count_beats_audio_run_parent_connectable_interface->disconnect(connectable);
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(connectable);

  g_object_get(count_beats_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  ags_connectable_disconnect_connection(connectable,
					(GObject *) delay_audio_run);  

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_count_beats_audio_run_connect_connection(AgsConnectable *connectable,
					     GObject *connection)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(connectable);

  g_object_get(count_beats_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if((GObject *) delay_audio_run == connection){
    g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "wave-alloc-output",
		     G_CALLBACK(ags_count_beats_audio_run_wave_alloc_output_callback), count_beats_audio_run);

    g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "wave-count",
		     G_CALLBACK(ags_count_beats_audio_run_wave_count_callback), count_beats_audio_run);

    g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "sequencer-alloc-output",
		     G_CALLBACK(ags_count_beats_audio_run_sequencer_alloc_output_callback), count_beats_audio_run);

    g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "sequencer-count",
		     G_CALLBACK(ags_count_beats_audio_run_sequencer_count_callback), count_beats_audio_run);

    g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "notation-alloc-output",
		     G_CALLBACK(ags_count_beats_audio_run_notation_alloc_output_callback), count_beats_audio_run);

    g_signal_connect(G_OBJECT(count_beats_audio_run->delay_audio_run), "notation-count",
		     G_CALLBACK(ags_count_beats_audio_run_notation_count_callback), count_beats_audio_run);
  }

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_count_beats_audio_run_disconnect_connection(AgsConnectable *connectable,
						GObject *connection)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsDelayAudioRun *delay_audio_run;

  if(connection == NULL){
    return;
  }
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(connectable);

  g_object_get(count_beats_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  if((GObject *) delay_audio_run == connection){
    g_object_disconnect(delay_audio_run,
			"any_signal::wave-alloc-output",
			G_CALLBACK(ags_count_beats_audio_run_wave_alloc_output_callback),
			count_beats_audio_run,
			"any_signal::wave-count",
			G_CALLBACK(ags_count_beats_audio_run_wave_count_callback),
			count_beats_audio_run,
			"any_signal::sequencer-alloc-output",
			G_CALLBACK(ags_count_beats_audio_run_sequencer_alloc_output_callback),
			count_beats_audio_run,
			"any_signal::sequencer-count",
			G_CALLBACK(ags_count_beats_audio_run_sequencer_count_callback),
			count_beats_audio_run,
			"any_signal::notation-alloc-output",
			G_CALLBACK(ags_count_beats_audio_run_notation_alloc_output_callback),
			count_beats_audio_run,
			"any_signal::notation-count",
			G_CALLBACK(ags_count_beats_audio_run_notation_count_callback),
			count_beats_audio_run,
			NULL);
  }

  if(delay_audio_run != NULL){
    g_object_unref(delay_audio_run);
  }
}

void
ags_count_beats_audio_run_seek(AgsSeekable *seekable,
			       gint64 offset,
			       guint whence)
{
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsPort *sequencer_duration;
  
  guint64 duration;

  GValue value = {0,};
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(seekable);

  g_object_get(count_beats_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);

  g_object_get(delay_audio_run,
	       "recall-audio", &delay_audio,
	       NULL);

  /* sequencer */
  g_object_get(delay_audio,
	       "sequencer-duration", &sequencer_duration,
	       NULL);

  g_value_init(&value,
	       G_TYPE_UINT64);

  ags_port_safe_read(sequencer_duration,
		     &value);
  duration = g_value_get_uint64(&value);

  g_value_unset(&value);
  
  switch(whence){
  case AGS_SEEK_CUR:
    {
      guint64 sequencer_counter;
      guint64 notation_counter;
      guint64 wave_counter;
      guint64 midi_counter;
      
      g_object_get(count_beats_audio_run,
		   "sequencer-counter", &sequencer_counter,
		   "notation-counter", &notation_counter,
		   "wave-counter", &wave_counter,
		   "midi-counter", &midi_counter,
		   NULL);

      if(sequencer_counter + offset < 0){
	sequencer_counter = duration - ((offset - sequencer_counter) % duration);
      }else{
	sequencer_counter = (sequencer_counter + offset) % duration;
      }

      if(notation_counter + offset < 0){
	notation_counter = 0;
      }

      if(wave_counter + offset < 0){
	wave_counter = 0;
      }

      if(midi_counter + offset < 0){
	midi_counter = 0;
      }

      g_object_set(count_beats_audio_run,
		   "sequencer-counter", sequencer_counter,
		   "notation-counter", notation_counter,
		   "wave-counter", wave_counter,
		   "midi-counter", midi_counter,
		   NULL);
    }
    break;
  case AGS_SEEK_END:
    {
      AgsPort *notation_duration;
      AgsPort *midi_duration;
      AgsPort *wave_duration;

      guint64 sequencer_counter;
      guint64 notation_counter;
      guint64 wave_counter;
      guint64 midi_counter;

      g_object_get(delay_audio,
		   "notation-duration", &notation_duration,
		   "wave-duration", &wave_duration,
		   "midi-duration", &midi_duration,
		   NULL);

      /* sequencer */
      if(duration + offset < 0){
	sequencer_counter = duration - ((offset - duration) % duration);
      }else{
	sequencer_counter = (duration + offset) % duration;
      }
      
      /* notation */
      g_value_reset(&value);
      
      ags_port_safe_read(notation_duration,
			 &value);
      duration = g_value_get_uint64(&value);

      if(duration + offset < 0){
	notation_counter = 0;
      }else{
	notation_counter = (duration + offset);
      }
      
      /* wave */
      g_value_reset(&value);
      
      ags_port_safe_read(wave_duration,
			 &value);
      duration = g_value_get_uint64(&value);

      if(duration + offset < 0){
	wave_counter = 0;
      }else{
	wave_counter = (duration + offset);
      }

      /* midi */
      g_value_reset(&value);
      
      ags_port_safe_read(midi_duration,
			 &value);
      duration = g_value_get_uint64(&value);

      if(duration + offset < 0){
	midi_counter = 0;
      }else{
	midi_counter = (duration + offset);
      }

      g_object_set(count_beats_audio_run,
		   "sequencer-counter", sequencer_counter,
		   "notation-counter", notation_counter,
		   "wave-counter", wave_counter,
		   "midi-counter", midi_counter,
		   NULL);
    }
    break;
  case AGS_SEEK_SET:
    {
      guint64 sequencer_counter;

      g_object_set(count_beats_audio_run,
		   "sequencer-counter", offset % duration,
		   "notation-counter", offset,
		   "wave-counter", offset,
		   "midi-counter", offset,
		   NULL);
    }
    break;
  }

  g_object_unref(delay_audio_run);

  g_object_unref(delay_audio);
}

guint64
ags_count_beats_audio_run_get_sequencer_counter(AgsCountable *countable)
{
  guint64 sequencer_counter;

  g_object_get(G_OBJECT(countable),
	       "sequencer-counter", &sequencer_counter,
	       NULL);
  
  return(sequencer_counter);
}

guint64
ags_count_beats_audio_run_get_notation_counter(AgsCountable *countable)
{
  guint64 notation_counter;

  g_object_get(G_OBJECT(countable),
	       "notation-counter", &notation_counter,
	       NULL);

  return(notation_counter);
}

guint64
ags_count_beats_audio_run_get_wave_counter(AgsCountable *countable)
{
  guint64 wave_counter;

  g_object_get(G_OBJECT(countable),
	       "wave-counter", &wave_counter,
	       NULL);

  return(wave_counter);
}

guint64
ags_count_beats_audio_run_get_midi_counter(AgsCountable *countable)
{
  guint64 midi_counter;

  g_object_get(G_OBJECT(countable),
	       "midi-counter", &midi_counter,
	       NULL);

  return(midi_counter);
}

void
ags_count_beats_audio_run_notify_output_soundcard_callback(GObject *gobject,
							   GParamSpec *pspec,
							   gpointer user_data)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GObject *output_soundcard;
  
  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(gobject);

  output_soundcard = AGS_RECALL(count_beats_audio_run)->output_soundcard;

  if(output_soundcard == NULL){
    return;
  }
  
  /* bpm */
  count_beats_audio_run->bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));

  /* tact */
  count_beats_audio_run->tact = AGS_SOUNDCARD_DEFAULT_TACT;
}

void
ags_count_beats_audio_run_resolve_dependency(AgsRecall *recall)
{
  AgsRecall *template;
  AgsRecallContainer *recall_container;
  AgsRecallDependency *recall_dependency;
  AgsRecallID *recall_id;
  
  AgsDelayAudioRun *delay_audio_run;

  GObject *output_soundcard;
  
  GList *list_start, *list;

  guint i, i_stop;

  GRecMutex *recall_mutex;
  
  g_object_get(recall,
	       "output-soundcard", &output_soundcard,
	       NULL);
  
  if(output_soundcard == NULL){
    return;
  }

  /* get mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* get template */
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

  /* check recall dependency */
  g_object_get(template,
	       "recall-dependency", &list_start,
	       NULL);

  delay_audio_run = NULL;
  list = list_start;

  i_stop = 1;

  for(i = 0; i < i_stop && list != NULL;){
    GObject *dependency;
    
    recall_dependency = AGS_RECALL_DEPENDENCY(list->data);

    g_object_get(recall_dependency,
		 "dependency", &dependency,
		 NULL);
    
    if(AGS_IS_DELAY_AUDIO_RUN(dependency)){
      AgsPort *sequencer_duration;
      
      AgsDelayAudio *delay_audio;

      guint note_offset;

      GValue value = {0,};
      
      delay_audio_run = (AgsDelayAudioRun *) ags_recall_dependency_resolve(recall_dependency,
									   recall_id);

      delay_audio = NULL;
      sequencer_duration = NULL;
      
      g_object_get(delay_audio_run,
		   "recall-audio", &delay_audio,
		   NULL);

      note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));

      g_object_get(delay_audio,
		   "sequencer-duration", &sequencer_duration,
		   NULL);

      g_value_init(&value, G_TYPE_UINT64);
      ags_port_safe_read(sequencer_duration, &value);
      
      g_rec_mutex_lock(recall_mutex);
      
      AGS_COUNT_BEATS_AUDIO_RUN(recall)->sequencer_counter = note_offset % g_value_get_uint64(&value);
      AGS_COUNT_BEATS_AUDIO_RUN(recall)->notation_counter = note_offset;
      AGS_COUNT_BEATS_AUDIO_RUN(recall)->wave_counter = note_offset;
      AGS_COUNT_BEATS_AUDIO_RUN(recall)->midi_counter = note_offset;

      g_rec_mutex_unlock(recall_mutex);

      g_value_unset(&value);

      /* unref */
      g_object_unref(sequencer_duration);
      
      g_object_unref(delay_audio);
      
      i++;
    }

    g_object_unref(dependency);
    
    list = list->next;
  }
  
  g_object_set(G_OBJECT(recall),
	       "delay-audio-run", delay_audio_run,
	       NULL);

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(recall_container);
  
  g_list_free_full(list_start,
		   g_object_unref);
  
}

void
ags_count_beats_audio_run_notify_dependency(AgsRecall *recall,
					    guint dependency, gboolean increase)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  GRecMutex *recall_mutex;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall);

  /* get mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* notify */
  g_rec_mutex_lock(recall_mutex);

  switch(dependency){
  case AGS_RECALL_NOTIFY_RUN:
    if(increase){
      count_beats_audio_run->hide_ref += 1;
    }else{
      count_beats_audio_run->hide_ref -= 1;
    }
    
    //    g_message("count_beats_audio_run->hide_ref: %u\n", count_beats_audio_run->hide_ref);
    break;
  case AGS_RECALL_NOTIFY_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    if(increase){
      count_beats_audio_run->recall_ref += 1;
    }else{
      count_beats_audio_run->recall_ref -= 1;
    }
    
    break;
  default:
    g_message("ags_count_beats_audio_run.c - ags_count_beats_audio_run_notify: unknown notify");
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_count_beats_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  void (*parent_class_run_init_pre)(AgsRecall *recall);

  GRecMutex *recall_mutex;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall);
  
  /* get parent class */
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_count_beats_audio_run_parent_class)->run_init_pre;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);

  /* call parent */
  parent_class_run_init_pre(recall);

  /* basic initialization */
  g_rec_mutex_lock(recall_mutex);
  
  count_beats_audio_run->first_run = TRUE;

  count_beats_audio_run->sequencer_hide_ref_counter = 0;
  count_beats_audio_run->notation_hide_ref_counter = 0;
  count_beats_audio_run->wave_hide_ref_counter = 0;
  count_beats_audio_run->midi_hide_ref_counter = 0;
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_count_beats_audio_run_done(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  
  AgsCountBeatsAudio *count_beats_audio;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsDelayAudioRun *delay_audio_run;
  
  AgsCancelAudio *cancel_audio;

  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
    
  void (*parent_class_done)(AgsRecall *recall);

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(recall);
  
  /* get parent class */
  parent_class_done = AGS_RECALL_CLASS(ags_count_beats_audio_run_parent_class)->done;

  g_object_get(count_beats_audio_run,
	       "audio", &audio,
	       "recall-audio", &count_beats_audio,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(count_beats_audio_run,
	       "delay-audio-run", &delay_audio_run,
	       NULL);
  
  /* get application_context */  
  application_context = ags_application_context_get_instance();
  
  /* get task thread */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  /* create cancel task */
  cancel_audio = ags_cancel_audio_new(audio,
				      AGS_SOUND_SCOPE_SEQUENCER);
  
  /* append AgsCancelAudio */
  ags_task_launcher_add_task(task_launcher,
			     (AgsTask *) cancel_audio);  
  
  /* call parent */
  parent_class_done(recall);

  /* chained events */
  ags_recall_done((AgsRecall *) delay_audio_run);
  ags_audio_done_recall(audio,
			recall_id);

  /* unref */
  g_object_unref(audio);

  g_object_unref(count_beats_audio);

  g_object_unref(recall_id);

  g_object_unref(delay_audio_run);

  g_object_unref(task_launcher);
}

/**
 * ags_count_beats_audio_run_sequencer_start:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as sequencer starts playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_sequencer_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					  guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[SEQUENCER_START], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_sequencer_loop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as sequencer loops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_sequencer_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[SEQUENCER_LOOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_sequencer_stop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as sequencer stops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_sequencer_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[SEQUENCER_STOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_notation_start:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as notation starts playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_notation_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[NOTATION_START], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_notation_loop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as notation loops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_notation_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[NOTATION_LOOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_notation_stop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as notation stops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_notation_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[NOTATION_STOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_wave_start:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as wave starts playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_wave_start(AgsCountBeatsAudioRun *count_beats_audio_run,
				     guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[WAVE_START], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_wave_loop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as wave loops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_wave_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
				    guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[WAVE_LOOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_wave_stop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as wave stops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_wave_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
				    guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[WAVE_STOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_midi_start:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as midi starts playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_midi_start(AgsCountBeatsAudioRun *count_beats_audio_run,
					 guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[MIDI_START], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_midi_loop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as midi loops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_midi_loop(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[MIDI_LOOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

/**
 * ags_count_beats_audio_run_midi_stop:
 * @count_beats_audio_run: the #AgsCountBeatsAudioRun
 * @nth_run: the nth run order
 *
 * Emitted as midi stops playback.
 *
 * Since: 2.0.0
 */
void
ags_count_beats_audio_run_midi_stop(AgsCountBeatsAudioRun *count_beats_audio_run,
					guint nth_run)
{
  g_return_if_fail(AGS_IS_COUNT_BEATS_AUDIO_RUN(count_beats_audio_run));

  g_object_ref(G_OBJECT(count_beats_audio_run));
  g_signal_emit(G_OBJECT(count_beats_audio_run),
		count_beats_audio_run_signals[MIDI_STOP], 0,
		nth_run);
  g_object_unref(G_OBJECT(count_beats_audio_run));
}

void
ags_count_beats_audio_run_sequencer_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							  guint nth_run,
							  gdouble delay, guint attack,
							  AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsRecallID *recall_id;
  AgsPort *sequencer_loop;
  
  AgsCountBeatsAudio *count_beats_audio;

  guint64 sequencer_counter;
  gboolean first_run;
  gboolean do_loop;
  
  GValue loop_value = {0,};  

  GRecMutex *recall_mutex;
  
  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);
  
  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_SEQUENCER)){
    g_object_unref(recall_id);
    
    return;
  }

  g_object_get(count_beats_audio_run,
	       "recall-audio", &count_beats_audio,
	       NULL);

  /* loop */
  g_object_get(count_beats_audio,
	       "sequencer-loop", &sequencer_loop,
	       NULL);

  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);
  
  ags_port_safe_read(sequencer_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(sequencer_loop);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  sequencer_counter = count_beats_audio_run->sequencer_counter;
  
  first_run = count_beats_audio_run->first_run;
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* start */
  if(first_run){
    //    g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: start\n");
    ags_count_beats_audio_run_sequencer_start(count_beats_audio_run,
					      nth_run);
  }

  /* loop */
  if(!first_run &&
     sequencer_counter == 0){
    /* emit sequencer signals */
    if(do_loop){
      //	g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: loop\n");

      ags_count_beats_audio_run_sequencer_loop(count_beats_audio_run,
					      nth_run);
    }
  }

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_notation_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
							 guint nth_run, gdouble delay, guint attack,
							 AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsRecallID *recall_id;
  AgsPort *notation_loop;
  
  AgsCountBeatsAudio *count_beats_audio;

  guint64 notation_counter;
  gboolean first_run;
  gboolean do_loop;
  
  GValue loop_value = {0,};  

  GRecMutex *recall_mutex;
  
  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);
  
  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_NOTATION)){
    g_object_unref(recall_id);

    return;
  }

  g_object_get(count_beats_audio_run,
	       "recall-audio", &count_beats_audio,
	       NULL);

  /* loop */
  g_object_get(count_beats_audio,
	       "notation-loop", &notation_loop,
	       NULL);

  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);
  
  ags_port_safe_read(notation_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(notation_loop);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  notation_counter = count_beats_audio_run->notation_counter;
  
  first_run = count_beats_audio_run->first_run;
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* start */
  if(first_run){
    //    g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: start\n");
    ags_count_beats_audio_run_notation_start(count_beats_audio_run,
					     nth_run);
  }

  /* loop */
  if(!first_run &&
     notation_counter == 0){
    /* emit notation signals */
    if(do_loop){
      //	g_message("ags_count_beats_audio_run_notation_alloc_output_callback: loop\n");

      ags_count_beats_audio_run_notation_loop(count_beats_audio_run,
					      nth_run);
    }
  }

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_wave_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
						     guint nth_run, gdouble delay, guint attack,
						     AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsRecallID *recall_id;
  AgsPort *wave_loop;
  
  AgsCountBeatsAudio *count_beats_audio;

  guint64 wave_counter;
  gboolean first_run;
  gboolean do_loop;
  
  GValue loop_value = {0,};  

  GRecMutex *recall_mutex;
  
  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);
  
  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    g_object_unref(recall_id);

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_object_get(count_beats_audio_run,
	       "recall-audio", &count_beats_audio,
	       NULL);

  /* loop */
  g_object_get(count_beats_audio,
	       "wave-loop", &wave_loop,
	       NULL);

  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);
  
  ags_port_safe_read(wave_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(wave_loop);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  wave_counter = count_beats_audio_run->wave_counter;
  
  first_run = count_beats_audio_run->first_run;
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* start */
  if(first_run){
    //    g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: start\n");
    ags_count_beats_audio_run_wave_start(count_beats_audio_run,
					     nth_run);
  }

  /* loop */
  if(!first_run &&
     wave_counter == 0){
    /* emit wave signals */
    if(do_loop){
      //	g_message("ags_count_beats_audio_run_wave_alloc_output_callback: loop\n");

      ags_count_beats_audio_run_wave_loop(count_beats_audio_run,
					      nth_run);
    }
  }

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_midi_alloc_output_callback(AgsDelayAudioRun *delay_audio_run,
						     guint nth_run, gdouble delay, guint attack,
						     AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsRecallID *recall_id;
  AgsPort *midi_loop;
  
  AgsCountBeatsAudio *count_beats_audio;

  guint64 midi_counter;
  gboolean first_run;
  gboolean do_loop;
  
  GValue loop_value = {0,};  

  GRecMutex *recall_mutex;
  
  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);
  
  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    g_object_unref(recall_id);

    return;
  }

  g_object_get(count_beats_audio_run,
	       "recall-audio", &count_beats_audio,
	       NULL);

  /* loop */
  g_object_get(count_beats_audio,
	       "midi-loop", &midi_loop,
	       NULL);

  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);
  
  ags_port_safe_read(midi_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(midi_loop);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  midi_counter = count_beats_audio_run->midi_counter;
  
  first_run = count_beats_audio_run->first_run;
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* start */
  if(first_run){
    //    g_message("ags_count_beats_audio_run_sequencer_alloc_output_callback: start\n");
    ags_count_beats_audio_run_midi_start(count_beats_audio_run,
					     nth_run);
  }

  /* loop */
  if(!first_run &&
     midi_counter == 0){
    /* emit midi signals */
    if(do_loop){
      //	g_message("ags_count_beats_audio_run_midi_alloc_output_callback: loop\n");

      ags_count_beats_audio_run_midi_loop(count_beats_audio_run,
					      nth_run);
    }
  }

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run,
						   guint nth_run, gdouble delay, guint attack,
						   AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  AgsPort *sequencer_loop;
  AgsPort *sequencer_loop_start;
  AgsPort *sequencer_loop_end;

  AgsCountBeatsAudio *count_beats_audio;

  gboolean loop;

  guint64 loop_start, loop_end;
  gboolean do_loop;
  gboolean is_done;
  
  GValue loop_value = {0,};
  GValue loop_start_value = {0,};  
  GValue loop_end_value = {0,};  

  GRecMutex *recall_mutex;

  if(delay != 0.0){
    return;
  }
  
  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);

  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_SEQUENCER)){
    g_object_unref(recall_id);

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_object_get(count_beats_audio_run,
	       "audio", &audio,
	       "recall-audio", &count_beats_audio,
	       NULL);

  g_object_get(count_beats_audio,
	       "sequencer-loop", &sequencer_loop,
	       "sequencer-loop-start", &sequencer_loop_start,
	       "sequencer-loop-end", &sequencer_loop_end,
	       NULL);

  /* loop */
  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(sequencer_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(sequencer_loop);
  
  /* loop start */
  g_value_init(&loop_start_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(sequencer_loop_start,
		     &loop_start_value);

  loop_start = g_value_get_uint64(&loop_start_value);
  g_value_unset(&loop_start_value);

  g_object_unref(sequencer_loop_start);

  /* loop end */
  g_value_init(&loop_end_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(sequencer_loop_end,
		     &loop_end_value);

  loop_end = g_value_get_uint64(&loop_end_value);
  g_value_unset(&loop_end_value);

  g_object_unref(sequencer_loop_end);
  
  /* count */
  g_rec_mutex_lock(recall_mutex);

  is_done = FALSE;
  
  if(count_beats_audio_run->first_run){
    count_beats_audio_run->first_run = FALSE;
  }
    
  if(do_loop){
    if(count_beats_audio_run->sequencer_counter >= (guint) loop_end - 1.0){
      count_beats_audio_run->sequencer_counter = loop_start;
    }else{
      count_beats_audio_run->sequencer_counter += 1;
    }
  }else{      
    if(count_beats_audio_run->sequencer_counter >= (guint) loop_end - 1.0){
      count_beats_audio_run->sequencer_counter = 0;
//      is_done = TRUE;
    }else{
      count_beats_audio_run->sequencer_counter += 1;
    }
  }

  g_rec_mutex_unlock(recall_mutex);

  if(is_done){
    /* reset sequencer counter */
    g_rec_mutex_lock(recall_mutex);
    
    count_beats_audio_run->sequencer_counter = 0;

    g_rec_mutex_unlock(recall_mutex);

    /* emit done */
    ags_recall_done((AgsRecall *) count_beats_audio_run);
  }

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(audio);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_notation_count_callback(AgsDelayAudioRun *delay_audio_run,
						  guint nth_run,
						  gdouble delay, guint attack,
						  AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  AgsPort *notation_loop;
  AgsPort *notation_loop_start;
  AgsPort *notation_loop_end;

  AgsCountBeatsAudio *count_beats_audio;

  guint64 loop_start, loop_end;
  gboolean do_loop;

  GValue loop_value = {0,};
  GValue loop_start_value = {0,};  
  GValue loop_end_value = {0,};  

  GRecMutex *recall_mutex;

  if(delay != 0.0){
    return;
  }

  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);

  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_NOTATION)){
    g_object_unref(recall_id);

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_object_get(count_beats_audio_run,
	       "audio", &audio,
	       "recall-audio", &count_beats_audio,
	       NULL);

  g_object_get(count_beats_audio,
	       "notation-loop", &notation_loop,
	       "notation-loop-start", &notation_loop_start,
	       "notation-loop-end", &notation_loop_end,
	       NULL);

  /* loop */
  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(notation_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(notation_loop);
  
  /* loop start */
  g_value_init(&loop_start_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(notation_loop_start,
		     &loop_start_value);

  loop_start = g_value_get_uint64(&loop_start_value);
  g_value_unset(&loop_start_value);

  g_object_unref(notation_loop_start);
  
  /* loop end */
  g_value_init(&loop_end_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(notation_loop_end,
		     &loop_end_value);

  loop_end = g_value_get_uint64(&loop_end_value);
  g_value_unset(&loop_end_value);

  g_object_unref(notation_loop_end);
    
  /* count */
  g_rec_mutex_lock(recall_mutex);

  if(count_beats_audio_run->first_run){
    count_beats_audio_run->first_run = FALSE;
  }

  if(do_loop){
    if(count_beats_audio_run->notation_counter >= loop_end - 1){
      count_beats_audio_run->notation_counter = loop_start;
    }else{
      count_beats_audio_run->notation_counter += 1;
    }
  }else{
    count_beats_audio_run->notation_counter += 1;
  }

  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(audio);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_wave_count_callback(AgsDelayAudioRun *delay_audio_run,
					      guint nth_run,
					      gdouble delay, guint attack,
					      AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  AgsPort *wave_loop;
  AgsPort *wave_loop_start;
  AgsPort *wave_loop_end;

  AgsCountBeatsAudio *count_beats_audio;

  guint64 loop_start, loop_end;
  gboolean do_loop;

  GValue loop_value = {0,};
  GValue loop_start_value = {0,};  
  GValue loop_end_value = {0,};  

  GRecMutex *recall_mutex;

  if(delay != 0.0){
    return;
  }

  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);

  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    g_object_unref(recall_id);

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_object_get(count_beats_audio_run,
	       "audio", &audio,
	       "recall-audio", &count_beats_audio,
	       NULL);

  g_object_get(count_beats_audio,
	       "wave-loop", &wave_loop,
	       "wave-loop-start", &wave_loop_start,
	       "wave-loop-end", &wave_loop_end,
	       NULL);

  /* loop */
  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(wave_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(wave_loop);
  
  /* loop start */
  g_value_init(&loop_start_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(wave_loop_start,
		     &loop_start_value);

  loop_start = g_value_get_uint64(&loop_start_value);
  g_value_unset(&loop_start_value);

  g_object_unref(wave_loop_start);

  /* loop end */
  g_value_init(&loop_end_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(wave_loop_end,
		     &loop_end_value);

  loop_end = g_value_get_uint64(&loop_end_value);
  g_value_unset(&loop_end_value);

  g_object_unref(wave_loop_end);

  /* count */
  g_rec_mutex_lock(recall_mutex);

  if(count_beats_audio_run->first_run){
    count_beats_audio_run->first_run = FALSE;
  }

  if(do_loop){
    if(count_beats_audio_run->wave_counter >= loop_end - 1){
      count_beats_audio_run->wave_counter = loop_start;
    }else{
      count_beats_audio_run->wave_counter += 1;
    }
  }else{
    count_beats_audio_run->wave_counter += 1;
  }

  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(audio);

  g_object_unref(count_beats_audio);
}

void
ags_count_beats_audio_run_midi_count_callback(AgsDelayAudioRun *delay_audio_run,
					      guint nth_run,
					      gdouble delay, guint attack,
					      AgsCountBeatsAudioRun *count_beats_audio_run)
{
  AgsAudio *audio;
  AgsRecallID *recall_id;
  AgsPort *midi_loop;
  AgsPort *midi_loop_start;
  AgsPort *midi_loop_end;

  AgsCountBeatsAudio *count_beats_audio;

  guint64 loop_start, loop_end;
  gboolean do_loop;

  GValue loop_value = {0,};
  GValue loop_start_value = {0,};  
  GValue loop_end_value = {0,};  

  GRecMutex *recall_mutex;

  if(delay != 0.0){
    return;
  }

  g_object_get(count_beats_audio_run,
	       "recall-id", &recall_id,
	       NULL);

  if(!ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    g_object_unref(recall_id);

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* get some fields */
  g_object_get(count_beats_audio_run,
	       "audio", &audio,
	       "recall-audio", &count_beats_audio,
	       NULL);

  g_object_get(count_beats_audio,
	       "midi-loop", &midi_loop,
	       "midi-loop-start", &midi_loop_start,
	       "midi-loop-end", &midi_loop_end,
	       NULL);

  /* loop */
  g_value_init(&loop_value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(midi_loop,
		     &loop_value);

  do_loop = g_value_get_boolean(&loop_value);
  g_value_unset(&loop_value);

  g_object_unref(midi_loop);

  /* loop start */
  g_value_init(&loop_start_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(midi_loop_start,
		     &loop_start_value);

  loop_start = g_value_get_uint64(&loop_start_value);
  g_value_unset(&loop_start_value);

  g_object_unref(midi_loop_start);

  /* loop end */
  g_value_init(&loop_end_value,
	       G_TYPE_UINT64);
  
  ags_port_safe_read(midi_loop_end,
		     &loop_end_value);

  loop_end = g_value_get_uint64(&loop_end_value);
  g_value_unset(&loop_end_value);

  g_object_unref(midi_loop_end);

  /* count */
  g_rec_mutex_lock(recall_mutex);

  if(count_beats_audio_run->first_run){
    count_beats_audio_run->first_run = FALSE;
  }

  if(do_loop){
    if(count_beats_audio_run->midi_counter >= loop_end - 1){
      count_beats_audio_run->midi_counter = loop_start;
    }else{
      count_beats_audio_run->midi_counter += 1;
    }
  }else{
    count_beats_audio_run->midi_counter += 1;
  }

  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  g_object_unref(recall_id);

  g_object_unref(audio);

  g_object_unref(count_beats_audio);
}

gdouble
ags_count_beats_audio_run_get_bpm(AgsTactable *tactable)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  
  gdouble bpm;
  
  GRecMutex *recall_mutex;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(tactable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* bpm */
  g_rec_mutex_lock(recall_mutex);

  bpm = count_beats_audio_run->bpm;
  
  g_rec_mutex_unlock(recall_mutex);

  return(bpm);
}

gdouble
ags_count_beats_audio_run_get_tact(AgsTactable *tactable)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;
  
  gdouble tact;
  
  GRecMutex *recall_mutex;

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(tactable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(count_beats_audio_run);

  /* tact */
  g_rec_mutex_lock(recall_mutex);

  tact = count_beats_audio_run->tact;
  
  g_rec_mutex_unlock(recall_mutex);

  return(tact);
}

void
ags_count_beats_audio_run_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  /* empty */
}

void
ags_count_beats_audio_run_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact)
{
  /* empty */
}

/**
 * ags_count_beats_audio_run_new:
 * @audio: the #AgsAudio
 * @delay_audio_run: the #AgsDelayAudioRun dependency
 *
 * Create a new instance of #AgsCountBeatsAudioRun
 *
 * Returns: the new #AgsCountBeatsAudioRun
 *
 * Since: 2.0.0
 */
AgsCountBeatsAudioRun*
ags_count_beats_audio_run_new(AgsAudio *audio,
			      AgsDelayAudioRun *delay_audio_run)
{
  AgsCountBeatsAudioRun *count_beats_audio_run;

  count_beats_audio_run = (AgsCountBeatsAudioRun *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
								 "audio", audio,
								 "delay-audio-run", delay_audio_run,
								 NULL);
  
  return(count_beats_audio_run);
}
