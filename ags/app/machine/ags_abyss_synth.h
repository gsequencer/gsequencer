/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_ABYSS_SYNTH_H__
#define __AGS_ABYSS_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>
G_BEGIN_DECLS

#define AGS_TYPE_ABYSS_SYNTH                (ags_abyss_synth_get_type())
#define AGS_ABYSS_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ABYSS_SYNTH, AgsAbyssSynth))
#define AGS_ABYSS_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ABYSS_SYNTH, AgsAbyssSynthClass))
#define AGS_IS_ABYSS_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ABYSS_SYNTH))
#define AGS_IS_ABYSS_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ABYSS_SYNTH))
#define AGS_ABYSS_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ABYSS_SYNTH, AgsAbyssSynthClass))

typedef struct _AgsAbyssSynth AgsAbyssSynth;
typedef struct _AgsAbyssSynthClass AgsAbyssSynthClass;

struct _AgsAbyssSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *abyss_synth_play_container;
  AgsRecallContainer *abyss_synth_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  /* ENV 0-3 */
  AgsDial *env_0_attack;
  AgsDial *env_0_decay;
  AgsDial *env_0_sustain;
  AgsDial *env_0_release;

  AgsDial *env_0_gain;
  AgsDial *env_0_frequency;

  AgsDial *env_1_attack;
  AgsDial *env_1_decay;
  AgsDial *env_1_sustain;
  AgsDial *env_1_release;

  AgsDial *env_1_gain;
  AgsDial *env_1_frequency;

  AgsDial *env_2_attack;
  AgsDial *env_2_decay;
  AgsDial *env_2_sustain;
  AgsDial *env_2_release;

  AgsDial *env_2_gain;
  AgsDial *env_2_frequency;
  
  AgsDial *env_3_attack;
  AgsDial *env_3_decay;
  AgsDial *env_3_sustain;
  AgsDial *env_3_release;

  AgsDial *env_3_gain;
  AgsDial *env_3_frequency;

  /* LFO 0-3 */
  GtkDropDown *lfo_0_oscillator;

  AgsDial *lfo_0_frequency;
  AgsDial *lfo_0_depth;
  AgsDial *lfo_0_tuning;
  
  GtkDropDown *lfo_1_oscillator;

  AgsDial *lfo_1_frequency;
  AgsDial *lfo_1_depth;
  AgsDial *lfo_1_tuning;

  GtkDropDown *lfo_2_oscillator;

  AgsDial *lfo_2_frequency;
  AgsDial *lfo_2_depth;
  AgsDial *lfo_2_tuning;
  
  GtkDropDown *lfo_3_oscillator;

  AgsDial *lfo_3_frequency;
  AgsDial *lfo_3_depth;
  AgsDial *lfo_3_tuning;

  /* pink noise */
  AgsDial *pink_noise_frequency;
  AgsDial *pink_noise_gain;

  /* SEQ 0-3 */
  AgsDial *seq_0_modulation_0;
  AgsDial *seq_0_modulation_1;
  AgsDial *seq_0_modulation_2;
  AgsDial *seq_0_modulation_3;
  AgsDial *seq_0_modulation_4;
  AgsDial *seq_0_modulation_5;
  AgsDial *seq_0_modulation_6;
  AgsDial *seq_0_modulation_7;
  AgsDial *seq_0_modulation_8;
  AgsDial *seq_0_modulation_9;
  AgsDial *seq_0_modulation_10;
  AgsDial *seq_0_modulation_11;
  AgsDial *seq_0_modulation_12;
  AgsDial *seq_0_modulation_13;
  AgsDial *seq_0_modulation_14;
  AgsDial *seq_0_modulation_15;

  GtkCheckButton *seq_0_pingpong;
  GtkDropDown *seq_0_mode;
  GtkSpinButton *seq_0_lfo_frequency;
  
  AgsDial *seq_1_modulation_0;
  AgsDial *seq_1_modulation_1;
  AgsDial *seq_1_modulation_2;
  AgsDial *seq_1_modulation_3;
  AgsDial *seq_1_modulation_4;
  AgsDial *seq_1_modulation_5;
  AgsDial *seq_1_modulation_6;
  AgsDial *seq_1_modulation_7;
  AgsDial *seq_1_modulation_8;
  AgsDial *seq_1_modulation_9;
  AgsDial *seq_1_modulation_10;
  AgsDial *seq_1_modulation_11;
  AgsDial *seq_1_modulation_12;
  AgsDial *seq_1_modulation_13;
  AgsDial *seq_1_modulation_14;
  AgsDial *seq_1_modulation_15;

  GtkCheckButton *seq_1_pingpong;
  GtkDropDown *seq_1_mode;
  GtkSpinButton *seq_1_lfo_frequency;
  
  AgsDial *seq_2_modulation_0;
  AgsDial *seq_2_modulation_1;
  AgsDial *seq_2_modulation_2;
  AgsDial *seq_2_modulation_3;
  AgsDial *seq_2_modulation_4;
  AgsDial *seq_2_modulation_5;
  AgsDial *seq_2_modulation_6;
  AgsDial *seq_2_modulation_7;
  AgsDial *seq_2_modulation_8;
  AgsDial *seq_2_modulation_9;
  AgsDial *seq_2_modulation_10;
  AgsDial *seq_2_modulation_11;
  AgsDial *seq_2_modulation_12;
  AgsDial *seq_2_modulation_13;
  AgsDial *seq_2_modulation_14;
  AgsDial *seq_2_modulation_15;

  GtkCheckButton *seq_2_pingpong;
  GtkDropDown *seq_2_mode;
  GtkSpinButton *seq_2_lfo_frequency;
  
  AgsDial *seq_3_modulation_0;
  AgsDial *seq_3_modulation_1;
  AgsDial *seq_3_modulation_2;
  AgsDial *seq_3_modulation_3;
  AgsDial *seq_3_modulation_4;
  AgsDial *seq_3_modulation_5;
  AgsDial *seq_3_modulation_6;
  AgsDial *seq_3_modulation_7;
  AgsDial *seq_3_modulation_8;
  AgsDial *seq_3_modulation_9;
  AgsDial *seq_3_modulation_10;
  AgsDial *seq_3_modulation_11;
  AgsDial *seq_3_modulation_12;
  AgsDial *seq_3_modulation_13;
  AgsDial *seq_3_modulation_14;
  AgsDial *seq_3_modulation_15;

  GtkCheckButton *seq_3_pingpong;
  GtkDropDown *seq_3_mode;
  GtkSpinButton *seq_3_lfo_frequency;

  /* OSC 0-4 */
  GtkDropDown *osc_0_oscillator;

  AgsDial *osc_0_octave;
  AgsDial *osc_0_key;

  AgsDial *osc_0_phase;
  AgsDial *osc_0_volume;

  GtkCheckButton *osc_0_low_pass_0;
  GtkCheckButton *osc_0_low_pass_1;
  GtkCheckButton *osc_0_no_low_pass;

  GtkDropDown *osc_1_oscillator;  

  AgsDial *osc_1_octave;
  AgsDial *osc_1_key;

  AgsDial *osc_1_phase;
  AgsDial *osc_1_volume;

  GtkCheckButton *osc_1_low_pass_0;
  GtkCheckButton *osc_1_low_pass_1;
  GtkCheckButton *osc_1_no_low_pass;
  
  GtkDropDown *osc_2_oscillator;

  AgsDial *osc_2_octave;
  AgsDial *osc_2_key;

  AgsDial *osc_2_phase;
  AgsDial *osc_2_volume;

  GtkCheckButton *osc_2_low_pass_0;
  GtkCheckButton *osc_2_low_pass_1;
  GtkCheckButton *osc_2_no_low_pass;
  
  GtkDropDown *osc_3_oscillator;

  AgsDial *osc_3_octave;
  AgsDial *osc_3_key;

  AgsDial *osc_3_phase;
  AgsDial *osc_3_volume;

  GtkCheckButton *osc_3_low_pass_0;
  GtkCheckButton *osc_3_low_pass_1;
  GtkCheckButton *osc_3_no_low_pass;
  
  GtkDropDown *osc_4_oscillator;

  AgsDial *osc_4_octave;
  AgsDial *osc_4_key;

  AgsDial *osc_4_phase;
  AgsDial *osc_4_volume;

  GtkCheckButton *osc_4_low_pass_0;
  GtkCheckButton *osc_4_low_pass_1;
  GtkCheckButton *osc_4_no_low_pass;

  /* modulation to generator sends */
  
  /* modulation to filter sends */

  /* ring 0-1 */
  GtkCheckButton *ring_0_enabled;
  
  AgsDial *ring_0_lfo_frequency;
  AgsDial *ring_0_lfo_depth;
  
  AgsDial *ring_0_dry;
  AgsDial *ring_0_wet;

  AgsDial *ring_0_gain;

  GtkCheckButton *ring_1_enabled;
  
  AgsDial *ring_1_lfo_frequency;
  AgsDial *ring_1_lfo_depth;
  
  AgsDial *ring_1_dry;
  AgsDial *ring_1_wet;

  AgsDial *ring_1_gain;
  
  /* volume */
  AgsDial *volume;

  /* pitch */
  GtkDropDown *pitch_type;
  AgsDial *pitch_tuning;

  /* noise */
  AgsDial *noise_gain;

  /* amplifier */
  GtkScale *amplifier_0_amp_0_gain;
  GtkScale *amplifier_0_amp_1_gain;
  GtkScale *amplifier_0_amp_2_gain;
  GtkScale *amplifier_0_amp_3_gain;

  GtkScale *amplifier_0_filter_gain;
  
  /* low-pass 0-1 */
  AgsDial *low_pass_0_cut_off_frequency;
  AgsDial *low_pass_0_filter_gain;
  AgsDial *low_pass_0_no_clip;

  AgsDial *low_pass_1_cut_off_frequency;
  AgsDial *low_pass_1_filter_gain;
  AgsDial *low_pass_1_no_clip;

  /* delay 0-1 */
  AgsDial *delay_0_feedback;
  AgsDial *delay_0_dry;
  AgsDial *delay_0_wet;
  AgsDial *delay_0_time;
  AgsDial *delay_0_gain;
  
  AgsDial *delay_1_feedback;
  AgsDial *delay_1_dry;
  AgsDial *delay_1_wet;
  AgsDial *delay_1_time;
  AgsDial *delay_1_gain;

  /* tremolo 0-1 */
  GtkCheckButton *tremolo_0_enabled;

  AgsDial *tremolo_0_gain;
  AgsDial *tremolo_0_lfo_depth;
  AgsDial *tremolo_0_lfo_freq;
  AgsDial *tremolo_0_tuning;
  
  GtkCheckButton *tremolo_1_enabled;

  AgsDial *tremolo_1_gain;
  AgsDial *tremolo_1_lfo_depth;
  AgsDial *tremolo_1_lfo_freq;
  AgsDial *tremolo_1_tuning;

  /* vibrato 0-3 */
  GtkCheckButton *vibrato_0_enabled;

  AgsDial *vibrato_0_gain; 
  AgsDial *vibrato_0_lfo_depth;
  AgsDial *vibrato_0_lfo_freq;
  AgsDial *vibrato_0_tuning;
  
  GtkCheckButton *vibrato_1_enabled;

  AgsDial *vibrato_1_gain; 
  AgsDial *vibrato_1_lfo_depth;
  AgsDial *vibrato_1_lfo_freq;
  AgsDial *vibrato_1_tuning;
  
  GtkCheckButton *vibrato_2_enabled;

  AgsDial *vibrato_2_gain; 
  AgsDial *vibrato_2_lfo_depth;
  AgsDial *vibrato_2_lfo_freq;
  AgsDial *vibrato_2_tuning;
  
  GtkCheckButton *vibrato_3_enabled;

  AgsDial *vibrato_3_gain; 
  AgsDial *vibrato_3_lfo_depth;
  AgsDial *vibrato_3_lfo_freq;
  AgsDial *vibrato_3_tuning;

  /* wah-wah */
  AgsDial *wah_wah_attack;
  AgsDial *wah_wah_decay;
  AgsDial *wah_wah_sustain;
  AgsDial *wah_wah_release;
  AgsDial *wah_wah_gain;
  
  AgsDial *wah_wah_lfo_depth;
  AgsDial *wah_wah_lfo_freq;
  AgsDial *wah_wah_lfo_tuning;

  /* chorus */
  GtkCheckButton *chorus_enabled;

  AgsDial *chorus_input_volume;
  AgsDial *chorus_output_volume;

  GtkDropDown *chorus_lfo_oscillator;  
  GtkSpinButton *chorus_lfo_frequency;

  AgsDial *chorus_depth;
  AgsDial *chorus_mix;
  AgsDial *chorus_delay;
};

struct _AgsAbyssSynthClass
{
  AgsMachineClass machine;
};

GType ags_abyss_synth_get_type(void);

AgsAbyssSynth* ags_abyss_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_ABYSS_SYNTH_H__*/
