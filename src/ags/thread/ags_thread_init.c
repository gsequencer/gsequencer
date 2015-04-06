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

#include <ags/thread/ags_thread_init.h>

#include <ags/object/ags_init.h>
#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_thread_application_context.h>

extern pthread_key_t application_context;

void
ags_thread_init_context(int *argc, gchar ***argv)
{
  AgsApplicationContext *ags_application_context =  pthread_getspecific(application_context);
  AgsThreadApplicationContext *ags_thread_application_context;

  /* complete thread pool */
  ags_thread_application_context->thread_pool->parent = AGS_THREAD(ags_application_context->main_loop);
  ags_thread_pool_start(ags_thread_application_context->thread_pool);

  /* instantiate application context */
  ags_thread_application_context = ags_thread_application_context_new(NULL,
								      NULL);
  ags_application_context_add_sibling(ags_application_context,
				      ags_thread_application_context);
}
