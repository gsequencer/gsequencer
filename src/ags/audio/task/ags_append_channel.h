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

#ifndef __AGS_APPEND_CHANNEL_H__
#define __AGS_APPEND_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#define AGS_TYPE_APPEND_CHANNEL                (ags_append_channel_get_type())
#define AGS_APPEND_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_APPEND_CHANNEL, AgsAppendChannel))
#define AGS_APPEND_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_APPEND_CHANNEL, AgsAppendChannelClass))
#define AGS_IS_APPEND_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_APPEND_CHANNEL))
#define AGS_IS_APPEND_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_APPEND_CHANNEL))
#define AGS_APPEND_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_APPEND_CHANNEL, AgsAppendChannelClass))

typedef struct _AgsAppendChannel AgsAppendChannel;
typedef struct _AgsAppendChannelClass AgsAppendChannelClass;

struct _AgsAppendChannel
{
  AgsTask task;

  GObject *audio_loop;
  GObject *channel;
};

struct _AgsAppendChannelClass
{
  AgsTaskClass task;
};

GType ags_append_channel_get_type();

AgsAppendChannel* ags_append_channel_new(GObject *audio_loop,
					 GObject *channel);

#endif /*__AGS_APPEND_CHANNEL_H__*/
