/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/ags_machine_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_midi_dialog.h>

#include <ags/X/editor/ags_envelope_dialog.h>
#include <ags/X/editor/ags_machine_radio_button.h>
#include <ags/X/editor/ags_file_selection.h>

#include <ags/i18n.h>

#define AGS_RENAME_ENTRY "AgsRenameEntry"

int ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_rename_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_reposition_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine);

int
ags_machine_button_press_callback(GtkWidget *handle_box, GdkEventButton *event, AgsMachine *machine)
{
  AgsWindow *window = AGS_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET(handle_box)));

  if(event->button == 3){
    gtk_menu_popup (GTK_MENU (machine->popup),
                    NULL, NULL, NULL, NULL,
                    event->button, event->time);
  }else if(event->button == 1){
    window->selected = machine;
  }

  return(0);
}

void
ags_machine_popup_move_up_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(GTK_WIDGET(machine)->parent),
				   GTK_WIDGET(machine),
				   "position", &val);

  if(g_value_get_int (&val) > 0){
    gtk_box_reorder_child(GTK_BOX(GTK_WIDGET(machine)->parent),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) - 1);
  }

  g_value_unset (&val);
}

void
ags_machine_popup_move_down_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(GTK_WIDGET(machine)->parent),
				   GTK_WIDGET(machine),
				   "position", &val);

  if(g_value_get_int (&val) < g_list_length(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine)->parent)) - 1){
    gtk_box_reorder_child(GTK_BOX(GTK_WIDGET(machine)->parent),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) + 1);
  }

  g_value_unset (&val);
}

void
ags_machine_popup_hide_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_hide(GTK_BIN(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine))->data)->child);
}

void
ags_machine_popup_show_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_show(GTK_BIN(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine))->data)->child);
}

void
ags_machine_popup_destroy_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsWindow *window;

  AgsAudio *audio;
  
  AgsRemoveAudio *remove_audio;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);

  application_context = ags_application_context_get_instance();
  
  /* destroy editor */
  list =
    list_start = gtk_container_get_children((GtkContainer *) window->notation_editor->machine_selector);

  list = list->next;

  while(list != NULL){
    if(AGS_IS_MACHINE_RADIO_BUTTON(list->data) && AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
      gtk_widget_destroy(list->data);
      break;
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  /* destroy automation editor */  
  list =
    list_start = gtk_container_get_children((GtkContainer *) window->automation_window->automation_editor->machine_selector);

  list = list->next;

  while(list != NULL){
    if(AGS_IS_MACHINE_RADIO_BUTTON(list->data) && AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
      gtk_widget_destroy(list->data);
      break;
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  /* destroy machine */
  audio = machine->audio;
  g_object_ref(audio);

  ags_connectable_disconnect(AGS_CONNECTABLE(machine));
  gtk_widget_destroy((GtkWidget *) machine);

  /* get task thread */
  remove_audio = ags_remove_audio_new(application_context,
				      audio);
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) remove_audio);
}

void
ags_machine_popup_rename_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GtkDialog *dialog;
  GtkEntry *entry;

  if(machine->rename != NULL){
    return;
  }
  
  machine->rename =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("rename"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(entry, machine->machine_name);
  gtk_box_pack_start((GtkBox *) dialog->vbox,
		     (GtkWidget *) entry,
		     FALSE, FALSE,
		     0);

  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_machine_popup_rename_response_callback), (gpointer) machine);
}

int
ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    GList *children;

    gchar *str;

    children = gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox);
    
    str = gtk_editable_get_chars(GTK_EDITABLE(children->data),
				 0, -1);
    g_object_set(machine,
		 "machine-name", str,
		 NULL);        
    
    g_list_free(children);
  }
  
  machine->rename = NULL;
  gtk_widget_destroy(widget);

  return(0);
}

void
ags_machine_popup_rename_audio_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GtkDialog *dialog;
  GtkEntry *entry;

  AgsAudio *audio;

  gchar *audio_name;
  
  pthread_mutex_t *audio_mutex;
  
  if(machine->rename_audio != NULL){
    return;
  }

  audio = machine->audio;

  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());
  
  machine->rename_audio =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("rename audio"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);

  pthread_mutex_lock(audio_mutex);

  audio_name = g_strdup(audio->audio_name);
  
  pthread_mutex_unlock(audio_mutex);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(entry, audio_name);
  gtk_box_pack_start((GtkBox *) dialog->vbox,
		     (GtkWidget *) entry,
		     FALSE, FALSE,
		     0);

  g_free(audio_name);
  
  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_machine_popup_rename_audio_response_callback), (gpointer) machine);
}

int
ags_machine_popup_rename_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    GList *children;

    gchar *str;

    children = gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox);
    
    str = gtk_editable_get_chars(GTK_EDITABLE(children->data),
				 0, -1);
    g_object_set(machine->audio,
		 "audio-name", str,
		 NULL);
    
    g_list_free(children);
  }
  
  machine->rename_audio = NULL;
  gtk_widget_destroy(widget);

  return(0);
}

void
ags_machine_popup_reposition_audio_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GtkDialog *dialog;
  GtkSpinButton *spin_button;

  AgsAudio *audio;

  AgsApplicationContext *application_context;

  GList *start_list;

  gint length;
  gint position;
    
  if(machine->reposition_audio != NULL){
    return;
  }

  application_context = ags_application_context_get_instance();
  
  audio = machine->audio;

  start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

  length = g_list_length(start_list);
  position = g_list_index(start_list,
			  audio);
  
  machine->reposition_audio =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("reposition audio"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);

  if(position >= 0){
    spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, (gdouble) (length - 1), 1.0);
  }else{
    spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0, -1.0, 0.0);
  }
  
  gtk_spin_button_set_value(spin_button,
			    (gdouble) position);
  gtk_box_pack_start((GtkBox *) dialog->vbox,
		     (GtkWidget *) spin_button,
		     FALSE, FALSE,
		     0);
  
  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_machine_popup_reposition_audio_response_callback), (gpointer) machine);

  g_list_free_full(start_list,
		   g_object_unref);
}

int
ags_machine_popup_reposition_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsAudio *audio;

    AgsApplicationContext *application_context;

    GList *children;
    GList *start_list;
  
    gint new_position;
    
    application_context = ags_application_context_get_instance();
    
    audio = machine->audio;
    
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));

    children = gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox);
    
    new_position = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(children->data));
    
    start_list = g_list_remove(start_list,
			       audio);
    start_list = g_list_insert(start_list,
			       audio,
			       new_position);

    ags_sound_provider_set_audio(AGS_SOUND_PROVIDER(application_context),
				 start_list);
    g_list_foreach(start_list,
		   (GFunc) g_object_unref,
		   NULL);

    g_list_free(children);
  }
  
  machine->reposition_audio = NULL;
  gtk_widget_destroy(widget);

  return(0);
}

void
ags_machine_popup_properties_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  machine->properties = (GtkDialog *) ags_machine_editor_new(machine);
  g_signal_connect_after(machine->properties, "destroy",
			 G_CALLBACK(ags_machine_popup_properties_destroy_callback), machine);

  gtk_window_set_default_size((GtkWindow *) machine->properties, -1, 400);
  
  ags_connectable_connect(AGS_CONNECTABLE(machine->properties));

  ags_applicable_reset(AGS_APPLICABLE(machine->properties));

  gtk_widget_show_all((GtkWidget *) machine->properties);
}

void
ags_machine_popup_sticky_controls_toggled_callback(GtkWidget *widget, AgsMachine *machine)
{
  if(gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))){
    machine->flags |= AGS_MACHINE_STICKY_CONTROLS;
  }else{
    machine->flags &= (~AGS_MACHINE_STICKY_CONTROLS);
  }
}

int
ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine)
{
  machine->properties = NULL;

  return(0);
}

void
ags_machine_popup_copy_pattern_callback(GtkWidget *widget, AgsMachine *machine)
{
  ags_machine_copy_pattern(machine);
}

void
ags_machine_popup_paste_pattern_callback(GtkWidget *widget, AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_popup_envelope_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsEnvelopeDialog *envelope_dialog;
  
  if(machine->envelope_dialog == NULL){
    envelope_dialog = ags_envelope_dialog_new(machine);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0){
      ags_envelope_dialog_add_pattern_tab(envelope_dialog);
    }
    
    machine->envelope_dialog = (GtkDialog *) envelope_dialog;
    
    ags_connectable_connect(AGS_CONNECTABLE(envelope_dialog));
    ags_applicable_reset(AGS_APPLICABLE(envelope_dialog));

    gtk_widget_show_all((GtkWidget *) envelope_dialog);
  }
}

void
ags_machine_popup_connection_editor_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsConnectionEditor *connection_editor;
  
  if(machine->connection_editor == NULL){
    connection_editor = ags_connection_editor_new(NULL);

    if((AGS_MACHINE_SHOW_AUDIO_OUTPUT_CONNECTION & (machine->connection_flags)) != 0){
      connection_editor->flags |= AGS_CONNECTION_EDITOR_SHOW_OUTPUT;
    }

    if((AGS_MACHINE_SHOW_AUDIO_INPUT_CONNECTION & (machine->connection_flags)) != 0){
      connection_editor->flags |= AGS_CONNECTION_EDITOR_SHOW_INPUT;
    }

    ags_connection_editor_set_machine(connection_editor, machine);
	  
    machine->connection_editor = (GtkDialog *) connection_editor;
    
    ags_connectable_connect(AGS_CONNECTABLE(connection_editor));
    ags_applicable_reset(AGS_APPLICABLE(connection_editor));

    gtk_widget_show_all((GtkWidget *) connection_editor);
  }else{
    connection_editor = (AgsConnectionEditor *) machine->connection_editor;
  }

  gtk_widget_show_all((GtkWidget *) connection_editor);
}

void
ags_machine_popup_midi_dialog_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsMidiDialog *midi_dialog;
  
  if(machine->midi_dialog == NULL){
    midi_dialog = ags_midi_dialog_new(machine);
    machine->midi_dialog = (GtkDialog *) midi_dialog;
    midi_dialog->flags |= (AGS_MIDI_DIALOG_IO_OPTIONS |
			   AGS_MIDI_DIALOG_MAPPING |
			   AGS_MIDI_DIALOG_DEVICE);

    ags_connectable_connect(AGS_CONNECTABLE(midi_dialog));
    ags_applicable_reset(AGS_APPLICABLE(midi_dialog));

    gtk_widget_show_all((GtkWidget *) midi_dialog);
  }else{
    midi_dialog = (AgsMidiDialog *) machine->midi_dialog;
  }

  gtk_widget_show_all((GtkWidget *) midi_dialog);
}

void
ags_machine_open_response_callback(GtkDialog *dialog, gint response, AgsMachine *machine)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *overwrite;
  GtkCheckButton *create;
  GSList *filenames;

  file_chooser = GTK_FILE_CHOOSER_DIALOG(dialog);

  if(response == GTK_RESPONSE_ACCEPT){
    filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
    overwrite = g_object_get_data(G_OBJECT(dialog), "overwrite");
    create = g_object_get_data(G_OBJECT(dialog), "create");

    ags_machine_open_files(machine,
			   filenames,
			   GTK_TOGGLE_BUTTON(overwrite)->active,
			   GTK_TOGGLE_BUTTON(create)->active);
  }

  gtk_widget_destroy(GTK_WIDGET(file_chooser));
}

void
ags_machine_open_extended_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  GtkFileChooserDialog *file_chooser;
  AgsFileSelection *file_selection;
  GtkCheckButton *overwrite;
  GtkCheckButton *create;

  GSList *filenames;

  gchar *current_folder;

  GError *error;

  file_chooser = (GtkFileChooserDialog *) gtk_widget_get_toplevel(widget);

  if(response == GTK_RESPONSE_ACCEPT){
    filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
    overwrite = g_object_get_data((GObject *) widget, "overwrite");
    create = g_object_get_data((GObject *) widget, "create");

    current_folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(file_chooser));
    //TODO:JK: you need to check against recently used
    //TODO:JK: add more file types to AgsFileSelection

    /* check for supported packed audio files */
    file_selection = (AgsFileSelection *) gtk_file_chooser_get_extra_widget(GTK_FILE_CHOOSER(file_chooser));

    if(file_selection != NULL && g_strcmp0(file_selection->directory, current_folder)){
      gtk_widget_destroy(GTK_WIDGET(file_selection));

      file_selection = NULL;
    }

    if(file_selection == NULL ||
       (AGS_FILE_SELECTION_COMPLETED & (file_selection->flags)) == 0){

      if((AGS_MACHINE_ACCEPT_SOUNDFONT2 & (machine->file_input_flags)) != 0){
	GDir *current_directory;

	GList *new_entry, *old_entry;	  
	GSList *slist;

	gchar *current_filename;
	
	slist = filenames;
	new_entry = NULL;
	
	while(slist != NULL){
	  if(g_str_has_suffix(slist->data,
			      ".sf2")){
	    AgsFileSelectionEntry *entry;
	    
	    
	    entry = ags_file_selection_entry_alloc();
	    entry->filename = slist->data;
	  
	    new_entry = g_list_prepend(new_entry,
				       entry);
	  }
	  
	  slist = slist->next;
	}
	
	old_entry = NULL;
	
	if(file_selection == NULL){
	  if(new_entry != NULL){
	    file_selection = ags_file_selection_new();
	    gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(file_chooser),
					      GTK_WIDGET(file_selection));

	    ags_file_selection_set_entry(file_selection,
					 new_entry);
	    ags_connectable_connect(AGS_CONNECTABLE(file_selection));

	    gtk_widget_show_all(GTK_WIDGET(file_selection));

	    return;
	  }
	}else if(AGS_IS_FILE_SELECTION(file_selection)){
	  GList *really_new_entry;
	  GList *list;
	  
	  old_entry = file_selection->entry;
	  list = new_entry;
	  really_new_entry = NULL;
	  
	  /* check against existing entries */
	  if(new_entry != NULL){
	    while(list != NULL){
	      if(g_list_find(old_entry, list->data) == NULL){
		really_new_entry = g_list_prepend(really_new_entry,
						  list->data);
	      }else{
		free(list->data);
	      }
	      
	      list = list->next;
	    }
	    
	    g_list_free(new_entry);
	  }
	  
	  ags_file_selection_set_entry(file_selection,
				       really_new_entry);

	  /* adding lost files */
	  //TODO:JK: figure out if you need to copy the GSList of filenames
	  gtk_file_chooser_select_all(GTK_FILE_CHOOSER(file_chooser));

	  error = NULL;
	  current_directory = g_dir_open(current_folder,
					 0,
					 &error);

	  if(error != NULL){
	    g_message("%s", error->message);

	    g_error_free(error);
	  }
	  
	  while((current_filename = (gchar *) g_dir_read_name(current_directory)) != NULL){
	    if(!g_strcmp0(".", current_filename) ||
	       !g_strcmp0("..", current_filename))
	      continue;

	    if(!ags_file_selection_contains_file(file_selection,
						 current_filename) &&
	       g_slist_find(filenames, current_filename) == NULL){
	      gtk_file_chooser_unselect_filename(GTK_FILE_CHOOSER(file_chooser),
						 current_filename);
	    }
	  }
	  
	  g_dir_close(current_directory);
	  
	  return;
	}
      }
    }
    
    //TODO:JK: fix GSList filenames memory leak
    ags_machine_open_files(machine,
			   filenames,
			   overwrite->toggle_button.active,
			   create->toggle_button.active);
  }
}

void
ags_machine_play_callback(GtkWidget *toggle_button, AgsMachine *machine)
{
  if(machine == NULL){
    return;
  }

  if(GTK_TOGGLE_BUTTON(toggle_button)->active){
    if((AGS_MACHINE_BLOCK_PLAY & (machine->flags)) != 0){      
      return;
    }

    g_message("machine: on");

    machine->flags |= AGS_MACHINE_BLOCK_PLAY;

    ags_machine_set_run_extended(machine,
				 TRUE,
				 TRUE, FALSE, FALSE, FALSE);

    machine->flags &= (~AGS_MACHINE_BLOCK_PLAY);
  }else{
    if((AGS_MACHINE_BLOCK_STOP & (machine->flags)) != 0){
      return;
    }

    g_message("machine: off");

    machine->flags |= AGS_MACHINE_BLOCK_STOP;

    ags_machine_set_run_extended(machine,
				 FALSE,
				 TRUE, FALSE, FALSE, FALSE);

    machine->flags &= (~AGS_MACHINE_BLOCK_STOP);
  }
}

void
ags_machine_resize_audio_channels_callback(AgsMachine *machine,
					   guint audio_channels, guint audio_channels_old,
					   gpointer data)
{
  GList *pad_list;
  GList *line_list;
  
  guint i;
  
  /* resize */
  if((AGS_MACHINE_CONNECTED & (machine->flags)) != 0){
    if(audio_channels > audio_channels_old){
      if(machine->input != NULL){
	pad_list = gtk_container_get_children(GTK_CONTAINER(machine->input));
      
	while(pad_list != NULL){
	  line_list = gtk_container_get_children((GtkContainer *) AGS_PAD(pad_list->data)->expander_set);
	  line_list = g_list_nth(line_list,
				 audio_channels_old);
	
	  for(i = 0; i < audio_channels - audio_channels_old; i++){
	    ags_connectable_connect(AGS_CONNECTABLE(line_list->data));

	    line_list = line_list->next;
	  }
	
	  pad_list = pad_list->next;
	}
      }

      if(machine->output != NULL){
	pad_list = gtk_container_get_children(GTK_CONTAINER(machine->output));
      
	while(pad_list != NULL){
	  line_list = gtk_container_get_children((GtkContainer *) AGS_PAD(pad_list->data)->expander_set);
	  line_list = g_list_nth(line_list,
				 audio_channels_old);
	
	  for(i = 0; i < audio_channels - audio_channels_old; i++){
	    ags_connectable_connect(AGS_CONNECTABLE(line_list->data));

	    line_list = line_list->next;
	  }
	  
	  pad_list = pad_list->next;
	}
      }
    }
  }
}

void
ags_machine_resize_pads_callback(AgsMachine *machine, GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data)
{
  GList *pad_list;
  
  /* resize */
  if((AGS_MACHINE_CONNECTED & (machine->flags)) != 0){
    if(pads > pads_old){
      if(g_type_is_a(channel_type,
		     AGS_TYPE_INPUT)){
	if(machine->input != NULL){
	  pad_list = gtk_container_get_children(GTK_CONTAINER(machine->input));
	  pad_list = g_list_nth(pad_list,
				pads_old);
      
	  while(pad_list != NULL){
	    ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
	
	    pad_list = pad_list->next;
	  }
	}
      }

      if(g_type_is_a(channel_type,
		     AGS_TYPE_OUTPUT)){
	if(machine->output != NULL){
	  pad_list = gtk_container_get_children(GTK_CONTAINER(machine->output));
	  pad_list = g_list_nth(pad_list,
				pads_old);
      
	  while(pad_list != NULL){
	    ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
	
	    pad_list = pad_list->next;
	  }
	}
      }
    }
  }
}

void
ags_machine_stop_callback(AgsMachine *machine,
			  GList *recall_id, gint sound_scope,
			  gpointer data)
{
  gboolean reset_active;
  
  if((AGS_MACHINE_BLOCK_STOP_CALLBACK & (machine->flags)) != 0){
    return;
  }
  
  machine->flags |= AGS_MACHINE_BLOCK_STOP_CALLBACK;

  /* play button - check reset active */
  reset_active = (sound_scope == AGS_SOUND_SCOPE_SEQUENCER) ? TRUE: FALSE;
  
  if(reset_active){
    gtk_toggle_button_set_active(machine->play, FALSE);
  }

#if 0
  if(sound_scope == AGS_SOUND_SCOPE_SEQUENCER){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 TRUE, FALSE, FALSE, FALSE);
  }

  if(sound_scope == AGS_SOUND_SCOPE_NOTATION){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 FALSE, TRUE, FALSE, FALSE);
  }

  if(sound_scope == AGS_SOUND_SCOPE_WAVE){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 FALSE, FALSE, TRUE, FALSE);
  }

  if(sound_scope == AGS_SOUND_SCOPE_MIDI){
    ags_machine_set_run_extended(machine,
				 FALSE,
				 FALSE, FALSE, FALSE, TRUE);
  }
#endif

  machine->flags &= (~AGS_MACHINE_BLOCK_STOP_CALLBACK);
}

void
ags_machine_active_playback_start_channel_launch_callback(AgsTask *task,
							  AgsPlayback *playback)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;
  AgsAudioSignal *audio_signal; 
  AgsRecallID *recall_id;
  AgsNote *play_note;

  GObject *output_soundcard;
  
  g_object_get(playback,
	       "channel", &channel,
	       "play-note", &play_note,
	       NULL);

  g_object_get(channel,
	       "audio", &audio,
	       NULL);
  
  recall_id = ags_playback_get_recall_id(playback,
					 AGS_SOUND_SCOPE_PLAYBACK);

  /* get some fields */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  end_recycling = ags_recycling_next(last_recycling);

  recycling = first_recycling;
  g_object_ref(recycling);

  next_recycling = NULL;
  
  while(recycling != end_recycling){
    g_object_get(recycling,
		 "output-soundcard", &output_soundcard,
		 NULL);
    
    if(!ags_recall_global_get_rt_safe()){
      /* instantiate audio signal */
      audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					  (GObject *) recycling,
					  (GObject *) recall_id);
      g_object_set(audio_signal,
		   "note", play_note,
		   NULL);

      /* add audio signal */
      if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_PATTERN_MODE)){
	ags_recycling_create_audio_signal_with_defaults(recycling,
							audio_signal,
							0.0, 0);
      }else{
	gdouble notation_delay;
	guint buffer_size;
	guint note_x0, note_x1;

	notation_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));

	g_object_get(recycling,
		     "buffer-size", &buffer_size,
		     NULL);
	
	g_object_get(play_note,
		     "x0", &note_x0,
		     "x1", &note_x1,
		     NULL);

	ags_recycling_create_audio_signal_with_frame_count(recycling,
							   audio_signal,
							   (guint) (((gdouble) buffer_size * notation_delay) * (gdouble) (note_x1 - note_x0)),
							   0.0, 0);
      }
      
      audio_signal->stream_current = audio_signal->stream;
      ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
	
      /*
       * emit add_audio_signal on AgsRecycling
       */
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);
    }else{
      GList *start_list, *list;

      g_object_get(recycling,
		   "audio-signal", &start_list,
		   NULL);
      
      audio_signal = NULL;
      list = ags_audio_signal_find_by_recall_id(start_list,
						(GObject *) recall_id);
	    
      if(list != NULL){
	audio_signal = list->data;

	g_object_set(audio_signal,
		     "note", play_note,
		     NULL);
      }

      g_list_free_full(start_list,
		       g_object_unref);

      g_object_set(play_note,
		   "rt-offset", 0,
		   NULL);
    }

    g_object_unref(output_soundcard);
    
    /* iterate */
    next_recycling = ags_recycling_next(recycling);

    g_object_unref(recycling);

    recycling = next_recycling;
  }

  if(next_recycling != NULL){
    g_object_unref(next_recycling);
  }
  
  /* unref */
  g_object_unref(audio);
  
  g_object_unref(channel);

  if(first_recycling != NULL){
    g_object_unref(first_recycling);
    g_object_unref(last_recycling);
  }
  
  if(end_recycling != NULL){
    g_object_unref(end_recycling);
  }
  
  g_object_unref(recall_id);

  g_object_unref(play_note);
}
