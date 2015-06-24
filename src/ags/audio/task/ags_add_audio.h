/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_ADD_AUDIO_H__
#define __AGS_ADD_AUDIO_H__

#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>

#define AGS_TYPE_ADD_AUDIO                (ags_add_audio_get_type())
#define AGS_ADD_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_AUDIO, AgsAddAudio))
#define AGS_ADD_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_AUDIO, AgsAddAudioClass))
#define AGS_IS_ADD_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_AUDIO))
#define AGS_IS_ADD_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_AUDIO))
#define AGS_ADD_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_AUDIO, AgsAddAudioClass))

typedef struct _AgsAddAudio AgsAddAudio;
typedef struct _AgsAddAudioClass AgsAddAudioClass;

struct _AgsAddAudio
{
  AgsTask task;

  AgsDevout *devout;
  AgsAudio *audio;
};

struct _AgsAddAudioClass
{
  AgsTaskClass task;
};

GType ags_add_audio_get_type();

AgsAddAudio* ags_add_audio_new(AgsDevout *devout,
			       AgsAudio *audio);

#endif /*__AGS_ADD_AUDIO_H__*/
