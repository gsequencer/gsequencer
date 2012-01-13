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

#ifndef __AGS_PLAYABLE_CHOOSER_H__
#define __AGS_PLAYABLE_CHOOSER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define AGS_TYPE_PLAYABLE_CHOOSER                (ags_playable_chooser_get_type ())
#define AGS_PLAYABLE_CHOOSER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYABLE_CHOOSER, AgsPlayableChooser))
#define AGS_PLAYABLE_CHOOSER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PLAYABLE_CHOOSER, AgsPlayableChooserClass))
#define AGS_IS_PLAYABLE_CHOOSER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_PLAYABLE_CHOOSER))
#define AGS_IS_PLAYABLE_CHOOSER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PLAYABLE_CHOOSER))
#define AGS_PLAYABLE_CHOOSER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PLAYABLE_CHOOSER, AgsPlayableChooserClass))

typedef struct _AgsPlayableChooser AgsPlayableChooser;
typedef struct _AgsPlayableChooserClass AgsPlayableChooserClass;

struct _AgsPlayableChooser
{
  GtkPlayableChooser playable_chooser;

  GtkWidget *chooser;
};

struct _AgsPlayableChooserClass
{
  GtkPlayableChooserClass playable_chooser;
};

GType ags_playable_chooser_get_type();

AgsPlayableChooser* ags_playable_chooser_new();

#endif /*__AGS_PLAYABLE_CHOOSER_H__*/

