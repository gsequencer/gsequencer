/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_HYBRID_FM_SYNTH_H__
#define __AGS_HYBRID_FM_SYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_HYBRID_FM_SYNTH                (ags_hybrid_fm_synth_get_type())
#define AGS_HYBRID_FM_SYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_HYBRID_FM_SYNTH, AgsHybridFMSynth))
#define AGS_HYBRID_FM_SYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_HYBRID_FM_SYNTH, AgsHybridFMSynthClass))
#define AGS_IS_HYBRID_FM_SYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_HYBRID_FM_SYNTH))
#define AGS_IS_HYBRID_FM_SYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_HYBRID_FM_SYNTH))
#define AGS_HYBRID_FM_SYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_HYBRID_FM_SYNTH, AgsHybridFMSynthClass))

typedef struct _AgsHybridFMSynth AgsHybridFMSynth;
typedef struct _AgsHybridFMSynthClass AgsHybridFMSynthClass;

struct _AgsHybridFMSynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *fm_synth_play_container;
  AgsRecallContainer *fm_synth_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  GtkComboBox *synth_0_oscillator;

  AgsDial *synth_0_octave;
  AgsDial *synth_0_key;

  AgsDial *synth_0_phase;
  AgsDial *synth_0_volume;

  GtkComboBox *synth_0_lfo_oscillator;

  GtkSpinButton *synth_0_lfo_frequency;
  AgsDial *synth_0_lfo_depth;
  AgsDial *synth_0_lfo_tuning;

  GtkComboBox *synth_1_oscillator;

  AgsDial *synth_1_octave;
  AgsDial *synth_1_key;

  AgsDial *synth_1_phase;
  AgsDial *synth_1_volume;

  GtkComboBox *synth_1_lfo_oscillator;

  GtkSpinButton *synth_1_lfo_frequency;
  AgsDial *synth_1_lfo_depth;
  AgsDial *synth_1_lfo_tuning;
  
  GtkComboBox *synth_2_oscillator;

  AgsDial *synth_2_octave;
  AgsDial *synth_2_key;

  AgsDial *synth_2_phase;
  AgsDial *synth_2_volume;

  GtkComboBox *synth_2_lfo_oscillator;

  GtkSpinButton *synth_2_lfo_frequency;
  AgsDial *synth_2_lfo_depth;
  AgsDial *synth_2_lfo_tuning;

  GtkCheckButton *sequencer_enabled;
  GtkComboBox *sequencer_sign;

  AgsDial *pitch_tuning;

  AgsDial *noise_gain;

  GtkCheckButton *low_pass_enabled;

  AgsDial *low_pass_q_lin;
  AgsDial *low_pass_filter_gain;

  GtkCheckButton *high_pass_enabled;

  AgsDial *high_pass_q_lin;
  AgsDial *high_pass_filter_gain;

  GtkCheckButton *chorus_enabled;

  AgsDial *chorus_input_volume;
  AgsDial *chorus_output_volume;

  GtkComboBox *chorus_lfo_oscillator;  
  GtkSpinButton *chorus_lfo_frequency;

  AgsDial *chorus_depth;
  AgsDial *chorus_mix;
  AgsDial *chorus_delay;
};

struct _AgsHybridFMSynthClass
{
  AgsMachineClass machine;
};

GType ags_hybrid_fm_synth_get_type(void);

AgsHybridFMSynth* ags_hybrid_fm_synth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_HYBRID_FM_SYNTH_H__*/
