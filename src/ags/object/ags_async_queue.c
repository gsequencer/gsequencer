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

#include <ags/object/ags_async_queue.h>

#include <stdio.h>

void ags_async_queue_base_init(AgsAsyncQueueInterface *interface);

/**
 * SECTION:ags_async_queue
 * @short_description: asynchronous run
 * @title: AgsAsyncQueue
 * @section_id:
 * @include: ags/object/ags_async_queue.h
 *
 * The #AgsAsyncQueue interface determines if it is safe to run.
 */

GType
ags_async_queue_get_type()
{
  static GType ags_type_async_queue = 0;

  if(!ags_type_async_queue){
    static const GTypeInfo ags_async_queue_info = {
      sizeof(AgsAsyncQueueInterface),
      (GBaseInitFunc) ags_async_queue_base_init,
      NULL, /* base_finalize */
    };
    
    ags_type_async_queue = g_type_register_static(G_TYPE_INTERFACE,
						  "AgsAsyncQueue\0", &ags_async_queue_info,
						  0);
  }

  return(ags_type_async_queue);
}

void
ags_async_queue_base_init(AgsAsyncQueueInterface *interface)
{
  /* empty */
}

void
ags_async_queue_set_run_mutex(AgsAsyncQueue *async_queue, pthread_mutex_t *run_mutex)
{
  AgsAsyncQueueInterface *async_queue_interface;

  g_return_if_fail(AGS_IS_ASYNC_QUEUE(async_queue));
  async_queue_interface = AGS_ASYNC_QUEUE_GET_INTERFACE(async_queue);
  g_return_if_fail(async_queue_interface->set_run_mutex);
  async_queue_interface->set_run_mutex(async_queue, run_mutex);
}

pthread_mutex_t*
ags_async_queue_get_run_mutex(AgsAsyncQueue *async_queue)
{
  AgsAsyncQueueInterface *async_queue_interface;

  g_return_val_if_fail(AGS_IS_ASYNC_QUEUE(async_queue), NULL);
  async_queue_interface = AGS_ASYNC_QUEUE_GET_INTERFACE(async_queue);
  g_return_val_if_fail(async_queue_interface->get_run_mutex, NULL);
  async_queue_interface->get_run_mutex(async_queue);
}

void
ags_async_queue_set_run_cond(AgsAsyncQueue *async_queue, pthread_cond_t *run_cond)
{
  AgsAsyncQueueInterface *async_queue_interface;

  g_return_if_fail(AGS_IS_ASYNC_QUEUE(async_queue));
  async_queue_interface = AGS_ASYNC_QUEUE_GET_INTERFACE(async_queue);
  g_return_if_fail(async_queue_interface->set_run_cond);
  async_queue_interface->set_run_cond(async_queue, run_cond);
}

pthread_cond_t*
ags_async_queue_get_run_cond(AgsAsyncQueue *async_queue)
{
  AgsAsyncQueueInterface *async_queue_interface;

  g_return_val_if_fail(AGS_IS_ASYNC_QUEUE(async_queue), NULL);
  async_queue_interface = AGS_ASYNC_QUEUE_GET_INTERFACE(async_queue);
  g_return_val_if_fail(async_queue_interface->get_run_cond, NULL);
  async_queue_interface->get_run_cond(async_queue);
}

gboolean
ags_async_queue_is_run(AgsAsyncQueue *async_queue)
{
  AgsAsyncQueueInterface *async_queue_interface;

  g_return_val_if_fail(AGS_IS_ASYNC_QUEUE(async_queue), FALSE);
  async_queue_interface = AGS_ASYNC_QUEUE_GET_INTERFACE(async_queue);
  g_return_val_if_fail(async_queue_interface->is_run, FALSE);
  async_queue_interface->is_run(async_queue);
}

