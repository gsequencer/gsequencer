/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/machine/ags_sf2_synth_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

#include <complex.h>
#include <math.h>

void ags_sf2_synth_open_dialog_response_callback(AgsFileDialog *file_dialog, gint response,
						 AgsMachine *machine);

void
ags_sf2_synth_destroy_callback(GtkWidget *widget, AgsSF2Synth *sf2_synth)
{
  if(sf2_synth->open_dialog != NULL){
    gtk_window_destroy((GtkWindow *) sf2_synth->open_dialog);
  }
}

void
ags_sf2_synth_open_clicked_callback(GtkWidget *widget, AgsSF2Synth *sf2_synth)
{
  AgsWindow *window;
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;
  GtkLinkButton *link_button;
  
  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *current_path;
  gchar *str;

  gchar *sf2_bookmark_filename;

  sf2_bookmark_filename = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sf2_bookmark_filename = NULL;
#endif
  
#if defined(AGS_FLATPAK_SANDBOX)
  sf2_bookmark_filename = g_strdup("/usr/share/sounds/sf2");
#endif

#if defined(AGS_SNAP_SANDBOX)
  sf2_bookmark_filename = g_strdup_printf("%s/usr/share/sounds/sf2",
					  getenv("AGS_SNAP_PATH"));
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  sf2_bookmark_filename = g_strdup("/usr/share/sounds/sf2");
#endif
  
  /* get application context */  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  file_dialog = (AgsFileDialog *) ags_file_dialog_new((GtkWidget *) window,
						      i18n("open Soundfont2 file"));
  
  sf2_synth->open_dialog = (GtkWidget *) file_dialog;

  ags_file_dialog_set_flags(file_dialog,
			    AGS_FILE_DIALOG_SHOW_DOWNLOAD_LINK);

  link_button = ags_file_dialog_get_download_link(file_dialog);
 
  gtk_link_button_set_uri(link_button,
			  "https://gsequencer.com/samples.html");
  gtk_button_set_label((GtkButton *) link_button,
		       i18n("download samples"));
  
  file_widget = ags_file_dialog_get_file_widget(file_dialog);
  
  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/Containers/%s/Data",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
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

  /* current path */
  current_path = NULL;
    
#if defined(AGS_MACOS_SANDBOX)
  current_path = g_strdup_printf("%s/Music",
				 home_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  current_path = g_strdup(home_path);
#endif

  ags_file_widget_set_current_path(file_widget,
				   current_path);

  g_free(current_path);

  ags_file_widget_refresh(file_widget);

#if defined(AGS_MACOS_SANDBOX)
  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  
#endif

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

#if !defined(AGS_MACOS_SANDBOX)
  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);
#endif
  
  if(g_file_test(sf2_bookmark_filename,
		 (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))){
    ags_file_widget_add_bookmark(file_widget,
				 sf2_bookmark_filename);
  }

  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);

  g_signal_connect((GObject *) file_dialog, "response",
		   G_CALLBACK(ags_sf2_synth_open_dialog_response_callback), AGS_MACHINE(sf2_synth));

  g_free(sf2_bookmark_filename);
}

void
ags_sf2_synth_open_dialog_response_callback(AgsFileDialog *file_dialog, gint response,
					    AgsMachine *machine)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = AGS_SF2_SYNTH(machine);

  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;
    
    gchar *filename;

    gint strv_length;

    file_widget = ags_file_dialog_get_file_widget(file_dialog);
    
    filename = ags_file_widget_get_filename(file_widget);
    
    if(!g_strv_contains((const gchar * const *) file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }

    gtk_editable_set_text(GTK_EDITABLE(sf2_synth->filename),
			  filename);

    ags_sf2_synth_open_filename(sf2_synth,
				filename);
  }

  sf2_synth->open_dialog = NULL;

  gtk_window_destroy((GtkWindow *) file_dialog);
}

void
ags_sf2_synth_synth_pitch_type_callback(GObject *gobject,
					GParamSpec *pspec,
					AgsSF2Synth *sf2_synth)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;
    
  guint selected;

  GValue value = G_VALUE_INIT;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }
  
  selected = gtk_drop_down_get_selected((GtkDropDown *) gobject);

  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value,
		    (gfloat) selected);

  /* play */
  fx_sf2_synth_audio = (AgsFxSF2SynthAudio *) ags_recall_container_get_recall_audio(sf2_synth->sf2_synth_play_container);

  ags_port_safe_write(fx_sf2_synth_audio->synth_pitch_type,
		      &value);

  /* recall */
  fx_sf2_synth_audio = (AgsFxSF2SynthAudio *) ags_recall_container_get_recall_audio(sf2_synth->sf2_synth_recall_container);
  
  ags_port_safe_write(fx_sf2_synth_audio->synth_pitch_type,
		      &value);
}

void
ags_sf2_synth_bank_tree_view_callback(GtkTreeView *tree_view,
				      GtkTreePath *path,
				      GtkTreeViewColumn *column,
				      AgsSF2Synth *sf2_synth)
{
  GtkTreeModel *bank_model;
  GtkTreeIter iter;

  gint bank;
  
  bank_model = gtk_tree_view_get_model(sf2_synth->bank_tree_view);

  if(gtk_tree_model_get_iter(bank_model, &iter, path)){
    gtk_tree_model_get(bank_model,
		       &iter,
		       0, &bank,
		       -1);

    ags_sf2_synth_load_bank(sf2_synth,
			    bank);
  }  
}

void
ags_sf2_synth_program_tree_view_callback(GtkTreeView *tree_view,
					 GtkTreePath *path,
					 GtkTreeViewColumn *column,
					 AgsSF2Synth *sf2_synth)
{
  GtkTreeModel *program_model;
  GtkTreeIter iter;

  gint bank;
  gint program;
  
  program_model = gtk_tree_view_get_model(sf2_synth->program_tree_view);

  bank = -1;
  program = -1;
  
  if(gtk_tree_model_get_iter(program_model, &iter, path)){
    gtk_tree_model_get(program_model,
		       &iter,
		       0, &bank,
		       1, &program,
		       -1);

    ags_sf2_synth_load_midi_locale(sf2_synth,
				   bank,
				   program);
    
  }
}

void
ags_sf2_synth_synth_octave_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble octave;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }
  
  audio = AGS_MACHINE(sf2_synth)->audio;

  octave = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-octave", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) octave);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_synth_key_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble key;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  key = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-key", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) key);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_synth_volume_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "synth-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_chorus_enabled_callback(GtkButton *button, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_chorus_input_volume_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_input_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_input_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-input-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_input_volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_chorus_output_volume_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_output_volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_output_volume = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-output-volume", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_output_volume);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_chorus_lfo_oscillator_callback(GtkComboBox *combo_box, AgsSF2Synth *sf2_synth)
{
  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  //TODO:JK: implement me
}

void
ags_sf2_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_lfo_frequency;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_lfo_frequency = gtk_spin_button_get_value(spin_button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-lfo-frequency", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_lfo_frequency);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}  

void
ags_sf2_synth_chorus_depth_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_chorus_mix_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_mix;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_mix = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-mix", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_mix);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_chorus_delay_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble chorus_delay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  chorus_delay = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "chorus-delay", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) chorus_delay);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_volume_callback(GtkRange *range, AgsSF2Synth *sf2_synth)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(sf2_synth)->audio,
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

void
ags_sf2_synth_tremolo_enabled_callback(GtkCheckButton *button, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  tremolo_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_tremolo_gain_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  tremolo_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_tremolo_lfo_depth_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  tremolo_lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_lfo_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_tremolo_lfo_freq_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  tremolo_lfo_freq = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-lfo-freq", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_lfo_freq);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_tremolo_tuning_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble tremolo_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  tremolo_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TREMOLO_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "tremolo-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) tremolo_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_vibrato_enabled_callback(GtkCheckButton *button, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  vibrato_enabled = gtk_check_button_get_active((GtkCheckButton *) button);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-enabled", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_enabled);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_vibrato_gain_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_gain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  vibrato_gain = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-gain", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_gain);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_vibrato_lfo_depth_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  vibrato_lfo_depth = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-lfo-depth", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_lfo_depth);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_vibrato_lfo_freq_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  vibrato_lfo_freq = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-lfo-freq", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_lfo_freq);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_vibrato_tuning_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  
  GList *start_play, *start_recall, *recall;

  gdouble vibrato_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  vibrato_tuning = ags_dial_get_value(dial);
  
  start_play = ags_audio_get_play(audio);
  start_recall = ags_audio_get_recall(audio);
    
  recall =
    start_recall = g_list_concat(start_play, start_recall);

  while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_SF2_SYNTH_AUDIO)) != NULL){
    AgsPort *port;

    port = NULL;
      
    g_object_get(recall->data,
		 "vibrato-tuning", &port,
		 NULL);

    if(port != NULL){
      GValue value = G_VALUE_INIT;

      g_value_init(&value,
		   G_TYPE_FLOAT);

      g_value_set_float(&value,
			(gfloat) vibrato_tuning);

      ags_port_safe_write(port,
			  &value);

      g_object_unref(port);
    }
    
    /* iterate */
    recall = recall->next;
  }

  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

void
ags_sf2_synth_wah_wah_enabled_callback(GtkCheckButton *button, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_enabled;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_enabled = (gdouble) gtk_check_button_get_active((GtkCheckButton *) button);

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-enabled", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_enabled);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_sf2_synth_wah_wah_length_callback(GtkComboBox *combo_box, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_length;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_length = gtk_combo_box_get_active(sf2_synth->wah_wah_length);

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-length-mode", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_length);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_sf2_synth_wah_wah_attack_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_attack;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_attack = ags_dial_get_value(sf2_synth->wah_wah_attack_x) + ags_dial_get_value(sf2_synth->wah_wah_attack_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-attack", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_attack);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw((GtkWidget *) sf2_synth->wah_wah_drawing_area);
}

void
ags_sf2_synth_wah_wah_decay_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_decay;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_decay = ags_dial_get_value(sf2_synth->wah_wah_decay_x) + ags_dial_get_value(sf2_synth->wah_wah_decay_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-decay", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_decay);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw((GtkWidget *) sf2_synth->wah_wah_drawing_area);
}

void
ags_sf2_synth_wah_wah_sustain_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_sustain;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_sustain = ags_dial_get_value(sf2_synth->wah_wah_sustain_x) + ags_dial_get_value(sf2_synth->wah_wah_sustain_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-sustain", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_sustain);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw((GtkWidget *) sf2_synth->wah_wah_drawing_area);
}

void
ags_sf2_synth_wah_wah_release_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_release;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_release = ags_dial_get_value(sf2_synth->wah_wah_release_x) + ags_dial_get_value(sf2_synth->wah_wah_release_y) * I;

  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
  
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-release", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_release);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  gtk_widget_queue_draw((GtkWidget *) sf2_synth->wah_wah_drawing_area);
}

void
ags_sf2_synth_wah_wah_ratio_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  double _Complex wah_wah_ratio;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_ratio = ags_dial_get_value(sf2_synth->wah_wah_ratio);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-ratio", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_ratio);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  gtk_widget_queue_draw((GtkWidget *) sf2_synth->wah_wah_drawing_area);
}

void
ags_sf2_synth_wah_wah_lfo_depth_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_lfo_depth;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_lfo_depth = ags_dial_get_value(dial);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-lfo-depth", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_lfo_depth);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_sf2_synth_wah_wah_lfo_freq_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_lfo_freq;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_lfo_freq = ags_dial_get_value(dial);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-lfo-freq", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_lfo_freq);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}

void
ags_sf2_synth_wah_wah_tuning_callback(AgsDial *dial, AgsSF2Synth *sf2_synth)
{
  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  
  GList *start_play, *start_recall, *recall;

  gdouble wah_wah_tuning;

  if((AGS_MACHINE_NO_UPDATE & (AGS_MACHINE(sf2_synth)->flags)) != 0){
    return;
  }

  audio = AGS_MACHINE(sf2_synth)->audio;

  wah_wah_tuning = ags_dial_get_value(dial);
  
  start_channel =
    channel = ags_audio_get_input(audio);

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_WAH_WAH_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "wah-wah-tuning", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  (gfloat) wah_wah_tuning);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
    
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);

    /**/
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
}
