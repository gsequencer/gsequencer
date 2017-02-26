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

#include <ags/X/ags_line_editor_callbacks.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_machine_editor.h>

int
ags_line_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLineEditor *line_editor)
{
  if(old_parent != NULL){
    return(0);
  }

  if(line_editor->link_editor != NULL){
    gtk_box_pack_start((GtkBox *) line_editor,
		       (GtkWidget *) line_editor->link_editor,
		       FALSE, FALSE,
		       0);
  }

  if(line_editor->output_editor != NULL){
    gtk_box_pack_start((GtkBox *) line_editor,
		       (GtkWidget *) line_editor->output_editor,
		       FALSE, FALSE,
		       0);
  }
    
  return(0);
}

int
ags_line_editor_show_callback(GtkWidget *widget, AgsLineEditor *line_editor)
{
  if(line_editor->link_editor != NULL){
    gtk_widget_show((GtkWidget *) line_editor->link_editor);
  }
  
  if(line_editor->output_editor != NULL){
    gtk_widget_show((GtkWidget *) line_editor->output_editor);
  }

  if(line_editor->member_editor != NULL){
    gtk_widget_show((GtkWidget *) line_editor->member_editor);
  }
  
  return(0);
}
