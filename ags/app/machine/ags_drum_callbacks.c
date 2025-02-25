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

#include <ags/app/machine/ags_drum_callbacks.h>
#include <ags/app/machine/ags_drum_input_pad.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_navigation.h>

#include <stdlib.h>
#include <math.h>

#include <ags/ags_api_config.h>

#include <ags/i18n.h>

#define AGS_AUDIO_FILE_DEVOUT "AgsAudioFileDevout"
#define AGS_DRUM_PLAY_RECALL "AgsDrumPlayRecall"

void ags_drum_open_response_callback(AgsPCMFileDialog *pcm_file_dialog, gint response,
				     AgsDrum *drum);

void
ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsPCMFileDialog *pcm_file_dialog;
  AgsFileWidget *file_widget;
  GtkLinkButton *link_button;
  
  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *current_path;
  gchar *str;

  gchar *drumkits_bookmark_filename;
  
  if(drum->open_dialog != NULL){
    return;
  }

#if defined(AGS_MACOS_SANDBOX)
  drumkits_bookmark_filename = NULL;
#endif
  
#if defined(AGS_FLATPAK_SANDBOX)
  drumkits_bookmark_filename = g_strdup("/usr/share/hydrogen/data/drumkits");
#endif

#if defined(AGS_SNAP_SANDBOX)
  drumkits_bookmark_filename = g_strdup_printf("%s/usr/share/hydrogen/data/drumkits",
					       getenv("AGS_SNAP_PATH"));
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  drumkits_bookmark_filename = g_strdup("/usr/share/hydrogen/data/drumkits");
#endif
  
  /* get application context */  
  application_context = ags_application_context_get_instance();
  
  pcm_file_dialog = ags_pcm_file_dialog_new((GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)),
					    i18n("open audio files"));

  drum->open_dialog = pcm_file_dialog;

#if defined(AGS_MACOS_SANDBOX)
  ags_pcm_file_dialog_set_flags(pcm_file_dialog,
				AGS_PCM_FILE_DIALOG_SHOW_DOWNLOAD_LINK);

  link_button = ags_pcm_file_dialog_get_download_link(pcm_file_dialog);
 
  gtk_link_button_set_uri(link_button,
			  "https://gsequencer.com/samples.html");
  gtk_button_set_label((GtkButton *) link_button,
		       i18n("download samples"));
#endif
  
  file_widget = ags_pcm_file_dialog_get_file_widget(pcm_file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/Containers/%s/Data",
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

  /* multi-selection */
  ags_file_widget_set_flags(file_widget,
  			    AGS_FILE_WIDGET_WITH_MULTI_SELECTION);

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

  ags_file_widget_refresh(file_widget);

  g_free(current_path);

#if !defined(AGS_MACOS_SANDBOX)
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
  
  if(g_file_test(drumkits_bookmark_filename,
		 (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))){
    ags_file_widget_add_bookmark(file_widget,
				 drumkits_bookmark_filename);
  }
  
  ags_pcm_file_dialog_unset_flags(pcm_file_dialog,
				  AGS_PCM_FILE_DIALOG_SHOW_AUDIO_CHANNEL);
  ags_pcm_file_dialog_set_flags(pcm_file_dialog,
				(AGS_PCM_FILE_DIALOG_SHOW_NEW_CHANNEL |
				 AGS_PCM_FILE_DIALOG_SHOW_EXISTING_CHANNEL));
  
  gtk_widget_set_visible(GTK_WIDGET(pcm_file_dialog),
			 TRUE);

  g_signal_connect(G_OBJECT(pcm_file_dialog), "response",
		   G_CALLBACK(ags_drum_open_response_callback), drum);

  g_signal_connect(G_OBJECT(pcm_file_dialog), "response",
		   G_CALLBACK(ags_machine_open_response_callback), drum);

  g_free(drumkits_bookmark_filename);
}

void
ags_drum_open_response_callback(AgsPCMFileDialog *pcm_file_dialog, gint response,
				AgsDrum *drum)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;
    
    GSList *start_filename, *filename;

    gint strv_length;

    file_widget = ags_pcm_file_dialog_get_file_widget(pcm_file_dialog);

    filename =
      start_filename = ags_file_widget_get_filenames(file_widget);

    while(filename != NULL){
      if(!g_strv_contains((const gchar * const *) file_widget->recently_used, filename->data)){
	strv_length = g_strv_length(file_widget->recently_used);

	file_widget->recently_used = g_realloc(file_widget->recently_used,
					       (strv_length + 2) * sizeof(gchar *));

	file_widget->recently_used[strv_length] = g_strdup(filename->data);
	file_widget->recently_used[strv_length + 1] = NULL; 
    
	ags_file_widget_write_recently_used(file_widget);
      }

      filename = filename->next;
    }
  }

  drum->open_dialog = NULL;
}

void
ags_drum_loop_button_callback(GObject *gobject,
			      GParamSpec *pspec,
			      AgsDrum *drum)
{
  GList *start_list, *list;

  gboolean loop;

  loop = gtk_check_button_get_active(GTK_CHECK_BUTTON(gobject));

  /* play - ags-fx-pattern */
  g_object_get(AGS_MACHINE(drum)->audio,
	       "play", &start_list,
	       NULL);
  
  list = start_list;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
    AgsFxPatternAudio *fx_pattern_audio;
    AgsPort *port;
    
    fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

    port = NULL;
    
    g_object_get(fx_pattern_audio,
		 "loop", &port,
		 NULL);

    if(port != NULL){
      GValue value = {0,};
      
      g_value_init(&value,
		   G_TYPE_BOOLEAN);

      g_value_set_boolean(&value,
			  loop);
      
      ags_port_safe_write(port,
			  &value);
      
      g_value_unset(&value);
      
      g_object_unref(port);
    }
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* recall - ags-fx-pattern */
  g_object_get(AGS_MACHINE(drum)->audio,
	       "recall", &start_list,
	       NULL);
  
  list = start_list;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
    AgsFxPatternAudio *fx_pattern_audio;
    AgsPort *port;
    
    fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

    port = NULL;
    
    g_object_get(fx_pattern_audio,
		 "loop", &port,
		 NULL);

    if(port != NULL){
      GValue value = {0,};
      
      g_value_init(&value,
		   G_TYPE_BOOLEAN);

      g_value_set_boolean(&value,
			  loop);
      
      ags_port_safe_write(port,
			  &value);
      
      g_value_unset(&value);
      
      g_object_unref(port);
    }
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  AgsApplySequencerLength *apply_sequencer_length;

  AgsApplicationContext *application_context;
  
  gdouble length;

  /* get application context */  
  application_context = ags_application_context_get_instance();

  /* task - apply length */
  length = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button));

  apply_sequencer_length = ags_apply_sequencer_length_new((GObject *) AGS_MACHINE(drum)->audio,
							  length);

  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) apply_sequencer_length);
}

void
ags_drum_index0_callback(GObject *gobject,
			 GParamSpec *pspec,
			 AgsDrum *drum)
{
  if(drum->selected0 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(gobject) != drum->selected0){
      GList *start_list, *list;

      gchar *str;

      guint64 bank_index_0;

      toggle_button = drum->selected0;
      drum->selected0 = NULL;

      gtk_toggle_button_set_active(toggle_button,
				   FALSE);

      drum->selected0 = (GtkToggleButton *) gobject;

      /* calculate index 0 */
      str = gtk_button_get_label((GtkButton *) drum->selected0);
      bank_index_0 =
	AGS_MACHINE(drum)->bank_0 = ((guint) str[0] - 'a');

      /* play - ags-fx-pattern */
      g_object_get(AGS_MACHINE(drum)->audio,
		   "play", &start_list,
		   NULL);
  
      list = start_list;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
	AgsFxPatternAudio *fx_pattern_audio;
	AgsPort *port;
    
	fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

	port = NULL;
    
	g_object_get(fx_pattern_audio,
		     "bank-index-0", &port,
		     NULL);

	if(port != NULL){
	  GValue value = {0,};
      
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			    (gfloat) bank_index_0);
      
	  ags_port_safe_write(port,
			      &value);
      
	  g_value_unset(&value);
      
	  g_object_unref(port);
	}
    
	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);

      /* recall - ags-fx-pattern */
      g_object_get(AGS_MACHINE(drum)->audio,
		   "recall", &start_list,
		   NULL);
  
      list = start_list;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
	AgsFxPatternAudio *fx_pattern_audio;
	AgsPort *port;
    
	fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

	port = NULL;
    
	g_object_get(fx_pattern_audio,
		     "bank-index-0", &port,
		     NULL);

	if(port != NULL){
	  GValue value = {0,};
      
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			    (gfloat) bank_index_0);
      
	  ags_port_safe_write(port,
			      &value);
      
	  g_value_unset(&value);
      
	  g_object_unref(port);
	}
    
	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);
    }else if(! gtk_toggle_button_get_active(drum->selected0)){
      drum->selected0 = NULL;

      gtk_toggle_button_set_active((GtkToggleButton *) gobject,
				   TRUE);

      drum->selected0 = (GtkToggleButton *) gobject;
    }

    ags_pattern_box_set_pattern(drum->pattern_box);
  }
}

void
ags_drum_index1_callback(GObject *gobject,
			 GParamSpec *pspec,
			 AgsDrum *drum)
{
  if(drum->selected1 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(gobject) != drum->selected1){
      GList *start_list, *list;

      gchar *str;

      guint64 bank_index_1;

      toggle_button = drum->selected1;
      drum->selected1 = NULL;

      gtk_toggle_button_set_active(toggle_button,
				   FALSE);

      drum->selected1 = (GtkToggleButton *) gobject;

      /* calculate index 1 */
      str = gtk_button_get_label((GtkButton *) drum->selected1);
      bank_index_1 =
	AGS_MACHINE(drum)->bank_1 = ((guint) g_ascii_strtoull(str, NULL, 10)) - 1;

      /* play - ags-fx-pattern */
      g_object_get(AGS_MACHINE(drum)->audio,
		   "play", &start_list,
		   NULL);
  
      list = start_list;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
	AgsFxPatternAudio *fx_pattern_audio;
	AgsPort *port;
    
	fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

	port = NULL;
    
	g_object_get(fx_pattern_audio,
		     "bank-index-1", &port,
		     NULL);

	if(port != NULL){
	  GValue value = {0,};
      
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			    (gfloat) bank_index_1);
      
	  ags_port_safe_write(port,
			      &value);
      
	  g_value_unset(&value);
      
	  g_object_unref(port);
	}
    
	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);

      /* recall - ags-fx-pattern */
      g_object_get(AGS_MACHINE(drum)->audio,
		   "recall", &start_list,
		   NULL);
  
      list = start_list;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
	AgsFxPatternAudio *fx_pattern_audio;
	AgsPort *port;
    
	fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

	port = NULL;
    
	g_object_get(fx_pattern_audio,
		     "bank-index-1", &port,
		     NULL);

	if(port != NULL){
	  GValue value = {0,};
      
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			    (gfloat) bank_index_1);
      
	  ags_port_safe_write(port,
			      &value);
      
	  g_value_unset(&value);
      
	  g_object_unref(port);
	}
    
	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);
    }else if(!gtk_toggle_button_get_active(drum->selected1)){
      drum->selected1 = NULL;

      gtk_toggle_button_set_active((GtkToggleButton *) gobject,
				   TRUE);

      drum->selected1 = (GtkToggleButton *) gobject;
    }

    ags_pattern_box_set_pattern(drum->pattern_box);
  }
}

void
ags_drum_stop_callback(AgsDrum *drum,
		       GList *recall_id, gint sound_scope,
		       gpointer data)
{
  if(sound_scope != AGS_SOUND_SCOPE_SEQUENCER){
    return;
  }
  
  /* all done */
  ags_led_array_unset_all((AgsLedArray *) drum->pattern_box->hled_array);
}
