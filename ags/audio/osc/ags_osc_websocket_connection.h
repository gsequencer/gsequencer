/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_OSC_WEBSOCKET_CONNECTION_H__
#define __AGS_OSC_WEBSOCKET_CONNECTION_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libsoup/soup.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_connection.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_WEBSOCKET_CONNECTION                (ags_osc_websocket_connection_get_type ())
#define AGS_OSC_WEBSOCKET_CONNECTION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_WEBSOCKET_CONNECTION, AgsOscWebsocketConnection))
#define AGS_OSC_WEBSOCKET_CONNECTION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_WEBSOCKET_CONNECTION, AgsOscWebsocketConnectionClass))
#define AGS_IS_OSC_WEBSOCKET_CONNECTION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_WEBSOCKET_CONNECTION))
#define AGS_IS_OSC_WEBSOCKET_CONNECTION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_WEBSOCKET_CONNECTION))
#define AGS_OSC_WEBSOCKET_CONNECTION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_WEBSOCKET_CONNECTION, AgsOscWebsocketConnectionClass))

typedef struct _AgsOscWebsocketConnection AgsOscWebsocketConnection;
typedef struct _AgsOscWebsocketConnectionClass AgsOscWebsocketConnectionClass;

struct _AgsOscWebsocketConnection
{
  AgsOscConnection connection;

  SoupWebsocketConnection *websocket_connection;
  
  SoupClientContext *client;
  
  GObject *security_context;
  
  gchar *path;
  
  gchar *login;
  gchar *security_token;
};

struct _AgsOscWebsocketConnectionClass
{
  AgsOscConnectionClass connection;
};

GType ags_osc_websocket_connection_get_type(void);

/* instance */
AgsOscWebsocketConnection* ags_osc_websocket_connection_new(GObject *osc_server);

G_END_DECLS

#endif /*__AGS_OSC_WEBSOCKET_CONNECTION_H__*/
