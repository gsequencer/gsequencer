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

#include <ags/audio/recall/ags_play_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_play_audio_signal.h>

void ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling);
void ags_play_recycling_init(AgsPlayRecycling *play_recycling);
void ags_play_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_play_recycling
 * @short_description: plays recycling
 * @title: AgsPlayRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_play_recycling.h
 *
 * The #AgsPlayRecycling class plays the recycling.
 */

static gpointer ags_play_recycling_parent_class = NULL;

GType
ags_play_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_recycling;

    static const GTypeInfo ags_play_recycling_info = {
      sizeof (AgsPlayRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_recycling_init,
    };

    ags_type_play_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsPlayRecycling",
						     &ags_play_recycling_info,
						     0);
  }

  return g_define_type_id__volatile;
}

void
ags_play_recycling_class_init(AgsPlayRecyclingClass *play_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_play_recycling_parent_class = g_type_class_peek_parent(play_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) play_recycling;

  gobject->finalize = ags_play_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_recycling;
}

void
ags_play_recycling_init(AgsPlayRecycling *play_recycling)
{
  AGS_RECALL(play_recycling)->child_type = AGS_TYPE_PLAY_AUDIO_SIGNAL;
}

void
ags_play_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_play_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_play_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsPlayRecycling
 *
 * Returns: the new #AgsPlayRecycling
 *
 * Since: 2.0.0
 */
AgsPlayRecycling*
ags_play_recycling_new(AgsRecycling *source)
{
  AgsPlayRecycling *play_recycling;

  play_recycling = (AgsPlayRecycling *) g_object_new(AGS_TYPE_PLAY_RECYCLING,
						     "source", source,
						     NULL);

  return(play_recycling);
}
