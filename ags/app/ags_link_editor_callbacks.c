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

#include <ags/app/ags_link_editor_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_line.h>

#include <ags/i18n.h>

int ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor);

#define AGS_LINK_EDITOR_OPEN_SPIN_BUTTON "AgsLinkEditorOpenSpinButton"

void
ags_link_editor_combo_callback(GtkComboBox *combo, AgsLinkEditor *link_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(link_editor->combo,
				   &iter)){
    AgsMachine *machine, *link_machine;
    AgsMachineEditorLine *machine_editor_line;

    AgsAudio *audio;
    AgsChannel *channel;

    GtkTreeModel *model;

    machine_editor_line = AGS_MACHINE_EDITOR_LINE(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
									  AGS_TYPE_MACHINE_EDITOR_LINE));

    channel = machine_editor_line->channel;

    g_object_get(channel,
		 "audio", &audio,
		 NULL);

    g_object_unref(audio);

    //FIXME:JK: don't access AgsAudio to obtain widget
    machine = AGS_MACHINE(audio->machine_widget);
    
    model = gtk_combo_box_get_model(link_editor->combo);
    
    if(!((AGS_MACHINE_TAKES_FILE_INPUT & (machine->flags)) != 0 &&
	 ((AGS_MACHINE_ACCEPT_WAV & (machine->file_input_flags)) != 0 ||
	  ((AGS_MACHINE_ACCEPT_OGG & (machine->file_input_flags)) != 0)) &&
	 AGS_IS_INPUT(channel) &&
	 gtk_combo_box_get_active(link_editor->combo) + 1 == gtk_tree_model_iter_n_children(model,
											    NULL))){
      gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
			       TRUE);

      /* set machine link */
      gtk_tree_model_get(model,
			 &iter,
			 1, &link_machine,
			 -1);
      
      if(link_machine == NULL){
	gtk_spin_button_set_value(link_editor->spin_button, 0.0);
      }else{
	if(AGS_IS_INPUT(channel)){
	  gtk_spin_button_set_range(link_editor->spin_button, 0.0, (gdouble) (link_machine->audio->output_lines - 1));
	}else{
	  gtk_spin_button_set_range(link_editor->spin_button, 0.0, (gdouble) (link_machine->audio->input_lines - 1));
	}
      }
    }else{
      gchar *str, *tmp;
      
      /* set file link */
      if(link_editor->pcm_file_chooser_dialog != NULL || (AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER & (link_editor->flags)) != 0){
	return;
      }

      gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
			       FALSE);

      link_editor->pcm_file_chooser_dialog = (GtkFileChooserDialog *) ags_pcm_file_chooser_dialog_new(i18n("Select audio file"),
												      (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) link_editor,
																	    GTK_TYPE_WINDOW));
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(link_editor->pcm_file_chooser_dialog->file_chooser),
					   FALSE);

      /*  */
      str = NULL;
      
      gtk_tree_model_get(model,
			 &iter,
			 0, &str,
			 -1);

      if(str != NULL){
	tmp = g_strdup(str + 7);

	if(!g_strcmp0(tmp, "") == FALSE){
	  GFile *file;

	  GError *error;

	  file = g_file_new_for_path(tmp);
	
	  error = NULL;
	  gtk_file_chooser_set_file(GTK_FILE_CHOOSER(link_editor->pcm_file_chooser_dialog->file_chooser),
				    file,
				    &error);

	  if(error != NULL){
	    g_message("%s", error->message);
	  
	    g_error_free(error);
	  }
	}
      }
      
      g_signal_connect((GObject *) link_editor->pcm_file_chooser_dialog, "response",
		       G_CALLBACK(ags_link_editor_file_chooser_response_callback), (gpointer) link_editor);

      gtk_widget_show((GtkWidget *) link_editor->pcm_file_chooser_dialog);
    }
  }
}

int
ags_link_editor_option_changed_callback(GtkWidget *widget, AgsLinkEditor *link_editor)
{
  /*
    AgsMachineEditorLine *machine_editor_line;
    AgsMachine *machine;

    machine = (AgsMachine *) g_object_get_data((GObject *) link_editor->option->menu_item, g_type_name(AGS_TYPE_MACHINE));

    if(machine == NULL)
    return;

    machine_editor_line = (AgsMachineEditorLine *) gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_MACHINE_EDITOR_LINE);
    link_editor->spin_button->adjustment->upper = (gdouble) (AGS_IS_OUTPUT(machine_editor_line->channel) ? machine->audio->input_lines - 1: machine->audio->output_lines - 1);
  */

  return(0);
}

int
ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  pcm_file_chooser_dialog = AGS_PCM_FILE_CHOOSER_DIALOG(widget);

  if(response == GTK_RESPONSE_ACCEPT){
    GtkTreeModel *model;

    GFile *file;
    
    GtkTreeIter iter;

    char *filename;

    /* set filename in combo box */
    model = gtk_combo_box_get_model(link_editor->combo);
    
    file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser));

    filename = g_file_get_path(file);
    
    gtk_tree_model_iter_nth_child(model,
				  &iter,
				  NULL,
				  gtk_tree_model_iter_n_children(model,
								 NULL) - 1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       0, g_strdup_printf("file://%s", filename),
		       -1);

    /* set audio channel */
    gtk_spin_button_set_value(link_editor->spin_button,
			      gtk_spin_button_get_value(pcm_file_chooser_dialog->audio_channel));
  }

  link_editor->pcm_file_chooser_dialog = NULL;
  
  gtk_window_destroy((GtkWindow *) pcm_file_chooser_dialog);

  return(0);
}
