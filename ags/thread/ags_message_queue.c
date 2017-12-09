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

#include <ags/thread/ags_message_queue.h>

#include <ags/object/ags_connectable.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_message_queue_class_init(AgsMessageQueueClass *message_queue);
void ags_message_queue_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_message_queue_init(AgsMessageQueue *message_queue);
void ags_message_queue_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_message_queue_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_message_queue_connect(AgsConnectable *connectable);
void ags_message_queue_disconnect(AgsConnectable *connectable);
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
  PROP_NAMESPACE,
};

static gpointer ags_message_queue_parent_class = NULL;
static AgsConnectableInterface *ags_message_queue_parent_connectable_interface;

GType
ags_message_queue_get_type()
{
  static GType ags_type_message_queue = 0;

  if(!ags_type_message_queue){
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_message_queue_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_message_queue = g_type_register_static(G_TYPE_OBJECT,
						    "AgsMessageQueue",
						    &ags_message_queue_info,
						    0);
    
    g_type_add_interface_static(ags_type_message_queue,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_message_queue);
}

void
ags_message_queue_class_init(AgsMessageQueueClass *message_queue)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_message_queue_parent_class = g_type_class_peek_parent(message_queue);

  gobject->set_property = ags_message_queue_set_property;
  gobject->get_property = ags_message_queue_get_property;

  gobject->dispose = ags_message_queue_dispose;
  gobject->finalize = ags_message_queue_finalize;

  /* properties */
  /**
   * AgsMessageQueue:namespace:
   *
   * The assigned namespace.
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_string("namespace",
				   i18n_pspec("namespace of message queue"),
				   i18n_pspec("The namespace this message queue is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NAMESPACE,
				  param_spec);
}

void
ags_message_queue_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_message_queue_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_message_queue_connect;
  connectable->disconnect = ags_message_queue_disconnect;
}

void
ags_message_queue_init(AgsMessageQueue *message_queue)
{
  message_queue->namespace = NULL;
  
  message_queue->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(message_queue->mutexattr);
  pthread_mutexattr_settype(message_queue->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

  message_queue->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(message_queue->mutex,
		     message_queue->mutexattr);

  message_queue->message = NULL;
}

void
ags_message_queue_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsMessageQueue *message_queue;

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  switch(prop_id){
  case PROP_NAMESPACE:
    {
      gchar *namespace;

      namespace = (gchar *) g_value_get_string(value);

      if(message_queue->namespace == namespace){
	return;
      }
      
      if(message_queue->namespace != NULL){
	g_free(message_queue->namespace);
      }

      message_queue->namespace = g_strdup(namespace);
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

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  switch(prop_id){
  case PROP_NAMESPACE:
    {
      g_value_set_string(value, message_queue->namespace);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_message_queue_connect(AgsConnectable *connectable)
{
  /* empty */

  ags_message_queue_parent_connectable_interface->connect(connectable);
}

void
ags_message_queue_disconnect(AgsConnectable *connectable)
{
  ags_message_queue_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_message_queue_dispose(GObject *gobject)
{
  AgsMessageQueue *message_queue;

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_message_queue_parent_class)->dispose(gobject);
}

void
ags_message_queue_finalize(GObject *gobject)
{
  AgsMessageQueue *message_queue;

  message_queue = AGS_MESSAGE_QUEUE(gobject);

  g_free(message_queue->namespace);

  /* mutex */
  pthread_mutexattr_destroy(message_queue->mutexattr);
  free(message_queue->mutexattr);

  pthread_mutex_destroy(message_queue->mutex);
  free(message_queue->mutex);

  /* message */
  if(message_queue->message != NULL){
    g_list_free_full(message_queue->message,
		     ags_message_envelope_free);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_message_queue_parent_class)->finalize(gobject);
}

/**
 * ags_message_envelope_alloc:
 * @sender: the sender as #GObject
 * @recipient: the recipient as #GObject
 * @doc: the message document
 * 
 * Allocate #AgsMessageEnvelope.
 * 
 * Since: 1.2.0
 */
AgsMessageEnvelope*
ags_message_envelope_alloc(GObject *sender,
			   GObject *recipient,
			   xmlDoc *doc)
{
  AgsMessageEnvelope *message;

  message = (AgsMessageEnvelope *) malloc(sizeof(AgsMessageEnvelope));

  if(sender != NULL){
    g_object_ref(sender);
  }
  
  message->sender = sender;

  if(recipient != NULL){
    g_object_ref(recipient);
  }
  
  message->recipient = recipient;

  message->doc = doc;

  message->parameter = NULL;
  message->n_params = 0;
  
  return(message);
}

/**
 * ags_message_envelope_free:
 * @message: the #AgsMessageEnvelope
 * 
 * Free @message.
 * 
 * Since: 1.2.0
 */
void
ags_message_envelope_free(AgsMessageEnvelope *message)
{
  if(message == NULL){
    return;
  }

  if(message->sender != NULL){
    g_object_unref(message->sender);
  }

  if(message->recipient != NULL){
    g_object_unref(message->recipient);
  }
  
  if(message->doc != NULL){
    xmlFreeDoc(message->doc);
  }
  
  g_free(message->parameter);
  
  g_free(message);
}

/**
 * ags_message_queue_add_message:
 * @message_queue: the #AgsMessageQueue
 * @message: the #AgsMessageEnvelope
 * 
 * Add @message to @message_queue.
 * 
 * Since: 1.2.0
 */
void
ags_message_queue_add_message(AgsMessageQueue *message_queue,
			      gpointer message)
{
  if(!AGS_IS_MESSAGE_QUEUE(message_queue) ||
     message == NULL){
    return;
  }

  pthread_mutex_lock(message_queue->mutex);

  message_queue->message = g_list_prepend(message_queue->message,
					  message);
  
  pthread_mutex_unlock(message_queue->mutex);
}

/**
 * ags_message_queue_remove_message:
 * @message_queue: the #AgsMessageQueue
 * @message: the #AgsMessageEnvelope
 * 
 * Remove @message from @message_queue.
 * 
 * Since: 1.2.0
 */
void
ags_message_queue_remove_message(AgsMessageQueue *message_queue,
				 gpointer message)
{
  if(!AGS_IS_MESSAGE_QUEUE(message_queue) ||
     message == NULL){
    return;
  }


  pthread_mutex_lock(message_queue->mutex);

  message_queue->message = g_list_remove(message_queue->message,
					 message);
  
  pthread_mutex_unlock(message_queue->mutex);
}

/**
 * ags_message_queue_find_sender:
 * @message_queue: the #AgsMessageQueue
 * @sender: the sender as #GObject
 * 
 * Find @sender in @message_queue's messages.
 * 
 * Returns: all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 1.2.0
 */
GList*
ags_message_queue_find_sender(AgsMessageQueue *message_queue,
			      GObject *sender)
{
  GList *message;
  GList *match;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }

  match = NULL;

  pthread_mutex_lock(message_queue->mutex);
 
  message = message_queue->message;

  while(message != NULL){
    if(AGS_MESSAGE_ENVELOPE(message->data)->sender == sender){
      match = g_list_prepend(match,
			     message->data);
    }
    
    message = message->next;
  }
  
  pthread_mutex_unlock(message_queue->mutex);

  return(match);
}

/**
 * ags_message_queue_find_recipient:
 * @message_queue: the #AgsMessageQueue
 * @recipient: the recipient as #GObject
 * 
 * Find @recipient in @message_queue's messages.
 * 
 * Returns: all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 1.2.0
 */
GList*
ags_message_queue_find_recipient(AgsMessageQueue *message_queue,
				 GObject *recipient)
{
  GList *message;
  GList *match;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }

  match = NULL;

  pthread_mutex_lock(message_queue->mutex);
 
  message = message_queue->message;

  while(message != NULL){
    if(AGS_MESSAGE_ENVELOPE(message->data)->recipient == recipient){
      match = g_list_prepend(match,
			     message->data);
    }
    
    message = message->next;
  }
  
  pthread_mutex_unlock(message_queue->mutex);

  return(match);
}

/**
 * ags_message_queue_query_message:
 * @message_queue: the #AgsMessageQueue
 * @recipient: the recipient as #GObject
 * 
 * Query @xpath in @message_queue's messages.
 * 
 * Returns: all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 1.2.0
 */
GList*
ags_message_queue_query_message(AgsMessageQueue *message_queue,
				gchar *xpath)
{
  GList *message;
  GList *match;
  
  if(!AGS_IS_MESSAGE_QUEUE(message_queue)){
    return(NULL);
  }

  match = NULL;

  pthread_mutex_lock(message_queue->mutex);
 
  message = message_queue->message;

  while(message != NULL){
    xmlXPathContext *xpath_context;
    xmlXPathObject *xpath_object;

    xmlNode **node;

    guint i;

    xpath_context = xmlXPathNewContext(AGS_MESSAGE_ENVELOPE(message->data)->doc);
    xpath_object = xmlXPathEval((xmlChar *) xpath,
				xpath_context);
    
    if(xpath_object->nodesetval != NULL){
      node = xpath_object->nodesetval->nodeTab;
      
      for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
	if(node[i]->type == XML_ELEMENT_NODE){
	  match = g_list_prepend(match,
				 message->data);
	  
	  break;
	}
      }
    }
    
    message = message->next;
  }
  
  pthread_mutex_unlock(message_queue->mutex);

  return(match);
}

/**
 * ags_message_queue_new:
 *
 * Create a new #AgsMessageQueue.
 *
 * Returns: the new #AgsMessageQueue
 *
 * Since: 1.2.0
 */ 
AgsMessageQueue*
ags_message_queue_new(gchar *namespace)
{
  AgsMessageQueue *message_queue;

  message_queue = (AgsMessageQueue *) g_object_new(AGS_TYPE_MESSAGE_QUEUE,
						   "namespace", namespace,
						   NULL);


  return(message_queue);
}
