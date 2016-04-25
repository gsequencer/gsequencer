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

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_main_loop.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playable.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/ags_open_sf2_sample.h>
#include <ags/audio/task/ags_add_audio_signal.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>

#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>

#include <math.h>

void ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
						AgsMachine *machine);

void
ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;
  AgsAudio *audio;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  audio = ffplayer->machine.audio;
  audio->soundcard = (GObject *) window->soundcard;
  
  AGS_MACHINE(ffplayer)->name = g_strdup_printf("Default %d\0",
						ags_window_find_machine_counter(window, AGS_TYPE_FFPLAYER)->counter);
  ags_window_increment_machine_counter(window,
				       AGS_TYPE_FFPLAYER);
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

  ffplayer->open_dialog = 
    file_chooser = ags_machine_file_chooser_dialog_new(AGS_MACHINE(ffplayer));
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser),
				       FALSE);

  g_signal_connect((GObject *) file_chooser, "response\0",
		   G_CALLBACK(ags_ffplayer_open_dialog_response_callback), AGS_MACHINE(ffplayer));

  gtk_widget_show_all((GtkWidget *) file_chooser);
}

void
ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
					   AgsMachine *machine)
{
  AgsWindow *window;
  AgsFFPlayer *ffplayer;
  GtkFileChooserDialog *file_chooser;
  GObject *soundcard;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(machine)));
  ffplayer = AGS_FFPLAYER(machine);
  file_chooser = GTK_FILE_CHOOSER_DIALOG(widget);

  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));

    if(g_str_has_suffix(filename, ".sf2\0")){
      AgsIpatch *ipatch;
      AgsPlayable *playable;
      gchar **preset;
      GError *error;

      /* clear preset and instrument */
      gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(ffplayer->preset)));
      gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(ffplayer->instrument)));

      /* Ipatch related */
      ipatch = g_object_new(AGS_TYPE_IPATCH,
			    "mode\0", AGS_IPATCH_READ,
			    "filename\0", filename,
			    NULL);
      ffplayer->ipatch = ipatch;
      ipatch->soundcard = window->soundcard;

      playable = AGS_PLAYABLE(ipatch);

      error = NULL;
      ags_playable_level_select(playable,
				0, filename,
				&error);

      /* select first preset */
      ipatch->nth_level = 1;
      preset = ags_playable_sublevel_names(playable);

      error = NULL;
      ags_playable_level_select(playable,
				1, *preset,
				&error);

      /* fill ffplayer->preset */
      while(preset != NULL && preset[0] != NULL){
	gtk_combo_box_text_append_text(ffplayer->preset,
				       *preset);

	preset++;
      }
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->preset),
			     0);
  }

  ffplayer->open_dialog = NULL;
  gtk_widget_destroy(widget);
}

void
ags_ffplayer_preset_changed_callback(GtkComboBox *preset, AgsFFPlayer *ffplayer)
{
  AgsPlayable *playable;
  AgsIpatch *ipatch;
  gchar *preset_name;
  gchar **instrument;
  GError *error;

  playable = AGS_PLAYABLE(ffplayer->ipatch);
  ipatch = ffplayer->ipatch;

  if(ipatch == NULL){
    return;
  }
  
  preset_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(preset));

  /* load presets */
  error = NULL;
  ags_playable_level_select(playable,
			    1, preset_name,
			    &error);

  /* select first instrument */
  gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(ffplayer->instrument)));

  ipatch->nth_level = 2;
  instrument = ags_playable_sublevel_names(playable);
  
  error = NULL;
  ags_playable_level_select(playable,
			    2, *instrument,
			    &error);
  
  /* fill ffplayer->instrument */
  while(instrument != NULL && instrument[0] != NULL){
    gtk_combo_box_text_append_text(ffplayer->instrument,
				   *instrument);

    instrument++;
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->instrument),
			   0);
}

void
ags_ffplayer_instrument_changed_callback(GtkComboBox *instrument, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;
  
  GObject *soundcard;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *recycling;

  //  AgsLinkChannel *link_channel;
  AgsOpenSf2Sample *open_sf2_sample;
  AgsAddAudioSignal *add_audio_signal;

  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  
  AgsPlayable *playable;

  gchar *filename;
  gchar *preset_name;
  gchar *instrument_name;
  gchar **preset;
  gchar **sample;
  GList *task;
  GList *list;
  guint count;
  int i;
  gboolean has_more;
  
  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel(ffplayer);
  application_context = window->application_context;
  audio = AGS_MACHINE(ffplayer)->audio;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  audio_loop = application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(audio_loop,
						       AGS_TYPE_TASK_THREAD);

  /* lookup audio mutex */
  pthread_mutex_lock(application_mutex);
    
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  playable = AGS_PLAYABLE(ffplayer->ipatch);

  filename = ffplayer->ipatch->filename;
  preset_name = AGS_IPATCH_SF2_READER(ffplayer->ipatch->reader)->selected[1];
  instrument_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(instrument));

  /* select instrument */
  error = NULL;

  ags_playable_level_select(playable,
			    2, instrument_name,
			    &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }
  
  /* select first sample */
  sample = NULL;

  AGS_IPATCH(ffplayer->ipatch)->nth_level = 3;
  sample = ags_playable_sublevel_names(playable);

  count = g_strv_length(sample);

  /* read all samples */
  ags_audio_set_audio_channels(audio,
			       AGS_IPATCH_DEFAULT_CHANNELS);

  ags_audio_set_pads(audio, AGS_TYPE_INPUT,
		     count);

  pthread_mutex_lock(audio_mutex);

  soundcard = audio->soundcard;
  channel = audio->input;

  pthread_mutex_unlock(audio_mutex);

  task = NULL;
  has_more = TRUE;

  while(channel != NULL && *sample != NULL){
    for(i = 0; i < AGS_IPATCH_DEFAULT_CHANNELS; i++){
      if(AGS_IPATCH_SF2_READER(ffplayer->ipatch->reader)->selected[3] == NULL){
	continue;
      }
      
      /* create tasks */
      open_sf2_sample = ags_open_sf2_sample_new(channel,
						g_strdup(filename),
						g_strdup(preset_name),
						g_strdup(instrument_name),
						g_strdup(*sample));
      task = g_list_prepend(task,
			    open_sf2_sample);
      
      /* iterate */
      channel = channel->next;
    }

    sample++;
  }
      
  /* append tasks */
  task = g_list_reverse(task);
  ags_task_thread_append_tasks(task_thread,
			       task);
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
