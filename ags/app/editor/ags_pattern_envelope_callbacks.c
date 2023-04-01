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

#include <ags/app/editor/ags_pattern_envelope_callbacks.h>

#include <ags/app/ags_window.h>
#include <ags/app/ags_input_dialog.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_envelope_dialog.h>

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
		     AGS_PATTERN_ENVELOPE_COLUMN_EDIT, &do_edit,
		     -1);

  /* do something with the value */
  do_edit ^= 1;

  /* set new value */
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_EDIT, do_edit,
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
			   AGS_PATTERN_ENVELOPE_COLUMN_EDIT, FALSE,
			   -1);
      }

      g_free(str);
    }while(gtk_tree_model_iter_next(model, &iter));
  }

  /* reset control */
  ags_pattern_envelope_reset_control(pattern_envelope);
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
ags_pattern_envelope_audio_channel_start_callback(GtkWidget *spin_button,
						  AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  GValue value = G_VALUE_INIT;

  guint audio_channel_start;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  /* set property */
  audio_channel_start = gtk_spin_button_get_value_as_int((GtkSpinButton *) spin_button);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   audio_channel_start);

  ags_pattern_envelope_set_preset_property(pattern_envelope,
					   preset,
					   "audio-channel-start", &value);
}

void
ags_pattern_envelope_audio_channel_end_callback(GtkWidget *spin_button,
						AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;

  guint audio_channel_end;

  GValue value = G_VALUE_INIT;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  /* set property */
  audio_channel_end = gtk_spin_button_get_value_as_int((GtkSpinButton *) spin_button);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   audio_channel_end);

  ags_pattern_envelope_set_preset_property(pattern_envelope,
					   preset,
					   "audio-channel-end", &value);
}

void
ags_pattern_envelope_pad_start_callback(GtkWidget *spin_button,
					AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  guint pad_start;

  GValue value = G_VALUE_INIT;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  /* set property */
  pad_start = gtk_spin_button_get_value_as_int((GtkSpinButton *) spin_button);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   pad_start);

  ags_pattern_envelope_set_preset_property(pattern_envelope,
					   preset,
					   "pad-start", &value);
}

void
ags_pattern_envelope_pad_end_callback(GtkWidget *spin_button,
				      AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  guint pad_end;

  GValue value = G_VALUE_INIT;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  /* set property */
  pad_end = gtk_spin_button_get_value_as_int((GtkSpinButton *) spin_button);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   pad_end);

  ags_pattern_envelope_set_preset_property(pattern_envelope,
					   preset,
					   "pad-end", &value);
}

void
ags_pattern_envelope_x_start_callback(GtkWidget *spin_button,
				      AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  guint x_start;

  GValue value = G_VALUE_INIT;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  /* set property */
  x_start = gtk_spin_button_get_value_as_int((GtkSpinButton *) spin_button);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   x_start);

  ags_pattern_envelope_set_preset_property(pattern_envelope,
					   preset,
					   "x-start", &value);
}

void
ags_pattern_envelope_x_end_callback(GtkWidget *spin_button,
				    AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  guint x_end;

  GValue value = G_VALUE_INIT;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  /* set property */
  x_end = gtk_spin_button_get_value_as_int((GtkSpinButton *) spin_button);

  g_value_init(&value,
	       G_TYPE_UINT);
  g_value_set_uint(&value,
		   x_end);

  ags_pattern_envelope_set_preset_property(pattern_envelope,
					   preset,
					   "x-end", &value);
}

void
ags_pattern_envelope_attack_x_callback(GtkWidget *range,
				       AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble attack_x;

  GValue value = G_VALUE_INIT;

  GError *error;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  attack_x = gtk_range_get_value(GTK_RANGE(range));

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = attack_x;

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_attack_y_callback(GtkWidget *range,
				       AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble attack_y;

  GValue value = G_VALUE_INIT;

  GError *error;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  attack_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = attack_y;

  ags_preset_add_parameter(preset,
			   "attack", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_decay_x_callback(GtkWidget *range,
				      AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble decay_x;

  GValue value = G_VALUE_INIT;

  GError *error;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  decay_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = decay_x;

  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_decay_y_callback(GtkWidget *range,
				      AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble decay_y;

  GValue value = G_VALUE_INIT;

  GError *error;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  decay_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = decay_y;
  
  ags_preset_add_parameter(preset,
			   "decay", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_sustain_x_callback(GtkWidget *range,
					AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble sustain_x;

  GValue value = G_VALUE_INIT;

  GError *error;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  sustain_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = sustain_x;
  
  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_sustain_y_callback(GtkWidget *range,
					AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble sustain_y;

  GValue value = G_VALUE_INIT;

  GError *error;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  sustain_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = sustain_y;
  
  ags_preset_add_parameter(preset,
			   "sustain", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_release_x_callback(GtkWidget *range,
					AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble release_x;

  GValue value = G_VALUE_INIT;

  GError *error;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  release_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].real = release_x;
  
  ags_preset_add_parameter(preset,
			   "release", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_release_y_callback(GtkWidget *range,
					AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble release_y;

  GValue value = G_VALUE_INIT;

  GError *error;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  release_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = release_y;
  
  ags_preset_add_parameter(preset,
			   "release", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_ratio_callback(GtkWidget *range,
				    AgsPatternEnvelope *pattern_envelope)
{
  AgsPreset *preset;
  
  AgsComplex *val;  

  gdouble ratio;

  GValue value = G_VALUE_INIT;

  GError *error;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  /* get value and update preset */
  ratio = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "ratio", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    g_error_free(error);
    
    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0].imag = ratio;

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_preset_move_up_callback(GtkWidget *button,
					     AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsAudio *audio;
  AgsPreset *current;
  
  GList *start_preset;
  GList *preset, *prev;

  gchar *preset_name, *prev_name;
  
  gint nth;
  gboolean do_edit;

  GRecMutex *audio_mutex;

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* get model */
  model = gtk_tree_view_get_model(pattern_envelope->tree_view);
  
  /* get position */
  do_edit = FALSE;
  
  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 AGS_PATTERN_ENVELOPE_COLUMN_EDIT, &do_edit,
			 -1);
      
      if(do_edit){
	break;
      }
      
      nth++;
    }while(gtk_tree_model_iter_next(model, &iter));
  }

  /* move position */
  if(!do_edit ||
     nth == 0){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get prev and current preset name */
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, &preset_name,
		     -1);

  gtk_tree_model_iter_nth_child(model,
				&iter,
				NULL,
				nth - 1);
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, &prev_name,
		     -1);

  /* find preset */
  g_object_get(audio,
	       "preset", &start_preset,
	       NULL);

  preset = ags_preset_find_name(start_preset,
				preset_name);

  prev = ags_preset_find_name(start_preset,
			      prev_name);

  /* reorder list */
  current = preset->data;
  
  g_rec_mutex_lock(audio_mutex);

  audio->preset = g_list_delete_link(audio->preset,
				     preset);

  audio->preset = g_list_insert_before(audio->preset,
				       prev,
				       current);

  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_preset,
		   g_object_unref);
  
  /* load preset */
  ags_envelope_dialog_load_preset(envelope_dialog);    
}

void
ags_pattern_envelope_preset_move_down_callback(GtkWidget *button,
					       AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsAudio *audio;
  AgsPreset *current;
  
  GList *start_preset;
  GList *preset, *next;

  gchar *preset_name, *next_name;
  
  guint nth;
  gboolean do_edit;

  GRecMutex *audio_mutex;

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* get model */
  model = gtk_tree_view_get_model(pattern_envelope->tree_view);
  
  /* get position */
  nth = 0;
  do_edit = FALSE;
  
  if(gtk_tree_model_get_iter_first(model, &iter)){
    do{
      gtk_tree_model_get(model, &iter,
			 AGS_PATTERN_ENVELOPE_COLUMN_EDIT, &do_edit,
			 -1);
      
      if(do_edit){
	break;
      }
      
      nth++;
    }while(gtk_tree_model_iter_next(model, &iter));
  }

  /* move position */
  if(!do_edit){
    return;
  }

  /* get audio mutex */
  audio_mutex = AGS_AUDIO_GET_OBJ_MUTEX(audio);

  /* get next and current preset name */
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, &preset_name,
		     -1);

  if(!gtk_tree_model_iter_next(model, &iter)){
    return;
  }
  
  gtk_tree_model_get(model, &iter,
		     AGS_PATTERN_ENVELOPE_COLUMN_PRESET_NAME, &next_name,
		     -1);

  /* find preset */
  g_object_get(audio,
	       "preset", &start_preset,
	       NULL);
  
  preset = ags_preset_find_name(start_preset,
				preset_name);

  next = ags_preset_find_name(start_preset,
			      next_name);

  /* reorder list */
  current = next->data;
  
  g_rec_mutex_lock(audio_mutex);

  audio->preset = g_list_delete_link(audio->preset,
				     next);

  audio->preset = g_list_insert_before(audio->preset,
				       preset,
				       current);

  g_rec_mutex_unlock(audio_mutex);

  g_list_free_full(start_preset,
		   g_object_unref);
  
  /* load preset */
  ags_envelope_dialog_load_preset(envelope_dialog);    
}

void
ags_pattern_envelope_preset_add_callback(GtkWidget *button,
					 AgsPatternEnvelope *pattern_envelope)
{
  AgsInputDialog *dialog;

  if(pattern_envelope->rename != NULL){
    return;
  }
  
  dialog = (AgsInputDialog *) ags_input_dialog_new(i18n("preset name"),
						   (GtkWindow *) gtk_widget_get_ancestor((GtkWidget *) pattern_envelope,
											 AGS_TYPE_ENVELOPE_DIALOG));
  pattern_envelope->rename = (GtkDialog *) dialog;
  
  ags_input_dialog_set_flags(dialog,
			     AGS_INPUT_DIALOG_SHOW_STRING_INPUT);

  gtk_widget_show((GtkWidget *) dialog);

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
    AgsEnvelopeDialog *envelope_dialog;

    envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) pattern_envelope,
								    AGS_TYPE_ENVELOPE_DIALOG);
    
    ags_pattern_envelope_remove_preset(pattern_envelope,
				       nth);

    /* load preset */
    ags_envelope_dialog_load_preset(envelope_dialog);
  }
}

int
ags_pattern_envelope_preset_rename_response_callback(GtkWidget *widget, gint response,
						     AgsPatternEnvelope *pattern_envelope)
{
  if(response == GTK_RESPONSE_ACCEPT){
    AgsEnvelopeDialog *envelope_dialog;
    
    gchar *text;

    envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor((GtkWidget *) pattern_envelope,
								    AGS_TYPE_ENVELOPE_DIALOG);

    /* get name */
    text = gtk_editable_get_chars(GTK_EDITABLE(AGS_INPUT_DIALOG(widget)->string_input),
				  0, -1);
    
    /* add preset */
    ags_pattern_envelope_add_preset(pattern_envelope,
				    text);
    
    /* load preset */
    ags_envelope_dialog_load_preset(envelope_dialog);
  }
  
  pattern_envelope->rename = NULL;

  gtk_window_destroy((GtkWindow *) widget);

  return(0);
}
