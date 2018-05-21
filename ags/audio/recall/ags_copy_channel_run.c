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

#include <ags/audio/recall/ags_copy_channel_run.h>
#include <ags/audio/recall/ags_copy_recycling.h>

#include <ags/libags.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_channel_run_class_init(AgsCopyChannelRunClass *copy_channel_run);
void ags_copy_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_channel_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_copy_channel_run_init(AgsCopyChannelRun *copy_channel_run);
void ags_copy_channel_run_finalize(GObject *gobject);

AgsRecall* ags_copy_channel_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_copy_channel_run
 * @short_description: copy channel
 * @title: AgsCopyChannelRun
 * @section_id:
 * @include: ags/audio/recall/ags_copy_channel_run.h
 *
 * The #AgsCopyChannelRun class copies the channel.
 */

static gpointer ags_copy_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_copy_channel_run_parent_connectable_interface;
static AgsPluginInterface *ags_copy_channel_run_parent_plugin_interface;

GType
ags_copy_channel_run_get_type()
{
  static GType ags_type_copy_channel_run = 0;

  if(!ags_type_copy_channel_run){
    static const GTypeInfo ags_copy_channel_run_info = {
      sizeof(AgsCopyChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCopyChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsCopyChannelRun",
						       &ags_copy_channel_run_info,
						       0);

    g_type_add_interface_static(ags_type_copy_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_channel_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_copy_channel_run);
}

void
ags_copy_channel_run_class_init(AgsCopyChannelRunClass *copy_channel_run)
{
  GObjectClass *gobject;

  ags_copy_channel_run_parent_class = g_type_class_peek_parent(copy_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel_run;

  gobject->finalize = ags_copy_channel_run_finalize;
}

void
ags_copy_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_copy_channel_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_copy_channel_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_copy_channel_run_init(AgsCopyChannelRun *copy_channel_run)
{
  AGS_RECALL(copy_channel_run)->name = "ags-copy";
  AGS_RECALL(copy_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_channel_run)->xml_type = "ags-copy-channel-run";
  AGS_RECALL(copy_channel_run)->port = NULL;

  AGS_RECALL(copy_channel_run)->flags |= (AGS_RECALL_OUTPUT_ORIENTATED |
					  AGS_RECALL_INPUT_ORIENTATED);
  AGS_RECALL(copy_channel_run)->child_type = AGS_TYPE_COPY_RECYCLING;
}

void
ags_copy_channel_run_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_run_parent_class)->finalize(gobject);
}

AgsRecall*
ags_copy_channel_run_duplicate(AgsRecall *recall,
			       AgsRecallID *recall_id,
			       guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsCopyChannelRun *copy_copy_chhannel_run;

  copy_copy_chhannel_run = (AgsCopyChannelRun *) AGS_RECALL_CLASS(ags_copy_channel_run_parent_class)->duplicate(recall,
														recall_id,
														n_params, parameter_name, value);
  
  return((AgsRecall *) copy_copy_chhannel_run);
}

/**
 * ags_copy_channel_run_new:
 * @destination: the destination #AgsChannel
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsCopyChannelRun
 *
 * Returns: the new #AgsCopyChannelRun
 *
 * Since: 2.0.0
 */
AgsCopyChannelRun*
ags_copy_channel_run_new(AgsChannel *destination,
			 AgsChannel *source)
{
  AgsCopyChannelRun *copy_channel_run;

  copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
							"destination", destination,
							"source", source,
							NULL);

  return(copy_channel_run);
}
