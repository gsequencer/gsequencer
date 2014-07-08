/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_ASYNC_QUEUE_H__
#define __AGS_ASYNC_QUEUE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_ASYNC_QUEUE                (ags_async_queue_get_type())
#define AGS_ASYNC_QUEUE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueue))
#define AGS_ASYNC_QUEUE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueueClass))
#define AGS_IS_ASYNC_QUEUE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ASYNC_QUEUE))
#define AGS_IS_ASYNC_QUEUE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ASYNC_QUEUE))
#define AGS_ASYNC_QUEUE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_ASYNC_QUEUE, AgsAsyncQueueClass))

typedef struct _AgsAsyncQueue AgsAsyncQueue;
typedef struct _AgsAsyncQueueClass AgsAsyncQueueClass;

struct _AgsAsyncQueue
{
  GObject object;

  GQueue *stack;
};

struct _AgsAsyncQueueClass
{
  GObjectClass object;

  void (*interrupt)(AgsAsyncQueue *async_queue);
};

GType ags_async_queue_get_type();

void ags_async_queue_add(AgsAsyncQueue *queue, AgsStackable *stackable);
gboolean ags_async_queue_remove(AgsAsyncQueue *queue, AgsStackable *stackable);

void ags_async_queue_initerrupt(AgsAsyncQueue *async_queue);

AgsAsyncQueue* ags_async_queue_new();

#endif /*__AGS_ASYNC_QUEUE_H__*/
