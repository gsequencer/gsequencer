/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_sf2_synth_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_sf2_synth_channel_class_init(AgsFxSF2SynthChannelClass *fx_sf2_synth_channel);
void ags_fx_sf2_synth_channel_init(AgsFxSF2SynthChannel *fx_sf2_synth_channel);
void ags_fx_sf2_synth_channel_dispose(GObject *gobject);
void ags_fx_sf2_synth_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_sf2_synth_channel
 * @short_description: fx Soundfont2 synth channel
 * @title: AgsFxSF2SynthChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_sf2_synth_channel.h
 *
 * The #AgsFxSF2SynthChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_sf2_synth_channel_parent_class = NULL;

const gchar *ags_fx_sf2_synth_channel_plugin_name = "ags-fx-sf2-synth";

const gchar* ags_fx_sf2_synth_channel_specifier[] = {
  NULL,
};

const gchar* ags_fx_sf2_synth_channel_control_port[] = {
  NULL,
};

GType
ags_fx_sf2_synth_channel_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_sf2_synth_channel = 0;

    static const GTypeInfo ags_fx_sf2_synth_channel_info = {
      sizeof (AgsFxSF2SynthChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_sf2_synth_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxSF2SynthChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_sf2_synth_channel_init,
    };

    ags_type_fx_sf2_synth_channel = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL,
							   "AgsFxSF2SynthChannel",
							   &ags_fx_sf2_synth_channel_info,
							   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_sf2_synth_channel);
  }

  return g_define_type_id__static;
}

void
ags_fx_sf2_synth_channel_class_init(AgsFxSF2SynthChannelClass *fx_sf2_synth_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_sf2_synth_channel_parent_class = g_type_class_peek_parent(fx_sf2_synth_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_sf2_synth_channel;

  gobject->dispose = ags_fx_sf2_synth_channel_dispose;
  gobject->finalize = ags_fx_sf2_synth_channel_finalize;

  /* properties */
}

void
ags_fx_sf2_synth_channel_init(AgsFxSF2SynthChannel *fx_sf2_synth_channel)
{
  AGS_RECALL(fx_sf2_synth_channel)->name = "ags-fx-sf2-synth";
  AGS_RECALL(fx_sf2_synth_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_sf2_synth_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_sf2_synth_channel)->xml_type = "ags-fx-sf2-synth-channel";
}

void
ags_fx_sf2_synth_channel_dispose(GObject *gobject)
{
  AgsFxSF2SynthChannel *fx_sf2_synth_channel;
  
  fx_sf2_synth_channel = AGS_FX_SF2_SYNTH_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_channel_parent_class)->dispose(gobject);
}

void
ags_fx_sf2_synth_channel_finalize(GObject *gobject)
{
  AgsFxSF2SynthChannel *fx_sf2_synth_channel;
  
  fx_sf2_synth_channel = AGS_FX_SF2_SYNTH_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_sf2_synth_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxSF2SynthChannel
 *
 * Returns: the new #AgsFxSF2SynthChannel
 *
 * Since: 3.16.0
 */
AgsFxSF2SynthChannel*
ags_fx_sf2_synth_channel_new(AgsChannel *channel)
{
  AgsFxSF2SynthChannel *fx_sf2_synth_channel;

  fx_sf2_synth_channel = (AgsFxSF2SynthChannel *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_CHANNEL,
							       "source", channel,
							       NULL);

  return(fx_sf2_synth_channel);
}
