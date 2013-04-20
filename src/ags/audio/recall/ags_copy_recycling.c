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

#include <ags/audio/recall/ags_copy_recycling.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_run_connectable.h>

#include <ags/audio/recall/ags_copy_audio_signal.h>

#include <stdlib.h>
#include <stdio.h>

void ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling);
void ags_copy_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable);
void ags_copy_recycling_init(AgsCopyRecycling *copy_recycling);
void ags_copy_recycling_connect(AgsConnectable *connectable);
void ags_copy_recycling_disconnect(AgsConnectable *connectable);
void ags_copy_recycling_run_connect(AgsRunConnectable *run_connectable);
void ags_copy_recycling_run_disconnect(AgsRunConnectable *run_connectable);
void ags_copy_recycling_finalize(GObject *gobject);

void ags_copy_recycling_done(AgsRecall *recall);
void ags_copy_recycling_cancel(AgsRecall *recall);
void ags_copy_recycling_remove(AgsRecall *recall);
AgsRecall* ags_copy_recycling_duplicate(AgsRecall *recall,
					AgsRecallID *recall_id,
					guint *n_params, GParameter *parameter);

static gpointer ags_copy_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_copy_recycling_parent_connectable_interface;
static AgsRunConnectableInterface *ags_copy_recycling_parent_run_connectable_interface;

GType
ags_copy_recycling_get_type()
{
  static GType ags_type_copy_recycling = 0;

  if(!ags_type_copy_recycling){
    static const GTypeInfo ags_copy_recycling_info = {
      sizeof (AgsCopyRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_run_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_recycling_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_copy_recycling = g_type_register_static(AGS_TYPE_RECALL_RECYCLING,
						     "AgsCopyRecycling\0",
						     &ags_copy_recycling_info,
						     0);

    g_type_add_interface_static(ags_type_copy_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_recycling,
				AGS_TYPE_RUN_CONNECTABLE,
				&ags_run_connectable_interface_info);
  }

  return(ags_type_copy_recycling);
}

void
ags_copy_recycling_class_init(AgsCopyRecyclingClass *copy_recycling)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  AgsRecallRecyclingClass *recall_recycling;

  ags_copy_recycling_parent_class = g_type_class_peek_parent(copy_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_recycling;

  gobject->finalize = ags_copy_recycling_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_recycling;

  recall->done = ags_copy_recycling_done;
  recall->cancel = ags_copy_recycling_cancel;
  recall->remove = ags_copy_recycling_remove;

  recall->duplicate = ags_copy_recycling_duplicate;

  /* AgsRecallRecycling */
  recall_recycling = (AgsRecallRecyclingClass *) copy_recycling;
}

void
ags_copy_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_recycling_connect;
  connectable->disconnect = ags_copy_recycling_disconnect;
}

void
ags_copy_recycling_run_connectable_interface_init(AgsRunConnectableInterface *run_connectable)
{
  ags_copy_recycling_parent_run_connectable_interface = g_type_interface_peek_parent(run_connectable);

  run_connectable->connect = ags_copy_recycling_run_connect;
  run_connectable->disconnect = ags_copy_recycling_run_disconnect;
}

void
ags_copy_recycling_init(AgsCopyRecycling *copy_recycling)
{
  AGS_RECALL(copy_recycling)->child_type = AGS_TYPE_COPY_AUDIO_SIGNAL;
  AGS_RECALL_RECYCLING(copy_recycling)->flags |= AGS_RECALL_RECYCLING_MAP_CHILD_DESTINATION;
}

void
ags_copy_recycling_finalize(GObject *gobject)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = AGS_COPY_RECYCLING(gobject);

  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_copy_recycling_parent_class)->finalize(gobject);
}

void
ags_copy_recycling_connect(AgsConnectable *connectable)
{
  AgsCopyRecycling *copy_recycling;

  ags_copy_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_copy_recycling_disconnect(AgsConnectable *connectable)
{
  AgsCopyRecycling *copy_recycling;

  ags_copy_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_copy_recycling_run_connect(AgsRunConnectable *run_connectable)
{
  AgsCopyRecycling *copy_recycling;
  GObject *gobject;

  ags_copy_recycling_parent_run_connectable_interface->connect(run_connectable);
}

void
ags_copy_recycling_run_disconnect(AgsRunConnectable *run_connectable)
{
  AgsCopyRecycling *copy_recycling;
  GObject *gobject;

  ags_copy_recycling_parent_run_connectable_interface->connect(run_connectable);

  /* empty */
}

void 
ags_copy_recycling_done(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->done(recall);

  /* empty */
}

void
ags_copy_recycling_cancel(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->cancel(recall);

  /* empty */
}

void 
ags_copy_recycling_remove(AgsRecall *recall)
{
  AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->remove(recall);

  /* empty */
}

AgsRecall*
ags_copy_recycling_duplicate(AgsRecall *recall,
			     AgsRecallID *recall_id,
			     guint *n_params, GParameter *parameter)
{
  AgsCopyRecycling *copy_recycling, *copy;

  copy_recycling = (AgsCopyRecycling *) recall;
  copy = (AgsCopyRecycling *) AGS_RECALL_CLASS(ags_copy_recycling_parent_class)->duplicate(recall,
											   recall_id,
											   n_params, parameter);

  return((AgsRecall *) copy);
}

AgsCopyRecycling*
ags_copy_recycling_new(AgsRecycling *destination,
		       AgsRecycling *source,
		       AgsDevout *devout)
{
  AgsCopyRecycling *copy_recycling;

  copy_recycling = (AgsCopyRecycling *) g_object_new(AGS_TYPE_COPY_RECYCLING,
						     "devout\0", devout,
						     "destination\0", destination,
						     "source\0", source,
						     NULL);

  return(copy_recycling);
}
