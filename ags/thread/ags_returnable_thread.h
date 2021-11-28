/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_RETURNABLE_THREAD_H__
#define __AGS_RETURNABLE_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

G_BEGIN_DECLS

#define AGS_TYPE_RETURNABLE_THREAD                (ags_returnable_thread_get_type())
#define AGS_TYPE_RETURNABLE_THREAD_FLAGS          (ags_returnable_thread_flags_get_type())
#define AGS_RETURNABLE_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RETURNABLE_THREAD, AgsReturnableThread))
#define AGS_RETURNABLE_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_RETURNABLE_THREAD, AgsReturnableThreadClass))
#define AGS_IS_RETURNABLE_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_RETURNABLE_THREAD))
#define AGS_IS_RETURNABLE_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_RETURNABLE_THREAD))
#define AGS_RETURNABLE_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_RETURNABLE_THREAD, AgsReturnableThreadClass))

#define AGS_RETURNABLE_THREAD_GET_RESET_MUTEX(obj) (&(((AgsReturnableThread *) obj)->reset_mutex))

#define AGS_RETURNABLE_THREAD_DEFAULT_JIFFIE (AGS_THREAD_DEFAULT_MAX_PRECISION)

typedef struct _AgsReturnableThread AgsReturnableThread;
typedef struct _AgsReturnableThreadClass AgsReturnableThreadClass;

typedef void (*AgsReturnableThreadCallback)(AgsReturnableThread *returnable_thread, gpointer data);

/**
 * AgsReturnableThreadFlags:
 * @AGS_RETURNABLE_THREAD_IN_USE: the thread is in use
 * @AGS_RETURNABLE_THREAD_RESET: not used
 * @AGS_RETURNABLE_THREAD_RUN_ONCE: call #AgsThread::run() only one time
 * 
 * Enum values to control the behavior or indicate internal state of #AgsReturnableThread by
 * enable/disable as flags.
 */
typedef enum{
  AGS_RETURNABLE_THREAD_IN_USE              = 1,
  AGS_RETURNABLE_THREAD_RESET               = 1 << 1,
  AGS_RETURNABLE_THREAD_RUN_ONCE            = 1 << 2,
}AgsReturnableThreadFlags;

struct _AgsReturnableThread
{
  AgsThread thread;

  volatile guint flags;

  GObject *thread_pool;
  
  GRecMutex reset_mutex;
  volatile void *safe_data;

  gulong handler;
};

struct _AgsReturnableThreadClass
{
  AgsThreadClass thread;

  void (*safe_run)(AgsReturnableThread *returnable_thread);
};

GType ags_returnable_thread_get_type();
GType ags_returnable_thread_flags_get_type();

gboolean ags_returnable_thread_test_flags(AgsReturnableThread *returnable_thread, guint flags);
void ags_returnable_thread_set_flags(AgsReturnableThread *returnable_thread, guint flags);
void ags_returnable_thread_unset_flags(AgsReturnableThread *returnable_thread, guint flags);

void ags_returnable_thread_safe_run(AgsReturnableThread *returnable_thread);

void ags_returnable_thread_connect_safe_run(AgsReturnableThread *returnable_thread, AgsReturnableThreadCallback callback);
void ags_returnable_thread_disconnect_safe_run(AgsReturnableThread *returnable_thread);

AgsReturnableThread* ags_returnable_thread_new(GObject *thread_pool);

G_END_DECLS

#endif /*__AGS_RETURNABLE_THREAD_H__*/
