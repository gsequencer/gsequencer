/* This file is part of GSequencer.
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

#ifndef __AGS_MUTABLE_H__
#define __AGS_MUTABLE_H__

#include <glib-object.h>

#include <ags/audio/ags_devout.h>

#define AGS_TYPE_MUTABLE                    (ags_mutable_get_type())
#define AGS_MUTABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MUTABLE, AgsMutable))
#define AGS_MUTABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_MUTABLE, AgsMutableInterface))
#define AGS_IS_MUTABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MUTABLE))
#define AGS_IS_MUTABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_MUTABLE))
#define AGS_MUTABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_MUTABLE, AgsMutableInterface))

typedef void AgsMutable;
typedef struct _AgsMutableInterface AgsMutableInterface;

struct _AgsMutableInterface
{
  GTypeInterface interface;

  void (*set_muted)(AgsMutable *mutable, gboolean muted);
};

GType ags_mutable_get_type();

void ags_mutable_set_muted(AgsMutable *mutable, gboolean muted);

#endif /*__AGS_MUTABLE_H__*/
