/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_QUANTUM_SYNTH_H__
#define __AGS_QUANTUM_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_QUANTUM_SYNTH                (ags_quantum_synth_get_type())
#define AGS_QUANTUM_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_QUANTUM_SYNTH, AgsQuantumSynth))
#define AGS_QUANTUM_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_QUANTUM_SYNTH, AgsQuantumSynthClass))
#define AGS_IS_QUANTUM_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_QUANTUM_SYNTH))
#define AGS_IS_QUANTUM_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_QUANTUM_SYNTH))
#define AGS_QUANTUM_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_QUANTUM_SYNTH, AgsQuantumSynthClass))

typedef struct _AgsQuantumSynth AgsQuantumSynth;
typedef struct _AgsQuantumSynthClass AgsQuantumSynthClass;

struct _AgsQuantumSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *star_synth_play_container;
  AgsRecallContainer *star_synth_recall_container;

  AgsRecallContainer *tremolo_play_container;
  AgsRecallContainer *tremolo_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  GtkDropDown *synth_0_oscillator;

  AgsDial *synth_0_octave;
  AgsDial *synth_0_key;

  AgsDial *synth_0_phase;
  AgsDial *synth_0_volume;

  AgsDial *synth_0_seq_tuning_0;
  AgsDial *synth_0_seq_tuning_1;
  AgsDial *synth_0_seq_tuning_2;
  AgsDial *synth_0_seq_tuning_3;
  AgsDial *synth_0_seq_tuning_4;
  AgsDial *synth_0_seq_tuning_5;
  AgsDial *synth_0_seq_tuning_6;
  AgsDial *synth_0_seq_tuning_7;

  GtkCheckButton *synth_0_seq_tuning_pingpong;
  
  AgsDial *synth_0_seq_volume_0;
  AgsDial *synth_0_seq_volume_1;
  AgsDial *synth_0_seq_volume_2;
  AgsDial *synth_0_seq_volume_3;
  AgsDial *synth_0_seq_volume_4;
  AgsDial *synth_0_seq_volume_5;
  AgsDial *synth_0_seq_volume_6;
  AgsDial *synth_0_seq_volume_7;

  GtkCheckButton *synth_0_seq_volume_pingpong;
  
  GtkDropDown *synth_0_lfo_oscillator;

  GtkSpinButton *synth_0_lfo_frequency;
  AgsDial *synth_0_lfo_depth;
  AgsDial *synth_0_lfo_tuning;

  GtkCheckButton *synth_0_sync_enabled;
  
  AgsDial *synth_0_sync_relative_attack_factor;

  AgsDial *synth_0_sync_attack_0;
  AgsDial *synth_0_sync_phase_0;

  AgsDial *synth_0_sync_attack_1;
  AgsDial *synth_0_sync_phase_1;

  AgsDial *synth_0_sync_attack_2;
  AgsDial *synth_0_sync_phase_2;

  AgsDial *synth_0_sync_attack_3;
  AgsDial *synth_0_sync_phase_3;

  GtkDropDown *synth_1_oscillator;

  GtkDropDown *synth_0_sync_lfo_oscillator;
  GtkSpinButton *synth_0_sync_lfo_frequency;

  AgsDial *synth_1_octave;
  AgsDial *synth_1_key;

  AgsDial *synth_1_phase;
  AgsDial *synth_1_volume;

  AgsDial *synth_1_seq_tuning_0;
  AgsDial *synth_1_seq_tuning_1;
  AgsDial *synth_1_seq_tuning_2;
  AgsDial *synth_1_seq_tuning_3;
  AgsDial *synth_1_seq_tuning_4;
  AgsDial *synth_1_seq_tuning_5;
  AgsDial *synth_1_seq_tuning_6;
  AgsDial *synth_1_seq_tuning_7;

  GtkCheckButton *synth_1_seq_tuning_pingpong;
  GtkCheckButton *synth_1_seq_tuning_linear;
  GtkCheckButton *synth_1_seq_tuning_interpolate;
  
  AgsDial *synth_1_seq_volume_0;
  AgsDial *synth_1_seq_volume_1;
  AgsDial *synth_1_seq_volume_2;
  AgsDial *synth_1_seq_volume_3;
  AgsDial *synth_1_seq_volume_4;
  AgsDial *synth_1_seq_volume_5;
  AgsDial *synth_1_seq_volume_6;
  AgsDial *synth_1_seq_volume_7;

  GtkCheckButton *synth_1_seq_volume_pingpong;
  GtkCheckButton *synth_1_seq_volume_linear;
  GtkCheckButton *synth_1_seq_volume_interpolate;

  GtkDropDown *synth_1_lfo_oscillator;

  GtkSpinButton *synth_1_lfo_frequency;
  AgsDial *synth_1_lfo_depth;
  AgsDial *synth_1_lfo_tuning;

  GtkCheckButton *synth_1_sync_enabled;
  
  AgsDial *synth_1_sync_relative_attack_factor;

  AgsDial *synth_1_sync_attack_0;
  AgsDial *synth_1_sync_phase_0;

  AgsDial *synth_1_sync_attack_1;
  AgsDial *synth_1_sync_phase_1;

  AgsDial *synth_1_sync_attack_2;
  AgsDial *synth_1_sync_phase_2;

  AgsDial *synth_1_sync_attack_3;
  AgsDial *synth_1_sync_phase_3;

  GtkDropDown *synth_1_sync_lfo_oscillator;
  GtkSpinButton *synth_1_sync_lfo_frequency;

  GtkDropDown *pitch_type;
  AgsDial *pitch_tuning;

  AgsDial *noise_gain;

  GtkCheckButton *tremolo_enabled;

  AgsDial *tremolo_gain;
  AgsDial *tremolo_lfo_depth;
  AgsDial *tremolo_lfo_freq;
  AgsDial *tremolo_tuning;

  GtkCheckButton *vibrato_enabled;

  AgsDial *vibrato_gain; 
  AgsDial *vibrato_lfo_depth;
  AgsDial *vibrato_lfo_freq;
  AgsDial *vibrato_tuning;

  GtkCheckButton *chorus_enabled;

  AgsDial *chorus_input_volume;
  AgsDial *chorus_output_volume;

  GtkDropDown *chorus_lfo_oscillator;  
  GtkSpinButton *chorus_lfo_frequency;

  AgsDial *chorus_depth;
  AgsDial *chorus_mix;
  AgsDial *chorus_delay;
};

struct _AgsQuantumSynthClass
{
  AgsMachineClass machine;
};

GType ags_quantum_synth_get_type(void);

AgsQuantumSynth* ags_quantum_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_QUANTUM_SYNTH_H__*/
