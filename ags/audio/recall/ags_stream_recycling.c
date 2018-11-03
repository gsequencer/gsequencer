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

#include <ags/audio/recall/ags_stream_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_stream_audio_signal.h>

void ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling);
void ags_stream_recycling_init(AgsStreamRecycling *stream_recycling);
void ags_stream_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_stream_recycling
 * @short_description: streams recycling
 * @title: AgsStreamRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_stream_recycling.h
 *
 * The #AgsStreamRecycling streams the recycling with appropriate #AgsRecallID.
 */

static gpointer ags_stream_recycling_parent_class = NULL;

GType
ags_stream_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_stream_recycling = 0;

    static const GTypeInfo ags_stream_recycling_info = {
      sizeof (AgsStreamRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_recycling_init,
    };

    ags_type_stream_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsStreamRecycling",
						       &ags_stream_recycling_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_stream_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_stream_recycling_class_init(AgsStreamRecyclingClass *stream_recycling)
{
  GObjectClass *gobject;

  ags_stream_recycling_parent_class = g_type_class_peek_parent(stream_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_recycling;

  gobject->finalize = ags_stream_recycling_finalize;
}

void
ags_stream_recycling_init(AgsStreamRecycling *stream_recycling)
{
  AGS_RECALL(stream_recycling)->name = "ags-stream";
  AGS_RECALL(stream_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(stream_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(stream_recycling)->xml_type = "ags-stream-recycling";
  AGS_RECALL(stream_recycling)->port = NULL;

  AGS_RECALL(stream_recycling)->child_type = AGS_TYPE_STREAM_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(stream_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_stream_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_stream_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_stream_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsStreamRecycling
 *
 * Returns: the new #AgsStreamRecycling
 *
 * Since: 2.0.0
 */
AgsStreamRecycling*
ags_stream_recycling_new(AgsRecycling *source)
{
  AgsStreamRecycling *stream_recycling;

  stream_recycling = (AgsStreamRecycling *) g_object_new(AGS_TYPE_STREAM_RECYCLING,
							 "source", source,
							 NULL);

  return(stream_recycling);
}
