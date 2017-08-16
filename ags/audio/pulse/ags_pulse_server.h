/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_PULSE_SERVER_H__
#define __AGS_PULSE_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <pulse/pulseaudio.h>
#include <pulse/stream.h>
#include <pulse/error.h>

#define AGS_TYPE_PULSE_SERVER                (ags_pulse_server_get_type())
#define AGS_PULSE_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PULSE_SERVER, AgsPulseServer))
#define AGS_PULSE_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PULSE_SERVER, AgsPulseServer))
#define AGS_IS_PULSE_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PULSE_SERVER))
#define AGS_IS_PULSE_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PULSE_SERVER))
#define AGS_PULSE_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PULSE_SERVER, AgsPulseServerClass))

typedef struct _AgsPulseServer AgsPulseServer;
typedef struct _AgsPulseServerClass AgsPulseServerClass;

typedef enum{
  AGS_PULSE_SERVER_CONNECTED       = 1,
}AgsPulseServerFlags;

struct _AgsPulseServer
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

struct _AgsPulseServerClass
{
  GObjectClass object;
};

GType ags_pulse_server_get_type();

GList* ags_pulse_server_find_url(GList *pulse_server,
				 gchar *url);

GObject* ags_pulse_server_find_client(AgsPulseServer *pulse_server,
				      gchar *client_uuid);

GObject* ags_pulse_server_find_port(AgsPulseServer *pulse_server,
				    gchar *port_uuid);

void ags_pulse_server_add_client(AgsPulseServer *pulse_server,
				 GObject *pulse_client);
void ags_pulse_server_remove_client(AgsPulseServer *pulse_server,
				    GObject *pulse_client);

void ags_pulse_server_connect_client(AgsPulseServer *pulse_server);

AgsPulseServer* ags_pulse_server_new(GObject *application_context,
				     gchar *url);

#endif /*__AGS_PULSE_SERVER_H__*/
