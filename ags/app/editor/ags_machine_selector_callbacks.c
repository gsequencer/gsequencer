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

void ags_machine_selector_selection_response(GtkWidget *machine_selection,
					     gint response,
					     AgsMachineSelector *machine_selector);

void
ags_machine_selector_add_index_callback(GAction *action, GVariant *parameter,
					AgsMachineSelector *machine_selector)
{
  ags_machine_selector_add_index(machine_selector);
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
    if(gtk_check_button_get_active(GTK_TOGGLE_BUTTON(list->data))){
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
ags_machine_selector_link_index_callback(GAction *action, GVariant *parameter,
					 AgsMachineSelector *machine_selector)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsMachineSelection *machine_selection;
  AgsMachineRadioButton *machine_radio_button;

  window = gtk_widget_get_ancestor(machine_selector,
				   AGS_TYPE_WINDOW);

  machine_selection = (AgsMachineSelection *) ags_machine_selection_new(window);
  machine_selector->machine_selection = (GtkDialog *) machine_selection;

  if((AGS_MACHINE_SELECTOR_EDIT_NOTATION & (machine_selector->edit)) != 0){
    ags_machine_selection_set_edit(machine_selection, AGS_MACHINE_SELECTION_EDIT_NOTATION);
  }

  if((AGS_MACHINE_SELECTOR_EDIT_AUTOMATION & (machine_selector->edit)) != 0){
    ags_machine_selection_set_edit(machine_selection, AGS_MACHINE_SELECTION_EDIT_AUTOMATION);
  }

  if((AGS_MACHINE_SELECTOR_EDIT_WAVE & (machine_selector->edit)) != 0){
    ags_machine_selection_set_edit(machine_selection, AGS_MACHINE_SELECTION_EDIT_WAVE);
  }
  
  ags_machine_selection_load_defaults(machine_selection);

  g_signal_connect(G_OBJECT(machine_selection), "response",
		   G_CALLBACK(ags_machine_selector_selection_response), machine_selector);

  gtk_widget_show((GtkWidget *) machine_selection);
}

void
ags_machine_selector_selection_response(GtkWidget *machine_selection,
					gint response,
					AgsMachineSelector *machine_selector)
{
  AgsMachine *machine;

  GList *start_list, *list;

  if(response == GTK_RESPONSE_ACCEPT){
    /* retrieve machine */
    machine = NULL;

    if(response == GTK_RESPONSE_ACCEPT){
      list =
	start_list = ags_machine_selection_get_radio_button(machine_selection);

      while(list != NULL){
	if(GTK_IS_CHECK_BUTTON(list->data) &&
	   gtk_check_button_get_active(GTK_CHECK_BUTTON(list->data))){
	  machine = g_object_get_data(list->data,
				      AGS_MACHINE_SELECTION_INDEX);
	  
	  break;
	}
	
	list = list->next;
      }

      g_list_free(start_list);
    }

    /* link index  */
    ags_machine_selector_link_index(machine_selector,
				    machine);
  }

  /* unset machine selection and destroy */
  machine_selector->machine_selection = NULL;

  gtk_window_destroy(machine_selection);
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
