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

#include <ags/X/ags_output_listing_editor_callbacks.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_connection_editor.h>

int
ags_output_listing_editor_parent_set_callback(GtkWidget *widget,
					      GtkObject *old_parent,
					      AgsOutputListingEditor *output_listing_editor)
{
  AgsConnectionEditor *connection_editor;

  if(old_parent != NULL){
    return(0);
  }

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(widget,
								      AGS_TYPE_CONNECTION_EDITOR);

  if(connection_editor != NULL &&
     connection_editor->machine != NULL){
    ags_output_listing_editor_add_children(output_listing_editor,
					   connection_editor->machine->audio, 0,
					   FALSE);
  }
  
  return(0);
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
    AgsMutexManager *mutex_manager;

    guint audio_channels;
    guint nth_channel;

    pthread_mutex_t *application_mutex;
    pthread_mutex_t *audio_mutex;
    
    mutex_manager = ags_mutex_manager_get_instance();
    application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

    /* lookup audio mutex */
    pthread_mutex_lock(application_mutex);
  
    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   audio);
  
    pthread_mutex_unlock(application_mutex);

    /* get some audio fields */
    pthread_mutex_lock(audio_mutex);

    audio_channels = audio->audio_channels;

    pthread_mutex_unlock(audio_mutex);

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
