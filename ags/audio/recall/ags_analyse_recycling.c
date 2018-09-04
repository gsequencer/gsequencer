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

#include <ags/audio/recall/ags_analyse_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_analyse_audio_signal.h>

void ags_analyse_recycling_class_init(AgsAnalyseRecyclingClass *analyse_recycling);
void ags_analyse_recycling_init(AgsAnalyseRecycling *analyse_recycling);
void ags_analyse_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_analyse_recycling
 * @short_description: analyses recycling
 * @title: AgsAnalyseRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_analyse_recycling.h
 *
 * The #AgsAnalyseRecycling class analyses the recycling.
 */

static gpointer ags_analyse_recycling_parent_class = NULL;

GType
ags_analyse_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_analyse_recycling = 0;

    static const GTypeInfo ags_analyse_recycling_info = {
      sizeof (AgsAnalyseRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_analyse_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAnalyseRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_analyse_recycling_init,
    };

    ags_type_analyse_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							"AgsAnalyseRecycling",
							&ags_analyse_recycling_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_analyse_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_analyse_recycling_class_init(AgsAnalyseRecyclingClass *analyse_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_analyse_recycling_parent_class = g_type_class_peek_parent(analyse_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) analyse_recycling;

  gobject->finalize = ags_analyse_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) analyse_recycling;
}

void
ags_analyse_recycling_init(AgsAnalyseRecycling *analyse_recycling)
{
  AGS_RECALL(analyse_recycling)->name = "ags-analyse";
  AGS_RECALL(analyse_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(analyse_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(analyse_recycling)->xml_type = "ags-analyse-recycling";
  AGS_RECALL(analyse_recycling)->port = NULL;

  AGS_RECALL(analyse_recycling)->child_type = AGS_TYPE_ANALYSE_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(analyse_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_analyse_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_analyse_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_analyse_recycling_new:
 * @recycling: the source #AgsRecycling
 *
 * Create a new instance of #AgsAnalyseRecycling
 *
 * Returns: the new #AgsAnalyseRecycling
 *
 * Since: 2.0.0
 */
AgsAnalyseRecycling*
ags_analyse_recycling_new(AgsRecycling *source)
{
  AgsAnalyseRecycling *analyse_recycling;

  analyse_recycling = (AgsAnalyseRecycling *) g_object_new(AGS_TYPE_ANALYSE_RECYCLING,
							   "source", source,
							   NULL);

  return(analyse_recycling);
}
