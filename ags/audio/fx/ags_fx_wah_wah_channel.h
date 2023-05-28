/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_FX_WAH_WAH_CHANNEL_H__
#define __AGS_FX_WAH_WAH_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_WAH_WAH_CHANNEL                (ags_fx_wah_wah_channel_get_type())
#define AGS_FX_WAH_WAH_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_WAH_WAH_CHANNEL, AgsFxWahWahChannel))
#define AGS_FX_WAH_WAH_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_WAH_WAH_CHANNEL, AgsFxWahWahChannelClass))
#define AGS_IS_FX_WAH_WAH_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_WAH_WAH_CHANNEL))
#define AGS_IS_FX_WAH_WAH_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_WAH_WAH_CHANNEL))
#define AGS_FX_WAH_WAH_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_WAH_WAH_CHANNEL, AgsFxWahWahChannelClass))

typedef struct _AgsFxWahWahChannel AgsFxWahWahChannel;
typedef struct _AgsFxWahWahChannelClass AgsFxWahWahChannelClass;

typedef enum{
  AGS_FX_WAH_WAH_CHANNEL_MODE_STREAM_LENGTH,
  AGS_FX_WAH_WAH_CHANNEL_MODE_NOTE_LENGTH,
  AGS_FX_WAH_WAH_CHANNEL_MODE_FIXED_LENGTH,
}AgsFxWahWahChannelMode;

typedef enum{
  AGS_FX_WAH_WAH_1_1_TH,
  AGS_FX_WAH_WAH_2_2_TH,
  AGS_FX_WAH_WAH_4_4_TH,
  AGS_FX_WAH_WAH_8_8_TH,
  AGS_FX_WAH_WAH_16_16_TH,
}AgsFxWahWahLengthMode;

struct _AgsFxWahWahChannel
{
  AgsRecallChannel recall_channel;

  guint wah_wah_mode;
  
  AgsPort *wah_wah_enabled;

  AgsPort *wah_wah_length_mode;
  AgsPort *wah_wah_fixed_length;

  AgsPort *wah_wah_attack;
  AgsPort *wah_wah_decay;
  AgsPort *wah_wah_sustain;
  AgsPort *wah_wah_release;
  AgsPort *wah_wah_ratio;

  AgsPort *wah_wah_lfo_depth;
  AgsPort *wah_wah_lfo_freq;
  AgsPort *wah_wah_tuning;
};

struct _AgsFxWahWahChannelClass
{
  AgsRecallChannelClass recall_channel;
};

GType ags_fx_wah_wah_channel_get_type();

/*  */
AgsFxWahWahChannel* ags_fx_wah_wah_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_WAH_WAH_CHANNEL_H__*/
