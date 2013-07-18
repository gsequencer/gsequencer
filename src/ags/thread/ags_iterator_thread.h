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

#ifndef __AGS_ITERATOR_THREAD_H__
#define __AGS_ITERATOR_THREAD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>

#define AGS_TYPE_ITERATOR_THREAD                (ags_iterator_thread_get_type())
#define AGS_ITERATOR_THREAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ITERATOR_THREAD, AgsIteratorThread))
#define AGS_ITERATOR_THREAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_ITERATOR_THREAD, AgsIteratorThread))
#define AGS_IS_ITERATOR_THREAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ITERATOR_THREAD))
#define AGS_IS_ITERATOR_THREAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ITERATOR_THREAD))
#define AGS_ITERATOR_THREAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_ITERATOR_THREAD, AgsIteratorThreadClass))

typedef struct _AgsIteratorThread AgsIteratorThread;
typedef struct _AgsIteratorThreadClass AgsIteratorThreadClass;

typedef enum{
  AGS_ITERATOR_THREAD_RUNNING   = 1,
  AGS_ITERATOR_THREAD_WAIT      = 1 << 1,
}AgsIteratorThreadFlags;

struct _AgsIteratorThread
{
  AgsThread thread;

  guint flags;

  AgsThread *recycling_thread;

  pthread_mutex_t iteration_mutex;
  pthread_cond_t iteration_cond;
};

struct _AgsIteratorThreadClass
{
  AgsThreadClass thread;
};

GType ags_iterator_thread_get_type();

AgsIteratorThread* ags_iterator_thread_new(GObject *iterator);

#endif /*__AGS_ITERATOR_THREAD_H__*/
