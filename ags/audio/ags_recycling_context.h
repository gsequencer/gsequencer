/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_RECYCLING_CONTEXT_H__
#define __AGS_RECYCLING_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_recycling.h>

#define AGS_TYPE_RECYCLING_CONTEXT                (ags_recycling_context_get_type())
#define AGS_RECYCLING_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECYCLING_CONTEXT, AgsRecyclingContext))
#define AGS_RECYCLING_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECYCLING_CONTEXT, AgsRecyclingContextClass))
#define AGS_IS_RECYCLING_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RECYCLING_CONTEXT))
#define AGS_IS_RECYCLING_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RECYCLING_CONTEXT))
#define AGS_RECYCLING_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_RECYCLING_CONTEXT, AgsRecyclingContextClass))

typedef struct _AgsRecyclingContext AgsRecyclingContext;
typedef struct _AgsRecyclingContextClass AgsRecyclingContextClass;

struct _AgsRecyclingContext
{
  GObject object;

  pthread_mutexattr_t *mutexattr;
  pthread_mutex_t *mutex;
  
  AgsRecyclingContext *parent;

  GObject *recall_id;

  AgsRecycling **recycling;
  guint64 length;

  GList *children;
};

struct _AgsRecyclingContextClass
{
  GObjectClass object;
};

GType ags_recycling_context_get_type();

/* replace, add, remove and insert */
void ags_recycling_context_replace(AgsRecyclingContext *recycling_context,
				   AgsRecycling *recycling,
				   gint position);

AgsRecyclingContext* ags_recycling_context_add(AgsRecyclingContext *recycling_context,
					       AgsRecycling *recycling);
AgsRecyclingContext* ags_recycling_context_remove(AgsRecyclingContext *recycling_context,
						  AgsRecycling *recycling);
AgsRecyclingContext* ags_recycling_context_insert(AgsRecyclingContext *recycling_context,
						  AgsRecycling *recycling,
						  gint position);

/* tolevel, find, find child and find parent */
AgsRecyclingContext* ags_recycling_context_get_toplevel(AgsRecyclingContext *recycling_context);

gint ags_recycling_context_find(AgsRecyclingContext *recycling_context,
				AgsRecycling *recycling);
gint ags_recycling_context_find_child(AgsRecyclingContext *recycling_context,
				      AgsRecycling *recycling);
gint ags_recycling_context_find_parent(AgsRecyclingContext *recycling_context,
				       AgsRecycling *recycling);

/* add and remove child */
void ags_recycling_context_add_child(AgsRecyclingContext *parent,
				     AgsRecyclingContext *child);
void ags_recycling_context_remove_child(AgsRecyclingContext *parent,
					AgsRecyclingContext *child);

/* child recall id */
GList* ags_recycling_context_get_child_recall_id(AgsRecyclingContext *recycling_context);

/* instantiate - reset recycling */
AgsRecyclingContext* ags_recycling_context_reset_recycling(AgsRecyclingContext *recycling_context,
							   AgsRecycling *old_first_recycling, AgsRecycling *old_last_recycling,
							   AgsRecycling *new_first_recycling, AgsRecycling *new_last_recycling);

/* instantiate */
AgsRecyclingContext* ags_recycling_context_new(guint64 length);

#endif /*__AGS_RECYCLING_CONTEXT_H__*/
