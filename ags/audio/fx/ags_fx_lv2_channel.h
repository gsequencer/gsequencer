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

#ifndef __AGS_FX_LV2_CHANNEL_H__
#define __AGS_FX_LV2_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <alsa/seq_event.h>

#include <lv2.h>

#include <ags/plugin/ags_lv2_plugin.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/fx/ags_fx_notation_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_LV2_CHANNEL                (ags_fx_lv2_channel_get_type())
#define AGS_FX_LV2_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_LV2_CHANNEL, AgsFxLv2Channel))
#define AGS_FX_LV2_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_LV2_CHANNEL, AgsFxLv2ChannelClass))
#define AGS_IS_FX_LV2_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_LV2_CHANNEL))
#define AGS_IS_FX_LV2_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_LV2_CHANNEL))
#define AGS_FX_LV2_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_LV2_CHANNEL, AgsFxLv2ChannelClass))

#define AGS_FX_LV2_CHANNEL_INPUT_DATA(ptr) ((AgsFxLv2ChannelInputData *)(ptr))
#define AGS_FX_LV2_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(ptr) (&(((AgsFxLv2ChannelInputData *)(ptr))->strct_mutex))

typedef struct _AgsFxLv2Channel AgsFxLv2Channel;
typedef struct _AgsFxLv2ChannelInputData AgsFxLv2ChannelInputData;
typedef struct _AgsFxLv2ChannelClass AgsFxLv2ChannelClass;

struct _AgsFxLv2Channel
{
  AgsFxNotationChannel fx_notation_channel;

  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;

  AgsFxLv2ChannelInputData* input_data[AGS_SOUND_SCOPE_LAST];

  AgsLv2Plugin *lv2_plugin;

  AgsPort **lv2_port;
};

struct _AgsFxLv2ChannelClass
{
  AgsFxNotationChannelClass fx_notation_channel;
};

struct _AgsFxLv2ChannelInputData
{
  GRecMutex strct_mutex;
  
  gpointer parent;

  float *output;
  float *input;

  LV2_Handle *lv2_handle;
};

GType ags_fx_lv2_channel_get_type();

/* runtime */
AgsFxLv2ChannelInputData* ags_fx_lv2_channel_input_data_alloc();
void ags_fx_lv2_channel_input_data_free(AgsFxLv2ChannelInputData *input_data);

/* load/unload */
void ags_fx_lv2_channel_load_plugin(AgsFxLv2Channel *fx_lv2_channel);
void ags_fx_lv2_channel_load_port(AgsFxLv2Channel *fx_lv2_channel);

/* instantiate */
AgsFxLv2Channel* ags_fx_lv2_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_LV2_CHANNEL_H__*/
