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

#include <ags/audio/recall/ags_copy_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_copy_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling);
void ags_copy_recycling_init(AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_copy_recycling
 * @short_description: copy recycling
 * @title: AgsCopyRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_copy_recycling.h
 *
 * The #AgsCopyRecycling class copies the recycling.
 */

static gpointer ags_copy_recycling_parent_class = NULL;

GType
ags_copy_recycling_get_type()
{
  static GType ags_type_copy_recycling = 0;

  if(!ags_type_copy_recycling){
    static const GTypeInfo ags_copy_recycling_info = {
      sizeof(AgsCopyRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCopyRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_recycling_init,
    };

    ags_type_copy_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsCopyRecycling",
						     &ags_copy_recycling_info,
						     0);
  }

  return(ags_type_copy_recycling);
}

void
ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_copy_recycling_parent_class = g_type_class_peek_parent(copy_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_recycling;

  gobject->finalize = ags_copy_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_recycling;
}

void
ags_copy_recycling_init(AgsCopyRecycling *copy_recycling)
{
  AGS_RECALL(copy_recycling)->name = "ags-copy";
  AGS_RECALL(copy_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_recycling)->xml_type = "ags-copy-recycling";
  AGS_RECALL(copy_recycling)->port = NULL;

  AGS_RECALL(copy_recycling)->child_type = AGS_TYPE_COPY_AUDIO_SIGNAL;
  AGS_RECALL_RECYCLING(copy_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_DESTINATION);
}

void
ags_copy_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_copy_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_copy_recycling_new:
 * @destination: the destination #AgsRecycling
 * @source: the source #AgsRecycling
 *
 * Create a new instance of #AgsCopyRecycling
 *
 * Returns: the new #AgsCopyRecycling
 *
 * Since: 2.0.0
 */
AgsCopyRecycling*
ags_copy_recycling_new(AgsRecycling *destination,
		       AgsRecycling *source)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = (AgsCopyRecycling *) g_object_new(AGS_TYPE_COPY_RECYCLING,
						     "destination", destination,
						     "source", source,
						     NULL);

  return(copy_recycling);
}
