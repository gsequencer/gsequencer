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

#include <ags/X/machine/ags_equalizer10_callbacks.h>

#include <ags/X/ags_window.h>

void
ags_equalizer10_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsEqualizer10 *equalizer10)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) equalizer10, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_EQUALIZER10)->counter);

  g_object_set(AGS_MACHINE(equalizer10),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_EQUALIZER10);
  g_free(str);
}

void
ags_equalizer10_resize_audio_channels_callback(AgsEqualizer10 *equalizer10,
					       guint audio_channels, guint audio_channels_old,
					       gpointer data)
{
  AgsAudio *audio;

  guint i;
  
  audio = AGS_MACHINE(equalizer10)->audio;
  
  if(audio_channels > audio_channels_old){
    if((AGS_MACHINE_MAPPED_RECALL & (AGS_MACHINE(equalizer10)->flags)) != 0){
      AgsChannel *channel;
      AgsPort *port;
            
      guint i;

      /* get some fields */
      g_object_get(audio,
		   "audio-channels", &audio_channels,
		   "input", &channel,
		   NULL);

      if(channel != NULL){
	g_object_unref(channel);
      }
  
      /* ags-analyse */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-eq10",
				audio_channels_old, audio_channels,
				0, 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      channel = ags_channel_nth(channel, audio_channels_old);
      
      for(i = audio_channels_old; i < audio_channels; i++){
	GList *start_play, *play;
	GList *start_recall, *recall;

	g_object_get(channel,
		     "play", &start_play,
		     "recall", &start_recall,
		     NULL);
	
	/* peak 28Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-28hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_28hz_play_port = g_list_prepend(equalizer10->peak_28hz_play_port,
							    port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-28hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_28hz_recall_port = g_list_prepend(equalizer10->peak_28hz_recall_port,
							      port);
	}

	/* peak 56Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-56hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_56hz_play_port = g_list_prepend(equalizer10->peak_56hz_play_port,
							    port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-56hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_56hz_recall_port = g_list_prepend(equalizer10->peak_56hz_recall_port,
							      port);
	}

	/* peak 112Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-112hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_112hz_play_port = g_list_prepend(equalizer10->peak_112hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-112hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_112hz_recall_port = g_list_prepend(equalizer10->peak_112hz_recall_port,
							       port);
	}

	/* peak 224Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-224hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_224hz_play_port = g_list_prepend(equalizer10->peak_224hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-224hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_224hz_recall_port = g_list_prepend(equalizer10->peak_224hz_recall_port,
							       port);
	}

	/* peak 448Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-448hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_448hz_play_port = g_list_prepend(equalizer10->peak_448hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-448hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_448hz_recall_port = g_list_prepend(equalizer10->peak_448hz_recall_port,
							       port);
	}

	/* peak 896Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-896hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_896hz_play_port = g_list_prepend(equalizer10->peak_896hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-896hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_896hz_recall_port = g_list_prepend(equalizer10->peak_896hz_recall_port,
							       port);
	}

	/* peak 1792Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-1792hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_1792hz_play_port = g_list_prepend(equalizer10->peak_1792hz_play_port,
							      port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-1792hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_1792hz_recall_port = g_list_prepend(equalizer10->peak_1792hz_recall_port,
								port);
	}

	/* peak 3584Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-3584hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_3584hz_play_port = g_list_prepend(equalizer10->peak_3584hz_play_port,
							      port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-3584hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_3584hz_recall_port = g_list_prepend(equalizer10->peak_3584hz_recall_port,
								port);
	}

	/* peak 7168Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-7168hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_7168hz_play_port = g_list_prepend(equalizer10->peak_7168hz_play_port,
							      port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-7168hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_7168hz_recall_port = g_list_prepend(equalizer10->peak_7168hz_recall_port,
								port);
	}

	/* peak 14336Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-14336hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_14336hz_play_port = g_list_prepend(equalizer10->peak_14336hz_play_port,
							       port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-14336hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_14336hz_recall_port = g_list_prepend(equalizer10->peak_14336hz_recall_port,
								 port);
	}

	/* pressure  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./pressure[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->pressure_play_port = g_list_prepend(equalizer10->pressure_play_port,
							    port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./pressure[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->pressure_recall_port = g_list_prepend(equalizer10->pressure_recall_port,
							      port);
	}

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);

	/* iterate */
	g_object_get(channel,
		     "next", &channel,
		     NULL);

	if(channel != NULL){
	  g_object_unref(channel);
	}
      }
    }
  }else{
    GList *list_start, *list;

    /* peak 28Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_28hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_28hz_play_port = g_list_remove(equalizer10->peak_28hz_play_port,
							list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_28hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_28hz_recall_port = g_list_remove(equalizer10->peak_28hz_recall_port,
							  list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 56Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_56hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_56hz_play_port = g_list_remove(equalizer10->peak_56hz_play_port,
							list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_56hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_56hz_recall_port = g_list_remove(equalizer10->peak_56hz_recall_port,
							  list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 112Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_112hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_112hz_play_port = g_list_remove(equalizer10->peak_112hz_play_port,
							 list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_112hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_112hz_recall_port = g_list_remove(equalizer10->peak_112hz_recall_port,
							   list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 224Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_224hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_224hz_play_port = g_list_remove(equalizer10->peak_224hz_play_port,
							 list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_224hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_224hz_recall_port = g_list_remove(equalizer10->peak_224hz_recall_port,
							   list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 448Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_448hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_448hz_play_port = g_list_remove(equalizer10->peak_448hz_play_port,
							 list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_448hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_448hz_recall_port = g_list_remove(equalizer10->peak_448hz_recall_port,
							   list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 896Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_896hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_896hz_play_port = g_list_remove(equalizer10->peak_896hz_play_port,
							 list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_896hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_896hz_recall_port = g_list_remove(equalizer10->peak_896hz_recall_port,
							   list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 1792Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_1792hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_1792hz_play_port = g_list_remove(equalizer10->peak_1792hz_play_port,
							  list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_1792hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_1792hz_recall_port = g_list_remove(equalizer10->peak_1792hz_recall_port,
							    list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 3584Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_3584hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_3584hz_play_port = g_list_remove(equalizer10->peak_3584hz_play_port,
							  list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_3584hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_3584hz_recall_port = g_list_remove(equalizer10->peak_3584hz_recall_port,
							    list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 7168Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_7168hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_7168hz_play_port = g_list_remove(equalizer10->peak_7168hz_play_port,
							  list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_7168hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_7168hz_recall_port = g_list_remove(equalizer10->peak_7168hz_recall_port,
							    list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* peak 14336Hz - port */
    list = 
      list_start = g_list_copy(equalizer10->peak_14336hz_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_14336hz_play_port = g_list_remove(equalizer10->peak_14336hz_play_port,
							   list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->peak_14336hz_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->peak_14336hz_recall_port = g_list_remove(equalizer10->peak_14336hz_recall_port,
							     list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    /* pressure - port */
    list = 
      list_start = g_list_copy(equalizer10->pressure_play_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->pressure_play_port = g_list_remove(equalizer10->pressure_play_port,
							list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);

    list = 
      list_start = g_list_copy(equalizer10->pressure_recall_port);

    for(i = audio_channels; i < audio_channels_old && list != NULL; i++){
      equalizer10->pressure_recall_port = g_list_remove(equalizer10->pressure_recall_port,
							  list->data);
      g_object_unref(list->data);
      
      list = list->next;
    }

    g_list_free(list_start);
  }
}

void
ags_equalizer10_resize_pads_callback(AgsEqualizer10 *equalizer10,
				     GType channel_type,
				     guint pads, guint pads_old,
				     gpointer data)
{  
  if(pads_old == 0 && channel_type == AGS_TYPE_INPUT){
    if((AGS_MACHINE_MAPPED_RECALL & (AGS_MACHINE(equalizer10)->flags)) != 0){
      AgsAudio *audio;
      AgsChannel *channel;
      AgsPort *port;

      GList *start_play, *play;
      GList *start_recall, *recall;
      
      guint audio_channels;
      guint i;

      audio = AGS_MACHINE(equalizer10)->audio;
  
      /* get some fields */
      g_object_get(audio,
		   "audio-channels", &audio_channels,
		   "input", &channel,
		   NULL);

      if(channel != NULL){
	g_object_unref(channel);
      }
  
      /* ags-analyse */
      ags_recall_factory_create(audio,
				NULL, NULL,
				"ags-eq10",
				0, audio_channels,
				0, 1,
				(AGS_RECALL_FACTORY_INPUT |
				 AGS_RECALL_FACTORY_PLAY |
				 AGS_RECALL_FACTORY_RECALL |
				 AGS_RECALL_FACTORY_ADD),
				0);

      for(i = 0; i < audio_channels; i++){
	g_object_get(channel,
		     "play", &start_play,
		     "recall", &start_recall,
		     NULL);

	/* peak 28Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-28hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_28hz_play_port = g_list_prepend(equalizer10->peak_28hz_play_port,
							    port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-28hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_28hz_recall_port = g_list_prepend(equalizer10->peak_28hz_recall_port,
							      port);
	}

	/* peak 56Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-56hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_56hz_play_port = g_list_prepend(equalizer10->peak_56hz_play_port,
							    port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-56hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_56hz_recall_port = g_list_prepend(equalizer10->peak_56hz_recall_port,
							      port);
	}

	/* peak 112Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-112hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_112hz_play_port = g_list_prepend(equalizer10->peak_112hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-112hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_112hz_recall_port = g_list_prepend(equalizer10->peak_112hz_recall_port,
							       port);
	}

	/* peak 224Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-224hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_224hz_play_port = g_list_prepend(equalizer10->peak_224hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-224hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_224hz_recall_port = g_list_prepend(equalizer10->peak_224hz_recall_port,
							       port);
	}

	/* peak 448Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-448hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_448hz_play_port = g_list_prepend(equalizer10->peak_448hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-448hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_448hz_recall_port = g_list_prepend(equalizer10->peak_448hz_recall_port,
							       port);
	}

	/* peak 896Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-896hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_896hz_play_port = g_list_prepend(equalizer10->peak_896hz_play_port,
							     port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-896hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_896hz_recall_port = g_list_prepend(equalizer10->peak_896hz_recall_port,
							       port);
	}

	/* peak 1792Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-1792hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_1792hz_play_port = g_list_prepend(equalizer10->peak_1792hz_play_port,
							      port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-1792hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_1792hz_recall_port = g_list_prepend(equalizer10->peak_1792hz_recall_port,
								port);
	}

	/* peak 3584Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-3584hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_3584hz_play_port = g_list_prepend(equalizer10->peak_3584hz_play_port,
							      port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-3584hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_3584hz_recall_port = g_list_prepend(equalizer10->peak_3584hz_recall_port,
								port);
	}

	/* peak 7168Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-7168hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_7168hz_play_port = g_list_prepend(equalizer10->peak_7168hz_play_port,
							      port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-7168hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_7168hz_recall_port = g_list_prepend(equalizer10->peak_7168hz_recall_port,
								port);
	}

	/* peak 14336Hz  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./peak-14336hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_14336hz_play_port = g_list_prepend(equalizer10->peak_14336hz_play_port,
							       port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./peak-14336hz[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->peak_14336hz_recall_port = g_list_prepend(equalizer10->peak_14336hz_recall_port,
								 port);
	}

	/* pressure  - find port */
	port = ags_equalizer10_find_specifier(start_play,
					      "./pressure[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->pressure_play_port = g_list_prepend(equalizer10->pressure_play_port,
							    port);
	}

	port = ags_equalizer10_find_specifier(start_recall,
					      "./pressure[0]");

	if(port != NULL){
	  g_object_ref(port);

	  equalizer10->pressure_recall_port = g_list_prepend(equalizer10->pressure_recall_port,
							      port);
	}

	g_list_free_full(start_play,
			 g_object_unref);
	g_list_free_full(start_recall,
			 g_object_unref);
    
	/* iterate */
	g_object_get(channel,
		     "next", &channel,
		     NULL);

	if(channel != NULL){
	  g_object_unref(channel);
	}
      }
    }
  }

  if(pads == 0 && channel_type == AGS_TYPE_INPUT){
    /* peak 28Hz - ports */
    g_list_free_full(equalizer10->peak_28hz_play_port, g_object_unref);
    equalizer10->peak_28hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_28hz_recall_port, g_object_unref);
    equalizer10->peak_28hz_recall_port = NULL;

    /* peak 56Hz - ports */
    g_list_free_full(equalizer10->peak_56hz_play_port, g_object_unref);
    equalizer10->peak_56hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_56hz_recall_port, g_object_unref);
    equalizer10->peak_56hz_recall_port = NULL;

    /* peak 112Hz - ports */
    g_list_free_full(equalizer10->peak_112hz_play_port, g_object_unref);
    equalizer10->peak_112hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_112hz_recall_port, g_object_unref);
    equalizer10->peak_112hz_recall_port = NULL;

    /* peak 224Hz - ports */
    g_list_free_full(equalizer10->peak_224hz_play_port, g_object_unref);
    equalizer10->peak_224hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_224hz_recall_port, g_object_unref);
    equalizer10->peak_224hz_recall_port = NULL;

    /* peak 448Hz - ports */
    g_list_free_full(equalizer10->peak_448hz_play_port, g_object_unref);
    equalizer10->peak_448hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_448hz_recall_port, g_object_unref);
    equalizer10->peak_448hz_recall_port = NULL;

    /* peak 896Hz - ports */
    g_list_free_full(equalizer10->peak_896hz_play_port, g_object_unref);
    equalizer10->peak_896hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_896hz_recall_port, g_object_unref);
    equalizer10->peak_896hz_recall_port = NULL;

    /* peak 1792Hz - ports */
    g_list_free_full(equalizer10->peak_1792hz_play_port, g_object_unref);
    equalizer10->peak_1792hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_1792hz_recall_port, g_object_unref);
    equalizer10->peak_1792hz_recall_port = NULL;

    /* peak 3584Hz - ports */
    g_list_free_full(equalizer10->peak_3584hz_play_port, g_object_unref);
    equalizer10->peak_3584hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_3584hz_recall_port, g_object_unref);
    equalizer10->peak_3584hz_recall_port = NULL;

    /* peak 7168Hz - ports */
    g_list_free_full(equalizer10->peak_7168hz_play_port, g_object_unref);
    equalizer10->peak_7168hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_7168hz_recall_port, g_object_unref);
    equalizer10->peak_7168hz_recall_port = NULL;

    /* peak 14336Hz - ports */
    g_list_free_full(equalizer10->peak_14336hz_play_port, g_object_unref);
    equalizer10->peak_14336hz_play_port = NULL;
    
    g_list_free_full(equalizer10->peak_14336hz_recall_port, g_object_unref);
    equalizer10->peak_14336hz_recall_port = NULL;

    /* pressure - ports */
    g_list_free_full(equalizer10->pressure_play_port, g_object_unref);
    equalizer10->pressure_play_port = NULL;
    
    g_list_free_full(equalizer10->pressure_recall_port, g_object_unref);
    equalizer10->pressure_recall_port = NULL;
  }
}

void
ags_equalizer10_peak_28hz_callback(GtkRange *range,
				   AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_28hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_28hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_56hz_callback(GtkRange *range,
				   AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_56hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_56hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_112hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_112hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_112hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_224hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_224hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_224hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_448hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_448hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_448hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_896hz_callback(GtkRange *range,
				    AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_896hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_896hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_1792hz_callback(GtkRange *range,
				     AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_1792hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_1792hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_3584hz_callback(GtkRange *range,
				     AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_3584hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_3584hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_7168hz_callback(GtkRange *range,
				     AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_7168hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_7168hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_peak_14336hz_callback(GtkRange *range,
				      AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->peak_14336hz_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->peak_14336hz_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}

void
ags_equalizer10_pressure_callback(GtkRange *range,
				  AgsEqualizer10 *equalizer10)
{
  GList *list;

  GValue value = {0,};

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value, gtk_range_get_value(range));

  /* play port */
  list = equalizer10->pressure_play_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }

  /* recall port */
  list = equalizer10->pressure_recall_port;
  
  while(list != NULL){
    ags_port_safe_write(list->data,
			&value);

    list = list->next;
  }
  
  g_value_unset(&value);
}
