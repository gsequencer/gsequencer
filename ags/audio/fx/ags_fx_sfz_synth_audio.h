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

#ifndef __AGS_FX_SFZ_SYNTH_AUDIO_H__
#define __AGS_FX_SFZ_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_sfz_synth_util.h>
#include <ags/audio/ags_common_pitch_util.h>
#include <ags/audio/ags_volume_util.h>
#include <ags/audio/ags_chorus_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_SFZ_SYNTH_AUDIO                (ags_fx_sfz_synth_audio_get_type())
#define AGS_FX_SFZ_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_SFZ_SYNTH_AUDIO, AgsFxSFZSynthAudio))
#define AGS_FX_SFZ_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_SFZ_SYNTH_AUDIO, AgsFxSFZSynthAudioClass))
#define AGS_IS_FX_SFZ_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_SFZ_SYNTH_AUDIO))
#define AGS_IS_FX_SFZ_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_SFZ_SYNTH_AUDIO))
#define AGS_FX_SFZ_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_SFZ_SYNTH_AUDIO, AgsFxSFZSynthAudioClass))

typedef struct _AgsFxSFZSynthAudio AgsFxSFZSynthAudio;
typedef struct _AgsFxSFZSynthAudioScopeData AgsFxSFZSynthAudioScopeData;
typedef struct _AgsFxSFZSynthAudioChannelData AgsFxSFZSynthAudioChannelData;
typedef struct _AgsFxSFZSynthAudioInputData AgsFxSFZSynthAudioInputData;
typedef struct _AgsFxSFZSynthAudioClass AgsFxSFZSynthAudioClass;

struct _AgsFxSFZSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;
  
  AgsPort *synth_octave;
  AgsPort *synth_key;
  
  AgsPort *synth_pitch_type;

  AgsPort *synth_volume;  
  
  AgsPort *chorus_enabled;
  
  AgsPort *chorus_pitch_type;

  AgsPort *chorus_input_volume;
  AgsPort *chorus_output_volume;
  
  AgsPort *chorus_lfo_oscillator;
  AgsPort *chorus_lfo_frequency;

  AgsPort *chorus_depth;
  AgsPort *chorus_mix;
  AgsPort *chorus_delay;
 
  AgsFxSFZSynthAudioScopeData* scope_data[AGS_SOUND_SCOPE_LAST];
};

struct _AgsFxSFZSynthAudioClass
{
  AgsFxNotationAudioClass fx_notation_audio;
};

struct _AgsFxSFZSynthAudioScopeData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
  
  guint audio_channels;
  
  AgsFxSFZSynthAudioChannelData **channel_data;
};

struct _AgsFxSFZSynthAudioChannelData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  AgsSFZSynthUtil *synth;
    
  AgsChorusUtil *chorus_util;
  
  AgsFxSFZSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxSFZSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  guint key_on;
};

GType ags_fx_sfz_synth_audio_get_type();

AgsFxSFZSynthAudioScopeData* ags_fx_sfz_synth_audio_scope_data_alloc();
void ags_fx_sfz_synth_audio_scope_data_free(AgsFxSFZSynthAudioScopeData *scope_data);

AgsFxSFZSynthAudioChannelData* ags_fx_sfz_synth_audio_channel_data_alloc();
void ags_fx_sfz_synth_audio_channel_data_free(AgsFxSFZSynthAudioChannelData *channel_data);

AgsFxSFZSynthAudioInputData* ags_fx_sfz_synth_audio_input_data_alloc();
void ags_fx_sfz_synth_audio_input_data_free(AgsFxSFZSynthAudioInputData *input_data);

/* instantiate */
AgsFxSFZSynthAudio* ags_fx_sfz_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_SFZ_SYNTH_AUDIO_H__*/
