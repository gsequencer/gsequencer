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

#ifndef __AGS_EFFECT_H__
#define __AGS_EFFECT_H__

#include <glib-object.h>

#include <libxml/tree.h>

#define AGS_TYPE_EFFECT                    (ags_effect_get_type())
#define AGS_EFFECT(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT, AgsEffect))
#define AGS_EFFECT_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_EFFECT, AgsEffectInterface))
#define AGS_IS_EFFECT(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT))
#define AGS_IS_EFFECT_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_EFFECT))
#define AGS_EFFECT_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_EFFECT, AgsEffectInterface))

typedef void AgsEffect;
typedef struct _AgsEffectInterface AgsEffectInterface;

struct _AgsEffectInterface
{
  GTypeInterface interface;
};

GType ags_effect_get_type();

#endif /*__AGS_EFFECT_H__*/
