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

#ifndef __AGS_PLAY_CHANNEL_RUN_MASTER_H__
#define __AGS_PLAY_CHANNEL_RUN_MASTER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>

#include <ags/audio/recall/ags_stream_channel_run.h>

#define AGS_TYPE_PLAY_CHANNEL_RUN_MASTER                (ags_play_channel_run_master_get_type())
#define AGS_PLAY_CHANNEL_RUN_MASTER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAY_CHANNEL_RUN_MASTER, AgsPlayChannelRunMaster))
#define AGS_PLAY_CHANNEL_RUN_MASTER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAY_CHANNEL_RUN_MASTER, AgsPlayChannelRunMasterClass))
#define AGS_IS_PLAY_CHANNEL_RUN_MASTER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAY_CHANNEL_RUN_MASTER))
#define AGS_IS_PLAY_CHANNEL_RUN_MASTER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLAY_CHANNEL_RUN_MASTER))
#define AGS_PLAY_CHANNEL_RUN_MASTER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLAY_CHANNEL_RUN_MASTER, AgsPlayChannelRunMasterClass))
#define AGS_PLAY_CHANNEL_RUN_MASTER_STREAMER(strct)     ((AgsPlayChannelRunMasterStreamer *)(strct))

typedef struct _AgsPlayChannelRunMaster AgsPlayChannelRunMaster;
typedef struct _AgsPlayChannelRunMasterClass AgsPlayChannelRunMasterClass;
typedef struct _AgsPlayChannelRunMasterStreamer AgsPlayChannelRunMasterStreamer;

typedef enum{
  AGS_PLAY_CHANNEL_RUN_MASTER_TERMINATING        = 1,
}AgsPlayChannelRunMasterFlags;

struct _AgsPlayChannelRunMaster
{
  AgsRecallChannelRun recall_channel_run;

  guint flags;

  GList *streamer;

  gulong source_recycling_changed_handler;
};

struct _AgsPlayChannelRunMasterClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

struct _AgsPlayChannelRunMasterStreamer
{
  AgsPlayChannelRunMaster *play_channel_run_master;

  AgsStreamChannelRun *stream_channel_run;
  gulong done_handler;
};

GType ags_play_channel_run_master_get_type();

AgsPlayChannelRunMasterStreamer* ags_play_channel_run_master_streamer_alloc(AgsPlayChannelRunMaster *play_channel_run_master,
									    AgsStreamChannelRun *stream_channel_run);
GList* ags_play_channel_run_master_find_streamer(GList *list,
						 AgsStreamChannelRun *stream_channel_run);

AgsPlayChannelRunMaster* ags_play_channel_run_master_new();

#endif /*__AGS_PLAY_CHANNEL_RUN_MASTER_H__*/
