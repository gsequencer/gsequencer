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

#include <ags/audio/recall/ags_feed_channel.h>

#include <ags/libags.h>

void ags_feed_channel_class_init(AgsFeedChannelClass *feed_channel);
void ags_feed_channel_init(AgsFeedChannel *feed_channel);
void ags_feed_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_feed_channel
 * @short_description: feeds channel
 * @title: AgsFeedChannel
 * @section_id:
 * @include: ags/audio/recall/ags_feed_channel.h
 *
 * The #AgsFeedChannel is responsible to provide enough audio data
 * for real-time MIDI.
 */

static gpointer ags_feed_channel_parent_class = NULL;

static const gchar *ags_feed_channel_plugin_name = "ags-feed";

GType
ags_feed_channel_get_type()
{
  static GType ags_type_feed_channel = 0;

  if(!ags_type_feed_channel){
    static const GTypeInfo ags_feed_channel_info = {
      sizeof (AgsFeedChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_channel_init,
    };

    ags_type_feed_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsFeedChannel",
						   &ags_feed_channel_info,
						   0);    
  }

  return (ags_type_feed_channel);
}

void
ags_feed_channel_class_init(AgsFeedChannelClass *feed_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_feed_channel_parent_class = g_type_class_peek_parent(feed_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_channel;

  gobject->finalize = ags_feed_channel_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) feed_channel;
}

void
ags_feed_channel_init(AgsFeedChannel *feed_channel)
{
  AGS_RECALL(feed_channel)->name = "ags-feed";
  AGS_RECALL(feed_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_channel)->xml_type = "ags-feed-channel";
}

void
ags_feed_channel_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_feed_channel_parent_class)->finalize(gobject);
}

/**
 * ags_feed_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsFeedChannel
 *
 * Returns: the new #AgsFeedChannel
 *
 * Since: 2.0.0
 */
AgsFeedChannel*
ags_feed_channel_new(AgsChannel *source)
{
  AgsFeedChannel *feed_channel;

  feed_channel = (AgsFeedChannel *) g_object_new(AGS_TYPE_FEED_CHANNEL,
						 "source", source,
						 NULL);

  return(feed_channel);
}
