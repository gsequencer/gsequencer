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

#include <ags/app/ags_link_editor_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_line.h>

#include <ags/i18n.h>

void ags_link_editor_pcm_file_dialog_response_callback(AgsPCMFileDialog *pcm_file_dialog, guint response,
						       AgsLinkEditor *link_editor);

#define AGS_LINK_EDITOR_OPEN_SPIN_BUTTON "AgsLinkEditorOpenSpinButton"

void
ags_link_editor_combo_callback(GtkComboBox *combo, AgsLinkEditor *link_editor)
{
  GtkTreeIter iter;

  if(gtk_combo_box_get_active_iter(link_editor->combo,
				   &iter)){
    AgsMachine *machine, *link_machine;
    AgsMachineEditorLine *machine_editor_line;
    AgsFileWidget *file_widget;

    AgsAudio *audio;
    AgsChannel *channel;

    AgsApplicationContext *application_context;
  
    GtkTreeModel *model;

    gchar *recently_used_filename;
    gchar *bookmark_filename;
    gchar *home_path;
    gchar *sandbox_path;
    
    /* get application context */  
    application_context = ags_application_context_get_instance();

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
      if(link_editor->open_dialog != NULL || (AGS_LINK_EDITOR_BLOCK_FILE_CHOOSER & (link_editor->flags)) != 0){
	return;
      }

      gtk_widget_set_sensitive((GtkWidget *) link_editor->spin_button,
			       FALSE);

      link_editor->open_dialog = ags_pcm_file_dialog_new((GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)),
							 i18n("open audio file"));

      file_widget = ags_pcm_file_dialog_get_file_widget(link_editor->open_dialog);

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

      ags_file_widget_set_file_action(file_widget,
				      AGS_FILE_WIDGET_OPEN);

      ags_file_widget_set_default_bundle(file_widget,
					 AGS_DEFAULT_BUNDLE_ID);

      /*  */
      str = NULL;
      
      gtk_tree_model_get(model,
			 &iter,
			 0, &str,
			 -1);

      if(str != NULL){
	tmp = g_strdup(str + 7);

	if((!g_strcmp0(tmp, "")) == FALSE){
	  ags_file_widget_set_current_path(file_widget,
					   tmp);

	  ags_file_widget_refresh(file_widget);
	}
      }
      
      g_signal_connect((GObject *) link_editor->open_dialog, "response",
		       G_CALLBACK(ags_link_editor_pcm_file_dialog_response_callback), (gpointer) link_editor);

      gtk_widget_show((GtkWidget *) link_editor->open_dialog);
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

void
ags_link_editor_pcm_file_dialog_response_callback(AgsPCMFileDialog *open_dialog, guint response,
						  AgsLinkEditor *link_editor)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;
    
    GtkTreeModel *model;
    
    GtkTreeIter iter;

    char *filename;

    gint strv_length;

    /* set filename in combo box */
    model = gtk_combo_box_get_model(link_editor->combo);
    
    file_widget = ags_pcm_file_dialog_get_file_widget(open_dialog);

    filename = ags_file_widget_get_filename(file_widget);
    
    if(!g_strv_contains((const gchar * const *) file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }

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
			      gtk_spin_button_get_value(open_dialog->audio_channel));
  }

  link_editor->open_dialog = NULL;
  
  gtk_window_destroy((GtkWindow *) open_dialog);
}
