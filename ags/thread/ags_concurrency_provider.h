/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_CONCURRENCY_PROVIDER_H__
#define __AGS_CONCURRENCY_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_thread.h>
#include <ags/thread/ags_task_launcher.h>
#include <ags/thread/ags_thread_pool.h>

G_BEGIN_DECLS

#define AGS_TYPE_CONCURRENCY_PROVIDER                    (ags_concurrency_provider_get_type())
#define AGS_CONCURRENCY_PROVIDER(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONCURRENCY_PROVIDER, AgsConcurrencyProvider))
#define AGS_CONCURRENCY_PROVIDER_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_CONCURRENCY_PROVIDER, AgsConcurrencyProviderInterface))
#define AGS_IS_CONCURRENCY_PROVIDER(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONCURRENCY_PROVIDER))
#define AGS_IS_CONCURRENCY_PROVIDER_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_CONCURRENCY_PROVIDER))
#define AGS_CONCURRENCY_PROVIDER_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_CONCURRENCY_PROVIDER, AgsConcurrencyProviderInterface))

typedef struct _AgsConcurrencyProvider AgsConcurrencyProvider;
typedef struct _AgsConcurrencyProviderInterface AgsConcurrencyProviderInterface;

struct _AgsConcurrencyProviderInterface
{
  GTypeInterface ginterface;

  AgsThread* (*get_main_loop)(AgsConcurrencyProvider *concurrency_provider);
  void (*set_main_loop)(AgsConcurrencyProvider *concurrency_provider,
			AgsThread *main_loop);

  AgsTaskLauncher* (*get_task_launcher)(AgsConcurrencyProvider *concurrency_provider);
  void (*set_task_launcher)(AgsConcurrencyProvider *concurrency_provider,
			    AgsTaskLauncher *task_launcher);

  AgsThreadPool* (*get_thread_pool)(AgsConcurrencyProvider *concurrency_provider);
  void (*set_thread_pool)(AgsConcurrencyProvider *concurrency_provider,
			  AgsThreadPool *thread_pool);
  
  GList* (*get_worker)(AgsConcurrencyProvider *concurrency_provider);
  void (*set_worker)(AgsConcurrencyProvider *concurrency_provider,
		     GList *worker);
};

GType ags_concurrency_provider_get_type();

AgsThread* ags_concurrency_provider_get_main_loop(AgsConcurrencyProvider *concurrency_provider);
void ags_concurrency_provider_set_main_loop(AgsConcurrencyProvider *concurrency_provider,
					    AgsThread *main_loop);

AgsTaskLauncher* ags_concurrency_provider_get_task_launcher(AgsConcurrencyProvider *concurrency_provider);
void ags_concurrency_provider_set_task_launcher(AgsConcurrencyProvider *concurrency_provider,
						AgsTaskLauncher *task_launcher);

AgsThreadPool* ags_concurrency_provider_get_thread_pool(AgsConcurrencyProvider *concurrency_provider);
void ags_concurrency_provider_set_thread_pool(AgsConcurrencyProvider *concurrency_provider,
					      AgsThreadPool *thread_pool);

GList* ags_concurrency_provider_get_worker(AgsConcurrencyProvider *concurrency_provider);
void ags_concurrency_provider_set_worker(AgsConcurrencyProvider *concurrency_provider,
					 GList *worker);

G_END_DECLS

#endif /*__AGS_CONCURRENCY_PROVIDER_H__*/
