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

#ifndef __AGS_REMOVE_NOTE_H__
#define __AGS_REMOVE_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/audio/ags_notation.h>

#define AGS_TYPE_REMOVE_NOTE                (ags_remove_note_get_type())
#define AGS_REMOVE_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_NOTE, AgsRemoveNote))
#define AGS_REMOVE_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_NOTE, AgsRemoveNoteClass))
#define AGS_IS_REMOVE_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REMOVE_NOTE))
#define AGS_IS_REMOVE_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_REMOVE_NOTE))
#define AGS_REMOVE_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_REMOVE_NOTE, AgsRemoveNoteClass))

typedef struct _AgsRemoveNote AgsRemoveNote;
typedef struct _AgsRemoveNoteClass AgsRemoveNoteClass;

struct _AgsRemoveNote
{
  AgsTask task;

  AgsNotation *notation;

  guint x;
  guint y;
};

struct _AgsRemoveNoteClass
{
  AgsTaskClass task;
};

GType ags_remove_note_get_type();

AgsRemoveNote* ags_remove_note_new(AgsNotation *notation,
				   guint x, guint y);

#endif /*__AGS_REMOVE_NOTE_H__*/
