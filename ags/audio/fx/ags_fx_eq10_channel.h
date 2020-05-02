/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_FX_EQ10_CHANNEL_H__
#define __AGS_FX_EQ10_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_EQ10_CHANNEL                (ags_fx_eq10_channel_get_type())
#define AGS_FX_EQ10_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_EQ10_CHANNEL, AgsFxEq10Channel))
#define AGS_FX_EQ10_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_EQ10_CHANNEL, AgsFxEq10ChannelClass))
#define AGS_IS_FX_EQ10_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_EQ10_CHANNEL))
#define AGS_IS_FX_EQ10_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_EQ10_CHANNEL))
#define AGS_FX_EQ10_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_EQ10_CHANNEL, AgsFxEq10ChannelClass))

#define AGS_FX_EQ10_CHANNEL_INPUT_DATA(ptr) ((AgsFxEq10ChannelInputData *)(ptr))

#define AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE (8)

typedef struct _AgsFxEq10Channel AgsFxEq10Channel;
typedef struct _AgsFxEq10ChannelInputData AgsFxEq10ChannelInputData;
typedef struct _AgsFxEq10ChannelClass AgsFxEq10ChannelClass;

struct _AgsFxEq10Channel
{
  AgsRecallChannel recall_channel;

  AgsFxEq10ChannelInputData* input_data[AGS_SOUND_SCOPE_LAST];

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

struct _AgsFxEq10ChannelClass
{
  AgsRecallChannelClass recall_channel;
};

struct _AgsFxEq10ChannelInputData
{
  gpointer parent;

  gdouble cache_28hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_56hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_112hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_224hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_448hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_896hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_1792hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_3584hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_7168hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];
  gdouble cache_14336hz[AGS_FX_EQ10_CHANNEL_INPUT_DATA_CACHE_SIZE];

  gdouble *output;
  gdouble *input;
};

GType ags_fx_eq10_channel_get_type();

/* runtime */
AgsFxEq10ChannelInputData* ags_fx_eq10_channel_input_data_alloc();
void ags_fx_eq10_channel_input_data_free(AgsFxEq10ChannelInputData *input_data);

/* instantiate */
AgsFxEq10Channel* ags_fx_eq10_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_EQ10_CHANNEL_H__*/
