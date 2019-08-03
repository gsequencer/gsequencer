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

#include <ags/audio/ags_generic_recall_recycling.h>

#include <ags/libags.h>

#include <ags/audio/ags_recall_audio_signal.h>

void ags_generic_recall_recycling_class_init(AgsGenericRecallRecyclingClass *generic_recall_recycling);
void ags_generic_recall_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_generic_recall_recycling_init(AgsGenericRecallRecycling *generic_recall_recycling);

/**
 * SECTION:ags_generic_recall_recycling
 * @short_description: generic recycling context of recall
 * @title: AgsGenericRecallRecycling
 * @section_id:
 * @include: ags/audio/ags_generic_recall_recycling.h
 *
 * #AgsGenericRecallRecycling acts as generic recycling recall.
 */

static gpointer ags_generic_recall_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_generic_recall_recycling_parent_connectable_interface;

GType
ags_generic_recall_recycling_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_generic_recall_recycling = 0;

    static const GTypeInfo ags_generic_recall_recycling_info = {
      sizeof (AgsGenericRecallRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_generic_recall_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGenericRecallRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_generic_recall_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_generic_recall_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_generic_recall_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							       "AgsGenericRecallRecycling",
							       &ags_generic_recall_recycling_info,
							       0);

    g_type_add_interface_static(ags_type_generic_recall_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_generic_recall_recycling);
  }

  return g_define_type_id__volatile;
}

void
ags_generic_recall_recycling_class_init(AgsGenericRecallRecyclingClass *generic_recall_recycling)
{
  ags_generic_recall_recycling_parent_class = g_type_class_peek_parent(generic_recall_recycling);
}

void
ags_generic_recall_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_generic_recall_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_generic_recall_recycling_init(AgsGenericRecallRecycling *generic_recall_recycling)
{
  AGS_RECALL(generic_recall_recycling)->name = "ags-recall";
  AGS_RECALL(generic_recall_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(generic_recall_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(generic_recall_recycling)->xml_type = "ags-generic-recall-recycling";
  AGS_RECALL(generic_recall_recycling)->port = NULL;

  AGS_RECALL(generic_recall_recycling)->behaviour_flags |= AGS_SOUND_BEHAVIOUR_PERSISTENT;
  
  AGS_RECALL(generic_recall_recycling)->child_type = G_TYPE_NONE;
}

/**
 * ags_generic_recall_recycling_new:
 * @recycling: the source #AgsRecycling
 * @child_type: child type
 *
 * Creates an #AgsGenericRecallRecycling.
 *
 * Returns: a new #AgsGenericRecallRecycling.
 *
 * Since: 2.0.0
 */
AgsGenericRecallRecycling*
ags_generic_recall_recycling_new(AgsRecycling *recycling, GType child_type)
{
  AgsGenericRecallRecycling *generic_recall_recycling;

  generic_recall_recycling = (AgsGenericRecallRecycling *) g_object_new(AGS_TYPE_GENERIC_RECALL_RECYCLING,
									"source", recycling,
									"child-type", child_type,
									NULL);
  
  return(generic_recall_recycling);
}
