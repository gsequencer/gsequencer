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

#include <ags/audio/ags_acceleration.h>

#include <ags/object/ags_connectable.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_acceleration_class_init(AgsAccelerationClass *acceleration);
void ags_acceleration_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_acceleration_init(AgsAcceleration *acceleration);
void ags_acceleration_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_acceleration_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_acceleration_connect(AgsConnectable *connectable);
void ags_acceleration_disconnect(AgsConnectable *connectable);
void ags_acceleration_finalize(GObject *gobject);

/**
 * SECTION:ags_acceleration
 * @short_description: Acceleration class.
 * @title: AgsAcceleration
 * @section_id:
 * @include: ags/audio/ags_acceleration.h
 *
 * #AgsAcceleration represents a downhill-grade.
 */

static gpointer ags_acceleration_parent_class = NULL;

enum{
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_ACCELERATION_NAME,
};

GType
ags_acceleration_get_type()
{
  static GType ags_type_acceleration = 0;

  if(!ags_type_acceleration){
    static const GTypeInfo ags_acceleration_info = {
      sizeof(AgsAccelerationClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_acceleration_class_init,
      NULL,
      NULL,
      sizeof(AgsAcceleration),
      0,
      (GInstanceInitFunc) ags_acceleration_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_acceleration_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_acceleration = g_type_register_static(G_TYPE_OBJECT,
						   "AgsAcceleration",
						   &ags_acceleration_info,
						   0);
    
    g_type_add_interface_static(ags_type_acceleration,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_acceleration);
}

void 
ags_acceleration_class_init(AgsAccelerationClass *acceleration)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_acceleration_parent_class = g_type_class_peek_parent(acceleration);

  gobject = (GObjectClass *) acceleration;

  gobject->set_property = ags_acceleration_set_property;
  gobject->get_property = ags_acceleration_get_property;

  gobject->finalize = ags_acceleration_finalize;

  /* properties */
  /**
   * AgsAcceleration:x:
   *
   * Acceleration offset x.
   * 
   * Since: 0.8.2
   */
  param_spec = g_param_spec_uint("x",
				 i18n_pspec("offset x"),
				 i18n_pspec("The x offset"),
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X,
				  param_spec);

  /**
   * AgsAcceleration:y:
   *
   * Acceleration offset y.
   * 
   * Since: 0.8.2
   */
  param_spec = g_param_spec_uint("y",
				 i18n_pspec("offset y"),
				 i18n_pspec("The y offset"),
				 0,
				 65535,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_Y,
				  param_spec);

  /**
   * AgsAcceleration:acceleration-name:
   *
   * The acceleration's name.
   * 
   * Since: 0.9.0
   */
  param_spec = g_param_spec_string("acceleration-name",
				   i18n_pspec("acceleration name"),
				   i18n_pspec("The acceleration's name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_ACCELERATION_NAME,
				  param_spec);
}

void
ags_acceleration_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_acceleration_connect;
  connectable->disconnect = ags_acceleration_disconnect;
}

void
ags_acceleration_init(AgsAcceleration *acceleration)
{
  acceleration->flags = 0;

  acceleration->x = 0;
  acceleration->y = 0;

  acceleration->acceleration_name = NULL;
}


void
ags_acceleration_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsAcceleration *acceleration;

  acceleration = AGS_ACCELERATION(gobject);

  switch(prop_id){
  case PROP_X:
    {
      acceleration->x = g_value_get_uint(value);
    }
    break;
  case PROP_Y:
    {
      acceleration->y = g_value_get_uint(value);
    }
    break;
  case PROP_ACCELERATION_NAME:
    {
      gchar *acceleration_name;
      
      acceleration_name = g_value_get_string(value);
      
      if(acceleration_name == acceleration->acceleration_name){
	return;
      }

      if(acceleration->acceleration_name != NULL){
	g_free(acceleration->acceleration_name);
      }

      acceleration->acceleration_name = g_strdup(acceleration_name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_acceleration_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsAcceleration *acceleration;

  acceleration = AGS_ACCELERATION(gobject);

  switch(prop_id){
  case PROP_X:
    {
      g_value_set_uint(value, acceleration->x);
    }
    break;
  case PROP_Y:
    {
      g_value_set_uint(value, acceleration->y);
    }
    break;
  case PROP_ACCELERATION_NAME:
    {
      g_value_set_string(value, acceleration->acceleration_name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_acceleration_connect(AgsConnectable *connectable)
{
  AgsAcceleration *acceleration;

  acceleration = AGS_ACCELERATION(connectable);

  if((AGS_ACCELERATION_CONNECTED & (acceleration->flags)) != 0){
    return;
  }

  acceleration->flags |= AGS_ACCELERATION_CONNECTED;
}

void
ags_acceleration_disconnect(AgsConnectable *connectable)
{
  AgsAcceleration *acceleration;

  acceleration = AGS_ACCELERATION(connectable);

  if((AGS_ACCELERATION_CONNECTED & (acceleration->flags)) == 0){
    return;
  }

  acceleration->flags &= (~AGS_ACCELERATION_CONNECTED);
}

void
ags_acceleration_finalize(GObject *gobject)
{
  AgsAcceleration *acceleration;

  acceleration = AGS_ACCELERATION(gobject);
  
  if(acceleration->acceleration_name != NULL){
    free(acceleration->acceleration_name);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_acceleration_parent_class)->finalize(gobject);
}

/**
 * ags_acceleration_duplicate:
 * @acceleration: an #AgsAcceleration
 * 
 * Duplicate a acceleration.
 *
 * Returns: the duplicated #AgsAcceleration.
 *
 * Since: 0.7.0
 */
AgsAcceleration*
ags_acceleration_duplicate(AgsAcceleration *acceleration)
{
  AgsAcceleration *copy;

  copy = ags_acceleration_new();

  copy->flags = 0;

  copy->x = acceleration->x;
  copy->y = acceleration->y;

  copy->acceleration_name = g_strdup(acceleration->acceleration_name);
  
  return(copy);
}

/**
 * ags_acceleration_new:
 *
 * Creates an #AgsAcceleration
 *
 * Returns: a new #AgsAcceleration
 *
 * Since: 0.7.0
 */
AgsAcceleration*
ags_acceleration_new()
{
  AgsAcceleration *acceleration;

  acceleration = (AgsAcceleration *) g_object_new(AGS_TYPE_ACCELERATION, NULL);

  return(acceleration);
}
