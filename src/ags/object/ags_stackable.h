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

#ifndef __AGS_STACKABLE_H__
#define __AGS_STACKABLE_H__

#include <glib-object.h>

#define AGS_TYPE_STACKABLE                    (ags_stackable_get_type())
#define AGS_STACKABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_STACKABLE, AgsStackable))
#define AGS_STACKABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_STACKABLE, AgsStackableInterface))
#define AGS_IS_STACKABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_STACKABLE))
#define AGS_IS_STACKABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_STACKABLE))
#define AGS_STACKABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_STACKABLE, AgsStackableInterface))

typedef void AgsStackable;
typedef struct _AgsStackableInterface AgsStackableInterface;

struct _AgsStackableInterface
{
  GTypeInterface interface;

  void (*push)(AgsStackable *stackable);
  void (*pop)(AgsStackable *stackable);
};

GType ags_stackable_get_type();

void ags_stackable_push(AgsStackable *stackable);
void ags_stackable_pop(AgsStackable *stackable);

#endif /*__AGS_STACKABLE_H__*/
