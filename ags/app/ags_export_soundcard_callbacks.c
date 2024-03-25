/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_pcm_file_dialog.h>
#include <ags/app/ags_export_window.h>

#include <ags/i18n.h>

void ags_export_soundcard_open_response_callback(AgsPCMFileDialog *pcm_file_dialog,
						 gint response,
						 AgsExportSoundcard *export_soundcard);

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
ags_export_soundcard_open_response_callback(AgsPCMFileDialog *pcm_file_dialog,
					    gint response,
					    AgsExportSoundcard *export_soundcard)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;

    gchar *filename;

    gint strv_length;

    file_widget = ags_pcm_file_dialog_get_file_widget(pcm_file_dialog);

    filename = ags_file_widget_get_filename(pcm_file_dialog->file_widget);

    if(!g_strv_contains(file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }

    ags_export_soundcard_set_filename(export_soundcard,
				      filename);

    g_free(filename);
  }
  
  gtk_window_destroy((GtkWindow *) pcm_file_dialog);
}

void
ags_export_soundcard_file_chooser_button_callback(GtkWidget *file_chooser_button,
						  AgsExportSoundcard *export_soundcard)
{
  AgsPCMFileDialog *pcm_file_dialog;
  AgsFileWidget *file_widget;
  
  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  
  /* get application context */  
  application_context = ags_application_context_get_instance();
  
  pcm_file_dialog = ags_pcm_file_dialog_new((GtkWindow *) ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context)),
					    i18n("open audio files"));
  ags_pcm_file_dialog_unset_flags(pcm_file_dialog,
				  (AGS_PCM_FILE_DIALOG_SHOW_AUDIO_CHANNEL));

  file_widget = ags_pcm_file_dialog_get_file_widget(pcm_file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/%s",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#else
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      home_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

  /* recently-used */
  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

  /* bookmark */
  ags_file_widget_set_bookmark_filename(file_widget,
					bookmark_filename);

  ags_file_widget_read_bookmark(file_widget);

#if defined(AGS_MACOS_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#else
  ags_file_widget_set_current_path(file_widget,
				   home_path);
#endif

  ags_file_widget_refresh(file_widget);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);

  gtk_widget_set_visible((GtkWidget *) pcm_file_dialog,
			 TRUE);

  g_signal_connect((GObject *) pcm_file_dialog, "response",
		   G_CALLBACK(ags_export_soundcard_open_response_callback), (gpointer) export_soundcard);
}
