/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/object/ags_cursor.h>

void ags_cursor_class_init(AgsCursorInterface *ginterface);

/**
 * SECTION:ags_cursor
 * @short_description: unique access to cursor classes
 * @title: AgsCursor
 * @section_id: AgsCursor
 * @include: ags/object/ags_cursor.h
 *
 * The #AgsCursor interface does cursor position abstraction.
 */

GType
ags_cursor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_cursor = 0;

    ags_type_cursor = g_type_register_static_simple(G_TYPE_INTERFACE,
						    "AgsCursor",
						    sizeof (AgsCursorInterface),
						    (GClassInitFunc) ags_cursor_class_init,
						    0, NULL, 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_cursor);
  }

  return g_define_type_id__volatile;
}

void
ags_cursor_class_init(AgsCursorInterface *ginterface)
{
  /* empty */
}

void
ags_cursor_base_init(AgsCursorInterface *ginterface)
{
  /* empty */
}

/**
 * ags_cursor_get_default_offset:
 * @cursor: the #AgsCursor interface
 * 
 * Get the default offset.
 * 
 * Returns: the default offset
 * 
 * Since: 3.0.0
 */
gdouble
ags_cursor_get_default_offset(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0.0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_default_offset, 0.0);

  return(cursor_interface->get_default_offset(cursor));
}

/**
 * ags_cursor_cursor_get_tact:
 * @cursor: the #AgsCursor interface
 * 
 * Get the tact.
 * 
 * Returns: the tact
 * 
 * Since: 3.0.0
 */
gdouble
ags_cursor_cursor_get_tact(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0.0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_tact, 0.0);

  return(cursor_interface->get_tact(cursor));
}

/**
 * ags_cursor_cursor_get_bpm:
 * @cursor: the #AgsCursor interface
 * 
 * Get BPM.
 * 
 * Returns: the BPM 
 * 
 * Since: 3.0.0
 */
gdouble
ags_cursor_cursor_get_bpm(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0.0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_bpm, 0.0);

  return(cursor_interface->get_bpm(cursor));
}

/**
 * ags_cursor_cursor_get_rate:
 * @cursor: the #AgsCursor interface
 * 
 * Get offset counter rate.
 * 
 * Returns: 
 * 
 * Since: 3.0.0
 */
guint
ags_cursor_cursor_get_rate(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_rate, 0);

  return(cursor_interface->get_rate(cursor));
}

/**
 * ags_cursor_cursor_get_delay:
 * @cursor: the #AgsCursor interface
 * 
 * Get delay.
 * 
 * Returns: the delay
 * 
 * Since: 3.0.0
 */
gdouble
ags_cursor_cursor_get_delay(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0.0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_delay, 0.0);

  return(cursor_interface->get_delay(cursor));
}

/**
 * ags_cursor_cursor_get_duration:
 * @cursor: the #AgsCursor interface
 * 
 * Get duration.
 * 
 * Returns: the duration
 * 
 * Since: 3.0.0
 */
guint64
ags_cursor_cursor_get_duration(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_duration, 0);

  return(cursor_interface->get_duration(cursor));
}

/**
 * ags_cursor_get_delay_counter:
 * @cursor: the #AgsCursor interface
 * 
 * Get delay counter.
 * 
 * Returns: the delay counter
 * 
 * Since: 3.0.0
 */
gdouble
ags_cursor_get_delay_counter(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0.0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_delay_counter, 0.0);

  return(cursor_interface->get_delay_counter(cursor));
}

/**
 * ags_cursor_get_offset:
 * @cursor: the #AgsCursor interface
 * 
 * Get offset.
 * 
 * Returns: the offset
 * 
 * Since: 3.0.0
 */
guint64
ags_cursor_get_offset(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), 0);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_offset, 0);

  return(cursor_interface->get_offset(cursor));
}

/**
 * ags_cursor_get_prev:
 * @cursor: the #AgsCursor interface
 * 
 * Get prev.
 * 
 * Returns: (element-type GObject) (transfer full): the #GList-struct pointing to previous
 * 
 * Since: 3.0.0
 */
GList*
ags_cursor_get_prev(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), NULL);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_prev, NULL);

  return(cursor_interface->get_prev(cursor));
}

/**
 * ags_cursor_get_next:
 * @cursor: the #AgsCursor interface
 * 
 * Get next.
 * 
 * Returns: (element-type GObject) (transfer full): the #GList-struct pointing to next
 * 
 * Since: 3.0.0
 */
GList*
ags_cursor_get_next(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), NULL);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_next, NULL);

  return(cursor_interface->get_next(cursor));
}

/**
 * ags_cursor_get_current_copy:
 * @cursor: the #AgsCursor interface
 * 
 * Get current as copy.
 * 
 * Returns: (element-type GObject) (transfer full): the #GList-struct of current as copy
 * 
 * Since: 3.0.0
 */
GList*
ags_cursor_get_current_copy(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), NULL);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_current_copy, NULL);

  return(cursor_interface->get_current_copy(cursor));
}

/**
 * ags_cursor_get_current:
 * @cursor: the #AgsCursor interface
 * 
 * Get current.
 * 
 * Returns: (element-type GObject) (transfer none): the #GList-struct pointing to current
 * 
 * Since: 3.0.0
 */
GList*
ags_cursor_get_current(AgsCursor *cursor)
{
  AgsCursorInterface *cursor_interface;

  g_return_val_if_fail(AGS_IS_CURSOR(cursor), NULL);
  cursor_interface = AGS_CURSOR_GET_INTERFACE(cursor);
  g_return_val_if_fail(cursor_interface->get_current, NULL);

  return(cursor_interface->get_current(cursor));
}
