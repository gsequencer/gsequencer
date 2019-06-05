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

#ifndef __AGS_CURSOR_H__
#define __AGS_CURSOR_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_CURSOR                    (ags_cursor_get_type())
#define AGS_CURSOR(obj)                    (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CURSOR, AgsCursor))
#define AGS_CURSOR_INTERFACE(vtable)       (G_TYPE_CHECK_CLASS_CAST((vtable), AGS_TYPE_CURSOR, AgsCursorInterface))
#define AGS_IS_CURSOR(obj)                 (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CURSOR))
#define AGS_IS_CURSOR_INTERFACE(vtable)    (G_TYPE_CHECK_CLASS_TYPE((vtable), AGS_TYPE_CURSOR))
#define AGS_CURSOR_GET_INTERFACE(obj)      (G_TYPE_INSTANCE_GET_INTERFACE((obj), AGS_TYPE_CURSOR, AgsCursorInterface))

typedef struct _AgsCursor AgsCursor;
typedef struct _AgsCursorInterface AgsCursorInterface;

struct _AgsCursorInterface
{
  GTypeInterface ginterface;

  gdouble (*get_default_offset)(AgsCursor *cursor);
  
  gdouble (*cursor_get_tact)(AgsCursor *cursor);
  gdouble (*cursor_get_bpm)(AgsCursor *cursor);

  guint (*cursor_get_rate)(AgsCursor *cursor);

  gdouble (*cursor_get_delay)(AgsCursor *cursor);
  guint (*cursor_get_duration)(AgsCursor *cursor);

  gdouble (*get_delay_counter)(AgsCursor *cursor);
  guint64 (*get_offset)(AgsCursor *cursor);

  GList* (*get_prev)(AgsCursor *cursor);
  GList* (*get_next)(AgsCursor *cursor);

  GList* (*get_current_copy)(AgsCursor *cursor);
  GList* (*get_current)(AgsCursor *cursor);
};

GType ags_cursor_get_type();

gdouble ags_cursor_get_default_offset(AgsCursor *cursor);

gdouble ags_cursor_cursor_get_tact(AgsCursor *cursor);
gdouble ags_cursor_cursor_get_bpm(AgsCursor *cursor);

guint ags_cursor_cursor_get_rate(AgsCursor *cursor);

gdouble ags_cursor_cursor_get_delay(AgsCursor *cursor);
guint ags_cursor_cursor_get_duration(AgsCursor *cursor);

gdouble ags_cursor_get_delay_counter(AgsCursor *cursor);
guint64 ags_cursor_get_offset(AgsCursor *cursor);

GList* ags_cursor_get_prev(AgsCursor *cursor);
GList* ags_cursor_get_next(AgsCursor *cursor);

GList* ags_cursor_get_current_copy(AgsCursor *cursor);
GList* ags_cursor_get_current(AgsCursor *cursor);

#endif /*__AGS_CURSOR_H__*/
