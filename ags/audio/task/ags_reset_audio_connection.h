/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_RESET_AUDIO_CONNECTION_H__
#define __AGS_RESET_AUDIO_CONNECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_audio.h>

#define AGS_TYPE_RESET_AUDIO_CONNECTION                (ags_reset_audio_connection_get_type())
#define AGS_RESET_AUDIO_CONNECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESET_AUDIO_CONNECTION, AgsResetAudioConnection))
#define AGS_RESET_AUDIO_CONNECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESET_AUDIO_CONNECTION, AgsResetAudioConnectionClass))
#define AGS_IS_RESET_AUDIO_CONNECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESET_AUDIO_CONNECTION))
#define AGS_IS_RESET_AUDIO_CONNECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESET_AUDIO_CONNECTION))
#define AGS_RESET_AUDIO_CONNECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESET_AUDIO_CONNECTION, AgsResetAudioConnectionClass))

typedef struct _AgsResetAudioConnection AgsResetAudioConnection;
typedef struct _AgsResetAudioConnectionClass AgsResetAudioConnectionClass;

struct _AgsResetAudioConnection
{
  AgsTask task;

  GObject *soundcard;

  AgsAudio *audio;
  GType channel_type;
  
  guint pad;
  guint audio_channel;
  guint mapped_line;
};

struct _AgsResetAudioConnectionClass
{
  AgsTaskClass task;
};

GType ags_reset_audio_connection_get_type();

AgsResetAudioConnection* ags_reset_audio_connection_new(GObject *soundcard,
							AgsAudio *audio,
							GType channel_type,
							guint pad,
							guint audio_channel,
							guint mapped_line);

#endif /*__AGS_RESET_AUDIO_CONNECTION_H__*/
