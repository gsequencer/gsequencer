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

#ifndef __AGS_LINK_CHANNEL_H__
#define __AGS_LINK_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/audio/ags_channel.h>

#define AGS_TYPE_LINK_CHANNEL                (ags_link_channel_get_type())
#define AGS_LINK_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LINK_CHANNEL, AgsLinkChannel))
#define AGS_LINK_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LINK_CHANNEL, AgsLinkChannelClass))
#define AGS_IS_LINK_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_LINK_CHANNEL))
#define AGS_IS_LINK_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_LINK_CHANNEL))
#define AGS_LINK_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_LINK_CHANNEL, AgsLinkChannelClass))

typedef struct _AgsLinkChannel AgsLinkChannel;
typedef struct _AgsLinkChannelClass AgsLinkChannelClass;

struct _AgsLinkChannel
{
  AgsTask task;

  AgsChannel *channel;
  AgsChannel *link;

  GError *error;
};

struct _AgsLinkChannelClass
{
  AgsTaskClass task;
};

GType ags_link_channel_get_type();

AgsLinkChannel* ags_link_channel_new(AgsChannel *channel, AgsChannel *link);

#endif /*__AGS_LINK_CHANNEL_H__*/
