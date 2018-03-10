/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_RECALL_ID_H__
#define __AGS_RECALL_ID_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_recycling_context.h>

#define AGS_TYPE_RECALL_ID                (ags_recall_id_get_type ())
#define AGS_RECALL_ID(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_ID, AgsRecallID))
#define AGS_RECALL_ID_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_ID, AgsRecallIDClass))
#define AGS_IS_RECALL_ID(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_ID))
#define AGS_IS_RECALL_ID_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_ID))
#define AGS_RECALL_ID_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_ID, AgsRecallIDClass))

typedef struct _AgsRecallID AgsRecallID;
typedef struct _AgsRecallIDClass AgsRecallIDClass;

typedef enum{
  AGS_RECALL_ID_CONNECTED         = 1,
}AgsRecallIDFlags;

struct _AgsRecallID
{
  GObject object;
  
  guint flags;
  gint sound_scope;
  guint staging_flags;
  guint state_flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  AgsRecyclingContext *recycling_context;
};

struct _AgsRecallIDClass
{
  GObjectClass object;
};

GType ags_recall_id_get_type(void);

pthread_mutex_t* ags_recall_id_get_class_mutex();

/* scope */
void ags_recall_id_set_sound_scope(AgsRecallID *recall_id, gint sound_scope);

gboolean ags_recall_id_check_sound_scope(AgsRecallID *recall_id, gint sound_scope);

/* staging flags */
void ags_recall_id_set_staging_flags(AgsRecallID *recall_id, guint staging_flags);
void ags_recall_id_unset_staging_flags(AgsRecallID *recall_id, guint staging_flags);

gboolean ags_recall_id_check_staging_flags(AgsRecallID *recall_id, guint staging_flags);

/* state flags */
void ags_recall_id_set_state_flags(AgsRecallID *recall_id, guint state_flags);
void ags_recall_id_unset_state_flags(AgsRecallID *recall_id, guint state_flags);

gboolean ags_recall_id_check_state_flags(AgsRecallID *recall_id, guint state_flags);

/* query recycling context */
AgsRecallID* ags_recall_id_find_recycling_context(GList *recall_id,
						  AgsRecyclingContext *recycling_context);
AgsRecallID* ags_recall_id_find_parent_recycling_context(GList *recall_id,
							 AgsRecyclingContext *parent_recycling_context);

/* instantiate */
AgsRecallID* ags_recall_id_new();

#endif /*__AGS_RECALL_ID_H__*/
