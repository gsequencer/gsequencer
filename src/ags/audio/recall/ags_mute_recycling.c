/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/recall/ags_mute_recycling.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_dynamic_connectable.h>

#include <ags/audio/recall/ags_mute_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_mute_recycling_class_init(AgsMuteRecyclingClass *mute_recycling);
void ags_mute_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mute_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_mute_recycling_init(AgsMuteRecycling *mute_recycling);
void ags_mute_recycling_connect(AgsConnectable *connectable);
void ags_mute_recycling_disconnect(AgsConnectable *connectable);
void ags_mute_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_mute_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_mute_recycling_finalize(GObject *gobject);

void ags_mute_recycling_done(AgsRecall *recall);
void ags_mute_recycling_cancel(AgsRecall *recall);
void ags_mute_recycling_remove(AgsRecall *recall);
AgsRecall* ags_mute_recycling_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

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
static AgsConnectableInterface *ags_mute_recycling_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_mute_recycling_parent_dynamic_connectable_interface;

GType
ags_mute_recycling_get_type()
{
  static GType ags_type_mute_recycling = 0;

  if(!ags_type_mute_recycling){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mute_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mute_recycling_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_mute_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsMuteRecycling\0",
						     &ags_mute_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_mute_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_mute_recycling,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_mute_recycling);
}

void
ags_mute_recycling_class_init(AgsMuteRecyclingClass *mute_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  AgsRecallRecyclingClass *recall_recycling;

  ags_mute_recycling_parent_class = g_type_class_peek_parent(mute_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_recycling;

  gobject->finalize = ags_mute_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) mute_recycling;

  recall->done = ags_mute_recycling_done;
  recall->cancel = ags_mute_recycling_cancel;
  recall->remove = ags_mute_recycling_remove;

  recall->duplicate = ags_mute_recycling_duplicate;

  /* AgsRecallRecycling */
  recall_recycling = (AgsRecallRecyclingClass *) mute_recycling;
}

void
ags_mute_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mute_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mute_recycling_connect;
  connectable->disconnect = ags_mute_recycling_disconnect;
}

void
ags_mute_recycling_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_mute_recycling_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_mute_recycling_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_mute_recycling_disconnect_dynamic;
}

void
ags_mute_recycling_init(AgsMuteRecycling *mute_recycling)
{
  AGS_RECALL(mute_recycling)->name = "ags-mute\0";
  AGS_RECALL(mute_recycling)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(mute_recycling)->build_id = AGS_BUILD_ID;
  AGS_RECALL(mute_recycling)->xml_type = "ags-mute-recycling\0";
  AGS_RECALL(mute_recycling)->port = NULL;

  AGS_RECALL(mute_recycling)->child_type = AGS_TYPE_MUTE_AUDIO_SIGNAL;
}

void
ags_mute_recycling_finalize(GObject *gobject)
{
  AgsMuteRecycling *mute_recycling;

  mute_recycling = AGS_MUTE_RECYCLING(gobject);

  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_mute_recycling_parent_class)->finalize(gobject);
}

void
ags_mute_recycling_connect(AgsConnectable *connectable)
{
  AgsMuteRecycling *mute_recycling;

  ags_mute_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_mute_recycling_disconnect(AgsConnectable *connectable)
{
  AgsMuteRecycling *mute_recycling;

  ags_mute_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mute_recycling_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsMuteRecycling *mute_recycling;
  GObject *gobject;

  ags_mute_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);
}

void
ags_mute_recycling_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsMuteRecycling *mute_recycling;
  GObject *gobject;

  ags_mute_recycling_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void 
ags_mute_recycling_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_mute_recycling_parent_class)->done(recall);

  /* empty */
}

void
ags_mute_recycling_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_mute_recycling_parent_class)->cancel(recall);

  /* empty */
}

void 
ags_mute_recycling_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_mute_recycling_parent_class)->remove(recall);

  /* empty */
}

AgsRecall*
ags_mute_recycling_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsMuteRecycling *mute_recycling, *mute;

  mute_recycling = (AgsMuteRecycling *) recall;
  mute = (AgsMuteRecycling *) AGS_RECALL_CLASS(ags_mute_recycling_parent_class)->duplicate(recall,
											   recall_id,
											   n_params, parameter);

  return((AgsRecall *) mute);
}

/**
 * ags_mute_recycling_new:
 * @source: the source #AgsRecycling
 *
 * Creates an #AgsMuteRecycling
 *
 * Returns: a new #AgsMuteRecycling
 *
 * Since: 0.4
 */
AgsMuteRecycling*
ags_mute_recycling_new(AgsRecycling *source)
{
  AgsMuteRecycling *mute_recycling;

  mute_recycling = (AgsMuteRecycling *) g_object_new(AGS_TYPE_MUTE_RECYCLING,
						     "source\0", source,
						     NULL);

  return(mute_recycling);
}
