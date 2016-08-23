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

#ifndef __GSEQUENCER_MAIN_H__
#define __GSEQUENCER_MAIN_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

#include <gtk/gtk.h>

#include <jack/jslist.h>
#include <jack/jack.h>
//#include <jack/control.h>
#include <stdbool.h>

#include <libintl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/mman.h>

//#include <ao/ao.h>

#include <pthread.h>

#include <sys/types.h>
#include <pwd.h>

#define ALSA_PCM_NEW_HW_PARAMS_API

#define AGS_DEFAULT_DIRECTORY ".gsequencer\0"
#define AGS_DEFAULT_CONFIG "ags.conf\0"

#define GSEQUENCER_RT_PRIORITY (95)

#endif /*__GSEQUENCER_MAIN_H__*/
