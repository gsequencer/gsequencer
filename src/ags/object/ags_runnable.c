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

#include <ags/object/ags_runnable.h>

#include <stdio.h>

void ags_runnable_base_init(AgsRunnableInterface *interface);

GType
ags_runnable_get_type()
{
  static GType ags_type_runnable = 0;

  if(!ags_type_runnable){
    static const GTypeInfo ags_runnable_info = {
      sizeof(AgsRunnableInterface),
      (GBaseInitFunc) ags_runnable_base_init,
      NULL, /* base_finalize */
    };

    ags_type_runnable = g_type_register_static(G_TYPE_INTERFACE,
					       "AgsRunnable", &ags_runnable_info,
					       0);
  }

  return(ags_type_runnable);
}

void
ags_runnable_base_init(AgsRunnableInterface *interface)
{
  /* empty */
}

void
ags_runnable_run(AgsRunnable *runnable)
{
  AgsRunnableInterface *runnable_interface;

  g_return_if_fail(AGS_IS_RUNNABLE(runnable));
  runnable_interface = AGS_RUNNABLE_GET_INTERFACE(runnable);
  g_return_if_fail(runnable_interface->run);
  runnable_interface->run(runnable);
}

void
ags_runnable_stop(AgsRunnable *runnable)
{
  AgsRunnableInterface *runnable_interface;

  g_return_if_fail(AGS_IS_RUNNABLE(runnable));
  runnable_interface = AGS_RUNNABLE_GET_INTERFACE(runnable);
  g_return_if_fail(runnable_interface->stop);
  runnable_interface->stop(runnable);
}
