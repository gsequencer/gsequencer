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

#ifndef __AGS_PEAK_CHANNEL_H__
#define __AGS_PEAK_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_channel.h>

#define AGS_TYPE_PEAK_CHANNEL                (ags_peak_channel_get_type())
#define AGS_PEAK_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PEAK_CHANNEL, AgsPeakChannel))
#define AGS_PEAK_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PEAK_CHANNEL, AgsPeakChannel))
#define AGS_IS_PEAK_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PEAK_CHANNEL))
#define AGS_IS_PEAK_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PEAK_CHANNEL))
#define AGS_PEAK_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_PEAK_CHANNEL, AgsPeakChannelClass))

typedef struct _AgsPeakChannel AgsPeakChannel;
typedef struct _AgsPeakChannelClass AgsPeakChannelClass;

struct _AgsPeakChannel
{
  AgsRecallChannel recall_channel;

  AgsPort *peak;
};

struct _AgsPeakChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_peak_channel_get_type();

void ags_peak_channel_retrieve_peak(AgsPeakChannel *peak_channel,
				    gboolean is_play);

AgsPeakChannel* ags_peak_channel_new(AgsChannel *source);

#endif /*__AGS_PEAK_CHANNEL_H__*/
