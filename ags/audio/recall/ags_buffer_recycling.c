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

#include <ags/audio/recall/ags_buffer_recycling.h>

#include <ags/audio/recall/ags_buffer_audio_signal.h>

void ags_buffer_recycling_class_init(AgsBufferRecyclingClass *buffer_recycling);
void ags_buffer_recycling_init(AgsBufferRecycling *buffer_recycling);
void ags_buffer_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_buffer_recycling
 * @short_description: buffer recycling
 * @title: AgsBufferRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_buffer_recycling.h
 *
 * The #AgsBufferRecycling class buffers the recycling.
 */

static gpointer ags_buffer_recycling_parent_class = NULL;

GType
ags_buffer_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_buffer_recycling = 0;

    static const GTypeInfo ags_buffer_recycling_info = {
      sizeof(AgsBufferRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_buffer_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsBufferRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_buffer_recycling_init,
    };

    ags_type_buffer_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						       "AgsBufferRecycling",
						       &ags_buffer_recycling_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_buffer_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_buffer_recycling_class_init(AgsBufferRecyclingClass *buffer_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_buffer_recycling_parent_class = g_type_class_peek_parent(buffer_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) buffer_recycling;

  gobject->finalize = ags_buffer_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) buffer_recycling;
}

void
ags_buffer_recycling_init(AgsBufferRecycling *buffer_recycling)
{
  AGS_RECALL(buffer_recycling)->name = "ags-buffer";
  AGS_RECALL(buffer_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(buffer_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(buffer_recycling)->xml_type = "ags-buffer-recycling";

  AGS_RECALL(buffer_recycling)->port = NULL;

  AGS_RECALL(buffer_recycling)->child_type = AGS_TYPE_BUFFER_AUDIO_SIGNAL;
}

void
ags_buffer_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_buffer_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_buffer_recycling_new:
 * @destination: the destination #AgsRecycling
 * @source: the source #AgsRecycling
 *
 * Create a new instance of #AgsBufferRecycling
 *
 * Returns: the new #AgsBufferRecycling
 *
 * Since: 3.0.0
 */
AgsBufferRecycling*
ags_buffer_recycling_new(AgsRecycling *destination,
			 AgsRecycling *source)
{
  AgsBufferRecycling *buffer_recycling;

  buffer_recycling = (AgsBufferRecycling *) g_object_new(AGS_TYPE_BUFFER_RECYCLING,
							 "destination", destination,
							 "source", source,
							 NULL);

  return(buffer_recycling);
}
