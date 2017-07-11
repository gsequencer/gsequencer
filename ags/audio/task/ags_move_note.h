/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_MOVE_NOTE_H__
#define __AGS_MOVE_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_notation.h>

#define AGS_TYPE_MOVE_NOTE                (ags_move_note_get_type())
#define AGS_MOVE_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MOVE_NOTE, AgsMoveNote))
#define AGS_MOVE_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MOVE_NOTE, AgsMoveNoteClass))
#define AGS_IS_MOVE_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_MOVE_NOTE))
#define AGS_IS_MOVE_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MOVE_NOTE))
#define AGS_MOVE_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MOVE_NOTE, AgsMoveNoteClass))

#define AGS_MOVE_NOTE_DEFAULT_X_LENGTH (16 * 16 * 1200)

typedef struct _AgsMoveNote AgsMoveNote;
typedef struct _AgsMoveNoteClass AgsMoveNoteClass;

struct _AgsMoveNote
{
  AgsTask task;

  AgsNotation *notation;

  GList *selection;
  
  guint first_x;
  guint first_y;
  guint move_x;
  guint move_y;
  
  gboolean relative;
  gboolean absolute;
};

struct _AgsMoveNoteClass
{
  AgsTaskClass task;
};

GType ags_move_note_get_type();

AgsMoveNote* ags_move_note_new(AgsNotation *notation,
			       GList *selection,
			       guint first_x, guint first_y,
			       guint move_x, guint move_y,
			       gboolean relative, gboolean absolute);

#endif /*__AGS_MOVE_NOTE_H__*/
