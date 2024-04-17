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

#include <ags/app/machine/ags_pitch_sampler_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/i18n.h>

void ags_pitch_sampler_open_response_callback(AgsFileDialog *file_dialog, gint response,
					      AgsPitchSampler *pitch_sampler);

void
ags_pitch_sampler_open_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  AgsWindow *window;
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;

  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;

  gchar *sfz_bookmark_filename;

  sfz_bookmark_filename = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sfz_bookmark_filename = NULL;
#endif
  
#if defined(AGS_FLATPAK_SANDBOX)
  sfz_bookmark_filename = g_strdup("/usr/share/sounds/sfz");
#endif

#if defined(AGS_SNAP_SANDBOX)
  sfz_bookmark_filename = g_strdup_printf("%s/usr/share/sounds/sfz",
					  getenv("AGS_SNAP_PATH"));
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  sfz_bookmark_filename = g_strdup("/usr/share/sounds/sfz");
#endif

  /* get application context */  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  file_dialog = (AgsFileDialog *) ags_file_dialog_new((GtkWindow *) window,
						      i18n("open Soundfont2 file"));

  file_widget = ags_file_dialog_get_file_widget(file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/%s",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sfz_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sfz_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sfz_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sfz_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sfz_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sfz_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sfz_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sfz_bookmark.xml",
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

  if(g_file_test(sfz_bookmark_filename,
		 (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))){
    ags_file_widget_add_bookmark(file_widget,
				 sfz_bookmark_filename);
  }
  
  pitch_sampler->open_dialog = file_dialog;
  
  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);

  g_signal_connect((GObject *) file_dialog, "response",
		   G_CALLBACK(ags_pitch_sampler_open_response_callback), pitch_sampler);

  g_free(sfz_bookmark_filename);
}

void
ags_pitch_sampler_open_response_callback(AgsFileDialog *file_dialog, gint response,
					 AgsPitchSampler *pitch_sampler)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;
    
    gchar *filename;

    gint strv_length;

    file_widget = ags_file_dialog_get_file_widget(file_dialog);
    
    filename = ags_file_widget_get_filename(file_widget);

    if(!g_strv_contains(file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }
    
    gtk_editable_set_text(GTK_EDITABLE(pitch_sampler->filename),
			  filename);

    ags_pitch_sampler_open_filename(pitch_sampler,
				    filename);
  }

  pitch_sampler->open_dialog = NULL;

  gtk_window_destroy((GtkWindow *) file_dialog);
}

void
ags_pitch_sampler_file_control_changed_callback(AgsPitchSamplerFile *pitch_sampler_file,
						AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_update_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  ags_pitch_sampler_update(pitch_sampler);
}

void
ags_pitch_sampler_enable_lfo_callback(GtkToggleButton *toggle, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "enabled", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			(gtk_check_button_get_active(toggle) ? 1.0: 0.0));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "enabled", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			(gtk_check_button_get_active(toggle) ? 1.0: 0.0));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_freq_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-freq", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-freq", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_phase_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-phase", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-phase", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_depth_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-depth", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-depth", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_lfo_tuning_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  AgsAudio *audio;  
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;
  AgsPort *port;

  GList *start_play, *play;
  GList *start_recall, *recall;

  audio = AGS_MACHINE(pitch_sampler)->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  while(channel != NULL){  
    g_object_get(channel,
		 "play", &start_play,
		 "recall", &start_recall,
		 NULL);

    /* play */
    play = ags_recall_find_type(start_play,
				AGS_TYPE_FX_LFO_CHANNEL);
      
    if(play != NULL){
      GValue value = {0};
      
      g_object_get(play->data,
		   "lfo-tuning", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_play);

    /* recall */
    recall = ags_recall_find_type(start_recall,
				  AGS_TYPE_FX_LFO_CHANNEL);
      
    if(recall != NULL){
      GValue value = {0};
      
      g_object_get(recall->data,
		   "lfo-tuning", &port,
		   NULL);

      g_value_init(&value,
		   G_TYPE_FLOAT);
      g_value_set_float(&value,
			gtk_spin_button_get_value(spin_button));

      ags_port_safe_write(port,
			  &value);
	
      g_object_unref(port);
    }

    g_list_free(start_recall);

    /* iterate */
    next_channel = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next_channel;
  }
}

void
ags_pitch_sampler_enable_aliase_callback(GtkToggleButton *toggle, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat enabled;

  enabled = gtk_check_button_get_active(toggle) ? 1.0: 0.0;
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "enabled", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  enabled);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_aliase_a_amount_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat amount;

  amount = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "a-amount", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  amount);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_aliase_a_phase_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat phase;

  phase = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "a-phase", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  phase);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_aliase_b_amount_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat amount;

  amount = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "b-amount", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  amount);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_aliase_b_phase_callback(AgsDial *dial, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat phase;

  phase = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "b-phase", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  phase);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_pitch_sampler_volume_callback(GtkRange *range, AgsPitchSampler *pitch_sampler)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(pitch_sampler)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VOLUME_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "volume", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  volume);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
