/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2012 Joël Krähemann
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

#ifndef __AGS_INLINE_PLAYER_H__
#define __AGS_INLINE_PLAYER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/object/ags_playable.h>

#define AGS_TYPE_INLINE_PLAYER                (ags_inline_player_get_type ())
#define AGS_INLINE_PLAYER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INLINE_PLAYER, AgsInlinePlayer))
#define AGS_INLINE_PLAYER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INLINE_PLAYER, AgsInlinePlayerClass))
#define AGS_IS_INLINE_PLAYER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INLINE_PLAYER))
#define AGS_IS_INLINE_PLAYER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INLINE_PLAYER))
#define AGS_INLINE_PLAYER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INLINE_PLAYER, AgsInlinePlayerClass))

typedef struct _AgsInlinePlayer AgsInlinePlayer;
typedef struct _AgsInlinePlayerClass AgsInlinePlayerClass;

struct _AgsInlinePlayer
{
  GtkHBox vbox;

  AgsPlayable *playable;

  GtkToggleButton *play;
  GtkHScale *position;
  GtkVolumeButton *volume;
};

struct _AgsInlinePlayerClass
{
  GtkHBoxClass vbox;
};

GType ags_inline_player_get_type();

AgsInlinePlayer* ags_inline_player_new();

#endif /*__AGS_INLINE_PLAYER_H__*/

