/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_feed_channel_run.h>
#include <ags/audio/recall/ags_feed_recycling.h>

#include <ags/libags.h>

void ags_feed_channel_run_class_init(AgsFeedChannelRunClass *feed_channel_run);
void ags_feed_channel_run_init(AgsFeedChannelRun *feed_channel_run);
void ags_feed_channel_run_finalize(GObject *gobject);

/**
 * SECTION:ags_feed_channel_run
 * @short_description: feeds channel
 * @title: AgsFeedChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_feed_channel_run.h
 *
 * The #AgsFeedChannelRun class feeds the channel.
 */

static gpointer ags_feed_channel_run_parent_class = NULL;

GType
ags_feed_channel_run_get_type()
{
  static GType ags_type_feed_channel_run = 0;

  if(!ags_type_feed_channel_run){
    static const GTypeInfo ags_feed_channel_run_info = {
      sizeof (AgsFeedChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_channel_run_init,
    };

    ags_type_feed_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsFeedChannelRun",
						       &ags_feed_channel_run_info,
						       0);
  }

  return(ags_type_feed_channel_run);
}

void
ags_feed_channel_run_class_init(AgsFeedChannelRunClass *feed_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_feed_channel_run_parent_class = g_type_class_peek_parent(feed_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_channel_run;

  gobject->finalize = ags_feed_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) feed_channel_run;
}

void
ags_feed_channel_run_init(AgsFeedChannelRun *feed_channel_run)
{
  AGS_RECALL(feed_channel_run)->name = "ags-feed";
  AGS_RECALL(feed_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_channel_run)->xml_type = "ags-feed-channel-run";

  AGS_RECALL(feed_channel_run)->child_type = AGS_TYPE_FEED_RECYCLING;
}

void
ags_feed_channel_run_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_feed_channel_run_parent_class)->finalize(gobject);
}

/**
 * ags_feed_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsFeedChannelRun
 *
 * Returns: a new #AgsFeedChannelRun
 *
 * Since: 2.0.0
 */
AgsFeedChannelRun*
ags_feed_channel_run_new(AgsChannel *source)
{
  AgsFeedChannelRun *feed_channel_run;

  feed_channel_run = (AgsFeedChannelRun *) g_object_new(AGS_TYPE_FEED_CHANNEL_RUN,
							"source", source,
							NULL);

  return(feed_channel_run);
}
