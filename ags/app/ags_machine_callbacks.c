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

#include <ags/app/ags_machine_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_pad.h>
#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_midi_dialog.h>
#include <ags/app/ags_input_dialog.h>
#include <ags/app/ags_machine_editor_dialog.h>
#include <ags/app/ags_connection_editor_dialog.h>

#include <ags/app/export/ags_wave_export_dialog.h>

#include <ags/app/editor/ags_envelope_dialog.h>
#include <ags/app/editor/ags_machine_radio_button.h>

#include <ags/app/machine/ags_panel.h>
#include <ags/app/machine/ags_drum.h>
#include <ags/app/machine/ags_matrix.h>
#include <ags/app/machine/ags_audiorec.h>

#include <ags/i18n.h>

void ags_machine_recall_set_loop(AgsMachine *machine,
				 AgsRecall *recall);

void ags_machine_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
void ags_machine_rename_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
void ags_machine_reposition_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
void ags_machine_editor_dialog_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
void ags_connection_editor_dialog_response_callback(GtkWidget *widget, gint response, AgsMachine *machine);
int ags_machine_popup_properties_destroy_callback(GtkWidget *widget, AgsMachine *machine);

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
		      gtk_check_button_get_active(navigation->loop));

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

void
ags_machine_move_up_callback(GAction *action, GVariant *parameter,
			     AgsMachine *machine)
{
  AgsWindow *window;
  AgsMachine *prev_machine;
  AgsMachineSelector *machine_selector;

  GList *start_list, *list;
  GList *start_machine_radio_button, *machine_radio_button;    
  GList *prev;
  
  gint position;
  gint popup_position;    
  gboolean has_machine_radio_button;
  gboolean move_radio_button;
  
  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  machine_selector = window->composite_editor->machine_selector;

  /* position in composite editor */
  prev_machine = NULL;
  
  has_machine_radio_button = FALSE;
  
  move_radio_button = FALSE;
  
  list = 
    start_list = ags_window_get_machine(window);

  popup_position = g_list_index(start_list,
				machine);

  start_machine_radio_button = ags_machine_selector_get_machine_radio_button(machine_selector);

  machine_radio_button = start_machine_radio_button;

  while(!has_machine_radio_button && machine_radio_button != NULL){
    if(AGS_MACHINE_RADIO_BUTTON(machine_radio_button->data)->machine == machine){
      GList *iter;
      
      has_machine_radio_button = TRUE;

      iter = g_list_find(start_list,
			 machine);
      
      if(iter->prev != NULL){
	prev_machine = iter->prev->data;
      }
    }
      
    machine_radio_button = machine_radio_button->next;
  }

  machine_radio_button = start_machine_radio_button;

  while(!move_radio_button && machine_radio_button != NULL){
    if(AGS_MACHINE_RADIO_BUTTON(machine_radio_button->data)->machine == prev_machine){
      move_radio_button = TRUE;
    }
      
    machine_radio_button = machine_radio_button->next;
  }
  
  position = 0;
  
  while(list != NULL){
    gboolean success;

    if(list->data == machine){
      break;
    }

    success = FALSE;

    machine_radio_button = start_machine_radio_button;

    while(!success && machine_radio_button != NULL){
      if(AGS_MACHINE_RADIO_BUTTON(machine_radio_button->data)->machine == list->data){
	success = TRUE;
      }
      
      machine_radio_button = machine_radio_button->next;
    }
    
    if(success){
      position++;
    }

    list = list->next;
  }

  g_list_free(start_list);
  g_list_free(start_machine_radio_button);    

  /* composite editor */
  start_list = ags_window_get_machine(window);
  
  if(start_list != NULL &&
     start_list->data != machine){
    GAction *action;
    
    gchar *action_name;
    
    ags_machine_selector_popup_remove_machine(machine_selector,
					      popup_position);
    
    ags_machine_selector_popup_insert_machine(machine_selector,
					      popup_position - 1,
					      machine);    
  
    if(move_radio_button && position > 0){
      ags_machine_selector_remove_index(machine_selector,
					position);    

      ags_machine_selector_insert_index(machine_selector,
					position - 1,
					machine);
    }

    action_name = g_strdup_printf("add-%s",
				  machine->uid);
    
    action = g_action_map_lookup_action(machine_selector->action_group,
					action_name);
    g_object_set(action,
		 "state", g_variant_new_boolean(has_machine_radio_button),
		 NULL);

    g_free(action_name);
  }

  g_list_free(start_list);

  /* window */
  start_list = ags_window_get_machine(window);

  list = g_list_find(start_list,
		     machine);

  if(list->prev != NULL && list->prev->prev != NULL){
    gtk_box_reorder_child_after(window->machine_box,
				machine,
				list->prev->prev->data);

    prev = list->prev;
    
    start_list = g_list_remove(start_list,
			       machine);

    start_list = g_list_insert_before(start_list,
				      prev,
				      machine);
    
    g_list_free(window->machine);

    window->machine = g_list_reverse(start_list);
  }else if(list->prev != NULL){    
    gtk_box_reorder_child_after(window->machine_box,
				machine,
				NULL);

    start_list = g_list_remove(start_list,
			       machine);

    start_list = g_list_prepend(start_list,
				machine);
    
    g_list_free(window->machine);

    window->machine = g_list_reverse(start_list);
  }
}

void
ags_machine_move_down_callback(GAction *action, GVariant *parameter,
			       AgsMachine *machine)
{
  AgsWindow *window;
  AgsMachine *next_machine;
  AgsMachineSelector *machine_selector;
  
  GList *start_list, *list;
  GList *start_machine_radio_button, *machine_radio_button;
  GList *next_next;

  gint position;
  gint popup_position;
  gboolean has_machine_radio_button;
  gboolean move_radio_button;
  
  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  machine_selector = window->composite_editor->machine_selector;

  /* position composite editor */
  next_machine = NULL;
  
  move_radio_button = FALSE;
  
  has_machine_radio_button = FALSE;
  
  list = 
    start_list = ags_window_get_machine(window);

  popup_position = g_list_index(start_list,
				machine);

  start_machine_radio_button = ags_machine_selector_get_machine_radio_button(machine_selector);

  machine_radio_button = start_machine_radio_button;

  while(!has_machine_radio_button && machine_radio_button != NULL){
    if(AGS_MACHINE_RADIO_BUTTON(machine_radio_button->data)->machine == machine){
      GList *iter;
      
      has_machine_radio_button = TRUE;

      iter = g_list_find(start_list,
			 machine);
      
      if(iter->next != NULL){
	next_machine = iter->next->data;
      }
    }
      
    machine_radio_button = machine_radio_button->next;
  }

  machine_radio_button = start_machine_radio_button;

  while(!move_radio_button && machine_radio_button != NULL){
    if(AGS_MACHINE_RADIO_BUTTON(machine_radio_button->data)->machine == next_machine){
      move_radio_button = TRUE;
    }
      
    machine_radio_button = machine_radio_button->next;
  }

  position = 0;
  
  while(list != NULL){
    gboolean success;

    if(list->data == machine){
      break;
    }

    success = FALSE;

    machine_radio_button = start_machine_radio_button;

    while(!success && machine_radio_button != NULL){
      if(AGS_MACHINE_RADIO_BUTTON(machine_radio_button->data)->machine == list->data){
	success = TRUE;
      }
      
      machine_radio_button = machine_radio_button->next;
    }
    
    if(success){
      position++;
    }
    
    list = list->next;
  }

  g_list_free(start_list);
  g_list_free(start_machine_radio_button);    
  
  /* composite editor */
  start_list = ags_window_get_machine(window);
  
  if(start_list != NULL &&
     g_list_last(start_list)->data != machine){
    GAction *action;
    
    gchar *action_name;
    
    ags_machine_selector_popup_remove_machine(machine_selector,
					      popup_position);
    
    ags_machine_selector_popup_insert_machine(machine_selector,
					      popup_position + 1,
					      machine);
  
    if(has_machine_radio_button && position + 1 <= g_list_length(machine_selector->machine_radio_button)){
      ags_machine_selector_remove_index(machine_selector,
					position);    

      ags_machine_selector_insert_index(machine_selector,
					position + 1,
					machine);
    }

    action_name = g_strdup_printf("add-%s",
				  machine->uid);
    
    action = g_action_map_lookup_action(machine_selector->action_group,
					action_name);
    g_object_set(action,
		 "state", g_variant_new_boolean(has_machine_radio_button),
		 NULL);

    g_free(action_name);
  }

  g_list_free(start_list);

  /* window */
  start_list = ags_window_get_machine(window);

  list = g_list_find(start_list,
		     machine);

  if(list->next != NULL){
    gtk_box_reorder_child_after(window->machine_box,
				machine,
				list->next->data);

    next_next = list->next->next;
    
    start_list = g_list_remove(start_list,
			       machine);

    if(next_next != NULL){
      start_list = g_list_insert_before(start_list,
					next_next,
					machine);
    }else{
      start_list = g_list_append(start_list,
				 machine);
    }
    
    g_list_free(window->machine);

    window->machine = g_list_reverse(start_list);
  }
}

void
ags_machine_hide_callback(GAction *action, GVariant *parameter,
			  AgsMachine *machine)
{
  gtk_widget_hide(machine->frame);
}

void
ags_machine_show_callback(GAction *action, GVariant *parameter,
			  AgsMachine *machine)
{
  gtk_widget_show(machine->frame);
}

void
ags_machine_destroy_callback(GAction *action, GVariant *parameter,
			     AgsMachine *machine)
{
  AgsWindow *window;

  AgsApplicationContext *application_context;

  AgsAudio *audio;  

  AgsRemoveAudio *remove_audio;

  GList *start_list, *list;

  gint popup_position;
  
  application_context = ags_application_context_get_instance();

  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  ags_machine_set_run(machine,
		      FALSE);

  start_list = ags_window_get_machine(window);

  popup_position = g_list_index(start_list,
				machine);
  
  ags_machine_selector_popup_remove_machine(window->composite_editor->machine_selector,
					    popup_position);

  g_list_free(start_list);

  list = 
    start_list = ags_machine_selector_get_machine_radio_button(window->composite_editor->machine_selector);
    
  while(list != NULL){
    if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine){
      ags_machine_selector_remove_machine_radio_button(window->composite_editor->machine_selector,
						       list->data);
      
      break;
    }
    
    list = list->next;
  }

  g_list_free(start_list);
  
  /* destroy machine */
  audio = machine->audio;
  g_object_ref(audio);

  ags_connectable_disconnect(AGS_CONNECTABLE(machine));

  ags_window_remove_machine(window,
			    machine);

  /* get task thread */
  remove_audio = ags_remove_audio_new(audio);
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) remove_audio);
}

void
ags_machine_rename_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *text;

    /* get name */
    text = gtk_editable_get_chars(GTK_EDITABLE(AGS_INPUT_DIALOG(widget)->string_input),
				  0,
				  -1);

    ags_machine_set_machine_name(machine,
				 text);
  }

  machine->rename = NULL;
  
  gtk_window_destroy(widget);
}

void
ags_machine_rename_callback(GAction *action, GVariant *parameter,
			    AgsMachine *machine)
{
  GtkDialog *dialog;

  dialog = (GtkDialog *) ags_input_dialog_new(i18n("rename machine"),
					      (GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(machine),
										    AGS_TYPE_WINDOW));
  ags_input_dialog_set_flags(dialog,
			     AGS_INPUT_DIALOG_SHOW_STRING_INPUT);

  gtk_widget_show((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_machine_rename_response_callback), (gpointer) machine);
}

void
ags_machine_rename_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *text;

    /* get name */
    text = gtk_editable_get_chars(GTK_EDITABLE(AGS_INPUT_DIALOG(widget)->string_input),
				  0,
				  -1);

    g_object_set(machine->audio,
		 "name", text,
		 NULL);
  }

  machine->rename_audio = NULL;
  
  gtk_window_destroy(widget);
}

void
ags_machine_rename_audio_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  GtkDialog *dialog;

  dialog = (GtkDialog *) ags_input_dialog_new(i18n("rename audio"),
					      (GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(machine),
										    AGS_TYPE_WINDOW));
  ags_input_dialog_set_flags(dialog,
			     AGS_INPUT_DIALOG_SHOW_STRING_INPUT);

  gtk_widget_show((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_machine_rename_audio_response_callback), (gpointer) machine);
}

void
ags_machine_reposition_audio_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsAudio *audio;

    AgsApplicationContext *application_context;

    GList *start_list;
  
    gint new_position;
    
    application_context = ags_application_context_get_instance();
    
    audio = machine->audio;
    
    start_list = ags_sound_provider_get_audio(AGS_SOUND_PROVIDER(application_context));
    
    new_position = gtk_spin_button_get_value_as_int(AGS_INPUT_DIALOG(widget)->spin_button_input);
    
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
  }

  machine->reposition_audio = NULL;
  
  gtk_window_destroy(widget);
}

void
ags_machine_reposition_audio_callback(GAction *action, GVariant *parameter,
				      AgsMachine *machine)
{
  GtkDialog *dialog;

  dialog = (GtkDialog *) ags_input_dialog_new(i18n("reposition audio"),
					      (GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(machine),
										    AGS_TYPE_WINDOW));
  ags_input_dialog_set_flags(dialog,
			     AGS_INPUT_DIALOG_SHOW_SPIN_BUTTON_INPUT);

  gtk_label_set_text(AGS_INPUT_DIALOG(dialog)->spin_button_label,
		     i18n("position"));
  
  gtk_widget_show((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_machine_rename_audio_response_callback), (gpointer) machine);
}

void
ags_machine_editor_dialog_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    ags_applicable_apply(AGS_APPLICABLE(AGS_MACHINE_EDITOR_DIALOG(widget)->machine_editor));
  }

  machine->machine_editor_dialog = NULL;
  
  gtk_window_destroy(widget);
}

void
ags_machine_properties_callback(GAction *action, GVariant *parameter,
				AgsMachine *machine)
{
  AgsMachineEditorDialog *machine_editor_dialog;

  gchar *str;

  str = g_strdup_printf("%s:%s - %s",
			G_OBJECT_TYPE_NAME(machine),
			machine->machine_name,
			i18n("properties"));
  
  machine_editor_dialog =
    machine->machine_editor_dialog = ags_machine_editor_dialog_new(str,
								   (GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(machine),
													 AGS_TYPE_WINDOW));
  
  ags_machine_editor_set_machine(machine_editor_dialog->machine_editor,
				 machine);
  
  ags_applicable_reset(AGS_APPLICABLE(machine_editor_dialog->machine_editor));

  ags_connectable_connect(AGS_CONNECTABLE(machine_editor_dialog->machine_editor));
    
  gtk_widget_show(machine_editor_dialog);

  g_signal_connect(machine_editor_dialog, "response",
		   G_CALLBACK(ags_machine_editor_dialog_response_callback), machine);
  
  g_free(str);
}

void
ags_machine_sticky_controls_callback(GAction *action, GVariant *parameter,
				     AgsMachine *machine)
{
  GVariant *variant;

  variant = g_action_get_state(action);

  if(g_variant_get_boolean(variant)){
    g_object_set(action,
		 "state", g_variant_new_boolean(FALSE),
		 NULL);

    machine->flags &= (~AGS_MACHINE_STICKY_CONTROLS);
  }else{
    g_object_set(action,
		 "state", g_variant_new_boolean(TRUE),
		 NULL);

    machine->flags |= AGS_MACHINE_STICKY_CONTROLS;
  }
}

void
ags_machine_copy_pattern_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  ags_machine_copy_pattern(machine);
}

void
ags_machine_paste_pattern_callback(GAction *action, GVariant *parameter,
				   AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_envelope_callback(GAction *action, GVariant *parameter,
			      AgsMachine *machine)
{
  AgsWindow *window;
  AgsEnvelopeDialog *envelope_dialog;

  gchar *title;
  
  window = gtk_widget_get_ancestor(machine,
				   AGS_TYPE_WINDOW);

  title = g_strdup_printf("%s:%s %s",
			  G_OBJECT_TYPE_NAME(machine),
			  machine->machine_name,
			  i18n("envelope"));
  
  envelope_dialog = ags_envelope_dialog_new(title,
					    window,
					    machine);

  if(AGS_IS_DRUM(machine) ||
     AGS_IS_MATRIX(machine)){
    ags_envelope_dialog_add_pattern_tab(envelope_dialog);
  }
  
  gtk_widget_show((GtkWidget *) envelope_dialog);
  
  g_free(title);
}

void
ags_connection_editor_dialog_response_callback(GtkWidget *widget, gint response, AgsMachine *machine)
{
  if(response == GTK_RESPONSE_ACCEPT){
    ags_applicable_apply(AGS_APPLICABLE(AGS_CONNECTION_EDITOR_DIALOG(widget)->connection_editor));    
  }

  gtk_window_destroy(widget);
}

void
ags_machine_audio_connection_callback(GAction *action, GVariant *parameter,
				      AgsMachine *machine)
{
  AgsConnectionEditorDialog *connection_editor_dialog;

  gchar *str;

  if(!AGS_IS_PANEL(machine) &&
     !AGS_IS_AUDIOREC(machine)){
    return;
  }
  
  str = g_strdup_printf("%s:%s - %s",
			G_OBJECT_TYPE_NAME(machine),
			machine->machine_name,
			i18n("connections"));
  
  connection_editor_dialog =
    machine->connection_editor_dialog = ags_connection_editor_dialog_new(str,
									 (GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(machine),
													       AGS_TYPE_WINDOW));

  if(AGS_IS_PANEL(machine)){
    connection_editor_dialog->connection_editor->flags |= (AGS_CONNECTION_EDITOR_SHOW_OUTPUT |
							   AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_OUTPUT);
  }

  if(AGS_IS_AUDIOREC(machine)){
    connection_editor_dialog->connection_editor->flags |= (AGS_CONNECTION_EDITOR_SHOW_INPUT |
							   AGS_CONNECTION_EDITOR_SHOW_SOUNDCARD_INPUT);
  }
  
  ags_connection_editor_set_machine(connection_editor_dialog->connection_editor,
				    machine);
  
  ags_applicable_reset(AGS_APPLICABLE(connection_editor_dialog->connection_editor));

  ags_connectable_connect(AGS_CONNECTABLE(connection_editor_dialog->connection_editor));
  
  gtk_widget_show(connection_editor_dialog);

  g_signal_connect(connection_editor_dialog, "response",
		   G_CALLBACK(ags_connection_editor_dialog_response_callback), machine);
  
  g_free(str);
}

void
ags_machine_midi_connection_callback(GAction *action, GVariant *parameter,
				     AgsMachine *machine)
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

    gtk_widget_show((GtkWidget *) midi_dialog);
  }else{
    midi_dialog = (AgsMidiDialog *) machine->midi_dialog;
  }

  gtk_widget_show((GtkWidget *) midi_dialog);
}

void
ags_machine_audio_export_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  AgsWaveExportDialog *wave_export_dialog;
  
  if(machine->wave_export_dialog == NULL){
    wave_export_dialog = ags_wave_export_dialog_new(machine);
    machine->wave_export_dialog = (GtkDialog *) wave_export_dialog;

    ags_connectable_connect(AGS_CONNECTABLE(wave_export_dialog));
    ags_applicable_reset(AGS_APPLICABLE(wave_export_dialog));
  }else{
    wave_export_dialog = (AgsWaveExportDialog *) machine->wave_export_dialog;
  }

  gtk_widget_show((GtkWidget *) wave_export_dialog);
}

void
ags_machine_midi_export_callback(GAction *action, GVariant *parameter,
				 AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_audio_import_callback(GAction *action, GVariant *parameter,
				  AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_midi_import_callback(GAction *action, GVariant *parameter,
				 AgsMachine *machine)
{
  //TODO:JK: implement me
}

void
ags_machine_open_response_callback(GtkDialog *dialog, gint response, AgsMachine *machine)
{
  AgsPCMFileChooserDialog *pcm_file_chooser_dialog;

  pcm_file_chooser_dialog = AGS_PCM_FILE_CHOOSER_DIALOG(dialog);

  if(response == GTK_RESPONSE_ACCEPT){
    GListModel *file;
    
    GSList *filename;

    guint i, i_stop;
    
    file = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(pcm_file_chooser_dialog->file_chooser));

    filename = NULL;
    
    i_stop = g_list_model_get_n_items(file);
    
    for(i = 0; i < i_stop; i++){
      GFile *current_file;

      gchar *path;
      
      current_file = g_list_model_get_item(file,
					   i);

      path = g_file_get_path(current_file);

      g_message("%s", path);
      
      filename = g_slist_append(filename,
				path);
    }
    
    ags_machine_open_files(machine,
			   filename,
			   gtk_check_button_get_active(pcm_file_chooser_dialog->existing_channel),
			   gtk_check_button_get_active(pcm_file_chooser_dialog->new_channel));
  }

  gtk_window_destroy((GtkWindow *) pcm_file_chooser_dialog);
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

  GList *start_pad, *pad;
  GList *start_line, *line;
  
  guint i;

  audio = machine->audio;
  
  /* resize */
  if((AGS_CONNECTABLE_CONNECTED & (machine->connectable_flags)) != 0){
    if(audio_channels > audio_channels_old){
      if(machine->input_pad != NULL){
	pad =
	  start_pad = ags_machine_get_input_pad(machine);
      
	while(pad != NULL){
	  start_line = ags_pad_get_line(AGS_PAD(pad->data));
	  line = g_list_nth(start_line,
			    audio_channels_old);
	
	  for(i = 0; i < audio_channels - audio_channels_old; i++){
	    ags_connectable_connect(AGS_CONNECTABLE(line->data));

	    line = line->next;
	  }

	  g_list_free(start_line);
	
	  pad = pad->next;
	}

	g_list_free(start_pad);
      }

      if(machine->output_pad != NULL){
	pad =
	  start_pad = ags_machine_get_output_pad(machine);
      
	while(pad != NULL){
	  start_line = ags_pad_get_line(AGS_PAD(pad->data));
	  line = g_list_nth(start_line,
			    audio_channels_old);
	
	  for(i = 0; i < audio_channels - audio_channels_old; i++){
	    ags_connectable_connect(AGS_CONNECTABLE(line->data));

	    line = line->next;
	  }

	  g_list_free(start_line);
	  
	  pad = pad->next;
	}

	g_list_free(start_pad);
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

  GList *start_pad, *pad;

  guint audio_channels;
  guint i;
  
  /* resize */
  if((AGS_CONNECTABLE_CONNECTED & (machine->connectable_flags)) != 0){
    if(pads > pads_old){
      if(g_type_is_a(channel_type,
		     AGS_TYPE_INPUT)){
	if(machine->input_pad != NULL){
	  start_pad = ags_machine_get_input_pad(machine);
	  pad = g_list_nth(start_pad,
			   pads_old);
      
	  while(pad != NULL){
	    ags_connectable_connect(AGS_CONNECTABLE(pad->data));
	
	    pad = pad->next;
	  }
	}
      }

      if(g_type_is_a(channel_type,
		     AGS_TYPE_OUTPUT)){
	if(machine->output_pad != NULL){
	  start_pad = ags_machine_get_output_pad(machine);
	  pad = g_list_nth(pad,
			   pads_old);
      
	  while(pad != NULL){
	    ags_connectable_connect(AGS_CONNECTABLE(pad->data));
	
	    pad = pad->next;
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
