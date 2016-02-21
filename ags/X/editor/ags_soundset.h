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
#ifndef __AGS_SOUNDSET_H__
#define __AGS_SOUNDSET_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <cairo.h>

#define AGS_TYPE_SOUNDSET                (ags_soundset_get_type())
#define AGS_SOUNDSET(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SOUNDSET, AgsSoundset))
#define AGS_SOUNDSET_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SOUNDSET, AgsSoundsetClass))
#define AGS_IS_SOUNDSET(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SOUNDSET))
#define AGS_IS_SOUNDSET_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SOUNDSET))
#define AGS_SOUNDSET_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SOUNDSET, AgsSoundsetClass))

typedef struct _AgsSoundset AgsSoundset;
typedef struct _AgsSoundsetClass AgsSoundsetClass;

struct _AgsSoundset
{
  GtkDrawingArea drawing_area;
};

struct _AgsSoundsetClass
{
  GtkDrawingAreaClass drawing_area;
};

GType ags_soundset_get_type(void);

char* ags_soundset_sound_string();
void ags_soundset_paint(AgsSoundset *soundset); // , guint rows, guint nth_y, guint y0

AgsSoundset* ags_soundset_new();

#endif /*__AGS_SOUNDSET_H__*/
