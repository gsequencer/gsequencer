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

#ifndef __AGS_THREAD_H__
#define __AGS_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_uuid.h>
#include <ags/lib/ags_time.h>

#include <time.h>

G_BEGIN_DECLS

#define AGS_TYPE_THREAD                (ags_thread_get_type())
#define AGS_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD, AgsThread))
#define AGS_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD, AgsThreadClass))
#define AGS_IS_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD))
#define AGS_IS_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD))
#define AGS_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD, AgsThreadClass))

#define AGS_THREAD_GET_OBJ_MUTEX(obj) (&(((AgsThread *) obj)->obj_mutex))

#define AGS_THREAD_GET_WAIT_MUTEX(obj) (&(((AgsThread *) obj)->wait_mutex))
#define AGS_THREAD_GET_WAIT_COND(obj) (&(((AgsThread *) obj)->wait_cond))
#define AGS_THREAD_GET_TIC_MUTEX(obj) (&(((AgsThread *) obj)->tic_mutex))
#define AGS_THREAD_GET_TIC_COND(obj) (&(((AgsThread *) obj)->tic_cond))
#define AGS_THREAD_GET_START_MUTEX(obj) (&(((AgsThread *) obj)->start_mutex))
#define AGS_THREAD_GET_START_COND(obj) (&(((AgsThread *) obj)->start_cond))

#ifndef AGS_RT_PRIORITY
#define AGS_RT_PRIORITY (45)
#endif

#define AGS_THREAD_HERTZ_JIFFIE (1000.0)
#define AGS_THREAD_YIELD_JIFFIE (2.0)

#define AGS_THREAD_DEFAULT_JIFFIE (250.0)
#define AGS_THREAD_DEFAULT_MAX_PRECISION (1000.0)

#define AGS_THREAD_MAX_PRECISION (1000.0)

#define AGS_THREAD_DEFAULT_ATTACK (1.0)

#define AGS_THREAD_TOLERANCE (0.0)

typedef struct _AgsThread AgsThread;
typedef struct _AgsThreadClass AgsThreadClass;

/**
 * AgsThreadFlags:
 * @AGS_THREAD_ADDED_TO_REGISTRY: the thread was added to registry, see #AgsConnectable::add_to_registry()
 * @AGS_THREAD_CONNECTED: the thread was connected by #AgsConnectable::connect()
 * @AGS_THREAD_UNREF_ON_EXIT: call g_object_unref() before g_thread_exit()
 * @AGS_THREAD_IMMEDIATE_SYNC: do sync immediately
 * @AGS_THREAD_INTERMEDIATE_PRE_SYNC: intermediate pre sync to parent thread
 * @AGS_THREAD_INTERMEDIATE_POST_SYNC: intermediate post sync to parent thread
 * @AGS_THREAD_START_SYNCED_FREQ: sync frequency as starting thread
 * @AGS_THREAD_BROADCAST_PARENT: broadcast parent
 * @AGS_THREAD_BROADCAST_SIBLING: broadcast sibling
 * @AGS_THREAD_BROADCAST_CHILDREN: broadcast children
 *
 * Enum values to control the behavior or indicate internal state of #AgsThread by
 * enable/disable as flags.
 */
typedef enum{
  AGS_THREAD_ADDED_TO_REGISTRY       = 1,
  AGS_THREAD_CONNECTED               = 1 <<  1,
  AGS_THREAD_UNREF_ON_EXIT           = 1 <<  2,
  AGS_THREAD_IMMEDIATE_SYNC          = 1 <<  3,
  AGS_THREAD_INTERMEDIATE_PRE_SYNC   = 1 <<  4,
  AGS_THREAD_INTERMEDIATE_POST_SYNC  = 1 <<  5,
  AGS_THREAD_START_SYNCED_FREQ       = 1 <<  6,
  AGS_THREAD_MARK_SYNCED             = 1 <<  7,
  AGS_THREAD_BROADCAST_PARENT        = 1 <<  8,
  AGS_THREAD_BROADCAST_SIBLING       = 1 <<  9,
  AGS_THREAD_BROADCAST_CHILDREN      = 1 << 10,
}AgsThreadFlags;

/**
 * AgsThreadStatusFlags:
 * @AGS_THREAD_STATUS_RT_SETUP: realtime setup was performed
 * @AGS_THREAD_STATUS_INITIAL_SYNC: initial sync indicates the thread wasn't synced before
 * @AGS_THREAD_STATUS_INITIAL_RUN: the first call to #AgsThread:run()
 * @AGS_THREAD_STATUS_IS_CHAOS_TREE: the thread is not synced
 * @AGS_THREAD_STATUS_START_WAIT: the thread start is waiting
 * @AGS_THREAD_STATUS_START_DONE: the thread start is done
 * @AGS_THREAD_STATUS_READY: the thread is ready
 * @AGS_THREAD_STATUS_WAITING: the thread is waiting
 * @AGS_THREAD_STATUS_RUNNING: the thread is running
 * @AGS_THREAD_STATUS_LOCKED: the thread is locked
 * @AGS_THREAD_STATUS_IDLE: the thread is idle
 * @AGS_THREAD_STATUS_WAIT_FOR_PARENT: wait for parent
 * @AGS_THREAD_STATUS_WAIT_FOR_SIBLING: wait for sibling
 * @AGS_THREAD_STATUS_WAIT_FOR_CHILDREN: wait for children
 * @AGS_THREAD_STATUS_WAITING_FOR_PARENT: the thread is waiting for parent
 * @AGS_THREAD_STATUS_WAITING_FOR_SIBLING: the thread is waiting for sibling
 * @AGS_THREAD_STATUS_WAITING_FOR_CHILDREN: the thread is waiting for children
 * @AGS_THREAD_STATUS_TREE_SYNC_0: exclusively synced tic group
 * @AGS_THREAD_STATUS_WAIT_0: wait tree to be synced
 * @AGS_THREAD_STATUS_TREE_SYNC_1: exclusively synced tic group
 * @AGS_THREAD_STATUS_WAIT_1: wait tree to be synced
 * @AGS_THREAD_STATUS_TREE_SYNC_2: exclusively synced tic group
 * @AGS_THREAD_STATUS_WAIT_2: wait tree to be synced
 * @AGS_THREAD_STATUS_SYNCED: the thread joined the tic based system, it is synced
 * @AGS_THREAD_STATUS_SYNCED_FREQ: the frequency was synced
 * @AGS_THREAD_STATUS_WAIT_ASYNC_QUEUE: wait exclusive async running queue
 * @AGS_THREAD_STATUS_DONE_ASYNC_QUEUE: done exclusive async running queue
 *
 * Enum values to control the behavior or indicate internal state of #AgsThread by
 * enable/disable as status flags.
 */
typedef enum{
  AGS_THREAD_STATUS_RT_SETUP                = 1,
  AGS_THREAD_STATUS_INITIAL_SYNC            = 1 <<  1,
  AGS_THREAD_STATUS_INITIAL_RUN             = 1 <<  2,
  AGS_THREAD_STATUS_IS_CHAOS_TREE           = 1 <<  3,
  AGS_THREAD_STATUS_START_WAIT              = 1 <<  4,
  AGS_THREAD_STATUS_START_DONE              = 1 <<  5,
  AGS_THREAD_STATUS_READY                   = 1 <<  6,
  AGS_THREAD_STATUS_WAITING                 = 1 <<  7,
  AGS_THREAD_STATUS_RUNNING                 = 1 <<  8,
  AGS_THREAD_STATUS_LOCKED                  = 1 <<  9,
  AGS_THREAD_STATUS_IDLE                    = 1 << 10,
  AGS_THREAD_STATUS_WAIT_FOR_PARENT         = 1 << 11,
  AGS_THREAD_STATUS_WAIT_FOR_SIBLING        = 1 << 12,
  AGS_THREAD_STATUS_WAIT_FOR_CHILDREN       = 1 << 13,
  AGS_THREAD_STATUS_WAITING_FOR_PARENT      = 1 << 14,
  AGS_THREAD_STATUS_WAITING_FOR_SIBLING     = 1 << 15,
  AGS_THREAD_STATUS_WAITING_FOR_CHILDREN    = 1 << 16,
  AGS_THREAD_STATUS_TREE_SYNC_0             = 1 << 17,
  AGS_THREAD_STATUS_WAIT_0                  = 1 << 18,
  AGS_THREAD_STATUS_TREE_SYNC_1             = 1 << 19,
  AGS_THREAD_STATUS_WAIT_1                  = 1 << 20,
  AGS_THREAD_STATUS_TREE_SYNC_2             = 1 << 21,
  AGS_THREAD_STATUS_WAIT_2                  = 1 << 22,
  AGS_THREAD_STATUS_SYNCED                  = 1 << 23,
  AGS_THREAD_STATUS_SYNCED_FREQ             = 1 << 24,
  AGS_THREAD_STATUS_WAIT_ASYNC_QUEUE        = 1 << 25,
  AGS_THREAD_STATUS_DONE_ASYNC_QUEUE        = 1 << 26,
}AgsThreadStatusFlags;

struct _AgsThread
{
  GObject gobject;

  guint my_flags;
  volatile guint status_flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  guint delay;
  guint tic_delay;
  guint current_tic;

  guint cycle_iteration;
  guint time_late;
  struct timespec *computing_time;
  
  GThread *thread;

  gdouble freq;
  gdouble max_precision;

  GMutex wait_mutex;
  GCond wait_cond;
  
  GMutex tic_mutex;
  GCond tic_cond;

  GList *start_queue;
  
  GMutex *start_mutex;
  GCond *start_cond;
  
  AgsThread *parent;

  AgsThread *next;
  AgsThread *prev;

  AgsThread *children;

  gpointer data;
};

struct _AgsThreadClass
{
  GObjectClass gobject;

  guint (*clock)(AgsThread *thread);
  
  void (*start)(AgsThread *thread);
  void (*run)(AgsThread *thread);
  void (*stop)(AgsThread *thread);
};

GType ags_thread_get_type();

gboolean ags_thread_global_get_use_sync_counter();

gboolean ags_thread_test_flags(AgsThread *thread, guint flags);
void ags_thread_set_flags(AgsThread *thread, guint flags);
void ags_thread_unset_flags(AgsThread *thread, guint flags);

gboolean ags_thread_test_status_flags(AgsThread *thread, guint status_flags);
void ags_thread_set_status_flags(AgsThread *thread, guint status_flags);
void ags_thread_unset_status_flags(AgsThread *thread, guint status_flags);

AgsThread* ags_thread_parent(AgsThread *thread);
AgsThread* ags_thread_next(AgsThread *thread);
AgsThread* ags_thread_prev(AgsThread *thread);
AgsThread* ags_thread_children(AgsThread *thread);

AgsThread* ags_thread_get_toplevel(AgsThread *thread);
AgsThread* ags_thread_first(AgsThread *thread);
AgsThread* ags_thread_last(AgsThread *thread);

void ags_thread_set_sync(AgsThread *thread, guint tic);
void ags_thread_set_sync_all(AgsThread *thread, guint tic);

void ags_thread_lock(AgsThread *thread);
gboolean ags_thread_trylock(AgsThread *thread);
void ags_thread_unlock(AgsThread *thread);

void ags_thread_remove_child(AgsThread *thread, AgsThread *child);
void ags_thread_add_child(AgsThread *thread, AgsThread *child);
void ags_thread_add_child_extended(AgsThread *thread, AgsThread *child,
				   gboolean no_start, gboolean no_wait);

gboolean ags_thread_parental_is_locked(AgsThread *thread, AgsThread *parent);
gboolean ags_thread_sibling_is_locked(AgsThread *thread);
gboolean ags_thread_children_is_locked(AgsThread *thread);

gboolean ags_thread_is_current_ready(AgsThread *current,
				     guint tic);
gboolean ags_thread_is_tree_ready(AgsThread *thread,
				  guint tic);

AgsThread* ags_thread_next_parent_locked(AgsThread *thread, AgsThread *parent);
AgsThread* ags_thread_next_sibling_locked(AgsThread *thread);
AgsThread* ags_thread_next_children_locked(AgsThread *thread);

void ags_thread_lock_parent(AgsThread *thread, AgsThread *parent);
void ags_thread_lock_sibling(AgsThread *thread);
void ags_thread_lock_children(AgsThread *thread);
void ags_thread_lock_all(AgsThread *thread);

void ags_thread_unlock_parent(AgsThread *thread, AgsThread *parent);
void ags_thread_unlock_sibling(AgsThread *thread);
void ags_thread_unlock_children(AgsThread *thread);
void ags_thread_unlock_all(AgsThread *thread);

void ags_thread_wait_parent(AgsThread *thread, AgsThread *parent);
void ags_thread_wait_sibling(AgsThread *thread);
void ags_thread_wait_children(AgsThread *thread);

void ags_thread_signal_parent(AgsThread *thread, AgsThread *parent, gboolean broadcast);
void ags_thread_signal_sibling(AgsThread *thread, gboolean broadcast);
void ags_thread_signal_children(AgsThread *thread, gboolean broadcast);

guint ags_thread_clock(AgsThread *thread);

void ags_thread_add_start_queue(AgsThread *thread,
				AgsThread *child);
void ags_thread_add_start_queue_all(AgsThread *thread,
				    GList *child);

gboolean ags_thread_is_running(AgsThread *thread);

void ags_thread_start(AgsThread *thread);
void ags_thread_run(AgsThread *thread);
void ags_thread_stop(AgsThread *thread);

void ags_thread_hangcheck(AgsThread *thread);

AgsThread* ags_thread_find_type(AgsThread *thread, GType type);
AgsThread* ags_thread_self(void);
AgsThread* ags_thread_chaos_tree(AgsThread *thread);

gboolean ags_thread_is_chaos_tree(AgsThread *thread,
				  guint tic_delay,
				  gboolean is_chaos_tree);

AgsThread* ags_thread_new(gpointer data);

G_END_DECLS

#endif /*__AGS_THREAD_H__*/
