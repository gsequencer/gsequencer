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

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_playable.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/ags_resize_audio.h>
#include <ags/audio/task/ags_open_sf2_sample.h>
#include <ags/audio/task/ags_add_audio_signal.h>

#include <ags/audio/file/ags_audio_file.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>

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
  AgsPlayable *playable;
  AgsIpatch *ipatch;

  gchar *preset_name;

  GError *error;

  playable = AGS_PLAYABLE(ffplayer->ipatch);
  ipatch = ffplayer->ipatch;

  if(ipatch == NULL ||
     (AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0){
    return;
  }
  
  preset_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(preset));

  /* load presets */
  error = NULL;
  ags_playable_level_select(playable,
			    1, preset_name,
			    &error);

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

  AgsAudio *audio;
  AgsChannel *start_channel, *channel;
  AgsRecycling *recycling;

  AgsIpatchSF2Reader *reader;
  
  //  AgsLinkChannel *link_channel;
  AgsResizeAudio *resize_audio;
  AgsOpenSf2Sample *open_sf2_sample;
  AgsAddAudioSignal *add_audio_signal;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsGuiThread *gui_thread;

  AgsApplicationContext *application_context;
  
  AgsPlayable *playable;

  gchar *filename;
  gchar *preset_name;
  gchar *instrument_name;
  gchar **preset;
  gchar **sample, **sample_iter;
  GList *task;
  GList *list;
  guint count;
  guint n_pads, n_audio_channels;
  int i;
  
  GError *error;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *channel_mutex;

  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0){
    return;
  }
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ffplayer);
  application_context = (AgsApplicationContext *) window->application_context;
  audio = AGS_MACHINE(ffplayer)->audio;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);

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

  if(filename == NULL || preset_name == NULL || instrument_name == NULL){
    return;
  }
  
  /* select instrument */
  error = NULL;

  ags_playable_level_select(playable,
			    2, instrument_name,
			    &error);

  if(error != NULL){
    g_error("%s", error->message);
  }
  
  /* select first sample */
  sample = NULL;

  AGS_IPATCH(ffplayer->ipatch)->nth_level = 3;
  sample = ags_playable_sublevel_names(playable);

#ifdef HAVE_GLIB_2_6
  count = g_strv_length(sample);
#else
  count = ags_strv_length(sample);
#endif

  /* read all samples */
  reader = AGS_IPATCH(ffplayer->ipatch)->reader;
  task = NULL;

  n_pads = 0;
  n_audio_channels = 2;
  
#ifdef AGS_WITH_LIBINSTPATCH
  for(sample_iter = sample; *sample_iter != NULL; sample_iter++){
    IpatchSF2Sample *sf2_sample;
    
    guint sample_channel;

    sf2_sample = (IpatchSF2Sample *) ipatch_sf2_find_sample(reader->sf2,
							    *sample_iter,
							    NULL);
    g_object_get(sf2_sample,
		 "channel", &sample_channel,
		 NULL);

    if(sample_channel == IPATCH_SF2_SAMPLE_CHANNEL_MONO ||
       sample_channel == IPATCH_SF2_SAMPLE_CHANNEL_LEFT){
      n_pads++;
    }
  }
#endif

  resize_audio = ags_resize_audio_new(audio,
				      audio->output_pads,
				      n_pads,
				      n_audio_channels);
  task = g_list_prepend(task,
			resize_audio);

  /* open sf2 sample task */
  pthread_mutex_lock(audio_mutex);

  start_channel = audio->input;

  pthread_mutex_unlock(audio_mutex);

  sample_iter = sample;

  i = 0;
  
#ifdef AGS_WITH_LIBINSTPATCH
  while(*sample_iter != NULL){
    IpatchSF2Sample *sf2_sample;
    
    guint sample_channel;

    sf2_sample = (IpatchSF2Sample *) ipatch_sf2_find_sample(reader->sf2,
							    *sample_iter,
							    NULL);
    g_object_get(sf2_sample,
		 "channel", &sample_channel,
		 NULL);

    if(sample_channel == IPATCH_SF2_SAMPLE_CHANNEL_MONO ||
       sample_channel == IPATCH_SF2_SAMPLE_CHANNEL_LEFT){
      channel = ags_channel_nth(start_channel,
				i * audio->audio_channels);
    }else{
      channel = ags_channel_nth(start_channel,
				i * audio->audio_channels + 1);
    }
    
    if(sample_channel == IPATCH_SF2_SAMPLE_CHANNEL_MONO ||
       sample_channel == IPATCH_SF2_SAMPLE_CHANNEL_RIGHT){
      i++;
    }

    if(channel == NULL){
      g_critical("channel == NULL - Soundfont 2 sample channel %d", sample_channel);

      /* iterate */
      sample_iter++;
      
      continue;
    }
    
    /* create tasks */
    open_sf2_sample = ags_open_sf2_sample_new(channel,
					      g_strdup(filename),
					      g_strdup(preset_name),
					      g_strdup(instrument_name),
					      g_strdup(*sample_iter));
    task = g_list_prepend(task,
			  open_sf2_sample);
    
    /* iterate */
    sample_iter++;
  }    
      
  /* append tasks */
  task = g_list_reverse(task);
  ags_gui_thread_schedule_task_list(gui_thread,
				    task);
#endif
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
