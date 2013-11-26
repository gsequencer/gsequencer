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

#include <ags/main.h>

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
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_cancel_audio.h>
#include <ags/audio/task/ags_link_channel.h>
#include <ags/audio/task/ags_start_devout.h>
#include <ags/audio/task/ags_toggle_pattern_bit.h>
#include <ags/audio/task/ags_toggle_led.h>

#include <ags/audio/task/recall/ags_apply_bpm.h>
#include <ags/audio/task/recall/ags_apply_tact.h>
#include <ags/audio/task/recall/ags_apply_sequencer_length.h>

#include <ags/audio/recall/ags_delay_audio.h>
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

void ags_drum_start_devout_failure(AgsTask *task, GError *error);

void
ags_drum_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsDrum *drum)
{
  AgsWindow *window;
  AgsAudio *audio;
  AgsApplyBpm *apply_bpm;
  AgsApplyTact *apply_tact;
  AgsApplySequencerLength *apply_sequencer_length;
  double bpm, tact, length;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));

  audio = drum->machine.audio;
  audio->devout = (GObject *) window->devout;

  AGS_MACHINE(drum)->name = g_strdup_printf("Default %d\0",
					    ags_window_find_machine_counter(window, AGS_TYPE_DRUM)->counter);
  ags_window_increment_machine_counter(window,
				       AGS_TYPE_DRUM);

  /* bpm */
  bpm = window->navigation->bpm->adjustment->value;

  /*  if(bpm > 60.0){
    bpm = exp2(-1.0 * 60.0 / bpm);
  }else{
    bpm = exp2(60.0 / bpm);
    } * /

  apply_bpm = ags_apply_bpm_new(G_OBJECT(AGS_MACHINE(drum)->audio),
				bpm);
  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_bpm));

  /* tact * /
  tact = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tact));

  apply_tact = ags_apply_tact_new(G_OBJECT(AGS_MACHINE(drum)->audio),
  				  tact);
  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
  			      AGS_TASK(apply_tact));

  /* length * /
  length = GTK_SPIN_BUTTON(drum->length_spin)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(drum)->audio),
							  length);
  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_sequencer_length));
  */
}

void
ags_drum_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run,
				  guint nth_run,
				  guint delay, guint attack,
				  AgsDrum *drum)
{
  AgsWindow *window;
  AgsAudio *audio;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsToggleLed *toggle_led;
  GList *list;
  guint counter, active_led;
  
  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) drum, AGS_TYPE_WINDOW));

  audio = AGS_MACHINE(drum)->audio;

  /* get some recalls */
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);
  
  if(list != NULL){
    play_count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
  }

  list = ags_recall_find_type_with_group_id(audio->play,
					    AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
					    AGS_RECALL(delay_audio_run)->recall_id->group_id);
  
  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  }

  /* set optical feedback */
  counter = (guint) floor(play_count_beats_audio_run->sequencer_counter);

  drum->active_led = counter;

  if(drum->active_led == 0){
    active_led =  (guint) floor(play_count_beats_audio->sequencer_loop_end->port_value.ags_port_double) - 1;
  }else{
    active_led = drum->active_led - 1;
  }

  toggle_led = ags_toggle_led_new(gtk_container_get_children(GTK_CONTAINER(drum->led)),
				  drum->active_led,
				  active_led);

  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(toggle_led));
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
  g_object_set_data(G_OBJECT(file_chooser), "create\0", (gpointer) check_button);

  check_button = (GtkCheckButton *) gtk_check_button_new_with_label(g_strdup("overwrite existing links\0"));
  gtk_toggle_button_set_active((GtkToggleButton *) check_button, TRUE);
  gtk_box_pack_start((GtkBox *) GTK_DIALOG(file_chooser)->vbox, (GtkWidget *) check_button, FALSE, FALSE, 0);
  g_object_set_data(G_OBJECT(file_chooser), "overwrite\0", (gpointer) check_button);

  gtk_widget_show_all(GTK_WIDGET(file_chooser));

  g_signal_connect(G_OBJECT(file_chooser), "response\0",
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
  list = AGS_MACHINE(drum)->audio->play;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }

  list = AGS_MACHINE(drum)->audio->recall;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }

}

void
ags_drum_run_callback(GtkWidget *toggle_button, AgsDrum *drum)
{
  AgsDevout *devout;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;
  AgsDevoutThread *devout_thread;
  AgsGroupId group_id;

  devout = AGS_DEVOUT(AGS_MACHINE(drum)->audio->devout);

  audio_loop = devout->audio_loop;
  task_thread = devout->task_thread;
  devout_thread = devout->devout_thread;

  if(GTK_TOGGLE_BUTTON(toggle_button)->active){
    AgsInitAudio *init_audio;
    AgsAppendAudio *append_audio;
    AgsStartDevout *start_devout;
    GList *tasks;

    printf("drum: on\n\0");

    tasks = NULL;

    /* create init task */
    init_audio = ags_init_audio_new(AGS_MACHINE(drum)->audio,
				    FALSE, TRUE, FALSE);
    tasks = g_list_prepend(tasks,
			   init_audio);

    /* create append task */
    append_audio = ags_append_audio_new(G_OBJECT(audio_loop),
					AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play));

    tasks = g_list_prepend(tasks,
			   append_audio);

    /* create start task */
    start_devout = ags_start_devout_new(devout);

    tasks = g_list_prepend(tasks,
			   start_devout);

    /* append tasks */
    tasks = g_list_reverse(tasks);
    
    ags_task_thread_append_tasks(task_thread,
				 tasks);

  }else{
    printf("drum: off\n\0");

    /* abort code */
    if((AGS_DEVOUT_PLAY_DONE & (AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play)->flags)) == 0){
      AgsCancelAudio *cancel_audio;

      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(AGS_MACHINE(drum)->audio, AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play)->group_id[1],
					  AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play));

      /* append AgsCancelAudio */
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(cancel_audio));
    }else{
      AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play)->flags |= AGS_DEVOUT_PLAY_REMOVE;
      AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play)->flags &= (~AGS_DEVOUT_PLAY_DONE);
    }
  }
}

void
ags_drum_start_devout_failure(AgsTask *task, GError *error)
{
  AgsWindow *window;
  GtkMessageDialog *dialog;
  AgsAudioLoop *audio_loop;

  /* show error message */
  window = AGS_MAIN(AGS_START_DEVOUT(task)->devout->main)->window;
  
  dialog = (GtkMessageDialog *) gtk_message_dialog_new(GTK_WINDOW(window),
						       GTK_DIALOG_MODAL,
						       GTK_MESSAGE_ERROR,
						       GTK_BUTTONS_CLOSE,
						       error->message);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(GTK_WIDGET(dialog));
}

void
ags_drum_run_delay_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrum *drum)
{ 
  //  AgsDelay *delay;

  fprintf(stdout, "ags_drum_run_delay_done\n\0");

  //  delay = AGS_DELAY(recall);
  //  drum = AGS_DRUM(AGS_AUDIO(delay->recall.parent)->machine);
  //  drum->block_run = TRUE;
  AGS_DEVOUT_PLAY(AGS_MACHINE(drum)->audio->devout_play)->flags |= AGS_DEVOUT_PLAY_DONE;
  gtk_toggle_button_set_active(drum->run, FALSE);
}

void
ags_drum_tact_callback(GtkWidget *option_menu, AgsDrum *drum)
{
  AgsWindow *window;
  AgsApplyTact *apply_tact;
  gdouble tact;

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(drum));

  tact = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) drum->tact));

  apply_tact = ags_apply_tact_new(G_OBJECT(AGS_MACHINE(drum)->audio),
				  tact);

  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_tact));
}

void
ags_drum_length_spin_callback(GtkWidget *spin_button, AgsDrum *drum)
{
  AgsWindow *window;
  AgsApplySequencerLength *apply_sequencer_length;
  gdouble length;

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(drum));

  length = GTK_SPIN_BUTTON(spin_button)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(drum)->audio),
							  length);

  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_sequencer_length));
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

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->play,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	copy_pattern_audio->i = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
      }

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->recall,
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

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->play,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	copy_pattern_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_DRUM_INDEX));
      }

      list = ags_recall_find_type(AGS_MACHINE(drum)->audio->recall,
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
    ags_task_thread_append_tasks(AGS_DEVOUT(AGS_MACHINE(drum)->audio->devout)->task_thread,
				 tasks);
  }else{
    AgsLine *selected_line;
    AgsTogglePatternBit *toggle_pattern_bit;    
    GList *line;

    line = ags_line_find_next_grouped(gtk_container_get_children(GTK_CONTAINER(AGS_PAD(drum->selected_pad)->expander_set)));
    selected_line = AGS_LINE(line->data);

    toggle_pattern_bit = ags_toggle_pattern_bit_new(selected_line->channel->pattern->data,
						    index0, index1,
						    offset);

    /* append AgsTogglePatternBit */
    ags_task_thread_append_task(AGS_DEVOUT(AGS_MACHINE(drum)->audio->devout)->task_thread,
				AGS_TASK(toggle_pattern_bit));
  }
}

void
ags_drum_offset_callback(GtkWidget *widget, AgsDrum *drum)
{
  ags_drum_set_pattern(drum);
}
