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

#include <ags/audio/fx/ags_fx_tremolo_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_tremolo_channel_class_init(AgsFxTremoloChannelClass *fx_tremolo_channel);
void ags_fx_tremolo_channel_init(AgsFxTremoloChannel *fx_tremolo_channel);
void ags_fx_tremolo_channel_dispose(GObject *gobject);
void ags_fx_tremolo_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_tremolo_channel
 * @short_description: fx tremolo channel
 * @title: AgsFxTremoloChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_tremolo_channel.h
 *
 * The #AgsFxTremoloChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_tremolo_channel_parent_class = NULL;

const gchar *ags_fx_tremolo_channel_plugin_name = "ags-fx-tremolo";

const gchar* ags_fx_tremolo_channel_specifier[] = {
  NULL,
};

const gchar* ags_fx_tremolo_channel_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_tremolo_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_tremolo_channel = 0;

    static const GTypeInfo ags_fx_tremolo_channel_info = {
      sizeof (AgsFxTremoloChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_tremolo_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxTremoloChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_tremolo_channel_init,
    };

    ags_type_fx_tremolo_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							 "AgsFxTremoloChannel",
							 &ags_fx_tremolo_channel_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_tremolo_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_tremolo_channel_class_init(AgsFxTremoloChannelClass *fx_tremolo_channel)
{
  GObjectClass *gobject;

  ags_fx_tremolo_channel_parent_class = g_type_class_peek_parent(fx_tremolo_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_tremolo_channel;

  gobject->set_property = ags_fx_tremolo_channel_set_property;
  gobject->get_property = ags_fx_tremolo_channel_get_property;

  gobject->dispose = ags_fx_tremolo_channel_dispose;
  gobject->finalize = ags_fx_tremolo_channel_finalize;

  /* properties */
}

void
ags_fx_tremolo_channel_init(AgsFxTremoloChannel *fx_tremolo_channel)
{
  AGS_RECALL(fx_tremolo_channel)->name = "ags-fx-tremolo";
  AGS_RECALL(fx_tremolo_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_tremolo_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_tremolo_channel)->xml_type = "ags-fx-tremolo-channel";
}

void
ags_fx_tremolo_channel_dispose(GObject *gobject)
{
  AgsFxTremoloChannel *fx_tremolo_channel;
  
  fx_tremolo_channel = AGS_FX_TREMOLO_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_tremolo_channel_parent_class)->dispose(gobject);
}

void
ags_fx_tremolo_channel_finalize(GObject *gobject)
{
  AgsFxTremoloChannel *fx_tremolo_channel;
  
  fx_tremolo_channel = AGS_FX_TREMOLO_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_tremolo_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_tremolo_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxTremoloChannel
 *
 * Returns: the new #AgsFxTremoloChannel
 *
 * Since: 5.2.0
 */
AgsFxTremoloChannel*
ags_fx_tremolo_channel_new(AgsChannel *channel)
{
  AgsFxTremoloChannel *fx_tremolo_channel;

  fx_tremolo_channel = (AgsFxTremoloChannel *) g_object_new(AGS_TYPE_FX_TREMOLO_CHANNEL,
							    "source", channel,
							    NULL);

  return(fx_tremolo_channel);
}
