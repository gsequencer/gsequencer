/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_copy_channel_run.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/recall/ags_copy_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_channel_run_class_init(AgsCopyChannelRunClass *copy_channel_run);
void ags_copy_channel_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable);
void ags_copy_channel_run_init(AgsCopyChannelRun *copy_channel_run);
void ags_copy_channel_run_connect(AgsConnectable *connectable);
void ags_copy_channel_run_disconnect(AgsConnectable *connectable);
void ags_copy_channel_run_dynamic_connect(AgsDynamicConnectable *dynamic_connectable);
void ags_copy_channel_run_dynamic_disconnect(AgsDynamicConnectable *dynamic_connectable);
void ags_copy_channel_run_finalize(GObject *gobject);

AgsRecall* ags_copy_channel_run_duplicate(AgsRecall *recall,
					  AgsRecallID *recall_id,
					  guint *n_params, GParameter *parameter);

static gpointer ags_copy_channel_run_parent_class = NULL;
static AgsConnectableInterface *ags_copy_channel_run_parent_connectable_interface;
static AgsDynamicConnectableInterface *ags_copy_channel_run_parent_dynamic_connectable_interface;

GType
ags_copy_channel_run_get_type()
{
  static GType ags_type_copy_channel_run = 0;

  if(!ags_type_copy_channel_run){
    static const GTypeInfo ags_copy_channel_run_info = {
      sizeof (AgsCopyChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_run_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_dynamic_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_run_dynamic_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						       "AgsCopyChannelRun\0",
						       &ags_copy_channel_run_info,
						       0);

    g_type_add_interface_static(ags_type_copy_channel_run,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_channel_run,
				AGS_TYPE_DYNAMIC_CONNECTABLE,
				&ags_dynamic_connectable_interface_info);
  }

  return(ags_type_copy_channel_run);
}

void
ags_copy_channel_run_class_init(AgsCopyChannelRunClass *copy_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_channel_run_parent_class = g_type_class_peek_parent(copy_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel_run;

  gobject->finalize = ags_copy_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_channel_run;
}

void
ags_copy_channel_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_channel_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_channel_run_connect;
  connectable->disconnect = ags_copy_channel_run_disconnect;
}

void
ags_copy_channel_run_dynamic_connectable_interface_init(AgsDynamicConnectableInterface *dynamic_connectable)
{
  ags_copy_channel_run_parent_dynamic_connectable_interface = g_type_interface_peek_parent(dynamic_connectable);

  dynamic_connectable->connect = ags_copy_channel_run_connect_dynamic;
  dynamic_connectable->disconnect = ags_copy_channel_run_disconnect_dynamic;
}

void
ags_copy_channel_run_init(AgsCopyChannelRun *copy_channel_run)
{
  AGS_RECALL(copy_channel_run)->flags |= (AGS_RECALL_OUTPUT_ORIENTATED |
					  AGS_RECALL_INPUT_ORIENTATED);
  AGS_RECALL(copy_channel_run)->child_type = AGS_TYPE_COPY_RECYCLING;
}

void
ags_copy_channel_run_connect(AgsConnectable *connectable)
{
  ags_copy_channel_run_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_copy_channel_run_disconnect(AgsConnectable *connectable)
{
  ags_copy_channel_run_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_copy_channel_run_connect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_copy_channel_run_parent_dynamic_connectable_interface->connect_dynamic(dynamic_connectable);

  /* empty */
}

void
ags_copy_channel_run_disconnect_dynamic(AgsDynamicConnectable *dynamic_connectable)
{
  ags_copy_channel_run_parent_dynamic_connectable_interface->disconnect_dynamic(dynamic_connectable);

  /* empty */
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
			       guint *n_params, GParameter *parameter)
{
  AgsCopyChannelRun *copy;

  copy = (AgsCopyChannelRun *) AGS_RECALL_CLASS(ags_copy_channel_run_parent_class)->duplicate(recall,
											      recall_id,
											      n_params, parameter);
  
  return((AgsRecall *) copy);
}

AgsCopyChannelRun*
ags_copy_channel_run_new(AgsChannel *destination,
			 AgsChannel *source,
			 AgsDevout *devout)
{
  AgsCopyChannelRun *copy_channel_run;

  copy_channel_run = (AgsCopyChannelRun *) g_object_new(AGS_TYPE_COPY_CHANNEL_RUN,
							"destination\0", destination,
							"source\0", source,
							"devout\0", devout,
							NULL);

  return(copy_channel_run);
}
