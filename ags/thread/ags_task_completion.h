/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_TASK_COMPLETION_H__
#define __AGS_TASK_COMPLETION_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_uuid.h>

G_BEGIN_DECLS

#define AGS_TYPE_TASK_COMPLETION                (ags_task_completion_get_type())
#define AGS_TYPE_TASK_COMPLETION_FLAGS          (ags_task_completion_flags_get_type())
#define AGS_TASK_COMPLETION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TASK_COMPLETION, AgsTaskCompletion))
#define AGS_TASK_COMPLETION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TASK_COMPLETION, AgsTaskCompletionClass))
#define AGS_IS_TASK_COMPLETION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TASK_COMPLETION))
#define AGS_IS_TASK_COMPLETION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TASK_COMPLETION))
#define AGS_TASK_COMPLETION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TASK_COMPLETION, AgsTaskCompletionClass))

#define AGS_TASK_COMPLETION_GET_OBJ_MUTEX(obj) (&(((AgsTaskCompletion *) obj)->obj_mutex))

typedef struct _AgsTaskCompletion AgsTaskCompletion;
typedef struct _AgsTaskCompletionClass AgsTaskCompletionClass;

/**
 * AgsTaskCompletionFlags:
 * @AGS_TASK_COMPLETION_ADDED_TO_REGISTRY: indicates the task completion was added to #AgsRegistry
 * @AGS_TASK_COMPLETION_CONNECTED: indicates the task completion was connected by calling #AgsConnectable::connect()
 * @AGS_TASK_COMPLETION_QUEUED: the assigned task has been queued
 * @AGS_TASK_COMPLETION_BUSY: the assigned task is busy
 * @AGS_TASK_COMPLETION_READY: the assigned task is ready
 * @AGS_TASK_COMPLETION_COMPLETED: the assigned task has been completed
 *
 * Enum values to control the behavior or indicate internal state of #AgsTaskCompletion by
 * enable/disable as flags.
 */
typedef enum{
  AGS_TASK_COMPLETION_ADDED_TO_REGISTRY  = 1,
  AGS_TASK_COMPLETION_CONNECTED          = 1 <<  1,
  AGS_TASK_COMPLETION_QUEUED             = 1 <<  2,
  AGS_TASK_COMPLETION_BUSY               = 1 <<  3,
  AGS_TASK_COMPLETION_READY              = 1 <<  4,
  AGS_TASK_COMPLETION_COMPLETED          = 1 <<  5,
}AgsTaskCompletionFlags;

struct _AgsTaskCompletion
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;
  
  GObject *task;
  gpointer data;
};

struct _AgsTaskCompletionClass
{
  GObjectClass gobject;

  void (*complete)(AgsTaskCompletion *task_completion);
};

GType ags_task_completion_get_type();
GType ags_task_completion_flags_get_type();

gboolean ags_task_completion_test_flags(AgsTaskCompletion *task_completion, guint flags);
void ags_task_completion_set_flags(AgsTaskCompletion *task_completion, guint flags);
void ags_task_completion_unset_flags(AgsTaskCompletion *task_completion, guint flags);

void ags_task_completion_complete(AgsTaskCompletion *task_completion);

AgsTaskCompletion* ags_task_completion_new(GObject *task,
					   gpointer data);

G_END_DECLS

#endif /*__AGS_TASK_COMPLETION_H__*/

