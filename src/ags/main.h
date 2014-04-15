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

#ifdef AGS_WITH_XMLRPC_C
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>
#endif

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_log.h>
#include <ags/thread/ags_thread.h>
#include <ags/thread/ags_thread_pool.h>
#include <ags/server/ags_server.h>
#include <ags/audio/ags_devout.h>
#include <ags/X/ags_window.h>

#define AGS_TYPE_MAIN                (ags_main_get_type())
#define AGS_MAIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MAIN, AgsMain))
#define AGS_MAIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MAIN, AgsMainClass))
#define AGS_IS_MAIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MAIN))
#define AGS_IS_MAIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MAIN))
#define AGS_MAIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MAIN, AgsMainClass))

#define AGS_VERSION "0.4.0-beta\0"
#define AGS_BUILD_ID "CEST 04-12-2013 03:07\0"
#define AGS_EFFECTS_DEFAULT_VERSION "0.4.0-beta\0"

#define __AGS_DEBUG__
#define AGS_PRIORITY (49)

typedef struct _AgsMain AgsMain;
typedef struct _AgsMainClass AgsMainClass;

typedef enum{
  AGS_MAIN_SINGLE_THREAD      = 1,
  AGS_MAIN_CONNECTED          = 1 <<  1,
}AgsMainFlags;

struct _AgsMain
{
  GObject object;

  guint flags;

  gchar *version;
  gchar *build_id;

#ifdef AGS_WITH_XMLRPC_C
  xmlrpc_env env;
#endif

  AgsThread *main_loop;
  AgsThreadPool *thread_pool;

  AgsServer *server;

  GList *devout;

  AgsWindow *window;

  AgsLog *log;
};

struct _AgsMainClass
{
  GObjectClass object;
};

GType ags_main_get_type();

void ags_main_add_devout(AgsMain *main,
			 AgsDevout *devout);

void ags_main_register_thread_type();

void ags_main_register_recall_type();
void ags_main_register_task_type();

void ags_main_register_widget_type();
void ags_main_register_machine_type();

void ags_main_quit(AgsMain *main);

AgsMain* ags_main_new();

#endif /*__AGS_MAIN_H__*/
