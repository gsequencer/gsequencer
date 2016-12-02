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

#ifndef __AGS_FEED_CHANNEL_H__
#define __AGS_FEED_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>

#define AGS_TYPE_FEED_CHANNEL                (ags_feed_channel_get_type())
#define AGS_FEED_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FEED_CHANNEL, AgsFeedChannel))
#define AGS_FEED_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FEED_CHANNEL, AgsFeedChannelClass))
#define AGS_IS_FEED_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FEED_CHANNEL))
#define AGS_IS_FEED_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FEED_CHANNEL))
#define AGS_FEED_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FEED_CHANNEL, AgsFeedChannelClass))

typedef struct _AgsFeedChannel AgsFeedChannel;
typedef struct _AgsFeedChannelClass AgsFeedChannelClass;

struct _AgsFeedChannel
{
  AgsRecallChannel recall_channel;
};

struct _AgsFeedChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_feed_channel_get_type();

AgsFeedChannel* ags_feed_channel_new();

#endif /*__AGS_FEED_CHANNEL_H__*/
