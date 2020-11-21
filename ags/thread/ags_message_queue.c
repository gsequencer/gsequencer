/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/thread/ags_message_queue.h>

#include <ags/thread/ags_message_envelope.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_message_queue_class_init(AgsMessageQueueClass *message_queue);
void ags_message_queue_init(AgsMessageQueue *message_queue);
void ags_message_queue_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_message_queue_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_message_queue_dispose(GObject *gobject);
void ags_message_queue_finalize(GObject *gobject);

/**
 * SECTION:ags_message_queue
 * @short_description: message queue
 * @title: AgsMessageQueue
 * @section_id:
 * @include: ags/thread/ags_message_queue.h
 *
 * The #AgsMessageQueue acts as messages passing system.
 */

enum{
  PROP_0,
  PROP_SENDER_NAMESPACE,
  PROP_RECIPIENT_NAMESPACE,
};

static gpointer ags_message_queue_parent_class = NULL;

GType
ags_message_queue_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_message_queue = 0;

    static const GTypeInfo ags_message_queue_info = {
      sizeof (AgsMessageQueueClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_message_queue_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMessageQueue),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_message_queue_init,
    };

    ags_type_message_queue = g_type_register_static(G_TYPE_OBJECT,
						    "AgsMessageQueue",
						    &ags_message_queue_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_message_queue);
  }

  return g_define_type_id__volatile;
}

void
ags_message_queue_class_init(AgsMessageQueueClass *message_queue)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_message_queue_parent_class = g_type_class_peek_parent(message_queue);

  /* GObjectClass */
  gobject = (GObjectClass *) message_queue;
  
  gobject->set_property = ags_message_queue_set_property;
  gobject->get_property = ags_message_queue_get_property;

  gobject->dispose = ags_message_queue_dispose;
  gobject->finalize = ags_message_queue_finalize;

  /* properties */
  /**
   * AgsMessageQueue:sender-namespace:
   *
   * The assigned namespace of sender.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("sender-namespace",
				   i18n_pspec("sender namespace of message queue"),
				   i18n_pspec("The sender's namespace this message queue is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SENDER_NAMESPACE,
				  param_spec);

  /**
   * AgsMessageQueue:recipient-namespace:
   *
   * The assigned namespace of recipient.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("recipient-namespace",
				   i18n_pspec("recipient namespace of message queue"),
				   i18n_pspec("The recipient's namespace this message queue is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECIPIENT_NAMESPACE,
				  param_spec);
}

void
ags_message_queue_init(AgsMessageQueue *message_queue)
{
  g_rec_mutex_init(&(message_queue->obj_mutex));

  message_queue->sender_namespace = NULL;
  message_queue->recipient_namespace = NULL;

  message_queue->message_envelope = NULL;
}

void
ags_message_queue_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsMessageQueue *message_queue;

  GRecMutex *message_queue_mutex;
  
  message_queue = AGS_MESSAGE_QUEUE(gobject);

  /* get message queue mutex */
  message_queue_mutex = AGS_MESSAGE_QUEUE_GET_OBJ_MUTEX(message_queue);
  
  switch(prop_id){
  case PROP_SENDER_NAMESPACE:
    {
      gchar *sender_namespace;

      sender_namespace = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(message_queue_mutex);
      
      if(message_queue->sender_namespace == sender_namespace){
	g_rec_mutex_unlock(message_queue_mutex);
	
	return;
      }
      
      if(message_queue->sender_namespace != NULL){
	g_free(message_queue->sender_namespace);
      }

      message_queue->sender_namespace = g_strdup(sender_namespace);

      g_rec_mutex_unlock(message_queue_mutex);
    }
    break;
  case PROP_RECIPIENT_NAMESPACE:
    {
      gchar *recipient_namespace;

      recipient_namespace = (gchar *) g_value_get_string(value);

      g_rec_mutex_lock(message_queue_mutex);
      
      if(message_queue->recipient_namespace == recipient_namespace){
	g_rec_mutex_unlock(message_queue_mutex);
	
	return;
      }
      
      if(message_queue->recipient_namespace != NULL){
	g_free(message_queue->recipient_namespace);
      }

      message_queue->recipient_namespace = g_strdup(recipient_namespace);

      g_rec_mutex_unlock(message_queue_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_message_queue_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsMessageQueue *message_queue;

  GRecMutex *message_queue_mutex;

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  /* get message queue mutex */
  message_queue_mutex = AGS_MESSAGE_QUEUE_GET_OBJ_MUTEX(message_queue);

  switch(prop_id){
  case PROP_SENDER_NAMESPACE:
    {
      g_rec_mutex_lock(message_queue_mutex);

      g_value_set_string(value, message_queue->sender_namespace);

      g_rec_mutex_unlock(message_queue_mutex);
    }
    break;
  case PROP_RECIPIENT_NAMESPACE:
    {
      g_rec_mutex_lock(message_queue_mutex);

      g_value_set_string(value, message_queue->recipient_namespace);

      g_rec_mutex_unlock(message_queue_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_message_queue_dispose(GObject *gobject)
{
  AgsMessageQueue *message_queue;

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  /* message */
  if(message_queue->message_envelope != NULL){
    g_list_free_full(message_queue->message_envelope,
		     (GDestroyNotify) g_object_unref);

    message_queue->message_envelope = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_message_queue_parent_class)->dispose(gobject);
}

void
ags_message_queue_finalize(GObject *gobject)
{
  AgsMessageQueue *message_queue;

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  g_free(message_queue->sender_namespace);
  g_free(message_queue->recipient_namespace);

  /* message */
  if(message_queue->message_envelope != NULL){
    g_list_free_full(message_queue->message_envelope,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_message_queue_parent_class)->finalize(gobject);
}

/**
 * ags_message_queue_set_sender_namespace:
 * @message_queue: the #AgsMessageQueue
 * @sender_namespace: the sender namespace
 * 
 * Set @sender_namespace of @message_queue.
 * 
 * Since: 3.6.16
 */
void
ags_message_queue_set_sender_namespace(AgsMessageQueue *message_queue,
				       gchar *sender_namespace)
{
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return;
  }
  
  g_object_set(message_queue,
	       "sender-namespace", sender_namespace,
	       NULL);
}

/**
 * ags_message_queue_get_sender_namespace:
 * @message_queue: the #AgsMessageQueue
 * 
 * Get sender namespace of @message_queue.
 * 
 * Returns: (transfer full): the sender namespace
 * 
 * Since: 3.6.16
 */
gchar*
ags_message_queue_get_sender_namespace(AgsMessageQueue *message_queue)
{
  gchar *sender_namespace;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }
  
  g_object_get(message_queue,
	       "sender-namespace", &sender_namespace,
	       NULL);

  return(sender_namespace);
}

/**
 * ags_message_queue_set_recipient_namespace:
 * @message_queue: the #AgsMessageQueue
 * @recipient_namespace: the recipient namespace
 * 
 * Set @recipient_namespace of @message_queue.
 * 
 * Since: 3.6.16
 */
void
ags_message_queue_set_recipient_namespace(AgsMessageQueue *message_queue,
					  gchar *recipient_namespace)
{
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return;
  }
  
  g_object_set(message_queue,
	       "recipient-namespace", recipient_namespace,
	       NULL);
}

/**
 * ags_message_queue_get_recipient_namespace:
 * @message_queue: the #AgsMessageQueue
 * 
 * Get recipient namespace of @message_queue.
 * 
 * Returns: (transfer full): the recipient namespace
 * 
 * Since: 3.6.16
 */
gchar*
ags_message_queue_get_recipient_namespace(AgsMessageQueue *message_queue)
{
  gchar *recipient_namespace;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }
  
  g_object_get(message_queue,
	       "recipient-namespace", &recipient_namespace,
	       NULL);

  return(recipient_namespace);
}

/**
 * ags_message_queue_set_message_envelope:
 * @message_queue: the #AgsMessageQueue
 * @message_envelope: (element-type Ags.MessageEnvelope) (transfer full): the #GList-struct containing #AgsMessageEnvelope
 * 
 * Set @message_envelope of @message_queue.
 * 
 * Since: 3.6.16
 */
void
ags_message_queue_set_message_envelope(AgsMessageQueue *message_queue,
				       GList *message_envelope)
{  
  GRecMutex *message_queue_mutex;

  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return;
  }

  /* get message queue mutex */
  message_queue_mutex = AGS_MESSAGE_QUEUE_GET_OBJ_MUTEX(message_queue);

  g_rec_mutex_lock(message_queue_mutex);

  message_queue->message_envelope = message_envelope;
  
  g_rec_mutex_unlock(message_queue_mutex);
}

/**
 * ags_message_queue_get_message_envelope:
 * @message_queue: the #AgsMessageQueue
 * 
 * Get message envelope of @message_queue.
 * 
 * Returns: (element-type Ags.MessageEnvelope) (transfer none): the #GList-struct containing #AgsMessageEnvelope
 * 
 * Since: 3.6.16
 */
GList*
ags_message_queue_get_message_envelope(AgsMessageQueue *message_queue)
{
  GList *message_envelope;
  
  GRecMutex *message_queue_mutex;

  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }

  /* get message queue mutex */
  message_queue_mutex = AGS_MESSAGE_QUEUE_GET_OBJ_MUTEX(message_queue);

  g_rec_mutex_lock(message_queue_mutex);

  message_envelope = message_queue->message_envelope;
  
  g_rec_mutex_unlock(message_queue_mutex);

  return(message_envelope);
}

/**
 * ags_message_queue_add_message_envelope:
 * @message_queue: the #AgsMessageQueue
 * @message_envelope: the #AgsMessageEnvelope
 * 
 * Add @message to @message_queue.
 * 
 * Since: 3.0.0
 */
void
ags_message_queue_add_message_envelope(AgsMessageQueue *message_queue,
				       GObject *message_envelope)
{
  if(!AGS_IS_MESSAGE_QUEUE(message_queue) ||
     !AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return;
  }

  g_rec_mutex_lock(&(message_queue->obj_mutex));

  if(g_list_find(message_queue->message_envelope, message_envelope) == NULL){
    message_queue->message_envelope = g_list_prepend(message_queue->message_envelope,
						     message_envelope);
    g_object_ref(message_envelope);
  }
  
  g_rec_mutex_unlock(&(message_queue->obj_mutex));
}

/**
 * ags_message_queue_remove_message_envelope:
 * @message_queue: the #AgsMessageQueue
 * @message_envelope: the #AgsMessageEnvelope
 * 
 * Remove @message from @message_queue.
 * 
 * Since: 3.0.0
 */
void
ags_message_queue_remove_message_envelope(AgsMessageQueue *message_queue,
					  GObject *message_envelope)
{
  if(!AGS_IS_MESSAGE_QUEUE(message_queue) ||
     !AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return;
  }

  g_rec_mutex_lock(&(message_queue->obj_mutex));

  if(g_list_find(message_queue->message_envelope, message_envelope) != NULL){
    message_queue->message_envelope = g_list_remove(message_queue->message_envelope,
						    message_envelope);
    g_object_unref(message_envelope);
  }
  
  g_rec_mutex_unlock(&(message_queue->obj_mutex));
}

/**
 * ags_message_queue_find_sender:
 * @message_queue: the #AgsMessageQueue
 * @sender: the sender as #GObject
 * 
 * Find @sender in @message_queue's messages.
 * 
 * Returns: (element-type Ags.MessageEnvelope) (transfer full): all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_message_queue_find_sender(AgsMessageQueue *message_queue,
			      GObject *sender)
{
  GList *start_message_envelope, *message_envelope;
  GList *match;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }
  
  g_rec_mutex_lock(&(message_queue->obj_mutex));
 
  message_envelope = 
    start_message_envelope = g_list_copy_deep(message_queue->message_envelope,
					      (GCopyFunc) g_object_ref,
					      NULL);
  
  g_rec_mutex_unlock(&(message_queue->obj_mutex));

  match = NULL;

  while(message_envelope != NULL){
    GRecMutex *message_envelope_mutex;

    message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope->data);

    g_rec_mutex_lock(message_envelope_mutex);
    
    if(AGS_MESSAGE_ENVELOPE(message_envelope->data)->sender == sender){
      match = g_list_prepend(match,
			     message_envelope->data);
      g_object_ref(message_envelope->data);
    }

    g_rec_mutex_unlock(message_envelope_mutex);
    
    message_envelope = message_envelope->next;
  }

  g_list_free_full(start_message_envelope,
		   g_object_unref);
  
  return(match);
}

/**
 * ags_message_queue_find_recipient:
 * @message_queue: the #AgsMessageQueue
 * @recipient: the recipient as #GObject
 * 
 * Find @recipient in @message_queue's messages.
 * 
 * Returns: (element-type Ags.MessageEnvelope) (transfer full): all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_message_queue_find_recipient(AgsMessageQueue *message_queue,
				 GObject *recipient)
{
  GList *start_message_envelope, *message_envelope;
  GList *match;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }

  match = NULL;

  g_rec_mutex_lock(&(message_queue->obj_mutex));
 
  message_envelope = 
    start_message_envelope = g_list_copy_deep(message_queue->message_envelope,
					      (GCopyFunc) g_object_ref,
					      NULL);

  g_rec_mutex_unlock(&(message_queue->obj_mutex));

  while(message_envelope != NULL){
    GRecMutex *message_envelope_mutex;

    message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope->data);

    g_rec_mutex_lock(message_envelope_mutex);

    if(AGS_MESSAGE_ENVELOPE(message_envelope->data)->recipient == recipient){
      match = g_list_prepend(match,
			     message_envelope->data);
      g_object_ref(message_envelope->data);
    }

    g_rec_mutex_unlock(message_envelope_mutex);
    
    message_envelope = message_envelope->next;
  }

  g_list_free_full(start_message_envelope,
		   g_object_unref);
  
  return(match);
}

/**
 * ags_message_queue_query_message:
 * @message_queue: the #AgsMessageQueue
 * @xpath: the XPath as string
 * 
 * Query @xpath in @message_queue's messages.
 * 
 * Returns: (element-type Ags.MessageEnvelope) (transfer full): all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 3.0.0
 */
GList*
ags_message_queue_query_message(AgsMessageQueue *message_queue,
				gchar *xpath)
{
  GList *start_message_envelope, *message_envelope;
  GList *match;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }

  match = NULL;

  g_rec_mutex_lock(&(message_queue->obj_mutex));
 
  message_envelope =
    start_message_envelope = g_list_copy_deep(message_queue->message_envelope,
					      (GCopyFunc) g_object_ref,
					      NULL);
  
  g_rec_mutex_unlock(&(message_queue->obj_mutex));

  while(message_envelope != NULL){
    xmlXPathContext *xpath_context;
    xmlXPathObject *xpath_object;

    xmlNode **node;

    guint i;

    GRecMutex *message_envelope_mutex;

    message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope->data);

    g_rec_mutex_lock(message_envelope_mutex);

    xpath_context = xmlXPathNewContext(AGS_MESSAGE_ENVELOPE(message_envelope->data)->doc);
    xpath_object = xmlXPathEval((xmlChar *) xpath,
				xpath_context);
    
    if(xpath_object->nodesetval != NULL){
      node = xpath_object->nodesetval->nodeTab;
      
      for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
	if(node[i]->type == XML_ELEMENT_NODE){
	  match = g_list_prepend(match,
				 message_envelope->data);
	  
	  break;
	}
      }
    }

    g_rec_mutex_unlock(message_envelope_mutex);
    
    message_envelope = message_envelope->next;
  }

  g_list_free_full(start_message_envelope,
		   g_object_unref);

  return(match);
}

/**
 * ags_message_queue_new:
 * @sender_namespace: the sender namespace
 *
 * Create a new instance of #AgsMessageQueue.
 *
 * Returns: the new #AgsMessageQueue
 *
 * Since: 3.0.0
 */ 
AgsMessageQueue*
ags_message_queue_new(gchar *sender_namespace)
{
  AgsMessageQueue *message_queue;

  message_queue = (AgsMessageQueue *) g_object_new(AGS_TYPE_MESSAGE_QUEUE,
						   "sender-namespace", sender_namespace,
						   NULL);

  return(message_queue);
}
