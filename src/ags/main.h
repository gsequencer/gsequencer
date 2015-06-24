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

#ifndef __AGS_MAIN_H__
#define __AGS_MAIN_H__

//#define AGS_DEBUG
#define _GNU_SOURCE
#define ALSA_PCM_NEW_HW_PARAMS_API

#ifdef AGS_WITH_XMLRPC_C
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>
#endif

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#include <ags/lib/ags_log.h>

#include <ags/thread/ags_thread_pool.h>
#include <ags/server/ags_server.h>
#include <ags/audio/ags_config.h>
#include <ags/audio/ags_devout.h>
#include <ags/X/ags_window.h>

#define AGS_TYPE_MAIN                (ags_main_get_type())
#define AGS_MAIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MAIN, AgsMain))
#define AGS_MAIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MAIN, AgsMainClass))
#define AGS_IS_MAIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MAIN))
#define AGS_IS_MAIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MAIN))
#define AGS_MAIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MAIN, AgsMainClass))

#define AGS_VERSION "0.4.2\0"
#define AGS_BUILD_ID "CEST 02-10-2014 19:36\0"
#define AGS_EFFECTS_DEFAULT_VERSION "0.4.2\0"
#define AGS_DEFAULT_DIRECTORY ".gsequencer\0"
#define AGS_DEFAULT_CONFIG "ags.conf\0"
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
  AgsThread *autosave_thread;
  AgsThreadPool *thread_pool;

  AgsServer *server;

  GList *devout;

  AgsWindow *window;

  AgsConfig *config;
  AgsLog *log;
};

struct _AgsMainClass
{
  GObjectClass object;
};

GType ags_main_get_type();

void ags_main_load_config(AgsMain *ags_main);

void ags_main_add_devout(AgsMain *ags_main,
			 AgsDevout *devout);

void ags_main_register_thread_type();

void ags_main_register_recall_type();
void ags_main_register_task_type();

void ags_main_register_widget_type();
void ags_main_register_machine_type();

void ags_main_quit(AgsMain *ags_main);

AgsMain* ags_main_new();

#endif /*__AGS_MAIN_H__*/
