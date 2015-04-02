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

#ifndef __AGS_STREAM_CHANNEL_H__
#define __AGS_STREAM_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>

#define AGS_TYPE_STREAM_CHANNEL                (ags_stream_channel_get_type())
#define AGS_STREAM_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STREAM_CHANNEL, AgsStreamChannel))
#define AGS_STREAM_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_STREAM_CHANNEL, AgsStreamChannelClass))
#define AGS_IS_STREAM_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_STREAM_CHANNEL))
#define AGS_IS_STREAM_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_STREAM_CHANNEL))
#define AGS_STREAM_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_STREAM_CHANNEL, AgsStreamChannelClass))

typedef struct _AgsStreamChannel AgsStreamChannel;
typedef struct _AgsStreamChannelClass AgsStreamChannelClass;

struct _AgsStreamChannel
{
  AgsRecallChannel recall_channel;

  AgsPort *auto_sense;
};

struct _AgsStreamChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_stream_channel_get_type();

AgsStreamChannel* ags_stream_channel_new();

#endif /*__AGS_STREAM_CHANNEL_H__*/
