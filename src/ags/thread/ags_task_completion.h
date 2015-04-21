/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_TASK_COMPLETION_H__
#define __AGS_TASK_COMPLETION_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_TASK_COMPLETION                (ags_task_completion_get_type())
#define AGS_TASK_COMPLETION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TASK_COMPLETION, AgsTaskCompletion))
#define AGS_TASK_COMPLETION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TASK_COMPLETION, AgsTaskCompletionClass))
#define AGS_IS_TASK_COMPLETION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TASK_COMPLETION))
#define AGS_IS_TASK_COMPLETION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TASK_COMPLETION))
#define AGS_TASK_COMPLETION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TASK_COMPLETION, AgsTaskCompletionClass))

typedef struct _AgsTaskCompletion AgsTaskCompletion;
typedef struct _AgsTaskCompletionClass AgsTaskCompletionClass;

typedef enum{
  AGS_TASK_COMPLETION_QUEUED      = 1,
  AGS_TASK_COMPLETION_BUSY        = 1 << 1,
  AGS_TASK_COMPLETION_READY       = 1 << 2,
  AGS_TASK_COMPLETION_COMPLETED   = 1 << 3,
}AgsTaskCompletionFlags;

struct _AgsTaskCompletion
{
  GObject gobject;

  volatile guint flags;
  
  GObject *task;
  gpointer data;
};

struct _AgsTaskCompletionClass
{
  GObjectClass gobject;

  void (*complete)(AgsTaskCompletion *task_completion);
};

GType ags_task_completion_get_type();

void ags_task_completion_complete(AgsTaskCompletion *task_completion);

AgsTaskCompletion* ags_task_completion_new(GObject *task,
					   gpointer data);

#endif /*__AGS_TASK_COMPLETION_H__*/

