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

#ifndef __AGS_THREAD_H__
#define __AGS_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#define _GNU_SOURCE

#include <signal.h>
#include <pthread.h>

#define AGS_TYPE_THREAD                (ags_thread_get_type())
#define AGS_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD, AgsThread))
#define AGS_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD, AgsThreadClass))
#define AGS_IS_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD))
#define AGS_IS_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD))
#define AGS_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD, AgsThreadClass))

#define AGS_ACCOUNTING_TABLE(ptr) ((AgsAccountingTable *)(ptr))

#define MSEC_PER_SEC    (1000000) /* The number of msecs per sec. */
#define NSEC_PER_SEC    (1000000000) /* The number of nsecs per sec. */
#define AGS_THREAD_RESUME_SIG SIGUSR2
#define AGS_THREAD_SUSPEND_SIG SIGUSR1
#define AGS_THREAD_DEFAULT_JIFFIE (250)
#define AGS_THREAD_MAX_PRECISION (1000)
#define AGS_THREAD_DEFAULT_ATTACK (1.0)

typedef struct _AgsThread AgsThread;
typedef struct _AgsThreadClass AgsThreadClass;
typedef struct _AgsAccountingTable AgsAccountingTable;

typedef enum{
  AGS_THREAD_RUNNING                 = 1,
  AGS_THREAD_IDLE                    = 1 << 1,
  AGS_THREAD_LOCKED                  = 1 << 2,
  AGS_THREAD_WAIT_FOR_PARENT         = 1 << 3,
  AGS_THREAD_WAIT_FOR_SIBLING        = 1 << 4,
  AGS_THREAD_WAIT_FOR_CHILDREN       = 1 << 5,
  AGS_THREAD_WAIT_FOR_BARRIER        = 1 << 6,
  AGS_THREAD_WAITING_FOR_PARENT      = 1 << 7,
  AGS_THREAD_WAITING_FOR_SIBLING     = 1 << 8,
  AGS_THREAD_WAITING_FOR_CHILDREN    = 1 << 9,
  AGS_THREAD_WAITING_FOR_BARRIER     = 1 << 10,
  AGS_THREAD_BROADCAST_PARENT        = 1 << 11,
  AGS_THREAD_BROADCAST_SIBLING       = 1 << 12,
  AGS_THREAD_BROADCAST_CHILDREN      = 1 << 13,
  AGS_THREAD_INITIAL_RUN             = 1 << 14,
  AGS_THREAD_TREE_SYNC_0             = 1 << 15,
  AGS_THREAD_WAIT_0                  = 1 << 16,
  AGS_THREAD_TREE_SYNC_1             = 1 << 17,
  AGS_THREAD_WAIT_1                  = 1 << 18,
  AGS_THREAD_TREE_SYNC_2             = 1 << 19,
  AGS_THREAD_WAIT_2                  = 1 << 20,
  AGS_THREAD_TIMELOCK_RUN            = 1 << 21,
  AGS_THREAD_TIMELOCK_WAIT           = 1 << 22,
  AGS_THREAD_TIMELOCK_RESUME         = 1 << 23,
  /*
   * prefered way would be unlocking greedy_locks
   * and the suspend to not become greedy
   * but while pthread_suspend and pthread_resume
   * are missing you need this as work-around
   */
  AGS_THREAD_SKIP_NON_GREEDY         = 1 << 24,
  AGS_THREAD_SKIPPED_BY_TIMELOCK     = 1 << 25,
  AGS_THREAD_LOCK_GREEDY_RUN_MUTEX   = 1 << 26,
  AGS_THREAD_SUSPENDED               = 1 << 27,
  AGS_THREAD_SINGLE_LOOP             = 1 << 28,
  AGS_THREAD_READY                   = 1 << 29,
  AGS_THREAD_UNREF_ON_EXIT           = 1 << 30,
  AGS_THREAD_CONNECTED               = 1 << 31,
}AgsThreadFlags;

struct _AgsThread
{
  GObject object;

  volatile guint flags;

  sigset_t wait_mask;

  GObject *devout;
  
  pthread_t *thread;
  pthread_attr_t thread_attr;

  gdouble freq;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t mutexattr;
  pthread_cond_t *cond;

  pthread_mutex_t *start_mutex;
  pthread_cond_t *start_cond;

  pthread_barrier_t **barrier;
  gboolean first_barrier;
  int wait_count[2];

  pthread_t timelock_thread;
  pthread_mutex_t *timelock_mutex;
  pthread_cond_t *timelock_cond;

  pthread_mutex_t *greedy_mutex;
  pthread_cond_t *greedy_cond;
  pthread_mutex_t *greedy_run_mutex;
  volatile guint locked_greedy;

  struct timespec timelock;
  GList *greedy_locks;

  pthread_mutex_t *suspend_mutex;
  volatile gboolean critical_region;

  AgsThread *parent;

  AgsThread *next;
  AgsThread *prev;

  AgsThread *children;

  gpointer data;
};

struct _AgsThreadClass
{
  GObjectClass object;

  void (*start)(AgsThread *thread);
  void (*run)(AgsThread *thread);
  void (*suspend)(AgsThread *thread);
  void (*resume)(AgsThread *thread);
  void (*timelock)(AgsThread *thread);
  void (*stop)(AgsThread *thread);
};

struct _AgsAccountingTable
{
  AgsThread *thread;
  gdouble sanity;
};

GType ags_thread_get_type();

AgsAccountingTable* ags_accounting_table_alloc(AgsThread *thread);
void ags_accounting_table_set_sanity(GList *table,
				     AgsThread *thread, gdouble sanity);

void ags_thread_set_sync(AgsThread *thread, guint tic);
void ags_thread_set_sync_all(AgsThread *thread, guint tic);

void ags_thread_lock(AgsThread *thread);
gboolean ags_thread_trylock(AgsThread *thread);
void ags_thread_unlock(AgsThread *thread);

AgsThread* ags_thread_get_toplevel(AgsThread *thread);
AgsThread* ags_thread_first(AgsThread *thread);
AgsThread* ags_thread_last(AgsThread *thread);

void ags_thread_remove_child(AgsThread *thread, AgsThread *child);
void ags_thread_add_child(AgsThread *thread, AgsThread *child);
void ags_thread_add_child_extended(AgsThread *thread, AgsThread *child,
				   gboolean no_start, gboolean no_Wait);

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

void ags_thread_start(AgsThread *thread);
void ags_thread_run(AgsThread *thread);
void ags_thread_suspend(AgsThread *thread);
void ags_thread_resume(AgsThread *thread);
void ags_thread_timelock(AgsThread *thread);
void ags_thread_stop(AgsThread *thread);

void ags_thread_hangcheck(AgsThread *thread);

AgsThread* ags_thread_find_type(AgsThread *thread, GType type);

AgsThread* ags_thread_new(gpointer data);

#endif /*__AGS_THREAD_H__*/
