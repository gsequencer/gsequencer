/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/app/ags_line_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_line_member.h>

void
ags_line_check_message_callback(GObject *application_context, AgsLine *line)
{
  ags_line_check_message(line);
}

void
ags_line_group_toggled_callback(GtkWidget *widget, AgsLine *line)
{
  AgsPad *pad;
  AgsLine *current;

  GList *start_list, *list;

  pad = (AgsPad *) line->parent_pad;

  list =
    start_list = ags_pad_get_line(pad);

  if(gtk_toggle_button_get_active(line->group)){
    ags_line_group_changed(line);

    while(list != NULL){
      current = AGS_LINE(list->data);

      if(!gtk_toggle_button_get_active(current->group)){
	g_list_free(start_list);

	return;
      }

      list = list->next;
    }

    gtk_toggle_button_set_active(pad->group, TRUE);
  }else{
    if(g_list_length(list) > 1){
      if(gtk_toggle_button_get_active(pad->group)){
	gtk_toggle_button_set_active(pad->group, FALSE);
      }

      while(list != NULL){
	current = AGS_LINE(list->data);

	if(gtk_toggle_button_get_active(current->group)){
	  ags_line_group_changed(line);
	  g_list_free(start_list);

	  return;
	}

	list = list->next;
      } 
    }

    gtk_toggle_button_set_active(line->group, TRUE);
  }

  g_list_free(start_list);
}

void
ags_line_stop_callback(AgsLine *line,
		       GList *recall_id, gint sound_scope,
		       gpointer data)
{
  AgsPad *pad;
  
  gboolean reset_active;

  pad = AGS_PAD(line->parent_pad);
  
  if((AGS_PAD_BLOCK_STOP & (pad->flags)) != 0){
    return;
  }
  
  pad->flags |= AGS_PAD_BLOCK_STOP;

  /* play button - check reset active */
  reset_active = (sound_scope == AGS_SOUND_SCOPE_PLAYBACK) ? TRUE: FALSE;
  
  if(reset_active){
    gtk_toggle_button_set_active(pad->play, FALSE);
  }
  
  pad->flags &= (~AGS_PAD_BLOCK_STOP);
}
