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

#include <ags/audio/ags_recall_id.h>

#include <ags/object/ags_connectable.h>

#include <stdio.h>

void ags_recall_id_class_init(AgsRecallIDClass *recall_id);
void ags_recall_id_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_recall_id_init(AgsRecallID *recall_id);
void ags_recall_id_connect(AgsConnectable *connectable);
void ags_recall_id_disconnect(AgsConnectable *connectable);
void ags_recall_id_finalize(GObject *gobject);

static gpointer ags_recall_id_parent_class = NULL;

static guint ags_recall_id_counter = 1;

GType
ags_recall_id_get_type(void)
{
  static GType ags_type_recall_id = 0;

  if(!ags_type_recall_id){
    static const GTypeInfo ags_recall_id_info = {
      sizeof (AgsRecallIDClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_recall_id_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsRecallID),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_recall_id_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_recall_id_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_recall_id = g_type_register_static(G_TYPE_OBJECT,
						"AgsRecallID\0",
						&ags_recall_id_info,
						0);

    g_type_add_interface_static(ags_type_recall_id,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_recall_id);
}

void
ags_recall_id_class_init(AgsRecallIDClass *recall_id)
{
  GObjectClass *gobject;

  ags_recall_id_parent_class = g_type_class_peek_parent(recall_id);

  gobject = (GObjectClass *) recall_id;
  gobject->finalize = ags_recall_id_finalize;
}

void
ags_recall_id_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_recall_id_connect;
  connectable->disconnect = ags_recall_id_disconnect;
}

void
ags_recall_id_init(AgsRecallID *recall_id)
{
  recall_id->flags = 0;

  recall_id->parent_group_id = 0;
  recall_id->group_id = 0;
  recall_id->child_group_id = 0;

  //  recall_id->first_recycling = NULL;
  //  recall_id->last_recycling = NULL;
}

void
ags_recall_id_connect(AgsConnectable *connectable)
{
}

void
ags_recall_id_disconnect(AgsConnectable *connectable)
{
}

void
ags_recall_id_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_recall_id_parent_class)->finalize(gobject);
}

AgsGroupId
ags_recall_id_generate_group_id()
{
  AgsGroupId group_id;

  group_id = ags_recall_id_counter;
  ags_recall_id_counter++;

  if(ags_recall_id_counter == G_MAXULONG)
    fprintf(stderr, "WARNING: ags_recall_id_generate_group_id - counter expired\n\0");

  return(group_id);
}

/*
 * output - the AgsChannel to check
 * stage - the current stage to check against
 * returns TRUE if the stage isn't run yet
 */
gboolean
ags_recall_id_get_run_stage(AgsRecallID *id, gint stage)
{
  switch(stage){
  case 0:
    if((AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE & (id->flags)) == 0)
      return(TRUE);

    break;
  case 1:
    if((AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE & (id->flags)) == 0)
      return(TRUE);

    break;
  case 2:
    if((AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE & (id->flags)) == 0)
      return(TRUE);
  }

  return(FALSE);
}

void
ags_recall_id_set_run_stage(AgsRecallID *recall_id, gint stage)
{
  if(stage == 0)
    recall_id->flags |= AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE;
  else if(stage == 1)
    recall_id->flags |= AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE;
  else
    recall_id->flags |= AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE;
}

void
ags_recall_id_unset_run_stage(AgsRecallID *recall_id, gint stage)
{
  if(stage == 0)
    recall_id->flags &= (~AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE);
  else if(stage == 1)
    recall_id->flags &= (~AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE);
  else
    recall_id->flags &= (~AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE);
}

GList*
ags_recall_id_add(GList *recall_id_list,
		  AgsGroupId parent_group_id, AgsGroupId group_id, AgsGroupId child_group_id,
		  AgsRecycling *first_recycling, AgsRecycling *last_recycling,
		  gboolean higher_level_is_recall)
{
  AgsRecallID *recall_id;
  GList *list;

  recall_id = ags_recall_id_new();

  if(higher_level_is_recall)
    recall_id->flags |= AGS_RECALL_ID_HIGHER_LEVEL_IS_RECALL;

  recall_id->parent_group_id = parent_group_id;
  recall_id->group_id = group_id;
  recall_id->child_group_id = child_group_id;

  //  recall_id->first_recycling = first_recycling;
  //  recall_id->last_recycling = last_recycling;

  list = g_list_prepend(recall_id_list,
			(gpointer) recall_id);

  return(list);
}

AgsRecallID*
ags_recall_id_find_group_id(GList *recall_id_list, AgsGroupId group_id)
{
  while(recall_id_list != NULL){
    if(AGS_RECALL_ID(recall_id_list->data)->group_id == group_id)
      return((AgsRecallID *) recall_id_list->data);

    recall_id_list = recall_id_list->next;
  }

  return(NULL);
}

AgsRecallID*
ags_recall_id_find_parent_group_id(GList *recall_id_list, AgsGroupId parent_group_id)
{
  while(recall_id_list != NULL){
    if(AGS_RECALL_ID(recall_id_list->data)->parent_group_id == parent_group_id)
      return((AgsRecallID *) recall_id_list->data);

    recall_id_list = recall_id_list->next;
  }

  return(NULL);
}

AgsRecallID*
ags_recall_id_new()
{
  AgsRecallID *recall_id;

  recall_id = (AgsRecallID *) g_object_new(AGS_TYPE_RECALL_ID, NULL);

  return(recall_id);
}
