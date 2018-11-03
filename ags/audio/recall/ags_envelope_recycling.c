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

#include <ags/audio/recall/ags_envelope_recycling.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_envelope_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_envelope_recycling_class_init(AgsEnvelopeRecyclingClass *envelope_recycling);
void ags_envelope_recycling_init(AgsEnvelopeRecycling *envelope_recycling);
void ags_envelope_recycling_finalize(GObject *gobject);

/**
 * SECTION:ags_envelope_recycling
 * @short_description: envelopes recycling
 * @title: AgsEnvelopeRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_envelope_recycling.h
 *
 * The #AgsEnvelopeRecycling class envelopes the recycling.
 */

static gpointer ags_envelope_recycling_parent_class = NULL;

GType
ags_envelope_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_envelope_recycling = 0;

    static const GTypeInfo ags_envelope_recycling_info = {
      sizeof (AgsEnvelopeRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_envelope_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEnvelopeRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_envelope_recycling_init,
    };

    ags_type_envelope_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							 "AgsEnvelopeRecycling",
							 &ags_envelope_recycling_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_envelope_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_envelope_recycling_class_init(AgsEnvelopeRecyclingClass *envelope_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_envelope_recycling_parent_class = g_type_class_peek_parent(envelope_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) envelope_recycling;

  gobject->finalize = ags_envelope_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) envelope_recycling;
}

void
ags_envelope_recycling_init(AgsEnvelopeRecycling *envelope_recycling)
{
  AGS_RECALL(envelope_recycling)->name = "ags-envelope";
  AGS_RECALL(envelope_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(envelope_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(envelope_recycling)->xml_type = "ags-envelope-recycling";
  AGS_RECALL(envelope_recycling)->port = NULL;

  AGS_RECALL(envelope_recycling)->child_type = AGS_TYPE_ENVELOPE_AUDIO_SIGNAL;
}

void
ags_envelope_recycling_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_envelope_recycling_parent_class)->finalize(gobject);
}

/**
 * ags_envelope_recycling_new:
 * @source: the #AgsRecycling
 *
 * Create a new instance of #AgsEnvelopeRecycling
 *
 * Returns: the new #AgsEnvelopeRecycling
 *
 * Since: 2.0.0
 */
AgsEnvelopeRecycling*
ags_envelope_recycling_new(AgsRecycling *source)
{
  AgsEnvelopeRecycling *envelope_recycling;

  envelope_recycling = (AgsEnvelopeRecycling *) g_object_new(AGS_TYPE_ENVELOPE_RECYCLING,
							     "source", source,
							     NULL);

  return(envelope_recycling);
}
