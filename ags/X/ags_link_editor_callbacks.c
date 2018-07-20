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

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_line_editor.h>

#include <ags/i18n.h>

int ags_link_editor_file_chooser_response_callback(GtkWidget *widget, guint response, AgsLinkEditor *link_editor);

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

      g_object_get(channel,
		   "audio", &audio,
		   NULL);

      if(audio != NULL){
	machine = AGS_MACHINE(audio->machine);
    
	model = GTK_TREE_MODEL(ags_machine_get_possible_links(machine));
  
	if(AGS_IS_INPUT(line_editor->channel) &&
	   (AGS_MACHINE_TAKES_FILE_INPUT & (machine->flags)) != 0 &&
	   ((AGS_MACHINE_ACCEPT_WAV & (machine->file_input_flags)) != 0 ||
	    ((AGS_MACHINE_ACCEPT_OGG & (machine->file_input_flags)) != 0))){
	  AgsFileLink *file_link;
	  
	  gtk_list_store_append(GTK_LIST_STORE(model), &iter);

	  g_object_get(channel,
		       "file-link", &file_link,
		       NULL);
      
	  if(file_link != NULL){
	    gchar *filename;

	    pthread_mutex_t *file_link_mutex;

	    /* get file link mutex */
	    pthread_mutex_lock(ags_file_link_get_class_mutex());
	
	    file_link_mutex = file_link->obj_mutex;

	    pthread_mutex_unlock(ags_file_link_get_class_mutex());

	    /* get some fields */
	    pthread_mutex_lock(file_link_mutex);
	
	    filename = g_strdup(file_link->filename);
	
	    pthread_mutex_unlock(file_link_mutex);

	    gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			       0, g_strdup_printf("file://%s", filename),
			       1, NULL,
			       -1);
	    gtk_combo_box_set_active_iter(link_editor->combo,
					  &iter);

	    g_free(filename);
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

    AgsAudio *audio;
    AgsChannel *channel;

    GtkTreeModel *model;

    line_editor = AGS_LINE_EDITOR(gtk_widget_get_ancestor(GTK_WIDGET(link_editor),
							  AGS_TYPE_LINE_EDITOR));

    channel = line_editor->channel;

    g_object_get(channel,
		 "audio", &audio,
		 NULL);

    machine = AGS_MACHINE(audio->machine);
    
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
