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
#ifndef __AGS_PIANO_H__
#define __AGS_PIANO_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <cairo.h>

#define AGS_TYPE_PIANO                (ags_piano_get_type())
#define AGS_PIANO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PIANO, AgsPiano))
#define AGS_PIANO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_PIANO, AgsPianoClass))
#define AGS_IS_PIANO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PIANO))
#define AGS_IS_PIANO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PIANO))
#define AGS_PIANO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_PIANO, AgsPianoClass))

typedef struct _AgsPiano AgsPiano;
typedef struct _AgsPianoClass AgsPianoClass;

struct _AgsPiano
{
  GtkDrawingArea drawing_area;
};

struct _AgsPianoClass
{
  GtkDrawingAreaClass drawing_area;
};

GType ags_piano_get_type(void);

char* ags_piano_sound_string();
void ags_piano_paint(AgsPiano *piano); // , guint rows, guint nth_y, guint y0

AgsPiano* ags_piano_new();

#endif /*__AGS_PIANO_H__*/
