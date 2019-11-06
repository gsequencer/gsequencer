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

#include <ags/object/ags_main_loop.h>

#include <stdio.h>

void ags_main_loop_class_init(AgsMainLoopInterface *ginterface);

/**
 * SECTION:ags_main_loop
 * @short_description: main loop interface
 * @title: AgsMainLoop
 * @section_id: AgsMainLoop
 * @include: ags/object/ags_main_loop.h
 *
 * The #AgsMainLoop interface gives you a unique access to
 * the main loop.
 */

enum {
  CHANGE_FREQUENCY,
  LAST_SIGNAL,
};

static guint main_loop_signals[LAST_SIGNAL];

GType
ags_main_loop_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_main_loop = 0;

    ags_type_main_loop = g_type_register_static_simple(G_TYPE_INTERFACE,
						       "AgsMainLoop",
						       sizeof (AgsMainLoopInterface),
						       (GClassInitFunc) ags_main_loop_class_init,
						       0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_main_loop);
  }

  return g_define_type_id__volatile;
}

void
ags_main_loop_class_init(AgsMainLoopInterface *ginterface)
{
  /**
   * AgsMainLoop::change-frequency:
   * @main_loop: the #AgsMainLoop
   * @frequency: the new frequency
   *
   * Change frequency.
   * 
   * Since: 3.0.0
   */
  main_loop_signals[CHANGE_FREQUENCY] =
    g_signal_new("change-frequency",
		 G_TYPE_FROM_INTERFACE(ginterface),
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
 * Since: 3.0.0
 */
GRecMutex*
ags_main_loop_get_tree_lock(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), NULL);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_tree_lock, NULL);

  return(main_loop_interface->get_tree_lock(main_loop));
}


/**
 * ags_main_loop_set_tic:
 * @main_loop: the #AgsMainLoop
 * @tic: tic
 *
 * Sets tic to @tic. 
 *
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * Since: 3.0.0
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
 * ags_main_loop_set_sync_tic:
 * @main_loop: the #AgsMainLoop
 * @tic: the current sync tic
 *
 * Set current tic of sync counter.
 *
 * Since: 3.0.0
 */
void
ags_main_loop_set_sync_tic(AgsMainLoop *main_loop, guint sync_tic)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->set_sync_tic);

  main_loop_interface->set_sync_tic(main_loop, sync_tic);
}

/**
 * ags_main_loop_get_sync_tic:
 * @main_loop: the #AgsMainLoop
 *
 * Retrieve current tic of sync counter.
 *
 * Returns: current sync tic
 *
 * Since: 3.0.0
 */
guint
ags_main_loop_get_sync_tic(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), G_MAXUINT);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_sync_tic, G_MAXUINT);

  return(main_loop_interface->get_sync_tic(main_loop));
}

/**
 * ags_main_loop_sync_counter_inc:
 * @main_loop: the #AgsMainLoop
 * @tic: the current tic
 *
 * Increment sync counter.
 *
 * Since: 3.0.0
 */
void
ags_main_loop_sync_counter_inc(AgsMainLoop *main_loop, guint tic)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->sync_counter_inc);

  main_loop_interface->sync_counter_inc(main_loop, tic);
}

/**
 * ags_main_loop_sync_counter_dec:
 * @main_loop: the #AgsMainLoop
 * @tic: the current tic
 *
 * Decrement sync counter.
 *
 * Since: 3.0.0
 */
void
ags_main_loop_sync_counter_dec(AgsMainLoop *main_loop, guint tic)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_if_fail(AGS_IS_MAIN_LOOP(main_loop));
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_if_fail(main_loop_interface->sync_counter_dec);

  main_loop_interface->sync_counter_dec(main_loop, tic);
}

/**
 * ags_main_loop_sync_counter_inc:
 * @main_loop: the #AgsMainLoop
 * @tic: the current tic
 *
 * test sync counter.
 *
 * Returns: %TRUE if synced, otherwise %FALSE
 * 
 * Since: 3.0.0
 */
gboolean
ags_main_loop_sync_counter_test(AgsMainLoop *main_loop, guint tic)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), FALSE);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->sync_counter_test, FALSE);

  return(main_loop_interface->sync_counter_test(main_loop, tic));
}

/**
 * ags_main_loop_change_frequency:
 * @main_loop: the #AgsMainLoop
 * @frequency: the new frequency
 *
 * Change frequency.
 *
 * Since: 3.0.0
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
