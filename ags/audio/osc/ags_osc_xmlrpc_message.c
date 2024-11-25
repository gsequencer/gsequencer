/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/osc/ags_osc_xmlrpc_message.h>

#include <ags/i18n.h>

void ags_osc_xmlrpc_message_class_init(AgsOscXmlrpcMessageClass *osc_xmlrpc_message);
void ags_osc_xmlrpc_message_init(AgsOscXmlrpcMessage *osc_xmlrpc_message);
void ags_osc_xmlrpc_message_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_osc_xmlrpc_message_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_osc_xmlrpc_message_dispose(GObject *gobject);
void ags_osc_xmlrpc_message_finalize(GObject *gobject);

/**
 * SECTION:ags_osc_xmlrpc_message
 * @short_description: the OSC server side XMLRPC message
 * @title: AgsOscXmlrpcMessage
 * @section_id:
 * @include: ags/audio/osc/ags_osc_xmlrpc_message.h
 *
 * #AgsOscXmlrpcMessage your OSC server side XMLRPC message.
 */

enum{
  PROP_0,
  PROP_SERVER_MSG,
  PROP_QUERY,
};

static gpointer ags_osc_xmlrpc_message_parent_class = NULL;

GType
ags_osc_xmlrpc_message_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_osc_xmlrpc_message = 0;

    static const GTypeInfo ags_osc_xmlrpc_message_info = {
      sizeof (AgsOscXmlrpcMessageClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_xmlrpc_message_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscXmlrpcMessage),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_xmlrpc_message_init,
    };

    ags_type_osc_xmlrpc_message = g_type_register_static(AGS_TYPE_OSC_MESSAGE,
							 "AgsOscXmlrpcMessage", &ags_osc_xmlrpc_message_info,
							 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_osc_xmlrpc_message);
  }

  return g_define_type_id__static;
}

void
ags_osc_xmlrpc_message_class_init(AgsOscXmlrpcMessageClass *osc_xmlrpc_message)
{
  GObjectClass *gobject;
  
  GParamSpec *param_spec;

  ags_osc_xmlrpc_message_parent_class = g_type_class_peek_parent(osc_xmlrpc_message);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_xmlrpc_message;
  
  gobject->set_property = ags_osc_xmlrpc_message_set_property;
  gobject->get_property = ags_osc_xmlrpc_message_get_property;
  
  gobject->dispose = ags_osc_xmlrpc_message_dispose;
  gobject->finalize = ags_osc_xmlrpc_message_finalize;

  /* properties */
  /**
   * AgsOscXmlrpcMessage:server-msg:
   *
   * The assigned #SoupMessage.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_object("server-msg",
				   i18n_pspec("assigned soup server message"),
				   i18n_pspec("The soup server message it is assigned with"),
				   SOUP_TYPE_SERVER_MESSAGE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SERVER_MSG,
				  param_spec);

  /**
   * AgsOscXmlrpcMessage:query:
   *
   * The current query.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("query",
				    i18n_pspec("query"),
				    i18n_pspec("The current query"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_QUERY,
				  param_spec);

  /* signals */
}

void
ags_osc_xmlrpc_message_init(AgsOscXmlrpcMessage *osc_xmlrpc_message)
{
  osc_xmlrpc_message->server_msg = NULL;

  osc_xmlrpc_message->query = NULL;
}

void
ags_osc_xmlrpc_message_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;

  GRecMutex *osc_message_mutex;

  osc_xmlrpc_message = AGS_OSC_XMLRPC_MESSAGE(gobject);

  /* get osc message mutex */
  osc_message_mutex = AGS_OSC_MESSAGE_GET_OBJ_MUTEX(osc_xmlrpc_message);
  
  switch(prop_id){
  case PROP_SERVER_MSG:
  {
    GObject *server_msg;

    server_msg = g_value_get_object(value);

    g_rec_mutex_lock(osc_message_mutex);

    if(osc_xmlrpc_message->server_msg == server_msg){
      g_rec_mutex_unlock(osc_message_mutex);

      return;
    }

    if(osc_xmlrpc_message->server_msg != NULL){
      g_object_unref(osc_xmlrpc_message->server_msg);
    }
      
    if(server_msg != NULL){
      g_object_ref(server_msg);
    }
      
    osc_xmlrpc_message->server_msg = (SoupServerMessage *) server_msg;

    g_rec_mutex_unlock(osc_message_mutex);
  }
  break;
  case PROP_QUERY:
  {
    GHashTable *query;

    query = g_value_get_pointer(value);

    g_rec_mutex_lock(osc_message_mutex);

    osc_xmlrpc_message->query = query;

    g_rec_mutex_unlock(osc_message_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_xmlrpc_message_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;

  GRecMutex *osc_message_mutex;

  osc_xmlrpc_message = AGS_OSC_XMLRPC_MESSAGE(gobject);

  /* get osc message mutex */
  osc_message_mutex = AGS_OSC_MESSAGE_GET_OBJ_MUTEX(osc_xmlrpc_message);
  
  switch(prop_id){
  case PROP_SERVER_MSG:
  {
    g_rec_mutex_lock(osc_message_mutex);

    g_value_set_object(value, osc_xmlrpc_message->server_msg);

    g_rec_mutex_unlock(osc_message_mutex);
  }
  break;
  case PROP_QUERY:
  {
    g_rec_mutex_lock(osc_message_mutex);

    g_value_set_pointer(value, osc_xmlrpc_message->query);

    g_rec_mutex_unlock(osc_message_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_xmlrpc_message_dispose(GObject *gobject)
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;
    
  osc_xmlrpc_message = (AgsOscXmlrpcMessage *) gobject;  

  if(osc_xmlrpc_message->server_msg != NULL){
    g_object_unref(osc_xmlrpc_message->server_msg);

    osc_xmlrpc_message->server_msg = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_message_parent_class)->dispose(gobject);
}

void
ags_osc_xmlrpc_message_finalize(GObject *gobject)
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;
    
  osc_xmlrpc_message = (AgsOscXmlrpcMessage *) gobject;

  if(osc_xmlrpc_message->server_msg != NULL){
    g_object_unref(osc_xmlrpc_message->server_msg);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_osc_xmlrpc_message_parent_class)->finalize(gobject);
}

/**
 * ags_osc_xmlrpc_message_new:
 * 
 * Creates a new instance of #AgsOscXmlrpcMessage
 *
 * Returns: the new #AgsOscXmlrpcMessage
 * 
 * Since: 3.0.0
 */
AgsOscXmlrpcMessage*
ags_osc_xmlrpc_message_new()
{
  AgsOscXmlrpcMessage *osc_xmlrpc_message;
   
  osc_xmlrpc_message = (AgsOscXmlrpcMessage *) g_object_new(AGS_TYPE_OSC_XMLRPC_MESSAGE,
							    NULL);
  
  return(osc_xmlrpc_message);
}
