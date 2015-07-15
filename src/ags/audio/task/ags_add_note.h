/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#ifndef __AGS_ADD_NOTE_H__
#define __AGS_ADD_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_note.h>

#define AGS_TYPE_ADD_NOTE                (ags_add_note_get_type())
#define AGS_ADD_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_NOTE, AgsAddNote))
#define AGS_ADD_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_NOTE, AgsAddNoteClass))
#define AGS_IS_ADD_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_NOTE))
#define AGS_IS_ADD_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_NOTE))
#define AGS_ADD_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_NOTE, AgsAddNoteClass))

typedef struct _AgsAddNote AgsAddNote;
typedef struct _AgsAddNoteClass AgsAddNoteClass;

struct _AgsAddNote
{
  AgsTask task;

  AgsNotation *notation;

  AgsNote *note;
  gboolean use_selection_list;
};

struct _AgsAddNoteClass
{
  AgsTaskClass task;
};

GType ags_add_note_get_type();

AgsAddNote* ags_add_note_new(AgsNotation *notation,
			     AgsNote *note,
			     gboolean use_selection_list);

#endif /*__AGS_ADD_NOTE_H__*/
