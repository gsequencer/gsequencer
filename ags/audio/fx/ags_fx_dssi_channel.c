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

#include <ags/audio/fx/ags_fx_dssi_channel.h>

#include <ags/plugin/ags_dssi_manager.h>

void ags_fx_dssi_channel_class_init(AgsFxDssiChannelClass *fx_dssi_channel);
void ags_fx_dssi_channel_init(AgsFxDssiChannel *fx_dssi_channel);
void ags_fx_dssi_channel_dispose(GObject *gobject);
void ags_fx_dssi_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_dssi_channel
 * @short_description: fx dssi channel
 * @title: AgsFxDssiChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_channel.h
 *
 * The #AgsFxDssiChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_channel_parent_class = NULL;

static const gchar *ags_fx_dssi_channel_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_channel = 0;

    static const GTypeInfo ags_fx_dssi_channel_info = {
      sizeof (AgsFxDssiChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxDssiChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_channel_init,
    };

    ags_type_fx_dssi_channel = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL,
						      "AgsFxDssiChannel",
						      &ags_fx_dssi_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_channel_class_init(AgsFxDssiChannelClass *fx_dssi_channel)
{
  GObjectClass *gobject;

  ags_fx_dssi_channel_parent_class = g_type_class_peek_parent(fx_dssi_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_channel;

  gobject->dispose = ags_fx_dssi_channel_dispose;
  gobject->finalize = ags_fx_dssi_channel_finalize;
}

void
ags_fx_dssi_channel_init(AgsFxDssiChannel *fx_dssi_channel)
{
  AGS_RECALL(fx_dssi_channel)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_channel)->xml_type = "ags-fx-dssi-channel";

  fx_dssi_channel->dssi_port = NULL;
}

void
ags_fx_dssi_channel_dispose(GObject *gobject)
{
  AgsFxDssiChannel *fx_dssi_channel;
  
  fx_dssi_channel = AGS_FX_DSSI_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_channel_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_channel_finalize(GObject *gobject)
{
  AgsFxDssiChannel *fx_dssi_channel;
  
  fx_dssi_channel = AGS_FX_DSSI_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_dssi_channel_load_port:
 * @fx_dssi_channel: the #AgsFxDssiChannel
 * 
 * Load port of @fx_dssi_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_channel_load_port(AgsFxDssiChannel *fx_dssi_channel)
{
  if(!AGS_IS_FX_DSSI_CHANNEL(fx_dssi_channel)){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_fx_dssi_channel_unload_port:
 * @fx_dssi_channel: the #AgsFxDssiChannel
 * 
 * Unload port of @fx_dssi_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_channel_unload_port(AgsFxDssiChannel *fx_dssi_channel)
{
  if(!AGS_IS_FX_DSSI_CHANNEL(fx_dssi_channel)){
    return;
  }

  //TODO:JK: implement me
}

/**
 * ags_fx_dssi_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxDssiChannel
 *
 * Returns: the new #AgsFxDssiChannel
 *
 * Since: 3.3.0
 */
AgsFxDssiChannel*
ags_fx_dssi_channel_new(AgsChannel *channel)
{
  AgsFxDssiChannel *fx_dssi_channel;

  fx_dssi_channel = (AgsFxDssiChannel *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL,
						      "channel", channel,
						      NULL);

  return(fx_dssi_channel);
}
