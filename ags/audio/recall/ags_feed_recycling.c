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

#include <ags/audio/recall/ags_feed_recycling.h>
#include <ags/audio/recall/ags_feed_channel.h>
#include <ags/audio/recall/ags_feed_audio_signal.h>

#include <ags/lib/ags_parameter.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>

void ags_feed_recycling_class_init(AgsFeedRecyclingClass *feed_recycling);
void ags_feed_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_feed_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_feed_recycling_init(AgsFeedRecycling *feed_recycling);
void ags_feed_recycling_connect(AgsConnectable *connectable);
void ags_feed_recycling_disconnect(AgsConnectable *connectable);
void ags_feed_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_feed_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_feed_recycling_finalize(GObject *gobject);

AgsRecall* ags_feed_recycling_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

/**
 * SECTION:ags_feed_recycling
 * @short_description: feeds recycling
 * @title: AgsFeedRecycling
 * @section_id:
 * @include: ags/audio/recall/ags_feed_recycling.h
 *
 * The #AgsFeedRecycling class feeds the recycling.
 */

static gpointer ags_feed_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_feed_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_feed_recycling_parent_dynamic_connectable_interface;

GType
ags_feed_recycling_get_type()
{
  static GType ags_type_feed_recycling = 0;

  if(!ags_type_feed_recycling){
    static const GTypeInfo ags_feed_recycling_info = {
      sizeof (AgsFeedRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_feed_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_feed_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_feed_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsFeedRecycling\0",
						     &ags_feed_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_feed_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_feed_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return (ags_type_feed_recycling);
}

void
ags_feed_recycling_class_init(AgsFeedRecyclingClass *feed_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_feed_recycling_parent_class = g_type_class_peek_parent(feed_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_recycling;

  gobject->finalize = ags_feed_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) feed_recycling;

  recall->duplicate = ags_feed_recycling_duplicate;
}

void
ags_feed_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_feed_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_feed_recycling_connect;
  connectable->disconnect = ags_feed_recycling_disconnect;
}

void
ags_feed_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_feed_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_feed_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_feed_recycling_disconnect_dynamic;
}

void
ags_feed_recycling_init(AgsFeedRecycling *feed_recycling)
{
  AGS_RECALL(feed_recycling)->name = "ags-feed\0";
  AGS_RECALL(feed_recycling)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_recycling)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_recycling)->xml_type = "ags-feed-recycling\0";
  AGS_RECALL(feed_recycling)->port = NULL;

  AGS_RECALL(feed_recycling)->child_type = AGS_TYPE_FEED_AUDIO_SIGNAL;

  AGS_RECALL_RECYCLING(feed_recycling)->flags |= (AGS_RECALL_RECYCLING_MAP_CHILD_SOURCE);
}

void
ags_feed_recycling_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_feed_recycling_parent_class)->finalize(gobject);
}

void
ags_feed_recycling_connect(AgsConnectable *connectable)
{ 
  /* call parent */
  ags_feed_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_feed_recycling_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_feed_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_feed_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_feed_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_feed_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_feed_recycling_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

AgsRecall*
ags_feed_recycling_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsFeedRecycling *copy;

  copy = (AgsFeedRecycling *) AGS_RECALL_CLASS(ags_feed_recycling_parent_class)->duplicate(recall,
											   recall_id,
											   n_params, parameter);

  return((AgsRecall *) copy);
}

/**
 * ags_feed_recycling_new:
 * @recycling: the source #AgsRecycling
 *
 * Creates an #AgsFeedRecycling
 *
 * Returns: a new #AgsFeedRecycling
 *
 * Since: 1.0.0
 */
AgsFeedRecycling*
ags_feed_recycling_new(AgsRecycling *recycling)
{
  AgsFeedRecycling *feed_recycling;

  feed_recycling = (AgsFeedRecycling *) g_object_new(AGS_TYPE_FEED_RECYCLING,
						     "source\0", recycling,
						     NULL);

  return(feed_recycling);
}
