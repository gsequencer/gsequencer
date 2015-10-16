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

#include <ags/X/task/ags_blink_cell_pattern_cursor.h>

#include <ags/object/ags_connectable.h>

#include <ags/widget/ags_led.h>

void ags_blink_cell_pattern_cursor_class_init(AgsBlinkCellPatternCursorClass *blink_cell_pattern_cursor);
void ags_blink_cell_pattern_cursor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_blink_cell_pattern_cursor_init(AgsBlinkCellPatternCursor *blink_cell_pattern_cursor);
void ags_blink_cell_pattern_cursor_connect(AgsConnectable *connectable);
void ags_blink_cell_pattern_cursor_disconnect(AgsConnectable *connectable);
void ags_blink_cell_pattern_cursor_finalize(GObject *gobject);
void ags_blink_cell_pattern_cursor_launch(AgsTask *task);

/**
 * SECTION:ags_blink_cell_pattern_cursor
 * @short_description: toggle led object
 * @title: AgsBlinkCellPatternCursor
 * @section_id:
 * @include: ags/audio/task/ags_blink_cell_pattern_cursor.h
 *
 * The #AgsBlinkCellPatternCursor task blinks the cell pattern cursor.
 */

static gpointer ags_blink_cell_pattern_cursor_parent_class = NULL;
static AgsConnectableInterface *ags_blink_cell_pattern_cursor_parent_connectable_interface;

GType
ags_blink_cell_pattern_cursor_get_type()
{
  static GType ags_type_blink_cell_pattern_cursor = 0;

  if(!ags_type_blink_cell_pattern_cursor){
    static const GTypeInfo ags_blink_cell_pattern_cursor_info = {
      sizeof (AgsBlinkCellPatternCursorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_blink_cell_pattern_cursor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsBlinkCellPatternCursor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_blink_cell_pattern_cursor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_blink_cell_pattern_cursor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_blink_cell_pattern_cursor = g_type_register_static(AGS_TYPE_TASK,
								"AgsBlinkCellPatternCursor\0",
								&ags_blink_cell_pattern_cursor_info,
								0);

    g_type_add_interface_static(ags_type_blink_cell_pattern_cursor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_blink_cell_pattern_cursor);
}

void
ags_blink_cell_pattern_cursor_class_init(AgsBlinkCellPatternCursorClass *blink_cell_pattern_cursor)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_blink_cell_pattern_cursor_parent_class = g_type_class_peek_parent(blink_cell_pattern_cursor);

  /* GObject */
  gobject = (GObjectClass *) blink_cell_pattern_cursor;

  gobject->finalize = ags_blink_cell_pattern_cursor_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) blink_cell_pattern_cursor;

  task->launch = ags_blink_cell_pattern_cursor_launch;
}

void
ags_blink_cell_pattern_cursor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_blink_cell_pattern_cursor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_blink_cell_pattern_cursor_connect;
  connectable->disconnect = ags_blink_cell_pattern_cursor_disconnect;
}

void
ags_blink_cell_pattern_cursor_init(AgsBlinkCellPatternCursor *blink_cell_pattern_cursor)
{
  blink_cell_pattern_cursor->cell_pattern = NULL;
  blink_cell_pattern_cursor->highlight_cursor = FALSE;
}

void
ags_blink_cell_pattern_cursor_connect(AgsConnectable *connectable)
{
  ags_blink_cell_pattern_cursor_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_blink_cell_pattern_cursor_disconnect(AgsConnectable *connectable)
{
  ags_blink_cell_pattern_cursor_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_blink_cell_pattern_cursor_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_blink_cell_pattern_cursor_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_blink_cell_pattern_cursor_launch(AgsTask *task)
{
  AgsBlinkCellPatternCursor *blink_cell_pattern_cursor;

  blink_cell_pattern_cursor = AGS_BLINK_CELL_PATTERN_CURSOR(task);
  
  if(blink_cell_pattern_cursor->highlight_cursor){
    blink_cell_pattern_cursor->cell_pattern->flags |= AGS_CELL_PATTERN_CURSOR_ON;
  }else{
    blink_cell_pattern_cursor->cell_pattern->flags &= (~AGS_CELL_PATTERN_CURSOR_ON);
  }

  gtk_widget_queue_draw(blink_cell_pattern_cursor->cell_pattern);
}

/**
 * ags_blink_cell_pattern_cursor_new:
 * @cell_pattern: the #AgsCellPattern to blink
 * @highlight_cursor: %TRUE if highlighted
 *
 * Creates an #AgsBlinkCellPatternCursor.
 *
 * Returns: an new #AgsBlinkCellPatternCursor.
 *
 * Since: 0.4
 */
AgsBlinkCellPatternCursor*
ags_blink_cell_pattern_cursor_new(AgsCellPattern *cell_pattern,
				  gboolean highlight_cursor)
{
  AgsBlinkCellPatternCursor *blink_cell_pattern_cursor;

  blink_cell_pattern_cursor = (AgsBlinkCellPatternCursor *) g_object_new(AGS_TYPE_BLINK_CELL_PATTERN_CURSOR,
									 NULL);

  blink_cell_pattern_cursor->cell_pattern = cell_pattern;
  blink_cell_pattern_cursor->highlight_cursor = highlight_cursor;
  
  return(blink_cell_pattern_cursor);
}
