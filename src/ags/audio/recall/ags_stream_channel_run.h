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

#ifndef __AGS_STREAM_CHANNEL_RUN_H__
#define __AGS_STREAM_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/recall/ags_stream_channel.h>

#define AGS_TYPE_STREAM_CHANNEL_RUN                (ags_stream_channel_run_get_type())
#define AGS_STREAM_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_CHANNEL_RUN, AgsStreamChannelRun))
#define AGS_STREAM_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_CHANNEL_RUN, AgsStreamChannelRunClass))
#define AGS_IS_STREAM_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_STREAM_CHANNEL_RUN))
#define AGS_IS_STREAM_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_STREAM_CHANNEL_RUN))
#define AGS_STREAM_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_STREAM_CHANNEL_RUN, AgsStreamChannelRunClass))

typedef struct _AgsStreamChannelRun AgsStreamChannelRun;
typedef struct _AgsStreamChannelRunClass AgsStreamChannelRunClass;

struct _AgsStreamChannelRun
{
  AgsRecallChannelRun recall_channel_run;
};

struct _AgsStreamChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_stream_channel_run_get_type();

AgsStreamChannelRun* ags_stream_channel_run_new();

#endif /*__AGS_STREAM_CHANNEL_RUN_H__*/
