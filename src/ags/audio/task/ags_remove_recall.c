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

#include <ags/audio/task/ags_remove_recall.h>

#include <ags/object/ags_connectable.h>

#include <math.h>

void ags_remove_recall_class_init(AgsRemoveRecallClass *remove_recall);
void ags_remove_recall_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_remove_recall_init(AgsRemoveRecall *remove_recall);
void ags_remove_recall_connect(AgsConnectable *connectable);
void ags_remove_recall_disconnect(AgsConnectable *connectable);
void ags_remove_recall_finalize(GObject *gobject);

void ags_remove_recall_launch(AgsTask *task);

static gpointer ags_remove_recall_parent_class = NULL;
static AgsConnectableInterface *ags_remove_recall_parent_connectable_interface;

GType
ags_remove_recall_get_type()
{
  static GType ags_type_remove_recall = 0;

  if(!ags_type_remove_recall){
    static const GTypeInfo ags_remove_recall_info = {
      sizeof (AgsRemoveRecallClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_remove_recall_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRemoveRecall),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_remove_recall_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_remove_recall_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_remove_recall = g_type_register_static(AGS_TYPE_TASK,
						  "AgsRemoveRecall\0",
						  &ags_remove_recall_info,
						  0);

    g_type_add_interface_static(ags_type_remove_recall,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_remove_recall);
}

void
ags_remove_recall_class_init(AgsRemoveRecallClass *remove_recall)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_remove_recall_parent_class = g_type_class_peek_parent(remove_recall);

  /* GObjectClass */
  gobject = (GObjectClass *) remove_recall;

  gobject->finalize = ags_remove_recall_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) remove_recall;

  task->launch = ags_remove_recall_launch;
}

void
ags_remove_recall_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_remove_recall_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_remove_recall_connect;
  connectable->disconnect = ags_remove_recall_disconnect;
}

void
ags_remove_recall_init(AgsRemoveRecall *remove_recall)
{
  remove_recall->recall = NULL;
}

void
ags_remove_recall_connect(AgsConnectable *connectable)
{
  ags_remove_recall_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_remove_recall_disconnect(AgsConnectable *connectable)
{
  ags_remove_recall_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_remove_recall_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_remove_recall_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_remove_recall_launch(AgsTask *task)
{
  AgsRemoveRecall *remove_recall;

  remove_recall = AGS_REMOVE_RECALL(task);

  ags_recall_remove(remove_recall->recall);
}

AgsRemoveRecall*
ags_remove_recall_new(AgsRecall *recall)
{
  AgsRemoveRecall *remove_recall;

  remove_recall = (AgsRemoveRecall *) g_object_new(AGS_TYPE_REMOVE_RECALL,
						   NULL);
  

  remove_recall->recall = recall;

  return(remove_recall);
}
