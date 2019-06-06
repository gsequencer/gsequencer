/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_NOTE_CURSOR_H__
#define __AGS_NOTE_CURSOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_NOTE_CURSOR                (ags_note_cursor_get_type())
#define AGS_NOTE_CURSOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTE_CURSOR, AgsNoteCursor))
#define AGS_NOTE_CURSOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTE_CURSOR, AgsNoteCursorClass))
#define AGS_IS_NOTE_CURSOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTE_CURSOR))
#define AGS_IS_NOTE_CURSOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTE_CURSOR))
#define AGS_NOTE_CURSOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTE_CURSOR, AgsNoteCursorClass))

typedef struct _AgsNoteCursor AgsNoteCursor;
typedef struct _AgsNoteCursorClass AgsNoteCursorClass;

struct _AgsNoteCursor
{
  GObject object;

  guint flags;

  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *audio;

  gdouble default_offset;

  gdouble tact;
  gdouble bpm;

  guint rate;

  gdouble delay;
  guint64 duration;

  gdouble delay_counter;
  guint64 offset;

  GList *next;
  GList *prev;

  GList *current;
};

struct _AgsNoteCursorClass
{
  GObjectClass object;
};

GType ags_note_cursor_get_type();

pthread_mutex_t* ags_note_cursor_get_class_mutex();

AgsNoteCursor* ags_note_cursor_new(GObject *audio);

#endif /*__AGS_NOTE_CURSOR_H__*/
