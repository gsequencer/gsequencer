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

#include <ags/thread/ags_message_delivery.h>

#include <ags/object/ags_connectable.h>

void ags_message_delivery_class_init(AgsMessageDeliveryClass *message_delivery);
void ags_message_delivery_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_message_delivery_init(AgsMessageDelivery *message_delivery);
void ags_message_delivery_connect(AgsConnectable *connectable);
void ags_message_delivery_disconnect(AgsConnectable *connectable);
void ags_message_delivery_dispose(GObject *gobject);
void ags_message_delivery_finalize(GObject *gobject);

/**
 * SECTION:ags_message_delivery
 * @short_description: message delivery
 * @title: AgsMessageDelivery
 * @section_id:
 * @include: ags/thread/ags_message_delivery.h
 *
 * The #AgsMessageDelivery acts as messages passing system.
 */

AgsMessageDelivery *ags_message_delivery = NULL;

static gpointer ags_message_delivery_parent_class = NULL;

GType
ags_message_delivery_get_type()
{
  static GType ags_type_message_delivery = 0;

  if(!ags_type_message_delivery){
    static const GTypeInfo ags_message_delivery_info = {
      sizeof (AgsMessageDeliveryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_message_delivery_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMessageDelivery),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_message_delivery_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_message_delivery_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_message_delivery = g_type_register_static(AGS_TYPE_THREAD,
						       "AgsMessageDelivery",
						       &ags_message_delivery_info,
						       0);
    
    g_type_add_interface_static(ags_type_message_delivery,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return(ags_type_message_delivery);
}

void
ags_message_delivery_class_init(AgsMessageDeliveryClass *message_delivery)
{
  GObjectClass *gobject;

  ags_message_delivery_parent_class = g_type_class_peek_parent(message_delivery);

  gobject->dispose = ags_message_delivery_dispose;
  gobject->finalize = ags_message_delivery_finalize;
}

void
ags_message_delivery_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_message_delivery_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_message_delivery_connect;
  connectable->disconnect = ags_message_delivery_disconnect;
}

void
ags_message_delivery_init(AgsMessageDelivery *message_delivery)
{
  message_delivery->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(message_delivery->mutexattr);
  pthread_mutexattr_settype(message_delivery->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

  message_delivery->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(message_delivery->mutex,
		     message_delivery->mutexattr);

  message_delivery->message_queue = NULL;
}

void
ags_message_delivery_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_message_delivery_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_message_delivery_dispose(GObject *gobject)
{
  AgsMessageDelivery *message_delivery;

  message_delivery = AGS_MESSAGE_DELIVERY(gobject);

  /* message queue */
  if(message_delivery->message_queue != NULL){
    g_list_free_full(message_delivery->message_queue,
		     g_object_unref);

    message_delivery->message_queue = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_message_delivery_parent_class)->dispose(gobject);
}

void
ags_message_delivery_finalize(GObject *gobject)
{
  AgsMessageDelivery *message_delivery;

  message_delivery = AGS_MESSAGE_DELIVERY(gobject);

  /* mutex */
  pthread_mutexattr_destroy(message_delivery->mutexattr);
  free(message_delivery->mutexattr);

  pthread_mutex_destroy(message_delivery->mutex);
  free(message_delivery->mutex);

  /* message queue */
  if(message_delivery->message_queue != NULL){
    g_list_free_full(message_delivery->message_queue,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_message_delivery_parent_class)->finalize(gobject);
}

/**
 * ags_message_delivery_add_queue:
 * @message_delivery: the #AgsMessageDelivery
 * @message_queue: the #AgsMessageQueue
 *
 * Add @message_queue to @message_delivery.
 * 
 * Since: 1.2.0
 */
void
ags_message_delivery_add_queue(AgsMessageDelivery *message_delivery,
			       GObject *message_queue)
{
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery) ||
     !AGS_IS_MESSAGE_QUEUE(message_queue)){
    return;
  }

  pthread_mutex_lock(message_delivery->mutex);

  message_delivery->message_queue = g_list_prepend(message_delivery->message_queue,
						   message_queue);
  
  pthread_mutex_unlock(message_delivery->mutex);
}

/**
 * ags_message_delivery_remove_queue:
 * @message_delivery: the #AgsMessageDelivery
 * @message_queue: the #AgsMessageQueue
 *
 * Remove @message_queue to @message_delivery.
 * 
 * Since: 1.2.0
 */
void
ags_message_delivery_remove_queue(AgsMessageDelivery *message_delivery,
				  GObject *message_queue)
{
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery) ||
     !AGS_IS_MESSAGE_QUEUE(message_queue)){
    return;
  }

  pthread_mutex_lock(message_delivery->mutex);

  message_delivery->message_queue = g_list_remove(message_delivery->message_queue,
						  message_queue);
  
  pthread_mutex_unlock(message_delivery->mutex);
}

/**
 * ags_message_delivery_find_namespace:
 * @message_delivery: the #AgsMessageDelivery to search
 * @namespace: the namespace as string to find
 * 
 * Find namespace of #AgsMessageQueue in @message_delivery.
 * 
 * Returns: the matching #AgsMessageQueue
 * 
 * Since: 1.2.0
 */
GObject*
ags_message_delivery_find_namespace(AgsMessageDelivery *message_delivery,
				    gchar *namespace)
{
  AgsMessageQueue *message_queue;

  GList *list;
  
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery) ||
     namespace == NULL){
    return(NULL);
  }

  pthread_mutex_lock(message_delivery->mutex);

  list = message_delivery->message_queue;
  
  pthread_mutex_unlock(message_delivery->mutex);

  while(list != NULL){
    pthread_mutex_lock(message_delivery->mutex);

    message_queue = AGS_MESSAGE_QUEUE(list->data);
    
    pthread_mutex_unlock(message_delivery->mutex);

    /* compare namespace */
    pthread_mutex_lock(message_queue->mutex);
    
    if(!g_strcmp0(namespace,
		  message_queue->namespace)){
      pthread_mutex_unlock(message_queue->mutex);

      return(message_queue);
    }

    pthread_mutex_unlock(message_queue->mutex);
    
    /* iterate */
    pthread_mutex_lock(message_delivery->mutex);
    
    list = list->next;

    pthread_mutex_unlock(message_delivery->mutex);
  }
  
  return(NULL);
}

/**
 * ags_message_delivery_add_message:
 * @message_delivery: the #AgsMessageDelivery
 * @namespace: the namespace as string
 * @message: the #AgsMessageEnvelope
 * 
 * Add @message to an #AgsMessageQueue specified by @namespace.
 * 
 * Since: 1.2.0
 */
void
ags_message_delivery_add_message(AgsMessageDelivery *message_delivery,
				 gchar *namespace,
				 gpointer message)
{
  AgsMessageQueue *message_queue;
  
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery) ||
     namespace == NULL){
    return;
  }

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      namespace);

  if(message_queue == NULL){
    message_queue = ags_message_queue_new(namespace);
    ags_message_delivery_add_queue(message_delivery,
				   message_queue);
  }
  
  ags_message_queue_add_message(message_queue,
				message);  
}

/**
 * ags_message_delivery_remove_message:
 * @message_delivery: the #AgsMessageDelivery
 * @namespace: the namespace as string
 * @message: the #AgsMessageEnvelope
 * 
 * Remove @message from an #AgsMessageQueue specified by @namespace.
 * 
 * Since: 1.2.0
 */
void
ags_message_delivery_remove_message(AgsMessageDelivery *message_delivery,
				    gchar *namespace,
				    gpointer message)
{
  AgsMessageQueue *message_queue;
  
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery) ||
     namespace == NULL){
    return;
  }

  message_queue = ags_message_delivery_find_namespace(message_delivery,
						      namespace);
  
  ags_message_queue_remove_message(message_queue,
				   message);  
}

/**
 * ags_message_delivery_find_sender:
 * @message_delivery: the #AgsMessageDelivery
 * @namespace: the namespace as string, maybe %NULL
 * @sender: the sender as #GObject
 * 
 * Find @sender for @namespace matching #AgsMessageQueue. If @namespace equals %NULL match
 * all available message queues.
 * 
 * Returns: all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 1.2.0
 */
GList*
ags_message_delivery_find_sender(AgsMessageDelivery *message_delivery,
				 gchar *namespace,
				 GObject *sender)
{
  AgsMessageQueue *message_queue;

  GList *list;
  GList *current_match, *match;
  
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery)){
    return;
  }

  match = NULL;
  
  if(namespace == NULL){
    pthread_mutex_lock(message_delivery->mutex);

    list = message_delivery->message_queue;

    pthread_mutex_unlock(message_delivery->mutex);

    while(list != NULL){
      pthread_mutex_lock(message_delivery->mutex);

      message_queue = list->data;
      
      pthread_mutex_unlock(message_delivery->mutex);

      current_match = ags_message_queue_find_sender(message_queue,
						    sender);

      if(current_match != NULL){
	if(match == NULL){
	  match = current_match;
	}else{
	  match = g_list_concat(current_match,
				match);
	}
      }
      
      /* iterate */
      pthread_mutex_lock(message_delivery->mutex);
      
      list = list->next;

      pthread_mutex_unlock(message_delivery->mutex);
    }
  }else{
    message_queue = ags_message_delivery_find_namespace(message_delivery,
							namespace);

    match = ags_message_queue_find_sender(message_queue,
					  sender);
  }

  return(match);
}

/**
 * ags_message_delivery_find_recipient:
 * @message_delivery: the #AgsMessageDelivery
 * @namespace: the namespace as string, maybe %NULL
 * @recipient: the recipient as #GObject
 * 
 * Find @recipient for @namespace matching #AgsMessageQueue. If @namespace equals %NULL match
 * all available message queues.
 * 
 * Returns: all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 1.2.0
 */
GList*
ags_message_delivery_find_recipient(AgsMessageDelivery *message_delivery,
				    gchar *namespace,
				    GObject *recipient)
{
  AgsMessageQueue *message_queue;

  GList *list;
  GList *current_match, *match;
  
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery)){
    return;
  }

  match = NULL;
  
  if(namespace == NULL){
    pthread_mutex_lock(message_delivery->mutex);

    list = message_delivery->message_queue;

    pthread_mutex_unlock(message_delivery->mutex);

    while(list != NULL){
      pthread_mutex_lock(message_delivery->mutex);

      message_queue = list->data;
      
      pthread_mutex_unlock(message_delivery->mutex);

      current_match = ags_message_queue_find_recipient(message_queue,
						       recipient);

      if(current_match != NULL){
	if(match == NULL){
	  match = current_match;
	}else{
	  match = g_list_concat(current_match,
				match);
	}
      }
      
      /* iterate */
      pthread_mutex_lock(message_delivery->mutex);
      
      list = list->next;

      pthread_mutex_unlock(message_delivery->mutex);
    }
  }else{
    message_queue = ags_message_delivery_find_namespace(message_delivery,
							namespace);

    match = ags_message_queue_find_recipient(message_queue,
					     recipient);
  }

  return(match);
}

/**
 * ags_message_delivery_query_message:
 * @message_delivery: the #AgsMessageDelivery
 * @namespace: the namespace as string, maybe %NULL
 * @xpath: the xpath to query
 * 
 * Query @xpath for @namespace matching #AgsMessageQueue. If @namespace equals %NULL match
 * all available message queues.
 * 
 * Returns: all matching #AgsMessageEnvelope as #GList-struct
 * 
 * Since: 1.2.0
 */
GList*
ags_message_delivery_query_message(AgsMessageDelivery *message_delivery,
				   gchar *namespace,
				   gchar *xpath)
{
  AgsMessageQueue *message_queue;

  GList *list;
  GList *current_match, *match;
  
  if(!AGS_IS_MESSAGE_DELIVERY(message_delivery)){
    return;
  }

  match = NULL;
  
  if(namespace == NULL){
    pthread_mutex_lock(message_delivery->mutex);

    list = message_delivery->message_queue;

    pthread_mutex_unlock(message_delivery->mutex);

    while(list != NULL){
      pthread_mutex_lock(message_delivery->mutex);

      message_queue = list->data;
      
      pthread_mutex_unlock(message_delivery->mutex);

      current_match = ags_message_queue_query_message(message_queue,
						      recipient);

      if(current_match != NULL){
	if(match == NULL){
	  match = current_match;
	}else{
	  match = g_list_concat(current_match,
				match);
	}
      }
      
      /* iterate */
      pthread_mutex_lock(message_delivery->mutex);
      
      list = list->next;

      pthread_mutex_unlock(message_delivery->mutex);
    }
  }else{
    message_queue = ags_message_delivery_find_namespace(message_delivery,
							namespace);

    match = ags_message_queue_query_message(message_queue,
					    recipient);
  }

  return(match);
}

/**
 * ags_message_delivery_get_instance:
 *
 * Get the #AgsMessageDelivery instance.
 *
 * Returns: the #AgsMessageDelivery
 *
 * Since: 1.2.0
 */ 
AgsMessageDelivery*
ags_message_delivery_get_instance()
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(ags_message_delivery == NULL){
    ags_message_delivery = ags_message_delivery_new();
  }
  
  pthread_mutex_unlock(&mutex);

  return(ags_message_delivery);
}

/**
 * ags_message_delivery_new:
 *
 * Create a new #AgsMessageDelivery.
 *
 * Returns: the new #AgsMessageDelivery
 *
 * Since: 1.2.0
 */ 
AgsMessageDelivery*
ags_message_delivery_new()
{
  AgsMessageDelivery *message_delivery;

  message_delivery = (AgsMessageDelivery *) g_object_new(AGS_TYPE_MESSAGE_DELIVERY,
							 NULL);


  return(message_delivery);
}
