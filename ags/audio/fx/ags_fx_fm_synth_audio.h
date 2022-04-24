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

#ifndef __AGS_FX_FM_SYNTH_AUDIO_H__
#define __AGS_FX_FM_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_fm_synth_util.h>
#include <ags/audio/ags_noise_util.h>
#include <ags/audio/ags_hq_pitch_util.h>
#include <ags/audio/ags_chorus_util.h>
#include <ags/audio/ags_fluid_iir_filter_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_FM_SYNTH_AUDIO                (ags_fx_fm_synth_audio_get_type())
#define AGS_FX_FM_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_FM_SYNTH_AUDIO, AgsFxFMSynthAudio))
#define AGS_FX_FM_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_FM_SYNTH_AUDIO, AgsFxFMSynthAudioClass))
#define AGS_IS_FX_FM_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_FM_SYNTH_AUDIO))
#define AGS_IS_FX_FM_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_FM_SYNTH_AUDIO))
#define AGS_FX_FM_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_FM_SYNTH_AUDIO, AgsFxFMSynthAudioClass))

#define AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE (65536)

typedef struct _AgsFxFMSynthAudio AgsFxFMSynthAudio;
typedef struct _AgsFxFMSynthAudioScopeData AgsFxFMSynthAudioScopeData;
typedef struct _AgsFxFMSynthAudioChannelData AgsFxFMSynthAudioChannelData;
typedef struct _AgsFxFMSynthAudioInputData AgsFxFMSynthAudioInputData;
typedef struct _AgsFxFMSynthAudioClass AgsFxFMSynthAudioClass;

struct _AgsFxFMSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;

  AgsPort *synth_0_oscillator;
  
  AgsPort *synth_0_octave;
  AgsPort *synth_0_key;

  AgsPort *synth_0_phase;
  AgsPort *synth_0_volume;

  AgsPort *synth_0_lfo_oscillator;
  AgsPort *synth_0_lfo_frequency;

  AgsPort *synth_0_lfo_depth;
  AgsPort *synth_0_lfo_tuning;

  AgsPort *synth_1_oscillator;
  
  AgsPort *synth_1_octave;
  AgsPort *synth_1_key;

  AgsPort *synth_1_phase;
  AgsPort *synth_1_volume;

  AgsPort *synth_1_lfo_oscillator;
  AgsPort *synth_1_lfo_frequency;

  AgsPort *synth_1_lfo_depth;
  AgsPort *synth_1_lfo_tuning;

  AgsPort *synth_2_oscillator;
  
  AgsPort *synth_2_octave;
  AgsPort *synth_2_key;

  AgsPort *synth_2_phase;
  AgsPort *synth_2_volume;

  AgsPort *synth_2_lfo_oscillator;
  AgsPort *synth_2_lfo_frequency;

  AgsPort *synth_2_lfo_depth;
  AgsPort *synth_2_lfo_tuning;
  
  AgsPort *sequencer_enabled;
  AgsPort *sequencer_sign;
  
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

  AgsPort *low_pass_enabled;

  AgsPort *low_pass_q_lin;
  AgsPort *low_pass_filter_gain;

  AgsPort *high_pass_enabled;

  AgsPort *high_pass_q_lin;
  AgsPort *high_pass_filter_gain;

  AgsFxFMSynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxFMSynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxFMSynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxFMSynthAudioChannelData **channel_data;
};

struct _AgsFxFMSynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  AgsFMSynthUtil *synth_0;
  AgsFMSynthUtil *synth_1;
  AgsFMSynthUtil *synth_2;

  AgsNoiseUtil *noise_util;

  GType pitch_type;
  gpointer pitch_util;
  
  AgsChorusUtil *chorus_util;
  
  AgsFluidIIRFilterUtil *low_pass_filter;
  AgsFluidIIRFilterUtil *high_pass_filter;

  AgsFxFMSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxFMSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_fm_synth_audio_get_type();

AgsFxFMSynthAudioScopeData* ags_fx_fm_synth_audio_scope_data_alloc();
void ags_fx_fm_synth_audio_scope_data_free(AgsFxFMSynthAudioScopeData *scope_data);

AgsFxFMSynthAudioChannelData* ags_fx_fm_synth_audio_channel_data_alloc();
void ags_fx_fm_synth_audio_channel_data_free(AgsFxFMSynthAudioChannelData *channel_data);

AgsFxFMSynthAudioInputData* ags_fx_fm_synth_audio_input_data_alloc();
void ags_fx_fm_synth_audio_input_data_free(AgsFxFMSynthAudioInputData *input_data);

/* instantiate */
AgsFxFMSynthAudio* ags_fx_fm_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_FM_SYNTH_AUDIO_H__*/
