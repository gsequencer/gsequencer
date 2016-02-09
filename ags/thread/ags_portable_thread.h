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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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

#ifndef __AGS_PORTABLE_THREAD_H__
#define __AGS_PORTABLE_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#define _GNU_SOURCE

#include <pth.h>

#define AGS_TYPE_PORTABLE_THREAD                (ags_portable_thread_get_type())
#define AGS_PORTABLE_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD, AgsPortableThread))
#define AGS_PORTABLE_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD, AgsPortableThreadClass))
#define AGS_IS_PORTABLE_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD))
#define AGS_IS_PORTABLE_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD))
#define AGS_PORTABLE_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD, AgsPortableThreadClass))

typedef struct _AgsPortableThread AgsPortableThread;
typedef struct _AgsPortableThreadClass AgsPortableThreadClass;

typedef enum{
  AGS_PORTABLE_THREAD_RUNNING                 = 1,
  AGS_PORTABLE_THREAD_IDLE                    = 1 << 1,
  AGS_PORTABLE_THREAD_LOCKED                  = 1 << 2,
  AGS_PORTABLE_THREAD_WAIT_FOR_PARENT         = 1 << 3,
  AGS_PORTABLE_THREAD_WAIT_FOR_SIBLING        = 1 << 4,
  AGS_PORTABLE_THREAD_WAIT_FOR_CHILDREN       = 1 << 5,
  AGS_PORTABLE_THREAD_WAIT_FOR_BARRIER        = 1 << 6,
  AGS_PORTABLE_THREAD_WAITING_FOR_PARENT      = 1 << 7,
  AGS_PORTABLE_THREAD_WAITING_FOR_SIBLING     = 1 << 8,
  AGS_PORTABLE_THREAD_WAITING_FOR_CHILDREN    = 1 << 9,
  AGS_PORTABLE_THREAD_WAITING_FOR_BARRIER     = 1 << 10,
  AGS_PORTABLE_THREAD_BROADCAST_PARENT        = 1 << 11,
  AGS_PORTABLE_THREAD_BROADCAST_SIBLING       = 1 << 12,
  AGS_PORTABLE_THREAD_BROADCAST_CHILDREN      = 1 << 13,
  AGS_PORTABLE_THREAD_INITIAL_RUN             = 1 << 14,
  AGS_PORTABLE_THREAD_TREE_SYNC_0             = 1 << 15,
  AGS_PORTABLE_THREAD_WAIT_0                  = 1 << 16,
  AGS_PORTABLE_THREAD_TREE_SYNC_1             = 1 << 17,
  AGS_PORTABLE_THREAD_WAIT_1                  = 1 << 18,
  AGS_PORTABLE_THREAD_TREE_SYNC_2             = 1 << 19,
  AGS_PORTABLE_THREAD_WAIT_2                  = 1 << 20,
  AGS_PORTABLE_THREAD_TIMELOCK_RUN            = 1 << 21,
  AGS_PORTABLE_THREAD_TIMELOCK_WAIT           = 1 << 22,
  AGS_PORTABLE_THREAD_TIMELOCK_RESUME         = 1 << 23,
}AgsPortableThreadFlags;

struct _AgsPortableThread
{
  GObject object;

  volatile guint flags;

  pth_t thread;
  pth_attr_t thread_attr;

  pth_mutex_t mutex;
  pth_cond_t cond;

  pth_mutex_t start_mutex;
  pth_cond_t start_cond;

  pth_barrier_t barrier[2];
  gboolean first_barrier;
  int wait_count[2];

  pth_t timelock_thread;
  pth_mutex_t timelock_mutex;
  pth_cond_t timelock_cond;

  long timelock;

  GObject *devout;
  AgsPortableThread *parent;

  AgsPortableThread *next;
  AgsPortableThread *prev;

  AgsPortableThread *children;

  GObject *data;
};

struct _AgsPortableThreadClass
{
  GObjectClass object;

  void (*start)(AgsPortableThread *thread);
  void (*run)(AgsPortableThread *thread);
  void (*timelock)(AgsPortableThread *thread);
  void (*stop)(AgsPortableThread *thread);
};

GType ags_portable_thread_get_type();

void ags_portable_thread_lock(AgsPortableThread *thread);
gboolean ags_portable_thread_trylock(AgsPortableThread *thread);
void ags_portable_thread_unlock(AgsPortableThread *thread);

AgsPortableThread* ags_portable_thread_get_toplevel(AgsPortableThread *thread);
AgsPortableThread* ags_portable_thread_first(AgsPortableThread *thread);
AgsPortableThread* ags_portable_thread_last(AgsPortableThread *thread);

void ags_portable_thread_remove_child(AgsPortableThread *thread, AgsPortableThread *child);
void ags_portable_thread_add_child(AgsPortableThread *thread, AgsPortableThread *child);

gboolean ags_portable_thread_parental_is_locked(AgsPortableThread *thread, AgsPortableThread *parent);
gboolean ags_portable_thread_sibling_is_locked(AgsPortableThread *thread);
gboolean ags_portable_thread_children_is_locked(AgsPortableThread *thread);

gboolean ags_portable_thread_is_current_ready(AgsPortableThread *current);
gboolean ags_portable_thread_is_current_synced(AgsPortableThread *current);
gboolean ags_portable_thread_is_tree_ready(AgsPortableThread *thread);
gboolean ags_portable_thread_is_tree_synced(AgsPortableThread *thread);
void ags_portable_thread_main_loop_unlock_children(AgsPortableThread *thread);

AgsPortableThread* ags_portable_thread_next_parent_locked(AgsPortableThread *thread, AgsPortableThread *parent);
AgsPortableThread* ags_portable_thread_next_sibling_locked(AgsPortableThread *thread);
AgsPortableThread* ags_portable_thread_next_children_locked(AgsPortableThread *thread);

void ags_portable_thread_lock_parent(AgsPortableThread *thread, AgsPortableThread *parent);
void ags_portable_thread_lock_sibling(AgsPortableThread *thread);
void ags_portable_thread_lock_children(AgsPortableThread *thread);
void ags_portable_thread_lock_all(AgsPortableThread *thread);

void ags_portable_thread_unlock_parent(AgsPortableThread *thread, AgsPortableThread *parent);
void ags_portable_thread_unlock_sibling(AgsPortableThread *thread);
void ags_portable_thread_unlock_children(AgsPortableThread *thread);
void ags_portable_thread_unlock_all(AgsPortableThread *thread);

void ags_portable_thread_wait_parent(AgsPortableThread *thread, AgsPortableThread *parent);
void ags_portable_thread_wait_sibling(AgsPortableThread *thread);
void ags_portable_thread_wait_children(AgsPortableThread *thread);

void ags_portable_thread_signal_parent(AgsPortableThread *thread, AgsPortableThread *parent, gboolean broadcast);
void ags_portable_thread_signal_sibling(AgsPortableThread *thread, gboolean broadcast);
void ags_portable_thread_signal_children(AgsPortableThread *thread, gboolean broadcast);

void ags_portable_thread_start(AgsPortableThread *thread);
void ags_portable_thread_run(AgsPortableThread *thread);
void ags_portable_thread_timelock(AgsPortableThread *thread);
void ags_portable_thread_stop(AgsPortableThread *thread);

AgsPortableThread* ags_portable_thread_new(GObject *data);

#endif /*__AGS_PORTABLE_THREAD_H__*/
