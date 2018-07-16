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

#include <ags/audio/recall/ags_prepare_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_prepare_audio_signal.h>

void ags_prepare_recycling_class_init(AgsPrepareRecyclingClass *prepare_recycling);
void ags_prepare_recycling_init(AgsPrepareRecycling *prepare_recycling);
void ags_prepare_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_prepare_recycling
 * @short_description: prepares recycling
 * @title: AgsPrepareRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_prepare_recycling.h
 *
 * The #AgsPrepareRecycling class prepares the recycling.
 */

static gpointer ags_prepare_recycling_parent_class = NULL;

GType
ags_prepare_recycling_get_type()
{
  static GType ags_type_prepare_recycling = 0;

  if(!ags_type_prepare_recycling){
    static const GTypeInfo ags_prepare_recycling_info = {
      sizeof (AgsPrepareRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_prepare_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPrepareRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_prepare_recycling_init,
    };

    ags_type_prepare_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							"AgsPrepareRecycling",
							&ags_prepare_recycling_info,
							0);
  }

  return (ags_type_prepare_recycling);
}

void
ags_prepare_recycling_class_init(AgsPrepareRecyclingClass *prepare_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_prepare_recycling_parent_class = g_type_class_peek_parent(prepare_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) prepare_recycling;

  gobject->finalize = ags_prepare_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) prepare_recycling;
}

void
ags_prepare_recycling_init(AgsPrepareRecycling *prepare_recycling)
{
  AGS_RECALL(prepare_recycling)->name = "ags-prepare";
  AGS_RECALL(prepare_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(prepare_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(prepare_recycling)->xml_type = "ags-prepare-recycling";
  AGS_RECALL(prepare_recycling)->port = NULL;

  AGS_RECALL(prepare_recycling)->child_type = AGS_TYPE_PREPARE_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(prepare_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_prepare_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_prepare_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_prepare_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsPrepareRecycling
 *
 * Returns: the new #AgsPrepareRecycling
 *
 * Since: 2.0.0
 */
AgsPrepareRecycling*
ags_prepare_recycling_new(AgsRecycling *source)
{
  AgsPrepareRecycling *prepare_recycling;

  prepare_recycling = (AgsPrepareRecycling *) g_object_new(AGS_TYPE_PREPARE_RECYCLING,
							   "source", source,
							   NULL);

  return(prepare_recycling);
}
