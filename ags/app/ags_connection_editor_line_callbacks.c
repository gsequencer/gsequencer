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

#include <ags/app/ags_connection_editor_line_callbacks.h>

void
ags_connection_editor_line_output_soundcard_callback(GtkComboBox *combo_box, AgsConnectionEditorLine *connection_editor_line)
{
  GtkTreeModel *model;

  GObject *output_soundcard;

  GtkTreeIter iter;

  guint pcm_channels;

  output_soundcard = NULL;
  
  model = gtk_combo_box_get_model(connection_editor_line->output_soundcard);

  if(gtk_combo_box_get_active_iter(connection_editor_line->output_soundcard,
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

  gtk_spin_button_set_range(connection_editor_line->output_line,
			    0.0,
			    (gdouble) pcm_channels - 1.0);
}

void
ags_connection_editor_line_input_soundcard_callback(GtkComboBox *combo_box, AgsConnectionEditorLine *connection_editor_line)
{
  GtkTreeModel *model;

  GObject *input_soundcard;

  GtkTreeIter iter;

  guint pcm_channels;

  input_soundcard = NULL;
  
  model = gtk_combo_box_get_model(connection_editor_line->input_soundcard);

  if(gtk_combo_box_get_active_iter(connection_editor_line->input_soundcard,
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

  gtk_spin_button_set_range(connection_editor_line->input_line,
			    0.0,
			    (gdouble) pcm_channels - 1.0);
}
