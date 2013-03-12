/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_link_editor_callbacks.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file.h>

#include <ags/audio/recall/ags_play_audio_file.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

int ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor);
int ags_link_editor_file_chooser_play_callback(GtkToggleButton *toggle_button, AgsLinkEditor *link_editor);

void ags_link_editor_file_chooser_play_done(AgsRecall *recall, AgsLinkEditor *link_editor);
void ags_link_editor_file_chooser_play_cancel(AgsRecall *recall, AgsLinkEditor *link_editor);

int
ags_link_editor_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsLinkEditor *link_editor)
{
  AgsLineEditor *line_editor;

  if(old_parent != NULL)
    return(0);

  line_editor = (AgsLineEditor *) gtk_widget_get_ancestor(widget, AGS_TYPE_LINE_EDITOR);

  if(line_editor != NULL &&
     line_editor->channel != NULL &&
     line_editor->channel->audio != NULL &&
     AGS_AUDIO(line_editor->channel->audio)->machine != NULL){
    gtk_combo_box_set_model(link_editor->combo,
			    GTK_TREE_MODEL(ags_machine_get_possible_links(AGS_MACHINE(AGS_AUDIO(line_editor->channel->audio)->machine))));
  }

  return(0);
}

int
ags_link_editor_destroy_callback(GtkObject *object, AgsLinkEditor *link_editor)
{
  ags_link_editor_destroy(object);
  return(0);
}

int
ags_link_editor_show_callback(GtkWidget *widget, AgsLinkEditor *link_editor)
{
  ags_link_editor_show(widget);
  return(0);
}

void
ags_link_editor_combo_callback(GtkComboBox *combo, AgsLinkEditor *link_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(link_editor->combo,
				   &iter)){
    AgsMachine *machine;
    AgsLineEditor *line_editor;
    GtkTreeModel *model;
    AgsChannel *channel;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;

    model = gtk_combo_box_get_model(link_editor->combo);
    gtk_tree_model_get(model,
		       &iter,
		       1, &machine,
		       -1);

    if(machine == NULL){
      gtk_spin_button_set_value(link_editor->spin_button, 0.0);
    }else{
      if(AGS_IS_INPUT(channel))
	gtk_spin_button_set_range(link_editor->spin_button, 0.0, (gdouble) (machine->audio->output_lines - 1));
      else
	gtk_spin_button_set_range(link_editor->spin_button, 0.0, (gdouble) (machine->audio->input_lines - 1));
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
}

int
ags_link_editor_menu_item_callback(GtkWidget *widget, AgsLinkEditor *link_editor)
{
  /*
  AgsLineEditor *line_editor;
  AgsMachine *machine;

  machine = (AgsMachine *) g_object_get_data((GObject *) widget, g_type_name(AGS_TYPE_MACHINE));

  if(machine == NULL){
    link_editor->spin_button->adjustment->upper = 0.0;
  }else{
    line_editor = (AgsLineEditor *) gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_LINE_EDITOR);
    link_editor->spin_button->adjustment->upper = (gdouble) (AGS_IS_OUTPUT(line_editor->channel) ? machine->audio->input_lines - 1: machine->audio->output_lines - 1);
  }

  return(0);
  */
}

int
ags_link_editor_menu_item_file_callback(GtkWidget *widget, AgsLinkEditor *link_editor)
{
  /*
  GtkToggleButton *play;
  char *tmp, *dir, *name;

  if(link_editor->file_chooser != NULL)
    return(0);

  link_editor->file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(g_strdup("select audio file"),
										   (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) link_editor),
										   GTK_FILE_CHOOSER_ACTION_OPEN,
										   GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
										   GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
										   NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(link_editor->file_chooser), FALSE);
  g_object_set_data((GObject *) link_editor->file_chooser, (char *) g_type_name(AGS_TYPE_AUDIO_FILE), link_editor->audio_file);

  tmp = (char *) gtk_label_get_label(GTK_LABEL(GTK_BIN(widget)->child));
  tmp = g_strdup(tmp + 7);

  if(g_strcmp0(tmp, "")){
    char *tmp0;

    tmp0 = g_strrstr(tmp, "/");
    name = g_strdup(tmp0 + 1);
    dir = g_strndup(tmp, tmp0 - tmp);

    gtk_file_chooser_set_current_folder((GtkFileChooser *) link_editor->file_chooser, dir);
    gtk_file_chooser_set_current_name((GtkFileChooser *) link_editor->file_chooser, name);
  }

  play = (GtkToggleButton *) g_object_new(GTK_TYPE_TOGGLE_BUTTON,
					  "label", GTK_STOCK_MEDIA_PLAY,
					  "use-stock", TRUE,
					  "use-underline", TRUE,
					  NULL);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(link_editor->file_chooser)->action_area, (GtkWidget *) play, FALSE, FALSE, 0);
  gtk_box_reorder_child((GtkBox *) GTK_DIALOG(link_editor->file_chooser)->action_area, (GtkWidget *) play, 0);

  gtk_widget_show_all((GtkWidget *) link_editor->file_chooser);

  g_signal_connect((GObject *) link_editor->file_chooser, "response",
		   G_CALLBACK(ags_link_editor_file_chooser_response_callback), (gpointer) link_editor);
  g_signal_connect((GObject *) play, "toggled",
		   G_CALLBACK(ags_link_editor_file_chooser_play_callback), (gpointer) link_editor);
  */
}

int
ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor)
{
  /*
  GtkFileChooserDialog *file_chooser;
  AgsDevout *devout;
  AgsAudioFile *audio_file;
  char *name;

  file_chooser = link_editor->file_chooser;
  audio_file = link_editor->audio_file;

  if(response == GTK_RESPONSE_ACCEPT){
    name = gtk_file_chooser_get_filename((GtkFileChooser *) file_chooser);

    if(audio_file != NULL){
      if(g_strcmp0(audio_file->name, name)){
	g_object_unref(G_OBJECT(audio_file));
	goto ags_link_editor_file_chooser_response_callback0;
      }
    }else{
    ags_link_editor_file_chooser_response_callback0:
      gtk_label_set_label(GTK_LABEL(GTK_BIN(link_editor->option)->child), g_strconcat("file://", name, NULL));

      audio_file = ags_audio_file_new();
      audio_file->name = name;

      devout = AGS_DEVOUT(AGS_AUDIO(AGS_LINE_EDITOR(gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_LINE_EDITOR))->channel->audio)->devout);
      ags_audio_file_set_devout(audio_file, devout);

      ags_audio_file_open(audio_file);
      AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);

      link_editor->audio_file = audio_file;
    }
  }else if(response == GTK_RESPONSE_CANCEL){
    if(audio_file != NULL){
      g_object_set_data((GObject *) link_editor->option->menu_item, (char *) g_type_name(AGS_TYPE_AUDIO_FILE), NULL);
      g_object_unref(G_OBJECT(audio_file));
    }
  }

  link_editor->file_chooser = NULL;
  gtk_widget_destroy((GtkWidget *) file_chooser);
  */
}

int
ags_link_editor_file_chooser_play_callback(GtkToggleButton *toggle_button, AgsLinkEditor *link_editor)
{
  /*
  GtkFileChooserDialog *file_chooser;
  AgsDevout *devout;
  AgsPlayAudioFile *play_audio_file;
  AgsAudioFile *audio_file;
  char *name;
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  file_chooser = link_editor->file_chooser;
  audio_file = link_editor->audio_file;

  devout = AGS_DEVOUT(AGS_AUDIO(AGS_LINE_EDITOR(gtk_widget_get_ancestor((GtkWidget *) link_editor, AGS_TYPE_LINE_EDITOR))->channel->audio)->devout);

  if(toggle_button->active){
    /* AgsPlayAudioFile * /
    play_audio_file = ags_play_audio_file_new();
    play_audio_file->devout = devout;
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

      ags_audio_file_set_devout(audio_file, devout);

      ags_audio_file_open(audio_file);

      AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
    }

    play_audio_file->audio_file = audio_file;

    /* AgsDevout * /
    g_static_mutex_lock(&mutex);
    devout->play_recall = g_list_append(devout->play_recall, play_audio_file);
    devout->flags |= AGS_DEVOUT_PLAY_RECALL;
    devout->play_recall_ref++;
    AGS_DEVOUT_GET_CLASS(devout)->run((void *) devout);
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

  recall->flags |= AGS_RECALL_REMOVE;

  toggle_button = (GtkToggleButton *) gtk_container_get_children((GtkContainer *) GTK_DIALOG(link_editor->file_chooser)->action_area)->data;

  link_editor->flags |= AGS_LINK_EDITOR_FILE_CHOOSER_PLAY_DONE;
  gtk_toggle_button_set_active(toggle_button, FALSE);

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
