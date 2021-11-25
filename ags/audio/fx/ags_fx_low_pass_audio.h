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

#ifndef __AGS_FX_LOW_PASS_AUDIO_H__
#define __AGS_FX_LOW_PASS_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_LOW_PASS_AUDIO                (ags_fx_low_pass_audio_get_type())
#define AGS_FX_LOW_PASS_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_LOW_PASS_AUDIO, AgsFxLowPassAudio))
#define AGS_FX_LOW_PASS_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_LOW_PASS_AUDIO, AgsFxLowPassAudioClass))
#define AGS_IS_FX_LOW_PASS_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_LOW_PASS_AUDIO))
#define AGS_IS_FX_LOW_PASS_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_LOW_PASS_AUDIO))
#define AGS_FX_LOW_PASS_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_LOW_PASS_AUDIO, AgsFxLowPassAudioClass))

typedef struct _AgsFxLowPassAudio AgsFxLowPassAudio;
typedef struct _AgsFxLowPassAudioClass AgsFxLowPassAudioClass;

struct _AgsFxLowPassAudio
{
  AgsRecallAudio recall_audio;
};

struct _AgsFxLowPassAudioClass
{
  AgsRecallAudioClass recall_audio;
};

GType ags_fx_low_pass_audio_get_type();

/* instantiate */
AgsFxLowPassAudio* ags_fx_low_pass_audio_new(AgsAudio *audio);

G_END_DECLS

#endif /*__AGS_FX_LOW_PASS_AUDIO_H__*/
