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

#include <ags/audio/recall/ags_feed_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_feed_audio_signal.h>

void ags_feed_recycling_class_init(AgsFeedRecyclingClass *feed_recycling);
void ags_feed_recycling_init(AgsFeedRecycling *feed_recycling);
void ags_feed_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_feed_recycling
 * @short_description: feeds recycling
 * @title: AgsFeedRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_feed_recycling.h
 *
 * The #AgsFeedRecycling class feeds the recycling.
 */

static gpointer ags_feed_recycling_parent_class = NULL;

GType
ags_feed_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_feed_recycling;

    static const GTypeInfo ags_feed_recycling_info = {
      sizeof (AgsFeedRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_recycling_init,
    };

    ags_type_feed_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsFeedRecycling",
						     &ags_feed_recycling_info,
						     0);
  }

  return(ags_type_feed_recycling);
}

void
ags_feed_recycling_class_init(AgsFeedRecyclingClass *feed_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_feed_recycling_parent_class = g_type_class_peek_parent(feed_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_recycling;

  gobject->finalize = ags_feed_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) feed_recycling;
}

void
ags_feed_recycling_init(AgsFeedRecycling *feed_recycling)
{
  AGS_RECALL(feed_recycling)->name = "ags-feed";
  AGS_RECALL(feed_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_recycling)->xml_type = "ags-feed-recycling";

  AGS_RECALL(feed_recycling)->child_type = AGS_TYPE_FEED_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(feed_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_feed_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_feed_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_feed_recycling_new:
 * @source: the #AgsRecycling
 *
 * Creates an #AgsFeedRecycling
 *
 * Returns: a new #AgsFeedRecycling
 *
 * Since: 2.0.0
 */
AgsFeedRecycling*
ags_feed_recycling_new(AgsRecycling *source)
{
  AgsFeedRecycling *feed_recycling;

  feed_recycling = (AgsFeedRecycling *) g_object_new(AGS_TYPE_FEED_RECYCLING,
						     "source", source,
						     NULL);

  return(feed_recycling);
}
