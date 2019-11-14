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

#include <ags/audio/recall/ags_eq10_recycling.h>

#include <ags/audio/recall/ags_eq10_audio_signal.h>

void ags_eq10_recycling_class_init(AgsEq10RecyclingClass *eq10_recycling);
void ags_eq10_recycling_init(AgsEq10Recycling *eq10_recycling);
void ags_eq10_recycling_connect(AgsConnectable *connectable);
void ags_eq10_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_eq10_recycling
 * @short_description: 10 band equalizer on recycling
 * @title: AgsEq10Recycling
 * @section_id:
 * @include: ags/audio/recall/ags_eq10_recycling.h
 *
 * The #AgsEq10Recycling class does a 10 band equalizer on the recycling.
 */

static gpointer ags_eq10_recycling_parent_class = NULL;

GType
ags_eq10_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_eq10_recycling = 0;

    static const GTypeInfo ags_eq10_recycling_info = {
      sizeof(AgsEq10RecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_eq10_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEq10Recycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_eq10_recycling_init,
    };

    ags_type_eq10_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsEq10Recycling",
						     &ags_eq10_recycling_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_eq10_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_eq10_recycling_class_init(AgsEq10RecyclingClass *eq10_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_eq10_recycling_parent_class = g_type_class_peek_parent(eq10_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) eq10_recycling;

  gobject->finalize = ags_eq10_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) eq10_recycling;
}

void
ags_eq10_recycling_init(AgsEq10Recycling *eq10_recycling)
{
  AGS_RECALL(eq10_recycling)->name = "ags-eq10";
  AGS_RECALL(eq10_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(eq10_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(eq10_recycling)->xml_type = "ags-eq10-recycling";
  AGS_RECALL(eq10_recycling)->port = NULL;

  AGS_RECALL(eq10_recycling)->child_type = AGS_TYPE_EQ10_AUDIO_SIGNAL;
}

void
ags_eq10_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_eq10_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_eq10_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsEq10Recycling
 *
 * Returns: the new #AgsEq10Recycling
 *
 * Since: 2.0.0
 */
AgsEq10Recycling*
ags_eq10_recycling_new(AgsRecycling *source)
{
  AgsEq10Recycling *eq10_recycling;

  eq10_recycling = (AgsEq10Recycling *) g_object_new(AGS_TYPE_EQ10_RECYCLING,
						     "source", source,
						     NULL);

  return(eq10_recycling);
}
