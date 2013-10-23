/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/X/machine/ags_ffplayer_callbacks.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/object/ags_playable.h>

#include <ags/audio/ags_channel.h>

#include <ags/audio/task/ags_link_channel.h>

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
  audio->devout = (GObject *) window->devout;

  ffplayer->machine.name = g_strdup_printf("Default %d\0", window->counter->ffplayer);
  window->counter->ffplayer++;

  /* AgsPlayNotation */
  g_object_set(G_OBJECT(ffplayer->play_notation),
	       "devout\0", audio->devout,
	       NULL);

  g_object_set(G_OBJECT(ffplayer->recall_notation),
	       "devout\0", audio->devout,
	       NULL);
}

void
ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  GtkFileChooserDialog *file_chooser;

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
      AgsIpatchSF2Reader *sf2_reader;
      AgsPlayable *playable;
      gchar **preset;
      gchar **instrument;
      gchar **sample;
      GError *error;

      /* clear preset, instrument and sample*/
      ags_combo_box_text_remove_all(ffplayer->instrument);

      /* Ipatch related */
      ffplayer->ipatch =
	ipatch = g_object_new(AGS_TYPE_IPATCH,
			      "mode\0", AGS_IPATCH_READ,
			      "filename\0", filename,
			      NULL);
      ipatch->devout = window->devout;
      ags_ipatch_open(ipatch, filename);

      sf2_reader = ags_ipatch_sf2_reader_new();
      sf2_reader->ipatch =  ipatch;
      ipatch->reader = (GObject *) sf2_reader;

      playable = AGS_PLAYABLE(ipatch->reader);
      
      ags_playable_open(playable, filename);

      error = NULL;
      ags_playable_level_select(playable,
				0, filename,
				&error);

      /* fill ffplayer->instrument */
      AGS_IPATCH_SF2_READER(ipatch->reader)->nth_level = 1;
      instrument = ags_playable_sublevel_names(playable);
      
      while(*instrument != NULL){
	gtk_combo_box_text_append_text(ffplayer->instrument,
				       *instrument);


	instrument++;
      }

      /* reset nth_level */
      //      AGS_IPATCH_SF2_READER(ffplayer->ipatch->reader)->nth_level = 0;
    }
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->instrument),
			   0);
  
  gtk_widget_destroy(widget);
}

void
ags_ffplayer_instrument_changed_callback(GtkComboBox *instrument, AgsFFPlayer *ffplayer)
{
  AgsChannel *channel;
  AgsLinkChannel *link_channel;
  AgsAudioSignal *audio_signal_source_old;
  gchar *instrument_name;
  GList *list;
  int i;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GError *error;

  instrument_name = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(instrument));

  error = NULL;

  AGS_IPATCH_SF2_READER(ffplayer->ipatch->reader)->nth_level = 2;
  ags_playable_level_select(AGS_PLAYABLE(ffplayer->ipatch->reader),
			    0, instrument_name,
			    &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }

  ags_playable_iter_start(AGS_PLAYABLE(ffplayer->ipatch->reader));

  ags_audio_set_audio_channels(AGS_MACHINE(ffplayer)->audio,
			       2);

  ags_audio_set_pads(AGS_MACHINE(ffplayer)->audio, AGS_TYPE_INPUT,
		     AGS_IPATCH_SF2_READER(ffplayer->ipatch->reader)->count);
  
  channel = AGS_MACHINE(ffplayer)->audio->input;

  while(channel != NULL){
    ags_playable_iter_next(AGS_PLAYABLE(ffplayer->ipatch->reader));

    ags_ipatch_read_audio_signal(ffplayer->ipatch);
    list = ffplayer->ipatch->audio_signal;

    for(i = 0; i < 2; i++){
      /* create task */
      link_channel = ags_link_channel_new(channel, NULL);
      
      /* append AgsLinkChannel */
      // FIXME:JK: has a need for the unavaible task
      //	    ags_devout_append_task(AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout),
      //				   AGS_TASK(link_channel));
      
      AGS_AUDIO_SIGNAL(list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
      AGS_AUDIO_SIGNAL(list->data)->recycling = (GObject *) channel->first_recycling;
      audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
      
      // FIXME:JK: create a task
      channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal, (gpointer) audio_signal_source_old);
      channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal, list->data);

      g_object_unref(G_OBJECT(audio_signal_source_old));

      /* iterate */
      if(list != NULL){
	list = list->next;
      }

      channel = channel->next;
    }
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
