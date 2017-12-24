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

#ifndef __AGS_AUDIO_CONNECTION_H__
#define __AGS_AUDIO_CONNECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_AUDIO_CONNECTION                (ags_audio_connection_get_type())
#define AGS_AUDIO_CONNECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_CONNECTION, AgsAudioConnection))
#define AGS_AUDIO_CONNECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_CONNECTION, AgsAudioConnection))
#define AGS_IS_AUDIO_CONNECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_CONNECTION))
#define AGS_IS_AUDIO_CONNECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_CONNECTION))
#define AGS_AUDIO_CONNECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_CONNECTION, AgsAudioConnectionClass))

typedef struct _AgsAudioConnection AgsAudioConnection;
typedef struct _AgsAudioConnectionClass AgsAudioConnectionClass;

/**
 * AgsAudioConnectionFlags:
 * @AGS_AUDIO_CONNECTION_IS_AUDIO: the connection is related to audio
 * @AGS_AUDIO_CONNECTION_IS_OUTPUT: the connection is related to output
 * @AGS_AUDIO_CONNECTION_IS_INPUT: the connection is related to input
 * @AGS_AUDIO_CONNECTION_IS_REMOTE: the connection is related to remote
 * @AGS_AUDIO_CONNECTION_IS_SOUNDCARD_DATA: the connection is related to soundcard data
 * @AGS_AUDIO_CONNECTION_IS_SEQUENCER_DATA: the connection is related to sequencer data
 * @AGS_AUDIO_CONNECTION_SCOPE_AUDIO: it applies to audio scope
 * @AGS_AUDIO_CONNECTION_SCOPE_CHANNEL: it applies to audio scope
 * @AGS_AUDIO_CONNECTION_SCOPE_PAD: it applies to audio scope
 * @AGS_AUDIO_CONNECTION_SCOPE_LINE: it applies to audio scope
 * 
 * Enum values to control the behavior or indicate internal state of #AgsAudioConnection by
 * enable/disable as flags.
 */
typedef enum{
  AGS_AUDIO_CONNECTION_IS_AUDIO           = 1,
  AGS_AUDIO_CONNECTION_IS_OUTPUT          = 1,
  AGS_AUDIO_CONNECTION_IS_INPUT           = 1 <<  1,
  AGS_AUDIO_CONNECTION_IS_REMOTE          = 1 <<  3,
  AGS_AUDIO_CONNECTION_IS_SOUNDCARD_DATA  = 1 <<  4,
  AGS_AUDIO_CONNECTION_IS_SEQUENCER_DATA  = 1 <<  5,
  AGS_AUDIO_CONNECTION_SCOPE_AUDIO        = 1 <<  6,
  AGS_AUDIO_CONNECTION_SCOPE_CHANNEL      = 1 <<  7,
  AGS_AUDIO_CONNECTION_SCOPE_PAD          = 1 <<  8,
  AGS_AUDIO_CONNECTION_SCOPE_LINE         = 1 <<  9,
}AgsAudioConnectionFlags;

struct _AgsAudioConnection
{
  AgsConnection connection;

  guint flags;

  GObject *audio;
  GType channel_type;
  
  guint pad;
  guint audio_channel;
  guint line;

  guint mapped_line;
};

struct _AgsAudioConnectionClass
{
  AgsConnectionClass connection;
};

GType ags_audio_connection_get_type();

GList* ags_audio_connection_find(GList *list,
				 GType channel_type,
				 guint pad,
				 guint audio_channel);

AgsAudioConnection* ags_audio_connection_new();

#endif /*__AGS_AUDIO_CONNECTION_H__*/
