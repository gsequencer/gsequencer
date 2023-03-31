/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/ags_machine_editor_bulk_callbacks.h>

#include <ags/app/ags_machine_editor.h>
#include <ags/app/ags_machine_editor_collection.h>

void
ags_machine_editor_bulk_remove_bulk_callback(GtkButton *button,
					     AgsMachineEditorBulk *machine_editor_bulk)
{
  AgsMachine *machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;

  xmlNode *node;
  
  GList *start_dialog_model, *dialog_model;
  GList *start_list;
  
  gint i, i_stop;

  machine_editor_collection = (AgsMachineEditorCollection *) gtk_widget_get_ancestor((GtkWidget *) machine_editor_bulk,
										     AGS_TYPE_MACHINE_EDITOR_COLLECTION);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *)machine_editor_bulk,
								AGS_TYPE_MACHINE_EDITOR);

  machine = machine_editor->machine;

  start_list = ags_machine_editor_collection_get_bulk(machine_editor_collection);

  i_stop = g_list_index(start_list,
			machine_editor_bulk);
  
  dialog_model =
    start_dialog_model = ags_machine_get_dialog_model(machine);
  
  node = NULL;

  for(i = 0; dialog_model != NULL && i <= i_stop; i++){
    if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-machine-editor-bulk",
						   "direction",
						   "output");
    }else{
      dialog_model = ags_machine_find_dialog_model(machine,
						   dialog_model,
						   "ags-machine-editor-bulk",
						   "direction",
						   "input");
    }

    if(dialog_model != NULL){
      if(i == i_stop){
	node = dialog_model->data;

	ags_machine_remove_dialog_model(machine,
					node);
      }
    
      dialog_model = dialog_model->next;
    }
  }
  
  ags_machine_editor_collection_remove_bulk(machine_editor_collection,
					    machine_editor_bulk);

  g_list_free(start_dialog_model);
  
  g_list_free(start_list);
}

void
ags_machine_editor_bulk_link_callback(GtkComboBox *combo_box, AgsMachineEditorBulk *machine_editor_bulk)
{
  AgsMachine *machine;
  AgsMachine *link_machine;
  AgsMachineEditor *machine_editor;
  AgsMachineEditorCollection *machine_editor_collection;

  GtkTreeModel *model;

  GtkTreeIter iter;

  guint output_lines, input_lines;  
  guint max_count;

  machine_editor_collection = (AgsMachineEditorCollection *) gtk_widget_get_ancestor((GtkWidget *) machine_editor_bulk,
										     AGS_TYPE_MACHINE_EDITOR_COLLECTION);

  machine_editor = (AgsMachineEditor *) gtk_widget_get_ancestor((GtkWidget *) machine_editor_bulk,
								AGS_TYPE_MACHINE_EDITOR);

  machine = machine_editor->machine;

  model = gtk_combo_box_get_model(machine_editor_bulk->link);  

  gtk_combo_box_get_active_iter(machine_editor_bulk->link,
				&iter);
  
  gtk_combo_box_get_active_iter(machine_editor_bulk->link,
				&iter);

  link_machine = NULL;

  gtk_tree_model_get(model,
		     &iter,
		     1, &link_machine,
		     -1);

  if(g_type_is_a(machine_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    output_lines = ags_audio_get_output_lines(machine->audio);

    input_lines = 0;
    
    if(link_machine != NULL){
      input_lines = ags_audio_get_input_lines(link_machine->audio);
    }
    
    max_count = 0;
    
    if(output_lines < input_lines){
      max_count = output_lines;
    }else{
      max_count = input_lines;
    }

    gtk_spin_button_set_range(machine_editor_bulk->first_line,
			      0.0,
			      (gdouble) output_lines);

    gtk_spin_button_set_range(machine_editor_bulk->first_link_line,
			      0.0,
			      (gdouble) input_lines);
        
    gtk_spin_button_set_range(machine_editor_bulk->count,
			      0.0,
			      (gdouble) max_count);
  }else{
    output_lines = 0;

    if(link_machine != NULL){
      output_lines = ags_audio_get_output_lines(link_machine->audio);
    }
    
    input_lines = ags_audio_get_input_lines(machine->audio);

    max_count = 0;
    
    if(input_lines < output_lines){
      max_count = input_lines;
    }else{
      max_count = output_lines;
    }

    gtk_spin_button_set_range(machine_editor_bulk->first_line,
			      0.0,
			      (gdouble) input_lines);

    gtk_spin_button_set_range(machine_editor_bulk->first_link_line,
			      0.0,
			      (gdouble) output_lines);
        
    gtk_spin_button_set_range(machine_editor_bulk->count,
			      0.0,
			      (gdouble) max_count);
  }
}
