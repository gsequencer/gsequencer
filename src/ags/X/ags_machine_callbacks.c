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

#include <ags/X/ags_machine_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/audio/task/ags_remove_audio.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine_editor.h>

#include <ags/X/editor/ags_file_selection.h>

int ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);

#define AGS_RENAME_ENTRY "AgsRenameEntry"


int
ags_machine_button_press_callback(GtkWidget *handle_box, GdkEventButton *event, AgsMachine *machine)
{
  AgsWindow *window = AGS_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET(handle_box)));

  if(event->button == 3)
    gtk_menu_popup (GTK_MENU (machine->popup),
                    NULL, NULL, NULL, NULL,
                    event->button, event->time);
  else if(event->button == 1)
    window->selected = machine;

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
ags_machine_popup_destroy_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsWindow *window;
  AgsRemoveAudio *remove_audio;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) machine);

  remove_audio = ags_remove_audio_new(window->devout,
				      machine->audio);
  ags_devout_append_task(window->devout,
			 AGS_TASK(remove_audio));

  ags_connectable_disconnect(AGS_CONNECTABLE(machine));
  gtk_widget_destroy((GtkWidget *) machine);
}

int
ags_machine_popup_rename_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GtkDialog *dialog;
  GtkEntry *entry;

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
    if(machine->name != NULL)
      free(machine->name);

    text = gtk_editable_get_chars(GTK_EDITABLE(gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox)->data), 0, -1);
    machine->name = text;

    gtk_frame_set_label((GtkFrame *) gtk_container_get_children((GtkContainer *) machine)->data, g_strconcat(G_OBJECT_TYPE_NAME(machine), ": \0", text, NULL));
    g_free(text);
  }

  gtk_widget_destroy(widget);

  return(0);
}

int
ags_machine_popup_properties_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  machine->properties = (GtkDialog *) ags_machine_editor_new(machine);
  gtk_window_set_default_size((GtkWindow *) machine->properties, -1, 400);
  ags_connectable_connect(AGS_CONNECTABLE(machine->properties));
  ags_applicable_reset(AGS_APPLICABLE(machine->properties));
  gtk_widget_show_all((GtkWidget *) machine->properties);

  return(0);
}

void
ags_machine_open_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *overwrite;
  GtkCheckButton *create;
  GSList *filenames;

  file_chooser = (GtkFileChooserDialog *) gtk_widget_get_toplevel(widget);

  if(response == GTK_RESPONSE_ACCEPT){
    filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
    overwrite = g_object_get_data((GObject *) widget, "overwrite\0");
    create = g_object_get_data((GObject *) widget, "create\0");

    ags_machine_open_files(machine,
			   filenames,
			   GTK_TOGGLE_BUTTON(overwrite)->active,
			   GTK_TOGGLE_BUTTON(create)->active);
  }

  gtk_widget_destroy((GtkWidget *) file_chooser);
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

	    if(!ags_file_selection_contains_file(file_selection, current_filename) &&
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

    ags_machine_open_files(machine,
			   filenames,
			   overwrite->toggle_button.active,
			   create->toggle_button.active);
  }
}
