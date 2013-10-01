/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __AGS_INIT_CHANNEL_H__
#define __AGS_INIT_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_INIT_CHANNEL                (ags_init_channel_get_type())
#define AGS_INIT_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INIT_CHANNEL, AgsInitChannel))
#define AGS_INIT_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INIT_CHANNEL, AgsInitChannelClass))
#define AGS_IS_INIT_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INIT_CHANNEL))
#define AGS_IS_INIT_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INIT_CHANNEL))
#define AGS_INIT_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INIT_CHANNEL, AgsInitChannelClass))

typedef struct _AgsInitChannel AgsInitChannel;
typedef struct _AgsInitChannelClass AgsInitChannelClass;

struct _AgsInitChannel
{
  AgsTask task;

  AgsChannel *channel;
  gboolean play_pad;

  AgsGroupId group_id[3];
  AgsGroupId child_group_id[3];

  gboolean playback;
  gboolean sequencer;
  gboolean notation;
};

struct _AgsInitChannelClass
{
  AgsTaskClass task;
};

GType ags_init_channel_get_type();

AgsInitChannel* ags_init_channel_new(AgsChannel *channel, gboolean play_pad,
				     AgsGroupId group_id[3], AgsGroupId child_group_id[3],
				     gboolean playback, gboolean sequencer, gboolean notation);

#endif /*__AGS_INIT_CHANNEL_H__*/
