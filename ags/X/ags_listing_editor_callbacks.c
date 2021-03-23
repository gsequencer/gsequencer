/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_machine_editor.h>

void
ags_listing_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					guint pads, guint pads_old,
					AgsListingEditor *listing_editor)
{
  AgsAudio *audio;

  audio = machine->audio;
  
  if(channel_type != listing_editor->channel_type){
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
