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

#include <ags/thread/ags_message_envelope.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_message_envelope_class_init(AgsMessageEnvelopeClass *message_envelope);
void ags_message_envelope_init(AgsMessageEnvelope *message_envelope);
void ags_message_envelope_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_message_envelope_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_message_envelope_dispose(GObject *gobject);
void ags_message_envelope_finalize(GObject *gobject);

/**
 * SECTION:ags_message_envelope
 * @short_description: message envelope
 * @title: AgsMessageEnvelope
 * @section_id:
 * @include: ags/thread/ags_message_envelope.h
 *
 * The #AgsMessageEnvelope acts as messages passing system.
 */

enum{
  PROP_0,
  PROP_SENDER,
  PROP_RECIPIENT,
  PROP_XML_DOC,
};

static gpointer ags_message_envelope_parent_class = NULL;

GType
ags_message_envelope_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_message_envelope = 0;

    static const GTypeInfo ags_message_envelope_info = {
      sizeof (AgsMessageEnvelopeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_message_envelope_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMessageEnvelope),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_message_envelope_init,
    };

    ags_type_message_envelope = g_type_register_static(G_TYPE_OBJECT,
						       "AgsMessageEnvelope",
						       &ags_message_envelope_info,
						       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_message_envelope);
  }

  return g_define_type_id__volatile;
}

void
ags_message_envelope_class_init(AgsMessageEnvelopeClass *message_envelope)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_message_envelope_parent_class = g_type_class_peek_parent(message_envelope);

  /* GObjectClass */
  gobject = (GObjectClass *) message_envelope;
  
  gobject->set_property = ags_message_envelope_set_property;
  gobject->get_property = ags_message_envelope_get_property;

  gobject->dispose = ags_message_envelope_dispose;
  gobject->finalize = ags_message_envelope_finalize;

  /* properties */
  /**
   * AgsMessageEnvelope:sender:
   *
   * The assigned sender.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("sender",
				   i18n_pspec("sender of message envelope"),
				   i18n_pspec("The sender of message envelope"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SENDER,
				  param_spec);

  /**
   * AgsMessageEnvelope:recipient:
   *
   * The assigned recipient.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("recipient",
				   i18n_pspec("recipient of message envelope"),
				   i18n_pspec("The recipient of message envelope"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECIPIENT,
				  param_spec);

  /**
   * AgsMessageEnvelope:xml-doc:
   *
   * The assigned xml-doc.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("xml-doc",
				    i18n_pspec("xml document of message envelope"),
				    i18n_pspec("The xml document provided by message envelope"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XML_DOC,
				  param_spec);
}

void
ags_message_envelope_init(AgsMessageEnvelope *message_envelope)
{
  g_rec_mutex_init(&(message_envelope->obj_mutex));

  message_envelope->sender = NULL;
  message_envelope->recipient = NULL;
  
  message_envelope->doc = NULL;

  message_envelope->n_params = 0;

  message_envelope->parameter_name = NULL;
  message_envelope->value = NULL;
}

void
ags_message_envelope_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsMessageEnvelope *message_envelope;

  GRecMutex *message_envelope_mutex;

  message_envelope = AGS_MESSAGE_ENVELOPE(gobject);

  /* get message envelope mutex */
  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  switch(prop_id){
  case PROP_SENDER:
  {
    GObject *sender;

    sender = g_value_get_object(value);

    g_rec_mutex_lock(message_envelope_mutex);

    if(message_envelope->sender == sender){
      g_rec_mutex_unlock(message_envelope_mutex);
	
      return;
    }

    if(message_envelope->sender != NULL){
      g_object_unref(G_OBJECT(message_envelope->sender));
    }

    if(sender != NULL){
      g_object_ref(G_OBJECT(sender));
    }

    message_envelope->sender = (GObject *) sender;

    g_rec_mutex_unlock(message_envelope_mutex);
  }
  break;
  case PROP_RECIPIENT:
  {
    GObject *recipient;

    recipient = g_value_get_object(value);

    g_rec_mutex_lock(message_envelope_mutex);

    if(message_envelope->recipient == recipient){
      g_rec_mutex_unlock(message_envelope_mutex);
	
      return;
    }

    if(message_envelope->recipient != NULL){
      g_object_unref(G_OBJECT(message_envelope->recipient));
    }

    if(recipient != NULL){
      g_object_ref(G_OBJECT(recipient));
    }

    message_envelope->recipient = (GObject *) recipient;

    g_rec_mutex_unlock(message_envelope_mutex);
  }
  break;
  case PROP_XML_DOC:
  {
    xmlDoc *doc;

    doc = (xmlDoc *) g_value_get_pointer(value);

    g_rec_mutex_lock(message_envelope_mutex);
      
    message_envelope->doc = doc;

    g_rec_mutex_unlock(message_envelope_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_message_envelope_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsMessageEnvelope *message_envelope;

  GRecMutex *message_envelope_mutex;

  message_envelope = AGS_MESSAGE_ENVELOPE(gobject);

  /* get message envelope mutex */
  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  switch(prop_id){
  case PROP_SENDER:
  {
    g_rec_mutex_lock(message_envelope_mutex);

    g_value_set_object(value, message_envelope->sender);

    g_rec_mutex_unlock(message_envelope_mutex);
  }
  break;
  case PROP_RECIPIENT:
  {
    g_rec_mutex_lock(message_envelope_mutex);

    g_value_set_object(value, message_envelope->recipient);

    g_rec_mutex_unlock(message_envelope_mutex);
  }
  break;
  case PROP_XML_DOC:
  {
    g_rec_mutex_lock(message_envelope_mutex);

    g_value_set_pointer(value, message_envelope->doc);

    g_rec_mutex_unlock(message_envelope_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_message_envelope_dispose(GObject *gobject)
{
  AgsMessageEnvelope *message_envelope;

  message_envelope = AGS_MESSAGE_ENVELOPE(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_message_envelope_parent_class)->dispose(gobject);
}

void
ags_message_envelope_finalize(GObject *gobject)
{
  AgsMessageEnvelope *message_envelope;

  guint i;
  
  message_envelope = AGS_MESSAGE_ENVELOPE(gobject);

  if(message_envelope->sender != NULL){
    g_object_unref(message_envelope->sender);
  }

  if(message_envelope->recipient != NULL){
    g_object_unref(message_envelope->recipient);
  }

  if(message_envelope->doc != NULL){
    xmlFreeDoc(message_envelope->doc);
  }

  g_strfreev(message_envelope->parameter_name);

  for(i = 0; i < message_envelope->n_params; i++){
    gpointer data;

    data = NULL;
    
    if(G_VALUE_HOLDS_STRING(&(message_envelope->value[i]))){
      data = g_value_get_string(&(message_envelope->value[i]));
    }else if(G_VALUE_HOLDS_BOXED(&(message_envelope->value[i]))){
      data = g_value_get_boxed(&(message_envelope->value[i]));
    }else if(G_VALUE_HOLDS_POINTER(&(message_envelope->value[i]))){
      data = g_value_get_pointer(&(message_envelope->value[i]));
    }else if(G_VALUE_HOLDS_OBJECT(&(message_envelope->value[i]))){
      data = g_value_get_object(&(message_envelope->value[i]));
    }
    
    if(message_envelope->free_func != NULL &&
       message_envelope->free_func[i] != NULL){
      message_envelope->free_func[i](data);
    }

    g_value_unset(&(message_envelope->value[i]));
  }

  g_free(message_envelope->value);
  g_free(message_envelope->free_func);
  
  /* call parent */
  G_OBJECT_CLASS(ags_message_envelope_parent_class)->finalize(gobject);
}

/**
 * ags_message_envelope_get_sender:
 * @message_envelope: the #AgsMessageEnvelope
 * 
 * Get sender.
 * 
 * Returns: (transfer full): the sender
 * 
 * Since: 3.0.0
 */
GObject*
ags_message_envelope_get_sender(AgsMessageEnvelope *message_envelope)
{
  GObject *sender;

  GRecMutex *message_envelope_mutex;
  
  if(!AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return(NULL);
  }

  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  /* get sender */
  g_rec_mutex_lock(message_envelope_mutex);
  
  sender = message_envelope->sender;

  if(sender != NULL){
    g_object_ref(sender);
  }
  
  g_rec_mutex_unlock(message_envelope_mutex);
  
  return(sender);
}

/**
 * ags_message_envelope_get_recipient:
 * @message_envelope: the #AgsMessageEnvelope
 * 
 * Get recipient.
 * 
 * Returns: (transfer full): the recipient
 * 
 * Since: 3.0.0
 */
GObject*
ags_message_envelope_get_recipient(AgsMessageEnvelope *message_envelope)
{
  GObject *recipient;

  GRecMutex *message_envelope_mutex;
  
  if(!AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return(NULL);
  }

  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  /* get recipient */
  g_rec_mutex_lock(message_envelope_mutex);
  
  recipient = message_envelope->recipient;

  if(recipient != NULL){
    g_object_ref(recipient);
  }
  
  g_rec_mutex_unlock(message_envelope_mutex);
  
  return(recipient);
}

/**
 * ags_message_envelope_get_doc:
 * @message_envelope: the #AgsMessageEnvelope
 * 
 * Get xmlDoc.
 * 
 * Returns: (transfer none): the doc
 * 
 * Since: 3.0.0
 */
xmlDoc*
ags_message_envelope_get_doc(AgsMessageEnvelope *message_envelope)
{
  xmlDoc *doc;
  
  GRecMutex *message_envelope_mutex;
  
  if(!AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return(NULL);
  }

  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  /* get doc */
  g_rec_mutex_lock(message_envelope_mutex);
  
  doc = message_envelope->doc;
  
  g_rec_mutex_unlock(message_envelope_mutex);
  
  return(doc);
}

/**
 * ags_message_envelope_get_parameter:
 * @message_envelope: the #AgsMessageEnvelope
 * @n_params: return location of parameter count
 * @parameter_name: return location %NULL terminated string vector containing parameter name
 * @value: return location of #GValue-struct array containing values
 * 
 * Get parameters.
 * 
 * Since: 3.0.0
 */
void
ags_message_envelope_get_parameter(AgsMessageEnvelope *message_envelope,
				   guint *n_params,
				   gchar ***parameter_name, GValue **value)
{
  GRecMutex *message_envelope_mutex;
  
  if(!AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return;
  }

  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  /* get parameter */
  g_rec_mutex_lock(message_envelope_mutex);

  if(n_params != NULL){
    n_params[0] = message_envelope->n_params;
  }

  if(parameter_name != NULL){
    parameter_name[0] = message_envelope->parameter_name;
  }

  if(value != NULL){
    value[0] = message_envelope->value;
  }
  
  g_rec_mutex_unlock(message_envelope_mutex);
}

/**
 * ags_message_envelope_set_parameter:
 * @message_envelope: the #AgsMessageEnvelope
 * @n_params: parameter count
 * @parameter_name: (transfer full): %NULL terminated string vector containing parameter name
 * @value: (transfer full): #GValue-struct array containing values
 * 
 * Set parameters.
 * 
 * Since: 3.6.16
 */
void
ags_message_envelope_set_parameter(AgsMessageEnvelope *message_envelope,
				   guint n_params,
				   gchar **parameter_name, GValue *value)
{
  GRecMutex *message_envelope_mutex;
  
  if(!AGS_IS_MESSAGE_ENVELOPE(message_envelope)){
    return;
  }

  message_envelope_mutex = AGS_MESSAGE_ENVELOPE_GET_OBJ_MUTEX(message_envelope);

  /* set parameter */
  g_rec_mutex_lock(message_envelope_mutex);

  message_envelope->n_params = n_params;

  message_envelope->parameter_name = parameter_name;

  message_envelope->value = value;
  
  g_rec_mutex_unlock(message_envelope_mutex);
}

/**
 * ags_message_envelope_new:
 * @sender: the #GObject as sender
 * @recipient: the #GObject as recipient
 * @doc: the xmlDoc
 *
 * Create a new instance of #AgsMessageEnvelope.
 *
 * Returns: the new #AgsMessageEnvelope
 *
 * Since: 3.0.0
 */ 
AgsMessageEnvelope*
ags_message_envelope_new(GObject *sender,
			 GObject *recipient,
			 xmlDoc *doc)
{
  AgsMessageEnvelope *message_envelope;

  message_envelope = (AgsMessageEnvelope *) g_object_new(AGS_TYPE_MESSAGE_ENVELOPE,
							 "sender", sender,
							 "recipient", recipient,
							 "xml-doc", doc,
							 NULL);

  return(message_envelope);
}

/**
 * ags_message_envelope_new_with_params:
 * @sender: the #GObject as sender
 * @recipient: the #GObject as recipient
 * @doc: the xmlDoc
 * @n_params: n params
 * @parameter_name: %NULL terminated string vector containing parameter names
 * @value: the #GValue-struct array containing values
 *
 * Create a new instance of #AgsMessageEnvelope.
 *
 * Returns: the new #AgsMessageEnvelope
 *
 * Since: 3.0.0
 */ 
AgsMessageEnvelope*
ags_message_envelope_new_with_params(GObject *sender,
				     GObject *recipient,
				     xmlDoc *doc,
				     guint n_params,
				     gchar **parameter_name,
				     GValue *value)
{
  AgsMessageEnvelope *message_envelope;

  message_envelope = (AgsMessageEnvelope *) g_object_new(AGS_TYPE_MESSAGE_ENVELOPE,
							 "sender", sender,
							 "recipient", recipient,
							 "xml-doc", doc,
							 NULL);

  message_envelope->n_params = n_params;

  message_envelope->parameter_name = parameter_name;
  message_envelope->value = value;
  
  return(message_envelope);
}
