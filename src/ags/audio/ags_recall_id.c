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

#include <stdlib.h>
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

  recall_id->first_recycling = NULL;
  recall_id->last_recycling = NULL;
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
  g_warning("ags_recall_id_finalize\0");

  G_OBJECT_CLASS(ags_recall_id_parent_class)->finalize(gobject);
}

/**
 * ags_recall_id_generate_group_id:
 * 
 * Generate a new AgsGroupId
 */
AgsGroupId
ags_recall_id_generate_group_id()
{
  AgsGroupId group_id;

  group_id = ags_recall_id_counter;
  ags_recall_id_counter++;

  if(ags_recall_id_counter == G_MAXULONG)
    g_warning("WARNING: ags_recall_id_generate_group_id - counter expired\n\0");

  return(group_id);
}

/**
 * ags_recall_id_get_run_stage:
 * @id the #AgsRecallID to check
 * @stage the current run stage to check against
 * Returns: TRUE if the stage isn't run yet otherwise FALSE
 *
 * Check if a run stage already has been passed for current run. This
 * function is intended to handle AGS_AUDIO_ASYNC correctly.
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

/**
 * ags_recall_id_set_run_stage:
 * @recall_id the #AgsRecallID which has been passed
 * @stage the run stage the networked channels are in
 *
 * Marks the run stage to be passed for audio channel.
 */
void
ags_recall_id_set_run_stage(AgsRecallID *recall_id, gint stage)
{
  guint i;

  if(stage == 0){
    recall_id->flags |= AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE;
  }else if(stage == 1){
    recall_id->flags |= AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE;
  }else{
    recall_id->flags |= AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE;
  }
}

/**
 * ags_recall_id_unset_run_stage:
 * @recall_id the #AgsRecallID which has been passed
 * @stage the run stage the networked channels are in
 *
 * Unmarks the run stage to be passed for audio channel.
 */
void
ags_recall_id_unset_run_stage(AgsRecallID *recall_id, gint stage)
{
  if(stage == 0){
    recall_id->flags &= (~AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE);
  }else if(stage == 1){
    recall_id->flags &= (~AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE);
  }else{
    recall_id->flags &= (~AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE);
  }
}

/**
 * ags_recall_id_add:
 * @recall_id_list the #GList the new #AgsRecall should be added
 * @parent_group_id the parent #AgsGroupId
 * @group_id the #AgsGroupId
 * @child_group_id the child #AgsGroupId
 * @first_recycling the first #AgsRecycling this #AgsRecallID belongs to
 * @last_recycling the last #AgsRecycling this #AgsRecallID belongs to
 * @higher_level_is_recall set to TRUE if above the next #AgsRecycling is still recall
 * and not play
 * Returns: the newly allocated #GList which is the new start of the #GList, too.
 *
 * Adds an #AgsRecallID with given properties to the passed #GList.
 */
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

  recall_id->first_recycling = first_recycling;
  recall_id->last_recycling = last_recycling;

  list = g_list_prepend(recall_id_list,
			(gpointer) recall_id);

  return(list);
}

/**
 * ags_recall_id_find_group_id:
 * @recall_id_list the #GList to search within
 * @group_id the #AgsGroupId to search for
 * Returns: the #AgsRecallID containing @group_id if found otherwise %NULL
 * 
 * Find the first occurence of @group_id within a #GList of #AgsRecallID.
 */
AgsRecallID*
ags_recall_id_find_group_id(GList *recall_id_list, AgsGroupId group_id)
{
  while(recall_id_list != NULL){
    if(AGS_RECALL_ID(recall_id_list->data)->group_id == group_id){
      return((AgsRecallID *) recall_id_list->data);
    }

    recall_id_list = recall_id_list->next;
  }

  return(NULL);
}

/**
 * ags_recall_id_find_group_id_with_recycling:
 * @recall_id_list the #GList to search within
 * @group_id the #AgsGroupId to search for
 * @first_recycling the first #AgsRecycling
 * @last_recycling the last #AgsRecycling
 * Returns: the #AgsRecallID containing @group_id, @first_recycling and @last_recycling
 * if found otherwise %NULL
 *
 * Find the unique occurence of @group_id, @first_recycling and @last_recycling within a
 * #GList of #AgsRecallID.
 */
AgsRecallID*
ags_recall_id_find_group_id_with_recycling(GList *recall_id_list,
					   AgsGroupId group_id,
					   AgsRecycling *first_recycling, AgsRecycling *last_recycling)
{
  while(recall_id_list != NULL){
    if(AGS_RECALL_ID(recall_id_list->data)->group_id == group_id &&
       AGS_RECALL_ID(recall_id_list->data)->first_recycling == first_recycling &&
       AGS_RECALL_ID(recall_id_list->data)->last_recycling == last_recycling){
      return((AgsRecallID *) recall_id_list->data);
    }

    recall_id_list = recall_id_list->next;
  }

  printf("ags_recall_id_find_group_id_with_recycling: couldn't find matching recall id\n\0");

  return(NULL);
}

/**
 * ags_recall_id_find_parent_group_id:
 * @recall_id_list the #GList to search within
 * @parent_group_id the #AgsGroupId to search for
 *
 * Find the first occurence of @parent_group_id within a #GList of #AgsRecallID.
 */
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

/**
 * ags_recall_id_reset_recycling:
 * @recall_ids
 * @old_first_recycling
 * @first_recycling
 * @last_recycling
 *
 *
 */
void
ags_recall_id_reset_recycling(GList *recall_ids,
			      AgsRecycling *old_first_recycling,
			      AgsRecycling *first_recycling, AgsRecycling *last_recycling)
{
  AgsRecallID *recall_id;

  while(recall_ids != NULL){
    recall_id = AGS_RECALL_ID(recall_ids->data);
    
    if(recall_id->first_recycling == old_first_recycling){
      recall_id->first_recycling = first_recycling;
      recall_id->last_recycling = last_recycling;
    }

    recall_ids = recall_ids->next;
  }
}

AgsRecallID*
ags_recall_id_new()
{
  AgsRecallID *recall_id;

  recall_id = (AgsRecallID *) g_object_new(AGS_TYPE_RECALL_ID, NULL);

  return(recall_id);
}
