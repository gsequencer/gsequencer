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

#ifndef __AGS_CHANNEL_SET_RECYCLING_H__
#define __AGS_CHANNEL_SET_RECYCLING_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_CHANNEL_SET_RECYCLING                (ags_channel_set_recycling_get_type())
#define AGS_CHANNEL_SET_RECYCLING(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANNEL_SET_RECYCLING, AgsChannelSetRecycling))
#define AGS_CHANNEL_SET_RECYCLING_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANNEL_SET_RECYCLING, AgsChannelSetRecyclingClass))
#define AGS_IS_CHANNEL_SET_RECYCLING(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANNEL_SET_RECYCLING))
#define AGS_IS_CHANNEL_SET_RECYCLING_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CHANNEL_SET_RECYCLING))
#define AGS_CHANNEL_SET_RECYCLING_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANNEL_SET_RECYCLING, AgsChannelSetRecyclingClass))

typedef struct _AgsChannelSetRecycling AgsChannelSetRecycling;
typedef struct _AgsChannelSetRecyclingClass AgsChannelSetRecyclingClass;

struct _AgsChannelSetRecycling
{
  AgsTask task;

  AgsChannel *channel;

  AgsRecycling *first_recycling;
  AgsRecycling *last_recycling;
};

struct _AgsChannelSetRecyclingClass
{
  AgsTaskClass task;
};

GType ags_channel_set_recycling_get_type();

AgsChannelSetRecycling* ags_channel_set_recycling_new(AgsChannel *channel,
						      AgsRecycling *first_recycling,
						      AgsRecycling *last_recycling);

#endif /*__AGS_CHANNEL_SET_RECYCLING_H__*/

