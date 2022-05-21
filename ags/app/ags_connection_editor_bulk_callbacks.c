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

#include <ags/app/ags_connection_editor_bulk_callbacks.h>

#include <ags/app/ags_connection_editor.h>
#include <ags/app/ags_connection_editor_collection.h>

void
ags_connection_editor_bulk_remove_bulk_callback(GtkButton *button,
						AgsConnectionEditorBulk *connection_editor_bulk)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;

  connection_editor_collection = (AgsConnectionEditorCollection *) gtk_widget_get_ancestor(connection_editor_bulk,
											   AGS_TYPE_CONNECTION_EDITOR_COLLECTION);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_bulk,
								      AGS_TYPE_CONNECTION_EDITOR);

  machine = connection_editor->machine;
  
  ags_connection_editor_collection_remove_bulk(connection_editor_collection,
					       connection_editor_bulk);
}

void
ags_connection_editor_bulk_output_soundcard_callback(GtkComboBox *combo_box, AgsConnectionEditorBulk *connection_editor_bulk)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;

  GtkTreeModel *model;

  GObject *output_soundcard;

  GtkTreeIter iter;

  guint pcm_channels;
  gdouble first_line_upper;
  gdouble first_soundcard_line_upper;
  gdouble count_upper;

  connection_editor_collection = (AgsConnectionEditorCollection *) gtk_widget_get_ancestor(connection_editor_bulk,
											   AGS_TYPE_CONNECTION_EDITOR_COLLECTION);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_bulk,
								      AGS_TYPE_CONNECTION_EDITOR);

  machine = connection_editor->machine;

  if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    first_line_upper = ags_audio_get_output_lines(machine->audio);
  }else{
    first_line_upper = ags_audio_get_input_lines(machine->audio);
  }

  gtk_spin_button_set_range(connection_editor_bulk->output_first_line,
			    0.0,
			    first_line_upper - 1.0);
  
  output_soundcard = NULL;
  
  model = gtk_combo_box_get_model(connection_editor_bulk->output_soundcard);

  if(gtk_combo_box_get_active_iter(connection_editor_bulk->output_soundcard,
				   &iter)){
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
		       1, &output_soundcard,
		       -1);    
  }

  pcm_channels = 0;

  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    &pcm_channels,
			    NULL,
			    NULL,
			    NULL);  

  gtk_spin_button_set_range(connection_editor_bulk->output_first_soundcard_line,
			    0.0,
			    (gdouble) pcm_channels - 1.0);

  first_soundcard_line_upper = (gdouble) pcm_channels;

  count_upper = pcm_channels;

  if(count_upper > first_line_upper){
    count_upper = first_line_upper;
  }
  
  gtk_spin_button_set_range(connection_editor_bulk->output_count,
			    0.0,
			    count_upper);
}

void
ags_connection_editor_bulk_input_soundcard_callback(GtkComboBox *combo_box, AgsConnectionEditorBulk *connection_editor_bulk)
{
  AgsMachine *machine;
  AgsConnectionEditor *connection_editor;
  AgsConnectionEditorCollection *connection_editor_collection;

  GtkTreeModel *model;

  GObject *input_soundcard;

  GtkTreeIter iter;

  guint pcm_channels;
  gdouble first_line_upper;
  gdouble first_soundcard_line_upper;
  gdouble count_upper;

  connection_editor_collection = (AgsConnectionEditorCollection *) gtk_widget_get_ancestor(connection_editor_bulk,
											   AGS_TYPE_CONNECTION_EDITOR_COLLECTION);

  connection_editor = (AgsConnectionEditor *) gtk_widget_get_ancestor(connection_editor_bulk,
								      AGS_TYPE_CONNECTION_EDITOR);

  machine = connection_editor->machine;

  if(g_type_is_a(connection_editor_collection->channel_type, AGS_TYPE_OUTPUT)){
    first_line_upper = ags_audio_get_output_lines(machine->audio);
  }else{
    first_line_upper = ags_audio_get_input_lines(machine->audio);
  }

  gtk_spin_button_set_range(connection_editor_bulk->input_first_line,
			    0.0,
			    first_line_upper - 1.0);
  
  input_soundcard = NULL;
  
  model = gtk_combo_box_get_model(connection_editor_bulk->input_soundcard);

  if(gtk_combo_box_get_active_iter(connection_editor_bulk->input_soundcard,
				   &iter)){
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
		       1, &input_soundcard,
		       -1);    
  }

  pcm_channels = 0;

  ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
			    &pcm_channels,
			    NULL,
			    NULL,
			    NULL);  

  gtk_spin_button_set_range(connection_editor_bulk->input_first_soundcard_line,
			    0.0,
			    (gdouble) pcm_channels - 1.0);

  first_soundcard_line_upper = (gdouble) pcm_channels;

  count_upper = pcm_channels;

  if(count_upper > first_line_upper){
    count_upper = first_line_upper;
  }
  
  gtk_spin_button_set_range(connection_editor_bulk->input_count,
			    0.0,
			    count_upper);
}
