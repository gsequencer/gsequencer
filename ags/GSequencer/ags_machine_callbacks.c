/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/GSequencer/ags_machine_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_pad.h>
#include <ags/GSequencer/ags_automation_editor.h>
#include <ags/GSequencer/ags_notation_editor.h>
#include <ags/GSequencer/ags_machine_editor.h>
#include <ags/GSequencer/ags_connection_editor.h>
#include <ags/GSequencer/ags_midi_dialog.h>

#include <ags/GSequencer/export/ags_wave_export_dialog.h>

#include <ags/GSequencer/editor/ags_envelope_dialog.h>
#include <ags/GSequencer/editor/ags_machine_radio_button.h>

#include <ags/i18n.h>

#define AGS_RENAME_ENTRY "AgsRenameEntry"

void ags_machine_recall_set_loop(AgsMachine *machine,
				 AgsRecall *recall);

int ags_machine_popup_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_rename_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_reposition_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine);

void
ags_machine_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsMachine *machine)
{
  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;
  
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };
  
  if(old_parent != NULL){
    return;
  }

  audio = machine->audio;

  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);

  if(playback_domain != NULL){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsThread *audio_thread;
	  
      audio_thread = ags_playback_domain_get_audio_thread(playback_domain,
							  i);

      if(audio_thread != NULL){
	ags_audio_thread_set_do_fx_staging((AgsAudioThread *) audio_thread,
					   TRUE);
	ags_audio_thread_set_staging_program((AgsAudioThread *) audio_thread,
					     staging_program,
					     1);
	    
	g_object_unref(audio_thread);
      }
    }
    
    g_object_unref(playback_domain);
  }
}

void
ags_machine_check_message_callback(GObject *application_context, AgsMachine *machine)
{
  ags_machine_check_message(machine);
}

void
ags_machine_recall_set_loop(AgsMachine *machine,
			    AgsRecall *recall)
{
  AgsNavigation *navigation;
  AgsPort *port;

  AgsApplicationContext *application_context;

  GValue value = G_VALUE_INIT;

  application_context = ags_application_context_get_instance();

  navigation = ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));

  /* loop */
  port = NULL;
    
  g_object_get(recall,
	       "loop", &port,
	       NULL);

  g_value_init(&value,
	       G_TYPE_BOOLEAN);

  g_value_set_boolean(&value,
		      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

  ags_port_safe_write(port,
		      &value);

  if(port != NULL){
    g_object_unref(port);
  }
    
  /* loop start */
  port = NULL;
    
  g_object_get(recall,
	       "loop-start", &port,
	       NULL);

  g_value_unset(&value);
  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     16 * gtk_spin_button_get_value_as_int(navigation->loop_left_tact));

  ags_port_safe_write(port,
		      &value);

  if(port != NULL){
    g_object_unref(port);
  }
    
  /* loop end */
  port = NULL;
    
  g_object_get(recall,
	       "loop-end", &port,
	       NULL);

  g_value_unset(&value);
  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     16 * gtk_spin_button_get_value_as_int(navigation->loop_right_tact));

  ags_port_safe_write(port,
		      &value);

  if(port != NULL){
    g_object_unref(port);
  }
}

void
ags_machine_map_recall_callback(AgsMachine *machine,
				gpointer user_data)
{
  GList *start_play;
  GList *start_recall;
  GList *list;
  
  start_play = NULL;
  start_recall = NULL;
  
  g_object_get(machine->audio,
	       "play", &start_play,
	       "recall", &start_recall,
	       NULL);

  list = start_play;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }

  list = start_play;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }

  list = start_recall;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }
  
  list = start_recall;
  
  while((list = ags_recall_template_find_type(list, AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
    ags_machine_recall_set_loop(machine,
				list->data);

    /* iterate */
    list = list->next;
  }
  
  g_list_free_full(start_play,
		   (GDestroyNotify) g_object_unref);
  
  g_list_free_full(start_recall,
		   (GDestroyNotify) g_object_unref);
}

int
ags_machine_button_press_callback(GtkWidget *handle_box, GdkEventButton *event, AgsMachine *machine)
{
  if(event->button == 3){
    gtk_menu_popup_at_widget(GTK_MENU(machine->popup),
			     handle_box,
			     GDK_GRAVITY_SOUTH_EAST,
			     GDK_GRAVITY_NORTH_WEST,
			     NULL);
  }else if(event->button == 1){
    AgsWindow *window;

    AgsApplicationContext *application_context;

    application_context = ags_application_context_get_instance();
    
    window = ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
    window->selected = machine;
  }

  return(0);
}

void
ags_machine_popup_move_up_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GValue val = G_VALUE_INIT;

  g_value_init(&val,
	       G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(machine))),
				   GTK_WIDGET(machine),
				   "position", &val);

  if(g_value_get_int (&val) > 0){
    gtk_box_reorder_child(GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(machine))),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) - 1);
  }

  g_value_unset (&val);
}

void
ags_machine_popup_move_down_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GList *start_list;
  
  GValue val={0,};

  g_value_init (&val, G_TYPE_INT);

  gtk_container_child_get_property(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(machine))),
				   GTK_WIDGET(machine),
				   "position", &val);

  start_list = gtk_container_get_children((GtkContainer *) gtk_widget_get_parent(GTK_WIDGET(machine)));
  
  if(g_value_get_int (&val) < g_list_length(start_list) - 1){
    gtk_box_reorder_child(GTK_BOX(gtk_widget_get_parent(GTK_WIDGET(machine))),
			  GTK_WIDGET(machine),
			  g_value_get_int (&val) + 1);
  }

  g_value_unset (&val);

  g_list_free(start_list);
}

void
ags_machine_popup_hide_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GList *start_list;

  start_list = gtk_container_get_children((GtkContainer *) machine);
  
  gtk_widget_hide(gtk_bin_get_child(GTK_BIN(start_list->data)));

  g_list_free(start_list);
}

void
ags_machine_popup_show_activate_callback(GtkWidget *widget, AgsMachine *machine)
{
  GList *start_list;

  start_list = gtk_container_get_children((GtkContainer *) machine);  

  gtk_widget_show(gtk_bin_get_child(GTK_BIN(start_list->data)));

  g_list_free(start_list);
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

  ags_machine_set_run(machine,
		      FALSE);
  
  /* destroy editor */
  list =
    list_start = gtk_container_get_children((GtkContainer *) window->composite_editor->machine_selector);

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
  remove_audio = ags_remove_audio_new(audio);
  
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
						       "_OK", GTK_RESPONSE_ACCEPT,
						       "_Cancel", GTK_RESPONSE_REJECT,
						       NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(entry, machine->machine_name);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(dialog),
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

    children = gtk_container_get_children((GtkContainer *) gtk_dialog_get_content_area(GTK_DIALOG(widget)));
    
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
    
  if(machine->rename_audio != NULL){
    return;
  }

  audio = machine->audio;
  
  machine->rename_audio =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("rename audio"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(machine)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       "_OK", GTK_RESPONSE_ACCEPT,
						       "_Cancel", GTK_RESPONSE_REJECT,
						       NULL);

  g_object_get(audio,
	       "audio-name", &audio_name,
	       NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(entry, audio_name);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(dialog),
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

    children = gtk_container_get_children((GtkContainer *) gtk_dialog_get_content_area(GTK_DIALOG(widget)));
    
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
						       "_OK", GTK_RESPONSE_ACCEPT,
						       "_Cancel", GTK_RESPONSE_REJECT,
						       NULL);

  if(position >= 0){
    spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, (gdouble) (length - 1), 1.0);
  }else{
    spin_button = (GtkSpinButton *) gtk_spin_button_new_with_range(-1.0, -1.0, 0.0);
  }
  
  gtk_spin_button_set_value(spin_button,
			    (gdouble) position);
  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area(dialog),
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

    children = gtk_container_get_children((GtkContainer *) gtk_dialog_get_content_area(GTK_DIALOG(widget)));
    
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
ags_machine_popup_midi_export_callback(GtkWidget *widget, AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_popup_wave_export_callback(GtkWidget *widget, AgsMachine *machine)
{
  AgsWaveExportDialog *wave_export_dialog;
  
  if(machine->wave_export_dialog == NULL){
    wave_export_dialog = ags_wave_export_dialog_new(machine);
    machine->wave_export_dialog = (GtkDialog *) wave_export_dialog;

    ags_connectable_connect(AGS_CONNECTABLE(wave_export_dialog));
    ags_applicable_reset(AGS_APPLICABLE(wave_export_dialog));

    gtk_widget_show_all((GtkWidget *) wave_export_dialog);
  }else{
    wave_export_dialog = (AgsWaveExportDialog *) machine->wave_export_dialog;
  }

  gtk_widget_show_all((GtkWidget *) wave_export_dialog);
}

void
ags_machine_popup_midi_import_callback(GtkWidget *widget, AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_popup_wave_import_callback(GtkWidget *widget, AgsMachine *machine)
{
  //TODO:JK: implement me
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
			   gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(overwrite)),
			   gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(create)));
  }

  gtk_widget_destroy(GTK_WIDGET(file_chooser));
}

void
ags_machine_play_callback(GtkWidget *toggle_button, AgsMachine *machine)
{
  if(machine == NULL){
    return;
  }

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_button))){
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
  AgsAudio *audio;
  AgsPlayback *playback;
  AgsChannel *start_output;
  AgsChannel *start_input;
  AgsChannel *channel, *next_pad, *next_channel;

  GList *pad_list;
  GList *line_list;
  
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  audio = machine->audio;

  start_output = NULL;
  start_input = NULL;

  g_object_get(audio,
	       "output", &start_output,
	       "input", &start_input,
	       NULL);

  if(audio_channels > audio_channels_old){
    /* AgsOutput */
    channel = start_output;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_pad = NULL;
    
    while(channel != NULL){      
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);
      
      next_channel = ags_channel_nth(channel,
				     audio_channels_old);

      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
      
      while(channel != next_pad && channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(channel != NULL){
      g_object_unref(channel);
    }

    /* AgsInput */
    channel = start_input;

    if(channel != NULL){
      g_object_ref(channel);
    }

    next_pad = NULL;
    
    while(channel != NULL){      
      /* get some fields */
      next_pad = ags_channel_next_pad(channel);

      next_channel = ags_channel_nth(channel,
				     audio_channels_old);
      
      if(channel != NULL){
	g_object_unref(channel);
      }
      
      channel = next_channel;
      
      while(channel != next_pad && channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(next_pad != NULL){
	g_object_unref(next_pad);
      }
    }

    if(channel != NULL){
      g_object_unref(channel);
    }
  }
   
  /* unref */
  if(start_output != NULL){
    g_object_unref(start_output);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
  
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
ags_machine_resize_pads_callback(AgsMachine *machine,
				 GType channel_type,
				 guint pads, guint pads_old,
				 gpointer data)
{
  AgsAudio *audio;
  AgsPlayback *playback;
  AgsChannel *start_output;
  AgsChannel *start_input;
  AgsChannel *channel, *next_channel;

  GList *pad_list;

  guint audio_channels;
  guint i;

  static const guint staging_program[] = {
    (AGS_SOUND_STAGING_AUTOMATE | AGS_SOUND_STAGING_RUN_INTER | AGS_SOUND_STAGING_FX),
  };

  audio = machine->audio;

  start_output = NULL;
  start_input = NULL;
  
  audio_channels = 0;

  if(g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    if(pads > pads_old){
      /* get some fields */
      g_object_get(audio,
		   "input", &start_input,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(start_input,
				    pads_old);
      
      while(channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_input != NULL){
	g_object_unref(start_input);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
    }
  }else{
    if(pads > pads_old){
      /* get some fields */
      g_object_get(audio,
		   "output", &start_output,
		   "audio-channels", &audio_channels,
		   NULL);

      /* AgsOutput */
      channel = ags_channel_pad_nth(start_output,
				    pads_old);
      
      while(channel != NULL){
	/* fx engine */
	g_object_get(channel,
		     "playback", &playback,
		     NULL);

	if(playback != NULL){
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsThread *channel_thread;
	  
	    channel_thread = ags_playback_get_channel_thread(playback,
							     i);

	    if(channel_thread != NULL){
	      ags_channel_thread_set_do_fx_staging((AgsChannelThread *) channel_thread,
						   TRUE);
	      ags_channel_thread_set_staging_program((AgsChannelThread *) channel_thread,
						     staging_program,
						     1);
	    
	      g_object_unref(channel_thread);
	    }
	  }
	
	  g_object_unref(playback);
	}
	
	/* iterate */
	next_channel = ags_channel_next(channel);

	g_object_unref(channel);

	channel = next_channel;
      }

      if(start_output != NULL){
	g_object_unref(start_output);
      }

      if(channel != NULL){
	g_object_unref(channel);
      }
    }
  }
  
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
  AgsAudioSignal *template, *audio_signal; 
  AgsRecallID *recall_id;
  AgsNote *play_note;
  AgsFxPlaybackAudio *fx_playback_audio;
  
  GObject *output_soundcard;

  GList *start_recall, *recall;
  
  GRecMutex *recycling_mutex;

  audio = NULL;
  
  channel = NULL;

  first_recycling = NULL;
  last_recycling = NULL;
  
  play_note = NULL;

  fx_playback_audio = NULL;
  
  start_recall = NULL;
  
  g_object_get(playback,
	       "channel", &channel,
	       "play-note", &play_note,
	       NULL);

  g_object_get(channel,
	       "audio", &audio,
	       NULL);

  g_object_get(audio,
	       "play", &start_recall,
	       NULL);

  recall = ags_recall_template_find_type(start_recall, AGS_TYPE_FX_PLAYBACK_AUDIO);

  if(recall != NULL){
    fx_playback_audio = recall->data;
  }
  
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
    output_soundcard = NULL;
    
    g_object_get(recycling,
		 "output-soundcard", &output_soundcard,
		 NULL);

    recycling_mutex = AGS_RECYCLING_GET_OBJ_MUTEX(recycling);

    g_rec_mutex_lock(recycling_mutex);

    template = ags_audio_signal_get_template(recycling->audio_signal);
    
    g_rec_mutex_unlock(recycling_mutex);

    /* instantiate audio signal */
    audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					(GObject *) recycling,
					(GObject *) recall_id);
    ags_audio_signal_set_flags(audio_signal, (AGS_AUDIO_SIGNAL_FEED |
					      AGS_AUDIO_SIGNAL_STREAM));
    g_object_set(audio_signal,
		 "template", template,
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

    ags_fx_playback_audio_add_feed_audio_signal(fx_playback_audio, audio_signal);

    /*
     * emit add_audio_signal on AgsRecycling
     */
    ags_recycling_add_audio_signal(recycling,
				   audio_signal);

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
