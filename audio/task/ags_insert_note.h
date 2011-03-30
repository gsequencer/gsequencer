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

#ifndef __AGS_INSERT_NOTE_H__
#define __AGS_INSERT_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_notation.h>

#define AGS_TYPE_INSERT_NOTE                (ags_insert_note_get_type())
#define AGS_INSERT_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_INSERT_NOTE, AgsInsertNote))
#define AGS_INSERT_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_INSERT_NOTE, AgsInsertNoteClass))
#define AGS_IS_INSERT_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_INSERT_NOTE))
#define AGS_IS_INSERT_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_INSERT_NOTE))
#define AGS_INSERT_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_INSERT_NOTE, AgsInsertNoteClass))

typedef struct _AgsInsertNote AgsInsertNote;
typedef struct _AgsInsertNoteClass AgsInsertNoteClass;

struct _AgsInsertNote
{
  AgsTask task;

  AgsNotation *notation;
  AgsNote *note;
};

struct _AgsInsertNoteClass
{
  AgsTaskClass task;
};

GType ags_insert_note_get_type();

AgsInsertNote* ags_insert_note_new(AgsNotation *notation,
				   AgsNote *note);

#endif /*__AGS_INSERT_NOTE_H__*/
