/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_CLONE_CHANNEL_H__
#define __AGS_CLONE_CHANNEL_H__

#include <glib-object.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_recall_channel.h>

#define AGS_TYPE_CLONE_CHANNEL                (ags_clone_channel_get_type())
#define AGS_CLONE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLONE_CHANNEL, AgsCloneChannel))
#define AGS_CLONE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CLONE_CHANNEL, AgsCloneChannel))
#define AGS_IS_CLONE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CLONE_CHANNEL))
#define AGS_IS_CLONE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CLONE_CHANNEL))
#define AGS_CLONE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CLONE_CHANNEL, AgsCloneChannelClass))

typedef struct _AgsCloneChannel AgsCloneChannel;
typedef struct _AgsCloneChannelClass AgsCloneChannelClass;

struct _AgsCloneChannel
{
  AgsRecallChannel recall_channel;

  guint audio_channel;
};

struct _AgsCloneChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_clone_channel_get_type();

AgsCloneChannel* ags_clone_channel_new(AgsDevout *devout,
				       guint audio_channel);

#endif /*__AGS_CLONE_CHANNEL_H__*/
