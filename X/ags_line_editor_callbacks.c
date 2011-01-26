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

#include <ags/X/ags_line_editor_callbacks.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_machine_editor.h>

int
ags_line_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLineEditor *line_editor)
{
  if(old_parent != NULL)
    return(0);

  gtk_box_pack_start((GtkBox *) line_editor,
		     (GtkWidget *) line_editor->link_editor,
		     FALSE, FALSE, 0);
  return(0);
}

int
ags_line_editor_destroy_callback(GtkObject *object, AgsLineEditor *line_editor)
{
  ags_line_editor_destroy(object);

  return(0);
}

int
ags_line_editor_show_callback(GtkWidget *widget, AgsLineEditor *line_editor)
{
  ags_line_editor_show(widget);

  return(0);
}
