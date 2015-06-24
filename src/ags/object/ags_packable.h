/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_PACKABLE_H__
#define __AGS_PACKABLE_H__

#include <glib-object.h>

#define AGS_TYPE_PACKABLE                    (ags_packable_get_type())
#define AGS_PACKABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PACKABLE, AgsPackable))
#define AGS_PACKABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PACKABLE, AgsPackableInterface))
#define AGS_IS_PACKABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PACKABLE))
#define AGS_IS_PACKABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PACKABLE))
#define AGS_PACKABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PACKABLE, AgsPackableInterface))

typedef void AgsPackable;
typedef struct _AgsPackableInterface AgsPackableInterface;

struct _AgsPackableInterface
{
  GTypeInterface interface;

  gboolean (*pack)(AgsPackable *packable, GObject *container);
  gboolean (*unpack)(AgsPackable *packable);
};

GType ags_packable_get_type();

gboolean ags_packable_pack(AgsPackable *packable, GObject *container);
gboolean ags_packable_unpack(AgsPackable *packable);

#endif /*__AGS_PACKABLE_H__*/
