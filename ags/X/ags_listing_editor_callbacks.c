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

#include <ags/X/ags_listing_editor_callbacks.h>

int
ags_listing_editor_parent_set_callback(GtkWidget *widget,
				       GtkObject *old_parent,
				       AgsListingEditor *listing_editor)
{
  AgsMachineEditor *machine_editor;

  if(old_parent != NULL)
    return(0);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor(widget,
								AGS_TYPE_MACHINE_EDITOR);

  if(machine_editor->machine != NULL)
    ags_listing_editor_add_children(listing_editor,
				    machine_editor->machine->audio, 0,
				    FALSE);

  return(0);
}

void
ags_listing_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
				     guint pads, guint pads_old,
				     AgsListingEditor *listing_editor)
{
  if(channel_type != listing_editor->channel_type)
    return;

  if(pads_old < pads){
    guint nth_channel;
  
    nth_channel = pads_old * audio->audio_channels;
    
    ags_listing_editor_add_children(listing_editor,
				    audio, nth_channel,
				    TRUE);
  }else{
    GList *list, *list_next, *list_start;

    list_start = 
      list = gtk_container_get_children(GTK_CONTAINER(listing_editor->child));
    list = g_list_nth(list, pads);
    
    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list_next;
    }

    g_list_free(list_start);
  }
}
