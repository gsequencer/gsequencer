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

#include <ags/audio/recall/ags_copy_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>

#include <ags/audio/task/ags_cancel_recall.h>

#include <ags/audio/recall/ags_copy_recycling.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel);
void ags_copy_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_channel_init(AgsCopyChannel *copy_channel);
void ags_copy_channel_connect(AgsConnectable *connectable);
void ags_copy_channel_disconnect(AgsConnectable *connectable);
void ags_copy_channel_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_channel_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_channel_finalize(GObject *gobject);

void ags_copy_channel_run_init_pre(AgsRecall *recall);
void ags_copy_channel_done(AgsRecall *recall);
void ags_copy_channel_remove(AgsRecall *recall);
void ags_copy_channel_cancel(AgsRecall *recall);
AgsRecall* ags_copy_channel_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint *n_params, GParameter *parameter);

static gpointer ags_copy_channel_parent_class = NULL;
static AgsConnectableInterface *ags_copy_channel_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_channel_parent_run_connectable_interface;

GType
ags_copy_channel_get_type()
{
  static GType ags_type_copy_channel = 0;

  if(!ags_type_copy_channel){
    static const GTypeInfo ags_copy_channel_info = {
      sizeof (AgsCopyChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
						   "AgsCopyChannel\0",
						   &ags_copy_channel_info,
						   0);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_channel);
}

void
ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel;

  gobject->finalize = ags_copy_channel_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_channel;

  recall->run_init_pre = ags_copy_channel_run_init_pre;
  recall->done = ags_copy_channel_done;
  recall->remove = ags_copy_channel_remove;
  recall->cancel = ags_copy_channel_cancel;
  recall->duplicate = ags_copy_channel_duplicate;
}

void
ags_copy_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_channel_connect;
  connectable->disconnect = ags_copy_channel_disconnect;
}

void
ags_copy_channel_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_channel_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_channel_run_connect;
  run_connectable->disconnect = ags_copy_channel_run_disconnect;
}

void
ags_copy_channel_init(AgsCopyChannel *copy_channel)
{
  AGS_RECALL(copy_channel)->child_type = AGS_TYPE_COPY_RECYCLING;
}

void
ags_copy_channel_connect(AgsConnectable *connectable)
{
  ags_copy_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_copy_channel_disconnect(AgsConnectable *connectable)
{
  ags_copy_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_copy_channel_run_connect(AgsRunConnectable *run_connectable)
{
  ags_copy_channel_parent_run_connectable_interface->connect(run_connectable);

  /* empty */
}

void
ags_copy_channel_run_disconnect(AgsRunConnectable *run_connectable)
{
  ags_copy_channel_parent_run_connectable_interface->disconnect(run_connectable);

  /* empty */
}

void
ags_copy_channel_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

void
ags_copy_channel_run_init_pre(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->run_init_pre(recall);

  /* empty */
}

void
ags_copy_channel_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->done(recall);
  
  /* empty */
}

void
ags_copy_channel_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->cancel(recall);
  
  /* empty */
}

void 
ags_copy_channel_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_channel_parent_class)->remove(recall);
  
  /* empty */
}

AgsRecall*
ags_copy_channel_duplicate(AgsRecall *recall,
			   AgsRecallID *recall_id,
			   guint *n_params, GParameter *parameter)
{
  AgsCopyChannel *copy_channel, *copy;

  copy_channel = (AgsCopyChannel *) recall;
  copy = (AgsCopyChannel *) AGS_RECALL_CLASS(ags_copy_channel_parent_class)->duplicate(recall,
										       recall_id,
										       n_params, parameter);

  g_object_set(G_OBJECT(copy),
	       "devout\0", copy_channel->devout,
	       "destination\0", copy_channel->destination,
	       "source\0", copy_channel->source,
	       NULL);

  return((AgsRecall *) copy);
}

AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source,
		     AgsDevout *devout)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						 "destination\0", destination,
						 "source\0", source,
						 "devout\0", devout,
						 NULL);

  return(copy_channel);
}
