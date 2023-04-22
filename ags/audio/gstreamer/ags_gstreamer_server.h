/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_GSTREAMER_SERVER_H__
#define __AGS_GSTREAMER_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSTREAMER_SERVER                (ags_gstreamer_server_get_type())
#define AGS_GSTREAMER_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSTREAMER_SERVER, AgsGstreamerServer))
#define AGS_GSTREAMER_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_GSTREAMER_SERVER, AgsGstreamerServer))
#define AGS_IS_GSTREAMER_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_GSTREAMER_SERVER))
#define AGS_IS_GSTREAMER_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_GSTREAMER_SERVER))
#define AGS_GSTREAMER_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_GSTREAMER_SERVER, AgsGstreamerServerClass))

#define AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(obj) (&(((AgsGstreamerServer *) obj)->obj_mutex))

typedef struct _AgsGstreamerServer AgsGstreamerServer;
typedef struct _AgsGstreamerServerClass AgsGstreamerServerClass;

struct _AgsGstreamerServer
{
  GObject gobject;

  guint flags;
  AgsConnectableFlags connectable_flags;
  
  GRecMutex obj_mutex;

  volatile gboolean running;
  GThread *thread;
  
  AgsApplicationContext *application_context;

  AgsUUID *uuid;
  
  gchar *url;

  guint *port;
  guint port_count;

  guint n_soundcards;
  guint n_sequencers;
  
  GObject *default_soundcard;  

  GObject *default_client;
  GList *client;
};

struct _AgsGstreamerServerClass
{
  GObjectClass gobject;
};

GType ags_gstreamer_server_get_type();

GList* ags_gstreamer_server_find_url(GList *gstreamer_server,
				     gchar *url);

GObject* ags_gstreamer_server_find_client(AgsGstreamerServer *gstreamer_server,
					  gchar *client_uuid);

GObject* ags_gstreamer_server_find_port(AgsGstreamerServer *gstreamer_server,
					gchar *port_uuid);

void ags_gstreamer_server_add_client(AgsGstreamerServer *gstreamer_server,
				     GObject *gstreamer_client);
void ags_gstreamer_server_remove_client(AgsGstreamerServer *gstreamer_server,
					GObject *gstreamer_client);

void ags_gstreamer_server_connect_client(AgsGstreamerServer *gstreamer_server);
void ags_gstreamer_server_disconnect_client(AgsGstreamerServer *gstreamer_server);

void ags_gstreamer_server_start_poll(AgsGstreamerServer *gstreamer_server);

AgsGstreamerServer* ags_gstreamer_server_new(gchar *url);

G_END_DECLS

#endif /*__AGS_GSTREAMER_SERVER_H__*/
