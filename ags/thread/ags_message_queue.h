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

#ifndef __AGS_MESSAGE_QUEUE_H__
#define __AGS_MESSAGE_QUEUE_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <libxml/tree.h>

#define AGS_TYPE_MESSAGE_QUEUE                (ags_message_queue_get_type())
#define AGS_MESSAGE_QUEUE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MESSAGE_QUEUE, AgsMessageQueue))
#define AGS_MESSAGE_QUEUE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MESSAGE_QUEUE, AgsMessageQueue))
#define AGS_IS_MESSAGE_QUEUE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MESSAGE_QUEUE))
#define AGS_IS_MESSAGE_QUEUE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MESSAGE_QUEUE))
#define AGS_MESSAGE_QUEUE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_MESSAGE_QUEUE, AgsMessageQueueClass))

#define AGS_MESSAGE_ENVELOPE(ptr) ((AgsMessageEnvelope *)(ptr))

typedef struct _AgsMessageQueue AgsMessageQueue;
typedef struct _AgsMessageQueueClass AgsMessageQueueClass;
typedef struct _AgsMessageEnvelope AgsMessageEnvelope;

struct _AgsMessageQueue
{
  GObject gobject;

  gchar *namespace;

  pthread_mutexattr_t *mutexattr;
  pthread_mutex_t *mutex;

  GList *message;
};

struct _AgsMessageQueueClass
{
  GObjectClass gobject; 
};

struct _AgsMessageEnvelope
{
  GObject *sender;
  GObject *recipient;

  xmlDoc *doc;
};

GType ags_message_queue_get_type();

AgsMessageEnvelope* ags_message_envelope_alloc(GObject *sender,
					       GObject *recipient,
					       xmlDoc *doc);
void ags_message_envelope_free(AgsMessageEnvelope *message);

void ags_message_queue_add_message(AgsMessageQueue *message_queue,
				   gpointer message);
void ags_message_queue_remove_message(AgsMessageQueue *message_queue,
				      gpointer message);

GList* ags_message_queue_find_sender(AgsMessageQueue *message_queue,
				     GObject *sender);
GList* ags_message_queue_find_recipient(AgsMessageQueue *message_queue,
					GObject *recipient);

GList* ags_message_queue_query_message(AgsMessageQueue *message_queue,
				       gchar *xpath);

AgsMessageQueue* ags_message_queue_new(gchar *namespace);

#endif /*__AGS_MESSAGE_QUEUE_H__*/
