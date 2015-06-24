/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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

#include <ags/object/ags_main_loop.h>

#include <stdio.h>

void ags_main_loop_base_init(AgsMainLoopInterface *interface);

/**
 * SECTION:ags_main_loop
 * @short_description: toplevel threads
 * @title: AgsMainLoop
 * @section_id:
 * @include: ags/object/ags_main_loop.h
 *
 * The #AgsMainLoop interface gives you a unique access to toplevel
 * threads and has to be implemented for it.
 */

GType
ags_main_loop_get_type()
{
  static GType ags_type_main_loop = 0;

  if(!ags_type_main_loop){
    static const GTypeInfo ags_main_loop_info = {
      sizeof(AgsMainLoopInterface),
      (GBaseInitFunc) ags_main_loop_base_init,
      NULL, /* base_finalize */
    };
    
    ags_type_main_loop = g_type_register_static(G_TYPE_INTERFACE,
						"AgsMainLoop\0", &ags_main_loop_info,
						0);
  }

  return(ags_type_main_loop);
}

void
ags_main_loop_base_init(AgsMainLoopInterface *interface)
{
  /* empty */
}

/**
 * ags_main_loop_set_async_queue:
 * @main_loop: the #AgsMainLoop
 * @async_queue: the #AgsAsyncQueue
 *
 * Sets the asynchronous queue.
 *
 * Since: 0.4
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
 * Since: 0.4
 */
GObject*
ags_main_loop_get_async_queue(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), NULL);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_async_queue, NULL);
  main_loop_interface->get_async_queue(main_loop);
}

/**
 * ags_main_loop_set_tic:
 * @main_loop: the #AgsMainLoop
 * @tic: tic
 *
 * Sets tic to @tic. 
 *
 * Since: 0.4
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
 * Since: 0.4
 */
guint
ags_main_loop_get_tic(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), G_MAXUINT);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_tic, G_MAXUINT);
  main_loop_interface->get_tic(main_loop);
}

/**
 * ags_main_loop_set_last_sync:
 * @main_loop: the #AgsMainLoop
 * @last_sync: last sync
 *
 * Sets last sync to @last_sync. 
 *
 * Since: 0.4
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
 * Since: 0.4
 */
guint
ags_main_loop_get_last_sync(AgsMainLoop *main_loop)
{
  AgsMainLoopInterface *main_loop_interface;

  g_return_val_if_fail(AGS_IS_MAIN_LOOP(main_loop), G_MAXUINT);
  main_loop_interface = AGS_MAIN_LOOP_GET_INTERFACE(main_loop);
  g_return_val_if_fail(main_loop_interface->get_last_sync, G_MAXUINT);
  main_loop_interface->get_last_sync(main_loop);
}
