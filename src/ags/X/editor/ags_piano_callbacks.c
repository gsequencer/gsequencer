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

#include <ags/X/editor/ags_piano_callbacks.h>

#include <ags/X/ags_editor.h>

#include <math.h>

gboolean
ags_piano_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsPiano *piano)
{
  ags_piano_paint(piano);

  return(TRUE);
}

gboolean
ags_piano_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsPiano *piano)
{
  ags_piano_paint(piano);

  return(FALSE);
}
