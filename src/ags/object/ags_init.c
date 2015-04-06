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

#include <ags/object/ags_init.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>

static pthread_key_t config;
static pthread_key_t application_context;

void
ags_init_context(int *argc, gchar ***argv)
{
  AgsApplicationContext *ags_application_context;
  AgsConfig *ags_config;
  gchar *filename;

  /* instantiate config */
  ags_config = ags_config_new(NULL);
  
  /* instantiate application context */
  ags_application_context = ags_application_context_new(NULL,
							ags_config);
  g_object_set(config,
	       "application-context\0", ags_application_context,
	       NULL);

  /* set TSD-Data - application context */
  pthread_key_create(application_context,
		     NULL);
  pthread_setspecific(application_context,
		      ags_application_context);

  /* set TSD-Data - config */
  pthread_key_create(config,
		     NULL);
  pthread_setspecific(config,
		      ags_config);
}
