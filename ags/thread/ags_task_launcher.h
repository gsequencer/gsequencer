/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_TASK_LAUNCHER_H__
#define __AGS_TASK_LAUNCHER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_uuid.h>

#include <ags/thread/ags_task.h>

G_BEGIN_DECLS

#define AGS_TYPE_TASK_LAUNCHER                (ags_task_launcher_get_type())
#define AGS_TYPE_TASK_LAUNCHER_FLAGS          (ags_task_launcher_flags_get_type())
#define AGS_TASK_LAUNCHER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_TASK_LAUNCHER, AgsTaskLauncher))
#define AGS_TASK_LAUNCHER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_TASK_LAUNCHER, AgsTaskLauncher))
#define AGS_IS_TASK_LAUNCHER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_TASK_LAUNCHER))
#define AGS_IS_TASK_LAUNCHER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_TASK_LAUNCHER))
#define AGS_TASK_LAUNCHER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_TASK_LAUNCHER, AgsTaskLauncherClass))

#define AGS_TASK_LAUNCHER_GET_OBJ_MUTEX(obj) (&(((AgsTaskLauncher *) obj)->obj_mutex))

typedef struct _AgsTaskLauncher AgsTaskLauncher;
typedef struct _AgsTaskLauncherClass AgsTaskLauncherClass;

struct _AgsTaskLauncher
{
  GObject gobject;

  guint flags;
  guint connectable_flags;
  
  GRecMutex obj_mutex;
  
  AgsUUID *uuid;
  
  GMainContext *main_context;
  
  GList *task;
  GList *cyclic_task;

  volatile gboolean is_running;
  volatile gint wait_count;
  
  GMutex wait_mutex;
  GCond wait_cond;
};

struct _AgsTaskLauncherClass
{
  GObjectClass gobject;

  void (*run)(AgsTaskLauncher *task_launcher);
};

GType ags_task_launcher_get_type();
GType ags_task_launcher_flags_get_type();

gboolean ags_task_launcher_test_flags(AgsTaskLauncher *task_launcher, guint flags);
void ags_task_launcher_set_flags(AgsTaskLauncher *task_launcher, guint flags);
void ags_task_launcher_unset_flags(AgsTaskLauncher *task_launcher, guint flags);

void ags_task_launcher_attach(AgsTaskLauncher *task_launcher,
			      GMainContext *main_context);

void ags_task_launcher_add_task(AgsTaskLauncher *task_launcher,
				AgsTask *task);
void ags_task_launcher_add_task_all(AgsTaskLauncher *task_launcher,
				    GList *list);

void ags_task_launcher_add_cyclic_task(AgsTaskLauncher *task_launcher,
				       AgsTask *cyclic_task);
void ags_task_launcher_remove_cyclic_task(AgsTaskLauncher *task_launcher,
					  AgsTask *cyclic_task);

void ags_task_launcher_run(AgsTaskLauncher *task_launcher);

void ags_task_launcher_sync_run(AgsTaskLauncher *task_launcher);

AgsTaskLauncher* ags_task_launcher_new();

G_END_DECLS

#endif /*__AGS_TASK_LAUNCHER_H__*/
