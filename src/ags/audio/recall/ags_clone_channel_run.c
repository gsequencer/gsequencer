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

#include <ags/audio/recall/ags_clone_channel_run.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_dynamic_connectable.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_clone_channel.h>

#include <stdlib.h>
#include <stdio.h>

void ags_clone_channel_run_class_init(AgsCloneChannelRunClass *clone_channel_run);
void ags_clone_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_clone_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_clone_channel_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_clone_channel_run_init(AgsCloneChannelRun *clone_channel_run);
void ags_clone_channel_run_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_clone_channel_run_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_clone_channel_run_connect(AgsConnectable *connectable);
void ags_clone_channel_run_disconnect(AgsConnectable *connectable);
void ags_clone_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_clone_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable);
void ags_clone_channel_run_finalize(GObject *gobject);

void ags_clone_channel_run_run_init_pre(AgsRecall *recall);
void ags_clone_channel_run_done(AgsRecall *recall);
void ags_clone_channel_run_remove(AgsRecall *recall);
void ags_clone_channel_run_cancel(AgsRecall *recall);
AgsRecall* ags_clone_channel_run_duplicate(AgsRecall *recall,
					   AgsRecallID *recall_id,
					   guint *n_params, GParameter *parameter);

enum{
  PROP_0,
};

static gpointer ags_clone_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_clone_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_clone_channel_run_parent_dynamic_connectable_interface;
static AgsPluginInterface *ags_clone_channel_run_parent_plugin_interface;

GType
ags_clone_channel_run_get_type()
{
  static GType ags_type_clone_channel_run = 0;

  if(!ags_type_clone_channel_run){
    static const GTypeInfo ags_clone_channel_run_info = {
      sizeof (AgsCloneChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_clone_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCloneChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_clone_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_clone_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_clone_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_clone_channel_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_clone_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							"AgsCloneChannelRun\0",
							&ags_clone_channel_run_info,
							0);

    g_type_add_interface_static(ags_type_clone_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_clone_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);

    g_type_add_interface_static(ags_type_clone_channel_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_clone_channel_run);
}

void
ags_clone_channel_run_class_init(AgsCloneChannelRunClass *clone_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_clone_channel_run_parent_class = g_type_class_peek_parent(clone_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) clone_channel_run;

  gobject->set_property = ags_clone_channel_run_set_property;
  gobject->get_property = ags_clone_channel_run_get_property;

  gobject->finalize = ags_clone_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) clone_channel_run;

  recall->run_init_pre = ags_clone_channel_run_run_init_pre;
  recall->done = ags_clone_channel_run_done;
  recall->cancel = ags_clone_channel_run_cancel;
  recall->duplicate = ags_clone_channel_run_duplicate;
}

void
ags_clone_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_clone_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_clone_channel_run_connect;
  connectable->disconnect = ags_clone_channel_run_disconnect;
}

void
ags_clone_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_clone_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect_dynamic = ags_clone_channel_run_connect_dynamic;
  dynamic_connectable->disconnect_dynamic = ags_clone_channel_run_disconnect_dynamic;
}

void
ags_clone_channel_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_clone_channel_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_clone_channel_run_init(AgsCloneChannelRun *clone_channel_run)
{
  AGS_RECALL(clone_channel_run)->flags |= AGS_RECALL_INPUT_ORIENTATED;
  AGS_RECALL(clone_channel_run)->child_type = G_TYPE_NONE;
}

void
ags_clone_channel_run_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsCloneChannelRun *clone_channel_run;

  clone_channel_run = AGS_CLONE_CHANNEL_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clone_channel_run_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsCloneChannelRun *clone_channel_run;

  clone_channel_run = AGS_CLONE_CHANNEL_RUN(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clone_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_clone_channel_run_parent_class)->finalize(gobject);
}

void
ags_clone_channel_run_connect(AgsConnectable *connectable)
{
  AgsCloneChannelRun *clone_channel_run;

  ags_clone_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_clone_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_clone_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_clone_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCloneChannel *clone_channel;
  AgsCloneChannelRun *clone_channel_run;
  GObject *gobject;

  ags_clone_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* AgsCloneChannelRun */
  clone_channel_run = AGS_CLONE_CHANNEL_RUN(dynamic_connectable);
}

void
ags_clone_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  AgsCloneChannel *clone_channel;
  AgsCloneChannelRun *clone_channel_run;
  GObject *gobject;

  ags_clone_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* AgsCloneChannelRun */
  clone_channel_run = AGS_CLONE_CHANNEL_RUN(dynamic_connectable);

  /* AgsCloneChannel */
  clone_channel = AGS_CLONE_CHANNEL(AGS_RECALL_CHANNEL_RUN(clone_channel_run)->recall_channel);
}

void
ags_clone_channel_run_run_init_pre(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_clone_channel_run_parent_class)->run_init_pre(recall);

  /* empty */
}

void
ags_clone_channel_run_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_clone_channel_run_parent_class)->done(recall);

  /* empty */
}

void 
ags_clone_channel_run_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_clone_channel_run_parent_class)->cancel(recall);

  /* empty */
}

AgsRecall*
ags_clone_channel_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, GParameter *parameter)
{
  AgsCloneChannelRun *clone_channel_run, *copy;
  
  clone_channel_run = (AgsCloneChannelRun *) recall;
  copy = (AgsCloneChannelRun *) AGS_RECALL_CLASS(ags_clone_channel_run_parent_class)->duplicate(recall,
												recall_id,
												n_params, parameter);

  return((AgsRecall *) copy);
}

AgsCloneChannelRun*
ags_clone_channel_run_new()
{
  AgsCloneChannelRun *clone_channel_run;

  clone_channel_run = (AgsCloneChannelRun *) g_object_new(AGS_TYPE_CLONE_CHANNEL_RUN,
							  NULL);
  
  return(clone_channel_run);
}
