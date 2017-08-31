/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifdef AGS_WITH_XMLRPC_C
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>
#endif

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_thread_pool.h>

#include <ags/server/ags_registry.h>
#include <ags/server/ags_server.h>

#include <ags/server/security/ags_authentication_manager.h>
#include <ags/server/security/ags_certificate_manager.h>
#include <ags/server/security/ags_password_store_manager.h>

#define AGS_TYPE_SERVER_APPLICATION_CONTEXT                (ags_server_application_context_get_type())
#define AGS_SERVER_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContext))
#define AGS_SERVER_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContextClass))
#define AGS_IS_SERVER_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER_APPLICATION_CONTEXT))
#define AGS_IS_SERVER_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER_APPLICATION_CONTEXT))
#define AGS_SERVER_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER_APPLICATION_CONTEXT, AgsServerApplicationContextClass))

#define AGS_SERVER_BUILD_ID "Sun Feb 26 23:03:41 CET 2017\0"
#define AGS_SERVER_DEFAULT_VERSION "1.0.0\0"

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

  AgsThreadPool *thread_pool;

#ifdef AGS_WITH_XMLRPC_C
  xmlrpc_env *env;
#else
  void *env;
#endif

  AgsRegistry *registry;
  AgsServer *server;

  AgsCertificateManager *certificate_manager;
  AgsPasswordStoreManager *password_store_manager;
  AgsAuthenticationManager *authentication_manager;
};

struct _AgsServerApplicationContextClass
{
  AgsApplicationContextClass application_context;
};

GType ags_server_application_context_get_type();

AgsServerApplicationContext* ags_server_application_context_new();

#endif /*__AGS_SERVER_APPLICATION_CONTEXT_H__*/
