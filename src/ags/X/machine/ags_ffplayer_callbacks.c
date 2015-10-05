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

#include <ags/main.h>

#include <ags/object/ags_playable.h>
#include <ags/object/ags_main_loop.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/task/ags_link_channel.h>
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
void ags_ffplayer_link_channel_launch_callback(AgsTask *task, AgsAudioSignal *audio_signal);

extern pthread_mutex_t ags_application_mutex;

void
ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;
  AgsAudio *audio;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  audio = ffplayer->machine.audio;
  audio->devout = (GObject *) window->devout;
  
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
  AgsDevout *devout;

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
      ipatch->devout = window->devout;

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
  
  AgsDevout *devout;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *recycling;

  AgsLinkChannel *link_channel;
  AgsAddAudioSignal *add_audio_signal;

  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsMain *ags_main;
  
  AgsPlayable *playable;
  
  gchar *instrument_name;
  gchar **preset;
  gchar **sample;
  GList *task;
  GList *list;
  guint count;
  int i;
  gboolean has_more;
  
  GError *error;

  pthread_mutex_t *audio_loop_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel(ffplayer);
  ags_main = window->ags_main;
  audio = AGS_MACHINE(ffplayer)->audio;

  /* get audio loop */
  pthread_mutex_lock(&(ags_application_mutex));

  audio_loop = ags_main->main_loop;

  pthread_mutex_unlock(&(ags_application_mutex));

  /* lookup audio loop mutex */
  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();
    
  audio_loop_mutex = ags_mutex_manager_lookup(mutex_manager,
					      (GObject *) audio_loop);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  /* get task thread */
  pthread_mutex_lock(audio_loop_mutex);

  task_thread = (AgsTaskThread *) audio_loop->task_thread;

  pthread_mutex_unlock(audio_loop_mutex);

  /* lookup audio mutex */
  pthread_mutex_lock(&(ags_application_mutex));
    
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  /*  */
  playable = AGS_PLAYABLE(ffplayer->ipatch);

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

  error = NULL;
  ags_playable_level_select(playable,
			    3, *sample,
			    &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }

  count = 0;
  
  while(*sample != NULL){
    sample++;
    count++;
  }

  /* read all samples */
  ags_audio_set_audio_channels(audio,
			       AGS_IPATCH_DEFAULT_CHANNELS);

  AGS_IPATCH(ffplayer->ipatch)->nth_level = 3;

  ags_playable_iter_start(playable);

  ags_audio_set_pads(audio, AGS_TYPE_INPUT,
		     count);

  pthread_mutex_lock(audio_mutex);

  devout = audio->devout;
  channel = audio->input;

  pthread_mutex_unlock(audio_mutex);

  task = NULL;
  has_more = TRUE;

  while(channel != NULL && has_more){
    list = ags_playable_read_audio_signal(playable,
					  (AgsDevout *) AGS_MACHINE(ffplayer)->audio->devout,
					  channel->audio_channel, AGS_IPATCH_DEFAULT_CHANNELS);

    for(i = 0; i < AGS_IPATCH_DEFAULT_CHANNELS && list != NULL; i++){
      /* create tasks */
      link_channel = ags_link_channel_new(channel, NULL);
      task = g_list_prepend(task,
			    link_channel);     
      g_signal_connect_after((GObject *) link_channel, "launch\0",
			     G_CALLBACK(ags_ffplayer_link_channel_launch_callback), list->data);
      
      /* iterate */	
      channel = channel->next;
      list = list->next;
    }

    has_more = ags_playable_iter_next(playable);
  }
      
  /* append tasks */
  task = g_list_reverse(task);
  ags_task_thread_append_tasks(task_thread,
			       task);
}

void
ags_ffplayer_link_channel_launch_callback(AgsTask *task, AgsAudioSignal *audio_signal)
{
  AgsChannel *channel;
  AgsAudioSignal *audio_signal_source_old;

  channel = AGS_LINK_CHANNEL(task)->channel;
  
  /* replace template audio signal */
  audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
  
  ags_recycling_remove_audio_signal(channel->first_recycling,
				    (gpointer) audio_signal_source_old);
  
  audio_signal->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
  ags_recycling_add_audio_signal(channel->first_recycling,
				 audio_signal); 
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
