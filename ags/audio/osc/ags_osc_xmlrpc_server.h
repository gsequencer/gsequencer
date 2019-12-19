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

#ifndef __AGS_OSC_XMLRPC_SERVER_H__
#define __AGS_OSC_XMLRPC_SERVER_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_server.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_XMLRPC_SERVER                (ags_osc_xmlrpc_server_get_type ())
#define AGS_OSC_XMLRPC_SERVER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_XMLRPC_SERVER, AgsOscXmlrpcServer))
#define AGS_OSC_XMLRPC_SERVER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_XMLRPC_SERVER, AgsOscXmlrpcServerClass))
#define AGS_IS_OSC_XMLRPC_SERVER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_XMLRPC_SERVER))
#define AGS_IS_OSC_XMLRPC_SERVER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_XMLRPC_SERVER))
#define AGS_OSC_XMLRPC_SERVER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_XMLRPC_SERVER, AgsOscXmlrpcServerClass))

typedef struct _AgsOscXmlrpcServer AgsOscXmlrpcServer;
typedef struct _AgsOscXmlrpcServerClass AgsOscXmlrpcServerClass;

struct _AgsOscXmlrpcServer
{
  AgsOscServer osc_server;
};

struct _AgsOscXmlrpcServerClass
{
  AgsOscServerClass osc_server;
};

GType ags_osc_xmlrpc_server_get_type(void);

/* instance */
AgsOscXmlrpcServer* ags_osc_xmlrpc_server_new();

G_END_DECLS

#endif /*__AGS_OSC_XMLRPC_SERVER_H__*/
