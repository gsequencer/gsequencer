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

#include <ags/X/machine/ags_ffplayer_callbacks.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_ui_provider.h>
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
  gtk_file_chooser_add_shortcut_folder_uri(GTK_FILE_CHOOSER(file_chooser),
					   "file:///usr/share/sounds/sf2",
					   NULL);
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

  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->preset), 0);
}

void
ags_ffplayer_preset_changed_callback(GtkComboBox *preset, AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;
  
  gint position;
  
  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0 ||
     ffplayer->audio_container == NULL||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }

  audio_container = ffplayer->audio_container;

  /* reset */
  ags_sound_container_level_up(AGS_SOUND_CONTAINER(audio_container->sound_container),
			       3);

  /* load presets */
  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->preset));
  
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
					    position);
  AGS_IPATCH(audio_container->sound_container)->nesting_level += 1;
  
  /* select first instrument */
  ags_ffplayer_load_instrument(ffplayer);

  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->instrument), 0);
}

void
ags_ffplayer_instrument_changed_callback(GtkComboBox *instrument, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;

  AgsThread *gui_thread;

  AgsAudio *audio;

  AgsAudioContainer *audio_container;

  AgsOpenSf2Instrument *open_sf2_instrument;

  AgsApplicationContext *application_context;
  
  gint position;
  
  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0 ||
     ffplayer->audio_container == NULL||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ffplayer);

  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  audio = AGS_MACHINE(ffplayer)->audio;

  /*  */
  audio_container = ffplayer->audio_container;

  /* reset */
  ags_sound_container_level_up(AGS_SOUND_CONTAINER(audio_container->sound_container),
			       3);

  /* load presets */
  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->preset));

  if(position == -1){
    position = 0;
  }
  
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
					    position);
  AGS_IPATCH(audio_container->sound_container)->nesting_level += 1;

  /* load instrument */
  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->instrument));

  if(position == -1){
    position = 0;
  }

  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
  					    position);

  AGS_IPATCH(audio_container->sound_container)->nesting_level += 1;

  /* open sf2 instrument */
  open_sf2_instrument = ags_open_sf2_instrument_new(audio,
						    AGS_IPATCH(audio_container->sound_container),
						    NULL,
						    NULL,
						    NULL,
						    0);
  
  /* append task */
  ags_gui_thread_schedule_task((AgsGuiThread *) gui_thread,
			       (GObject *) open_sf2_instrument);
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
