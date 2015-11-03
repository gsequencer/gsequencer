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

typedef struct _AgsJackServer AgsJackServer;
typedef struct _AgsJackServerClass AgsJackServerClass;
typedef struct _AgsJackClient AgsJackClient;
typedef struct _AgsJackPort AgsJackPort;

typedef enum{
  AGS_JACK_SERVER_CONNECTED       = 1,
}AgsJackServerFlags;

struct _AgsJackServer
{
  GObject object;

  guint flags;
  
  gchar *url;

  guint n_soundcards;
  guint n_midicards;
  
  GList *client;
};

struct _AgsJackServerClass
{
  GObjectClass object;
};

struct _AgsJackClient
{
  gboolean default_client;
  
  gchar *uuid;
  
  jack_client_t *client;

  GList *port;
};

struct _AgsJackPort
{
  gchar *uuid;
  gchar *name;
  gchar *uri;
  
  jack_port_t *port;
  
  GObject *gobject;
};

GType ags_jack_server_get_type();

AgsJackClient* ags_jack_client_alloc();
void ags_jack_client_free(AgsJackClient *jack_client);

AgsJackPort* ags_jack_port_alloc();
void ags_jack_port_free(AgsJackPort *jack_port);

AgsJackClient* ags_jack_server_find_default_client(AgsJackServer *jack_server);

GObject* ags_jack_server_register_default_soundcard(AgsJackServer *jack_server);

AgsJackServer* ags_jack_server_new();

#endif /*__AGS_JACK_SERVER_H__*/
