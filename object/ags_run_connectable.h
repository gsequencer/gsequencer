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

#ifndef __AGS_RUN_CONNECTABLE_H__
#define __AGS_RUN_CONNECTABLE_H__

#include <glib-object.h>

#define AGS_TYPE_RUN_CONNECTABLE                    (ags_run_connectable_get_type())
#define AGS_RUN_CONNECTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RUN_CONNECTABLE, AgsRunConnectable))
#define AGS_RUN_CONNECTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_RUN_CONNECTABLE, AgsRunConnectableInterface))
#define AGS_IS_RUN_CONNECTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RUN_CONNECTABLE))
#define AGS_IS_RUN_CONNECTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_RUN_CONNECTABLE))
#define AGS_RUN_CONNECTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_RUN_CONNECTABLE, AgsRunConnectableInterface))

typedef void AgsRunConnectable;
typedef struct _AgsRunConnectableInterface AgsRunConnectableInterface;

struct _AgsRunConnectableInterface
{
  GTypeInterface interface;

  void (*connect)(AgsRunConnectable *connectable);
  void (*disconnect)(AgsRunConnectable *connectable);
};

GType ags_run_connectable_get_type();

void ags_run_connectable_connect(AgsRunConnectable *connectable);
void ags_run_connectable_disconnect(AgsRunConnectable *connectable);

#endif /*__AGS_RUN_CONNECTABLE_H__*/
