/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_PLAY_WAVE_AUDIO_H__
#define __AGS_PLAY_WAVE_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>
#include <ags/audio/ags_port.h>

#define AGS_TYPE_PLAY_WAVE_AUDIO                (ags_play_wave_audio_get_type())
#define AGS_PLAY_WAVE_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_WAVE_AUDIO, AgsPlayWaveAudio))
#define AGS_PLAY_WAVE_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_WAVE_AUDIO, AgsPlayWaveAudio))
#define AGS_IS_PLAY_WAVE_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAY_WAVE_AUDIO))
#define AGS_IS_PLAY_WAVE_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAY_WAVE_AUDIO))
#define AGS_PLAY_WAVE_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PLAY_WAVE_AUDIO, AgsPlayWaveAudioClass))

typedef struct _AgsPlayWaveAudio AgsPlayWaveAudio;
typedef struct _AgsPlayWaveAudioClass AgsPlayWaveAudioClass;

struct _AgsPlayWaveAudio
{
  AgsRecallAudio recall_audio;

  AgsPort *wave_loop;
  AgsPort *wave_loop_start;
  AgsPort *wave_loop_end;
};

struct _AgsPlayWaveAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_play_wave_audio_get_type();

AgsPlayWaveAudio* ags_play_wave_audio_new(AgsAudio *audio);

#endif /*__AGS_PLAY_WAVE_AUDIO_H__*/
