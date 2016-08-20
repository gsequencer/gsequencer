/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/plugin/ags_lv2_worker_manager.h>
#include <ags/plugin/ags_lv2_worker.h>

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_returnable_thread.h>

void ags_lv2_worker_manager_class_init(AgsLv2WorkerManagerClass *lv2_worker_manager);
void ags_lv2_worker_manager_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_lv2_worker_manager_init(AgsLv2WorkerManager *lv2_worker_manager);
void ags_lv2_worker_manager_connect(AgsConnectable *connectable);
void ags_lv2_worker_manager_disconnect(AgsConnectable *connectable);
void ags_lv2_worker_manager_finalize(GObject *gobject);

/**
 * SECTION:ags_lv2_worker_manager
 * @short_description: manage workers
 * @title: AgsLv2WorkerManager
 * @section_id:
 * @include: ags/plugin/ags_lv2_worker_manager.h
 *
 * The #AgsLv2WorkerManager tracks your workers.
 */

static gpointer ags_lv2_worker_manager_parent_class = NULL;

AgsLv2WorkerManager *ags_lv2_worker_manager = NULL;

GType
ags_lv2_worker_manager_get_type()
{
  static GType ags_type_lv2_worker_manager = 0;

  if(!ags_type_lv2_worker_manager){
    const GTypeInfo ags_lv2_worker_manager_info = {
      sizeof (AgsLv2WorkerManagerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_worker_manager_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLv2WorkerManager),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_worker_manager_init,
    };

    const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_lv2_worker_manager_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_lv2_worker_manager = g_type_register_static(G_TYPE_OBJECT,
						      "AgsLv2WorkerManager\0",
						      &ags_lv2_worker_manager_info,
						      0);

    g_type_add_interface_static(ags_type_lv2_worker_manager,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_lv2_worker_manager);
}

void
ags_lv2_worker_manager_class_init(AgsLv2WorkerManagerClass *lv2_worker_manager)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_lv2_worker_manager_parent_class = g_type_class_peek_parent(lv2_worker_manager);

  /* GObject */
  gobject = (GObjectClass *) lv2_worker_manager;

  gobject->finalize = ags_lv2_worker_manager_finalize;
}

void
ags_lv2_worker_manager_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_lv2_worker_manager_connect;
  connectable->disconnect = ags_lv2_worker_manager_disconnect;
}

void
ags_lv2_worker_manager_init(AgsLv2WorkerManager *worker_manager)
{
  worker_manager->thread_pool = NULL;

  g_atomic_pointer_set(&(worker_manager->worker),
		       NULL);
  
  /* empty */
}

void
ags_lv2_worker_manager_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_worker_manager_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_lv2_worker_manager_finalize(GObject *gobject)
{
  /* empty */
}

GObject*
ags_lv2_worker_manager_pull_worker(AgsLv2WorkerManager *worker_manager)
{
  AgsLv2Worker *lv2_worker;
  AgsThread *thread;

  thread = ags_thread_pool_pull(worker_manager->thread_pool);
  lv2_worker = ags_lv2_worker_new(thread);
  
  pthread_mutex_lock(AGS_RETURNABLE_THREAD(thread)->reset_mutex);

  g_atomic_pointer_set(&(AGS_RETURNABLE_THREAD(thread)->safe_data),
		       lv2_worker);
  
  ags_returnable_thread_connect_safe_run(AGS_RETURNABLE_THREAD(thread),
					 ags_lv2_worker_safe_run);
  
  g_atomic_int_or(&(AGS_RETURNABLE_THREAD(thread)->flags),
		  AGS_RETURNABLE_THREAD_IN_USE);
  g_atomic_int_and(&(AGS_RETURNABLE_THREAD(thread)->flags),
		   (~AGS_RETURNABLE_THREAD_RUN_ONCE));
  
  pthread_mutex_unlock(AGS_RETURNABLE_THREAD(thread)->reset_mutex);

  return(lv2_worker);
}

/**
 * ags_lv2_worker_manager_get_instance:
 * 
 * Singleton function to optain the id manager instance.
 *
 * Returns: an instance of #AgsLv2WorkerManager
 *
 * Since: 0.7.7
 */
AgsLv2WorkerManager*
ags_lv2_worker_manager_get_instance()
{
  if(ags_lv2_worker_manager == NULL){
    ags_lv2_worker_manager = ags_lv2_worker_manager_new();
    
    //    ags_lv2_worker_manager_load_default(ags_lv2_worker_manager);
  }

  return(ags_lv2_worker_manager);
}

/**
 * ags_lv2_worker_manager_new:
 *
 * Instantiate a id manager. 
 *
 * Returns: a new #AgsLv2WorkerManager
 *
 * Since: 0.7.7
 */
AgsLv2WorkerManager*
ags_lv2_worker_manager_new()
{
  AgsLv2WorkerManager *lv2_worker_manager;

  lv2_worker_manager = (AgsLv2WorkerManager *) g_object_new(AGS_TYPE_LV2_WORKER_MANAGER,
						      NULL);

  return(lv2_worker_manager);
}
