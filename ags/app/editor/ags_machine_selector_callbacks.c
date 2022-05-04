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

#include <ags/app/editor/ags_machine_selector_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_machine_selection.h>
#include <ags/app/editor/ags_machine_radio_button.h>
#include <ags/app/editor/ags_notation_edit.h>

void
ags_machine_selector_add_by_uuid_callback(GAction *action, GVariant *parameter,
					  AgsMachineSelector *machine_selector)
{
  AgsWindow *window;
  AgsMachine *selected_machine;
  
  GVariant *variant;

  GList *start_machine, *machine;
  GList *start_list, *list;

  gchar *action_name;
  gchar *uid;
    
  gint position;
  gboolean success;  

  window = gtk_widget_get_ancestor(machine_selector,
				   AGS_TYPE_WINDOW);

  machine = 
    start_machine = ags_window_get_machine(window);

  action_name = NULL;
  uid = NULL;
  
  g_object_get(G_SIMPLE_ACTION(action),
	       "name", &action_name,
	       NULL);

  if(action_name != NULL &&
     strlen(action_name) > 4){
    uid = action_name + 4;
  }
  
  selected_machine = NULL;

  while(machine != NULL){
    if(!g_strcmp0(AGS_MACHINE(machine->data)->uid,
		  uid)){
      selected_machine = machine->data;
      
      break;
    }
    
    machine = machine->next;
  }

  if(selected_machine == NULL){
    g_list_free(start_machine);
    
    return;
  }

  position = 0;

  machine = start_machine;
  start_list = ags_machine_selector_get_machine_radio_button(machine_selector);

  while(machine != NULL){
    if(machine->data == selected_machine){
      break;
    }
    
    list = start_list;

    success = FALSE;

    while(!success && list != NULL){
      if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == selected_machine){
	break;
      }
      
      if(AGS_MACHINE_RADIO_BUTTON(list->data)->machine == machine->data){
	success = TRUE;
      } 
      
      list = list->next;
    }
    
    if(success){
      position++;
    }
    
    machine = machine->next;
  }

  variant = g_action_get_state(action);

  if(g_variant_get_boolean(variant)){
    g_object_set(action,
		 "state", g_variant_new_boolean(FALSE),
		 NULL);

    ags_machine_selector_remove_index(machine_selector,
				      position);
  }else{
    g_object_set(action,
		 "state", g_variant_new_boolean(TRUE),
		 NULL);

    ags_machine_selector_insert_index(machine_selector,
				      position,
				      selected_machine);
  }
  
  g_list_free(start_machine);    
  g_list_free(start_list);    
}

void
ags_machine_selector_remove_index_callback(GAction *action, GVariant *parameter,
					   AgsMachineSelector *machine_selector)
{
  AgsMachineRadioButton *machine_radio_button;
  
  GList *start_list, *list;

  guint nth;
  
  /* find index */
  list =
    start_list = ags_machine_selector_get_machine_radio_button(machine_selector);

  nth = 0;
  
  while(list != NULL){
    if(gtk_check_button_get_active(GTK_CHECK_BUTTON(list->data))){
      ags_machine_selector_changed(machine_selector,
				   NULL);

      break;
    }

    list = list->next;
    nth++;
  }

  /* remove index */
  ags_machine_selector_remove_index(machine_selector,
				    nth);

  g_list_free(start_list);
}

void
ags_machine_selector_radio_changed(GtkWidget *radio_button, AgsMachineSelector *machine_selector)
{
  ags_machine_selector_changed(machine_selector, AGS_MACHINE_RADIO_BUTTON(radio_button)->machine);
}

void
ags_machine_selector_reverse_mapping_callback(GAction *action, GVariant *parameter,
					      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;

  GVariant *state;
  
  AgsApplicationContext *application_context;

  if((AGS_MACHINE_SELECTOR_BLOCK_REVERSE_MAPPING & (machine_selector->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

    
  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;
  
  if(machine != NULL){
    state = g_action_get_state(action);
    
    if(g_variant_get_boolean(state)){
      ags_audio_set_behaviour_flags(machine->audio,
				    AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING);
    }else{
      ags_audio_unset_behaviour_flags(machine->audio,
				      AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING);
    }

    g_variant_unref(state);
  }
}

void
ags_machine_selector_shift_a_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_A;
  base_key_code = 33;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_a_sharp_callback(GAction *action, GVariant *parameter,
					    AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_AIS;
  base_key_code = 34;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_h_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_H;
  base_key_code = 35;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_c_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_C;
  base_key_code = 24;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_c_sharp_callback(GAction *action, GVariant *parameter,
					    AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_CIS;
  base_key_code = 25;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_d_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_D;
  base_key_code = 26;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_d_sharp_callback(GAction *action, GVariant *parameter,
					    AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_DIS;
  base_key_code = 27;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_e_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_E;
  base_key_code = 28;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_f_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_F;
  base_key_code = 29;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_f_sharp_callback(GAction *action, GVariant *parameter,
					    AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_FIS;
  base_key_code = 30;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_g_callback(GAction *action, GVariant *parameter,
				      AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_G;
  base_key_code = 31;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}

void
ags_machine_selector_shift_g_sharp_callback(GAction *action, GVariant *parameter,
					    AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsPiano *piano;
  AgsNotationEdit *notation_edit;
    
  gchar *base_note;

  gint base_key_code;

  composite_editor = gtk_widget_get_ancestor(GTK_WIDGET(machine_selector),
					     AGS_TYPE_COMPOSITE_EDITOR);

  machine = composite_editor->selected_machine;

  notation_edit = composite_editor->notation_edit->edit;
    
  piano = AGS_SCROLLED_PIANO(composite_editor->notation_edit->edit_control)->piano;

  base_note = AGS_PIANO_KEYS_OCTAVE_2_GIS;
  base_key_code = 32;
  
  g_object_set(piano,
	       "base-note", base_note,
	       "base-key-code", base_key_code,
	       NULL);
  
  if(machine != NULL){
    g_free(machine->base_note);
    
    machine->base_note = g_strdup(base_note);
    machine->base_key_code = base_key_code;
  }
  
  gtk_widget_queue_draw((GtkWidget *) piano);
  gtk_widget_queue_draw((GtkWidget *) notation_edit);
}
