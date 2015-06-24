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

#ifndef __AGS_VOLUME_CHANNEL_RUN_H__
#define __AGS_VOLUME_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_VOLUME_CHANNEL_RUN            (ags_volume_channel_run_get_type())
#define AGS_VOLUME_CHANNEL_RUN(obj)            (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_VOLUME_CHANNEL_RUN, AgsVolumeChannelRun))
#define AGS_VOLUME_CHANNEL_RUN_CLASS(class)    (G_TYPE_CHECK_INSTANCE_CAST(class, AGS_TYPE_VOLUME_CHANNEL_RUN, AgsVolumeChannelRunClass))
#define AGS_IS_VOLUME_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_VOLUME_CHANNEL_RUN))
#define AGS_IS_VOLUME_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_VOLUME_CHANNEL_RUN))
#define AGS_VOLUME_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_VOLUME_CHANNEL_RUN, AgsVolumeChannelRunClass))

typedef struct _AgsVolumeChannelRun AgsVolumeChannelRun;
typedef struct _AgsVolumeChannelRunClass AgsVolumeChannelRunClass;

struct _AgsVolumeChannelRun
{
  AgsRecallChannelRun recall_channel_run;
};

struct _AgsVolumeChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_volume_channel_run_get_type();

AgsVolumeChannelRun* ags_volume_channel_run_new(AgsChannel *channel);

#endif /*__AGS_VOLUME_CHANNEL_RUN_H__*/
