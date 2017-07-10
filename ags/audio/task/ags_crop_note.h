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

#ifndef __AGS_CROP_NOTE_H__
#define __AGS_CROP_NOTE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/ags_notation.h>

#define AGS_TYPE_CROP_NOTE                (ags_crop_note_get_type())
#define AGS_CROP_NOTE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CROP_NOTE, AgsCropNote))
#define AGS_CROP_NOTE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CROP_NOTE, AgsCropNoteClass))
#define AGS_IS_CROP_NOTE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CROP_NOTE))
#define AGS_IS_CROP_NOTE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CROP_NOTE))
#define AGS_CROP_NOTE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CROP_NOTE, AgsCropNoteClass))

#define AGS_CROP_NOTE_DEFAULT_X_LENGTH (16 * 1200)

typedef struct _AgsCropNote AgsCropNote;
typedef struct _AgsCropNoteClass AgsCropNoteClass;

struct _AgsCropNote
{
  AgsTask task;

  AgsNotation *notation;

  GList *selection;
  
  gint x_offset;
  guint x_padding;
  guint x_crop;
  
  gboolean absolute;
  gboolean in_place;
  gboolean do_resize;
};

struct _AgsCropNoteClass
{
  AgsTaskClass task;
};

GType ags_crop_note_get_type();

AgsCropNote* ags_crop_note_new(AgsNotation *notation,
			       GList *selection,
			       gint x_offset, guint x_padding, guint x_crop,
			       gboolean absolute, gboolean in_place, gboolean do_resize);

#endif /*__AGS_CROP_NOTE_H__*/
