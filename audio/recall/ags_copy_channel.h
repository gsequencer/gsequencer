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

#ifndef __AGS_COPY_CHANNEL_H__
#define __AGS_COPY_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel_run.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_COPY_CHANNEL                (ags_copy_channel_get_type())
#define AGS_COPY_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_COPY_CHANNEL, AgsCopyChannel))
#define AGS_COPY_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_COPY_CHANNEL, AgsCopyChannelClass))
#define AGS_IS_COPY_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_COPY_CHANNEL))
#define AGS_IS_COPY_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_COPY_CHANNEL))
#define AGS_COPY_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_COPY_CHANNEL, AgsCopyChannelClass))

typedef struct _AgsCopyChannel AgsCopyChannel;
typedef struct _AgsCopyChannelClass AgsCopyChannelClass;

typedef enum{
  AGS_COPY_CHANNEL_EXACT_LENGTH         = 1,
  AGS_COPY_CHANNEL_OMIT_FURTHER_ATTACK  = 1 << 1,
}AgsCopyChannelFlags;

struct _AgsCopyChannel
{
  AgsRecallChannelRun recall_channel_run;

  guint flags;

  AgsDevout *devout;

  AgsChannel *destination;
  gulong destination_recycling_changed_handler;

  AgsChannel *source;
  gulong source_recycling_changed_handler;
};

struct _AgsCopyChannelClass
{
  AgsRecallChannelRunClass recall_channel_run;
};

GType ags_copy_channel_get_type();

AgsCopyChannel* ags_copy_channel_new(AgsChannel *destination,
				     AgsChannel *source,
				     AgsDevout *devout);

#endif /*__AGS_COPY_CHANNEL_H__*/
