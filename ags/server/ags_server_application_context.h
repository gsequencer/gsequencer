/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_SERVER_APPLICATION_CONTEXT_H__
#define __AGS_SERVER_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_thread_pool.h>
#include <ags/thread/ags_task_launcher.h>

#include <ags/server/ags_server.h>
#include <ags/server/ags_server_status.h>
#include <ags/server/ags_registry.h>

G_BEGIN_DECLS

#define AGS_TYPE_SERVER_APPLICATION_CONTEXT                (ags_server_application_context_get_type())
#define AGS_SERVER_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContext))
#define AGS_SERVER_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContextClass))
#define AGS_IS_SERVER_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER_APPLICATION_CONTEXT))
#define AGS_IS_SERVER_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER_APPLICATION_CONTEXT))
#define AGS_SERVER_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContextClass))

#define AGS_SERVER_VERSION "3.0.0"
#define AGS_SERVER_BUILD_ID "Sat Dec  7 10:02:36 UTC 2019"

typedef struct _AgsServerApplicationContext AgsServerApplicationContext;
typedef struct _AgsServerApplicationContextClass AgsServerApplicationContextClass;

struct _AgsServerApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;

  AgsThreadPool *thread_pool;

  GList *worker;

  GMainContext *server_main_context;

  gboolean is_operating;

  AgsServerStatus *server_status;
  
  AgsRegistry *registry;
  
  GList *server;
};

struct _AgsServerApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_server_application_context_get_type();

AgsServerApplicationContext* ags_server_application_context_new();

G_END_DECLS

#endif /*__AGS_SERVER_APPLICATION_CONTEXT_H__*/
