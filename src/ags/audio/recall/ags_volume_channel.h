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

#ifndef __AGS_VOLUME_CHANNEL_H__
#define __AGS_VOLUME_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>

#define AGS_TYPE_VOLUME_CHANNEL                (ags_volume_channel_get_type())
#define AGS_VOLUME_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_VOLUME_CHANNEL, AgsVolumeChannel))
#define AGS_VOLUME_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_VOLUME_CHANNEL, AgsVolumeChannelClass))
#define AGS_IS_VOLUME_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_VOLUME_CHANNEL))
#define AGS_IS_VOLUME_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_VOLUME_CHANNEL))
#define AGS_VOLUME_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_VOLUME_CHANNEL, AgsVolumeChannelClass))

typedef struct _AgsVolumeChannel AgsVolumeChannel;
typedef struct _AgsVolumeChannelClass AgsVolumeChannelClass;

struct _AgsVolumeChannel
{
  AgsRecallChannel recall_channel;

  AgsPort *volume;
};

struct _AgsVolumeChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_volume_channel_get_type();

AgsVolumeChannel* ags_volume_channel_new();

#endif /*__AGS_VOLUME_CHANNEL_H__*/
