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

#ifndef __AGS_MESSAGE_DELIVERY_H__
#define __AGS_MESSAGE_DELIVERY_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_MESSAGE_DELIVERY                (ags_message_delivery_get_type())
#define AGS_MESSAGE_DELIVERY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MESSAGE_DELIVERY, AgsMessageDelivery))
#define AGS_MESSAGE_DELIVERY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MESSAGE_DELIVERY, AgsMessageDelivery))
#define AGS_IS_MESSAGE_DELIVERY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MESSAGE_DELIVERY))
#define AGS_IS_MESSAGE_DELIVERY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MESSAGE_DELIVERY))
#define AGS_MESSAGE_DELIVERY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MESSAGE_DELIVERY, AgsMessageDeliveryClass))

#define AGS_MESSAGE_DELIVERY_GET_OBJ_MUTEX(obj) (&(((AgsMessageDelivery *) obj)->obj_mutex))

typedef struct _AgsMessageDelivery AgsMessageDelivery;
typedef struct _AgsMessageDeliveryClass AgsMessageDeliveryClass;

struct _AgsMessageDelivery
{
  GObject gobject;

  GRecMutex obj_mutex;

  GList *message_queue;
};

struct _AgsMessageDeliveryClass
{
  GObjectClass gobject; 
};

GType ags_message_delivery_get_type();

void ags_message_delivery_add_message_queue(AgsMessageDelivery *message_delivery,
					    GObject *message_queue);
void ags_message_delivery_remove_message_queue(AgsMessageDelivery *message_delivery,
					       GObject *message_queue);

GList* ags_message_delivery_find_sender_namespace(AgsMessageDelivery *message_delivery,
						  gchar *sender_namespace);

void ags_message_delivery_add_message_envelope(AgsMessageDelivery *message_delivery,
					       gchar *sender_namespace,
					       GObject *message_envelope);
void ags_message_delivery_remove_message_envelope(AgsMessageDelivery *message_delivery,
						  gchar *sender_namespace,
						  GObject *message_envelope);

GList* ags_message_delivery_find_sender(AgsMessageDelivery *message_delivery,
					gchar *sender_namespace,
					GObject *sender);
GList* ags_message_delivery_find_recipient(AgsMessageDelivery *message_delivery,
					   gchar *sender_namespace,
					   GObject *recipient);

GList* ags_message_delivery_query_message(AgsMessageDelivery *message_delivery,
					  gchar *sender_namespace,
					  gchar *xpath);

AgsMessageDelivery* ags_message_delivery_get_instance();

AgsMessageDelivery* ags_message_delivery_new();

G_END_DECLS

#endif /*__AGS_MESSAGE_DELIVERY_H__*/
