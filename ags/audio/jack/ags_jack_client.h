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

#ifndef __AGS_JACK_CLIENT_H__
#define __AGS_JACK_CLIENT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/ags_api_config.h>

#ifdef AGS_WITH_JACK
#include <jack/jack.h>
#endif

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_JACK_CLIENT                (ags_jack_client_get_type())
#define AGS_JACK_CLIENT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_JACK_CLIENT, AgsJackClient))
#define AGS_JACK_CLIENT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_JACK_CLIENT, AgsJackClient))
#define AGS_IS_JACK_CLIENT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_JACK_CLIENT))
#define AGS_IS_JACK_CLIENT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_JACK_CLIENT))
#define AGS_JACK_CLIENT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_JACK_CLIENT, AgsJackClientClass))

#define AGS_JACK_CLIENT_GET_OBJ_MUTEX(obj) (&(((AgsJackClient *) obj)->obj_mutex))

typedef struct _AgsJackClient AgsJackClient;
typedef struct _AgsJackClientClass AgsJackClientClass;

/**
 * AgsJackClientFlags:
 * @AGS_JACK_CLIENT_ACTIVATED: the client was activated
 * 
 * Enum values to control the behavior or indicate internal state of #AgsJackClient by
 * enable/disable as flags.
 */
typedef enum{
  AGS_JACK_CLIENT_ACTIVATED          = 1,
}AgsJackClientFlags;

struct _AgsJackClient
{
  GObject gobject;

  guint flags;
  guint connectable_flags;

  GRecMutex obj_mutex;

  GObject *jack_server;
  
  AgsUUID *uuid;

  gchar *client_uuid;
  gchar *client_name;
  
#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif

  GList *device;
  GList *port;
  
  volatile guint queued;
};

struct _AgsJackClientClass
{
  GObjectClass gobject;
};

GType ags_jack_client_get_type();
GType ags_jack_client_flags_get_type();

gboolean ags_jack_client_test_flags(AgsJackClient *jack_client, guint flags);
void ags_jack_client_set_flags(AgsJackClient *jack_client, guint flags);
void ags_jack_client_unset_flags(AgsJackClient *jack_client, guint flags);

GList* ags_jack_client_find_uuid(GList *jack_client,
				 gchar *client_uuid);
GList* ags_jack_client_find(GList *jack_client,
			    gchar *client_name);

void ags_jack_client_open(AgsJackClient *jack_client,
			  gchar *client_name);
void ags_jack_client_close(AgsJackClient *jack_client);

void ags_jack_client_add_device(AgsJackClient *jack_client,
				GObject *jack_device);
void ags_jack_client_remove_device(AgsJackClient *jack_client,
				   GObject *jack_device);

void ags_jack_client_add_port(AgsJackClient *jack_client,
			      GObject *jack_port);
void ags_jack_client_remove_port(AgsJackClient *jack_client,
				 GObject *jack_port);

void ags_jack_client_activate(AgsJackClient *jack_client);
void ags_jack_client_deactivate(AgsJackClient *jack_client);

AgsJackClient* ags_jack_client_new(GObject *jack_server);

G_END_DECLS

#endif /*__AGS_JACK_CLIENT_H__*/
