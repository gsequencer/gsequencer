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

#include <ags/app/ags_output_listing_editor_callbacks.h>

#include <ags/app/ags_machine.h>
#include <ags/app/ags_connection_editor.h>

void
ags_output_listing_editor_parent_set_callback(GtkWidget *widget,
					      GtkWidget *old_parent,
					      AgsOutputListingEditor *output_listing_editor)
{
  AgsConnectionEditor *connection_editor;

  if(old_parent != NULL){
    return;
  }

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(widget,
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor != NULL &&
     connection_editor->machine != NULL){
    ags_output_listing_editor_add_children(output_listing_editor,
					   connection_editor->machine->audio, 0,
					   FALSE);
  }  
}

void
ags_output_listing_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					       guint pads, guint pads_old,
					       AgsOutputListingEditor *output_listing_editor)
{
  AgsAudio *audio;

  audio = machine->audio;
  
  if(channel_type != output_listing_editor->channel_type){
    return;
  }

  if(pads_old < pads){
    guint audio_channels;
    guint nth_channel;

    /* get some audio fields */
    g_object_get(audio,
		 "audio-channels", &audio_channels,
		 NULL);

    /* add children */
    nth_channel = pads_old * audio_channels;
    
    ags_output_listing_editor_add_children(output_listing_editor,
					   audio, nth_channel,
					   TRUE);
  }else{
    GList *list, *list_next, *list_start;

    list_start = 
      list = gtk_container_get_children(GTK_CONTAINER(output_listing_editor->child));
    list = g_list_nth(list, pads);
    
    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list_next;
    }

    g_list_free(list_start);
  }
}
