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
 * @AGS_THREAD_MARK_SYNCED: mark thread synced
 * @AGS_THREAD_TIME_ACCOUNTING: time accounting causes to track time
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
  AGS_THREAD_TIME_ACCOUNTING         = 1 <<  8,
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
 * @AGS_THREAD_STATUS_BUSY: the thread is busy
 * @AGS_THREAD_STATUS_SYNCED: the thread joined the tic based system, it is synced
 * @AGS_THREAD_STATUS_SYNCED_FREQ: the frequency was synced
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
  AGS_THREAD_STATUS_BUSY                    = 1 << 10,
  AGS_THREAD_STATUS_SYNCED                  = 1 << 11,
  AGS_THREAD_STATUS_SYNCED_FREQ             = 1 << 12,
}AgsThreadStatusFlags;

/**
 * AgsThreadSyncTicFlags:
 * @AGS_THREAD_SYNC_TIC_WAIT_0: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_0: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_1: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_1: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_2: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_2: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_3: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_3: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_4: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_4: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_5: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_5: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_6: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_6: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_7: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_7: done tree to be synced
 * @AGS_THREAD_SYNC_TIC_WAIT_8: wait tree to be synced
 * @AGS_THREAD_SYNC_TIC_DONE_8: done tree to be synced
 *
 * Enum values to control the behavior or indicate internal state of #AgsThread by
 * enable/disable as sync tic flags.
 */
typedef enum{
  AGS_THREAD_SYNC_TIC_WAIT_0                  = 1,
  AGS_THREAD_SYNC_TIC_DONE_0                  = 1 <<  1,
  AGS_THREAD_SYNC_TIC_WAIT_1                  = 1 <<  2,
  AGS_THREAD_SYNC_TIC_DONE_1                  = 1 <<  3,
  AGS_THREAD_SYNC_TIC_WAIT_2                  = 1 <<  4,
  AGS_THREAD_SYNC_TIC_DONE_2                  = 1 <<  5,
  AGS_THREAD_SYNC_TIC_WAIT_3                  = 1 <<  6,
  AGS_THREAD_SYNC_TIC_DONE_3                  = 1 <<  7,
  AGS_THREAD_SYNC_TIC_WAIT_4                  = 1 <<  8,
  AGS_THREAD_SYNC_TIC_DONE_4                  = 1 <<  9,
  AGS_THREAD_SYNC_TIC_WAIT_5                  = 1 << 10,
  AGS_THREAD_SYNC_TIC_DONE_5                  = 1 << 11,
  AGS_THREAD_SYNC_TIC_WAIT_6                  = 1 << 12,
  AGS_THREAD_SYNC_TIC_DONE_6                  = 1 << 13,
  AGS_THREAD_SYNC_TIC_WAIT_7                  = 1 << 14,
  AGS_THREAD_SYNC_TIC_DONE_7                  = 1 << 15,
  AGS_THREAD_SYNC_TIC_WAIT_8                  = 1 << 16,
  AGS_THREAD_SYNC_TIC_DONE_8                  = 1 << 17,
}AgsThreadSyncTicFlags;

struct _AgsThread
{
  GObject gobject;

  guint my_flags;
  volatile guint status_flags;
  volatile guint sync_tic_flags;
  
  GRecMutex obj_mutex;

  AgsUUID *uuid;

  volatile guint current_sync_tic;

  gdouble delay;
  gdouble tic_delay;
  
  gdouble frequency;
  gdouble max_precision;

  gint64 last_run_start;
  gint64 last_run_end;
  
  GThread *thread;

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
void ags_thread_clear_status_flags(AgsThread *thread);

gboolean ags_thread_test_sync_tic_flags(AgsThread *thread, guint sync_tic_flags);
void ags_thread_set_sync_tic_flags(AgsThread *thread, guint sync_tic_flags);
void ags_thread_unset_sync_tic_flags(AgsThread *thread, guint sync_tic_flags);
void ags_thread_clear_sync_tic_flags(AgsThread *thread);

void ags_thread_set_current_sync_tic(AgsThread *thread, guint current_sync_tic);
guint ags_thread_get_current_sync_tic(AgsThread *thread);

void ags_thread_set_delay(AgsThread *thread, gdouble delay);
gdouble ags_thread_get_delay(AgsThread *thread);

void ags_thread_set_frequency(AgsThread *thread, gdouble frequency);
gdouble ags_thread_get_frequency(AgsThread *thread);

void ags_thread_set_max_precision(AgsThread *thread, gdouble max_precision);
gdouble ags_thread_get_max_precision(AgsThread *thread);

AgsThread* ags_thread_find_type(AgsThread *thread, GType gtype);
AgsThread* ags_thread_self(void);

AgsThread* ags_thread_parent(AgsThread *thread);
AgsThread* ags_thread_next(AgsThread *thread);
AgsThread* ags_thread_prev(AgsThread *thread);
AgsThread* ags_thread_children(AgsThread *thread);

AgsThread* ags_thread_get_toplevel(AgsThread *thread);
AgsThread* ags_thread_first(AgsThread *thread);
AgsThread* ags_thread_last(AgsThread *thread);

void ags_thread_lock(AgsThread *thread);
gboolean ags_thread_trylock(AgsThread *thread);
void ags_thread_unlock(AgsThread *thread);

void ags_thread_remove_child(AgsThread *thread, AgsThread *child);
void ags_thread_add_child(AgsThread *thread, AgsThread *child);
void ags_thread_add_child_extended(AgsThread *thread, AgsThread *child,
				   gboolean no_start, gboolean no_wait);

gboolean ags_thread_is_current_ready(AgsThread *current, guint current_sync_tic);
gboolean ags_thread_is_tree_ready_recursive(AgsThread *thread, guint current_sync_tic);

void ags_thread_prepare_current_sync(AgsThread *current, guint current_sync_tic);
void ags_thread_prepare_tree_sync_recursive(AgsThread *thread, guint current_sync_tic);

void ags_thread_set_current_sync(AgsThread *current, guint current_sync_tic);
void ags_thread_set_tree_sync_recursive(AgsThread *thread, guint current_sync_tic);

guint ags_thread_clock(AgsThread *thread);

void ags_thread_add_start_queue(AgsThread *thread,
				AgsThread *child);
void ags_thread_add_start_queue_all(AgsThread *thread,
				    GList *child);

void ags_thread_start(AgsThread *thread);
void ags_thread_run(AgsThread *thread);
void ags_thread_stop(AgsThread *thread);

AgsThread* ags_thread_new();

G_END_DECLS

#endif /*__AGS_THREAD_H__*/
