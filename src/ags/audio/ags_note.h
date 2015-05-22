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

#ifndef __AGS_NOTE_H__
#define __AGS_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_NOTE                (ags_note_get_type())
#define AGS_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTE, AgsNote))
#define AGS_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTE, AgsNoteClass))
#define AGS_IS_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTE))
#define AGS_IS_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTE))
#define AGS_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTE, AgsNoteClass))

typedef struct _AgsNote AgsNote;
typedef struct _AgsNoteClass AgsNoteClass;

typedef enum{
  AGS_NOTE_GUI             =  1,
  AGS_NOTE_RUNTIME         =  1 <<  1,
  AGS_NOTE_HUMAN_READABLE  =  1 <<  2,
  AGS_NOTE_DEFAULT_LENGTH  =  1 <<  3,
  AGS_NOTE_IS_SELECTED     =  1 <<  4,
}AgsNoteFlags;

struct _AgsNote
{
  GObject object;

  guint flags;

  // gui format, convert easy to visualization
  guint x[2];
  guint y;

  gchar *name;
  gdouble frequency;

  gdouble velocity[2];
};

struct _AgsNoteClass
{
  GObjectClass object;
};

GType ags_note_get_type();

GList* ags_note_find_prev(GList *note,
			  guint x0, guint y);
GList* ags_note_find_next(GList *note,
			  guint x0, guint y);

AgsNote* ags_note_duplicate(AgsNote *note);

AgsNote* ags_note_new();

#endif /*__AGS_NOTE_H__*/
