/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __GSEQUENCER_TEST_SETUP_UTIL_H__
#define __GSEQUENCER_TEST_SETUP_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#define GSEQUENCER_RT_PRIORITY (95)

void ags_test_enter();
void ags_test_leave();

pthread_mutex_t* ags_test_get_driver_mutex();

void ags_test_init(int *argc, char ***argv,
		   gchar *conf_str);

void ags_test_show_file_error(gchar *filename,
			      GError *error);

void ags_test_signal_handler(int signr);
void ags_test_signal_cleanup();

void ags_test_setup(int argc, char **argv);

void ags_test_start_animation(pthread_t *thread);
void* ags_test_start_animation_thread(void *ptr);

void ags_test_launch(gboolean single_thread);
void ags_test_launch_filename(gchar *filename,
			      gboolean single_thread);

#endif /*__GSEQUENCER_TEST_SETUP_UTIL_H__*/
