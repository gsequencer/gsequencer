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
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

void ags_audiorec_open_response_callback(GtkWidget *widget, gint response,
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
  GtkFileChooserDialog *dialog;

  if(audiorec->open_dialog != NULL){
    return;
  }
  
  audiorec->open_dialog = 
    dialog = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Open audio files"),
								  (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) audiorec,
													AGS_TYPE_WINDOW),
								  GTK_FILE_CHOOSER_ACTION_OPEN,
								  i18n("_OK"), GTK_RESPONSE_ACCEPT,
								  i18n("_Cancel"), GTK_RESPONSE_CANCEL,
								  NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),
				       FALSE);
  gtk_widget_set_visible((GtkWidget *) dialog,
			 TRUE);
  
  gtk_widget_set_size_request(GTK_WIDGET(dialog),
			      AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_audiorec_open_response_callback), audiorec);
}

void
ags_audiorec_open_response_callback(GtkWidget *widget, gint response,
				    AgsAudiorec *audiorec)
{
  if(response == GTK_RESPONSE_ACCEPT){
    GFile *file;
    
    gchar *filename;

    file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(widget));
    
    filename = g_file_get_path(file);

    gtk_editable_set_text(GTK_EDITABLE(audiorec->filename),
			  filename);
    ags_audiorec_open_filename(audiorec,
			       filename);

    g_object_unref(file);
    
    g_free(filename);
  }

  audiorec->open_dialog = NULL;
  
  gtk_window_destroy((GtkWindow *) widget);
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
