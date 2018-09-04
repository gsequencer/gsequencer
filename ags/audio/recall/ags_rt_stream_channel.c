/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_rt_stream_channel.h>

#include <ags/libags.h>

#include <ags/i18n.h>

void ags_rt_stream_channel_class_init(AgsRtStreamChannelClass *rt_stream_channel);
void ags_rt_stream_channel_init(AgsRtStreamChannel *rt_stream_channel);
void ags_rt_stream_channel_dispose(GObject *gobject);
void ags_rt_stream_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_rt_stream_channel
 * @short_description: rt_streams channel
 * @title: AgsRtStreamChannel
 * @section_id:
 * @include: ags/audio/recall/ags_rt_stream_channel.h
 *
 * The #AgsRtStreamChannel class provides ports to the effect processor.
 */

static gpointer ags_rt_stream_channel_parent_class = NULL;

static const gchar *ags_rt_stream_channel_plugin_name = "ags-rt-stream";

GType
ags_rt_stream_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_rt_stream_channel = 0;

    static const GTypeInfo ags_rt_stream_channel_info = {
      sizeof (AgsRtStreamChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_rt_stream_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRtStreamChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_rt_stream_channel_init,
    };

    ags_type_rt_stream_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							"AgsRtStreamChannel",
							&ags_rt_stream_channel_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_rt_stream_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_rt_stream_channel_class_init(AgsRtStreamChannelClass *rt_stream_channel)
{
  GObjectClass *gobject;

  ags_rt_stream_channel_parent_class = g_type_class_peek_parent(rt_stream_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) rt_stream_channel;

  gobject->dispose = ags_rt_stream_channel_dispose;
  gobject->finalize = ags_rt_stream_channel_finalize;
}

void
ags_rt_stream_channel_init(AgsRtStreamChannel *rt_stream_channel)
{
  AgsConfig *config;
  
  AGS_RECALL(rt_stream_channel)->name = "ags-rt-stream";
  AGS_RECALL(rt_stream_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(rt_stream_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(rt_stream_channel)->xml_type = "ags-rt-stream-channel";
}

void
ags_rt_stream_channel_dispose(GObject *gobject)
{
  AgsRtStreamChannel *rt_stream_channel;

  rt_stream_channel = AGS_RT_STREAM_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_channel_parent_class)->dispose(gobject);
}

void
ags_rt_stream_channel_finalize(GObject *gobject)
{
  AgsRtStreamChannel *rt_stream_channel;

  rt_stream_channel = AGS_RT_STREAM_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_rt_stream_channel_parent_class)->finalize(gobject);
}

/**
 * ags_rt_stream_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsRtStreamChannel
 *
 * Returns: the new #AgsRtStreamChannel
 *
 * Since: 2.0.0
 */
AgsRtStreamChannel*
ags_rt_stream_channel_new(AgsChannel *source)
{
  AgsRtStreamChannel *rt_stream_channel;

  rt_stream_channel = (AgsRtStreamChannel *) g_object_new(AGS_TYPE_RT_STREAM_CHANNEL,
							  "source", source,
							  NULL);

  return(rt_stream_channel);
}
