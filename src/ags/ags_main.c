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

#include <ags/object/ags_application_context.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif

#include <ags/X/thread/ags_gui_thread.h>

void
ags_main_quit(AgsApplicationContext *application_context)
{
  AgsThread *gui_thread;
  AgsThread *children;

  /* find gui thread */
  children = AGS_THREAD(application_context->main_loop)->children;

  while(children != NULL){
    if(AGS_IS_GUI_THREAD(children)){
      gui_thread = children;

      break;
    }

    children = children->next;
  }

  ags_thread_stop(gui_thread);
}
