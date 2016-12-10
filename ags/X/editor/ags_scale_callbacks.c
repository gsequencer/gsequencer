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

#include <ags/X/editor/ags_scale_callbacks.h>

gboolean
ags_scale_expose_event(GtkWidget *widget, GdkEventExpose *event, AgsScale *scale)
{
  ags_scale_paint(scale);

  return(TRUE);
}

gboolean
ags_scale_configure_event(GtkWidget *widget, GdkEventConfigure *event, AgsScale *scale)
{
  ags_scale_paint(scale);

  return(FALSE);
}
