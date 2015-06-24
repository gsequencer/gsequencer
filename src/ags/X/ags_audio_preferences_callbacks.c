/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_audio_preferences_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_task.h>

#include <ags/audio/task/ags_set_output_device.h>
#include <ags/audio/task/ags_set_audio_channels.h>
#include <ags/audio/task/ags_set_buffer_size.h>
#include <ags/audio/task/ags_set_samplerate.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

int
ags_audio_preferences_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsAudioPreferences *audio_preferences)
{
  GtkListStore *model;
  GtkTreeIter iter;
  GList *card_id, *card_name;
  
  if(old_parent != NULL)
    return(0);

  /* refresh */
  ags_devout_list_cards(&card_id, &card_name);
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  
  while(card_id != NULL){
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter,
		       0, card_id->data,
		       1, card_name->data,
		       -1);
    
    card_id = card_id->next;
    card_name = card_name->next;
  }

  gtk_combo_box_set_model(audio_preferences->card,
			  GTK_TREE_MODEL(model));
  
  g_list_free(card_id);
  g_list_free(card_name);

  return(0);
}

void
ags_audio_preferences_card_changed_callback(GtkComboBox *combo,
					    AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsSetOutputDevice *set_output_device;
  GtkListStore *model;
  GtkTreeIter current;
  gchar *str;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;
  GValue value =  {0,};

  GError *error;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							      AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);

  model = gtk_combo_box_get_model(audio_preferences->card);

  if(!gtk_combo_box_get_active_iter(audio_preferences->card,
				    &current)){
    return;
  }

  gtk_tree_model_get_value(model,
			   &current,
			   0,
			   &value);
  
  str = g_strdup(g_value_get_string(&value));
  
  /* create set output device task */
  set_output_device = ags_set_output_device_new((GObject *) devout,
						str);

  /* append AgsSetOutputDevice */
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
			      AGS_TASK(set_output_device));
  
  /* reset dialog */
  error = NULL;
  ags_devout_pcm_info(str,
  		      &channels_min, &channels_max,
  		      &rate_min, &rate_max,
  		      &buffer_size_min, &buffer_size_max,
  		      &error);

  if(error != NULL){
    GtkMessageDialog *dialog;

    dialog = (GtkMessageDialog *) gtk_message_dialog_new((GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
											       AGS_TYPE_PREFERENCES),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 "%s\0", error->message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));

    gtk_spin_button_set_range(audio_preferences->audio_channels, 0.0, 24.0);
    gtk_spin_button_set_range(audio_preferences->samplerate, 1.0, 192000.0);
    gtk_spin_button_set_range(audio_preferences->buffer_size, 1.0, 65535.0);

    return;
  }

  gtk_spin_button_set_range(audio_preferences->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(audio_preferences->samplerate,
			    rate_min, rate_max);
  gtk_spin_button_set_range(audio_preferences->buffer_size,
			    buffer_size_min, buffer_size_max);
}

void
ags_audio_preferences_audio_channels_changed(GtkSpinButton *spin_button,
					     AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsSetAudioChannels *set_audio_channels;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							      AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);

  /* create set output device task */
  set_audio_channels = ags_set_audio_channels_new(devout,
						  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetAudioChannels */
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
			      AGS_TASK(set_audio_channels));
}

void
ags_audio_preferences_samplerate_changed(GtkSpinButton *spin_button,
					 AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsSetSamplerate *set_samplerate;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							      AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);

  /* create set output device task */
  set_samplerate = ags_set_samplerate_new((GObject *) devout,
					  (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetSamplerate */
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
			      AGS_TASK(set_samplerate));
}

void
ags_audio_preferences_buffer_size_changed(GtkSpinButton *spin_button,
					  AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsSetBufferSize *set_buffer_size;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
									 AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);

  /* create set output device task */
  set_buffer_size = ags_set_buffer_size_new((GObject *) devout,
					    (guint) gtk_spin_button_get_value(spin_button));

  /* append AgsSetBufferSize */
  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread),
			      AGS_TASK(set_buffer_size));
}
