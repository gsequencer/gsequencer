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

#ifndef __AGS_FX_ABYSS_SYNTH_AUDIO_H__
#define __AGS_FX_ABYSS_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_abyss_synth_util.h>
#include <ags/audio/ags_amplifier_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_chorus_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_ABYSS_SYNTH_AUDIO                (ags_fx_abyss_synth_audio_get_type())
#define AGS_FX_ABYSS_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_ABYSS_SYNTH_AUDIO, AgsFxAbyssSynthAudio))
#define AGS_FX_ABYSS_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_ABYSS_SYNTH_AUDIO, AgsFxAbyssSynthAudioClass))
#define AGS_IS_FX_ABYSS_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_ABYSS_SYNTH_AUDIO))
#define AGS_IS_FX_ABYSS_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_ABYSS_SYNTH_AUDIO))
#define AGS_FX_ABYSS_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_ABYSS_SYNTH_AUDIO, AgsFxAbyssSynthAudioClass))

#define AGS_FX_ABYSS_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE (65536)

typedef struct _AgsFxAbyssSynthAudio AgsFxAbyssSynthAudio;
typedef struct _AgsFxAbyssSynthAudioScopeData AgsFxAbyssSynthAudioScopeData;
typedef struct _AgsFxAbyssSynthAudioChannelData AgsFxAbyssSynthAudioChannelData;
typedef struct _AgsFxAbyssSynthAudioInputData AgsFxAbyssSynthAudioInputData;
typedef struct _AgsFxAbyssSynthAudioClass AgsFxAbyssSynthAudioClass;

struct _AgsFxAbyssSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;

  AgsPort *synth_0_osc_0_oscillator;
  
  AgsPort *synth_0_osc_0_octave;
  AgsPort *synth_0_osc_0_key;

  AgsPort *synth_0_osc_0_phase;
  AgsPort *synth_0_osc_0_volume;

  AgsPort *synth_0_osc_0_low_pass_sends;
  
  AgsPort *synth_0_osc_1_oscillator;
  
  AgsPort *synth_0_osc_1_octave;
  AgsPort *synth_0_osc_1_key;

  AgsPort *synth_0_osc_1_phase;
  AgsPort *synth_0_osc_1_volume;

  AgsPort *synth_0_osc_1_low_pass_sends;

  AgsPort *synth_0_osc_2_oscillator;
  
  AgsPort *synth_0_osc_2_octave;
  AgsPort *synth_0_osc_2_key;

  AgsPort *synth_0_osc_2_phase;
  AgsPort *synth_0_osc_2_volume;

  AgsPort *synth_0_osc_2_low_pass_sends;

  AgsPort *synth_0_osc_3_oscillator;
  
  AgsPort *synth_0_osc_3_octave;
  AgsPort *synth_0_osc_3_key;

  AgsPort *synth_0_osc_3_phase;
  AgsPort *synth_0_osc_3_volume;
  
  AgsPort *synth_0_osc_3_low_pass_sends;

  AgsPort *synth_0_ring_0_enabled;
  
  AgsPort *synth_0_ring_0_tuning;

  AgsPort *synth_0_ring_0_drive;
  AgsPort *synth_0_ring_0_mix;
  AgsPort *synth_0_ring_0_gain;
  
  AgsPort *synth_0_ring_1_enabled;
  
  AgsPort *synth_0_ring_1_tuning;

  AgsPort *synth_0_ring_1_drive;
  AgsPort *synth_0_ring_1_mix;
  AgsPort *synth_0_ring_1_gain;
  
  AgsPort *synth_0_pitch_type;
  AgsPort *synth_0_pitch_tuning;

  AgsPort *synth_0_volume;
  
  AgsPort *synth_0_env_0_attack;
  AgsPort *synth_0_env_0_decay;
  AgsPort *synth_0_env_0_sustain;
  AgsPort *synth_0_env_0_release;
  AgsPort *synth_0_env_0_gain;
  AgsPort *synth_0_env_0_frequency;

  AgsPort *synth_0_env_0_sends;

  AgsPort *synth_0_env_1_attack;
  AgsPort *synth_0_env_1_decay;
  AgsPort *synth_0_env_1_sustain;
  AgsPort *synth_0_env_1_release;
  AgsPort *synth_0_env_1_gain;
  AgsPort *synth_0_env_1_frequency;

  AgsPort *synth_0_env_1_sends;

  AgsPort *synth_0_env_2_attack;
  AgsPort *synth_0_env_2_decay;
  AgsPort *synth_0_env_2_sustain;
  AgsPort *synth_0_env_2_release;
  AgsPort *synth_0_env_2_gain;
  AgsPort *synth_0_env_2_frequency;

  AgsPort *synth_0_env_2_sends;

  AgsPort *synth_0_env_3_attack;
  AgsPort *synth_0_env_3_decay;
  AgsPort *synth_0_env_3_sustain;
  AgsPort *synth_0_env_3_release;
  AgsPort *synth_0_env_3_gain;
  AgsPort *synth_0_env_3_frequency;

  AgsPort *synth_0_env_3_sends;
  
  AgsPort *synth_0_lfo_0_oscillator;
  AgsPort *synth_0_lfo_0_frequency;
  AgsPort *synth_0_lfo_0_depth;
  AgsPort *synth_0_lfo_0_tuning;

  AgsPort *synth_0_lfo_0_sends;
  
  AgsPort *synth_0_lfo_1_oscillator;
  AgsPort *synth_0_lfo_1_frequency;
  AgsPort *synth_0_lfo_1_depth;
  AgsPort *synth_0_lfo_1_tuning;

  AgsPort *synth_0_lfo_1_sends;

  AgsPort *synth_0_lfo_2_oscillator;
  AgsPort *synth_0_lfo_2_frequency;
  AgsPort *synth_0_lfo_2_depth;
  AgsPort *synth_0_lfo_2_tuning;

  AgsPort *synth_0_lfo_2_sends;
  
  AgsPort *synth_0_lfo_3_oscillator;
  AgsPort *synth_0_lfo_3_frequency;
  AgsPort *synth_0_lfo_3_depth;
  AgsPort *synth_0_lfo_3_tuning;

  AgsPort *synth_0_lfo_3_sends;

  AgsPort *synth_0_seq_0_modulation_0;
  AgsPort *synth_0_seq_0_modulation_1;
  AgsPort *synth_0_seq_0_modulation_2;
  AgsPort *synth_0_seq_0_modulation_3;
  AgsPort *synth_0_seq_0_modulation_4;
  AgsPort *synth_0_seq_0_modulation_5;
  AgsPort *synth_0_seq_0_modulation_6;
  AgsPort *synth_0_seq_0_modulation_7;
  AgsPort *synth_0_seq_0_modulation_8;
  AgsPort *synth_0_seq_0_modulation_9;
  AgsPort *synth_0_seq_0_modulation_10;
  AgsPort *synth_0_seq_0_modulation_11;
  AgsPort *synth_0_seq_0_modulation_12;
  AgsPort *synth_0_seq_0_modulation_13;
  AgsPort *synth_0_seq_0_modulation_14;
  AgsPort *synth_0_seq_0_modulation_15;

  AgsPort *synth_0_seq_0_pingpong;
  AgsPort *synth_0_seq_0_lfo_frequency;  

  AgsPort *synth_0_seq_0_sends;

  AgsPort *synth_0_seq_1_modulation_0;
  AgsPort *synth_0_seq_1_modulation_1;
  AgsPort *synth_0_seq_1_modulation_2;
  AgsPort *synth_0_seq_1_modulation_3;
  AgsPort *synth_0_seq_1_modulation_4;
  AgsPort *synth_0_seq_1_modulation_5;
  AgsPort *synth_0_seq_1_modulation_6;
  AgsPort *synth_0_seq_1_modulation_7;
  AgsPort *synth_0_seq_1_modulation_8;
  AgsPort *synth_0_seq_1_modulation_9;
  AgsPort *synth_0_seq_1_modulation_10;
  AgsPort *synth_0_seq_1_modulation_11;
  AgsPort *synth_0_seq_1_modulation_12;
  AgsPort *synth_0_seq_1_modulation_13;
  AgsPort *synth_0_seq_1_modulation_14;
  AgsPort *synth_0_seq_1_modulation_15;

  AgsPort *synth_0_seq_1_pingpong;
  AgsPort *synth_0_seq_1_lfo_frequency;

  AgsPort *synth_0_seq_1_sends;
  
  AgsPort *synth_0_seq_2_modulation_0;
  AgsPort *synth_0_seq_2_modulation_1;
  AgsPort *synth_0_seq_2_modulation_2;
  AgsPort *synth_0_seq_2_modulation_3;
  AgsPort *synth_0_seq_2_modulation_4;
  AgsPort *synth_0_seq_2_modulation_5;
  AgsPort *synth_0_seq_2_modulation_6;
  AgsPort *synth_0_seq_2_modulation_7;
  AgsPort *synth_0_seq_2_modulation_8;
  AgsPort *synth_0_seq_2_modulation_9;
  AgsPort *synth_0_seq_2_modulation_10;
  AgsPort *synth_0_seq_2_modulation_11;
  AgsPort *synth_0_seq_2_modulation_12;
  AgsPort *synth_0_seq_2_modulation_13;
  AgsPort *synth_0_seq_2_modulation_14;
  AgsPort *synth_0_seq_2_modulation_15;

  AgsPort *synth_0_seq_2_pingpong;
  AgsPort *synth_0_seq_2_lfo_frequency;

  AgsPort *synth_0_seq_2_sends;
  
  AgsPort *synth_0_seq_3_modulation_0;
  AgsPort *synth_0_seq_3_modulation_1;
  AgsPort *synth_0_seq_3_modulation_2;
  AgsPort *synth_0_seq_3_modulation_3;
  AgsPort *synth_0_seq_3_modulation_4;
  AgsPort *synth_0_seq_3_modulation_5;
  AgsPort *synth_0_seq_3_modulation_6;
  AgsPort *synth_0_seq_3_modulation_7;
  AgsPort *synth_0_seq_3_modulation_8;
  AgsPort *synth_0_seq_3_modulation_9;
  AgsPort *synth_0_seq_3_modulation_10;
  AgsPort *synth_0_seq_3_modulation_11;
  AgsPort *synth_0_seq_3_modulation_12;
  AgsPort *synth_0_seq_3_modulation_13;
  AgsPort *synth_0_seq_3_modulation_14;
  AgsPort *synth_0_seq_3_modulation_15;

  AgsPort *synth_0_seq_3_pingpong;
  AgsPort *synth_0_seq_3_lfo_frequency;
  
  AgsPort *synth_0_seq_3_sends;

  AgsPort *synth_0_pink_noise_0_frequency;
  AgsPort *synth_0_pink_noise_0_gain;  

  AgsPort *synth_0_pink_noise_0_sends;

  AgsPort *synth_0_pink_noise_1_frequency;
  AgsPort *synth_0_pink_noise_1_gain;

  AgsPort *synth_0_pink_noise_1_sends;

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

  AgsPort *chorus_enabled;
  
  AgsPort *chorus_pitch_type;

  AgsPort *chorus_input_volume;
  AgsPort *chorus_output_volume;
  
  AgsPort *chorus_lfo_oscillator;
  AgsPort *chorus_lfo_frequency;

  AgsPort *chorus_depth;
  AgsPort *chorus_mix;
  AgsPort *chorus_delay;
  
  AgsFxAbyssSynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxAbyssSynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxAbyssSynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxAbyssSynthAudioChannelData **channel_data;
};

struct _AgsFxAbyssSynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  gpointer synth_buffer_0;
  
  AgsAbyssSynthUtil *abyss_synth_util_0;

  AgsAmplifierUtil *amplifier_util_0;

  AgsChorusUtil *chorus_util;

  AgsFxAbyssSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxAbyssSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_abyss_synth_audio_get_type();

AgsFxAbyssSynthAudioScopeData* ags_fx_abyss_synth_audio_scope_data_alloc();
void ags_fx_abyss_synth_audio_scope_data_free(AgsFxAbyssSynthAudioScopeData *scope_data);

AgsFxAbyssSynthAudioChannelData* ags_fx_abyss_synth_audio_channel_data_alloc();
void ags_fx_abyss_synth_audio_channel_data_free(AgsFxAbyssSynthAudioChannelData *channel_data);

AgsFxAbyssSynthAudioInputData* ags_fx_abyss_synth_audio_input_data_alloc();
void ags_fx_abyss_synth_audio_input_data_free(AgsFxAbyssSynthAudioInputData *input_data);

/* instantiate */
AgsFxAbyssSynthAudio* ags_fx_abyss_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_ABYSS_SYNTH_AUDIO_H__*/
