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

#ifndef __AGS_FX_LADSPA_CHANNEL_H__
#define __AGS_FX_LADSPA_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/plugin/ags_ladspa_plugin.h>

#include <ags/audio/ags_port.h>
#include <ags/audio/ags_sound_enums.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_LADSPA_CHANNEL                (ags_fx_ladspa_channel_get_type())
#define AGS_FX_LADSPA_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_LADSPA_CHANNEL, AgsFxLadspaChannel))
#define AGS_FX_LADSPA_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_LADSPA_CHANNEL, AgsFxLadspaChannel))
#define AGS_IS_FX_LADSPA_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_LADSPA_CHANNEL))
#define AGS_IS_FX_LADSPA_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_LADSPA_CHANNEL))
#define AGS_FX_LADSPA_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_LADSPA_CHANNEL, AgsFxLadspaChannelClass))

#define AGS_FX_LADSPA_CHANNEL_INPUT_DATA(ptr) ((AgsFxLadspaChannelInputData *) (ptr))

typedef struct _AgsFxLadspaChannel AgsFxLadspaChannel;
typedef struct _AgsFxLadspaChannelInputData AgsFxLadspaChannelInputData;
typedef struct _AgsFxLadspaChannelClass AgsFxLadspaChannelClass;

struct _AgsFxLadspaChannel
{
  AgsRecallChannel recall_channel;

  guint output_port_count;
  guint *output_port;

  guint input_port_count;
  guint *input_port;

  AgsFxLadspaChannelInputData* input_data[AGS_SOUND_SCOPE_LAST];

  AgsLadspaPlugin *ladspa_plugin;

  AgsPort **ladspa_port;
};

struct _AgsFxLadspaChannelClass
{
  AgsRecallChannelClass recall_channel;
};

struct _AgsFxLadspaChannelInputData
{
  gpointer parent;

  LADSPA_Data *output;
  LADSPA_Data *input;

  LADSPA_Handle ladspa_handle;
};

GType ags_fx_ladspa_channel_get_type();

/* runtime */
AgsFxLadspaChannelInputData* ags_fx_ladspa_channel_input_data_alloc();
void ags_fx_ladspa_channel_input_data_free(AgsFxLadspaChannelInputData *input_data);

/* load/unload */
void ags_fx_ladspa_channel_load_plugin(AgsFxLadspaChannel *fx_ladspa_channel);
void ags_fx_ladspa_channel_load_port(AgsFxLadspaChannel *fx_ladspa_channel);

/* connect port */
void ags_fx_ladspa_channel_connect_port(AgsFxLadspaChannel *fx_ladspa_channel);

/* instantiate */
AgsFxLadspaChannel* ags_fx_ladspa_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_LADSPA_CHANNEL_H__*/
