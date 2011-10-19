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

#include <ags/X/ags_window.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>

#include <ags/audio/file/ags_audio_file.h>

void ags_ffplayer_open_response_callback(GtkWidget *widget, gint response, AgsFFPlayer *ffplayer);

void
ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  ffplayer->machine.name = g_strdup_printf("Default %d\0", window->counter->ffplayer);
  window->counter->ffplayer++;
}

gboolean
ags_ffplayer_destroy_callback(GtkObject *object, AgsFFPlayer *ffplayer)
{
  ags_ffplayer_destroy(object);

  return(TRUE);
}

void
ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *check_button;

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(g_strdup("open audio files\0"),
								      (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) ffplayer),
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("open in new channel\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "create\0", (gpointer) check_button);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("overwrite existing links\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data((GObject *) file_chooser, "overwrite\0", (gpointer) check_button);

  gtk_widget_show_all((GtkWidget *) file_chooser);

  g_signal_connect((GObject *) file_chooser, "response\0",
		   G_CALLBACK(ags_ffplayer_open_response_callback), file_chooser);
}

void
ags_ffplayer_open_response_callback(GtkWidget *widget, gint response, AgsFFPlayer *ffplayer)
{
  /*
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *overwrite;
  GtkCheckButton *create;
  AgsChannel *channel;
  AgsAudioFile *audio_file;
  GList *list;
  GSList *filenames;
  guint i, j;
  gboolean reset;

  file_chooser = (GtkFileChooserDialog *) gtk_widget_get_toplevel(widget);

  if(response == GTK_RESPONSE_ACCEPT){
    filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
    overwrite = g_object_get_data((GObject *) widget, "overwrite\0");
    create = g_object_get_data((GObject *) widget, "create\0");

    channel = ffplayer->machine.audio->input;

    if(overwrite->toggle_button.active){
      if(channel == NULL)
	if(create->toggle_button.active)
	  goto ags_ffplayer_open_response_callback0;
	else
	  return;

      for(i = 0; i < ffplayer->machine.audio->input_pads && filenames != NULL; i++){
	audio_file = ags_audio_file_new((char *) filenames->data);
	ags_audio_file_open(audio_file);
	AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
	ags_audio_file_read_audio_signal(audio_file);
	list = audio_file->audio_signal;

	for(j = 0; j < ffplayer->machine.audio->audio_channels && list != NULL; j++){
	  ags_channel_set_link(channel, NULL);

	  channel->first_recycling->audio_signal = g_list_append(channel->first_recycling->audio_signal, list->data);

	  list = list->next;
	}

	filenames = filenames->next;
	channel = channel->next;
      }
    }

    if(create->toggle_button.active){
    ags_ffplayer_open_response_callback0:

      if(filenames == NULL)
	return;

      if(channel == NULL)
	reset = TRUE;
      else
	channel = ags_channel_last(channel);

      i = ffplayer->machine.audio->input_pads;
      ags_audio_set_pads((AgsAudio *) channel->audio, AGS_TYPE_INPUT,
			 AGS_AUDIO(channel->audio)->input_pads + 1);

      for(; filenames != NULL;){
	audio_file = ags_audio_file_new((char *) filenames->data);
	ags_audio_file_open(audio_file);
	AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
	ags_audio_file_read_audio_signal(audio_file);
	list = audio_file->audio_signal;

	for(j = 0; j < ffplayer->machine.audio->audio_channels && list != NULL; j++){
	  ags_channel_set_link(channel, NULL);

	  channel->first_recycling->audio_signal = g_list_append(channel->first_recycling->audio_signal, list->data);

	  list = list->next;
	}

	i++;
	filenames = filenames->next;
	channel = channel->next;

	if(filenames != NULL)
	  ags_audio_set_pads((AgsAudio *) channel->audio, AGS_TYPE_INPUT,
			     AGS_AUDIO(channel->audio)->input_pads +1);
      }
    }

    gtk_widget_destroy((GtkWidget *) file_chooser);
  }else if(response == GTK_RESPONSE_CANCEL){
    gtk_widget_destroy((GtkWidget *) file_chooser);
  }
  */
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
