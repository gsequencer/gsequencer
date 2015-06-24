/* This file is part of GSequencer.
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

#ifndef __AGS_AUDIO_SET_RECYCLING_H__
#define __AGS_AUDIO_SET_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_AUDIO_SET_RECYCLING                (ags_audio_set_recycling_get_type())
#define AGS_AUDIO_SET_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_SET_RECYCLING, AgsAudioSetRecycling))
#define AGS_AUDIO_SET_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUDIO_SET_RECYCLING, AgsAudioSetRecyclingClass))
#define AGS_IS_AUDIO_SET_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_AUDIO_SET_RECYCLING))
#define AGS_IS_AUDIO_SET_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_AUDIO_SET_RECYCLING))
#define AGS_AUDIO_SET_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_AUDIO_SET_RECYCLING, AgsAudioSetRecyclingClass))

typedef struct _AgsAudioSetRecycling AgsAudioSetRecycling;
typedef struct _AgsAudioSetRecyclingClass AgsAudioSetRecyclingClass;

struct _AgsAudioSetRecycling
{
  AgsTask task;

  AgsAudio *audio;

  GParameter *parameter;
};

struct _AgsAudioSetRecyclingClass
{
  AgsTaskClass task;
};

GType ags_audio_set_recycling_get_type();

AgsAudioSetRecycling* ags_audio_set_recycling_new(AgsAudio *audio,
						  GParameter *parameter);

#endif /*__AGS_AUDIO_SET_RECYCLING_H__*/

