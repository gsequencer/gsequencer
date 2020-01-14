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

#include <ags/audio/recall/ags_mute_recycling.h>

#include <ags/audio/recall/ags_mute_audio_signal.h>

void ags_mute_recycling_class_init(AgsMuteRecyclingClass *mute_recycling);
void ags_mute_recycling_init(AgsMuteRecycling *mute_recycling);
void ags_mute_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_mute_recycling
 * @short_description: mutes recycling
 * @title: AgsMuteRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_mute_recycling.h
 *
 * The #AgsMuteRecycling class mutes the recycling.
 */

static gpointer ags_mute_recycling_parent_class = NULL;

GType
ags_mute_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_mute_recycling = 0;

    static const GTypeInfo ags_mute_recycling_info = {
      sizeof (AgsMuteRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMuteRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_recycling_init,
    };

    ags_type_mute_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsMuteRecycling",
						     &ags_mute_recycling_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_mute_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_mute_recycling_class_init(AgsMuteRecyclingClass *mute_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_mute_recycling_parent_class = g_type_class_peek_parent(mute_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_recycling;

  gobject->finalize = ags_mute_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) mute_recycling;
}

void
ags_mute_recycling_init(AgsMuteRecycling *mute_recycling)
{
  AGS_RECALL(mute_recycling)->name = "ags-mute";
  AGS_RECALL(mute_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_recycling)->xml_type = "ags-mute-recycling";
  AGS_RECALL(mute_recycling)->port = NULL;

  AGS_RECALL(mute_recycling)->child_type = AGS_TYPE_MUTE_AUDIO_SIGNAL;
}

void
ags_mute_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_mute_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_mute_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsMuteRecycling
 *
 * Returns: the new #AgsMuteRecycling
 *
 * Since: 3.0.0
 */
AgsMuteRecycling*
ags_mute_recycling_new(AgsRecycling *source)
{
  AgsMuteRecycling *mute_recycling;

  mute_recycling = (AgsMuteRecycling *) g_object_new(AGS_TYPE_MUTE_RECYCLING,
						     "source", source,
						     NULL);

  return(mute_recycling);
}
