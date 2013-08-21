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

#ifndef __AGS_MAIN_H__
#define __AGS_MAIN_H__

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_log.h>
#include <ags/thread/ags_thread.h>
#include <ags/server/ags_server.h>
#include <ags/audio/ags_devout.h>
#include <ags/X/ags_window.h>

#define AGS_TYPE_MAIN                (ags_main_get_type())
#define AGS_MAIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MAIN, AgsMain))
#define AGS_MAIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MAIN, AgsMainClass))
#define AGS_IS_MAIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MAIN))
#define AGS_IS_MAIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MAIN))
#define AGS_MAIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MAIN, AgsMainClass))

#define AGS_VERSION "0.3.15-SNAPSHOT"

#define __AGS_DEBUG__

typedef struct _AgsMain AgsMain;
typedef struct _AgsMainClass AgsMainClass;

struct _AgsMain
{
  GObject object;

  xmlrpc_env env;

  AgsThread *main_loop;
  AgsServer *server;
  AgsDevout *devout;
  AgsWindow *window;

  AgsLog *log;
};

struct _AgsMainClass
{
  GObjectClass object;
};

GType ags_main_get_type();

AgsMain* ags_main_new();

#endif /*__AGS_MAIN_H__*/
