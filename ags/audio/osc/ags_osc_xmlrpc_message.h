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

#ifndef __AGS_OSC_XMLRPC_MESSAGE_H__
#define __AGS_OSC_XMLRPC_MESSAGE_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libsoup/soup.h>

#include <ags/libags.h>

#include <ags/audio/osc/ags_osc_message.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_XMLRPC_MESSAGE                (ags_osc_xmlrpc_message_get_type ())
#define AGS_OSC_XMLRPC_MESSAGE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_XMLRPC_MESSAGE, AgsOscXmlrpcMessage))
#define AGS_OSC_XMLRPC_MESSAGE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_OSC_XMLRPC_MESSAGE, AgsOscXmlrpcMessageClass))
#define AGS_IS_OSC_XMLRPC_MESSAGE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_XMLRPC_MESSAGE))
#define AGS_IS_OSC_XMLRPC_MESSAGE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_XMLRPC_MESSAGE))
#define AGS_OSC_XMLRPC_MESSAGE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_OSC_XMLRPC_MESSAGE, AgsOscXmlrpcMessageClass))

typedef struct _AgsOscXmlrpcMessage AgsOscXmlrpcMessage;
typedef struct _AgsOscXmlrpcMessageClass AgsOscXmlrpcMessageClass;

struct _AgsOscXmlrpcMessage
{
  AgsOscMessage osc_message;

  SoupMessage *msg;
  GHashTable *query;

  gchar *resource_id;
};

struct _AgsOscXmlrpcMessageClass
{
  AgsOscMessageClass osc_message;
};

GType ags_osc_xmlrpc_message_get_type(void);

GList* ags_osc_xmlrpc_message_find_resource_id(GList *osc_xmlrpc_message,
					       gchar *resource_id);

/* instance */
AgsOscXmlrpcMessage* ags_osc_xmlrpc_message_new();

G_END_DECLS

#endif /*__AGS_OSC_XMLRPC_MESSAGE_H__*/
