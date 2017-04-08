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

#include <ags/X/ags_machine_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_notation.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/task/ags_remove_audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_automation_editor.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_connection_editor.h>
#include <ags/X/ags_midi_dialog.h>

#include <ags/X/editor/ags_machine_radio_button.h>
#include <ags/X/editor/ags_file_selection.h>

#include <ags/X/thread/ags_gui_thread.h>

#define AGS_RENAME_ENTRY "AgsRenameEntry\0"

int ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine);
void ags_machine_connection_editor_delete_event_callback(GtkWidget *dialog, gint response, AgsMachine *machine);
void ags_machine_midi_dialog_delete_event_callback(GtkWidget *dialog, gint response, AgsMachine *machine);
void ags_machine_start_complete_response(GtkWidget *dialog, gint response, AgsMachine *machine);
void ags_machine_remove_audio_launch_callback(AgsTask *task, AgsMachine *machine);

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

int
ags_machine_popup_move_up_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(GTK_WIDGET(machine)->parent),
				   GTK_WIDGET(machine),
				   "position\0", &val);

  if(g_value_get_int (&val) > 0){
    gtk_box_reorder_child(GTK_BOX(GTK_WIDGET(machine)->parent),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) - 1);
  }

  g_value_unset (&val);

  return(0);
}

int
ags_machine_popup_move_down_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(GTK_WIDGET(machine)->parent),
				   GTK_WIDGET(machine),
				   "position\0", &val);

  if(g_value_get_int (&val) < g_list_length(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine)->parent)) - 1){
    gtk_box_reorder_child(GTK_BOX(GTK_WIDGET(machine)->parent),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) + 1);
  }

  g_value_unset (&val);

  return(0);
}

int
ags_machine_popup_hide_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_hide(GTK_BIN(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine))->data)->child);

  return(0);
}

int
ags_machine_popup_show_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  gtk_widget_show(GTK_BIN(gtk_container_get_children((GtkContainer *) GTK_WIDGET(machine))->data)->child);

  return(0);
}

void
ags_machine_remove_audio_launch_callback(AgsTask *task, AgsMachine *machine)
{
  AgsWindow *window;

  GList *list, *list_start;

  gdk_threads_enter();
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);
  
  /* destroy editor */
  list = window->editor->editor_child;

  while(list != NULL){
    if(AGS_EDITOR_CHILD(list->data)->machine == machine){
      //TODO:JK: remove work-around
      g_signal_handlers_disconnect_by_data(machine->audio,
					   AGS_EDITOR_CHILD(list->data)->notebook);
      g_signal_handlers_disconnect_by_data(machine->audio,
					   AGS_EDITOR_CHILD(list->data)->edit_widget);
      
      ags_connectable_disconnect(AGS_CONNECTABLE(AGS_EDITOR_CHILD(list->data)->notebook));
      ags_connectable_disconnect(AGS_CONNECTABLE(AGS_EDITOR_CHILD(list->data)->edit_widget));
      
      gtk_widget_destroy((GtkWidget *) AGS_EDITOR_CHILD(list->data)->notebook);
      gtk_widget_destroy((GtkWidget *) AGS_EDITOR_CHILD(list->data)->meter);
      gtk_widget_destroy((GtkWidget *) AGS_EDITOR_CHILD(list->data)->edit_widget);
      break;
    }

    list = list->next;
  }

  list =
    list_start = gtk_container_get_children((GtkContainer *) window->editor->machine_selector);

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
  list = window->automation_window->automation_editor->automation_editor_child;

  while(list != NULL){
    if(AGS_AUTOMATION_EDITOR_CHILD(list->data)->machine == machine){
      //TODO:JK: remove work-around
      g_signal_handlers_disconnect_by_data(machine->audio,
					   AGS_AUTOMATION_EDITOR_CHILD(list->data)->output_notebook);
      g_signal_handlers_disconnect_by_data(machine->audio,
					   AGS_AUTOMATION_EDITOR_CHILD(list->data)->input_notebook);
      
      ags_connectable_disconnect(AGS_CONNECTABLE(AGS_AUTOMATION_EDITOR_CHILD(list->data)->output_notebook));
      ags_connectable_disconnect(AGS_CONNECTABLE(AGS_AUTOMATION_EDITOR_CHILD(list->data)->input_notebook));
      
      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->audio_scale);
      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->audio_automation_edit);

      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->output_scale);
      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->output_notebook);
      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->output_automation_edit);

      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->input_scale);
      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->input_notebook);
      gtk_widget_destroy((GtkWidget *) AGS_AUTOMATION_EDITOR_CHILD(list->data)->input_automation_edit);

      break;
    }

    list = list->next;
  }
  
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
  ags_connectable_disconnect(AGS_CONNECTABLE(machine));
  gtk_widget_destroy((GtkWidget *) machine);

  gdk_threads_leave();
}

void
ags_machine_popup_destroy_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsWindow *window;
  AgsRemoveAudio *remove_audio;

  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsAudioLoop *) application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type((AgsThread *) audio_loop,
						       AGS_TYPE_TASK_THREAD);

  g_object_ref(machine->audio);
  remove_audio = ags_remove_audio_new(window->soundcard,
				      machine->audio);
  g_signal_connect(remove_audio, "launch\0",
		   G_CALLBACK(ags_machine_remove_audio_launch_callback), machine);
  ags_task_thread_append_task(task_thread,
			      AGS_TASK(remove_audio));
}

int
ags_machine_popup_rename_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GtkDialog *dialog;
  GtkEntry *entry;

  machine->rename =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(g_strdup("rename\0"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(entry, machine->name);
  gtk_box_pack_start((GtkBox *) dialog->vbox, (GtkWidget *) entry, FALSE, FALSE, 0);

  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response\0",
		   G_CALLBACK(ags_machine_popup_rename_response_callback), (gpointer) machine);

  return(0);
}

int
ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  gchar *text;

  if(response == GTK_RESPONSE_ACCEPT){
    AgsWindow *window;
    GtkMenuToolButton *menu_tool_button;

    GList *list, *list_start;
    
    window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);
    
    if(machine->name != NULL){
      g_free(machine->name);
    }

    text = gtk_editable_get_chars(GTK_EDITABLE(gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox)->data), 0, -1);
    machine->name = g_strdup(text);

    gtk_menu_tool_button_set_menu((GtkMenuToolButton *) gtk_frame_get_label_widget((GtkFrame *) gtk_bin_get_child((GtkBin *) machine)),
				  NULL);
    gtk_widget_destroy(gtk_frame_get_label_widget((GtkFrame *) gtk_bin_get_child((GtkBin *) machine)));

    menu_tool_button = 
      machine->menu_tool_button = g_object_new(GTK_TYPE_MENU_TOOL_BUTTON,
					       "label\0", g_strconcat(G_OBJECT_TYPE_NAME(machine), ": \0", text, NULL),
					       "menu\0", machine->popup,
					       NULL);
    gtk_frame_set_label_widget((GtkFrame *) gtk_bin_get_child((GtkBin *) machine),
			       (GtkWidget *) menu_tool_button);
    gtk_widget_show_all((GtkWidget *) menu_tool_button);
    
    /* update editor */
    list =
      list_start = gtk_container_get_children((GtkContainer *) window->editor->machine_selector);

    while(list != NULL){
      if(AGS_IS_MACHINE_RADIO_BUTTON(list->data) &&
	 AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
	g_object_set(list->data,
		     "label\0", g_strconcat(G_OBJECT_TYPE_NAME(machine), ": \0", text, NULL),
		     NULL);

	break;
      }

      list = list->next;
    }
    
    g_list_free(list_start);

    /* update automation editor */
    list =
      list_start = gtk_container_get_children((GtkContainer *) window->automation_window->automation_editor->machine_selector);
    list = list->next;

    while(list != NULL){
      if(AGS_IS_MACHINE_RADIO_BUTTON(list->data) &&
	 AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
	g_object_set(list->data,
		     "label\0", g_strconcat(G_OBJECT_TYPE_NAME(machine), ": \0", text, NULL),
		     NULL);

	break;
      }

      list = list->next;
    }
    
    g_list_free(list_start);
  }
  
  machine->rename = NULL;
  gtk_widget_destroy(widget);

  return(0);
}

int
ags_machine_popup_properties_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  machine->properties = (GtkDialog *) ags_machine_editor_new(machine);
  g_signal_connect_after(machine->properties, "destroy\0",
			 G_CALLBACK(ags_machine_popup_properties_destroy_callback), machine);

  gtk_window_set_default_size((GtkWindow *) machine->properties, -1, 400);
  
  ags_connectable_connect(AGS_CONNECTABLE(machine->properties));

  ags_applicable_reset(AGS_APPLICABLE(machine->properties));

  gtk_widget_show_all((GtkWidget *) machine->properties);

  return(0);
}

int
ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine)
{
  machine->properties = NULL;
}

int
ags_machine_popup_copy_pattern_callback(GtkWidget *widget, AgsMachine *machine)
{
  ags_machine_copy_pattern(machine);

  return(0);
}

int
ags_machine_popup_paste_pattern_callback(GtkWidget *widget, AgsMachine *machine)
{
  //TODO:JK: implement me
  
  return(0);
}

int
ags_machine_popup_connection_editor_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsConnectionEditor *connection_editor;
  
  if(machine->connection_editor == NULL){
    connection_editor = ags_connection_editor_new(machine);
    machine->connection_editor = (GtkDialog *) connection_editor;
    
    g_signal_connect(connection_editor, "delete-event\0",
		     G_CALLBACK(ags_machine_connection_editor_delete_event_callback), machine);

    ags_connectable_connect(AGS_CONNECTABLE(connection_editor));
    ags_applicable_reset(AGS_APPLICABLE(connection_editor));

    gtk_widget_show_all((GtkWidget *) connection_editor);
  }else{
    connection_editor = (AgsConnectionEditor *) machine->connection_editor;
  }

  gtk_widget_show_all((GtkWidget *) connection_editor);
  
  return(0);
}

void
ags_machine_connection_editor_delete_event_callback(GtkWidget *dialog, gint response, AgsMachine *machine)
{
  machine->connection_editor = NULL;
}

int
ags_machine_popup_midi_dialog_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsMidiDialog *midi_dialog;
  
  if(machine->midi_dialog == NULL){
    midi_dialog = ags_midi_dialog_new(machine);
    machine->midi_dialog = (GtkDialog *) midi_dialog;
    midi_dialog->flags |= (AGS_MIDI_DIALOG_IO_OPTIONS |
			   AGS_MIDI_DIALOG_MAPPING |
			   AGS_MIDI_DIALOG_DEVICE);
    
    g_signal_connect(midi_dialog, "delete-event\0",
		     G_CALLBACK(ags_machine_midi_dialog_delete_event_callback), machine);

    ags_connectable_connect(AGS_CONNECTABLE(midi_dialog));
    ags_applicable_reset(AGS_APPLICABLE(midi_dialog));

    gtk_widget_show_all((GtkWidget *) midi_dialog);
  }else{
    midi_dialog = (AgsMidiDialog *) machine->midi_dialog;
  }

  gtk_widget_show_all((GtkWidget *) midi_dialog);
  
  return(0);
}

void
ags_machine_midi_dialog_delete_event_callback(GtkWidget *dialog, gint response, AgsMachine *machine)
{
  machine->midi_dialog = NULL;
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
    overwrite = g_object_get_data(G_OBJECT(dialog), "overwrite\0");
    create = g_object_get_data(G_OBJECT(dialog), "create\0");

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
    overwrite = g_object_get_data((GObject *) widget, "overwrite\0");
    create = g_object_get_data((GObject *) widget, "create\0");

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
			      ".sf2\0")){
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
	  
	  current_directory = g_dir_open(current_folder,
					 0,
					 &error);
	  
	  while((current_filename = (gchar *) g_dir_read_name(current_directory)) != NULL){
	    if(!g_strcmp0(".\0", current_filename) ||
	       !g_strcmp0("..\0", current_filename))
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

    g_message("machine: on\0");

    machine->flags |= AGS_MACHINE_BLOCK_PLAY;

    ags_machine_set_run_extended(machine,
				 TRUE,
				 TRUE, FALSE);

    machine->flags &= (~AGS_MACHINE_BLOCK_PLAY);
  }else{
    if((AGS_MACHINE_BLOCK_STOP & (machine->flags)) != 0){
      
      return;
    }

    g_message("machine: off\0");

    machine->flags |= AGS_MACHINE_BLOCK_STOP;

    ags_machine_set_run_extended(machine,
				 FALSE,
				 TRUE, FALSE);

    machine->flags &= (~AGS_MACHINE_BLOCK_STOP);
  }
}

void
ags_machine_set_audio_channels_callback(AgsAudio *audio,
					guint audio_channels, guint audio_channels_old,
					AgsMachine *machine)
{
  AgsWindow *window;

  GList *pad_list;
  GList *line_list;
  
  guint i;
  
  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);

  /* resize */
  ags_machine_resize_audio_channels(machine,
				    audio_channels, audio_channels_old);

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

  gdk_threads_leave();
}

void
ags_machine_set_pads_callback(AgsAudio *audio, GType channel_type,
			      guint pads, guint pads_old,
			      AgsMachine *machine)
{
  AgsWindow *window;

  GList *pad_list;
  
  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);

  /* resize */
  ags_machine_resize_pads(machine,
			  channel_type,
			  pads, pads_old);

  if((AGS_MACHINE_CONNECTED & (machine->flags)) != 0){
    if(pads > pads_old){
      if(g_type_is_a(channel_type,
		     AGS_TYPE_INPUT)){
	pad_list = gtk_container_get_children(GTK_CONTAINER(machine->input));
	pad_list = g_list_nth(pad_list,
			      pads_old);
      
	while(pad_list != NULL){
	  ags_connectable_connect(AGS_CONNECTABLE(pad_list->data));
	
	  pad_list = pad_list->next;
	}
      }

      if(g_type_is_a(channel_type,
		     AGS_TYPE_OUTPUT)){
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

  gdk_threads_leave();
}

void
ags_machine_tact_callback(AgsAudio *audio,
			  AgsRecallID *recall_id,
			  AgsMachine *machine)
{
  /* empty */
}

void
ags_machine_done_callback(AgsAudio *audio,
			  AgsRecallID *recall_id,
			  AgsMachine *machine)
{
  AgsChannel *channel;

  if((AGS_MACHINE_BLOCK_STOP & (machine->flags)) != 0){
    return;
  }

  gdk_threads_enter();
  
  machine->flags |= AGS_MACHINE_BLOCK_STOP;

  if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) != 0){
    gtk_toggle_button_set_active(machine->play, FALSE);
  }
  
  machine->flags &= (~AGS_MACHINE_BLOCK_STOP);

  gdk_threads_leave();
}

void
ags_machine_start_complete_callback(AgsTaskCompletion *task_completion,
				    AgsMachine *machine)
{
  AgsWindow *window;
  GtkMessageDialog *dialog;
  
  AgsSoundcardThread *soundcard_thread;

  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  gdk_threads_enter();

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) machine,
						 AGS_TYPE_WINDOW);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsAudioLoop *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get soundcard thread */
  soundcard_thread = (AgsSoundcardThread *) ags_thread_find_type((AgsThread *) audio_loop,
								 AGS_TYPE_SOUNDCARD_THREAD);

  if(soundcard_thread->error != NULL){
    /* show error message */
    dialog = (GtkMessageDialog *) gtk_message_dialog_new(GTK_WINDOW(window),
							 GTK_DIALOG_DESTROY_WITH_PARENT,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 "Error: %s\0", soundcard_thread->error->message);
    g_signal_connect(dialog, "response\0",
		     G_CALLBACK(ags_machine_start_complete_response), machine);
    gtk_widget_show_all((GtkWidget *) dialog);
  }

  gdk_threads_leave();
}

void
ags_machine_start_complete_response(GtkWidget *dialog,
				    gint response,
				    AgsMachine *machine)
{
  gtk_widget_destroy(dialog);
}
