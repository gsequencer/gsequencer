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

#ifndef __AGS_CANCEL_CHANNEL_H__
#define __AGS_CANCEL_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_CANCEL_CHANNEL                (ags_cancel_channel_get_type())
#define AGS_CANCEL_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CANCEL_CHANNEL, AgsCancelChannel))
#define AGS_CANCEL_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CANCEL_CHANNEL, AgsCancelChannelClass))
#define AGS_IS_CANCEL_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CANCEL_CHANNEL))
#define AGS_IS_CANCEL_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CANCEL_CHANNEL))
#define AGS_CANCEL_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CANCEL_CHANNEL, AgsCancelChannelClass))

typedef struct _AgsCancelChannel AgsCancelChannel;
typedef struct _AgsCancelChannelClass AgsCancelChannelClass;

struct _AgsCancelChannel
{
  AgsTask task;

  AgsChannel *channel;
  AgsRecallID *recall_id;

  GObject *playback;
};

struct _AgsCancelChannelClass
{
  AgsTaskClass task;
};

GType ags_cancel_channel_get_type();

AgsCancelChannel* ags_cancel_channel_new(AgsChannel *channel, AgsRecallID *recall_id,
					 GObject *playback);

#endif /*__AGS_CANCEL_CHANNEL_H__*/
