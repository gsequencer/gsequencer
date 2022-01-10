/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/machine/ags_sf2_synth_callbacks.h>

#include <ags/X/ags_window.h>

#include <math.h>

void ags_sf2_synth_open_dialog_response_callback(GtkWidget *widget, gint response,
						 AgsMachine *machine);

void
ags_sf2_synth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsSF2Synth *sf2_synth)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_SF2_SYNTH)->counter);

  g_object_set(AGS_MACHINE(sf2_synth),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_SF2_SYNTH);

  g_free(str);
}

void
ags_sf2_synth_destroy_callback(GtkWidget *widget, AgsSF2Synth *sf2_synth)
{
  if(sf2_synth->open_dialog != NULL){
    gtk_widget_destroy(sf2_synth->open_dialog);
  }
}

void
ags_sf2_synth_open_clicked_callback(GtkWidget *widget, AgsSF2Synth *sf2_synth)
{
  GtkFileChooserDialog *file_chooser;

  file_chooser = ags_machine_file_chooser_dialog_new(AGS_MACHINE(sf2_synth));
  gtk_file_chooser_add_shortcut_folder_uri(GTK_FILE_CHOOSER(file_chooser),
					   "file:///usr/share/sounds/sf2",
					   NULL);
  sf2_synth->open_dialog = (GtkWidget *) file_chooser;
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser),
				       FALSE);

  g_signal_connect((GObject *) file_chooser, "response",
		   G_CALLBACK(ags_sf2_synth_open_dialog_response_callback), AGS_MACHINE(sf2_synth));

  gtk_widget_show_all((GtkWidget *) file_chooser);
}

void
ags_sf2_synth_open_dialog_response_callback(GtkWidget *widget, gint response,
					   AgsMachine *machine)
{
  AgsSF2Synth *sf2_synth;

  sf2_synth = AGS_SF2_SYNTH(machine);

  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));

    gtk_entry_set_text(sf2_synth->filename,
		       filename);

    ags_sf2_synth_open_filename(sf2_synth,
				filename);
  }

  sf2_synth->open_dialog = NULL;
  gtk_widget_destroy(widget);
}

void
ags_sf2_synth_bank_tree_view_callback(GtkTreeView *tree_view,
				      GtkTreePath *path,
				      GtkTreeViewColumn *column,
				      AgsSF2Synth *sf2_synth)
{
  GtkTreeModel *bank_model;
  GtkTreeIter iter;

  gint bank;
  
  bank_model = gtk_tree_view_get_model(sf2_synth->bank_tree_view);

  if(gtk_tree_model_get_iter(bank_model, &iter, path)){
    gtk_tree_model_get(bank_model,
		       &iter,
		       0, &bank,
		       -1);

    ags_sf2_synth_load_bank(sf2_synth,
			    bank);
  }  
}

void
ags_sf2_synth_program_tree_view_callback(GtkTreeView *tree_view,
					 GtkTreePath *path,
					 GtkTreeViewColumn *column,
					 AgsSF2Synth *sf2_synth)
{
  GtkTreeModel *program_model;
  GtkTreeModel *bank_model;
  GtkTreeIter iter;

  gint bank;
  gboolean success;
  
  program_model = gtk_tree_view_get_model(sf2_synth->program_tree_view);

  bank = -1;

  success = FALSE;

#if 0  
  if(gtk_tree_model_get_iter(program_model, &iter, path)){
    gtk_tree_model_get(program_model,
		       &iter,
		       0, &bank,
		       -1);
  }

  bank_model = gtk_tree_view_get_model(sf2_synth->bank_tree_view);

  if(gtk_tree_model_get_iter_first(bank_model, &iter)){
    do{
      gint current_bank;
      
      gtk_tree_model_get(bank_model,
			 &iter,
			 0, &current_bank,
			 -1);

      if(current_bank == bank){
	success = TRUE;

	break;
      }
      
    }while(gtk_tree_model_iter_next(bank_model, &iter));
    
    if(success){
      gtk_tree_view_set_cursor(sf2_synth->bank_tree_view,
			       gtk_tree_model_get_path(bank_model, &iter),
			       gtk_tree_view_get_column(sf2_synth->bank_tree_view, 0),
			       FALSE);
    }
  }
#endif
}
