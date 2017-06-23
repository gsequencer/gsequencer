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

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_preset.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <ags/X/editor/ags_envelope_dialog.h>

#include <ags/i18n.h>

pthread_mutex_t* ags_pattern_envelope_get_audio_mutex(AgsPatternEnvelope *pattern_envelope);

pthread_mutex_t*
ags_pattern_envelope_get_audio_mutex(AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
  machine = envelope_dialog->machine;

  audio = machine->audio;

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  return(audio_mutex);
}

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
  
  GValue value = {0,};

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

  GValue value = {0,};

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

  GValue value = {0,};

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

  GValue value = {0,};

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

  GValue value = {0,};

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

  GValue value = {0,};

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);
  
  /* get value and update preset */
  attack_x = gtk_range_get_value(GTK_RANGE(range));

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0][0] = attack_x;

  ags_preset_add_parameter(preset,
			   "attack", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);
  
  /* get value and update preset */
  attack_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "attack", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  val[0][1] = attack_y;

  ags_preset_add_parameter(preset,
			   "attack", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }

  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);
  
  /* get value and update preset */
  decay_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  decay_x + I * val[0][1]);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;
  
  pthread_mutex_t *audio_mutex;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);

  /* get value and update preset */
  decay_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "decay", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][0] + decay_y * I);

  ags_preset_add_parameter(preset,
			   "decay", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);

  /* get value and update preset */
  sustain_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  sustain_x + I * val[0][1]);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);

  /* get value and update preset */
  sustain_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "sustain", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][0] + sustain_y * I);

  ags_preset_add_parameter(preset,
			   "sustain", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;
  
  pthread_mutex_t *audio_mutex;

  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);

  /* get value and update preset */
  release_x = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  release_x + I * val[0][1]);

  ags_preset_add_parameter(preset,
			   "release", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);

  /* get value and update preset */
  release_y = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "release", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  val[0][0] + release_y * I);

  ags_preset_add_parameter(preset,
			   "release", &value);

  pthread_mutex_unlock(audio_mutex);

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

  GValue value = {0,};

  GError *error;

  pthread_mutex_t *audio_mutex;
  
  if((AGS_PATTERN_ENVELOPE_NO_UPDATE & (pattern_envelope->flags)) != 0){
    return;
  }
  
  /* get preset */
  preset = ags_pattern_envelope_get_active_preset(pattern_envelope);
  
  if(preset == NULL){
    return;
  }
  
  audio_mutex = ags_pattern_envelope_get_audio_mutex(pattern_envelope);

  /* get value and update preset */
  ratio = gtk_range_get_value((GtkRange *) range);

  g_value_init(&value,
	       AGS_TYPE_COMPLEX);

  pthread_mutex_lock(audio_mutex);

  error = NULL;
  ags_preset_get_parameter(preset,
			   "ratio", &value,
			   &error);

  if(error != NULL){
    g_message("%s", error->message);

    pthread_mutex_unlock(audio_mutex);

    return;
  }
  
  val = (AgsComplex *) g_value_get_boxed(&value);

  /* add parameter */
  ags_complex_set(val,
		  ratio * I);

  ags_preset_add_parameter(preset,
			   "ratio", &value);

  pthread_mutex_unlock(audio_mutex);

  /* plot */
  ags_pattern_envelope_plot(pattern_envelope);
}

void
ags_pattern_envelope_preset_move_up_callback(GtkWidget *button,
					     AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsAudio *audio;
  AgsPreset *current;
  
  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  GList *preset, *prev;

  gchar *preset_name, *prev_name;
  
  guint nth;
  gboolean do_edit;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
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
  if(!do_edit ||
     nth == 0){
    return;
  }

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

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
  pthread_mutex_lock(audio_mutex);

  preset = ags_preset_find_name(audio->preset,
				preset_name);

  prev = ags_preset_find_name(audio->preset,
			      prev_name);

  /* reorder list */
  current = preset->data;
  
  audio->preset = g_list_delete_link(audio->preset,
				     preset);

  audio->preset = g_list_insert_before(audio->preset,
				       prev,
				       current);

  pthread_mutex_unlock(audio_mutex);
  
  /* load preset */
  ags_pattern_envelope_load_preset(pattern_envelope);    
}

void
ags_pattern_envelope_preset_move_down_callback(GtkWidget *button,
					       AgsPatternEnvelope *pattern_envelope)
{
  AgsEnvelopeDialog *envelope_dialog;

  AgsWindow *window;
  AgsMachine *machine;

  GtkTreeModel *model;
  GtkTreeIter iter;

  AgsAudio *audio;
  AgsPreset *current;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;
  
  GList *preset, *next;

  gchar *preset_name, *next_name;
  
  guint nth;
  gboolean do_edit;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  envelope_dialog = (AgsEnvelopeDialog *) gtk_widget_get_ancestor(pattern_envelope,
								  AGS_TYPE_ENVELOPE_DIALOG);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) envelope_dialog->machine,
						 AGS_TYPE_WINDOW);
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

  /* application context and mutex manager */
  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

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
  pthread_mutex_lock(audio_mutex);

  preset = ags_preset_find_name(audio->preset,
				preset_name);

  next = ags_preset_find_name(audio->preset,
			      next_name);

  /* reorder list */
  current = next->data;
  
  audio->preset = g_list_delete_link(audio->preset,
				     next);

  audio->preset = g_list_insert_before(audio->preset,
				       preset,
				       current);

  pthread_mutex_unlock(audio_mutex);

  /* load preset */
  ags_pattern_envelope_load_preset(pattern_envelope);    
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
    dialog = (GtkDialog *) gtk_dialog_new_with_buttons(i18n("preset name"),
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

