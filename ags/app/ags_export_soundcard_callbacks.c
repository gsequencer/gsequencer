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

#include <ags/app/ags_export_soundcard_callbacks.h>

#include <ags/app/ags_export_window.h>

#include <ags/i18n.h>

void
ags_export_soundcard_backend_callback(GtkWidget *combo_box,
				      AgsExportSoundcard *export_soundcard)
{
  AgsApplicationContext *application_context;

  GList *start_soundcard, *soundcard;

  gchar *backend;
  gchar *device;
  gchar *tmp_device;
  
  gboolean found_card;

  application_context = ags_application_context_get_instance();

  /* refresh card */
  ags_export_soundcard_refresh_card(export_soundcard);

  /* get soundcard */
  start_soundcard = NULL;

  if(application_context != NULL){
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  }

  soundcard = start_soundcard;
  
  backend = gtk_combo_box_text_get_active_text(export_soundcard->backend);
  device = gtk_combo_box_text_get_active_text(export_soundcard->card);

  if(backend == NULL ||
     device == NULL){
    g_list_free_full(start_soundcard,
		     g_object_unref);
    
    return;
  }
  
  found_card = FALSE;
  
  while(soundcard != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "wasapi",
			    7)){
      if(AGS_IS_WASAPI_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if((tmp_device == NULL && !g_ascii_strcasecmp(device,
						      "(null)")) ||
	   (tmp_device != NULL && !g_ascii_strcasecmp(device,
						      tmp_device))){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);
	  
	  found_card = TRUE;
	}

	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa",
				  5)){
      if(AGS_IS_ALSA_DEVOUT(soundcard->data)){
	 tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	 	 
	 if(!g_ascii_strcasecmp(device,
				tmp_device)){
	   g_object_set(export_soundcard,
			"soundcard", soundcard->data,
			NULL);
	   
	   found_card = TRUE;
	 }
	 
	 g_free(tmp_device);
	 
	 if(found_card){
	   break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){    
      if(AGS_IS_OSS_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);
	  
	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "jack",
				  5)){
      if(AGS_IS_JACK_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){	
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);

	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "pulse",
				  6)){
      if(AGS_IS_PULSE_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);

	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "core-audio",
				  11)){
      if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);
	  
	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }
  
    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   g_object_unref);
  
  if(!found_card){
    g_object_set(export_soundcard,
		 "soundcard", NULL,
		 NULL);
  }
}

void
ags_export_soundcard_card_callback(GtkWidget *combo_box,
				   AgsExportSoundcard *export_soundcard)
{
  AgsApplicationContext *application_context;

  GList *start_soundcard, *soundcard;

  gchar *backend;
  gchar *device;
  gchar *tmp_device;

  gboolean found_card;

  application_context = ags_application_context_get_instance();

  /* get soundcard */
  start_soundcard = NULL;
    
  if(application_context != NULL){
    start_soundcard = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  }

  soundcard = start_soundcard;
  
  backend = gtk_combo_box_text_get_active_text(export_soundcard->backend);
  device = gtk_combo_box_text_get_active_text(export_soundcard->card);

  if(backend == NULL ||
     device == NULL){
    g_list_free_full(start_soundcard,
		     g_object_unref);

    return;
  }
  
  found_card = FALSE;
  
  while(soundcard != NULL){
    if(!g_ascii_strncasecmp(backend,
			    "wasapi",
			    7)){
      if(AGS_IS_WASAPI_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if((tmp_device == NULL && !g_ascii_strcasecmp(device,
						      "(null)")) ||
	   (tmp_device != NULL && !g_ascii_strcasecmp(device,
						      tmp_device))){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);
	  
	  found_card = TRUE;
	}

	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "alsa",
				  5)){
      if(AGS_IS_ALSA_DEVOUT(soundcard->data)){
	 tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	 	 
	 if(!g_ascii_strcasecmp(device,
				tmp_device)){
	   g_object_set(export_soundcard,
			"soundcard", soundcard->data,
			NULL);
	   
	   found_card = TRUE;
	 }
	 
	 g_free(tmp_device);
	 
	 if(found_card){
	   break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "oss",
				  4)){    
      if(AGS_IS_OSS_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);
	  
	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "jack",
				  5)){
      if(AGS_IS_JACK_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){	
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);

	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "pulse",
				  6)){
      if(AGS_IS_PULSE_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);

	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }else if(!g_ascii_strncasecmp(backend,
				  "core-audio",
				  11)){
      if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard->data)){
	tmp_device = ags_soundcard_get_device(AGS_SOUNDCARD(soundcard->data));
	
	if(!g_ascii_strcasecmp(device,
			       tmp_device)){
	  g_object_set(export_soundcard,
		       "soundcard", soundcard->data,
		       NULL);
	  
	  found_card = TRUE;	
	}
	
	g_free(tmp_device);
	
	if(found_card){
	  break;
	}
      }
    }

    soundcard = soundcard->next;
  }

  g_list_free_full(start_soundcard,
		   g_object_unref);
  
  if(!found_card){
    g_object_set(export_soundcard,
		 "soundcard", NULL,
		 NULL);
  }
}

void
ags_export_soundcard_file_chooser_button_callback(GtkWidget *file_chooser_button,
						  AgsExportSoundcard *export_soundcard)
{
  AgsExportWindow *export_window;
  GtkFileChooserDialog *file_chooser;
  
  export_window = (AgsExportWindow *) gtk_widget_get_ancestor(GTK_WIDGET(export_soundcard),
							      AGS_TYPE_EXPORT_WINDOW);
  
  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new("Export to file ...",
								      GTK_WINDOW(export_window),
								      GTK_FILE_CHOOSER_ACTION_SAVE,
								      i18n("_Cancel"), GTK_RESPONSE_CANCEL,
								      i18n("_Open"), GTK_RESPONSE_ACCEPT,
								      NULL);
  if(gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT){
    char *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    gtk_entry_set_text(export_soundcard->filename,
		       filename);
  }
  
  gtk_widget_destroy((GtkWidget *) file_chooser);
}
