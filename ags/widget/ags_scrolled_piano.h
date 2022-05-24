/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_SCROLLED_PIANO_H__
#define __AGS_SCROLLED_PIANO_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/widget/ags_piano.h>

G_BEGIN_DECLS

#define AGS_TYPE_SCROLLED_PIANO                (ags_scrolled_piano_get_type())
#define AGS_SCROLLED_PIANO(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SCROLLED_PIANO, AgsScrolledPiano))
#define AGS_SCROLLED_PIANO_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SCROLLED_PIANO, AgsScrolledPianoClass))
#define AGS_IS_SCROLLED_PIANO(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_SCROLLED_PIANO))
#define AGS_IS_SCROLLED_PIANO_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_SCROLLED_PIANO))
#define AGS_SCROLLED_PIANO_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_SCROLLED_PIANO, AgsScrolledPianoClass))

typedef struct _AgsScrolledPiano AgsScrolledPiano;
typedef struct _AgsScrolledPianoClass AgsScrolledPianoClass;

struct _AgsScrolledPiano
{
  GtkGrid grid;
  
  GtkScrolledWindow *scrolled_window;

  AgsPiano *piano;
};

struct _AgsScrolledPianoClass
{
  GtkGridClass grid;
};

GType ags_scrolled_piano_get_type(void);

GtkScrolledWindow* ags_scrolled_piano_get_scrolled_window(AgsScrolledPiano *scrolled_piano);

AgsPiano* ags_scrolled_piano_get_piano(AgsScrolledPiano *scrolled_piano);

AgsScrolledPiano* ags_scrolled_piano_new();

G_END_DECLS

#endif /*__AGS_SCROLLED_PIANO_H__*/
