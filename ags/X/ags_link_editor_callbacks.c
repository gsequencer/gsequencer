/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_link_editor_callbacks.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/file/ags_file_link.h>

#include <ags/audio/recall/ags_play_audio_file.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

#include <ags/i18n.h>

int ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor);
int ags_link_editor_file_chooser_play_callback(GtkToggleButton *toggle_button, AgsLinkEditor *link_editor);

void ags_link_editor_file_chooser_play_done(AgsRecall *recall, AgsLinkEditor *link_editor);
void ags_link_editor_file_chooser_play_cancel(AgsRecall *recall, AgsLinkEditor *link_editor);

#define AGS_LINK_EDITOR_OPEN_SPIN_BUTTON "AgsLinkEditorOpenSpinButton"

int
ags_link_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLinkEditor *link_editor)
{
  AgsMachine *machine;
  AgsLineEditor *line_editor;

  AgsAudio *audio;
  AgsChannel *channel;

  GtkTreeModel *model;

  if(old_parent != NULL){
    return(0);
  }

  //TODO:JK: missing mutex
  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_LINE_EDITOR);

  if(line_editor != NULL){
    channel = line_editor->channel;
  
    if(channel != NULL){
      GtkTreeIter iter;
      
      audio = AGS_AUDIO(channel->audio);

      if(audio != NULL){
	machine = AGS_MACHINE(audio->machine);
    
	model = GTK_TREE_MODEL(ags_machine_get_possible_links(machine));
  
	if(AGS_IS_INPUT(line_editor->channel) &&
	   (AGS_MACHINE_TAKES_FILE_INPUT & (machine->flags)) != 0 &&
	   ((AGS_MACHINE_ACCEPT_WAV & (machine->file_input_flags)) != 0 ||
	    ((AGS_MACHINE_ACCEPT_OGG & (machine->file_input_flags)) != 0))){
	  gtk_list_store_append(GTK_LIST_STORE(model), &iter);

	  if(AGS_INPUT(channel)->file_link != NULL){
	    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			       0, g_strdup_printf("file://%s", AGS_FILE_LINK(AGS_INPUT(channel)->file_link)->filename),
			       1, NULL,
			       -1);
	    gtk_combo_box_set_active_iter(link_editor->combo,
					  &iter);
	  }else{
	    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			       0, "file://",
			       1, NULL,
			       -1);
	  }

	}

	gtk_combo_box_set_model(link_editor->combo,
				model);
      }
    }
  }
  
  return(0);
}

void
ags_link_editor_combo_callback(GtkComboBox *combo, AgsLinkEditor *link_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(link_editor->combo,
				   &iter)){
    AgsMachine *machine, *link_machine;
    AgsLineEditor *line_editor;

    AgsChannel *channel;

    GtkTreeModel *model;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;
    machine = AGS_MACHINE(AGS_AUDIO(channel->audio)->machine);
    
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
      GtkHBox *hbox;
      GtkLabel *label;
      GtkSpinButton *spin_button;
      gchar *str, *tmp;
      
      /* set file link */
      if(link_editor->file_chooser != NULL || (AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER & (link_editor->flags)) != 0){
	return;
      }

      gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
				 FALSE);

      link_editor->file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("select audio file"),
										       (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) link_editor),
										       GTK_FILE_CHOOSER_ACTION_OPEN,
										       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
										       NULL);
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(link_editor->file_chooser),
					   FALSE);

      /* extra widget */
      hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				      0);
      gtk_file_chooser_set_extra_widget((GtkFileChooser *) link_editor->file_chooser,
					(GtkWidget *) hbox);

      label = (GtkLabel *) gtk_label_new(i18n("audio channel: "));
      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) label,
			 FALSE, FALSE,
			 0);

      spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
								     256.0,
								     1.0);
      g_object_set_data((GObject *) link_editor->file_chooser,
			AGS_LINK_EDITOR_OPEN_SPIN_BUTTON, spin_button);
      gtk_box_pack_start((GtkBox *) hbox,
			 (GtkWidget *) spin_button,
			 FALSE, FALSE,
			 0);

      /*  */
      gtk_tree_model_get(model,
			 &iter,
			 0, &str,
			 -1);
      
      tmp = g_strdup(str + 7);

      if(g_strcmp0(tmp, "")){
	char *tmp0, *name, *dir;

	tmp0 = g_strrstr(tmp, "/");
	name = g_strdup(tmp0 + 1);
	dir = g_strndup(tmp, tmp0 - tmp);

	gtk_file_chooser_set_current_folder((GtkFileChooser *) link_editor->file_chooser, dir);
	gtk_file_chooser_set_current_name((GtkFileChooser *) link_editor->file_chooser, name);
      }

      g_signal_connect((GObject *) link_editor->file_chooser, "response",
		       G_CALLBACK(ags_link_editor_file_chooser_response_callback), (gpointer) link_editor);

      gtk_widget_show_all((GtkWidget *) link_editor->file_chooser);
    }
  }
}

int
ags_link_editor_option_changed_callback(GtkWidget *widget, AgsLinkEditor *link_editor)
{
  /*
  AgsLineEditor *line_editor;
  AgsMachine *machine;

  machine = (AgsMachine *) g_object_get_data((GObject *) link_editor->option->menu_item, g_type_name(AGS_TYPE_MACHINE));

  if(machine == NULL)
    return;

  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_LINE_EDITOR);
  link_editor->spin_button->adjustment->upper = (gdouble) (AGS_IS_OUTPUT(line_editor->channel) ? machine->audio->input_lines - 1: machine->audio->output_lines - 1);
  */

  return(0);
}

int
ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor)
{
  GtkFileChooserDialog *file_chooser;

  char *name;

  file_chooser = link_editor->file_chooser;

  if(response == GTK_RESPONSE_ACCEPT){
    GtkSpinButton *spin_button;
    
    GtkTreeModel *model;
    GtkTreeIter iter;

    /* set filename in combo box */
    model = gtk_combo_box_get_model(link_editor->combo);
    
    name = gtk_file_chooser_get_filename((GtkFileChooser *) file_chooser);

    gtk_tree_model_iter_nth_child(model,
				  &iter,
				  NULL,
				  gtk_tree_model_iter_n_children(model,
								 NULL) - 1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		       0, g_strdup_printf("file://%s", name),
		       -1);

    /* set audio channel */
    spin_button = (GtkSpinButton *) g_object_get_data((GObject *) file_chooser, AGS_LINK_EDITOR_OPEN_SPIN_BUTTON);

    gtk_spin_button_set_value(link_editor->spin_button,
			      gtk_spin_button_get_value(spin_button));
  }

  link_editor->file_chooser = NULL;
  gtk_widget_destroy((GtkWidget *) file_chooser);

  return(0);
}

int
ags_link_editor_file_chooser_play_callback(GtkToggleButton *toggle_button, AgsLinkEditor *link_editor)
{
  /*
  GtkFileChooserDialog *file_chooser;
  AgsDevout *soundcard;
  AgsPlayAudioFile *play_audio_file;
  AgsAudioFile *audio_file;
  char *name;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  file_chooser = link_editor->file_chooser;
  audio_file = link_editor->audio_file;

  soundcard = AGS_DEVOUT(AGS_AUDIO(AGS_LINE_EDITOR(gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_LINE_EDITOR))->channel->audio)->soundcard);

  if(toggle_button->active){
    /* AgsPlayAudioFile * /
    play_audio_file = ags_play_audio_file_new();
    play_audio_file->soundcard = soundcard;
    ags_play_audio_file_connect(play_audio_file);

    g_object_set_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_PLAY_AUDIO_FILE), play_audio_file);

    g_signal_connect((GObject *) play_audio_file, "done",
		     G_CALLBACK(ags_link_editor_file_chooser_play_done), link_editor);
    g_signal_connect((GObject *) play_audio_file, "cancel",
		     G_CALLBACK(ags_link_editor_file_chooser_play_cancel), link_editor);

    /* AgsAudioFile * /
    name = gtk_file_chooser_get_filename((GtkFileChooser *) file_chooser);

    if(audio_file != NULL){
      if(g_strcmp0(audio_file->name, name)){
	g_object_unref(G_OBJECT(audio_file));
	goto ags_link_editor_file_chooser_play_callback0;
      }
    }else{
    ags_link_editor_file_chooser_play_callback0:
      audio_file = ags_audio_file_new();
      g_object_set_data((GObject *) link_editor->option->menu_item, (char *) g_type_name(AGS_TYPE_AUDIO_FILE), audio_file);

      audio_file->flags |= AGS_AUDIO_FILE_ALL_CHANNELS;
      audio_file->name = (gchar *) name;

      ags_audio_file_set_soundcard(audio_file, soundcard);

      ags_audio_file_open(audio_file);

      AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
    }

    play_audio_file->audio_file = audio_file;

    /* AgsDevout * /
    g_static_mutex_lock(&mutex);
    soundcard->play_recall = g_list_append(soundcard->play_recall, play_audio_file);
    soundcard->flags |= AGS_DEVOUT_PLAY_RECALL;
    soundcard->play_recall_ref++;
    AGS_DEVOUT_GET_CLASS(soundcard)->run((void *) soundcard);
    g_static_mutex_unlock(&mutex);
  }else{
    if((AGS_LINK_EDITOR_FILE_CHOOSER_PLAY_DONE & (link_editor->flags)) == 0){
      play_audio_file = (AgsPlayAudioFile *) g_object_get_data((GObject *) file_chooser, (char *) g_type_name(AGS_TYPE_PLAY_AUDIO_FILE));
      play_audio_file->recall.flags |= AGS_RECALL_CANCEL;
    }else
      link_editor->flags &= (~AGS_LINK_EDITOR_FILE_CHOOSER_PLAY_DONE);
  }
*/
  return(0);
}

void
ags_link_editor_file_chooser_play_done(AgsRecall *recall, AgsLinkEditor *link_editor)
{
  GtkToggleButton *toggle_button;
  GList *list;

  recall->flags |= AGS_RECALL_REMOVE;

  list = gtk_container_get_children((GtkContainer *) GTK_DIALOG(link_editor->file_chooser)->action_area);
  toggle_button = (GtkToggleButton *) list->data;

  link_editor->flags |= AGS_LINK_EDITOR_FILE_CHOOSER_PLAY_DONE;
  gtk_toggle_button_set_active(toggle_button, FALSE);

  g_list_free(list);
}

void
ags_link_editor_file_chooser_play_remove(AgsRecall *recall, AgsLinkEditor *link_editor)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(recall);
  g_object_unref((GObject *) play_audio_file);
}

void
ags_link_editor_file_chooser_play_cancel(AgsRecall *recall, AgsLinkEditor *link_editor)
{
  AgsPlayAudioFile *play_audio_file;

  play_audio_file = AGS_PLAY_AUDIO_FILE(recall);
  g_object_unref((GObject *) play_audio_file);
}
