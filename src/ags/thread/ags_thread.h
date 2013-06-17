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

#ifndef __AGS_THREAD_H__
#define __AGS_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#define __USE_GNU
#define __USE_UNIX98
#include <pthread.h>

#define AGS_TYPE_THREAD                (ags_thread_get_type())
#define AGS_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_THREAD, AgsThread))
#define AGS_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_THREAD, AgsThread))
#define AGS_IS_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_THREAD))
#define AGS_IS_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_THREAD))
#define AGS_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_THREAD, AgsThreadClass))

typedef struct _AgsThread AgsThread;
typedef struct _AgsThreadClass AgsThreadClass;

typedef enum{
  AGS_THREAD_RUNNING                 = 1,
  AGS_THREAD_IDLE                    = 1 << 1,
  AGS_THREAD_LOCKED                  = 1 << 2,
  AGS_THREAD_WAITING_FOR_PARENT      = 1 << 3,
  AGS_THREAD_WAITING_FOR_SIBLING     = 1 << 4,
  AGS_THREAD_WAITING_FOR_CHILDREN    = 1 << 5,
}AgsThreadFlags;

struct _AgsThread
{
  GObject object;

  guint flags;

  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_cond_t cond;

  AgsThread *parent;

  AgsThread *next;
  AgsThread *prev;

  AgsThread *children;

  GObject *data;
};

struct _AgsThreadClass
{
  GObjectClass object;

  void (*run)(AgsThread *thread);
};

GType ags_thread_get_type();

void ags_thread_start(AgsThread *thread);
void ags_thread_run(AgsThread *thread);

AgsThread* ags_thread_new(GObject *data);

#endif /*__AGS_THREAD_H__*/
