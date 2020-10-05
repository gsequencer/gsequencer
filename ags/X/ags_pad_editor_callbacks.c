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

#include <ags/X/ags_pad_editor_callbacks.h>

#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

void
ags_pad_editor_resize_audio_channels_callback(AgsMachine *machine,
					      guint audio_channels, guint audio_channels_old,
					      AgsPadEditor *pad_editor)
{
  AgsAudio *audio;
  
  if(audio_channels > audio_channels_old){
    AgsLineEditor *line_editor;

    AgsChannel *channel, *next_pad, *next_channel, *nth_channel;

    guint i;

    /* get some channel fields */
    next_pad = ags_channel_next_pad(pad_editor->pad);
    
    /* get current last of pad */
    nth_channel = ags_channel_nth(pad_editor->pad,
				  audio_channels_old);

    channel = nth_channel;

    next_channel = NULL;
    
    while(channel != next_pad && channel != NULL){
      /* instantiate line editor */
      line_editor = ags_line_editor_new(channel);
      line_editor->editor_type_count = pad_editor->editor_type_count;
      line_editor->editor_type = (GType *) malloc(line_editor->editor_type_count * sizeof(GType));

      for(i = 0; i < line_editor->editor_type_count; i++){
	line_editor->editor_type[i] = pad_editor->editor_type[i];
      }
      
      gtk_box_pack_start(GTK_BOX(pad_editor->line_editor),
			 GTK_WIDGET(line_editor),
			 FALSE, FALSE,
			 0);
      ags_connectable_connect(AGS_CONNECTABLE(line_editor));
      gtk_widget_show_all(GTK_WIDGET(line_editor));

      /* iterate */
      next_channel = ags_channel_next(channel);

      g_object_unref(channel);

      channel = next_channel;
    }

    if(next_pad != NULL){
      g_object_unref(next_pad);
    }

    if(next_channel != NULL){
      g_object_unref(next_channel);
    }
  }else{
    GList *list, *list_next, *list_start;

    list_start = 
      list = gtk_container_get_children(GTK_CONTAINER(pad_editor->line_editor));

    list = g_list_nth(list,
		      audio_channels);

    while(list != NULL){
      list_next = list->next;

      gtk_widget_destroy(GTK_WIDGET(list->data));

      list = list_next;
    }

    g_list_free(list_start);
  }
}
