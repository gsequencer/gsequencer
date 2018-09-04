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

#include <ags/audio/recall/ags_volume_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_volume_audio_signal.h>

void ags_volume_recycling_class_init(AgsVolumeRecyclingClass *volume_recycling);
void ags_volume_recycling_init(AgsVolumeRecycling *volume_recycling);
void ags_volume_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_volume_recycling
 * @short_description: volumes recycling
 * @title: AgsVolumeRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_volume_recycling.h
 *
 * The #AgsVolumeRecycling class volumes the recycling.
 */

static gpointer ags_volume_recycling_parent_class = NULL;

GType
ags_volume_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_volume_recycling = 0;

    static const GTypeInfo ags_volume_recycling_info = {
      sizeof (AgsVolumeRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_recycling_init,
    };

    ags_type_volume_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsVolumeRecycling",
						       &ags_volume_recycling_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_volume_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_volume_recycling_class_init(AgsVolumeRecyclingClass *volume_recycling)
{
  GObjectClass *gobject;

  ags_volume_recycling_parent_class = g_type_class_peek_parent(volume_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_recycling;

  gobject->finalize = ags_volume_recycling_finalize;
}

void
ags_volume_recycling_init(AgsVolumeRecycling *volume_recycling)
{
  AGS_RECALL(volume_recycling)->name = "ags-volume";
  AGS_RECALL(volume_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(volume_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(volume_recycling)->xml_type = "ags-volume-recycling";
  AGS_RECALL(volume_recycling)->port = NULL;

  AGS_RECALL(volume_recycling)->child_type = AGS_TYPE_VOLUME_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(volume_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_volume_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_volume_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_volume_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsVolumeRecycling
 *
 * Returns: the new #AgsVolumeRecycling
 *
 * Since: 2.0.0
 */
AgsVolumeRecycling*
ags_volume_recycling_new(AgsRecycling *source)
{
  AgsVolumeRecycling *volume_recycling;

  volume_recycling = (AgsVolumeRecycling *) g_object_new(AGS_TYPE_VOLUME_RECYCLING,
							 "source", source,
							 NULL);

  return(volume_recycling);
}

