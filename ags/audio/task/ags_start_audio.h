/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_START_AUDIO_H__
#define __AGS_START_AUDIO_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

G_BEGIN_DECLS

#define AGS_TYPE_START_AUDIO                (ags_start_audio_get_type())
#define AGS_START_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_START_AUDIO, AgsStartAudio))
#define AGS_START_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_START_AUDIO, AgsStartAudioClass))
#define AGS_IS_START_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_START_AUDIO))
#define AGS_IS_START_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_START_AUDIO))
#define AGS_START_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_START_AUDIO, AgsStartAudioClass))

typedef struct _AgsStartAudio AgsStartAudio;
typedef struct _AgsStartAudioClass AgsStartAudioClass;

struct _AgsStartAudio
{
  AgsTask task;

  AgsAudio *audio;

  gint sound_scope;
};

struct _AgsStartAudioClass
{
  AgsTaskClass task;
};

GType ags_start_audio_get_type();

AgsStartAudio* ags_start_audio_new(AgsAudio *audio,
				   gint sound_scope);

G_END_DECLS

#endif /*__AGS_START_AUDIO_H__*/
