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

#ifndef __AGS_FX_SF2_SYNTH_AUDIO_H__
#define __AGS_FX_SF2_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_sf2_synth_util.h>
#include <ags/audio/ags_hq_pitch_util.h>
#include <ags/audio/ags_volume_util.h>
#include <ags/audio/ags_chorus_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_SF2_SYNTH_AUDIO                (ags_fx_sf2_synth_audio_get_type())
#define AGS_FX_SF2_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_SF2_SYNTH_AUDIO, AgsFxSF2SynthAudio))
#define AGS_FX_SF2_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_SF2_SYNTH_AUDIO, AgsFxSF2SynthAudioClass))
#define AGS_IS_FX_SF2_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_SF2_SYNTH_AUDIO))
#define AGS_IS_FX_SF2_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_SF2_SYNTH_AUDIO))
#define AGS_FX_SF2_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_SF2_SYNTH_AUDIO, AgsFxSF2SynthAudioClass))

#define AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE (65536)

typedef struct _AgsFxSF2SynthAudio AgsFxSF2SynthAudio;
typedef struct _AgsFxSF2SynthAudioScopeData AgsFxSF2SynthAudioScopeData;
typedef struct _AgsFxSF2SynthAudioChannelData AgsFxSF2SynthAudioChannelData;
typedef struct _AgsFxSF2SynthAudioInputData AgsFxSF2SynthAudioInputData;
typedef struct _AgsFxSF2SynthAudioClass AgsFxSF2SynthAudioClass;

struct _AgsFxSF2SynthAudio
{
  AgsFxNotationAudio fx_notation_audio;
  
  AgsPort *synth_octave;
  AgsPort *synth_key;

  AgsPort *synth_volume;
  
  AgsPort *pitch_tuning;
  
  AgsPort *chorus_enabled;
  
  AgsPort *chorus_input_volume;
  AgsPort *chorus_output_volume;
  
  AgsPort *chorus_lfo_oscillator;
  AgsPort *chorus_lfo_frequency;

  AgsPort *chorus_depth;
  AgsPort *chorus_mix;
  AgsPort *chorus_delay;

  AgsFxSF2SynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxSF2SynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxSF2SynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxSF2SynthAudioChannelData **channel_data;
};

struct _AgsFxSF2SynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  AgsSF2SynthUtil synth;
  AgsResampleUtil synth_resample_util;
  AgsHQPitchUtil synth_hq_pitch_util;
  AgsLinearInterpolateUtil synth_hq_pitch_linear_interpolate_util;
  AgsVolumeUtil synth_volume_util;
  
  AgsHQPitchUtil hq_pitch_util;
  AgsLinearInterpolateUtil hq_pitch_linear_interpolate_util;
  
  AgsChorusUtil chorus_util;
  AgsHQPitchUtil chorus_hq_pitch_util;
  AgsLinearInterpolateUtil chorus_linear_interpolate_util;
  
  AgsFxSF2SynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxSF2SynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_sf2_synth_audio_get_type();

AgsFxSF2SynthAudioScopeData* ags_fx_sf2_synth_audio_scope_data_alloc();
void ags_fx_sf2_synth_audio_scope_data_free(AgsFxSF2SynthAudioScopeData *scope_data);

AgsFxSF2SynthAudioChannelData* ags_fx_sf2_synth_audio_channel_data_alloc();
void ags_fx_sf2_synth_audio_channel_data_free(AgsFxSF2SynthAudioChannelData *channel_data);

AgsFxSF2SynthAudioInputData* ags_fx_sf2_synth_audio_input_data_alloc();
void ags_fx_sf2_synth_audio_input_data_free(AgsFxSF2SynthAudioInputData *input_data);

/* instantiate */
AgsFxSF2SynthAudio* ags_fx_sf2_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_SF2_SYNTH_AUDIO_H__*/
