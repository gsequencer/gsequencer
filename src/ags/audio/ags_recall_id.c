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

#include <ags-lib/object/ags_connectable.h>

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

  recall_id->recycling = NULL;
  recall_id->recycling_container = NULL;
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
 * @recall_id the #AgsRecallID to add
 * Returns: the newly allocated #GList which is the new start of the #GList, too.
 *
 * Adds an #AgsRecallID with given properties to the passed #GList.
 */
GList*
ags_recall_id_add(GList *recall_id_list,
		  AgsRecallID *recall_id)
{
  GList *list;

  list = g_list_prepend(recall_id_list,
			(gpointer) recall_id);

  return(list);
}

AgsRecallID*
ags_recall_id_find_recycling_container(GList *recall_id_list,
				       AgsRecyclingContainer *recycling_container)
{
  AgsRecallID *recall_id;

  while(recall_id_list != NULL){
    recall_id = AGS_RECALL_ID(recall_id_list->data);

    if(recall_id->recycling_container == recycling_container){
      return(recall_id);
    }

    recall_id_list = recall_id_list->next;
  }

  return(recall_id);
}

AgsRecallID*
ags_recall_id_new(AgsRecycling *recycling)
{
  AgsRecallID *recall_id;

  recall_id = (AgsRecallID *) g_object_new(AGS_TYPE_RECALL_ID, NULL);

  return(recall_id);
}
