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

#ifndef __AGS_RESIZE_AUDIO_H__
#define __AGS_RESIZE_AUDIO_H__

#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/audio/ags_audio.h>

#define AGS_TYPE_RESIZE_AUDIO                (ags_resize_audio_get_type())
#define AGS_RESIZE_AUDIO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESIZE_AUDIO, AgsResizeAudio))
#define AGS_RESIZE_AUDIO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESIZE_AUDIO, AgsResizeAudioClass))
#define AGS_IS_RESIZE_AUDIO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESIZE_AUDIO))
#define AGS_IS_RESIZE_AUDIO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESIZE_AUDIO))
#define AGS_RESIZE_AUDIO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESIZE_AUDIO, AgsResizeAudioClass))

typedef struct _AgsResizeAudio AgsResizeAudio;
typedef struct _AgsResizeAudioClass AgsResizeAudioClass;

struct _AgsResizeAudio
{
  AgsTask task;

  AgsAudio *audio;
  guint output_pads;
  guint input_pads;
  guint audio_channels;
};

struct _AgsResizeAudioClass
{
  AgsTaskClass task;
};

GType ags_resize_audio_get_type();

AgsResizeAudio* ags_resize_audio_new(AgsAudio *audio,
				     guint output_pads,
				     guint input_pads,
				     guint audio_channels);

#endif /*__AGS_RESIZE_AUDIO_H__*/
