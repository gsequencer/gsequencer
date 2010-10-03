#include "ags_drum_callbacks.h"
#include "ags_drum_input_pad.h"

#include "../ags_window.h"
#include "../ags_pad.h"
#include "../ags_navigation.h"

#include "../../audio/ags_devout.h"
#include "../../audio/ags_audio.h"
#include "../../audio/ags_input.h"
#include "../../audio/ags_output.h"
#include "../../audio/ags_audio_signal.h"
#include "../../audio/ags_pattern.h"
#include "../../audio/ags_recall.h"

#include "../../audio/recall/ags_delay.h"
#include "../../audio/recall/ags_copy_pattern.h"
#include "../../audio/recall/ags_play_channel.h"
#include "../../audio/recall/ags_play_audio_signal.h"

#include "../../audio/file/ags_audio_file.h"
#include "../../audio/file/ags_audio_file_wav.h"

#include <stdlib.h>
#include <math.h>

#define AGS_AUDIO_FILE_DEVOUT "AgsAudioFileDevout\0"
#define AGS_DRUM_PLAY_RECALL "AgsDrumPlayRecall\0"

void ags_drum_open_response_callback(GtkWidget *widget, gint response, AgsDrum *drum);

extern const char *AGS_DRUM_INDEX;

void
ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum)
{
  AgsWindow *window;
  AgsDelaySharedAudio *delay_shared_audio;
  AgsCopyPatternSharedAudio *copy_pattern_shared_audio;
  GList *list;
  double tic;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));
  drum->machine.audio->devout = (GObject *) window->devout;

  drum->machine.name = g_strdup_printf("Default %d\0", window->counter->drum);
  window->counter->drum++;

  /* AgsDelay related */
  delay_shared_audio = drum->delay_shared_audio;

  tic = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tic));
  printf("tic = %f\n\0", tic);
  printf("tic = %f\n\0", window->navigation->bpm->adjustment->value);
  delay_shared_audio->delay = (guint) round(((double)window->devout->frequency / (double)window->devout->buffer_size) * (60.0 / gtk_adjustment_get_value(window->navigation->bpm->adjustment)) * tic);

  /* AgsCopyPattern related */
  copy_pattern_shared_audio = drum->copy_pattern_shared_audio;

  copy_pattern_shared_audio->devout = window->devout;

  copy_pattern_shared_audio->stream_length = (guint)drum->length_spin->adjustment->value * (guint)(delay_shared_audio->delay + 1) + 1;

  fprintf(stdout, "ags_drum_parent_set_callback: delay_shared_audio->delay = %d\n\0", delay_shared_audio->delay);
}

gboolean
ags_drum_destroy_callback(GtkObject *object, AgsDrum *drum)
{
  ags_drum_destroy(object);

  return(TRUE);
}

void
ags_drum_show_callback(GtkWidget *widget, AgsDrum *drum)
{
  ags_drum_show(widget);
}

void
ags_drum_bpm_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  AgsWindow *window;
  AgsChannel *channel;
  AgsDelay *delay;
  GList *list_recall_id;
  double tic;
  guint delay_value;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW);

  tic = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tic));
  delay_value = (guint) round(((double)AGS_DEVOUT(drum->machine.audio->devout)->frequency / (double)AGS_DEVOUT(drum->machine.audio->devout)->buffer_size) * (60.0 / gtk_adjustment_get_value(window->navigation->bpm->adjustment)) * tic);

  drum->delay_shared_audio->delay = delay_value;

  drum->copy_pattern_shared_audio->stream_length = (guint)drum->length_spin->adjustment->value * (guint)(delay_value + 1) + 1;

  channel = drum->machine.audio->output;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, drum->copy_pattern_shared_audio->stream_length);

    channel = channel->next;
  }
}

void
ags_drum_open_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *check_button;

  file_chooser = (GtkFileChooserDialog *) gtk_file_chooser_dialog_new(g_strdup("open audio files\0"),
								      (GtkWindow *) gtk_widget_get_toplevel((GtkWidget *) drum),
								      GTK_FILE_CHOOSER_ACTION_OPEN,
								      GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
								      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
								      NULL);
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
		   G_CALLBACK(ags_drum_open_response_callback), drum);
}

void
ags_drum_open_response_callback(GtkWidget *widget, gint response, AgsDrum *drum)
{
  GtkFileChooserDialog *file_chooser;
  GtkCheckButton *overwrite;
  GtkCheckButton *create;
  AgsChannel *channel;
  AgsAudioFile *audio_file;
  AgsAudioSignal *audio_signal_source_old;
  AgsPlayChannel *play_channel;
  GList *list, *pad_list;
  GSList *filenames;
  guint list_length;
  guint i, j;
  gboolean reset;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  file_chooser = (GtkFileChooserDialog *) gtk_widget_get_toplevel(widget);

  if(response == GTK_RESPONSE_ACCEPT){
    filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
    overwrite = g_object_get_data((GObject *) widget, "overwrite\0");
    create = g_object_get_data((GObject *) widget, "create\0");

    channel = drum->machine.audio->input;

    if(overwrite->toggle_button.active){
      if(channel != NULL){
	for(i = 0; i < drum->machine.audio->input_pads && filenames != NULL; i++){
	  audio_file = ags_audio_file_new((char *) filenames->data);
	  audio_file->name = (char *) filenames->data;
	  audio_file->flags |= AGS_AUDIO_FILE_ALL_CHANNELS;
	  ags_audio_file_open(audio_file);
	  //	  AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
	  ags_audio_file_read_wav(audio_file);
	  ags_audio_file_set_devout(audio_file, (AgsDevout *) drum->machine.audio->devout);
	  ags_audio_file_read_audio_signal(audio_file);
	  list = audio_file->audio_signal;

	  for(j = 0; j < drum->machine.audio->audio_channels && list != NULL; j++){
	    ags_channel_set_link(channel, NULL);

	    AGS_AUDIO_SIGNAL(list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	    AGS_AUDIO_SIGNAL(list->data)->recycling = (GObject *) channel->first_recycling;
	    audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	    g_static_mutex_lock(&mutex);
	    channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal, (gpointer) audio_signal_source_old);
	    channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal, list->data);
	    g_static_mutex_unlock(&mutex);

	    g_object_unref(G_OBJECT(audio_signal_source_old));

	    list = list->next;
	    channel = channel->next;
	  }

	  if(audio_file->channels < drum->machine.audio->audio_channels)
	    channel = ags_channel_nth(channel, drum->machine.audio->audio_channels - audio_file->channels);

	  filenames = filenames->next;
	}
      }
    }

    if(create->toggle_button.active && filenames != NULL){
      list_length = g_slist_length(filenames);

      ags_audio_set_pads((AgsAudio *) drum->machine.audio, AGS_TYPE_INPUT,
			 list_length + AGS_AUDIO(drum->machine.audio)->input_pads);
      channel = ags_channel_nth(AGS_AUDIO(drum->machine.audio)->input, (AGS_AUDIO(drum->machine.audio)->input_pads - list_length) * AGS_AUDIO(drum->machine.audio)->audio_channels);
      pad_list = g_list_nth(gtk_container_get_children((GtkContainer *) drum->input_pad), AGS_AUDIO(drum->machine.audio)->input_pads - list_length);

      while(filenames != NULL){
	audio_file = ags_audio_file_new((char *) filenames->data);
	audio_file->name = (char *) filenames->data;
	audio_file->flags |= AGS_AUDIO_FILE_ALL_CHANNELS;
	ags_audio_file_open(audio_file);
	//	AGS_AUDIO_FILE_GET_CLASS(audio_file)->read_buffer(audio_file);
	ags_audio_file_read_wav(audio_file);
	ags_audio_file_set_devout(audio_file, (AgsDevout *) drum->machine.audio->devout);
	ags_audio_file_read_audio_signal(audio_file);
	list = audio_file->audio_signal;

	//	ags_drum_input_pad_connect(AGS_DRUM_INPUT_PAD(pad_list->data));
	//	gtk_widget_show(GTK_WIDGET(pad_list->data));
	pad_list = pad_list->next;

	for(j = 0; j < drum->machine.audio->audio_channels && list != NULL; j++){
	  AGS_AUDIO_SIGNAL(list->data)->flags |= AGS_AUDIO_SIGNAL_TEMPLATE;
	  AGS_AUDIO_SIGNAL(list->data)->recycling = (GObject *) channel->first_recycling;
	  audio_signal_source_old = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

	  g_static_mutex_lock(&mutex);
	  channel->first_recycling->audio_signal = g_list_remove(channel->first_recycling->audio_signal, (gpointer) audio_signal_source_old);
	  channel->first_recycling->audio_signal = g_list_prepend(channel->first_recycling->audio_signal, list->data);
	  g_static_mutex_unlock(&mutex);

	  g_object_unref(G_OBJECT(audio_signal_source_old));

	  list = list->next;
	  channel = channel->next;
	}

	if(drum->machine.audio->audio_channels > audio_file->channels)
	  channel = ags_channel_nth(channel, drum->machine.audio->audio_channels - audio_file->channels);

	filenames = filenames->next;
      }
    }

    gtk_widget_destroy((GtkWidget *) file_chooser);
  }else if(response == GTK_RESPONSE_CANCEL){
    gtk_widget_destroy((GtkWidget *) file_chooser);
  }
}

void
ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum)
{
  drum->copy_pattern_shared_audio->loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;
}

void
ags_drum_run_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsDevout *devout;
  guint group_id;
  GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  devout = AGS_DEVOUT(drum->machine.audio->devout);

  if(GTK_TOGGLE_BUTTON(toggle_button)->active){
    /* do the init stuff here */
    group_id = ags_audio_recursive_play_init(drum->machine.audio);
    drum->machine.audio->devout_play->group_id = group_id;

    /* append to AgsDevout */
    g_static_mutex_lock(&mutex);
    drum->machine.audio->devout_play->flags &= (~AGS_DEVOUT_PLAY_REMOVE);
    devout->play_audio = g_list_append(devout->play_audio, (gpointer) drum->machine.audio->devout_play);
    devout->play_audio_ref++;
    g_static_mutex_unlock(&mutex);

    /* call run */
    if((AGS_DEVOUT_PLAY_AUDIO & (devout->flags)) == 0)
      devout->flags |= AGS_DEVOUT_PLAY_AUDIO;

    AGS_DEVOUT_GET_CLASS(devout)->run(devout);
  }else{
    /* abort code */
    if((AGS_DEVOUT_PLAY_DONE & (drum->machine.audio->devout_play->flags)) == 0){
      drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_CANCEL;
    }else{
      AgsDelay *delay;

      drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_REMOVE;
      drum->machine.audio->devout_play->flags &= (~AGS_DEVOUT_PLAY_DONE);

      //      delay = AGS_DELAY(ags_recall_find_by_effect(drum->machine.audio->play, (char *) g_type_name(AGS_TYPE_DELAY))->data);
      //      delay->recall_ref = drum->machine.audio->input_lines;
    }
  }
}

void
ags_drum_run_delay_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrum *drum)
{ 
  AgsDelay *delay;

  fprintf(stdout, "ags_drum_run_delay_done\n\0");

  //  delay = AGS_DELAY(recall);
  //  drum = AGS_DRUM(AGS_AUDIO(delay->recall.parent)->machine);
  //  drum->block_run = TRUE;
  drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
  gtk_toggle_button_set_active(drum->run, FALSE);
}

void
ags_drum_tic_callback(GtkWidget *option_menu, AgsDrum *drum)
{
  AgsWindow *window;
  AgsChannel *channel;
  GList *list_recall_id;
  double tic;
  guint delay_value;

  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) drum);

  tic = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tic));
  delay_value = (guint) round(((double)AGS_DEVOUT(drum->machine.audio->devout)->frequency / (double)AGS_DEVOUT(drum->machine.audio->devout)->buffer_size) * (60.0 / gtk_adjustment_get_value(window->navigation->bpm->adjustment)) * tic);

  drum->delay_shared_audio->delay = delay_value;

  //  delay = AGS_DELAY(ags_recall_find_by_effect(drum->machine.audio->recall, (char *) g_type_name(AGS_TYPE_DELAY))->data);
  //  delay->delay = delay_value;

  drum->copy_pattern_shared_audio->stream_length = ((guint)drum->length_spin->adjustment->value) * (delay_value + 1) + 1;

  channel = drum->machine.audio->output;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, drum->copy_pattern_shared_audio->stream_length);

    channel = channel->next;
  }
}

void
ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  AgsChannel *channel;
  AgsDelaySharedAudio *delay_shared_audio;

  channel = drum->machine.audio->output;
  delay_shared_audio = drum->delay_shared_audio;
  drum->copy_pattern_shared_audio->length = (guint) GTK_SPIN_BUTTON(spin_button)->adjustment->value;
  drum->copy_pattern_shared_audio->stream_length = ((guint)drum->length_spin->adjustment->value) * (delay_shared_audio->delay + 1) + 1;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, drum->copy_pattern_shared_audio->stream_length);

    channel = channel->next;
  }
}

void
ags_drum_index0_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected0 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected0){
      toggle_button = drum->selected0;
      drum->selected0 = NULL;
      gtk_toggle_button_set_active(toggle_button, FALSE);
      drum->selected0 = (GtkToggleButton*) widget;

      drum->copy_pattern_shared_audio->i = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
    }else if(! gtk_toggle_button_get_active(drum->selected0)){
      toggle_button = drum->selected0;
      drum->selected0 = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) widget, TRUE);
      drum->selected0 = (GtkToggleButton*) widget;
    }

    ags_drum_set_pattern(drum);
  }
}

void
ags_drum_index1_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected1 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected1){
      toggle_button = drum->selected1;
      drum->selected1 = NULL;
      gtk_toggle_button_set_active(toggle_button, FALSE);
      drum->selected1 = (GtkToggleButton*) widget;

      drum->copy_pattern_shared_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
    }else if(!gtk_toggle_button_get_active(drum->selected1)){
      toggle_button = drum->selected1;
      drum->selected1 = NULL;
      gtk_toggle_button_set_active((GtkToggleButton *) widget, TRUE);
      drum->selected1 = (GtkToggleButton*) widget;
    }

    ags_drum_set_pattern(drum);
  }
}

void
ags_drum_pad_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsPattern *pattern;
  GList *list;
  guint i, index0, index1, offset;

  if((AGS_DRUM_BLOCK_PATTERN & (drum->flags)) != 0){ /* AGS_DRUM_BLOCK_PATTERN & drum->flags */
    printf("AgsDrum pattern is blocked\n\0");
    return;
  }

  list = gtk_container_get_children((GtkContainer *) drum->pattern);

  for(i = 0; i < 16 && toggle_button != list->data; i++)
    list = list->next;

  offset = i;

  for(i = 0; i < 4 && drum->selected0 != drum->index0[i]; i++);

  index0 = i;

  for(i = 0; i < 12 && drum->selected1 != drum->index1[i]; i++);
  
  index1 = i;

  list = gtk_container_get_children((GtkContainer *) drum->offset);

  for(i = 0; i < 4 && ! GTK_TOGGLE_BUTTON(list->data)->active; i++)
    list = list->next;

  offset += (i * 16);

  if(drum->selected_pad->pad.group->active){
    AgsChannel *channel, *next_pad;

    channel = drum->selected_pad->pad.channel;
    next_pad = channel->next_pad;

    while(channel != next_pad){
      ags_pattern_toggle_bit((AgsPattern *) channel->pattern->data, index0, index1, offset);

      channel = channel->next;
    }
  }else
    ags_pattern_toggle_bit((AgsPattern *) drum->selected_pad->pad.selected_line->channel->pattern->data, index0, index1, offset);
}

void
ags_drum_offset_callback(GtkWidget *widget, AgsDrum *drum)
{
  ags_drum_set_pattern(drum);
}
