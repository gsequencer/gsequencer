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

#ifndef __AGS_LV2_WORKER_MANAGER_H__
#define __AGS_LV2_WORKER_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>

G_BEGIN_DECLS

#define AGS_TYPE_LV2_WORKER_MANAGER                (ags_lv2_worker_manager_get_type())
#define AGS_LV2_WORKER_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_WORKER_MANAGER, AgsLv2WorkerManager))
#define AGS_LV2_WORKER_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_WORKER_MANAGER, AgsLv2WorkerManagerClass))
#define AGS_IS_LV2_WORKER_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_WORKER_MANAGER))
#define AGS_IS_LV2_WORKER_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_WORKER_MANAGER))
#define AGS_LV2_WORKER_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_WORKER_MANAGER, AgsLv2WorkerManagerClass))

#define AGS_LV2_WORKER_MANAGER_GET_OBJ_MUTEX(obj) (&(((AgsLv2WorkerManager *) obj)->obj_mutex))

typedef struct _AgsLv2WorkerManager AgsLv2WorkerManager;
typedef struct _AgsLv2WorkerManagerClass AgsLv2WorkerManagerClass;

struct _AgsLv2WorkerManager
{
  GObject gobject;

  GRecMutex obj_mutex;

  AgsThreadPool *thread_pool;
  
  GList *worker;
};

struct _AgsLv2WorkerManagerClass
{
  GObjectClass gobject;
};

GType ags_lv2_worker_manager_get_type(void);

GObject* ags_lv2_worker_manager_pull_worker(AgsLv2WorkerManager *worker_manager);

AgsLv2WorkerManager* ags_lv2_worker_manager_get_instance();
AgsLv2WorkerManager* ags_lv2_worker_manager_new();

G_END_DECLS

#endif /*__AGS_LV2_WORKER_MANAGER_H__*/
