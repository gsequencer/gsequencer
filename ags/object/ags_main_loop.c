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

#include <ags/object/ags_main_loop.h>

#include <ags/object/ags_marshal.h>

#include <stdio.h>

void ags_main_loop_class_init(AgsMainLoopInterface *interface);

/**
 * SECTION:ags_main_loop
 * @short_description: toplevel threads
 * @title: AgsMainLoop
 * @section_id: AgsMainLoop
 * @include: ags/object/ags_main_loop.h
 *
 * The #AgsMainLoop interface gives you a unique access to toplevel
 * threads.
 */

enum {
  INTERRUPT,
  MONITOR,
  CHANGE_FREQUENCY,
  LAST_SIGNAL,
};

static guint main_loop_signals[LAST_SIGNAL];

GType
ags_main_loop_get_type()
{
  static GType main_loop_type = 0;

  if(!main_loop_type){
    main_loop_type = g_type_register_static_simple(G_TYPE_INTERFACE,
						   "AgsMainLoop",
						   sizeof (AgsMainLoopInterface),
						   (GClassInitFunc) ags_main_loop_class_init,
						   0, NULL, 0);
  }
  
  return(main_loop_type);
}

void
ags_main_loop_class_init(AgsMainLoopInterface *interface)
{
  /**
   * AgsMainLoop::interrupt:
   * @main_loop: the #AgsMainLoop
   * @sig: the signal number
   * @time_cycle: the amount of time of a cycle
   * @time_spent: the amount of time passed since last cycle
   *
   * Notify about interrupt threads.
   *
   * Since: 1.0.0
   */
  main_loop_signals[INTERRUPT] = 
    g_signal_new("interrupt",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMainLoopInterface, interrupt),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__INT_UINT_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_INT, G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * AgsMainLoop::monitor:
   * @main_loop: the #AgsMainLoop
   * @time_cycle: the amount of time of a cycle
   * @time_spent: the amount of time passed since last cycle
   *
   * Notify to monitor time.
   *
   * Returns: if monitor is allowed
   * 
   * Since: 1.0.0
   */
  main_loop_signals[MONITOR] =
    g_signal_new("monitor",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMainLoopInterface, monitor),
		 NULL, NULL,
		 g_cclosure_user_marshal_BOOLEAN__UINT_POINTER,
		 G_TYPE_BOOLEAN, 2,
		 G_TYPE_UINT, G_TYPE_POINTER);

  /**
   * AgsMainLoop::change-frequency:
   * @main_loop: the #AgsMainLoop
   * @frequency: the new frequency
   *
   * Change frequency.
   * 
   * Since: 1.0.0
   */
  main_loop_signals[CHANGE_FREQUENCY] =
    g_signal_new("change-frequency",
		 G_TYPE_FROM_INTERFACE(interface),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMainLoopInterface, change_frequency),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__DOUBLE,
		 G_TYPE_NONE, 1,
		 G_TYPE_DOUBLE);
}

/**
 * ags_main_loop_get_tree_lock:
 * @main_loop: the #AgsMainLoop
 *
 * Retrieve the tree mutex.
 *
 * Returns: the mutex
 *
 * Since: 1.0.0
 */
pthread_mutex_t*
ags_main_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), NULL);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_tree_lock, NULL);

  return(main_loop_interface->get_tree_lock(main_loop));
}

/**
 * ags_main_loop_set_application_context:
 * @main_loop: the #AgsMainLoop
 * @application_context: the #AgsApplicationContext
 *
 * Sets the application context.
 *
 * Since: 1.0.0
 */
void
ags_main_loop_set_application_context(AgsMainLoop *main_loop, AgsApplicationContext *application_context)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->set_application_context);
  main_loop_interface->set_application_context(main_loop, application_context);
}

/**
 * ags_main_loop_get_application_context:
 * @main_loop: the #AgsMainLoop
 *
 * Retrieve the #AgsApplicationContext.
 *
 * Returns: the #AgsApplicationContext
 *
 * Since: 1.0.0
 */
AgsApplicationContext*
ags_main_loop_get_application_context(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), NULL);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_application_context, NULL);
  return(main_loop_interface->get_application_context(main_loop));
}

/**
 * ags_main_loop_set_async_queue:
 * @main_loop: the #AgsMainLoop
 * @async_queue: the #AgsAsyncQueue
 *
 * Sets the asynchronous queue.
 *
 * Since: 1.0.0
 */
void
ags_main_loop_set_async_queue(AgsMainLoop *main_loop, GObject *async_queue)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->set_async_queue);
  main_loop_interface->set_async_queue(main_loop, async_queue);
}

/**
 * ags_main_loop_get_async_queue:
 * @main_loop: the #AgsMainLoop
 *
 * Retrieve the #AgsAsyncQueue.
 *
 * Returns: the #AgsAsyncQueue
 *
 * Since: 1.0.0
 */
GObject*
ags_main_loop_get_async_queue(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), NULL);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_async_queue, NULL);

  return(main_loop_interface->get_async_queue(main_loop));
}

/**
 * ags_main_loop_set_tic:
 * @main_loop: the #AgsMainLoop
 * @tic: tic
 *
 * Sets tic to @tic. 
 *
 * Since: 1.0.0
 */
void
ags_main_loop_set_tic(AgsMainLoop *main_loop, guint tic)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->set_tic);
  main_loop_interface->set_tic(main_loop, tic);
}

/**
 * ags_main_loop_get_tic:
 * @main_loop: the #AgsMainLoop
 *
 * Retrieve current tic of synchronization context.
 *
 * Returns: current tic
 *
 * Since: 1.0.0
 */
guint
ags_main_loop_get_tic(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), G_MAXUINT);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_tic, G_MAXUINT);

  return(main_loop_interface->get_tic(main_loop));
}

/**
 * ags_main_loop_set_last_sync:
 * @main_loop: the #AgsMainLoop
 * @last_sync: last sync
 *
 * Sets last sync to @last_sync. 
 *
 * Since: 1.0.0
 */
void
ags_main_loop_set_last_sync(AgsMainLoop *main_loop, guint last_sync)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->set_last_sync);
  main_loop_interface->set_last_sync(main_loop, last_sync);
}

/**
 * ags_main_loop_get_last_sync:
 * @main_loop: the #AgsMainLoop
 *
 * Retrieve last sync as tic of synchronization context.
 *
 * Returns: last synced tic
 *
 * Since: 1.0.0
 */
guint
ags_main_loop_get_last_sync(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), G_MAXUINT);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_last_sync, G_MAXUINT);

  return(main_loop_interface->get_last_sync(main_loop));
}

/**
 * ags_main_loop_interrupt:
 * @main_loop: the #AgsMainLoop
 * @sig: the singal number
 * @time_cycle: the amount of time of a cycle
 * @time_spent: the amount of time passed since last cycle
 * 
 * Notify about interrupt threads.
 *
 * Since: 1.0.0
 */
void
ags_main_loop_interrupt(AgsMainLoop *main_loop,
			int sig,
			guint time_cycle, guint *time_spent)
{
  g_signal_emit(main_loop,
		main_loop_signals[INTERRUPT],
		0,
		sig,
		time_cycle,
		time_spent);
}

/**
 * ags_main_loop_monitor:
 * @main_loop: the #AgsMainLoop
 * @time_cycle: the amount of time of a cycle
 * @time_spent: the amount of time passed since last cycle
 *
 * Notify to monitor time.
 *
 * Returns: if monitor is allowed
 *
 * Since: 1.0.0
 */
gboolean
ags_main_loop_monitor(AgsMainLoop *main_loop,
		      guint time_cycle, guint *time_spent)
{
  gboolean has_monitor;
  
  g_signal_emit(main_loop,
		main_loop_signals[MONITOR],
		0,
		time_cycle,
		time_spent,
		&has_monitor);

  return(has_monitor);
}

/**
 * ags_main_loop_change_frequency:
 * @main_loop: the #AgsMainLoop
 * @frequency: the new frequency
 *
 * Change frequency.
 *
 * Since: 1.0.0
 */
void
ags_main_loop_change_frequency(AgsMainLoop *main_loop,
			       gdouble frequency)
{
  g_signal_emit(main_loop,
		main_loop_signals[CHANGE_FREQUENCY],
		0,
		frequency);
}
