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

#ifndef __AGS_FX_FM_SYNTH_AUDIO_H__
#define __AGS_FX_FM_SYNTH_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_fm_synth_util.h>

#include <ags/audio/fx/ags_fx_notation_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_FM_SYNTH_AUDIO                (ags_fx_fm_synth_audio_get_type())
#define AGS_FX_FM_SYNTH_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_FM_SYNTH_AUDIO, AgsFxFMSynthAudio))
#define AGS_FX_FM_SYNTH_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_FM_SYNTH_AUDIO, AgsFxFMSynthAudioClass))
#define AGS_IS_FX_FM_SYNTH_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_FM_SYNTH_AUDIO))
#define AGS_IS_FX_FM_SYNTH_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_FM_SYNTH_AUDIO))
#define AGS_FX_FM_SYNTH_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_FM_SYNTH_AUDIO, AgsFxFMSynthAudioClass))

typedef struct _AgsFxFMSynthAudio AgsFxFMSynthAudio;
typedef struct _AgsFxFMSynthAudioScopeData AgsFxFMSynthAudioScopeData;
typedef struct _AgsFxFMSynthAudioChannelData AgsFxFMSynthAudioChannelData;
typedef struct _AgsFxFMSynthAudioInputData AgsFxFMSynthAudioInputData;
typedef struct _AgsFxFMSynthAudioClass AgsFxFMSynthAudioClass;

struct _AgsFxFMSynthAudio
{
  AgsFxNotationAudio fx_notation_audio;

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

  AgsFxFMSynthAudioInputData* input_data[AGS_SEQUENCER_MAX_MIDI_KEYS];
};

struct _AgsFxFMSynthAudioInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;
};

GType ags_fx_fm_synth_audio_get_type();

/* instantiate */
AgsFxFMSynthAudio* ags_fx_fm_synth_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_FM_SYNTH_AUDIO_H__*/
