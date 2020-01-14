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

#ifndef __AGS_MESSAGE_QUEUE_H__
#define __AGS_MESSAGE_QUEUE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

G_BEGIN_DECLS

#define AGS_TYPE_MESSAGE_QUEUE                (ags_message_queue_get_type())
#define AGS_MESSAGE_QUEUE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MESSAGE_QUEUE, AgsMessageQueue))
#define AGS_MESSAGE_QUEUE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MESSAGE_QUEUE, AgsMessageQueue))
#define AGS_IS_MESSAGE_QUEUE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MESSAGE_QUEUE))
#define AGS_IS_MESSAGE_QUEUE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MESSAGE_QUEUE))
#define AGS_MESSAGE_QUEUE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MESSAGE_QUEUE, AgsMessageQueueClass))

#define AGS_MESSAGE_QUEUE_GET_OBJ_MUTEX(obj) (&(((AgsMessageQueue *) obj)->obj_mutex))

typedef struct _AgsMessageQueue AgsMessageQueue;
typedef struct _AgsMessageQueueClass AgsMessageQueueClass;

struct _AgsMessageQueue
{
  GObject gobject;
  
  GRecMutex obj_mutex;

  gchar *sender_namespace;
  gchar *recipient_namespace;
  
  GList *message_envelope;
};

struct _AgsMessageQueueClass
{
  GObjectClass gobject; 
};

GType ags_message_queue_get_type();

void ags_message_queue_add_message_envelope(AgsMessageQueue *message_queue,
					    GObject *message_envelope);
void ags_message_queue_remove_message_envelope(AgsMessageQueue *message_queue,
					       GObject *message_envelope);

GList* ags_message_queue_find_sender(AgsMessageQueue *message_queue,
				     GObject *sender);
GList* ags_message_queue_find_recipient(AgsMessageQueue *message_queue,
					GObject *recipient);

GList* ags_message_queue_query_message(AgsMessageQueue *message_queue,
				       gchar *xpath);

AgsMessageQueue* ags_message_queue_new(gchar *sender_namespace);

G_END_DECLS

#endif /*__AGS_MESSAGE_QUEUE_H__*/
