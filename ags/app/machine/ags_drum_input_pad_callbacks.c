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

#include <ags/app/machine/ags_drum_input_pad_callbacks.h>
#include <ags/app/machine/ags_drum_input_line_callbacks.h>
#include <ags/app/machine/ags_drum.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_pcm_file_chooser_dialog.h>
#include <ags/app/ags_line_callbacks.h>

#include <math.h>

#include <ags/i18n.h>

void ags_drum_input_pad_open_response_callback(GtkWidget *widget, gint response, AgsDrumInputPad *pad);

#define AGS_DRUM_INPUT_PAD_OPEN_AUDIO_FILE_NAME "AgsDrumInputPadOpenAudioFileName"
#define AGS_DRUM_INPUT_PAD_OPEN_SPIN_BUTTON "AgsDrumInputPadOpenSpinButton"

void
ags_drum_input_pad_open_callback(GtkWidget *widget, AgsDrumInputPad *drum_input_pad)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  GFile *file;
  
  GError *error;

  if(drum_input_pad->file_chooser != NULL){
    return;
  }
  
  pcm_file_chooser_dialog = ags_pcm_file_chooser_dialog_new(i18n("open audio files"),
							    (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) drum_input_pad,
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

  drum_input_pad->file_chooser = pcm_file_chooser_dialog;
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser),
				       FALSE);

  if(gtk_toggle_button_get_active(AGS_PAD(drum_input_pad)->group)){
    gtk_widget_set_sensitive((GtkWidget *) pcm_file_chooser_dialog->audio_channel,
			     FALSE);
  }

  gtk_widget_show((GtkWidget *) pcm_file_chooser_dialog);

  g_signal_connect((GObject *) pcm_file_chooser_dialog, "response",
		   G_CALLBACK(ags_drum_input_pad_open_response_callback), (gpointer) drum_input_pad);
}

void
ags_drum_input_pad_open_response_callback(GtkWidget *widget, gint response, AgsDrumInputPad *drum_input_pad)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  AgsAudioFile *audio_file;

  AgsOpenSingleFile *open_single_file;

  AgsApplicationContext *application_context;

  GFile *file;
  
  GList *task;
  
  gchar *filename;

  application_context = ags_application_context_get_instance();

  pcm_file_chooser_dialog = drum_input_pad->file_chooser;

  if(response == GTK_RESPONSE_ACCEPT){
    file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser));

    /* task */
    task = NULL;
    
    if(gtk_toggle_button_get_active(AGS_PAD(drum_input_pad)->group)){
      AgsChannel *current, *next_pad, *next_current;

      guint i;

      current = AGS_PAD(drum_input_pad)->channel;

      if(current != NULL) {
	g_object_ref(current);
      }
      
      next_pad = ags_channel_next_pad(current);

      next_current = NULL;
      
      for(i = 0; current != next_pad; i++){
	open_single_file = ags_open_single_file_new(current,
						    filename,
						    i);
	task = g_list_prepend(task,
			      open_single_file);

	/* iterate */
	next_current = ags_channel_next(current);

	g_object_unref(current);

	current = next_current;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }

      if(next_current != NULL){
	g_object_unref(next_current);
      }
    }else{
      AgsLine *line;

      GList *start_list, *list;
      
      list =
	start_list = ags_pad_get_line(AGS_PAD(drum_input_pad));
      
      list = ags_line_find_next_grouped(start_list);

      if(list != NULL){
	line = list->data;
	
	open_single_file = ags_open_single_file_new(line->channel,
						    filename,
						    (guint) gtk_spin_button_get_value(pcm_file_chooser_dialog->audio_channel));
	task = g_list_prepend(task,
			      open_single_file);
      }
      
      g_list_free(start_list);
    }

    ags_ui_provider_schedule_task_all(AGS_UI_PROVIDER(application_context),
				      task);
  }

  gtk_window_destroy((GtkWindow *) pcm_file_chooser_dialog);

  drum_input_pad->file_chooser = NULL;
}

void
ags_drum_input_pad_play_callback(GtkToggleButton *toggle_button, AgsDrumInputPad *drum_input_pad)
{
  ags_pad_play(AGS_PAD(drum_input_pad));
}

void
ags_drum_input_pad_edit_callback(GtkWidget *toggle_button, AgsDrumInputPad *drum_input_pad)
{
  GtkToggleButton *toggle;
  AgsDrum *drum;

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_input_pad,
					     AGS_TYPE_DRUM);

  if(drum->selected_edit_button != NULL){
    if(GTK_TOGGLE_BUTTON(toggle_button) != drum->selected_edit_button){
      /* unset old */
      toggle = drum->selected_edit_button;

      drum->selected_edit_button = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) toggle, FALSE);

      /* apply new */
      drum->selected_edit_button = (GtkToggleButton *) toggle_button;
      drum->selected_pad = (AgsDrumInputPad *) gtk_widget_get_ancestor((GtkWidget *) toggle_button,
								       AGS_TYPE_DRUM_INPUT_PAD);

      AGS_MACHINE(drum)->selected_input_pad = (GtkWidget *) drum->selected_pad;
      
      ags_pattern_box_set_pattern(drum->pattern_box);
    }else{
      /* chain up */
      toggle = drum->selected_edit_button;
      
      drum->selected_edit_button = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) toggle, TRUE);

      /* reset */
      drum->selected_edit_button = toggle;
    }
  }
}
