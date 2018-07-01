/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/X/machine/ags_spectrometer_callbacks.h>

#include <ags/X/ags_window.h>

void
ags_spectrometer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsSpectrometer *spectrometer)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) spectrometer, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_SPECTROMETER)->counter);

  g_object_set(AGS_MACHINE(spectrometer),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_SPECTROMETER);
  g_free(str);
}

void
ags_spectrometer_resize_audio_channels_callback(AgsSpectrometer *spectrometer,
						guint audio_channels, guint audio_channels_old,
						gpointer data)
{
  AgsCartesian *cartesian;
  AgsPlot *fg_plot;

  AgsAudio *audio;

  guint i;

  cartesian = spectrometer->cartesian;
  
  audio = AGS_MACHINE(spectrometer)->audio;
  
  if(audio_channels > audio_channels_old){
    if((AGS_MACHINE_MAPPED_RECALL & (AGS_MACHINE(spectrometer)->flags)) != 0){
      AgsChannel *channel;
      AgsPort *port;
            
      AgsMutexManager *mutex_manager;
      
      guint i;

      pthread_mutex_t *application_mutex;
      pthread_mutex_t *audio_mutex;
      pthread_mutex_t *channel_mutex;
  
      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);      

      /* lookup audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_channels = audio->audio_channels;

      channel = audio->input;
  
      pthread_mutex_unlock(audio_mutex);
  
      /* ags-analyse */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-analyse",
				audio_channels_old, audio_channels,
				0, 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      channel = ags_channel_nth(channel, audio_channels_old);
      
      for(i = audio_channels_old; i < audio_channels; i++){
	fg_plot = ags_spectrometer_fg_plot_alloc(spectrometer,
						 0.125, 0.5, 1.0);
	ags_cartesian_add_plot(cartesian,
			       fg_plot);
    
	spectrometer->fg_plot = g_list_prepend(spectrometer->fg_plot,
					       fg_plot);

	/* lookup channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
    
	pthread_mutex_unlock(application_mutex);

	/* frequency - find port */
	pthread_mutex_lock(channel_mutex);

	port = ags_spectrometer_find_specifier(channel->play,
					       "./frequency-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->frequency_buffer_play_port = g_list_prepend(spectrometer->frequency_buffer_play_port,
								    port);
	}

	port = ags_spectrometer_find_specifier(channel->recall,
					       "./frequency-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->frequency_buffer_recall_port = g_list_prepend(spectrometer->frequency_buffer_recall_port,
								      port);
	}

	/* magnitude - find port */
	port = ags_spectrometer_find_specifier(channel->play,
					       "./magnitude-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->magnitude_buffer_play_port = g_list_prepend(spectrometer->magnitude_buffer_play_port,
								    port);
	}

	port = ags_spectrometer_find_specifier(channel->recall,
					       "./magnitude-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->magnitude_buffer_recall_port = g_list_prepend(spectrometer->magnitude_buffer_recall_port,
								      port);
	}
    
	/* iterate */
	channel = channel->next;
    
	pthread_mutex_unlock(channel_mutex);
      }
    }
  }else{
    GList *list_start, *list;

    /* plot */
    list = 
      list_start = g_list_copy(spectrometer->fg_plot);
    
    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      spectrometer->fg_plot = g_list_remove(spectrometer->fg_plot,
					    list->data);
      ags_cartesian_remove_plot(cartesian,
				list->data);
      ags_plot_free(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* frequency - port */
    list = 
      list_start = g_list_copy(spectrometer->frequency_buffer_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      spectrometer->frequency_buffer_play_port = g_list_remove(spectrometer->frequency_buffer_play_port,
							       list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(spectrometer->frequency_buffer_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      spectrometer->frequency_buffer_recall_port = g_list_remove(spectrometer->frequency_buffer_recall_port,
								 list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* magnitude - port */
    list = 
      list_start = g_list_copy(spectrometer->magnitude_buffer_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      spectrometer->magnitude_buffer_play_port = g_list_remove(spectrometer->magnitude_buffer_play_port,
							       list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(spectrometer->magnitude_buffer_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      spectrometer->magnitude_buffer_recall_port = g_list_remove(spectrometer->magnitude_buffer_recall_port,
								 list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_spectrometer_resize_pads_callback(AgsSpectrometer *spectrometer,
				      GType channel_type,
				      guint pads, guint pads_old,
				      gpointer data)
{
  AgsCartesian *cartesian;
  
  cartesian = spectrometer->cartesian;
  
  if(pads_old == 0 && channel_type == AGS_TYPE_INPUT){
    if((AGS_MACHINE_MAPPED_RECALL & (AGS_MACHINE(spectrometer)->flags)) != 0){
      AgsAudio *audio;
      AgsChannel *channel;
      AgsPort *port;
      
      AgsMutexManager *mutex_manager;
      
      guint audio_channels;
      guint i;

      pthread_mutex_t *application_mutex;
      pthread_mutex_t *audio_mutex;
      pthread_mutex_t *channel_mutex;
  
      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);      

      audio = AGS_MACHINE(spectrometer)->audio;

      /* lookup audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					     (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);
  
      /* get some fields */
      pthread_mutex_lock(audio_mutex);

      audio_channels = audio->audio_channels;

      channel = audio->input;
  
      pthread_mutex_unlock(audio_mutex);
  
      /* ags-analyse */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-analyse",
				0, audio_channels,
				0, 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      for(i = 0; i < audio_channels; i++){
	AgsPlot *fg_plot;
	
	fg_plot = ags_spectrometer_fg_plot_alloc(spectrometer,
						 0.125, 0.5, 1.0);
	ags_cartesian_add_plot(cartesian,
			       fg_plot);
    
	spectrometer->fg_plot = g_list_prepend(spectrometer->fg_plot,
					       fg_plot);

	/* lookup channel mutex */
	pthread_mutex_lock(application_mutex);

	channel_mutex = ags_mutex_manager_lookup(mutex_manager,
						 (GObject *) channel);
    
	pthread_mutex_unlock(application_mutex);

	/* frequency - find port */
	pthread_mutex_lock(channel_mutex);

	port = ags_spectrometer_find_specifier(channel->play,
					       "./frequency-buffer[0]");

	if(port != NULL){
 	  g_object_ref(port);

	  spectrometer->frequency_buffer_play_port = g_list_prepend(spectrometer->frequency_buffer_play_port,
								    port);
	}

	port = ags_spectrometer_find_specifier(channel->recall,
					       "./frequency-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->frequency_buffer_recall_port = g_list_prepend(spectrometer->frequency_buffer_recall_port,
								      port);
	}

	/* magnitude - find port */
	port = ags_spectrometer_find_specifier(channel->play,
					       "./magnitude-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->magnitude_buffer_play_port = g_list_prepend(spectrometer->magnitude_buffer_play_port,
								    port);
	}

	port = ags_spectrometer_find_specifier(channel->recall,
					       "./magnitude-buffer[0]");

	if(port != NULL){
	  g_object_ref(port);

	  spectrometer->magnitude_buffer_recall_port = g_list_prepend(spectrometer->magnitude_buffer_recall_port,
								      port);
	}
    
	/* iterate */
	channel = channel->next;
    
	pthread_mutex_unlock(channel_mutex);
      }
    }
  }

  if(pads == 0 && channel_type == AGS_TYPE_INPUT){
    GList *list_start, *list;

    list = 
      list_start = g_list_copy(spectrometer->fg_plot);
    
    while(list != NULL){
      spectrometer->fg_plot = g_list_remove(spectrometer->fg_plot,
					    list->data);
      ags_cartesian_remove_plot(cartesian,
				list->data);
      ags_plot_free(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* frequency - ports */
    g_list_free_full(spectrometer->frequency_buffer_play_port, g_object_unref);
    spectrometer->frequency_buffer_play_port = NULL;

    g_list_free_full(spectrometer->frequency_buffer_recall_port, g_object_unref);
    spectrometer->frequency_buffer_recall_port = NULL;

    /* magnitude - ports */
    g_list_free_full(spectrometer->magnitude_buffer_play_port, g_object_unref);
    spectrometer->magnitude_buffer_play_port = NULL;

    g_list_free_full(spectrometer->magnitude_buffer_recall_port, g_object_unref);
    spectrometer->magnitude_buffer_recall_port = NULL;
  }
}
