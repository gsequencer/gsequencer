/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_SERIALIZEABLE_H__
#define __AGS_SERIALIZEABLE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_SERIALIZEABLE                    (ags_serializeable_get_type())
#define AGS_SERIALIZEABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SERIALIZEABLE, AgsSerializeable))
#define AGS_SERIALIZEABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_SERIALIZEABLE, AgsSerializeableInterface))
#define AGS_IS_SERIALIZEABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SERIALIZEABLE))
#define AGS_IS_SERIALIZEABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_SERIALIZEABLE))
#define AGS_SERIALIZEABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_SERIALIZEABLE, AgsSerializeableInterface))

typedef void AgsSerializeable;
typedef struct _AgsSerializeableInterface AgsSerializeableInterface;

struct _AgsSerializeableInterface
{
  GTypeInterface interface;

  void (*serialize)(AgsSerializeable *serializeable);
};

GType ags_serializeable_get_type();

void ags_serializeable_serialize(AgsSerializeable *serializeable);

#endif /*__AGS_SERIALIZEABLE_H__*/
