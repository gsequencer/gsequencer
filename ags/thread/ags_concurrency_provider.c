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

#include <ags/thread/ags_concurrency_provider.h>

#include <math.h>

void ags_concurrency_provider_class_init(AgsConcurrencyProviderInterface *interface);

/**
 * SECTION:ags_concurrency_provider
 * @short_description: concurrency interface
 * @title: AgsConcurrencyProvider
 * @section_id: AgsConcurrencyProvider
 * @include: ags/thread/ags_concurrency_provider.h
 *
 * The #AgsConcurrencyProvider gives you unique access to threads.
 */

GType
ags_concurrency_provider_get_type()
{
  static GType ags_type_concurrency_provider = 0;

  if(!ags_type_concurrency_provider){
    ags_type_concurrency_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
								  "AgsConcurrencyProvider",
								  sizeof(AgsConcurrencyProviderInterface),
								  (GClassInitFunc) ags_concurrency_provider_class_init,
								  0, NULL, 0);
  }

  return(ags_type_concurrency_provider);
}

void
ags_concurrency_provider_class_init(AgsConcurrencyProviderInterface *interface)
{
  /* empty */
}

/**
 * ags_concurrency_provider_get_mutex_manager:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get mutex manager of application context.
 *
 * Returns: the assigned #AgsMutexManager
 *
 * Since: 0.7.0
 */
AgsMutexManager*
ags_concurrency_provider_get_mutex_manager(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_mutex_manager, NULL);

  return(concurrency_provider_interface->get_mutex_manager(concurrency_provider));
}

/**
 * ags_concurrency_provider_get_main_loop:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get main loop of application context.
 *
 * Returns: the #AgsThread implementing #AgsMainLoop interface
 *
 * Since: 0.7.0
 */
AgsThread*
ags_concurrency_provider_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_main_loop, NULL);

  return(concurrency_provider_interface->get_main_loop(concurrency_provider));
}

/**
 * ags_concurrency_provider_get_task_thread:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get task thread of application context.
 *
 * Returns: the #AgsThread implementing #AgsAsyncQueue interface
 * 
 * Since: 0.7.0
 */
AgsThread*
ags_concurrency_provider_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_task_thread, NULL);

  return(concurrency_provider_interface->get_task_thread(concurrency_provider));
}

/**
 * ags_concurrency_provider_get_thread_pool:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get thread pool of application context.
 *
 * Returns: the #AgsThreadPool
 * 
 * Since: 0.7.0
 */
AgsThreadPool*
ags_concurrency_provider_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_thread_pool, NULL);

  return(concurrency_provider_interface->get_thread_pool(concurrency_provider));
}

/**
 * ags_concurrency_provider_get_worker:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get workers of application context.
 *
 * Returns: the #GList-struct containing workers
 * 
 * Since: 0.7.122.8
 */
GList*
ags_concurrency_provider_get_worker(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_worker, NULL);

  return(concurrency_provider_interface->get_worker(concurrency_provider));
}

/**
 * ags_concurrency_provider_set_worker:
 * @concurrency_provider: the #AgsConcurrencyProvider
 * @worker: the #GList-struct containing workers
 * 
 * Set workers of application context.
 * 
 * Since: 0.7.122.8
 */
void
ags_concurrency_provider_set_worker(AgsConcurrencyProvider *concurrency_provider,
				    GList *worker)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider));
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_if_fail(concurrency_provider_interface->set_worker);

  concurrency_provider_interface->set_worker(concurrency_provider,
					     worker);
}
