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

#ifndef __AGS_TASK_H__
#define __AGS_TASK_H__

#include <pthread.h>

#include <glib-object.h>

#define AGS_TYPE_TASK                (ags_task_get_type())
#define AGS_TASK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TASK, AgsTask))
#define AGS_TASK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_TASK, AgsTaskClass))
#define AGS_IS_TASK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_TASK))
#define AGS_IS_TASK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_TASK))
#define AGS_TASK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_TASK, AgsTaskClass))

typedef struct _AgsTask AgsTask;
typedef struct _AgsTaskClass AgsTaskClass;

typedef enum{
  AGS_TASK_LOCKED = 1,
}AgsTaskFlags;

struct _AgsTask
{
  GObject object;

  guint flags;

  char *name;

  guint start;

  pthread_cond_t wait_sync_task_cond;
};

struct _AgsTaskClass
{
  GObjectClass object;

  void (*launch)(AgsTask *task);

  void (*failure)(AgsTask *task, GError *error);
};

GType ags_task_get_type();

void ags_task_launch(AgsTask *task);

AgsTask* ags_task_new();

#endif /*__AGS_TASK_H__*/
