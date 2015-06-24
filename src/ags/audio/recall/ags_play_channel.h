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

#ifndef __AGS_PLAY_CHANNEL_H__
#define __AGS_PLAY_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_PLAY_CHANNEL                (ags_play_channel_get_type())
#define AGS_PLAY_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_CHANNEL, AgsPlayChannel))
#define AGS_PLAY_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_CHANNEL, AgsPlayChannelClass))
#define AGS_IS_PLAY_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAY_CHANNEL))
#define AGS_IS_PLAY_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLAY_CHANNEL))
#define AGS_PLAY_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLAY_CHANNEL, AgsPlayChannelClass))

typedef struct _AgsPlayChannel AgsPlayChannel;
typedef struct _AgsPlayChannelClass AgsPlayChannelClass;

struct _AgsPlayChannel
{
  AgsRecallChannel recall_channel;

  AgsPort *audio_channel;

  AgsPort *muted;
};

struct _AgsPlayChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_play_channel_get_type();

AgsPlayChannel* ags_play_channel_new(AgsDevout *devout,
				     guint audio_channel);

#endif /*__AGS_PLAY_CHANNEL_H__*/
