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

#include <ags/app/machine/ags_audiorec_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_pcm_file_dialog.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

void ags_audiorec_open_response_callback(AgsPCMFileDialog *pcm_file_dialog, gint response,
					 AgsAudiorec *audiorec);

void
ags_audiorec_update_ui_callback(GObject *ui_provider,
				AgsAudiorec *audiorec)
{
  AgsAudio *audio;
  AgsChannel *start_channel;
  AgsChannel *channel, *next_channel;
    
  GList *start_list, *list;

  audio = AGS_MACHINE(audiorec)->audio;

  start_channel = ags_audio_get_input(audio);
    
  list =
    start_list = ags_audiorec_get_indicator(audiorec);
    
  /* check members */
  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }

  next_channel = NULL;
    
  while(list != NULL){
    GtkAdjustment *adjustment;
    GtkWidget *child;

    AgsPort *current;

    GList *start_port;
      
    gdouble average_peak;
    gdouble peak;
	
    GValue value = {0,};
	
    child = list->data;
      
    average_peak = 0.0;
      
    start_port = ags_channel_collect_all_channel_ports_by_specifier_and_context(channel,
										"./peak[0]",
										FALSE);

    current = NULL;

    if(start_port != NULL){
      current = start_port->data;
    }
      
    /* recall port - read value */
    g_value_init(&value, G_TYPE_FLOAT);
    ags_port_safe_read(current,
		       &value);
      
    peak = g_value_get_float(&value);
    g_value_unset(&value);

    /* calculate peak */
    average_peak += peak;
      
    /* apply */
    g_object_get(child,
		 "adjustment", &adjustment,
		 NULL);
	
    gtk_adjustment_set_value(adjustment,
			     10.0 * average_peak);

    /* queue draw */
    gtk_widget_queue_draw(child);

    g_list_free_full(start_port,
		     g_object_unref);
      
    /* iterate */
    list = list->next;

    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }
    
  g_list_free(start_list);
}

void
ags_audiorec_open_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPCMFileDialog *pcm_file_dialog;

  if(audiorec->open_dialog != NULL){
    return;
  }
  
  AgsFileWidget *file_widget;
  
  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *str;
  
  /* get application context */  
  application_context = ags_application_context_get_instance();
  
  pcm_file_dialog = ags_pcm_file_dialog_new((GtkWindow *) ags_ui_provider_get_export_window(AGS_UI_PROVIDER(application_context)),
					    i18n("open audio files"));

  audiorec->open_dialog = pcm_file_dialog;
  
  ags_pcm_file_dialog_unset_flags(pcm_file_dialog,
				  (AGS_PCM_FILE_DIALOG_SHOW_AUDIO_CHANNEL));

  file_widget = ags_pcm_file_dialog_get_file_widget(pcm_file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/%s/Data",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_pcm_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_pcm_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
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
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
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

  ags_file_widget_set_default_bundle(file_widget,
				     AGS_DEFAULT_BUNDLE_ID);
  
  gtk_widget_set_visible((GtkWidget *) pcm_file_dialog,
			 TRUE);

  g_signal_connect((GObject *) pcm_file_dialog, "response",
		   G_CALLBACK(ags_audiorec_open_response_callback), audiorec);
}

void
ags_audiorec_open_response_callback(AgsPCMFileDialog *pcm_file_dialog, gint response,
				    AgsAudiorec *audiorec)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;

    gchar *filename;

    gint strv_length;

    file_widget = ags_pcm_file_dialog_get_file_widget(pcm_file_dialog);

    filename = ags_file_widget_get_filename(file_widget);

    if(!g_strv_contains(file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }

    gtk_editable_set_text(GTK_EDITABLE(audiorec->filename),
			  filename);

    ags_audiorec_open_filename(audiorec,
			       filename);
  }

  audiorec->open_dialog = NULL;
  
  gtk_window_destroy((GtkWindow *) pcm_file_dialog);
}

void
ags_audiorec_keep_data_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPort *port;
  
  GList *start_recall, *recall;
  
  GValue capture_mode_value = {0,};

  if(gtk_check_button_get_active((GtkCheckButton *) button)){
    g_value_init(&capture_mode_value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&capture_mode_value,
		       AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_NONE);

    /* play context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "play", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "recall", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* unset value */
    g_value_unset(&capture_mode_value);
  }
}

void
ags_audiorec_replace_data_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPort *port;
  
  GList *start_recall, *recall;
  
  GValue capture_mode_value = {0,};

  if(gtk_check_button_get_active((GtkCheckButton *) button)){
    g_value_init(&capture_mode_value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&capture_mode_value,
		       AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE);

    /* play context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "play", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "recall", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* unset value */
    g_value_unset(&capture_mode_value);
  }
}

void
ags_audiorec_mix_data_callback(GtkWidget *button, AgsAudiorec *audiorec)
{
  AgsPort *port;
  
  GList *start_recall, *recall;
  
  GValue capture_mode_value = {0,};

  if(gtk_check_button_get_active((GtkCheckButton *) button)){
    g_value_init(&capture_mode_value,
		 G_TYPE_UINT64);

    g_value_set_uint64(&capture_mode_value,
		       AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_MIX);

    /* play context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "play", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* recall context */
    g_object_get(AGS_MACHINE(audiorec)->audio,
		 "recall", &start_recall,
		 NULL);

    recall = ags_recall_template_find_type(start_recall,
					   AGS_TYPE_FX_PLAYBACK_AUDIO);

    if(recall != NULL){
      g_object_get(recall->data,
		   "capture-mode", &port,
		   NULL);

      if(port != NULL){
	ags_port_safe_write(port,
			    &capture_mode_value);

	g_object_unref(port);
      }
    }

    g_list_free_full(start_recall,
		     g_object_unref);

    /* unset value */
    g_value_unset(&capture_mode_value);
  }
}
