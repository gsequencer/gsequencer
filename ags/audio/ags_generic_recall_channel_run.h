/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_GENERIC_RECALL_CHANNEL_RUN_H__
#define __AGS_GENERIC_RECALL_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>

#define AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN                (ags_generic_recall_channel_run_get_type())
#define AGS_GENERIC_RECALL_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN, AgsGenericRecallChannelRun))
#define AGS_GENERIC_RECALL_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN, AgsGenericRecallChannelRunClass))
#define AGS_IS_GENERIC_RECALL_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN))
#define AGS_IS_GENERIC_RECALL_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN))
#define AGS_GENERIC_RECALL_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_GENERIC_RECALL_CHANNEL_RUN, AgsGenericRecallChannelRunClass))

typedef struct _AgsGenericRecallChannelRun AgsGenericRecallChannelRun;
typedef struct _AgsGenericRecallChannelRunClass AgsGenericRecallChannelRunClass;

struct _AgsGenericRecallChannelRun
{
  AgsRecallChannelRun recall_channel_run;

  GType recycling_dummy_child_type;
};

struct _AgsGenericRecallChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_generic_recall_channel_run_get_type();

AgsGenericRecallChannelRun* ags_generic_recall_channel_run_new(AgsChannel *source,
							       GType child_type,
							       GType recycling_dummy_child_type);

#endif /*__AGS_GENERIC_RECALL_CHANNEL_RUN_H__*/
