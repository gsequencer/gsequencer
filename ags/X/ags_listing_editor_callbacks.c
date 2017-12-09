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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_machine_editor.h>

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

  if(machine_editor->machine != NULL){
    ags_listing_editor_add_children(listing_editor,
				    machine_editor->machine->audio, 0,
				    FALSE);
  }
  
  return(0);
}

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
