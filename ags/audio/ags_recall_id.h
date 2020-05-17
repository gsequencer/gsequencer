/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_recycling_context.h>

G_BEGIN_DECLS

#define AGS_TYPE_RECALL_ID                (ags_recall_id_get_type ())
#define AGS_RECALL_ID(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), AGS_TYPE_RECALL_ID, AgsRecallID))
#define AGS_RECALL_ID_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST ((class), AGS_TYPE_RECALL_ID, AgsRecallIDClass))
#define AGS_IS_RECALL_ID(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECALL_ID))
#define AGS_IS_RECALL_ID_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECALL_ID))
#define AGS_RECALL_ID_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECALL_ID, AgsRecallIDClass))

#define AGS_RECALL_ID_GET_OBJ_MUTEX(obj) (&(((AgsRecallID *) obj)->obj_mutex))

typedef struct _AgsRecallID AgsRecallID;
typedef struct _AgsRecallIDClass AgsRecallIDClass;

struct _AgsRecallID
{
  GObject gobject;
  
  guint flags;
  gint sound_scope;
  guint staging_flags;
  guint state_flags;
  
  GRecMutex obj_mutex;

  AgsRecyclingContext *recycling_context;
};

struct _AgsRecallIDClass
{
  GObjectClass gobject;
};

GType ags_recall_id_get_type(void);

/* scope */
void ags_recall_id_set_sound_scope(AgsRecallID *recall_id, gint sound_scope);

gboolean ags_recall_id_check_sound_scope(AgsRecallID *recall_id, gint sound_scope);

/* staging flags */
gboolean ags_recall_id_test_staging_flags(AgsRecallID *recall_id, guint staging_flags);
void ags_recall_id_set_staging_flags(AgsRecallID *recall_id, guint staging_flags);
void ags_recall_id_unset_staging_flags(AgsRecallID *recall_id, guint staging_flags);

gboolean ags_recall_id_check_staging_flags(AgsRecallID *recall_id, guint staging_flags);

/* state flags */
gboolean ags_recall_id_test_state_flags(AgsRecallID *recall_id, guint state_flags);
void ags_recall_id_set_state_flags(AgsRecallID *recall_id, guint state_flags);
void ags_recall_id_unset_state_flags(AgsRecallID *recall_id, guint state_flags);

gboolean ags_recall_id_check_state_flags(AgsRecallID *recall_id, guint state_flags);

/* properties */
AgsRecyclingContext* ags_recall_id_get_recycling_context(AgsRecallID *recall_id);
void ags_recall_id_set_recycling_context(AgsRecallID *recall_id, AgsRecyclingContext *recycling_context);

/* query recycling context */
AgsRecallID* ags_recall_id_find_recycling_context(GList *recall_id,
						  AgsRecyclingContext *recycling_context);
AgsRecallID* ags_recall_id_find_parent_recycling_context(GList *recall_id,
							 AgsRecyclingContext *parent_recycling_context);

/* instantiate */
AgsRecallID* ags_recall_id_new();

G_END_DECLS

#endif /*__AGS_RECALL_ID_H__*/
