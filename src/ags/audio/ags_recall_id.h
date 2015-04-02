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
#include <ags/audio/ags_recycling_container.h>

#define AGS_TYPE_RECALL_ID                (ags_recall_id_get_type ())
#define AGS_RECALL_ID(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_ID, AgsRecallID))
#define AGS_RECALL_ID_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_ID, AgsRecallIDClass))
#define AGS_IS_RECALL_ID(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_ID))
#define AGS_IS_RECALL_ID_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_ID))
#define AGS_RECALL_ID_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_ID, AgsRecallIDClass))

typedef struct _AgsRecallID AgsRecallID;
typedef struct _AgsRecallIDClass AgsRecallIDClass;

typedef enum{
  AGS_RECALL_ID_PLAYBACK          = 1,
  AGS_RECALL_ID_SEQUENCER         = 1 << 1,
  AGS_RECALL_ID_NOTATION          = 1 << 2,
  AGS_RECALL_ID_DUPLICATE         = 1 << 3,
  AGS_RECALL_ID_RESOLVE           = 1 << 4,
  AGS_RECALL_ID_INIT_PRE          = 1 << 5,
  AGS_RECALL_ID_INIT_INTER        = 1 << 6,
  AGS_RECALL_ID_INIT_POST         = 1 << 7,
  AGS_RECALL_ID_PRE               = 1 << 8,
  AGS_RECALL_ID_INTER             = 1 << 9,
  AGS_RECALL_ID_POST              = 1 << 10,
  AGS_RECALL_ID_CANCEL            = 1 << 11,
}AgsRecallIDFlags;

struct _AgsRecallID
{
  GObject object;
  
  guint flags;

  GObject *recycling;
  AgsRecyclingContainer *recycling_container;
};

struct _AgsRecallIDClass
{
  GObjectClass object;
};

GType ags_recall_id_get_type(void);

gboolean ags_recall_id_get_run_stage(AgsRecallID *id, gint stage);
void ags_recall_id_set_run_stage(AgsRecallID *recall_id, gint stage);
void ags_recall_id_unset_run_stage(AgsRecallID *recall_id, gint stage);

AgsRecallID* ags_recall_id_find_recycling_container(GList *recall_id_list,
						    AgsRecyclingContainer *recycling_container);
AgsRecallID* ags_recall_id_find_parent_recycling_container(GList *recall_id_list,
							   AgsRecyclingContainer *parent_recycling_container);

GList* ags_recall_id_add(GList *recall_id_list,
			 AgsRecallID *recall_id);

AgsRecallID* ags_recall_id_new(AgsRecycling *recycling);

#endif /*__AGS_RECALL_ID_H__*/
