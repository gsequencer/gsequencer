/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_CAPTURE_WAVE_CHANNEL_H__
#define __AGS_CAPTURE_WAVE_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_wave.h>

G_BEGIN_DECLS

#define AGS_TYPE_CAPTURE_WAVE_CHANNEL                (ags_capture_wave_channel_get_type())
#define AGS_CAPTURE_WAVE_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CAPTURE_WAVE_CHANNEL, AgsCaptureWaveChannel))
#define AGS_CAPTURE_WAVE_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CAPTURE_WAVE_CHANNEL, AgsCaptureWaveChannel))
#define AGS_IS_CAPTURE_WAVE_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_CAPTURE_WAVE_CHANNEL))
#define AGS_IS_CAPTURE_WAVE_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_CAPTURE_WAVE_CHANNEL))
#define AGS_CAPTURE_WAVE_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_CAPTURE_WAVE_CHANNEL, AgsCaptureWaveChannelClass))

typedef struct _AgsCaptureWaveChannel AgsCaptureWaveChannel;
typedef struct _AgsCaptureWaveChannelClass AgsCaptureWaveChannelClass;

struct _AgsCaptureWaveChannel
{
  AgsRecallChannel recall_channel;

  AgsPort *x_offset;
};

struct _AgsCaptureWaveChannelClass
{
  AgsRecallChannelClass recall_channel;
};

G_DEPRECATED
GType ags_capture_wave_channel_get_type();

G_DEPRECATED
AgsCaptureWaveChannel* ags_capture_wave_channel_new(AgsChannel *source);

G_END_DECLS

#endif /*__AGS_CAPTURE_WAVE_CHANNEL_H__*/
