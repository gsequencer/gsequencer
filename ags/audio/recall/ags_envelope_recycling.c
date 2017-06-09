/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/recall/ags_envelope_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_envelope_recycling_class_init(AgsEnvelopeRecyclingClass *envelope_recycling);
void ags_envelope_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_envelope_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_envelope_recycling_init(AgsEnvelopeRecycling *envelope_recycling);
void ags_envelope_recycling_connect(AgsConnectable *connectable);
void ags_envelope_recycling_disconnect(AgsConnectable *connectable);
void ags_envelope_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_envelope_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_envelope_recycling_finalize(GObject *gobject);

void ags_envelope_recycling_done(AgsRecall *recall);
void ags_envelope_recycling_cancel(AgsRecall *recall);
void ags_envelope_recycling_remove(AgsRecall *recall);
AgsRecall* ags_envelope_recycling_duplicate(AgsRecall *recall,
					    AgsRecallID *recall_id,
					    guint *n_params, GParameter *parameter);

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
static AgsConnectableInterface *ags_envelope_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_envelope_recycling_parent_dynamic_connectable_interface;

GType
ags_envelope_recycling_get_type()
{
  static GType ags_type_envelope_recycling = 0;

  if(!ags_type_envelope_recycling){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_envelope_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_envelope_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							 "AgsEnvelopeRecycling",
							 &ags_envelope_recycling_info,
							 0);

    g_type_add_interface_static(ags_type_envelope_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_envelope_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_envelope_recycling);
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

  recall->done = ags_envelope_recycling_done;
  recall->cancel = ags_envelope_recycling_cancel;
  recall->remove = ags_envelope_recycling_remove;

  recall->duplicate = ags_envelope_recycling_duplicate;
}

void
ags_envelope_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_envelope_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_envelope_recycling_connect;
  connectable->disconnect = ags_envelope_recycling_disconnect;
}

void
ags_envelope_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_envelope_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_envelope_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_envelope_recycling_disconnect_dynamic;
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
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_envelope_recycling_parent_class)->finalize(gobject);
}

void
ags_envelope_recycling_connect(AgsConnectable *connectable)
{
  AgsEnvelopeRecycling *envelope_recycling;

  ags_envelope_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_envelope_recycling_disconnect(AgsConnectable *connectable)
{
  AgsEnvelopeRecycling *envelope_recycling;

  ags_envelope_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_envelope_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsEnvelopeRecycling *envelope_recycling;
  GObject *gobject;

  ags_envelope_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_envelope_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsEnvelopeRecycling *envelope_recycling;
  GObject *gobject;

  ags_envelope_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void 
ags_envelope_recycling_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_envelope_recycling_parent_class)->done(recall);

  /* empty */
}

void
ags_envelope_recycling_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_envelope_recycling_parent_class)->cancel(recall);

  /* empty */
}

void 
ags_envelope_recycling_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_envelope_recycling_parent_class)->remove(recall);

  /* empty */
}

AgsRecall*
ags_envelope_recycling_duplicate(AgsRecall *recall,
				 AgsRecallID *recall_id,
				 guint *n_params, GParameter *parameter)
{
  AgsEnvelopeRecycling *envelope;

  envelope = (AgsEnvelopeRecycling *) AGS_RECALL_CLASS(ags_envelope_recycling_parent_class)->duplicate(recall,
												       recall_id,
												       n_params, parameter);

  return((AgsRecall *) envelope);
}

/**
 * ags_envelope_recycling_new:
 * @source: the source #AgsRecycling
 *
 * Creates an #AgsEnvelopeRecycling
 *
 * Returns: a new #AgsEnvelopeRecycling
 *
 * Since: 0.6
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
