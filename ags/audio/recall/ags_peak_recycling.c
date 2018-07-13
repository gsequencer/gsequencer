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

#include <ags/audio/recall/ags_peak_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_peak_audio_signal.h>

void ags_peak_recycling_class_init(AgsPeakRecyclingClass *peak_recycling);
void ags_peak_recycling_init(AgsPeakRecycling *peak_recycling);
void ags_peak_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_peak_recycling
 * @short_description: peaks recycling
 * @title: AgsPeakRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_peak_recycling.h
 *
 * The #AgsPeakRecycling class peaks the recycling.
 */

static gpointer ags_peak_recycling_parent_class = NULL;

GType
ags_peak_recycling_get_type()
{
  static GType ags_type_peak_recycling = 0;

  if(!ags_type_peak_recycling){
    static const GTypeInfo ags_peak_recycling_info = {
      sizeof (AgsPeakRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_peak_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPeakRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_peak_recycling_init,
    };

    ags_type_peak_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsPeakRecycling",
						     &ags_peak_recycling_info,
						     0);
  }

  return(ags_type_peak_recycling);
}

void
ags_peak_recycling_class_init(AgsPeakRecyclingClass *peak_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_peak_recycling_parent_class = g_type_class_peek_parent(peak_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) peak_recycling;

  gobject->finalize = ags_peak_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) peak_recycling;
}

void
ags_peak_recycling_init(AgsPeakRecycling *peak_recycling)
{
  AGS_RECALL(peak_recycling)->name = "ags-peak";
  AGS_RECALL(peak_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(peak_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(peak_recycling)->xml_type = "ags-peak-recycling";
  AGS_RECALL(peak_recycling)->port = NULL;

  AGS_RECALL(peak_recycling)->child_type = AGS_TYPE_PEAK_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(peak_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_peak_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_peak_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_peak_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsPeakRecycling
 *
 * Returns: a new #AgsPeakRecycling
 *
 * Since: 2.0.0
 */
AgsPeakRecycling*
ags_peak_recycling_new(AgsRecycling *source)
{
  AgsPeakRecycling *peak_recycling;

  peak_recycling = (AgsPeakRecycling *) g_object_new(AGS_TYPE_PEAK_RECYCLING,
						     "source", source,
						     NULL);

  return(peak_recycling);
}

