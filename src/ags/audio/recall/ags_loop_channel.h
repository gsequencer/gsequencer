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

#ifndef __AGS_LOOP_CHANNEL_H__
#define __AGS_LOOP_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/recall/ags_delay_audio.h>

#define AGS_TYPE_LOOP_CHANNEL                (ags_loop_channel_get_type())
#define AGS_LOOP_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LOOP_CHANNEL, AgsLoopChannel))
#define AGS_LOOP_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LOOP_CHANNEL, AgsLoopChannelClass))
#define AGS_IS_LOOP_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LOOP_CHANNEL))
#define AGS_IS_LOOP_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LOOP_CHANNEL))
#define AGS_LOOP_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LOOP_CHANNEL, AgsLoopChannelClass))

typedef struct _AgsLoopChannel AgsLoopChannel;
typedef struct _AgsLoopChannelClass AgsLoopChannelClass;

struct _AgsLoopChannel
{
  AgsRecallChannel recall_channel;
  
  AgsDelayAudio *delay_audio;
  gulong sequencer_duration_changed_handler;
};

struct _AgsLoopChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_loop_channel_get_type();

AgsLoopChannel* ags_loop_channel_new();

#endif /*__AGS_LOOP_CHANNEL_H__*/
