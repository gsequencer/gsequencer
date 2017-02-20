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

#include <ags/object/ags_connectable.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

int
ags_pad_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPadEditor *pad_editor)
{
  return(0);
}

void
ags_pad_editor_set_audio_channels_callback(AgsAudio *audio,
					   guint audio_channels, guint audio_channels_old,
					   AgsPadEditor *pad_editor)
{
  if(audio_channels > audio_channels_old){
    AgsLineEditor *line_editor;

    AgsChannel *channel, *next_pad;

    AgsMutexManager *mutex_manager;

    guint i;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *channel_mutex;
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* lookup channel mutex */
    pthread_mutex_lock(application_mutex);

    channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					     pad_editor->pad);
    
    pthread_mutex_unlock(application_mutex);

    /* get some channel fields */
    pthread_mutex_lock(channel_mutex);

    next_pad = pad_editor->pad->next_pad;

    pthread_mutex_unlock(channel_mutex);

    /* get current last of pad */
    channel = ags_channel_nth(pad_editor->pad,
			      audio_channels_old);

    while(channel != next_pad){
      /* lookup channel mutex */
      pthread_mutex_lock(application_mutex);

      channel_mutex = ags_mutex_manager_lookup(mutex_manager,
					       pad_editor->pad);
    
      pthread_mutex_unlock(application_mutex);

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
      pthread_mutex_lock(channel_mutex);
      
      channel = channel->next;

      pthread_mutex_unlock(channel_mutex);
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
