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

#ifndef __AGS_FX_RAVEN_SYNTH_AUDIO_H__
#define __AGS_FX_RAVEN_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_raven_synth_util.h>
#include <ags/audio/ags_low_pass_filter_util.h>
#include <ags/audio/ags_amplifier_util.h>
#include <ags/audio/ags_noise_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_chorus_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_RAVEN_SYNTH_AUDIO                (ags_fx_raven_synth_audio_get_type())
#define AGS_FX_RAVEN_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_RAVEN_SYNTH_AUDIO, AgsFxRavenSynthAudio))
#define AGS_FX_RAVEN_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_RAVEN_SYNTH_AUDIO, AgsFxRavenSynthAudioClass))
#define AGS_IS_FX_RAVEN_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_RAVEN_SYNTH_AUDIO))
#define AGS_IS_FX_RAVEN_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_RAVEN_SYNTH_AUDIO))
#define AGS_FX_RAVEN_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_RAVEN_SYNTH_AUDIO, AgsFxRavenSynthAudioClass))

#define AGS_FX_RAVEN_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE (65536)

typedef struct _AgsFxRavenSynthAudio AgsFxRavenSynthAudio;
typedef struct _AgsFxRavenSynthAudioScopeData AgsFxRavenSynthAudioScopeData;
typedef struct _AgsFxRavenSynthAudioChannelData AgsFxRavenSynthAudioChannelData;
typedef struct _AgsFxRavenSynthAudioInputData AgsFxRavenSynthAudioInputData;
typedef struct _AgsFxRavenSynthAudioClass AgsFxRavenSynthAudioClass;

struct _AgsFxRavenSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;

  AgsPort *synth_0_oscillator;
  
  AgsPort *synth_0_octave;
  AgsPort *synth_0_key;

  AgsPort *synth_0_phase;
  AgsPort *synth_0_volume;

  AgsPort *synth_0_seq_tuning_0;
  AgsPort *synth_0_seq_tuning_1;
  AgsPort *synth_0_seq_tuning_2;
  AgsPort *synth_0_seq_tuning_3;
  AgsPort *synth_0_seq_tuning_4;
  AgsPort *synth_0_seq_tuning_5;
  AgsPort *synth_0_seq_tuning_6;
  AgsPort *synth_0_seq_tuning_7;
  AgsPort *synth_0_seq_tuning_8;
  AgsPort *synth_0_seq_tuning_9;
  AgsPort *synth_0_seq_tuning_10;
  AgsPort *synth_0_seq_tuning_11;
  AgsPort *synth_0_seq_tuning_12;
  AgsPort *synth_0_seq_tuning_13;
  AgsPort *synth_0_seq_tuning_14;
  AgsPort *synth_0_seq_tuning_15;

  AgsPort *synth_0_seq_tuning_pingpong;
  AgsPort *synth_0_seq_tuning_lfo_frequency;

  AgsPort *synth_0_seq_volume_0;
  AgsPort *synth_0_seq_volume_1;
  AgsPort *synth_0_seq_volume_2;
  AgsPort *synth_0_seq_volume_3;
  AgsPort *synth_0_seq_volume_4;
  AgsPort *synth_0_seq_volume_5;
  AgsPort *synth_0_seq_volume_6;
  AgsPort *synth_0_seq_volume_7;
  AgsPort *synth_0_seq_volume_8;
  AgsPort *synth_0_seq_volume_9;
  AgsPort *synth_0_seq_volume_10;
  AgsPort *synth_0_seq_volume_11;
  AgsPort *synth_0_seq_volume_12;
  AgsPort *synth_0_seq_volume_13;
  AgsPort *synth_0_seq_volume_14;
  AgsPort *synth_0_seq_volume_15;

  AgsPort *synth_0_seq_volume_pingpong;
  AgsPort *synth_0_seq_volume_lfo_frequency;

  AgsPort *synth_0_lfo_oscillator;
  AgsPort *synth_0_lfo_frequency;

  AgsPort *synth_0_lfo_depth;
  AgsPort *synth_0_lfo_tuning;

  AgsPort *synth_0_sync_enabled;

  AgsPort *synth_0_sync_relative_attack_factor_0;
  AgsPort *synth_0_sync_attack_0;
  AgsPort *synth_0_sync_phase_0;

  AgsPort *synth_0_sync_relative_attack_factor_1;
  AgsPort *synth_0_sync_attack_1;
  AgsPort *synth_0_sync_phase_1;

  AgsPort *synth_0_sync_relative_attack_factor_2;
  AgsPort *synth_0_sync_attack_2;
  AgsPort *synth_0_sync_phase_2;

  AgsPort *synth_0_sync_relative_attack_factor_3;
  AgsPort *synth_0_sync_attack_3;
  AgsPort *synth_0_sync_phase_3;
  
  AgsPort *synth_1_oscillator;
  
  AgsPort *synth_1_octave;
  AgsPort *synth_1_key;

  AgsPort *synth_1_phase;
  AgsPort *synth_1_volume;

  AgsPort *synth_1_seq_tuning_0;
  AgsPort *synth_1_seq_tuning_1;
  AgsPort *synth_1_seq_tuning_2;
  AgsPort *synth_1_seq_tuning_3;
  AgsPort *synth_1_seq_tuning_4;
  AgsPort *synth_1_seq_tuning_5;
  AgsPort *synth_1_seq_tuning_6;
  AgsPort *synth_1_seq_tuning_7;
  AgsPort *synth_1_seq_tuning_8;
  AgsPort *synth_1_seq_tuning_9;
  AgsPort *synth_1_seq_tuning_10;
  AgsPort *synth_1_seq_tuning_11;
  AgsPort *synth_1_seq_tuning_12;
  AgsPort *synth_1_seq_tuning_13;
  AgsPort *synth_1_seq_tuning_14;
  AgsPort *synth_1_seq_tuning_15;

  AgsPort *synth_1_seq_tuning_pingpong;
  AgsPort *synth_1_seq_tuning_lfo_frequency;

  AgsPort *synth_1_seq_volume_0;
  AgsPort *synth_1_seq_volume_1;
  AgsPort *synth_1_seq_volume_2;
  AgsPort *synth_1_seq_volume_3;
  AgsPort *synth_1_seq_volume_4;
  AgsPort *synth_1_seq_volume_5;
  AgsPort *synth_1_seq_volume_6;
  AgsPort *synth_1_seq_volume_7;
  AgsPort *synth_1_seq_volume_8;
  AgsPort *synth_1_seq_volume_9;
  AgsPort *synth_1_seq_volume_10;
  AgsPort *synth_1_seq_volume_11;
  AgsPort *synth_1_seq_volume_12;
  AgsPort *synth_1_seq_volume_13;
  AgsPort *synth_1_seq_volume_14;
  AgsPort *synth_1_seq_volume_15;

  AgsPort *synth_1_seq_volume_pingpong;
  AgsPort *synth_1_seq_volume_lfo_frequency;

  AgsPort *synth_1_lfo_oscillator;
  AgsPort *synth_1_lfo_frequency;

  AgsPort *synth_1_lfo_depth;
  AgsPort *synth_1_lfo_tuning;

  AgsPort *synth_1_sync_enabled;

  AgsPort *synth_1_sync_relative_attack_factor_0;
  AgsPort *synth_1_sync_attack_0;
  AgsPort *synth_1_sync_phase_0;

  AgsPort *synth_1_sync_relative_attack_factor_1;
  AgsPort *synth_1_sync_attack_1;
  AgsPort *synth_1_sync_phase_1;

  AgsPort *synth_1_sync_relative_attack_factor_2;
  AgsPort *synth_1_sync_attack_2;
  AgsPort *synth_1_sync_phase_2;

  AgsPort *synth_1_sync_relative_attack_factor_3;
  AgsPort *synth_1_sync_attack_3;
  AgsPort *synth_1_sync_phase_3;

  AgsPort *low_pass_0_cut_off_frequency;
  AgsPort *low_pass_0_filter_gain;
  AgsPort *low_pass_0_no_clip;

  AgsPort *low_pass_1_cut_off_frequency;
  AgsPort *low_pass_1_filter_gain;
  AgsPort *low_pass_1_no_clip;

  AgsPort *amplifier_0_amp_0_gain;
  AgsPort *amplifier_0_amp_1_gain;
  AgsPort *amplifier_0_amp_2_gain;
  AgsPort *amplifier_0_amp_3_gain;

  AgsPort *amplifier_0_filter_gain;

  AgsPort *amplifier_1_amp_0_gain;
  AgsPort *amplifier_1_amp_1_gain;
  AgsPort *amplifier_1_amp_2_gain;
  AgsPort *amplifier_1_amp_3_gain;

  AgsPort *amplifier_1_filter_gain;

  AgsPort *pitch_type;
  AgsPort *pitch_tuning;
  
  AgsPort *noise_gain;

  AgsPort *chorus_enabled;
  
  AgsPort *chorus_pitch_type;

  AgsPort *chorus_input_volume;
  AgsPort *chorus_output_volume;
  
  AgsPort *chorus_lfo_oscillator;
  AgsPort *chorus_lfo_frequency;

  AgsPort *chorus_depth;
  AgsPort *chorus_mix;
  AgsPort *chorus_delay;

  AgsPort *vibrato_enabled;

  AgsPort *vibrato_gain;  
  AgsPort *vibrato_lfo_depth;
  AgsPort *vibrato_lfo_freq;
  AgsPort *vibrato_tuning;

  AgsFxRavenSynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxRavenSynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxRavenSynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxRavenSynthAudioChannelData **channel_data;
};

struct _AgsFxRavenSynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  gpointer synth_buffer_0;
  gpointer synth_buffer_1;
  
  AgsRavenSynthUtil *raven_synth_0;
  AgsRavenSynthUtil *raven_synth_1;

  AgsLowPassFilterUtil *low_pass_filter_util_0;
  AgsLowPassFilterUtil *low_pass_filter_util_1;

  AgsAmplifierUtil *amplifier_util_0;
  AgsAmplifierUtil *amplifier_util_1;
  
  AgsNoiseUtil *noise_util;

  GType pitch_type;
  gpointer pitch_util;
  
  AgsChorusUtil *chorus_util;

  AgsFxRavenSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxRavenSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_raven_synth_audio_get_type();

AgsFxRavenSynthAudioScopeData* ags_fx_raven_synth_audio_scope_data_alloc();
void ags_fx_raven_synth_audio_scope_data_free(AgsFxRavenSynthAudioScopeData *scope_data);

AgsFxRavenSynthAudioChannelData* ags_fx_raven_synth_audio_channel_data_alloc();
void ags_fx_raven_synth_audio_channel_data_free(AgsFxRavenSynthAudioChannelData *channel_data);

AgsFxRavenSynthAudioInputData* ags_fx_raven_synth_audio_input_data_alloc();
void ags_fx_raven_synth_audio_input_data_free(AgsFxRavenSynthAudioInputData *input_data);

/* instantiate */
AgsFxRavenSynthAudio* ags_fx_raven_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_RAVEN_SYNTH_AUDIO_H__*/
