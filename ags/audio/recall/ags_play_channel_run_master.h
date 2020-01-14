/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_PLAY_CHANNEL_RUN_MASTER_H__
#define __AGS_PLAY_CHANNEL_RUN_MASTER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

G_BEGIN_DECLS

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

/**
 * AgsPlayChannelRunMasterFlags:
 * @AGS_PLAY_CHANNEL_RUN_MASTER_TERMINATING: recall is terminating
 *
 * Enum values to control the behavior or indicate internal state of #AgsPlayChannelRunMaster by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PLAY_CHANNEL_RUN_MASTER_TERMINATING        = 1,
}AgsPlayChannelRunMasterFlags;

struct _AgsPlayChannelRunMaster
{
  AgsRecallChannelRun recall_channel_run;

  guint flags;

  GList *stream_channel_run;
};

struct _AgsPlayChannelRunMasterClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_play_channel_run_master_get_type();

gboolean ags_play_channel_run_master_test_flags(AgsPlayChannelRunMaster *play_channel_run_master, guint flags);
void ags_play_channel_run_master_set_flags(AgsPlayChannelRunMaster *play_channel_run_master, guint flags);
void ags_play_channel_run_master_unset_flags(AgsPlayChannelRunMaster *play_channel_run_master, guint flags);

AgsPlayChannelRunMaster* ags_play_channel_run_master_new(AgsChannel *source);

G_END_DECLS

#endif /*__AGS_PLAY_CHANNEL_RUN_MASTER_H__*/
