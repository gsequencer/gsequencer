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

#ifndef __AGS_PLAYABLE_H__
#define __AGS_PLAYABLE_H__

#include <glib-object.h>

#define AGS_TYPE_PLAYABLE                    (ags_playable_get_type())
#define AGS_PLAYABLE(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYABLE, AgsPlayable))
#define AGS_PLAYABLE_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_PLAYABLE, AgsPlayableInterface))
#define AGS_IS_PLAYABLE(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAYABLE))
#define AGS_IS_PLAYABLE_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_PLAYABLE))
#define AGS_PLAYABLE_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_PLAYABLE, AgsPlayableInterface))

typedef void AgsPlayable;
typedef struct _AgsPlayableInterface AgsPlayableInterface;

struct _AgsPlayableInterface
{
  GTypeInterface interface;

  gboolean (*open)(AgsPlayable *playable, gchar *name);
  void (*info)(AgsPlayable *playable, guint *channels, guint *frames);
  short* (*read)(AgsPlayable *playable, guint channel);
  void (*close)(AgsPlayable *playable);
};

GType ags_playable_get_type();

gboolean ags_playable_open(AgsPlayable *playable, gchar *name);
void ags_playable_info(AgsPlayable *playable, guint *channels, guint *frames);
short* ags_playable_read(AgsPlayable *playable, guint channel);
void ags_playable_close(AgsPlayable *playable);

#endif /*__AGS_PLAYABLE_H__*/
