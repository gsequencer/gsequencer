/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/audio/ags_acceleration.h>

#include <ags/object/ags_connectable.h>

#include <stdlib.h>

void ags_acceleration_class_init(AgsAccelerationClass *acceleration);
void ags_acceleration_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_acceleration_init(AgsAcceleration *acceleration);
void ags_acceleration_connect(AgsConnectable *connectable);
void ags_acceleration_disconnect(AgsConnectable *connectable);
void ags_acceleration_finalize(GObject *object);

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
					   "AgsAcceleration\0",
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

  ags_acceleration_parent_class = g_type_class_peek_parent(acceleration);

  gobject = (GObjectClass *) acceleration;

  gobject->finalize = ags_acceleration_finalize;
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

  acceleration->name = NULL;
}

void
ags_acceleration_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_acceleration_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_acceleration_finalize(GObject *gobject)
{
  /* empty */

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
 * Since: 0.4.3
 */
AgsAcceleration*
ags_acceleration_duplicate(AgsAcceleration *acceleration)
{
  AgsAcceleration *copy;

  copy = ags_acceleration_new();

  copy->flags = 0;

  copy->x = acceleration->x;
  copy->y = acceleration->y;

  copy->name = g_strdup(acceleration->name);
  
  return(copy);
}

/**
 * ags_acceleration_new:
 *
 * Creates an #AgsAcceleration
 *
 * Returns: a new #AgsAcceleration
 *
 * Since: 0.4.3
 */
AgsAcceleration*
ags_acceleration_new()
{
  AgsAcceleration *acceleration;

  acceleration = (AgsAcceleration *) g_object_new(AGS_TYPE_ACCELERATION, NULL);

  return(acceleration);
}
