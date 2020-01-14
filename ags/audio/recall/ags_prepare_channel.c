/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_prepare_channel.h>

void ags_prepare_channel_class_init(AgsPrepareChannelClass *prepare_channel);
void ags_prepare_channel_init(AgsPrepareChannel *prepare_channel);
void ags_prepare_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_prepare_channel
 * @short_description: prepares channel
 * @title: AgsPrepareChannel
 * @section_id:
 * @include: ags/audio/recall/ags_prepare_channel.h
 *
 * The #AgsPrepareChannel class provides ports to the effect processor.
 */

static gpointer ags_prepare_channel_parent_class = NULL;

static const gchar *ags_prepare_channel_plugin_name = "ags-prepare";

GType
ags_prepare_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_prepare_channel = 0;

    static const GTypeInfo ags_prepare_channel_info = {
      sizeof (AgsPrepareChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_prepare_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPrepareChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_prepare_channel_init,
    };

    ags_type_prepare_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						      "AgsPrepareChannel",
						      &ags_prepare_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_prepare_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_prepare_channel_class_init(AgsPrepareChannelClass *prepare_channel)
{
  GObjectClass *gobject;

  ags_prepare_channel_parent_class = g_type_class_peek_parent(prepare_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) prepare_channel;

  gobject->finalize = ags_prepare_channel_finalize;
}

void
ags_prepare_channel_init(AgsPrepareChannel *prepare_channel)
{
  AGS_RECALL(prepare_channel)->name = "ags-prepare";
  AGS_RECALL(prepare_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(prepare_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(prepare_channel)->xml_type = "ags-prepare-channel";
}

void
ags_prepare_channel_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_prepare_channel_parent_class)->finalize(gobject);
}

/**
 * ags_prepare_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPrepareChannel
 *
 * Returns: the new #AgsPrepareChannel
 *
 * Since: 3.0.0
 */
AgsPrepareChannel*
ags_prepare_channel_new(AgsChannel *source)
{
  AgsPrepareChannel *prepare_channel;

  prepare_channel = (AgsPrepareChannel *) g_object_new(AGS_TYPE_PREPARE_CHANNEL,
						       "source", source,
						       NULL);

  return(prepare_channel);
}
