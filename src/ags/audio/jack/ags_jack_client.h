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

#ifndef __AGS_JACK_CLIENT_H__
#define __AGS_JACK_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <jack/jack.h>

#define AGS_TYPE_JACK_CLIENT                (ags_jack_client_get_type())
#define AGS_JACK_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_CLIENT, AgsJackClient))
#define AGS_JACK_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_CLIENT, AgsJackClient))
#define AGS_IS_JACK_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_CLIENT))
#define AGS_IS_JACK_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_CLIENT))
#define AGS_JACK_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_CLIENT, AgsJackClientClass))

typedef struct _AgsJackClient AgsJackClient;
typedef struct _AgsJackClientClass AgsJackClientClass;

typedef enum{
  AGS_JACK_CLIENT_CONNECTED       = 1,
}AgsJackClientFlags;

struct _AgsJackClient
{
  GObject object;

  guint flags;

  GObject *jack_server;
  
  gchar *uuid;
  
  jack_client_t *client;

  GList *port;
};

struct _AgsJackClientClass
{
  GObjectClass object;
};

GType ags_jack_client_get_type();

void ags_jack_client_add_port(AgsJackClient *jack_client,
			      GObject *port);
void ags_jack_client_remove_port(AgsJackClient *jack_client,
				 GObject *port);

void ags_jack_client_open(AgsJackClient *jack_client,
			  gchar *client_name);

AgsJackClient* ags_jack_client_new(GObject *jack_server);

#endif /*__AGS_JACK_CLIENT_H__*/

