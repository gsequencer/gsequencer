/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_FX_SYNTH_AUDIO_H__
#define __AGS_FX_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_synth_util.h>
#include <ags/audio/ags_noise_util.h>
#include <ags/audio/ags_hq_pitch_util.h>
#include <ags/audio/ags_chorus_util.h>
#include <ags/audio/ags_fluid_iir_filter_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_SYNTH_AUDIO                (ags_fx_synth_audio_get_type())
#define AGS_FX_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_SYNTH_AUDIO, AgsFxSynthAudio))
#define AGS_FX_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_SYNTH_AUDIO, AgsFxSynthAudioClass))
#define AGS_IS_FX_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_SYNTH_AUDIO))
#define AGS_IS_FX_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_SYNTH_AUDIO))
#define AGS_FX_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_SYNTH_AUDIO, AgsFxSynthAudioClass))

#define AGS_FX_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE (65536)

typedef struct _AgsFxSynthAudio AgsFxSynthAudio;
typedef struct _AgsFxSynthAudioScopeData AgsFxSynthAudioScopeData;
typedef struct _AgsFxSynthAudioChannelData AgsFxSynthAudioChannelData;
typedef struct _AgsFxSynthAudioInputData AgsFxSynthAudioInputData;
typedef struct _AgsFxSynthAudioClass AgsFxSynthAudioClass;

struct _AgsFxSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;

  AgsPort *synth_0_oscillator;
  
  AgsPort *synth_0_octave;
  AgsPort *synth_0_key;

  AgsPort *synth_0_phase;
  AgsPort *synth_0_volume;

  AgsPort *synth_0_sync_enabled;

  AgsPort *synth_0_sync_relative_attack_factor;

  AgsPort *synth_0_sync_attack_0;
  AgsPort *synth_0_sync_phase_0;

  AgsPort *synth_0_sync_attack_1;
  AgsPort *synth_0_sync_phase_1;

  AgsPort *synth_0_sync_attack_2;
  AgsPort *synth_0_sync_phase_2;

  AgsPort *synth_0_sync_lfo_oscillator;
  AgsPort *synth_0_sync_lfo_frequency;

  AgsPort *synth_1_oscillator;
  
  AgsPort *synth_1_octave;
  AgsPort *synth_1_key;

  AgsPort *synth_1_phase;
  AgsPort *synth_1_volume;

  AgsPort *synth_1_sync_enabled;

  AgsPort *synth_1_sync_relative_attack_factor;

  AgsPort *synth_1_sync_attack_0;
  AgsPort *synth_1_sync_phase_0;

  AgsPort *synth_1_sync_attack_1;
  AgsPort *synth_1_sync_phase_1;

  AgsPort *synth_1_sync_attack_2;
  AgsPort *synth_1_sync_phase_2;

  AgsPort *synth_1_sync_lfo_oscillator;
  AgsPort *synth_1_sync_lfo_frequency;

  AgsPort *sequencer_enabled;
  AgsPort *sequencer_sign;
  
  AgsPort *pitch_tuning;
  
  AgsPort *noise_gain;

  AgsPort *chorus_enabled;
  
  AgsPort *chorus_input_volume;
  AgsPort *chorus_output_volume;
  
  AgsPort *chorus_lfo_oscillator;
  AgsPort *chorus_lfo_frequency;

  AgsPort *chorus_depth;
  AgsPort *chorus_mix;
  AgsPort *chorus_delay;

  AgsPort *low_pass_enabled;

  AgsPort *low_pass_q_lin;
  AgsPort *low_pass_filter_gain;

  AgsPort *high_pass_enabled;

  AgsPort *high_pass_q_lin;
  AgsPort *high_pass_filter_gain;
  
  AgsFxSynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxSynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxSynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxSynthAudioChannelData **channel_data;
};

struct _AgsFxSynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  AgsSynthUtil synth_0;
  AgsSynthUtil synth_1;

  AgsNoiseUtil noise_util;
  
  AgsHQPitchUtil hq_pitch_util;
  AgsLinearInterpolateUtil hq_pitch_linear_interpolate_util;
  
  AgsChorusUtil chorus_util;
  AgsHQPitchUtil chorus_hq_pitch_util;
  AgsLinearInterpolateUtil chorus_linear_interpolate_util;
  
  AgsFluidIIRFilterUtil low_pass_filter;
  AgsFluidIIRFilterUtil high_pass_filter;

  AgsFxSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_synth_audio_get_type();

AgsFxSynthAudioScopeData* ags_fx_synth_audio_scope_data_alloc();
void ags_fx_synth_audio_scope_data_free(AgsFxSynthAudioScopeData *scope_data);

AgsFxSynthAudioChannelData* ags_fx_synth_audio_channel_data_alloc();
void ags_fx_synth_audio_channel_data_free(AgsFxSynthAudioChannelData *channel_data);

AgsFxSynthAudioInputData* ags_fx_synth_audio_input_data_alloc();
void ags_fx_synth_audio_input_data_free(AgsFxSynthAudioInputData *input_data);

/* instantiate */
AgsFxSynthAudio* ags_fx_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_SYNTH_AUDIO_H__*/
