/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/machine/ags_ffplayer_callbacks.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>

#include <ags/X/thread/ags_gui_thread.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <math.h>

#include <ags/config.h>

void ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
						AgsMachine *machine);

void
ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  
  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_FFPLAYER)->counter);

  g_object_set(AGS_MACHINE(ffplayer),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_FFPLAYER);

  g_free(str);
}

void
ags_ffplayer_destroy_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  if(ffplayer->open_dialog != NULL){
    gtk_widget_destroy(ffplayer->open_dialog);
  }
}

void
ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  GtkFileChooserDialog *file_chooser;

  file_chooser = ags_machine_file_chooser_dialog_new(AGS_MACHINE(ffplayer));
  ffplayer->open_dialog = (GtkWidget *) file_chooser;
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser),
				       FALSE);

  g_signal_connect((GObject *) file_chooser, "response",
		   G_CALLBACK(ags_ffplayer_open_dialog_response_callback), AGS_MACHINE(ffplayer));

  gtk_widget_show_all((GtkWidget *) file_chooser);
}

void
ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
					   AgsMachine *machine)
{
  AgsFFPlayer *ffplayer;

  ffplayer = AGS_FFPLAYER(machine);

  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    ags_ffplayer_open_filename(ffplayer,
			       filename);
  }

  ffplayer->open_dialog = NULL;
  gtk_widget_destroy(widget);
}

void
ags_ffplayer_preset_changed_callback(GtkComboBox *preset, AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;

  gint position;

  audio_container = AGS_PLAYABLE(ffplayer->audio_container);

  if(audio_container == NULL ||
     (AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0){
    return;
  }
  
  position = gtk_combo_box_get_active(preset);
  
  /* load presets */
  AGS_IPATCH(audio_container->sound_container)->nesting_level = AGS_SF2_FILENAME;
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
					    position);
  
  /* select first instrument */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(ffplayer->instrument))));

  ags_ffplayer_load_instrument(ffplayer);
  
  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->instrument),
  			   0);
}

void
ags_ffplayer_instrument_changed_callback(GtkComboBox *instrument, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;

  AgsGuiThread *gui_thread;

  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  AgsRecycling *recycling;

  AgsAudioContainer *audio_container;

  AgsResizeAudio *resize_audio;
  AgsOpenSf2Sample *open_sf2_sample;

  AgsApplicationContext *application_context;
  
  GList *task;
  GList *start_list, *list;
  
  gchar *filename;
  gchar *preset_name;
  gchar *instrument_name;
  gchar **preset;
  gchar **sample, **sample_iter;

  gint position;
  guint output_pads;
  guint n_pads, n_audio_channels;
  int i;
  
  GError *error;

  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0){
    return;
  }
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ffplayer);

  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  audio = AGS_MACHINE(ffplayer)->audio;

  /*  */
  audio_container = ffplayer->audio_container;

  filename = audio_container->filename;
  preset_name = audio_container->preset;
  instrument_name = audio_container->instrument;

  if(filename == NULL || preset_name == NULL || instrument_name == NULL){
    return;
  }

  position = gtk_combo_box_get_active(instrument);
  
  /* select instrument */
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
					    position);
  
  /* select first sample */
  AGS_IPATCH(audio_container->sound_container)->nesting_level = AGS_SF2_IHDR;

  /* read all samples */
  list = 
    start_list = ags_sound_container_get_resource_current(AGS_SOUND_CONTAINER(audio_container->sound_container));

  g_object_get(audio,
	       "output-pads", &output_pads,
	       "input", &start_channel,
	       NULL);
  
  n_pads = g_list_length(start_list);
  n_audio_channels = 0;

  while(list != NULL){
    guint current_audio_channels;
    
    ags_sound_resource_get_presets(AGS_SOUND_RESOURCE(list->data),
				   &current_audio_channels,
				   NULL,
				   NULL,
				   NULL);

    if(current_audio_channels > n_audio_channels){
      n_audio_channels = current_audio_channels;
    }
    
    list = list->next;
  }

  /* read */
  task = NULL;

  if(n_audio_channels > 0 &&
     n_pads > 0){
    /* resize */
    resize_audio = ags_resize_audio_new(audio,
					output_pads,
					n_pads,
					n_audio_channels);
    task = g_list_prepend(task,
			  resize_audio);

    /* open sf2 sample task */
    list = start_list;
  
    i = 0;

    while(list != NULL){
      guint audio_channel;

      g_object_get(channel,
		   "audio-channel", &audio_channel,
		   NULL);
    
      /* create tasks */
      open_sf2_sample = ags_open_sf2_sample_new(channel,
						list->data,
						NULL,
						NULL,
						NULL,
						NULL,
						audio_channel);
      task = g_list_prepend(task,
			    open_sf2_sample);
    
      /* iterate */
      g_object_get(channel,
		   "next", &channel,
		   NULL);

      i++;

      if(i >= n_audio_channels ||
	 audio_channel + 1 >= n_audio_channels){
	list = list->next;
      }
    }    
      
    /* append tasks */
    task = g_list_reverse(task);
    ags_gui_thread_schedule_task_list(gui_thread,
				      task);
  }
}

gboolean
ags_ffplayer_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsFFPlayer *ffplayer)
{
  ags_ffplayer_paint(ffplayer);

  return(FALSE);
}

gboolean
ags_ffplayer_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsFFPlayer *ffplayer)
{
  ags_ffplayer_paint(ffplayer);

  return(FALSE);
}

gboolean
ags_ffplayer_drawing_area_button_press_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{

  return(FALSE);
}

void
ags_ffplayer_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsFFPlayer *ffplayer)
{
  ags_ffplayer_paint(ffplayer);
}
