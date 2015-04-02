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

#ifndef __AGS_DYNAMIC_CONNECTABLE_H__
#define __AGS_DYNAMIC_CONNECTABLE_H__

#include <glib-object.h>

#define AGS_TYPE_DYNAMIC_CONNECTABLE                    (ags_dynamic_connectable_get_type())
#define AGS_DYNAMIC_CONNECTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DYNAMIC_CONNECTABLE, AgsDynamicConnectable))
#define AGS_DYNAMIC_CONNECTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_DYNAMIC_CONNECTABLE, AgsDynamicConnectableInterface))
#define AGS_IS_DYNAMIC_CONNECTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DYNAMIC_CONNECTABLE))
#define AGS_IS_DYNAMIC_CONNECTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_DYNAMIC_CONNECTABLE))
#define AGS_DYNAMIC_CONNECTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_DYNAMIC_CONNECTABLE, AgsDynamicConnectableInterface))

typedef void AgsDynamicConnectable;
typedef struct _AgsDynamicConnectableInterface AgsDynamicConnectableInterface;

struct _AgsDynamicConnectableInterface
{
  GTypeInterface interface;

  void (*connect_dynamic)(AgsDynamicConnectable *connectable);
  void (*disconnect_dynamic)(AgsDynamicConnectable *connectable);
};

GType ags_dynamic_connectable_get_type();

void ags_dynamic_connectable_connect(AgsDynamicConnectable *connectable);
void ags_dynamic_connectable_disconnect(AgsDynamicConnectable *connectable);

#endif /*__AGS_DYNAMIC_CONNECTABLE_H__*/
