/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_MODULAR_SYNTH_H__
#define __AGS_MODULAR_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

#include <ags/app/machine/ags_modulation_matrix.h>

G_BEGIN_DECLS

#define AGS_TYPE_MODULAR_SYNTH                (ags_modular_synth_get_type())
#define AGS_MODULAR_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MODULAR_SYNTH, AgsModularSynth))
#define AGS_MODULAR_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MODULAR_SYNTH, AgsModularSynthClass))
#define AGS_IS_MODULAR_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MODULAR_SYNTH))
#define AGS_IS_MODULAR_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MODULAR_SYNTH))
#define AGS_MODULAR_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MODULAR_SYNTH, AgsModularSynthClass))

typedef struct _AgsModularSynth AgsModularSynth;
typedef struct _AgsModularSynthClass AgsModularSynthClass;

struct _AgsModularSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *modular_synth_play_container;
  AgsRecallContainer *modular_synth_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  AgsDial *env_0_attack;
  AgsDial *env_0_sustain;
  AgsDial *env_0_decay;
  AgsDial *env_0_release;

  AgsDial *env_0_gain;
  AgsDial *env_0_frequency;

  AgsDial *env_1_attack;
  AgsDial *env_1_sustain;
  AgsDial *env_1_decay;
  AgsDial *env_1_release;

  AgsDial *env_1_gain;
  AgsDial *env_1_frequency;

  GtkDropDown *lfo_0_oscillator;

  AgsDial *lfo_0_frequency;
  AgsDial *lfo_0_depth;
  AgsDial *lfo_0_tuning;
  
  GtkDropDown *lfo_1_oscillator;

  AgsDial *lfo_1_frequency;
  AgsDial *lfo_1_depth;
  AgsDial *lfo_1_tuning;

  AgsDial *noise_frequency;
  AgsDial *noise_gain;

  AgsModulationMatrix *modulation_matrix;

  GtkDropDown *osc_0_oscillator;

  AgsDial *osc_0_octave;
  AgsDial *osc_0_key;

  AgsDial *osc_0_phase;
  AgsDial *osc_0_volume;

  GtkDropDown *osc_1_oscillator;  

  AgsDial *osc_1_octave;
  AgsDial *osc_1_key;

  AgsDial *osc_1_phase;
  AgsDial *osc_1_volume;

  AgsDial *volume;

  GtkDropDown *pitch_type;
  AgsDial *pitch_tuning;

  AgsDial *low_pass_0_cut_off_frequency;
  AgsDial *low_pass_0_filter_gain;
  AgsDial *low_pass_0_no_clip;

  GtkScale *amplifier_0_amp_0_gain;
  GtkScale *amplifier_0_amp_1_gain;
  GtkScale *amplifier_0_amp_2_gain;
  GtkScale *amplifier_0_amp_3_gain;

  GtkScale *amplifier_0_filter_gain;
  
  GtkCheckButton *chorus_enabled;

  AgsDial *chorus_input_volume;
  AgsDial *chorus_output_volume;

  GtkDropDown *chorus_lfo_oscillator;  
  GtkSpinButton *chorus_lfo_frequency;

  AgsDial *chorus_depth;
  AgsDial *chorus_mix;
  AgsDial *chorus_delay;
};

struct _AgsModularSynthClass
{
  AgsMachineClass machine;
};

GType ags_modular_synth_get_type(void);

AgsModularSynth* ags_modular_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_MODULAR_SYNTH_H__*/
