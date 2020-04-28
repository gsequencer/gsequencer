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

#include <ags/audio/fx/ags_fx_analyse_channel.h>

#include <ags/i18n.h>

void ags_fx_analyse_channel_class_init(AgsFxAnalyseChannelClass *fx_analyse_channel);
void ags_fx_analyse_channel_init(AgsFxAnalyseChannel *fx_analyse_channel);
void ags_fx_analyse_channel_dispose(GObject *gobject);
void ags_fx_analyse_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_analyse_channel
 * @short_description: fx analyse channel
 * @title: AgsFxAnalyseChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_analyse_channel.h
 *
 * The #AgsFxAnalyseChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_analyse_channel_parent_class = NULL;

static const gchar *ags_fx_analyse_channel_plugin_name = "ags-fx-analyse";

GType
ags_fx_analyse_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_analyse_channel = 0;

    static const GTypeInfo ags_fx_analyse_channel_info = {
      sizeof (AgsFxAnalyseChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_analyse_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxAnalyseChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_analyse_channel_init,
    };

    ags_type_fx_analyse_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							 "AgsFxAnalyseChannel",
							 &ags_fx_analyse_channel_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_analyse_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_analyse_channel_class_init(AgsFxAnalyseChannelClass *fx_analyse_channel)
{
  GObjectClass *gobject;

  ags_fx_analyse_channel_parent_class = g_type_class_peek_parent(fx_analyse_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_analyse_channel;

  gobject->dispose = ags_fx_analyse_channel_dispose;
  gobject->finalize = ags_fx_analyse_channel_finalize;
}

void
ags_fx_analyse_channel_init(AgsFxAnalyseChannel *fx_analyse_channel)
{
  AGS_RECALL(fx_analyse_channel)->name = "ags-fx-analyse";
  AGS_RECALL(fx_analyse_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_analyse_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_analyse_channel)->xml_type = "ags-fx-analyse-channel";
}

void
ags_fx_analyse_channel_dispose(GObject *gobject)
{
  AgsFxAnalyseChannel *fx_analyse_channel;
  
  fx_analyse_channel = AGS_FX_ANALYSE_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_analyse_channel_parent_class)->dispose(gobject);
}

void
ags_fx_analyse_channel_finalize(GObject *gobject)
{
  AgsFxAnalyseChannel *fx_analyse_channel;
  
  fx_analyse_channel = AGS_FX_ANALYSE_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_analyse_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_analyse_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxAnalyseChannel
 *
 * Returns: the new #AgsFxAnalyseChannel
 *
 * Since: 3.3.0
 */
AgsFxAnalyseChannel*
ags_fx_analyse_channel_new(AgsChannel *channel)
{
  AgsFxAnalyseChannel *fx_analyse_channel;

  fx_analyse_channel = (AgsFxAnalyseChannel *) g_object_new(AGS_TYPE_FX_ANALYSE_CHANNEL,
							    "source", channel,
							    NULL);

  return(fx_analyse_channel);
}
