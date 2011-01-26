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

#ifndef __AGS_NOTATION_H__
#define __AGS_NOTATION_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_NOTATION                (ags_notation_get_type())
#define AGS_NOTATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTATION, AgsNotation))
#define AGS_NOTATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTATION, AgsNotationClass))
#define AGS_IS_NOTATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTATION))
#define AGS_IS_NOTATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTATION))
#define AGS_NOTATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTATION, AgsNotationClass))

#define AGS_NOTE(obj) ((AgsNote *) obj)

typedef struct _AgsNotation AgsNotation;
typedef struct _AgsNotationClass AgsNotationClass;
typedef struct _AgsNote AgsNote;

typedef enum{
  AGS_NOTATION_RASTER            =  1,
  AGS_NOTATION_DEFAULT_LENGTH    =  1 <<  1,
}AgsNotationFlags;

typedef enum{
  AGS_NOTE_GUI             =  1,
  AGS_NOTE_RUNTIME         =  1 <<  1,
  AGS_NOTE_HUMAN_READABLE  =  1 <<  2,
  AGS_NOTE_DEFAULT_LENGTH  =  1 <<  3,
}AgsNoteFlags;

struct _AgsNotation
{
  GObject object;

  guint flags;

  GObject *audio;

  guint base_frequence;

  char *tact;
  guint bpm;

  GList *note;
  guint pads;
};

struct _AgsNotationClass
{
  GObjectClass object;
};

struct _AgsNote
{
  guint flags;

  // gui format, convert easy to visualization
  guint x[2];
  guint y;
};

GType ags_notation_get_type();

void ags_notation_connect(AgsNotation *notation);

void ags_notation_add_note(AgsNotation *notation, AgsNote *note);

AgsNote* ags_note_alloc();

AgsNotation* ags_notation_new();

#endif /*__AGS_NOTATION_H__*/
