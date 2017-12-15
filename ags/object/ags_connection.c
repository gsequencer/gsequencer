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

#include <ags/object/ags_connection.h>

#include <ags/thread/ags_message_delivery.h>
#include <ags/thread/ags_message_queue.h>

void ags_connection_class_init(AgsConnectionClass *connection);
void ags_connection_init (AgsConnection *connection);
void ags_connection_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_connection_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_connection_dispose(GObject *gobject);
void ags_connection_finalize(GObject *gobject);

/**
 * SECTION:ags_connection
 * @short_description: Abstraction of connections
 * @title: AgsConnection
 * @section_id:
 * @include: ags/object/ags_connection.h
 *
 * The #AgsConnection provides you a data object and you might want to
 * use it in conjunction with #AgsConnectable to get connect with its
 * scope.
 */

static gpointer ags_connection_parent_class = NULL;

enum{
  PROP_0,
  PROP_DATA_OBJECT,
};

GType
ags_connection_get_type (void)
{
  static GType ags_type_connection = 0;

  if(!ags_type_connection){
    static const GTypeInfo ags_connection_info = {
      sizeof (AgsConnectionClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_connection_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsConnection),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_connection_init,
    };

    ags_type_connection = g_type_register_static(G_TYPE_OBJECT,
						 "AgsConnection",
						 &ags_connection_info,
						 0);
  }

  return (ags_type_connection);
}

void
ags_connection_class_init(AgsConnectionClass *connection)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_connection_parent_class = g_type_class_peek_parent(connection);

  /* GObjectClass */
  gobject = (GObjectClass *) connection;

  gobject->set_property = ags_connection_set_property;
  gobject->get_property = ags_connection_get_property;

  gobject->dispose = ags_connection_dispose;
  gobject->finalize = ags_connection_finalize;

  /* properties */
  /**
   * AgsConnection:data-object:
   *
   * The assigned #GObject acting as data object.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("data-object",
				   "assigned data object",
				   "The data object it is assigned with",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DATA_OBJECT,
				  param_spec);
}

void
ags_connection_init(AgsConnection *connection)
{
  connection->mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(connection->mutexattr);
  pthread_mutexattr_settype(connection->mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

  connection->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(connection->mutex,
		     connection->mutexattr);

  connection->data_object = NULL;
}

void
ags_connection_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsConnection *connection;

  connection = AGS_CONNECTION(gobject);

  switch(prop_id){
  case PROP_DATA_OBJECT:
    {
      GObject *data_object;

      AgsMessageDelivery *message_delivery;
      AgsMessageQueue *message_queue;
      
      data_object = (GObject *) g_value_get_object(value);

      if(connection->data_object == data_object){
	return;
      }

      if(connection->data_object != NULL){
	g_object_unref(connection->data_object);
      }

      if(data_object != NULL){
	g_object_ref(data_object);
      }

      connection->data_object = data_object;

      /* emit message */
      message_delivery = ags_message_delivery_get_instance();

      message_queue = ags_message_delivery_find_namespace(message_delivery,
							  "libags");

      if(message_queue != NULL){
	AgsMessageEnvelope *message;

	xmlDoc *doc;
	xmlNode *root_node;

	/* specify message body */
	doc = xmlNewDoc("1.0");

	root_node = xmlNewNode(NULL,
			       "ags-command");
	xmlDocSetRootElement(doc, root_node);    

	xmlNewProp(root_node,
		   "method",
		   "GObject::notify::data-object");

	/* add message */
	message = ags_message_envelope_alloc(connection,
					     NULL,
					     doc);

	/* set parameter */
	message->parameter = g_new0(GParameter,
				    1);
	message->n_params = 1;
    
	message->parameter[0].name = "data-object";
	g_value_init(&(message->parameter[0].value),
		     G_TYPE_OBJECT);
	g_value_set_object(&(message->parameter[0].value),
			   data_object);

	/* add message */
	ags_message_delivery_add_message(message_delivery,
					 "libags",
					 message);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_connection_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsConnection *connection;

  connection = AGS_CONNECTION(gobject);

  switch(prop_id){
  case PROP_DATA_OBJECT:
    {
      g_value_set_object(value, connection->data_object);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
  }
}

void
ags_connection_dispose(GObject *gobject)
{
  AgsConnection *connection;

  connection = AGS_CONNECTION(gobject);

  if(connection->data_object != NULL){
    g_object_unref(connection->data_object);

    connection->data_object = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_connection_parent_class)->dispose(gobject);
}

void
ags_connection_finalize(GObject *gobject)
{
  AgsConnection *connection;

  connection = AGS_CONNECTION(gobject);

  if(connection->data_object != NULL){
    g_object_unref(connection->data_object);
  }

  pthread_mutex_destroy(connection->mutex);
  free(connection->mutex);

  pthread_mutexattr_destroy(connection->mutexattr);
  free(connection->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_connection_parent_class)->finalize(gobject);
}

/**
 * ags_connection_find_type:
 * @connection: the #GList-struct to search
 * @connection_type: the #GType to find
 *
 * Find next matching @type within @connection.
 *
 * Returns: a matching #GList-struct
 * 
 * Since: 1.0.0
 */
GList*
ags_connection_find_type(GList *connection,
			 GType connection_type)
{
  if(connection == NULL ||
     connection_type == G_TYPE_NONE){
    return(NULL);
  }

  while(connection != NULL){
    /* check type */
    if(AGS_IS_CONNECTION(connection->data) &&
       g_type_is_a(G_OBJECT_TYPE(G_OBJECT(connection->data)),
				 connection_type)){
      return(connection);
    }

    connection = connection->next;
  }

  return(NULL);
}


/**
 * ags_connection_find_type_and_data_object_type:
 * @connection: the #GList-struct to search
 * @connection_type: the #GType to find
 * @data_object_type: the #GType to find
 *
 * Find next matching @connection_type and @data_object_type
 * within @connection.
 *
 * Returns: a matching #GList-struct
 *
 * Since: 1.0.0
 */
GList*
ags_connection_find_type_and_data_object_type(GList *connection,
					      GType connection_type,
					      GType data_object_type)
{
  GObject *data_object;
  
  if(connection == NULL ||
     connection_type == G_TYPE_NONE ||
     data_object_type == G_TYPE_NONE){
    return(NULL);
  }

  while(connection != NULL){
    if(!AGS_IS_CONNECTION(connection->data)){
      connection = connection->next;

      continue;
    }

    /* get data object */
    pthread_mutex_lock(AGS_CONNECTION(connection->data)->mutex);
    
    data_object = AGS_CONNECTION(connection->data)->data_object;
    
    pthread_mutex_unlock(AGS_CONNECTION(connection->data)->mutex);

    /* check type */
    if(g_type_is_a(G_OBJECT_TYPE(connection->data),
		   connection_type) &&
       g_type_is_a(G_OBJECT_TYPE(data_object),
		   data_object_type)){      
      return(connection);
    }

    connection = connection->next;
  }
  
  return(NULL);
}

/**
 * ags_connection_new:
 *
 * Creates an #AgsConnection
 *
 * Returns: a new #AgsConnection
 *
 * Since: 1.0.0
 */
AgsConnection*
ags_connection_new()
{
  AgsConnection *connection;

  connection = (AgsConnection *) g_object_new(AGS_TYPE_CONNECTION,
					      NULL);

  return(connection);
}
