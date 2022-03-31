/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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
#include <ags/app/ags_pcm_file_chooser_dialog.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_navigation.h>

#include <stdlib.h>
#include <math.h>

#include <ags/i18n.h>

#define AGS_AUDIO_FILE_DEVOUT "AgsAudioFileDevout"
#define AGS_DRUM_PLAY_RECALL "AgsDrumPlayRecall"

void ags_drum_open_response_callback(GtkDialog *dialog, gint response, AgsDrum *drum);

void
ags_drum_notify_parent_callback(GObject *gobject,
				GParamSpec *pspec,
				gpointer user_data)
{
  AgsWindow *window;

  gchar *str;

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) gobject,
					      AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_DRUM)->counter);

  g_object_set(gobject,
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_DRUM);
  g_free(str);
}

void
ags_drum_destroy_callback(GtkWidget *widget, AgsDrum *drum)
{
  GList *start_list, *list;

  if(drum->open_dialog != NULL){
    gtk_window_destroy(GTK_WINDOW(drum->open_dialog));
  }
  
  list =
    start_list = ags_machine_get_input_pad(AGS_MACHINE(drum));

  while(list != NULL){
    if(AGS_DRUM_INPUT_PAD(list->data)->file_chooser != NULL){
      gtk_window_destroy(GTK_WINDOW(AGS_DRUM_INPUT_PAD(list->data)->file_chooser));
    }

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  GFile *file;
  
  GError *error;
  
  if(drum->open_dialog != NULL){
    return;
  }
  
  pcm_file_chooser_dialog = ags_pcm_file_chooser_dialog_new(i18n("open audio files"),
							    (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) drum,
												  AGS_TYPE_WINDOW));

  file = g_file_new_for_path("/usr/share/hydrogen/data/drumkits");

  error = NULL;
  gtk_file_chooser_add_shortcut_folder(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser),
				       file,
				       &error);

  if(error != NULL){
    g_message("%s", error->message);
    
    g_error_free(error);
  }
  
  drum->open_dialog = (GtkWidget *) pcm_file_chooser_dialog;
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser),
				       TRUE);

  pcm_file_chooser_dialog->flags &= ~(AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_AUDIO_CHANNEL);
  pcm_file_chooser_dialog->flags |= (AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_NEW_CHANNEL |
				     AGS_PCM_FILE_CHOOSER_DIALOG_SHOW_EXISTING_CHANNEL);
  
  gtk_widget_show(GTK_WIDGET(pcm_file_chooser_dialog));

  g_signal_connect(G_OBJECT(pcm_file_chooser_dialog), "response",
		   G_CALLBACK(ags_drum_open_response_callback), drum);
  g_signal_connect(G_OBJECT(pcm_file_chooser_dialog), "response",
		   G_CALLBACK(ags_machine_open_response_callback), drum);
}

void
ags_drum_open_response_callback(GtkDialog *dialog, gint response, AgsDrum *drum)
{
  drum->open_dialog = NULL;
}

void
ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum)
{
  GList *start_list, *list;

  gboolean loop;

  loop = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

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
ags_drum_index0_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected0 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected0){
      GList *start_list, *list;

      gchar *str;

      guint64 bank_index_0;

      toggle_button = drum->selected0;
      drum->selected0 = NULL;

      gtk_toggle_button_set_active(toggle_button,
				   FALSE);

      drum->selected0 = (GtkToggleButton*) widget;

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

      gtk_toggle_button_set_active((GtkToggleButton *) widget,
				   TRUE);

      drum->selected0 = (GtkToggleButton*) widget;
    }

    ags_pattern_box_set_pattern(drum->pattern_box);
  }
}

void
ags_drum_index1_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected1 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected1){
      GList *start_list, *list;

      gchar *str;

      guint64 bank_index_1;

      toggle_button = drum->selected1;
      drum->selected1 = NULL;

      gtk_toggle_button_set_active(toggle_button,
				   FALSE);

      drum->selected1 = (GtkToggleButton*) widget;

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

      gtk_toggle_button_set_active((GtkToggleButton *) widget,
				   TRUE);

      drum->selected1 = (GtkToggleButton*) widget;
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
