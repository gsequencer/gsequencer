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

#ifndef __AGS_JACK_SERVER_H__
#define __AGS_JACK_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <jack/jack.h>

#define AGS_TYPE_JACK_SERVER                (ags_jack_server_get_type())
#define AGS_JACK_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_SERVER, AgsJackServer))
#define AGS_JACK_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_SERVER, AgsJackServer))
#define AGS_IS_JACK_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_SERVER))
#define AGS_IS_JACK_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_SERVER))
#define AGS_JACK_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_SERVER, AgsJackServerClass))

#define AGS_JACK_SERVER_DEFAULT_PROTOCOL "udp"
#define AGS_JACK_SERVER_DEFAULT_HOST "localhost"
#define AGS_JACK_SERVER_DEFAULT_PORT (3000)

typedef struct _AgsJackServer AgsJackServer;
typedef struct _AgsJackServerClass AgsJackServerClass;

typedef enum{
  AGS_JACK_SERVER_CONNECTED       = 1,
}AgsJackServerFlags;

struct _AgsJackServer
{
  GObject object;

  guint flags;

  GObject *application_context;

  gchar *url;
  
  guint *port;
  guint port_count;
  
  guint n_soundcards;
  guint n_sequencers;
  
  GObject *default_soundcard;  

  GObject *default_client;
  GList *client;
};

struct _AgsJackServerClass
{
  GObjectClass object;
};

GType ags_jack_server_get_type();

GObject* ags_jack_server_find_client(AgsJackServer *jack_server,
				     gchar *client_uuid);

GObject* ags_jack_server_find_port(AgsJackServer *jack_server,
				   gchar *port_uuid);

AgsJackServer* ags_jack_server_new(GObject *application_context,
				   gchar *url);

#endif /*__AGS_JACK_SERVER_H__*/
