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

#include <ags/audio/recall/ags_lfo_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_lfo_audio_signal.h>

void ags_lfo_recycling_class_init(AgsLfoRecyclingClass *lfo_recycling);
void ags_lfo_recycling_init(AgsLfoRecycling *lfo_recycling);
void ags_lfo_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_lfo_recycling
 * @short_description: lfos recycling
 * @title: AgsLfoRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_lfo_recycling.h
 *
 * The #AgsLfoRecycling class lfos the recycling.
 */

static gpointer ags_lfo_recycling_parent_class = NULL;

GType
ags_lfo_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lfo_recycling = 0;

    static const GTypeInfo ags_lfo_recycling_info = {
      sizeof (AgsLfoRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lfo_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLfoRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lfo_recycling_init,
    };

    ags_type_lfo_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						    "AgsLfoRecycling",
						    &ags_lfo_recycling_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lfo_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_lfo_recycling_class_init(AgsLfoRecyclingClass *lfo_recycling)
{
  GObjectClass *gobject;

  ags_lfo_recycling_parent_class = g_type_class_peek_parent(lfo_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) lfo_recycling;

  gobject->finalize = ags_lfo_recycling_finalize;
}

void
ags_lfo_recycling_init(AgsLfoRecycling *lfo_recycling)
{
  AGS_RECALL(lfo_recycling)->name = "ags-lfo";
  AGS_RECALL(lfo_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(lfo_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(lfo_recycling)->xml_type = "ags-lfo-recycling";
  AGS_RECALL(lfo_recycling)->port = NULL;

  AGS_RECALL(lfo_recycling)->child_type = AGS_TYPE_LFO_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(lfo_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_lfo_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_lfo_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_lfo_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsLfoRecycling
 *
 * Returns: the new #AgsLfoRecycling
 *
 * Since: 2.3.0
 */
AgsLfoRecycling*
ags_lfo_recycling_new(AgsRecycling *source)
{
  AgsLfoRecycling *lfo_recycling;

  lfo_recycling = (AgsLfoRecycling *) g_object_new(AGS_TYPE_LFO_RECYCLING,
						   "source", source,
						   NULL);

  return(lfo_recycling);
}
