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

#ifndef __AGS_BLINK_CELL_PATTERN_CURSOR_H__
#define __AGS_BLINK_CELL_PATTERN_CURSOR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_task.h>

#include <ags/X/machine/ags_cell_pattern.h>

#define AGS_TYPE_BLINK_CELL_PATTERN_CURSOR                (ags_blink_cell_pattern_cursor_get_type())
#define AGS_BLINK_CELL_PATTERN_CURSOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_BLINK_CELL_PATTERN_CURSOR, AgsBlinkCellPatternCursor))
#define AGS_BLINK_CELL_PATTERN_CURSOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_BLINK_CELL_PATTERN_CURSOR, AgsBlinkCellPatternCursorClass))
#define AGS_IS_BLINK_CELL_PATTERN_CURSOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_BLINK_CELL_PATTERN_CURSOR))
#define AGS_IS_BLINK_CELL_PATTERN_CURSOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_BLINK_CELL_PATTERN_CURSOR))
#define AGS_BLINK_CELL_PATTERN_CURSOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_BLINK_CELL_PATTERN_CURSOR, AgsBlinkCellPatternCursorClass))

typedef struct _AgsBlinkCellPatternCursor AgsBlinkCellPatternCursor;
typedef struct _AgsBlinkCellPatternCursorClass AgsBlinkCellPatternCursorClass;

struct _AgsBlinkCellPatternCursor
{
  AgsTask task;

  AgsCellPattern *cell_pattern;
  gboolean highlight_cursor;
};

struct _AgsBlinkCellPatternCursorClass
{
  AgsTaskClass task;
};

GType ags_blink_cell_pattern_cursor_get_type();

AgsBlinkCellPatternCursor* ags_blink_cell_pattern_cursor_new(AgsCellPattern *cell_pattern,
							     gboolean highlight_cursor);

#endif /*__AGS_BLINK_CELL_PATTERN_CURSOR_H__*/
