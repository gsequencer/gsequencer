/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_CONNECTABLE_H__
#define __AGS_CONNECTABLE_H__

#include <glib.h>
#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_CONNECTABLE                    (ags_connectable_get_type())
#define AGS_CONNECTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CONNECTABLE, AgsConnectable))
#define AGS_CONNECTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_CONNECTABLE, AgsConnectableInterface))
#define AGS_IS_CONNECTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CONNECTABLE))
#define AGS_IS_CONNECTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_CONNECTABLE))
#define AGS_CONNECTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_CONNECTABLE, AgsConnectableInterface))

typedef void AgsConnectable;
typedef struct _AgsConnectableInterface AgsConnectableInterface;

struct _AgsConnectableInterface
{
  GTypeInterface interface;

  void (*add_to_registry)(AgsConnectable *connectable);
  void (*remove_from_registry)(AgsConnectable *connectable);

  xmlNode* (*update)(AgsConnectable *connectable);

  gboolean (*is_ready)(AgsConnectable *connectable);
  gboolean (*is_connected)(AgsConnectable *connectable);

  void (*connect)(AgsConnectable *connectable);
  void (*disconnect)(AgsConnectable *connectable);
};

GType ags_connectable_get_type();

void ags_connectable_add_to_registry(AgsConnectable *connectable);

void ags_connectable_connect(AgsConnectable *connectable);
void ags_connectable_disconnect(AgsConnectable *connectable);


#endif /*__AGS_CONNECTABLE_H__*/
