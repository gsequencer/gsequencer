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

#ifndef __AGS_SERVER_APPLICATION_CONTEXT_H__
#define __AGS_SERVER_APPLICATION_CONTEXT_H__

#ifdef AGS_WITH_XMLRPC_C
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>
#endif

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_application_context.h>

#include <ags/server/ags_server.h>

#define AGS_TYPE_SERVER_APPLICATION_CONTEXT                (ags_server_application_context_get_type())
#define AGS_SERVER_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContext))
#define AGS_SERVER_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContextClass))
#define AGS_IS_SERVER_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER_APPLICATION_CONTEXT))
#define AGS_IS_SERVER_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER_APPLICATION_CONTEXT))
#define AGS_SERVER_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContextClass))

#define AGS_SERVER_BUILD_ID "Thu Apr  2 13:04:21 GMT 2015\0"
#define AGS_SERVER_DEFAULT_VERSION "0.4.3\0"

typedef struct _AgsServerApplicationContext AgsServerApplicationContext;
typedef struct _AgsServerApplicationContextClass AgsServerApplicationContextClass;

typedef enum{
  AGS_SERVER_APPLICATION_CONTEXT_ENABLE_SERVER      = 1,
}AgsServerApplicationContextFlags;

struct _AgsServerApplicationContext
{
  AgsApplicationContext application_context;

  guint flags;

  gchar *version;
  gchar *build_id;

#ifdef AGS_WITH_XMLRPC_C
  xmlrpc_env env;
#endif

  AgsServer *server;
};

struct _AgsServerApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_server_application_context_get_type();

AgsServerApplicationContext* ags_server_application_context_new(GObject *main_loop,
								AgsConfig *config);

#endif /*__AGS_SERVER_APPLICATION_CONTEXT_H__*/
