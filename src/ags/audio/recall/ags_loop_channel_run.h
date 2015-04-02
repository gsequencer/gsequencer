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

#ifndef __AGS_LOOP_CHANNEL_RUN_H__
#define __AGS_LOOP_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>

#define AGS_TYPE_LOOP_CHANNEL_RUN                (ags_loop_channel_run_get_type())
#define AGS_LOOP_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOOP_CHANNEL_RUN, AgsLoopChannelRun))
#define AGS_LOOP_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LOOP_CHANNEL_RUN, AgsLoopChannelRunClass))
#define AGS_IS_LOOP_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOOP_CHANNEL_RUN))
#define AGS_IS_LOOP_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOOP_CHANNEL_RUN))
#define AGS_LOOP_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LOOP_CHANNEL_RUN, AgsLoopChannelRunClass))

typedef struct _AgsLoopChannelRun AgsLoopChannelRun;
typedef struct _AgsLoopChannelRunClass AgsLoopChannelRunClass;

struct _AgsLoopChannelRun
{
  AgsRecallChannelRun recall_channel_run;

  AgsCountBeatsAudioRun *count_beats_audio_run;
  gulong start_handler;
  gulong loop_handler;
  gulong stop_handler;
};

struct _AgsLoopChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_loop_channel_run_get_type();

AgsLoopChannelRun* ags_loop_channel_run_new(AgsChannel *channel,
					    AgsCountBeatsAudioRun *count_beats_audio_run,
					    gboolean is_template);

#endif /*__AGS_LOOP_CHANNEL_RUN_H__*/
