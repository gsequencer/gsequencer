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

#ifndef __AGS_RECALL_CHANNEL_RUN_DUMMY_H__
#define __AGS_RECALL_CHANNEL_RUN_DUMMY_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#define AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY                (ags_recall_channel_run_dummy_get_type())
#define AGS_RECALL_CHANNEL_RUN_DUMMY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY, AgsRecallChannelRunDummy))
#define AGS_RECALL_CHANNEL_RUN_DUMMY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY, AgsRecallChannelRunDummyClass))
#define AGS_IS_RECALL_CHANNEL_RUN_DUMMY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY))
#define AGS_IS_RECALL_CHANNEL_RUN_DUMMY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY))
#define AGS_RECALL_CHANNEL_RUN_DUMMY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY, AgsRecallChannelRunDummyClass))

typedef struct _AgsRecallChannelRunDummy AgsRecallChannelRunDummy;
typedef struct _AgsRecallChannelRunDummyClass AgsRecallChannelRunDummyClass;

struct _AgsRecallChannelRunDummy
{
  AgsRecallChannelRun recall_channel_run;

  GType recycling_dummy_child_type;
};

struct _AgsRecallChannelRunDummyClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_recall_channel_run_dummy_get_type();

AgsRecallChannelRunDummy* ags_recall_channel_run_dummy_new(AgsChannel *source,
							   GType child_type,
							   GType recycling_dummy_child_type);

#endif /*__AGS_RECALL_CHANNEL_RUN_DUMMY_H__*/

