/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#include <ags/thread/ags_concurrency_provider.h>

#include <math.h>

void ags_concurrency_provider_class_init(AgsConcurrencyProviderInterface *interface);

GType
ags_concurrency_provider_get_type()
{
  static GType ags_type_concurrency_provider = 0;

  if(!ags_type_concurrency_provider){
    ags_type_concurrency_provider = g_type_register_static_simple(G_TYPE_INTERFACE,
								  "AgsConcurrencyProvider\0",
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

AgsThread*
ags_concurrency_provider_get_main_loop(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_main_loop, NULL);

  return(concurrency_provider_interface->get_main_loop(concurrency_provider));
}

AgsThread*
ags_concurrency_provider_get_task_thread(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_task_thread, NULL);

  return(concurrency_provider_interface->get_task_thread(concurrency_provider));
}

AgsThreadPool*
ags_concurrency_provider_get_thread_pool(AgsConcurrencyProvider *concurrency_provider)
{
  AgsConcurrencyProviderInterface *concurrency_provider_interface;

  g_return_val_if_fail(AGS_IS_CONCURRENCY_PROVIDER(concurrency_provider), NULL);
  concurrency_provider_interface = AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(concurrency_provider);
  g_return_val_if_fail(concurrency_provider_interface->get_thread_pool, NULL);

  return(concurrency_provider_interface->get_thread_pool(concurrency_provider));
}

