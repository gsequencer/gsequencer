/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/server/controller/ags_local_serialization_controller.h>

#include <ags/object/ags_marshal.h>

#include <ags/server/security/ags_authentication_manager.h>

void ags_local_serialization_controller_class_init(AgsLocalSerializationControllerClass *local_serialization_controller);
void ags_local_serialization_controller_init(AgsLocalSerializationController *local_serialization_controller);
void ags_local_serialization_controller_finalize(GObject *gobject);

gpointer ags_local_serialization_controller_real_send_object(AgsLocalSerializationController *local_serialization_controller,
							     GObject *gobject);
gpointer ags_local_serialization_controller_real_send_object_property(AgsLocalSerializationController *local_serialization_controller,
								      GObject *gobject, gchar **property_name);
  
gpointer ags_local_serialization_controller_real_receive_object(AgsLocalSerializationController *local_serialization_controller,
								gchar *str);
gpointer ags_local_serialization_controller_real_receive_object_property(AgsLocalSerializationController *local_serialization_controller,
									 gchar *object_id, gchar *str);

/**
 * SECTION:ags_local_serialization_controller
 * @short_description: local serialization controller
 * @title: AgsLocalSerializationController
 * @section_id:
 * @include: ags/server/controller/ags_local_serialization_controller.h
 *
 * The #AgsLocalSerializationController is a controller.
 */

enum{
  SEND_OBJECT,
  SEND_OBJECT_PROPERTY,
  RECEIVE_OBJECT,
  RECEIVE_OBJECT_PROPERTY,  
  LAST_SIGNAL,
};

static gpointer ags_local_serialization_controller_parent_class = NULL;
static guint local_serialization_controller_signals[LAST_SIGNAL];

GType
ags_local_serialization_controller_get_type()
{
  static GType ags_type_local_serialization_controller = 0;

  if(!ags_type_local_serialization_controller){
    static const GTypeInfo ags_local_serialization_controller_info = {
      sizeof (AgsLocalSerializationControllerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_local_serialization_controller_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLocalSerializationController),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_local_serialization_controller_init,
    };
    
    ags_type_local_serialization_controller = g_type_register_static(G_TYPE_OBJECT,
								     "AgsLocalSerializationController",
								     &ags_local_serialization_controller_info,
								     0);
  }

  return (ags_type_local_serialization_controller);
}

void
ags_local_serialization_controller_class_init(AgsLocalSerializationControllerClass *local_serialization_controller)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_local_serialization_controller_parent_class = g_type_class_peek_parent(local_serialization_controller);

  /* GObjectClass */
  gobject = (GObjectClass *) local_serialization_controller;

  gobject->finalize = ags_local_serialization_controller_finalize;

  /* AgsLocalSerializationController */
  local_serialization_controller->send_object = ags_local_serialization_controller_real_send_object;

  /* signals */
  /**
   * AgsLocalSerializationController::send-object:
   * @local_serialization_controller: the #AgsLocalSerializationController
   * @gobject: the #GObject to send
   *
   * The ::send-object signal is used to send a serialized object.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_serialization_controller_signals[SEND_OBJECT] =
    g_signal_new("send-object",
		 G_TYPE_FROM_CLASS(local_serialization_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalSerializationControllerClass, send_object),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__OBJECT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsLocalSerializationController::send-object-property:
   * @local_serialization_controller: the #AgsLocalSerializationController
   * @gobject: the #GObject to send
   * @property_name: a %NULL terminated string vector containing property names
   *
   * The ::send-object-property signal is used to send a serialized object properties.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_serialization_controller_signals[SEND_OBJECT_PROPERTY] =
    g_signal_new("send-object-property",
		 G_TYPE_FROM_CLASS(local_serialization_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalSerializationControllerClass, send_object_property),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__OBJECT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER);

  /**
   * AgsLocalSerializationController::receive-object:
   * @local_serialization_controller: the #AgsLocalSerializationController
   * @str: the string representation of the object
   *
   * The ::receive-object signal is used to receive a serialized object.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_serialization_controller_signals[RECEIVE_OBJECT] =
    g_signal_new("receive-object",
		 G_TYPE_FROM_CLASS(local_serialization_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalSerializationControllerClass, receive_object),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__OBJECT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_OBJECT);

  /**
   * AgsLocalSerializationController::receive-object-property:
   * @local_serialization_controller: the #AgsLocalSerializationController
   * @object_id: the object's id
   * @str: the string representation of the properties
   *
   * The ::receive-object-property signal is used to receive a serialized object properties.
   *
   * Returns: the response
   * 
   * Since: 1.0.0
   */
  local_serialization_controller_signals[RECEIVE_OBJECT_PROPERTY] =
    g_signal_new("receive-object-property",
		 G_TYPE_FROM_CLASS(local_serialization_controller),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsLocalSerializationControllerClass, receive_object_property),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__OBJECT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_OBJECT,
		 G_TYPE_POINTER);
}

void
ags_local_serialization_controller_init(AgsLocalSerializationController *local_serialization_controller)
{
  gchar *context_path;

  context_path = g_strdup_printf("%s%s",
				 AGS_CONTROLLER_BASE_PATH,
				 AGS_LOCAL_SERIALIZATION_CONTROLLER_CONTEXT_PATH);
  g_object_set(local_serialization_controller,
	       "context-path", context_path,
	       NULL);
  g_free(context_path);

  //TODO:JK: implement me
}

void
ags_local_serialization_controller_finalize(GObject *gobject)
{
  AgsLocalSerializationController *local_serialization_controller;

  local_serialization_controller = AGS_LOCAL_SERIALIZATION_CONTROLLER(gobject);

  G_OBJECT_CLASS(ags_local_serialization_controller_parent_class)->finalize(gobject);
}

gpointer
ags_local_serialization_controller_real_send_object(AgsLocalSerializationController *local_serialization_controller,
						    GObject *gobject)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_local_serialization_controller_send_object:
 * @local_serialization_controller: the #AgsLocalSerializationController
 * @gobject: the #GObject to send
 * 
 * Send object.
 * 
 * Returns: the response
 * 
 * Since: 1.0.0
 */
gpointer
ags_local_serialization_controller_send_object(AgsLocalSerializationController *local_serialization_controller,
					       GObject *gobject)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_LOCAL_SERIALIZATION_CONTROLLER(local_serialization_controller),
		       NULL);

  g_object_ref((GObject *) local_serialization_controller);
  g_signal_emit(G_OBJECT(local_serialization_controller),
		local_serialization_controller_signals[SEND_OBJECT], 0,
		gobject,
		&retval);
  g_object_unref((GObject *) local_serialization_controller);

  return(retval);
}

gpointer
ags_local_serialization_controller_real_send_object_property(AgsLocalSerializationController *local_serialization_controller,
							     GObject *gobject, gchar **property_name)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_local_serialization_controller_send_object_property:
 * @local_serialization_controller: the #AgsLocalSerializationController
 * @gobject: the #GObject containing the properties to send
 * @property_name: the property names as %NULL terminated string vector
 * 
 * Send object property.
 * 
 * Returns: the response
 * 
 * Since: 1.0.0
 */
gpointer
ags_local_serialization_controller_send_object_property(AgsLocalSerializationController *local_serialization_controller,
							GObject *gobject, gchar **property_name)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_LOCAL_SERIALIZATION_CONTROLLER(local_serialization_controller),
		       NULL);

  g_object_ref((GObject *) local_serialization_controller);
  g_signal_emit(G_OBJECT(local_serialization_controller),
		local_serialization_controller_signals[SEND_OBJECT_PROPERTY], 0,
		gobject,
		property_name,
		&retval);
  g_object_unref((GObject *) local_serialization_controller);

  return(retval);
}

gpointer
ags_local_serialization_controller_real_receive_object(AgsLocalSerializationController *local_serialization_controller,
						       gchar *str)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_local_serialization_controller_receive_object:
 * @local_serialization_controller: the #AgsLocalSerializationController
 * @str: the string representation of the object
 * 
 * Receive object.
 * 
 * Returns: the response
 * 
 * Since: 1.0.0
 */
gpointer
ags_local_serialization_controller_receive_object(AgsLocalSerializationController *local_serialization_controller,
						  gchar *str)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_LOCAL_SERIALIZATION_CONTROLLER(local_serialization_controller),
		       NULL);

  g_object_ref((GObject *) local_serialization_controller);
  g_signal_emit(G_OBJECT(local_serialization_controller),
		local_serialization_controller_signals[RECEIVE_OBJECT], 0,
		str,
		&retval);
  g_object_unref((GObject *) local_serialization_controller);

  return(retval);
}

gpointer
ags_local_serialization_controller_real_receive_object_property(AgsLocalSerializationController *local_serialization_controller,
								gchar *object_id, gchar *str)
{
  //TODO:JK: implement me

  return(NULL);
}

/**
 * ags_local_serialization_controller_receive_object_property:
 * @local_serialization_controller: the #AgsLocalSerializationController
 * @object_id: the id of the object to set the properties
 * @str: the string representation of the object properties
 * 
 * Receive object.
 * 
 * Returns: the response
 * 
 * Since: 1.0.0
 */
gpointer
ags_local_serialization_controller_receive_object_property(AgsLocalSerializationController *local_serialization_controller,
							   gchar *object_id, gchar *str)
{
  gpointer retval;

  g_return_val_if_fail(AGS_IS_LOCAL_SERIALIZATION_CONTROLLER(local_serialization_controller),
		       NULL);

  g_object_ref((GObject *) local_serialization_controller);
  g_signal_emit(G_OBJECT(local_serialization_controller),
		local_serialization_controller_signals[RECEIVE_OBJECT_PROPERTY], 0,
		object_id,
		str,
		&retval);
  g_object_unref((GObject *) local_serialization_controller);

  return(retval);
}

/**
 * ags_local_serialization_controller_new:
 * 
 * Instantiate new #AgsLocalSerializationController
 * 
 * Returns: the #AgsLocalSerializationController
 * 
 * Since: 1.0.0
 */
AgsLocalSerializationController*
ags_local_serialization_controller_new()
{
  AgsLocalSerializationController *local_serialization_controller;

  local_serialization_controller = (AgsLocalSerializationController *) g_object_new(AGS_TYPE_LOCAL_SERIALIZATION_CONTROLLER,
										    NULL);

  return(local_serialization_controller);
}
