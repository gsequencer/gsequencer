/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_line_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_listing_editor.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line_member.h>

#include <ags/X/thread/ags_gui_thread.h>

void
ags_line_group_clicked_callback(GtkWidget *widget, AgsLine *line)
{
  AgsPad *pad;
  AgsLine *current;

  GtkContainer *container;

  GList *list, *list_start;

  pad = (AgsPad *) gtk_widget_get_ancestor(GTK_WIDGET(line),
					   AGS_TYPE_PAD);

  container = (GtkContainer *) pad->expander_set;

  list_start =
    list = gtk_container_get_children(container);

  if(gtk_toggle_button_get_active(line->group)){
    ags_line_group_changed(line);

    while(list != NULL){
      current = AGS_LINE(list->data);

      if(!gtk_toggle_button_get_active(current->group)){
	g_list_free(list_start);

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
	  g_list_free(list_start);

	  return;
	}

	list = list->next;
      } 
    }

    gtk_toggle_button_set_active(line->group, TRUE);
  }

  g_list_free(list_start);

  return;
}

void
ags_line_volume_callback(GtkRange *range,
			 AgsLine *line)
{
  AgsVolumeChannel *volume_channel;

  GList *start_list, *list;
  
  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_DOUBLE);

  g_value_set_double(&value,
		     gtk_range_get_value(range));

  /* play context */
  g_object_get(line->channel,
	       "play", &start_list,
	       NULL);

  list = start_list;
  
  while((list = ags_recall_find_type(list, AGS_TYPE_VOLUME_CHANNEL)) != NULL){
    AgsPort *port;
    
    volume_channel = AGS_VOLUME_CHANNEL(list->data);

    g_object_get(volume_channel,
		 "volume", &port,
		 NULL);
    
    ags_port_safe_write(port,
			&value);

    /* iterate */
    list = list->next;
  }

  g_list_free(start_list);
  
  /* recall context */
  g_object_get(line->channel,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while((list = ags_recall_find_type(list, AGS_TYPE_VOLUME_CHANNEL)) != NULL){
    AgsPort *port;
    
    volume_channel = AGS_VOLUME_CHANNEL(list->data);

    g_object_get(volume_channel,
		 "volume", &port,
		 NULL);
    
    ags_port_safe_write(port,
			&value);

    /* iterate */
    list = list->next;
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

  pad = AGS_PAD(line->pad);
  
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
