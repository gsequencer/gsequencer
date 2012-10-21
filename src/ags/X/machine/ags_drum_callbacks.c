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

#include <ags/X/machine/ags_drum_callbacks.h>
#include <ags/X/machine/ags_drum_input_pad.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_navigation.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>

#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_cancel_audio.h>
#include <ags/audio/task/ags_link_channel.h>
#include <ags/audio/task/ags_start_devout.h>
#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_delay_audio_run.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>
#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_audio_signal.h>

#include <ags/audio/file/ags_audio_file.h>

#include <stdlib.h>
#include <math.h>

#define AGS_AUDIO_FILE_DEVOUT "AgsAudioFileDevout\0"
#define AGS_DRUM_PLAY_RECALL "AgsDrumPlayRecall\0"

extern const char *AGS_DRUM_INDEX;

void
ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsAudio *audio;
  AgsDelayAudio *delay_audio;
  AgsCountBeatsAudio *count_beats_audio;
  AgsCopyPatternAudio *copy_pattern_audio;
  GList *list;
  double delay, bpm, tact;
  guint length, stream_length;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));
  audio = drum->machine.audio;
  audio->devout = (GObject *) window->devout;

  drum->machine.name = g_strdup_printf("Default %d\0", window->counter->drum);
  window->counter->drum++;

  devout = AGS_DEVOUT(audio->devout);
  bpm = window->navigation->bpm->adjustment->value;

  if(bpm > 60.0){
    bpm = exp2(-1.0 * 60.0 / bpm);
  }else{
    bpm = exp2(60.0 / bpm);
  }
  tact = exp2(8.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tact));
  delay = (((double) devout->frequency / (double) devout->buffer_size) *
	   bpm / 
	   16.0);
  drum->play_delay_audio->notation_delay = (guint) floor(delay);
  drum->play_delay_audio->sequencer_delay = (guint) floor(delay * tact);
  delay = (((double) devout->frequency / (double) devout->buffer_size) *
	   bpm / 
	   16.0);
  drum->recall_delay_audio->notation_delay = (guint) floor(delay);
  drum->recall_delay_audio->sequencer_delay = (guint) floor(delay * tact);
  /*
   * FIXME:JK: the following code is ugly
   */
  /* pattern related * /
  length = (guint) drum->length_spin->adjustment->value;

  //FIXME:JK: take prove if (delay + 1) is necessary and not just (delay)
  stream_length = length * (delay + 1) + 1;

  /* AgsCountBeats * /
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->stream_length = stream_length;
  }

  list = ags_recall_find_type(audio->recall,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->stream_length = stream_length;
    } */

  /* AgsCopyPatternAudio */
  list = ags_recall_find_type(drum->machine.audio->play,
			      AGS_TYPE_COPY_PATTERN_AUDIO);

  if(list != NULL){
    copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);

    g_object_set(G_OBJECT(copy_pattern_audio),
		 "devout\0", window->devout,
		 NULL);
  }

  list = ags_recall_find_type(drum->machine.audio->recall,
			      AGS_TYPE_COPY_PATTERN_AUDIO);

  if(list != NULL){
    copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);

    g_object_set(G_OBJECT(copy_pattern_audio),
		 "devout\0", window->devout,
		 NULL);
  }

  //  fprintf(stdout, "ags_drum_parent_set_callback: delay = %d\n\0", delay);
}

void
ags_drum_bpm_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsDelayAudio *delay_audio;
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  double bpm, tact;
  guint delay, length, stream_length;

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW);

  bpm = gtk_adjustment_get_value(window->navigation->bpm->adjustment);
  g_signal_emit_by_name(AGS_TACTABLE(drum), "change_bpm\0",
			bpm);

  //  audio = drum->machine.audio;
  //  devout = AGS_DEVOUT(audio->devout);

  //  tact = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tact));
  /*
  delay = (guint) round(((double)devout->frequency /
			 (double)devout->buffer_size) *
			(60.0 / bpm) *
			tact);
  */
  /* AgsDelayAudio * /
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    delay_audio = AGS_DELAY_AUDIO(list->data);
    delay_audio->delay = delay;
  }

  list = ags_recall_find_type(audio->recall,
			      AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    delay_audio = AGS_DELAY_AUDIO(list->data);
    delay_audio->delay = delay;
  }

  length = (guint) drum->length_spin->adjustment->value;
  stream_length = length * (delay + 1) + 1;

  /* AgsCountBeatsAudio * /
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->stream_length = stream_length;
  }

  list = ags_recall_find_type(audio->recall,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->stream_length = stream_length;
  }

  /* resize audio signals * /
  channel = drum->machine.audio->output;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, stream_length);

    channel = channel->next;
  }
  */
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
		   G_CALLBACK(ags_machine_open_response_callback), drum);
}

void
ags_drum_loop_button_callback(GtkWidget *button, AgsDrum *drum)
{
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  gboolean loop;

  loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;

  /* AgsCopyPatternAudio */
  list = ags_recall_find_type(drum->machine.audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop = loop;
  }

  list = ags_recall_find_type(drum->machine.audio->recall,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop = loop;
  }
}

void
ags_drum_run_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsDevout *devout;
  AgsGroupId group_id;

  devout = AGS_DEVOUT(drum->machine.audio->devout);

  if(GTK_TOGGLE_BUTTON(toggle_button)->active){
    AgsInitAudio *init_audio;
    AgsAppendAudio *append_audio;
    AgsStartDevout *start_devout;

    /* create init task */
    init_audio = ags_init_audio_new(drum->machine.audio,
				    FALSE, TRUE, FALSE);

    /* append AgsInitAudio */
    ags_devout_append_task(AGS_DEVOUT(drum->machine.audio->devout),
			   AGS_TASK(init_audio));

    /* create append task */
    append_audio = ags_append_audio_new(AGS_DEVOUT(drum->machine.audio->devout),
					drum->machine.audio->devout_play);

    /* append AgsAppendAudio */
    ags_devout_append_task(AGS_DEVOUT(drum->machine.audio->devout),
			   AGS_TASK(append_audio));

    /* create start task */
    start_devout = ags_start_devout_new(AGS_DEVOUT(drum->machine.audio->devout));

    /* append AgsStartDevout */
    ags_devout_append_task(AGS_DEVOUT(drum->machine.audio->devout),
			   AGS_TASK(start_devout));

  }else{
    /* abort code */
    if((AGS_DEVOUT_PLAY_DONE & (drum->machine.audio->devout_play->flags)) == 0){
      AgsCancelAudio *cancel_audio;

      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(drum->machine.audio, drum->machine.audio->devout_play->group_id,
					  drum->machine.audio->devout_play);

      /* append AgsCancelAudio */
      ags_devout_append_task(AGS_DEVOUT(drum->machine.audio->devout),
			     AGS_TASK(cancel_audio));
    }else{
      drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_REMOVE;
      drum->machine.audio->devout_play->flags &= (~AGS_DEVOUT_PLAY_DONE);
    }
  }
}

void
ags_drum_run_delay_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrum *drum)
{ 
  //  AgsDelay *delay;

  fprintf(stdout, "ags_drum_run_delay_done\n\0");

  //  delay = AGS_DELAY(recall);
  //  drum = AGS_DRUM(AGS_AUDIO(delay->recall.parent)->machine);
  //  drum->block_run = TRUE;
  drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
  gtk_toggle_button_set_active(drum->run, FALSE);
}

void
ags_drum_tact_callback(GtkWidget *option_menu, AgsDrum *drum)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsAudio *audio;
  AgsChannel *channel;
  AgsDelayAudio *delay_audio;
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  double bpm, tact;
  guint length, stream_length, delay;

  tact = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tact));
  g_signal_emit_by_name(AGS_TACTABLE(drum), "change_tact\0",
			tact);

  /*
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) drum);
  audio = drum->machine.audio;
  devout = AGS_DEVOUT(audio->devout);
  */
  //  bpm = gtk_adjustment_get_value(window->navigation->bpm->adjustment);

  /*
  delay = (guint) round(((double)devout->frequency /
			 (double)devout->buffer_size) *
			(60.0 / bpm) *
			tact);

  /* AgsDelayAudio * /
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    delay_audio = AGS_DELAY_AUDIO(list->data);
    delay_audio->delay = delay;
  }

  list = ags_recall_find_type(audio->recall,
			      AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    delay_audio = AGS_DELAY_AUDIO(list->data);
    delay_audio->delay = delay;
  }

  length = (guint) drum->length_spin->adjustment->value;
  stream_length = length * (delay + 1) + 1;

  /* AgsCountBeatsAudio * /
  list = ags_recall_find_type(drum->machine.audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->stream_length = stream_length;
  }

  list = ags_recall_find_type(drum->machine.audio->recall,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->stream_length = stream_length;
  }

  /* resize audio signal * /
  channel = drum->machine.audio->output;

  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, stream_length);

    channel = channel->next;
  }
  */
}

void
ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  /*
  AgsChannel *channel;
  AgsDelayAudio *delay_audio;
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  guint delay, length, stream_length;
  */
  gdouble duration;

  duration = GTK_SPIN_BUTTON(spin_button)->adjustment->value;
  g_signal_emit_by_name(AGS_TACTABLE(drum), "change_duration\0",
			duration);

//  channel = drum->machine.audio->output;

  /* AgsDelayAudio * /
  list = ags_recall_find_type(drum->machine.audio->play,
			      AGS_TYPE_DELAY_AUDIO);

  if(list != NULL){
    delay_audio = AGS_DELAY_AUDIO(list->data);
    delay = delay_audio->delay;
  }else
    delay = 0;

  length = (guint) GTK_SPIN_BUTTON(spin_button)->adjustment->value;
  stream_length = length * (delay + 1) + 1;

  /* AgsCountBeatsAudio * /
  list = ags_recall_find_type(drum->machine.audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->length = length;
    count_beats_audio->stream_length = stream_length;
  }

  list = ags_recall_find_type(drum->machine.audio->recall,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->length = length;
    count_beats_audio->stream_length = stream_length;
  }

  /* resize audio signal * /
  while(channel != NULL){
    ags_channel_resize_audio_signal(channel, stream_length);

    channel = channel->next;
  }
  */
}

void
ags_drum_index0_callback(GtkWidget *widget, AgsDrum *drum)
{
  if(drum->selected0 != NULL){
    GtkToggleButton *toggle_button;

    if(GTK_TOGGLE_BUTTON(widget) != drum->selected0){
      AgsCopyPatternAudio *copy_pattern_audio;
      GList *list;

      toggle_button = drum->selected0;
      drum->selected0 = NULL;
      gtk_toggle_button_set_active(toggle_button, FALSE);
      drum->selected0 = (GtkToggleButton*) widget;

      list = ags_recall_find_type(drum->machine.audio->play,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	copy_pattern_audio->i = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
      }

      list = ags_recall_find_type(drum->machine.audio->recall,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	copy_pattern_audio->i = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
      }
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
      AgsCopyPatternAudio *copy_pattern_audio;
      GList *list;

      toggle_button = drum->selected1;
      drum->selected1 = NULL;
      gtk_toggle_button_set_active(toggle_button, FALSE);
      drum->selected1 = (GtkToggleButton*) widget;

      list = ags_recall_find_type(drum->machine.audio->play,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	copy_pattern_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
      }

      list = ags_recall_find_type(drum->machine.audio->recall,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	copy_pattern_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
      }
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

  if((AGS_DRUM_BLOCK_PATTERN & (drum->flags)) != 0){
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
    GList *tasks;

    channel = drum->selected_pad->pad.channel;
    next_pad = channel->next_pad;

    tasks = NULL;

    while(channel != next_pad){
      AgsTogglePatternBit *toggle_pattern_bit;

      toggle_pattern_bit = ags_toggle_pattern_bit_new((AgsPattern *) channel->pattern->data,
						      index0, index1,
						      offset);

      tasks = g_list_prepend(tasks,
			     toggle_pattern_bit);

      channel = channel->next;
    }

    /* append AgsTogglePatternBit */
    ags_devout_append_tasks(AGS_DEVOUT(drum->machine.audio->devout),
			    tasks);
  }else{
    AgsTogglePatternBit *toggle_pattern_bit;
    
    toggle_pattern_bit = ags_toggle_pattern_bit_new((AgsPattern *) drum->selected_pad->pad.selected_line->channel->pattern->data,
						    index0, index1,
						    offset);

    /* append AgsTogglePatternBit */
    ags_devout_append_task(AGS_DEVOUT(drum->machine.audio->devout),
			   AGS_TASK(toggle_pattern_bit));
  }
}

void
ags_drum_offset_callback(GtkWidget *widget, AgsDrum *drum)
{
  ags_drum_set_pattern(drum);
}
