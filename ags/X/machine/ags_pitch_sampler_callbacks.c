/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/machine/ags_pitch_sampler_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/i18n.h>

void ags_pitch_sampler_open_response_callback(GtkWidget *widget, gint response,
					      AgsPitchSampler *pitch_sampler);

void
ags_pitch_sampler_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsPitchSampler *pitch_sampler)
{
  AgsWindow *window;

  gchar *str;
  
  if(old_parent != NULL){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) pitch_sampler, AGS_TYPE_WINDOW));

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_PITCH_SAMPLER)->counter);

  g_object_set(AGS_MACHINE(pitch_sampler),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_PITCH_SAMPLER);
  g_free(str);
}

void
ags_pitch_sampler_open_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  GtkFileChooserDialog *dialog;

  if(pitch_sampler->open_dialog != NULL){
    return;
  }
  
  pitch_sampler->open_dialog = 
    dialog = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(i18n("Open audio files"),
								  (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) pitch_sampler),
								  GTK_FILE_CHOOSER_ACTION_OPEN,
								  GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
								  NULL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog),
				       FALSE);
  gtk_widget_show_all((GtkWidget *) dialog);

  g_signal_connect((GObject *) dialog, "response",
		   G_CALLBACK(ags_pitch_sampler_open_response_callback), pitch_sampler);
}

void
ags_pitch_sampler_open_response_callback(GtkWidget *widget, gint response,
					 AgsPitchSampler *pitch_sampler)
{
  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    gtk_entry_set_text(pitch_sampler->filename,
		       filename);
    ags_pitch_sampler_open_filename(pitch_sampler,
				    filename);
  }

  pitch_sampler->open_dialog = NULL;
  gtk_widget_destroy(widget);
}

void
ags_pitch_sampler_add_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  AgsPitchSamplerFile *pitch_sampler_file;

  pitch_sampler_file = ags_pitch_sampler_file_new();
  
  ags_pitch_sampler_add_file(pitch_sampler,
			     pitch_sampler_file);
  
  ags_connectable_connect(AGS_CONNECTABLE(pitch_sampler_file));
  
  g_signal_connect((GObject *) pitch_sampler_file, "control-changed",
		   G_CALLBACK(ags_pitch_sampler_file_control_changed_callback), (gpointer) pitch_sampler);
}

void
ags_pitch_sampler_remove_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  GList *list, *list_start;
  GList *child_start;

  guint nth;
  
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(pitch_sampler->file));

  nth = 0;
  
  while(list != NULL){
    child_start = gtk_container_get_children(GTK_CONTAINER(list->data));

    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(child_start->data))){
      ags_pitch_sampler_remove_file(pitch_sampler,
				    nth);
    }else{
      nth++;
    }
    
    g_list_free(child_start);
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_pitch_sampler_auto_update_callback(GtkToggleButton *toggle, AgsPitchSampler *pitch_sampler)
{
  if(gtk_toggle_button_get_active(toggle)){
    pitch_sampler->flags |= AGS_PITCH_SAMPLER_AUTO_UPDATE;
  }else{
    pitch_sampler->flags &= (~AGS_PITCH_SAMPLER_AUTO_UPDATE);
  }
}

void
ags_pitch_sampler_update_callback(GtkButton *button, AgsPitchSampler *pitch_sampler)
{
  ags_pitch_sampler_update(pitch_sampler);
}

void
ags_pitch_sampler_file_control_changed_callback(AgsPitchSamplerFile *pitch_sampler_file,
						AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_enable_lfo_callback(GtkToggleButton *toggle, AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_lfo_freq_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_lfo_phase_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_lfo_depth_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}

void
ags_pitch_sampler_lfo_tuning_callback(GtkSpinButton *spin_button, AgsPitchSampler *pitch_sampler)
{
  if((AGS_PITCH_SAMPLER_AUTO_UPDATE & (pitch_sampler->flags)) != 0){
    ags_pitch_sampler_update(pitch_sampler);
  }
}
