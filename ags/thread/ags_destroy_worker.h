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

#ifndef __AGS_DESTROY_WORKER_H__
#define __AGS_DESTROY_WORKER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_worker_thread.h>

#define AGS_TYPE_DESTROY_WORKER                (ags_destroy_worker_get_type())
#define AGS_DESTROY_WORKER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DESTROY_WORKER, AgsDestroyWorker))
#define AGS_DESTROY_WORKER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DESTROY_WORKER, AgsDestroyWorkerClass))
#define AGS_IS_DESTROY_WORKER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_DESTROY_WORKER))
#define AGS_IS_DESTROY_WORKER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_DESTROY_WORKER))
#define AGS_DESTROY_WORKER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DESTROY_WORKER, AgsDestroyWorkerClass))

#define AGS_DESTROY_ENTRY(ptr) ((AgsDestroyEntry *) (ptr))

typedef struct _AgsDestroyWorker AgsDestroyWorker;
typedef struct _AgsDestroyWorkerClass AgsDestroyWorkerClass;
typedef struct _AgsDestroyEntry AgsDestroyEntry;

typedef void (*AgsDestroyFunc)(gpointer ptr);

struct _AgsDestroyWorker
{
  AgsWorkerThread worker_thread;

  pthread_mutex_t *destroy_mutex;
  pthread_mutexattr_t *destroy_mutexattr;

  GList *destroy_list;
};

struct _AgsDestroyWorkerClass
{
  AgsWorkerThreadClass worker_thread;
};

struct _AgsDestroyEntry
{
  gpointer ptr;
  
  void (*destroy_func)(gpointer ptr);
};

GType ags_destroy_worker_get_type();

AgsDestroyEntry* ags_destroy_entry_alloc(gpointer ptr, AgsDestroyFunc destroy_func);

void ags_destroy_worker_add(AgsDestroyWorker *destroy_worker,
			    gpointer ptr, AgsDestroyFunc destroy_func);

AgsDestroyWorker* ags_destroy_worker_new();

#endif /*__AGS_DESTROY_WORKER_H__*/
