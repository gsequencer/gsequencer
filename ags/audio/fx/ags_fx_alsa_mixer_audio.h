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

#ifndef __AGS_FX_ALSA_MIXER_AUDIO_H__
#define __AGS_FX_ALSA_MIXER_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_ALSA_MIXER_AUDIO                (ags_fx_alsa_mixer_audio_get_type())
#define AGS_FX_ALSA_MIXER_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_ALSA_MIXER_AUDIO, AgsFxAlsaMixerAudio))
#define AGS_FX_ALSA_MIXER_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_ALSA_MIXER_AUDIO, AgsFxAlsaMixerAudioClass))
#define AGS_IS_FX_ALSA_MIXER_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_ALSA_MIXER_AUDIO))
#define AGS_IS_FX_ALSA_MIXER_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_ALSA_MIXER_AUDIO))
#define AGS_FX_ALSA_MIXER_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_ALSA_MIXER_AUDIO, AgsFxAlsaMixerAudioClass))

typedef struct _AgsFxAlsaMixerAudio AgsFxAlsaMixerAudio;
typedef struct _AgsFxAlsaMixerAudioClass AgsFxAlsaMixerAudioClass;

struct _AgsFxAlsaMixerAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *muted;
};

struct _AgsFxAlsaMixerAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_fx_alsa_mixer_audio_get_type();

/* instantiate */
AgsFxAlsaMixerAudio* ags_fx_alsa_mixer_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_ALSA_MIXER_AUDIO_H__*/
