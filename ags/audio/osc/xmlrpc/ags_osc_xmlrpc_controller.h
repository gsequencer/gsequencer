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

#ifndef __AGS_OSC_XMLRPC_CONTROLLER_H__
#define __AGS_OSC_XMLRPC_CONTROLLER_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libsoup/soup.h>

#include <ags/libags.h>

G_BEGIN_DECLS

#define AGS_TYPE_OSC_XMLRPC_CONTROLLER                (ags_osc_xmlrpc_controller_get_type())
#define AGS_OSC_XMLRPC_CONTROLLER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_OSC_XMLRPC_CONTROLLER, AgsOscXmlrpcController))
#define AGS_OSC_XMLRPC_CONTROLLER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_OSC_XMLRPC_CONTROLLER, AgsOscXmlrpcControllerClass))
#define AGS_IS_OSC_XMLRPC_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_OSC_XMLRPC_CONTROLLER))
#define AGS_IS_OSC_XMLRPC_CONTROLLER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_OSC_XMLRPC_CONTROLLER))
#define AGS_OSC_XMLRPC_CONTROLLER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_OSC_XMLRPC_CONTROLLER, AgsOscXmlrpcControllerClass))

typedef struct _AgsOscXmlrpcController AgsOscXmlrpcController;
typedef struct _AgsOscXmlrpcControllerClass AgsOscXmlrpcControllerClass;

typedef enum{
  AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_STARTED        = 1,
  AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_RUNNING        = 1 <<  1,
  AGS_OSC_XMLRPC_CONTROLLER_DELEGATE_TERMINATING    = 1 <<  2,
}AgsOscXmlrpcControllerFlags;

struct _AgsOscXmlrpcController
{
  AgsController controller;

  guint flags;

  GObject *osc_xmlrpc_server;

  gint64 delegate_timeout;
  
  volatile gboolean do_reset;

  GMutex delegate_mutex;
  GCond delegate_cond;
  
  GThread *delegate_thread;

  GList *message;
};

struct _AgsOscXmlrpcControllerClass
{
  AgsControllerClass controller;

  void (*start_delegate)(AgsOscXmlrpcController *osc_xmlrpc_controller);
  void (*stop_delegate)(AgsOscXmlrpcController *osc_xmlrpc_controller);
};

GType ags_osc_xmlrpc_controller_get_type();

gboolean ags_osc_xmlrpc_controller_test_flags(AgsOscXmlrpcController *osc_xmlrpc_controller, guint flags);
void ags_osc_xmlrpc_controller_set_flags(AgsOscXmlrpcController *osc_xmlrpc_controller, guint flags);
void ags_osc_xmlrpc_controller_unset_flags(AgsOscXmlrpcController *osc_xmlrpc_controller, guint flags);

void ags_osc_xmlrpc_controller_add_message(AgsOscXmlrpcController *osc_xmlrpc_controller,
					  GObject *message);
void ags_osc_xmlrpc_controller_remove_message(AgsOscXmlrpcController *osc_xmlrpc_controller,
					     GObject *message);

void ags_osc_xmlrpc_controller_start_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller);
void ags_osc_xmlrpc_controller_stop_delegate(AgsOscXmlrpcController *osc_xmlrpc_controller);

AgsOscXmlrpcController* ags_osc_xmlrpc_controller_new();

G_END_DECLS

#endif /*__AGS_OSC_XMLRPC_CONTROLLER_H__*/
