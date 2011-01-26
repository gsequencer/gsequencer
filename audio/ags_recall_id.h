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

typedef struct _AgsRecallID AgsRecallID;
typedef struct _AgsRecallIDClass AgsRecallIDClass;

typedef enum{
  AGS_RECALL_ID_RUN_PRE_SYNC_ASYNC_DONE     = 1,
  AGS_RECALL_ID_RUN_INTER_SYNC_ASYNC_DONE   = 1 << 1,
  AGS_RECALL_ID_RUN_POST_SYNC_ASYNC_DONE    = 1 << 2,
  AGS_RECALL_ID_CANCELED                    = 1 << 3,
  AGS_RECALL_ID_HIGHER_LEVEL_IS_RECALL      = 1 << 4,
}AgsRecallIDFlags;

struct _AgsRecallID
{
  GObject object;

  guint flags;

  guint parent_group_id;
  guint group_id;
  guint child_group_id;

  AgsRecycling *first_recycling; // the AgsRecycling that indicates the beginning for affecting AgsRecalls, NULL means AgsChannel's first_recycling
  AgsRecycling *last_recycling; // the AgsRecycling that indicates the end for affecting AgsRecalls, NULL means AgsChannel's last_recycling
};

struct _AgsRecallIDClass
{
  GObjectClass object;
};

GType ags_recall_id_get_type(void);

void ags_recall_id_connect(AgsRecallID *recall_id);

guint ags_recall_id_generate_group_id();

gboolean ags_recall_id_get_run_stage(AgsRecallID *id, gint stage);
void ags_recall_id_set_run_stage(AgsRecallID *recall_id, gint stage);
void ags_recall_id_unset_run_stage(AgsRecallID *recall_id, gint stage);

GList* ags_recall_id_add(GList *recall_id_list,
			 guint parent_group_id, guint group_id, guint child_group_id,
			 AgsRecycling *first_recycling, AgsRecycling *last_recycling,
			 gboolean higher_level_is_recall);
AgsRecallID* ags_recall_id_find_group_id(GList *recall_id_list, guint group_id);
AgsRecallID* ags_recall_id_find_parent_group_id(GList *recall_id_list, guint parent_group_id);

AgsRecallID* ags_recall_id_new();

#endif /*__AGS_RECALL_ID_H__*/
