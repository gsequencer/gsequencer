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

#ifndef __AGS_FX_DSSI_CHANNEL_H__
#define __AGS_FX_DSSI_CHANNEL_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_port.h>

#include <ags/audio/fx/ags_fx_notation_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_FX_DSSI_CHANNEL                (ags_fx_dssi_channel_get_type())
#define AGS_FX_DSSI_CHANNEL(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FX_DSSI_CHANNEL, AgsFxDssiChannel))
#define AGS_FX_DSSI_CHANNEL_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FX_DSSI_CHANNEL, AgsFxDssiChannelClass))
#define AGS_IS_FX_DSSI_CHANNEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_FX_DSSI_CHANNEL))
#define AGS_IS_FX_DSSI_CHANNEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FX_DSSI_CHANNEL))
#define AGS_FX_DSSI_CHANNEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_FX_DSSI_CHANNEL, AgsFxDssiChannelClass))

typedef struct _AgsFxDssiChannel AgsFxDssiChannel;
typedef struct _AgsFxDssiChannelClass AgsFxDssiChannelClass;

struct _AgsFxDssiChannel
{
  AgsFxNotationChannel fx_notation_channel;

  AgsPort **dssi_port;
};

struct _AgsFxDssiChannelClass
{
  AgsFxNotationChannelClass fx_notation_channel;
};

GType ags_fx_dssi_channel_get_type();

void ags_fx_dssi_channel_load_port(AgsFxDssiChannel *fx_dssi_channel);

AgsFxDssiChannel* ags_fx_dssi_channel_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FX_DSSI_CHANNEL_H__*/
