/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

void ags_concurrency_provider_class_init(AgsConcurrencyProviderInterface *ginterface);

/**
 * SECTION:ags_concurrency_provider
 * @short_description: Concurrency provider interface
 * @title: AgsConcurrencyProvider
 * @section_id: AgsConcurrencyProvider
 * @include: ags/thread/ags_concurrency_provider.h
 *
 * The #AgsConcurrencyProvider gives you unique access to threads.
 */

GType
ags_concurrency_provider_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_concurrency_provider = 0;

    ags_type_concurrency_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
								  "AgsConcurrencyProvider",
								  sizeof(AgsConcurrencyProviderInterface),
								  (GClassInitFunc) ags_concurrency_provider_class_init,
								  0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_concurrency_provider);
  }

  return g_define_type_id__volatile;
}

void
ags_concurrency_provider_class_init(AgsConcurrencyProviderInterface *ginterface)
{
  /* empty */
}

/**
 * ags_concurrency_provider_get_main_loop:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get main loop of application context.
 *
 * Returns: (transfer full): the #AgsThread implementing #AgsMainLoop
 * 
 * Since: 3.0.0
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
 * ags_concurrency_provider_set_main_loop:
 * @concurrency_provider: the #AgsConcurrencyProvider
 * @main_loop: the #AgsThread implementing #AgsMainLoop
 * 
 * Set main loop of application context.
 * 
 * Since: 3.0.0
 */
void
ags_concurrency_provider_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
				       AgsThread *main_loop)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider));
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_if_fail(concurrency_provider_interface->set_main_loop);

  concurrency_provider_interface->set_main_loop(concurrency_provider,
						main_loop);
}

/**
 * ags_concurrency_provider_get_task_launcher:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get task launcher of application context.
 *
 * Returns: (transfer full): the #AgsTaskLauncher
 * 
 * Since: 3.0.0
 */
AgsTaskLauncher*
ags_concurrency_provider_get_task_launcher(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_task_launcher, NULL);

  return(concurrency_provider_interface->get_task_launcher(concurrency_provider));
}

/**
 * ags_concurrency_provider_set_task_launcher:
 * @concurrency_provider: the #AgsConcurrencyProvider
 * @task_launcher: the #AgsTaskLauncher
 * 
 * Set task launcher of application context.
 * 
 * Since: 3.0.0
 */
void
ags_concurrency_provider_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
					   AgsTaskLauncher *task_launcher)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider));
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_if_fail(concurrency_provider_interface->set_task_launcher);

  concurrency_provider_interface->set_task_launcher(concurrency_provider,
						    task_launcher);
}

/**
 * ags_concurrency_provider_get_thread_pool:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get thread pool of application context.
 *
 * Returns: (transfer full): the #AgsThreadPool
 * 
 * Since: 3.0.0
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
 * ags_concurrency_provider_set_thread_pool:
 * @concurrency_provider: the #AgsConcurrencyProvider
 * @thread_pool: the #AgsThreadPool
 * 
 * Set thread pool of application context.
 * 
 * Since: 3.0.0
 */
void
ags_concurrency_provider_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
					 AgsThreadPool *thread_pool)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider));
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_if_fail(concurrency_provider_interface->set_thread_pool);

  concurrency_provider_interface->set_thread_pool(concurrency_provider,
						  thread_pool);
}

/**
 * ags_concurrency_provider_get_worker:
 * @concurrency_provider: the #AgsConcurrencyProvider
 *
 * Get workers of application context.
 *
 * Returns: (element-type Ags.WorkerThread) (transfer full): the #GList-struct containing workers
 * 
 * Since: 3.0.0
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
 * @worker: (element-type Ags.WorkerThread): the #GList-struct containing workers
 * 
 * Set workers of application context.
 * 
 * Since: 3.0.0
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
