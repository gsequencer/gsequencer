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

#include <ags/audio/recall/ags_delay_audio_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_delay_audio.h>

#include <stdlib.h>
#include <math.h>

void ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay_audio_run);
void ags_delay_audio_run_init(AgsDelayAudioRun *delay_audio_run);
void ags_delay_audio_run_finalize(GObject *gobject);

void ags_delay_audio_run_run_init_pre(AgsRecall *recall);
void ags_delay_audio_run_run_pre(AgsRecall *recall);
AgsRecall* ags_delay_audio_run_duplicate(AgsRecall *recall,
					 AgsRecallID *recall_id,
					 guint *n_params, gchar **parameter_name, GValue *value);
void ags_delay_audio_run_notify_dependency(AgsRecall *recall,
					   guint dependency, gboolean increase);

/**
 * SECTION:ags_delay_audio_run
 * @short_description: delay
 * @title: AgsDelayAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_delay_audio_run.h
 *
 * The #AgsDelayAudioRun class delays.
 */

enum{
  NOTATION_ALLOC_OUTPUT,
  NOTATION_ALLOC_INPUT,
  NOTATION_COUNT,
  SEQUENCER_ALLOC_OUTPUT,
  SEQUENCER_ALLOC_INPUT,
  SEQUENCER_COUNT,
  WAVE_ALLOC_OUTPUT,
  WAVE_ALLOC_INPUT,
  WAVE_COUNT,
  MIDI_ALLOC_OUTPUT,
  MIDI_ALLOC_INPUT,
  MIDI_COUNT,
  LAST_SIGNAL,
};

static gpointer ags_delay_audio_run_parent_class = NULL;

static guint delay_audio_run_signals[LAST_SIGNAL];

GType
ags_delay_audio_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_delay_audio_run = 0;

    static const GTypeInfo ags_delay_audio_run_info = {
      sizeof(AgsDelayAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDelayAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_audio_run_init,
    };

    ags_type_delay_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
						      "AgsDelayAudioRun",
						      &ags_delay_audio_run_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_delay_audio_run);
  }

  return g_define_type_id__volatile;
}

void
ags_delay_audio_run_class_init(AgsDelayAudioRunClass *delay_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_delay_audio_run_parent_class = g_type_class_peek_parent(delay_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) delay_audio_run;

  gobject->finalize = ags_delay_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) delay_audio_run;

  recall->run_init_pre = ags_delay_audio_run_run_init_pre;
  recall->run_pre = ags_delay_audio_run_run_pre;
  recall->duplicate = ags_delay_audio_run_duplicate;
  recall->notify_dependency = ags_delay_audio_run_notify_dependency;

  /* AgsDelayAudioRun */
  delay_audio_run->notation_alloc_output = NULL;
  delay_audio_run->notation_alloc_input = NULL;
  delay_audio_run->notation_count = NULL;

  delay_audio_run->sequencer_alloc_output = NULL;
  delay_audio_run->sequencer_alloc_input = NULL;
  delay_audio_run->sequencer_count = NULL;

  delay_audio_run->wave_alloc_output = NULL;
  delay_audio_run->wave_alloc_input = NULL;
  delay_audio_run->wave_count = NULL;

  delay_audio_run->midi_alloc_output = NULL;
  delay_audio_run->midi_alloc_input = NULL;
  delay_audio_run->midi_count = NULL;

  /* signals */
  /**
   * AgsDelayAudioRun::sequencer-alloc-output:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::sequencer-alloc-output signal is emited while allocating
   * sequencer output.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[SEQUENCER_ALLOC_OUTPUT] =
    g_signal_new("sequencer-alloc-output",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_alloc_output),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::sequencer-alloc-input:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::sequencer-alloc-input signal is emited while allocating
   * sequencer input.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[SEQUENCER_ALLOC_INPUT] =
    g_signal_new("sequencer-alloc-input",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_alloc_input),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::sequencer-count:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::sequencer-count signal is emited while counting
   * sequencer.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[SEQUENCER_COUNT] =
    g_signal_new("sequencer-count",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, sequencer_count),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::notation-alloc-output:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::notation-alloc-output signal is emited while allocating
   * notation output.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[NOTATION_ALLOC_OUTPUT] =
    g_signal_new("notation-alloc-output",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_alloc_output),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::notation-alloc-input:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::notation-alloc-input signal is emited while allocating
   * notation input.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[NOTATION_ALLOC_INPUT] =
    g_signal_new("notation-alloc-input",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_alloc_input),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::notation-count:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::notation-count signal is emited while counting
   * notation.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[NOTATION_COUNT] =
    g_signal_new("notation-count",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, notation_count),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);
  
  /**
   * AgsDelayAudioRun::wave-alloc-output:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::wave-alloc-output signal is emited while allocating
   * wave output.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[WAVE_ALLOC_OUTPUT] =
    g_signal_new("wave-alloc-output",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, wave_alloc_output),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::wave-alloc-input:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::wave-alloc-input signal is emited while allocating
   * wave input.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[WAVE_ALLOC_INPUT] =
    g_signal_new("wave-alloc-input",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, wave_alloc_input),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::wave-count:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::wave-count signal is emited while counting
   * wave.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[WAVE_COUNT] =
    g_signal_new("wave-count",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, wave_count),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::midi-alloc-output:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::midi-alloc-output signal is emited while allocating
   * midi output.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[MIDI_ALLOC_OUTPUT] =
    g_signal_new("midi-alloc-output",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, midi_alloc_output),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::midi-alloc-input:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::midi-alloc-input signal is emited while allocating
   * midi input.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[MIDI_ALLOC_INPUT] =
    g_signal_new("midi-alloc-input",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, midi_alloc_input),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);

  /**
   * AgsDelayAudioRun::midi-count:
   * @delay_audio_run: the #AgsDelayAudioRun
   * @nth_run: the nth run
   * @delay: the delay
   * @attack: the attack
   *
   * The ::midi-count signal is emited while counting
   * midi.
   *
   * Since: 2.0.0
   */
  delay_audio_run_signals[MIDI_COUNT] =
    g_signal_new("midi-count",
		 G_TYPE_FROM_CLASS(delay_audio_run),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioRunClass, midi_count),
		 NULL, NULL,
		 ags_cclosure_marshal_VOID__UINT_DOUBLE_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_UINT,
		 G_TYPE_DOUBLE, G_TYPE_UINT);
}

void
ags_delay_audio_run_init(AgsDelayAudioRun *delay_audio_run)
{
  ags_recall_set_ability_flags((AgsRecall *) delay_audio_run, (AGS_SOUND_ABILITY_SEQUENCER |
							       AGS_SOUND_ABILITY_NOTATION |
							       AGS_SOUND_ABILITY_WAVE |
							       AGS_SOUND_ABILITY_MIDI));

  AGS_RECALL(delay_audio_run)->behaviour_flags |= AGS_SOUND_BEHAVIOUR_PERSISTENT;

  AGS_RECALL(delay_audio_run)->name = "ags-delay";
  AGS_RECALL(delay_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(delay_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(delay_audio_run)->xml_type = "ags-delay-audio-run";
  AGS_RECALL(delay_audio_run)->port = NULL;

  delay_audio_run->dependency_ref = 0;

  delay_audio_run->hide_ref = 0;
  delay_audio_run->hide_ref_counter = 0;

  delay_audio_run->sequencer_counter = 0;
  delay_audio_run->notation_counter = 0;
  delay_audio_run->wave_counter = 0;
  delay_audio_run->midi_counter = 0;
}

void
ags_delay_audio_run_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_delay_audio_run_parent_class)->finalize(gobject);
}

void
ags_delay_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsDelayAudioRun *delay_audio_run;

  void (*parent_class_run_init_pre)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->run_init_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent class */
  parent_class_run_init_pre(recall);

  /* run order */
  pthread_mutex_lock(recall_mutex);

  delay_audio_run->hide_ref_counter = 0;

  pthread_mutex_unlock(recall_mutex);
}

void
ags_delay_audio_run_run_pre(AgsRecall *recall)
{
  AgsRecallID *recall_id;
  AgsDelayAudio *delay_audio;
  AgsDelayAudioRun *delay_audio_run;

  GObject *output_soundcard;
  
  guint dependency_ref;
  guint nth_run;
  gdouble delay;
  guint attack;

  void (*parent_class_run_pre)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_pre = AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_pre(recall);

  //  g_message("ags_delay_audio_run_run_pre()");
  

  /* check done */  
  pthread_mutex_lock(recall_mutex);

  dependency_ref = delay_audio_run->dependency_ref;
  
  pthread_mutex_unlock(recall_mutex);

  if(!ags_recall_test_behaviour_flags(recall, AGS_SOUND_BEHAVIOUR_PERSISTENT) &&
     dependency_ref == 0){
    pthread_mutex_lock(recall_mutex);
  
    delay_audio_run->sequencer_counter = 0;
    delay_audio_run->notation_counter = 0;
    delay_audio_run->wave_counter = 0;
    delay_audio_run->midi_counter = 0;

    pthread_mutex_unlock(recall_mutex);

    ags_recall_done(recall);

    return;
  }
  
  g_object_get(delay_audio_run,
	       "output-soundcard", &output_soundcard,
	       "recall-id", &recall_id,
	       "recall-audio", &delay_audio,
	       NULL);

  nth_run = 0;  
  delay = 0.0;
  attack = ags_soundcard_get_attack(AGS_SOUNDCARD(output_soundcard));

  /* sequencer scope */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_SEQUENCER)){
    AgsPort *sequencer_delay;
    
    gdouble current_delay;
    guint sequencer_counter;
    
    GValue value = { 0, };
    
    g_object_get(delay_audio,
		 "sequencer-delay", &sequencer_delay,
		 NULL);
    
    /* read sequencer delay port */  
    g_value_init(&value,
		 G_TYPE_DOUBLE);

    ags_port_safe_read(sequencer_delay, &value);

    current_delay = g_value_get_double(&value);

    g_value_unset(&value);

    /* counter */
    pthread_mutex_lock(recall_mutex);
    
    if(delay_audio_run->sequencer_counter + 1 >= (guint) current_delay){
      delay_audio_run->sequencer_counter = 0;
    }else{
      delay_audio_run->sequencer_counter += 1;
    }

    sequencer_counter = delay_audio_run->sequencer_counter;
    
    pthread_mutex_unlock(recall_mutex);

    /* sequencer */
    if(sequencer_counter == 0){
      /* sequencer speed */
      ags_delay_audio_run_sequencer_alloc_output(delay_audio_run,
						 nth_run,
						 delay, 0);

      ags_delay_audio_run_sequencer_alloc_input(delay_audio_run,
						nth_run,
						delay, attack);
      ags_delay_audio_run_sequencer_count(delay_audio_run,
					  nth_run,
					  delay, attack);
    }else{
      delay = (gdouble) delay_audio_run->sequencer_counter;
      attack = 0;

      /* sequencer speed */
      ags_delay_audio_run_sequencer_alloc_output(delay_audio_run,
						 nth_run,
						 delay, attack);

      ags_delay_audio_run_sequencer_alloc_input(delay_audio_run,
						nth_run,
						delay, attack);
      ags_delay_audio_run_sequencer_count(delay_audio_run,
					  nth_run,
					  delay, attack);
    }
  }

  /* notation scope */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_NOTATION)){
    AgsPort *notation_delay;
    
    gdouble current_delay;
    guint notation_counter;
    
    GValue value = { 0, };

    g_object_get(delay_audio,
		 "notation-delay", &notation_delay,
		 NULL);
    
    /* read notation delay port */  
    g_value_init(&value,
		 G_TYPE_DOUBLE);

    ags_port_safe_read(notation_delay, &value);

    current_delay = g_value_get_double(&value);

    g_value_unset(&value);

    /* counter */
    pthread_mutex_lock(recall_mutex);
    
    if(delay_audio_run->notation_counter + 1 >= (guint) current_delay){
      delay_audio_run->notation_counter = 0;
    }else{
      delay_audio_run->notation_counter += 1;
    }

    notation_counter = delay_audio_run->notation_counter;
    
    pthread_mutex_unlock(recall_mutex);

    /* notation */
    if(notation_counter == 0){
      /* notation speed */
      ags_delay_audio_run_notation_alloc_output(delay_audio_run,
						nth_run,
						delay, 0);

      ags_delay_audio_run_notation_alloc_input(delay_audio_run,
					       nth_run,
					       delay, attack);
      ags_delay_audio_run_notation_count(delay_audio_run,
					 nth_run,
					 delay, attack);
    }else{
      delay = (gdouble) delay_audio_run->notation_counter;
      attack = 0;

      /* notation speed */
      ags_delay_audio_run_notation_alloc_output(delay_audio_run,
						nth_run,
						delay, attack);

      ags_delay_audio_run_notation_alloc_input(delay_audio_run,
					       nth_run,
					       delay, attack);
      ags_delay_audio_run_notation_count(delay_audio_run,
					 nth_run,
					 delay, attack);
    }
  }

  /* wave scope */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    AgsPort *wave_delay;
    
    gdouble current_delay;
    guint wave_counter;
    
    GValue value = { 0, };

    g_object_get(delay_audio,
		 "wave-delay", &wave_delay,
		 NULL);
    
    /* read wave delay port */  
    g_value_init(&value,
		 G_TYPE_DOUBLE);

    ags_port_safe_read(wave_delay, &value);

    current_delay = g_value_get_double(&value);

    g_value_unset(&value);

    /* counter */
    pthread_mutex_lock(recall_mutex);
    
    if(delay_audio_run->wave_counter + 1 >= (guint) current_delay){
      delay_audio_run->wave_counter = 0;
    }else{
      delay_audio_run->wave_counter += 1;
    }

    wave_counter = delay_audio_run->wave_counter;
    
    pthread_mutex_unlock(recall_mutex);

    /* wave */
    if(wave_counter == 0){
      /* wave speed */
      ags_delay_audio_run_wave_alloc_output(delay_audio_run,
					    nth_run,
					    delay, 0);

      ags_delay_audio_run_wave_alloc_input(delay_audio_run,
					   nth_run,
					   delay, attack);
      ags_delay_audio_run_wave_count(delay_audio_run,
				     nth_run,
				     delay, attack);
    }else{
      delay = (gdouble) delay_audio_run->wave_counter;
      attack = 0;

      /* wave speed */
      ags_delay_audio_run_wave_alloc_output(delay_audio_run,
					    nth_run,
					    delay, attack);

      ags_delay_audio_run_wave_alloc_input(delay_audio_run,
					   nth_run,
					   delay, attack);
      ags_delay_audio_run_wave_count(delay_audio_run,
				     nth_run,
				     delay, attack);
    }
  }

  /* midi scope */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    AgsPort *midi_delay;
    
    gdouble current_delay;
    guint midi_counter;
    
    GValue value = { 0, };

    g_object_get(delay_audio,
		 "midi-delay", &midi_delay,
		 NULL);
    
    /* read midi delay port */  
    g_value_init(&value,
		 G_TYPE_DOUBLE);

    ags_port_safe_read(midi_delay, &value);

    current_delay = g_value_get_double(&value);

    g_value_unset(&value);

    /* counter */
    pthread_mutex_lock(recall_mutex);
    
    if(delay_audio_run->midi_counter + 1 >= (guint) current_delay){
      delay_audio_run->midi_counter = 0;
    }else{
      delay_audio_run->midi_counter += 1;
    }

    midi_counter = delay_audio_run->midi_counter;
    
    pthread_mutex_unlock(recall_mutex);

    /* midi */
    if(midi_counter == 0){
      /* midi speed */
      ags_delay_audio_run_midi_alloc_output(delay_audio_run,
					    nth_run,
					    delay, 0);

      ags_delay_audio_run_midi_alloc_input(delay_audio_run,
					   nth_run,
					   delay, attack);
      ags_delay_audio_run_midi_count(delay_audio_run,
				     nth_run,
				     delay, attack);
    }else{
      delay = (gdouble) delay_audio_run->midi_counter;
      attack = 0;

      /* midi speed */
      ags_delay_audio_run_midi_alloc_output(delay_audio_run,
					    nth_run,
					    delay, attack);

      ags_delay_audio_run_midi_alloc_input(delay_audio_run,
					   nth_run,
					   delay, attack);
      ags_delay_audio_run_midi_count(delay_audio_run,
				     nth_run,
				     delay, attack);
    }
  }
}

AgsRecall*
ags_delay_audio_run_duplicate(AgsRecall *recall,
			      AgsRecallID *recall_id,
			      guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsDelayAudioRun *delay_audio_run, *copy_delay_audio_run;

  AgsRecall* (*parent_class_duplicate)(AgsRecall *recall,
				       AgsRecallID *recall_id,
				       guint *n_params, gchar **parameter_name, GValue *value);
	      
  pthread_mutex_t *recall_mutex;
  
  delay_audio_run = (AgsDelayAudioRun *) recall;

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;

  parent_class_duplicate = AGS_RECALL_CLASS(ags_delay_audio_run_parent_class)->duplicate;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* duplicate */
  copy_delay_audio_run = (AgsDelayAudioRun *) parent_class_duplicate(recall,
								     recall_id,
								     n_params, parameter_name, value);
  
  /* initial values copied */
  pthread_mutex_lock(recall_mutex);

  copy_delay_audio_run->dependency_ref = delay_audio_run->dependency_ref;

  copy_delay_audio_run->hide_ref = delay_audio_run->hide_ref;
  //  copy_delay_audio_run->hide_ref_counter = delay_audio_run->hide_ref_counter;

  //TODO:JK: may be you want to make a AgsRecallDependency, but a AgsCountable isn't a AgsRecall at all
  copy_delay_audio_run->sequencer_counter = delay_audio_run->sequencer_counter;
  copy_delay_audio_run->notation_counter = delay_audio_run->notation_counter;
  copy_delay_audio_run->wave_counter = delay_audio_run->wave_counter;
  copy_delay_audio_run->midi_counter = delay_audio_run->midi_counter;

  pthread_mutex_unlock(recall_mutex);

  return((AgsRecall *) copy_delay_audio_run);
}

void
ags_delay_audio_run_notify_dependency(AgsRecall *recall,
				      guint dependency, gboolean increase)
{
  AgsDelayAudioRun *delay_audio_run;

  pthread_mutex_t *recall_mutex;

  delay_audio_run = AGS_DELAY_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* notify */
  pthread_mutex_lock(recall_mutex);

  switch(dependency){
  case AGS_RECALL_NOTIFY_RUN:
    if(increase){
      delay_audio_run->hide_ref += 1;
    }else{
      delay_audio_run->hide_ref -= 1;
    }

    //  g_message("delay_audio_run->hide_ref: %u\n", delay_audio_run->hide_ref);
    break;
  case AGS_RECALL_NOTIFY_AUDIO:
    break;
  case AGS_RECALL_NOTIFY_AUDIO_RUN:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL:
    break;
  case AGS_RECALL_NOTIFY_CHANNEL_RUN:
    if(increase){
      delay_audio_run->dependency_ref += 1;
    }else{
      delay_audio_run->dependency_ref -= 1;
    }

    break;
  default:
    g_message("ags_delay_audio_run.c - ags_delay_audio_run_notify: unknown notify");
  }

  pthread_mutex_unlock(recall_mutex);
}

/**
 * ags_delay_audio_run_notation_alloc_output:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as notation allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_notation_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					  gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_ALLOC_OUTPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_notation_alloc_input:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as notation allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_notation_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					 gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_ALLOC_INPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_notation_count:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as notation counts.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_notation_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				   gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[NOTATION_COUNT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_sequencer_alloc_output:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as sequencer allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_sequencer_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					   gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_ALLOC_OUTPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_sequencer_alloc_input:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as sequencer allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_sequencer_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
					  gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_ALLOC_INPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_sequencer_count:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as sequencer counts.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_sequencer_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				    gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[SEQUENCER_COUNT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_wave_alloc_output:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as wave allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_wave_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				      gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[WAVE_ALLOC_OUTPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_wave_alloc_input:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as wave allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_wave_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				     gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[WAVE_ALLOC_INPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_wave_count:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as wave counts.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_wave_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			       gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[WAVE_COUNT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_midi_alloc_output:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as midi allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_midi_alloc_output(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				      gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[MIDI_ALLOC_OUTPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_midi_alloc_input:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as midi allocs output.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_midi_alloc_input(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				     gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[MIDI_ALLOC_INPUT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_midi_count:
 * @delay_audio_run: the #AgsDelayAudioRun
 * @nth_run: the nth run
 * @delay: the delay
 * @attack: the attack
 *
 * Emitted as midi counts.
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_run_midi_count(AgsDelayAudioRun *delay_audio_run, guint nth_run,
			       gdouble delay, guint attack)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO_RUN(delay_audio_run));

  g_object_ref(G_OBJECT(delay_audio_run));
  g_signal_emit(G_OBJECT(delay_audio_run),
		delay_audio_run_signals[MIDI_COUNT], 0,
		nth_run,
		delay, attack);
  g_object_unref(G_OBJECT(delay_audio_run));
}

/**
 * ags_delay_audio_run_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsDelayAudioRun
 *
 * Returns: the new #AgsDelayAudioRun
 *
 * Since: 2.0.0
 */
AgsDelayAudioRun*
ags_delay_audio_run_new(AgsAudio *audio)
{
  AgsDelayAudioRun *delay_audio_run;

  delay_audio_run = (AgsDelayAudioRun *) g_object_new(AGS_TYPE_DELAY_AUDIO_RUN,
						      "audio", audio,
						      NULL);

  return(delay_audio_run);
}
