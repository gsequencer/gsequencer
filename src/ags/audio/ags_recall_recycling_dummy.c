/* This file is part of GSequencer.
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

#include <ags/audio/ags_recall_recycling_dummy.h>
#include <ags/audio/ags_recall_audio_signal.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/task/ags_remove_audio_signal.h>

void ags_recall_recycling_dummy_class_init(AgsRecallRecyclingDummyClass *recall_recycling_dummy);
void ags_recall_recycling_dummy_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_recycling_dummy_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_recycling_dummy_init(AgsRecallRecyclingDummy *recall_recycling_dummy);
void ags_recall_recycling_dummy_connect(AgsConnectable *connectable);
void ags_recall_recycling_dummy_disconnect(AgsConnectable *connectable);
void ags_recall_recycling_dummy_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_recycling_dummy_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_recycling_dummy_finalize(GObject *gobject);

AgsRecall* ags_recall_recycling_dummy_duplicate(AgsRecall *recall,
						AgsRecallID *recall_id,
						guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_recall_recycling_dummy
 * @short_description: recycling dummy dynamic context of recall
 * @title: AgsRecallRecyclingDummy
 * @section_id:
 * @include: ags/audio/ags_recall_recycling_dummy.h
 *
 * #AgsRecallRecyclingDummy acts as recycling dummy dynamic recall.
 */

static gpointer ags_recall_recycling_dummy_parent_class = NULL;
static AgsConnectableInterface *ags_recall_recycling_dummy_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_recall_recycling_dummy_parent_dynamic_connectable_interface;

GType
ags_recall_recycling_dummy_get_type()
{
  static GType ags_type_recall_recycling_dummy = 0;

  if(!ags_type_recall_recycling_dummy){
    static const GTypeInfo ags_recall_recycling_dummy_info = {
      sizeof (AgsRecallRecyclingDummyClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_recycling_dummy_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallRecyclingDummy),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_recycling_dummy_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_recycling_dummy_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_recycling_dummy_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_recycling_dummy = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
							     "AgsRecallRecyclingDummy\0",
							     &ags_recall_recycling_dummy_info,
							     0);

    g_type_add_interface_static(ags_type_recall_recycling_dummy,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_recycling_dummy,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_recall_recycling_dummy);
}

void
ags_recall_recycling_dummy_class_init(AgsRecallRecyclingDummyClass *recall_recycling_dummy)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_recall_recycling_dummy_parent_class = g_type_class_peek_parent(recall_recycling_dummy);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_recycling_dummy;

  gobject->finalize = ags_recall_recycling_dummy_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_recycling_dummy;

  recall->duplicate = ags_recall_recycling_dummy_duplicate;
}

void
ags_recall_recycling_dummy_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_recycling_dummy_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_recycling_dummy_connect;
  connectable->disconnect = ags_recall_recycling_dummy_disconnect;
}

void
ags_recall_recycling_dummy_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_recycling_dummy_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_recycling_dummy_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_recycling_dummy_disconnect_dynamic;
}

void
ags_recall_recycling_dummy_init(AgsRecallRecyclingDummy *recall_recycling_dummy)
{
  AGS_RECALL(recall_recycling_dummy)->name = "ags-recall\0";
  AGS_RECALL(recall_recycling_dummy)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(recall_recycling_dummy)->build_id = AGS_BUILD_ID;
  AGS_RECALL(recall_recycling_dummy)->xml_type = "ags-recall-recycling-dummy\0";
  AGS_RECALL(recall_recycling_dummy)->port = NULL;

  AGS_RECALL(recall_recycling_dummy)->flags |= AGS_RECALL_PERSISTENT;
  AGS_RECALL(recall_recycling_dummy)->child_type = G_TYPE_NONE;
}

void
ags_recall_recycling_dummy_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_recall_recycling_dummy_parent_class)->finalize(gobject);
}

void
ags_recall_recycling_dummy_connect(AgsConnectable *connectable)
{
  AgsRecallRecyclingDummy *recall_recycling_dummy;

  recall_recycling_dummy = AGS_RECALL_RECYCLING_DUMMY(connectable);

  ags_recall_recycling_dummy_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_recycling_dummy_disconnect(AgsConnectable *connectable)
{
  ags_recall_recycling_dummy_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_recycling_dummy_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsRecallRecyclingDummy *recall_recycling_dummy;

  ags_recall_recycling_dummy_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_recall_recycling_dummy_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_recall_recycling_dummy_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);
}

AgsRecall*
ags_recall_recycling_dummy_duplicate(AgsRecall *recall,
				     AgsRecallID *recall_id,
				     guint *n_params, GParameter *parameter)
{
  AgsRecallRecyclingDummy *copy;

  copy = (AgsRecallRecyclingDummy *) AGS_RECALL_CLASS(ags_recall_recycling_dummy_parent_class)->duplicate(recall,
													  recall_id,
													  n_params, parameter);
  AGS_RECALL(copy)->child_type = recall->child_type;

  return((AgsRecall *) copy);
}

/**
 * ags_recall_recycling_dummy_new:
 * @source the source #AgsRecycling
 * @child_type child type
 *
 * Creates an #AgsRecallRecyclingDummy.
 *
 * Returns: a new #AgsRecallRecyclingDummy.
 *
 * Since: 0.4
 */
AgsRecallRecyclingDummy*
ags_recall_recycling_dummy_new(AgsRecycling *recycling, GType child_type)
{
  AgsRecallRecyclingDummy *recall_recycling_dummy;

  recall_recycling_dummy = (AgsRecallRecyclingDummy *) g_object_new(AGS_TYPE_RECALL_RECYCLING_DUMMY,
								    "source\0", recycling,
								    NULL);
  AGS_RECALL(recall_recycling_dummy)->child_type = child_type;

  return(recall_recycling_dummy);
}
