/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/ags_recall_channel_run_dummy.h>
#include <ags/audio/ags_recall_recycling_dummy.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

void ags_recall_channel_run_dummy_class_init(AgsRecallChannelRunDummyClass *recall_channel_run_dummy);
void ags_recall_channel_run_dummy_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_channel_run_dummy_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_recall_channel_run_dummy_plugin_interface_init(AgsPluginInterface *plugin);
void ags_recall_channel_run_dummy_init(AgsRecallChannelRunDummy *recall_channel_run_dummy);
void ags_recall_channel_run_dummy_connect(AgsConnectable *connectable);
void ags_recall_channel_run_dummy_disconnect(AgsConnectable *connectable);
void ags_recall_channel_run_dummy_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_channel_run_dummy_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_recall_channel_run_dummy_finalize(GObject *gobject);

AgsRecall* ags_recall_channel_run_dummy_duplicate(AgsRecall *recall,
						  AgsRecallID *recall_id,
						  guint *n_params, GParameter *parameter);

static gpointer ags_recall_channel_run_dummy_parent_class = NULL;
static AgsConnectableInterface *ags_recall_channel_run_dummy_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_recall_channel_run_dummy_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_recall_channel_run_dummy_parent_plugin_interface;

GType
ags_recall_channel_run_dummy_get_type()
{
  static GType ags_type_recall_channel_run_dummy = 0;

  if(!ags_type_recall_channel_run_dummy){
    static const GTypeInfo ags_recall_channel_run_dummy_info = {
      sizeof (AgsRecallChannelRunDummyClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_channel_run_dummy_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallChannelRunDummy),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_channel_run_dummy_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_dummy_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_dummy_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_recall_channel_run_dummy_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_recall_channel_run_dummy = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY,
							       "AgsRecallChannelRunDummy\0",
							       &ags_recall_channel_run_dummy_info,
							       0);

    g_type_add_interface_static(ags_type_recall_channel_run_dummy,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_channel_run_dummy,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_recall_channel_run_dummy,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_recall_channel_run_dummy);
}

void
ags_recall_channel_run_dummy_class_init(AgsRecallChannelRunDummyClass *recall_channel_run_dummy)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_recall_channel_run_dummy_parent_class = g_type_class_peek_parent(recall_channel_run_dummy);

  /* GObjectClass */
  gobject = (GObjectClass *) recall_channel_run_dummy;

  gobject->finalize = ags_recall_channel_run_dummy_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) recall_channel_run_dummy;

  recall->duplicate = ags_recall_channel_run_dummy_duplicate;
}

void
ags_recall_channel_run_dummy_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_recall_channel_run_dummy_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_recall_channel_run_dummy_connect;
  connectable->disconnect = ags_recall_channel_run_dummy_disconnect;
}

void
ags_recall_channel_run_dummy_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_recall_channel_run_dummy_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_recall_channel_run_dummy_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_recall_channel_run_dummy_disconnect_dynamic;
}

void
ags_recall_channel_run_dummy_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_recall_channel_run_dummy_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_recall_channel_run_dummy_init(AgsRecallChannelRunDummy *recall_channel_run_dummy)
{
  AGS_RECALL(recall_channel_run_dummy)->name = "ags-recall\0";
  AGS_RECALL(recall_channel_run_dummy)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(recall_channel_run_dummy)->build_id = AGS_BUILD_ID;
  AGS_RECALL(recall_channel_run_dummy)->xml_type = "ags-recall-channel-run-dummy\0";
  AGS_RECALL(recall_channel_run_dummy)->port = NULL;

  AGS_RECALL(recall_channel_run_dummy)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(recall_channel_run_dummy)->child_type = G_TYPE_NONE;

  recall_channel_run_dummy = G_TYPE_NONE;
}

void
ags_recall_channel_run_dummy_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_recall_channel_run_dummy_parent_class)->finalize(gobject);
}

void
ags_recall_channel_run_dummy_connect(AgsConnectable *connectable)
{
  AgsRecallChannelRun *recall_channel_run;
  GObject *gobject;

  /* call parent */
  ags_recall_channel_run_dummy_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_recall_channel_run_dummy_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_recall_channel_run_dummy_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_recall_channel_run_dummy_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  /* call parent */
  ags_recall_channel_run_dummy_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_recall_channel_run_dummy_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsChannel *channel;
  AgsRecallChannelRunDummy *recall_channel_run_dummy;

  ags_recall_channel_run_dummy_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
}

AgsRecall*
ags_recall_channel_run_dummy_duplicate(AgsRecall *recall,
				       AgsRecallID *recall_id,
				       guint *n_params, GParameter *parameter)
{
  AgsRecallChannelRunDummy *recall_channel_run_dummy, *copy;
  GList *recycling_dummy;
  
  recall_channel_run_dummy = (AgsRecallChannelRunDummy *) recall;  
  copy = (AgsRecallChannelRunDummy *) AGS_RECALL_CLASS(ags_recall_channel_run_dummy_parent_class)->duplicate(recall,
													     recall_id,
													     n_params, parameter);
  AGS_RECALL(copy)->child_type = recall->child_type;
  copy->recycling_dummy_child_type = recall_channel_run_dummy->recycling_dummy_child_type;

  recycling_dummy = AGS_RECALL(copy)->children;

  while(recycling_dummy != NULL){
    AGS_RECALL(recycling_dummy->data)->child_type = recall_channel_run_dummy->recycling_dummy_child_type;

    recycling_dummy = recycling_dummy->next;
  }

  return((AgsRecall *) copy);
}

AgsRecallChannelRunDummy*
ags_recall_channel_run_dummy_new(AgsChannel *source,
				 GType child_type,
				 GType recycling_dummy_child_type)
{
  AgsRecallChannelRunDummy *recall_channel_run_dummy;

  recall_channel_run_dummy = (AgsRecallChannelRunDummy *) g_object_new(AGS_TYPE_RECALL_CHANNEL_RUN_DUMMY,
								       "source\0", source,
								       NULL);

  AGS_RECALL(recall_channel_run_dummy)->child_type = child_type;
  recall_channel_run_dummy->recycling_dummy_child_type = recycling_dummy_child_type;

  return(recall_channel_run_dummy);
}
