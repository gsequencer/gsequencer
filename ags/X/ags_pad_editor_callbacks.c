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

#include <ags/X/ags_pad_editor_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

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

    AgsChannel *channel, *next_pad;

    guint i;

    /* get some channel fields */
    g_ojbect_get(pad_editor->pad,
		 "next-pad", &next_pad,
		 NULL);

    /* get current last of pad */
    channel = ags_channel_nth(pad_editor->pad,
			      audio_channels_old);

    while(channel != next_pad){
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
      g_object_get(channel,
		   "next", &channel,
		   NULL);
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
