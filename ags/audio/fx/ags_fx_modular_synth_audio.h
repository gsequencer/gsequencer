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

#ifndef __AGS_FX_MODULAR_SYNTH_AUDIO_H__
#define __AGS_FX_MODULAR_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_modular_synth_util.h>
#include <ags/audio/ags_low_pass_filter_util.h>
#include <ags/audio/ags_amplifier_util.h>
#include <ags/audio/ags_noise_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_chorus_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_MODULAR_SYNTH_AUDIO                (ags_fx_modular_synth_audio_get_type())
#define AGS_FX_MODULAR_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_MODULAR_SYNTH_AUDIO, AgsFxModularSynthAudio))
#define AGS_FX_MODULAR_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_MODULAR_SYNTH_AUDIO, AgsFxModularSynthAudioClass))
#define AGS_IS_FX_MODULAR_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_MODULAR_SYNTH_AUDIO))
#define AGS_IS_FX_MODULAR_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_MODULAR_SYNTH_AUDIO))
#define AGS_FX_MODULAR_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_MODULAR_SYNTH_AUDIO, AgsFxModularSynthAudioClass))

#define AGS_FX_MODULAR_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE (65536)

typedef struct _AgsFxModularSynthAudio AgsFxModularSynthAudio;
typedef struct _AgsFxModularSynthAudioScopeData AgsFxModularSynthAudioScopeData;
typedef struct _AgsFxModularSynthAudioChannelData AgsFxModularSynthAudioChannelData;
typedef struct _AgsFxModularSynthAudioInputData AgsFxModularSynthAudioInputData;
typedef struct _AgsFxModularSynthAudioClass AgsFxModularSynthAudioClass;

struct _AgsFxModularSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;

  AgsPort *synth_0_osc_0_oscillator;
  
  AgsPort *synth_0_osc_0_octave;
  AgsPort *synth_0_osc_0_key;

  AgsPort *synth_0_osc_0_phase;
  AgsPort *synth_0_osc_0_volume;
  
  AgsPort *synth_0_osc_1_oscillator;
  
  AgsPort *synth_0_osc_1_octave;
  AgsPort *synth_0_osc_1_key;

  AgsPort *synth_0_osc_1_phase;
  AgsPort *synth_0_osc_1_volume;
  
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

  AgsPort *synth_0_noise_frequency;
  AgsPort *synth_0_noise_gain;

  AgsPort *synth_0_noise_sends;

  AgsPort *low_pass_0_cut_off_frequency;
  AgsPort *low_pass_0_filter_gain;
  AgsPort *low_pass_0_no_clip;

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
  
  AgsFxModularSynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxModularSynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxModularSynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxModularSynthAudioChannelData **channel_data;
};

struct _AgsFxModularSynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  gpointer synth_buffer_0;
  
  AgsModularSynthUtil *modular_synth_util_0;

  AgsLowPassFilterUtil *low_pass_filter_util_0;

  AgsAmplifierUtil *amplifier_util_0;

  AgsChorusUtil *chorus_util;

  AgsFxModularSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxModularSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_modular_synth_audio_get_type();

AgsFxModularSynthAudioScopeData* ags_fx_modular_synth_audio_scope_data_alloc();
void ags_fx_modular_synth_audio_scope_data_free(AgsFxModularSynthAudioScopeData *scope_data);

AgsFxModularSynthAudioChannelData* ags_fx_modular_synth_audio_channel_data_alloc();
void ags_fx_modular_synth_audio_channel_data_free(AgsFxModularSynthAudioChannelData *channel_data);

AgsFxModularSynthAudioInputData* ags_fx_modular_synth_audio_input_data_alloc();
void ags_fx_modular_synth_audio_input_data_free(AgsFxModularSynthAudioInputData *input_data);

/* instantiate */
AgsFxModularSynthAudio* ags_fx_modular_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_MODULAR_SYNTH_AUDIO_H__*/
