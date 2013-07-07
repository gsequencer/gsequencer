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

#ifndef __AGS_RECALL_ID_H__
#define __AGS_RECALL_ID_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_RECALL_ID                (ags_recall_id_get_type ())
#define AGS_RECALL_ID(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_ID, AgsRecallID))
#define AGS_RECALL_ID_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_ID, AgsRecallIDClass))
#define AGS_IS_RECALL_ID(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_ID))
#define AGS_IS_RECALL_ID_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_ID))
#define AGS_RECALL_ID_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_ID, AgsRecallIDClass))
#define AGS_POINTER_TO_GROUP_ID(ptr)      ((AgsGroupId)(gulong)(ptr))
#define AGS_GROUP_ID_TO_POINTER(id)       ((gpointer)(AgsGroupId)(id))

#define AGS_GROUP_ID_NOT_EXISTIBLE 0

typedef struct _AgsRecallID AgsRecallID;
typedef struct _AgsRecallIDClass AgsRecallIDClass;
typedef gulong AgsGroupId;

typedef enum{
  AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE     = 1,
  AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE   = 1 << 1,
  AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE    = 1 << 2,
  AGS_RECALL_ID_CANCELED                    = 1 << 3,
  AGS_RECALL_ID_HIGHER_LEVEL_IS_RECALL      = 1 << 4,
  AGS_RECALL_ID_AUDIO_RESOLVED              = 1 << 5,
}AgsRecallIDFlags;

struct _AgsRecallID
{
  GObject object;

  guint flags;

  AgsGroupId parent_group_id;
  AgsGroupId group_id;
  AgsGroupId child_group_id;

  AgsRecycling *first_recycling; // identify the channel in AgsAudio
  AgsRecycling *last_recycling; // identify the channel in AgsAudio
};

struct _AgsRecallIDClass
{
  GObjectClass object;
};

GType ags_recall_id_get_type(void);

AgsGroupId ags_recall_id_generate_group_id();

gboolean ags_recall_id_get_run_stage(AgsRecallID *id, gint stage);
void ags_recall_id_set_run_stage(AgsRecallID *recall_id, gint stage);
void ags_recall_id_unset_run_stage(AgsRecallID *recall_id, gint stage);

GList* ags_recall_id_add(GList *recall_id_list,
			 AgsGroupId parent_group_id, AgsGroupId group_id, AgsGroupId child_group_id,
			 AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			 gboolean higher_level_is_recall);
AgsRecallID* ags_recall_id_find_group_id(GList *recall_id_list,
					 AgsGroupId group_id);
AgsRecallID* ags_recall_id_find_group_id_with_recycling(GList *recall_id_list,
							AgsGroupId group_id,
							AgsRecycling *first_recycling, AgsRecycling *last_recycling);
AgsRecallID* ags_recall_id_find_parent_group_id(GList *recall_id_list,
						AgsGroupId parent_group_id);

void ags_recall_id_reset_recycling(GList *recall_ids,
				   AgsRecycling *old_first_recycling,
				   AgsRecycling *first_recycling, AgsRecycling *last_recycling);

AgsRecallID* ags_recall_id_new();

#endif /*__AGS_RECALL_ID_H__*/
