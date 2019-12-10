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

#ifndef __AGS_SERVER_H__
#define __AGS_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libsoup/soup.h>

#include <ags/lib/ags_uuid.h>

G_BEGIN_DECLS

#define AGS_TYPE_SERVER                (ags_server_get_type())
#define AGS_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERVER, AgsServer))
#define AGS_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SERVER, AgsServerClass))
#define AGS_IS_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SERVER))
#define AGS_IS_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SERVER))
#define AGS_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SERVER, AgsServerClass))

#define AGS_SERVER_GET_OBJ_MUTEX(obj) (&(((AgsServer *) obj)->obj_mutex))

#define AGS_SERVER_DEFAULT_SERVER_PORT (8080)
#define AGS_SERVER_DEFAULT_DOMAIN "localhost"
#define AGS_SERVER_DEFAULT_INET4_ADDRESS "127.0.0.1"
#define AGS_SERVER_DEFAULT_INET6_ADDRESS "::1"

#define AGS_SERVER_DEFAULT_AUTH_MODULE "ags-xml-authentication"

#define AGS_SERVER_DEFAULT_BACKLOG (512)

typedef struct _AgsServer AgsServer;
typedef struct _AgsServerClass AgsServerClass;
typedef struct _AgsServerInfo AgsServerInfo;

/**
 * AgsServerFlags:
 * @AGS_SERVER_STARTED: the server was started
 * @AGS_SERVER_RUNNING: the server is up and running
 * @AGS_TERMINATING: the server is closing connections and terminating
 * @AGS_SERVER_INET4: use IPv4
 * @AGS_SERVER_INET6: use IPv6
 * @AGS_SERVER_ANY_ADDRESS: listen on any address
 * @AGS_SERVER_AUTO_START: start the server
 * 
 * Enum values to control the behavior or indicate internal state of #AgsServer by
 * enable/disable as flags.
 */
typedef enum{
  AGS_SERVER_STARTED            = 1,
  AGS_SERVER_RUNNING            = 1 <<  1,
  AGS_SERVER_TERMINATING        = 1 <<  2,
  AGS_SERVER_INET4              = 1 <<  3,
  AGS_SERVER_INET6              = 1 <<  4,
  AGS_SERVER_ANY_ADDRESS        = 1 <<  5,
  AGS_SERVER_AUTO_START         = 1 <<  6,
}AgsServerFlags;

struct _AgsServer
{
  GObject gobject;

  guint flags;

  GRecMutex obj_mutex;

  AgsUUID *uuid;

  AgsServerInfo *server_info;

  gchar *ip4;
  gchar *ip6;
  
  gchar *domain;
  guint server_port;

  int ip4_fd;
  int ip6_fd;

  GSocket *ip4_socket;
  GSocket *ip6_socket;

  GSocketAddress *ip4_address;
  GSocketAddress *ip6_address;

  gchar *realm;
  
  SoupServer *soup_server;

  gchar *auth_module;

  SoupAuthDomain *auth_domain;
  
  GObject *front_controller;

  GList *controller;
};

struct _AgsServerClass
{
  GObjectClass gobject;
  
  void (*start)(AgsServer *server);
  void (*stop)(AgsServer *server);

  gboolean (*listen)(AgsServer *server);
};

/**
 * AgsServerInfo:
 * @uuid: the assigned uuid
 * @server_name: the server's name
 * 
 * #AgsServerInfo does identify the server and provides some basic information about it.
 */
struct _AgsServerInfo
{
  gchar *uuid;
  gchar *server_name;
};

GType ags_server_get_type();

gboolean ags_server_test_flags(AgsServer *server, guint flags);
void ags_server_set_flags(AgsServer *server, guint flags);
void ags_server_unset_flags(AgsServer *server, guint flags);

AgsServerInfo* ags_server_info_alloc(gchar *server_name, gchar *uuid);

/* fields */
void ags_server_add_controller(AgsServer *server,
			       GObject *controller);
void ags_server_remove_controller(AgsServer *server,
				  GObject *controller);

void ags_server_start(AgsServer *server);
void ags_server_stop(AgsServer *server);

gboolean ags_server_listen(AgsServer *server);

AgsServer* ags_server_lookup(AgsServerInfo *server_info);

AgsServer* ags_server_new();

G_END_DECLS

#endif /*__AGS_SERVER_H__*/
