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

#ifndef __AGS_EQ10_CHANNEL_H__
#define __AGS_EQ10_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>

#define AGS_TYPE_EQ10_CHANNEL                (ags_eq10_channel_get_type())
#define AGS_EQ10_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EQ10_CHANNEL, AgsEq10Channel))
#define AGS_EQ10_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EQ10_CHANNEL, AgsEq10ChannelClass))
#define AGS_IS_EQ10_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EQ10_CHANNEL))
#define AGS_IS_EQ10_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EQ10_CHANNEL))
#define AGS_EQ10_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_EQ10_CHANNEL, AgsEq10ChannelClass))

typedef struct _AgsEq10Channel AgsEq10Channel;
typedef struct _AgsEq10ChannelClass AgsEq10ChannelClass;

struct _AgsEq10Channel
{
  AgsRecallChannel recall_channel;

  AgsPort *peak_28hz;
  AgsPort *peak_56hz;
  AgsPort *peak_112hz;
  AgsPort *peak_224hz;
  AgsPort *peak_448hz;
  AgsPort *peak_896hz;
  AgsPort *peak_1792hz;
  AgsPort *peak_3584hz;
  AgsPort *peak_7168hz;
  AgsPort *peak_14336hz;

  AgsPort *pressure;
};

struct _AgsEq10ChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_eq10_channel_get_type();

AgsEq10Channel* ags_eq10_channel_new();

#endif /*__AGS_EQ10_CHANNEL_H__*/
