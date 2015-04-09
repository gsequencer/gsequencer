/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_CLONE_CHANNEL_RUN_H__
#define __AGS_CLONE_CHANNEL_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_CLONE_CHANNEL_RUN                (ags_clone_channel_run_get_type())
#define AGS_CLONE_CHANNEL_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CLONE_CHANNEL_RUN, AgsCloneChannelRun))
#define AGS_CLONE_CHANNEL_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CLONE_CHANNEL_RUN, AgsCloneChannelRunClass))
#define AGS_IS_CLONE_CHANNEL_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CLONE_CHANNEL_RUN))
#define AGS_IS_CLONE_CHANNEL_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CLONE_CHANNEL_RUN))
#define AGS_CLONE_CHANNEL_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CLONE_CHANNEL_RUN, AgsCloneChannelRunClass))

typedef struct _AgsCloneChannelRun AgsCloneChannelRun;
typedef struct _AgsCloneChannelRunClass AgsCloneChannelRunClass;

struct _AgsCloneChannelRun
{
  AgsRecallChannelRun recall_channel_run;
};

struct _AgsCloneChannelRunClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_clone_channel_run_get_type();

AgsCloneChannelRun* ags_clone_channel_run_new();

#endif /*__AGS_CLONE_CHANNEL_RUN_H__*/
