/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/editor/ags_pattern_envelope_callbacks.h>

#include <ags/i18n.h>

void
ags_pattern_envelope_edit_callback(GtkCellRendererToggle *cell_renderer,
				   gchar *path_str,
				   AgsPatternEnvelope *pattern_envelope)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreePath *path;

  gchar *str;
  
  gboolean do_edit;

  model = gtk_tree_view_get_model(pattern_envelope->tree_view);
  path = gtk_tree_path_new_from_string(path_str);
  
  /* get toggled iter */
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PLOT, &do_edit,
		     -1);

  /* do something with the value */
  do_edit ^= 1;

  /* set new value */
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PLOT, do_edit,
		     -1);

  /* clean up */
  gtk_tree_path_free(path);

  /* turn-off others */
  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      str = gtk_tree_model_get_string_from_iter(model,
						&iter);
      
      if(strcmp(path_str,
		str) != 0){
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			   AGS_PATTERN_ENVELOPE_COLUMN_PLOT, FALSE,
			   -1);
      }

      g_free(str);
    }while(gtk_tree_model_iter_next(model, &iter));
  }
}

void
ags_pattern_envelope_plot_callback(GtkCellRendererToggle *cell_renderer,
				   gchar *path_str,
				   AgsPatternEnvelope *pattern_envelope)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreePath *path;
  
  gboolean do_plot;

  model = gtk_tree_view_get_model(pattern_envelope->tree_view);
  path = gtk_tree_path_new_from_string(path_str);
  
  /* get toggled iter */
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PLOT, &do_plot,
		     -1);

  /* do something with the value */
  do_plot ^= 1;

  /* set new value */
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PLOT, do_plot,
		     -1);

  /* clean up */
  gtk_tree_path_free(path);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_preset_move_up_callback(GtkWidget *button,
					     AgsPatternEnvelope *pattern_envelope)
{
  //TODO:JK: implement me
}

void
ags_pattern_envelope_preset_move_down_callback(GtkWidget *button,
					       AgsPatternEnvelope *pattern_envelope)
{
  //TODO:JK: implement me
}

void
ags_pattern_envelope_preset_add_callback(GtkWidget *button,
					 AgsPatternEnvelope *pattern_envelope)
{
  GtkDialog *dialog;
  GtkEntry *entry;

  if(pattern_envelope->rename != NULL){
    return;
  }
  
  pattern_envelope->rename =
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("rename"),
						       (GtkWindow *) gtk_widget_get_toplevel(GTK_WIDGET(pattern_envelope)),
						       GTK_DIALOG_DESTROY_WITH_PARENT,
						       GTK_STOCK_OK,
						       GTK_RESPONSE_ACCEPT,
						       GTK_STOCK_CANCEL,
						       GTK_RESPONSE_REJECT,
						       NULL);

  entry = (GtkEntry *) gtk_entry_new();
  gtk_box_pack_start((GtkBox *) dialog->vbox,
		     (GtkWidget *) entry,
		     FALSE, FALSE,
		     0);

  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_pattern_envelope_preset_rename_response_callback), (gpointer) pattern_envelope);
}

void
ags_pattern_envelope_preset_remove_callback(GtkWidget *button,
					    AgsPatternEnvelope *pattern_envelope)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  gint nth;
  gboolean do_edit;
  
  model = gtk_tree_view_get_model(pattern_envelope->tree_view);

  nth = -1;
  do_edit = FALSE;

  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			 AGS_PATTERN_ENVELOPE_COLUMN_PLOT, &do_edit,
			 -1);

      nth++;
    }while(!do_edit &&
	   gtk_tree_model_iter_next(model, &iter));
  }
  
  /* remove preset */
  if(do_edit){
    ags_pattern_envelope_remove_preset(pattern_envelope,
				       nth);
  }
}

int
ags_pattern_envelope_preset_rename_response_callback(GtkWidget *widget, gint response,
						     AgsPatternEnvelope *pattern_envelope)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *text;

    /* get name */
    text = gtk_editable_get_chars(GTK_EDITABLE(gtk_container_get_children((GtkContainer *) GTK_DIALOG(widget)->vbox)->data),
				  0, -1);
    
    /* add preset */
    ags_pattern_envelope_add_preset(pattern_envelope,
				    text);
    
    /* load preset */
    ags_pattern_envelope_load_preset(pattern_envelope);    
  }
  
  pattern_envelope->rename = NULL;
  gtk_widget_destroy(widget);

  return(0);
}

