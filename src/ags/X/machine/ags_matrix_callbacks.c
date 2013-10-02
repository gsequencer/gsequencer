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

#include <ags/X/machine/ags_matrix_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_start_devout.h>
#include <ags/audio/task/ags_cancel_audio.h>
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

#include <math.h>

extern const char *AGS_MATRIX_INDEX;

void
ags_matrix_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsAudio *audio;
  AgsApplyBpm *apply_bpm;
  AgsApplyTact *apply_tact;
  AgsApplySequencerLength *apply_sequencer_length;
  double bpm, tact, length;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  audio = matrix->machine.audio;
  audio->devout = (GObject *) window->devout;

  matrix->machine.name = g_strdup_printf("Default %d\0", window->counter->matrix);
  window->counter->matrix++;

  devout = AGS_DEVOUT(audio->devout);


  /* bpm */
  bpm = window->navigation->bpm->adjustment->value;

  /*
  if(bpm > 60.0){
    bpm = exp2(-1.0 * 60.0 / bpm);
  }else{
    bpm = exp2(60.0 / bpm);
  }
  */

  apply_bpm = ags_apply_bpm_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
				bpm);
  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_bpm));

  /* tact */
  tact = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) matrix->tact));
 
  apply_tact = ags_apply_tact_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
  				  tact);
  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_tact));

  /* length */
  length = GTK_SPIN_BUTTON(matrix->length_spin)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
							  length);
  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_sequencer_length));

  //  fprintf(stdout, "ags_matrix_parent_set_callback: delay_audio->delay = %d\n\0", delay_audio->delay);
}

void
ags_matrix_run_callback(GtkWidget *toggle_button, AgsMatrix *matrix)
{
  AgsDevout *devout;
  AgsGroupId group_id;

  devout = AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout);

  if(GTK_TOGGLE_BUTTON(toggle_button)->active){
    AgsInitAudio *init_audio;
    AgsAppendAudio *append_audio;
    AgsStartDevout *start_devout;

    /* create init task */
    init_audio = ags_init_audio_new(AGS_MACHINE(matrix)->audio,
				    FALSE, TRUE, FALSE);

    /* append AgsInitAudio */
    ags_task_thread_append_task(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout)->task_thread,
				AGS_TASK(init_audio));

    /* create append task */
    append_audio = ags_append_audio_new(G_OBJECT(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout)->audio_loop),
					AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play));

    /* append AgsAppendAudio */
    ags_task_thread_append_task(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout)->task_thread,
			   AGS_TASK(append_audio));

    /* create start task */
    start_devout = ags_start_devout_new(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout));

    /* append AgsStartDevout */
    ags_task_thread_append_task(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout)->task_thread,
				AGS_TASK(start_devout));
  }else{
    /* abort code */
    if((AGS_DEVOUT_PLAY_DONE & (AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play)->flags)) == 0){
      AgsCancelAudio *cancel_audio;

      /* create cancel task */
      cancel_audio = ags_cancel_audio_new(AGS_MACHINE(matrix)->audio, AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play)->group_id[1],
					  AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play));

      /* append AgsAppendAudio */
      ags_task_thread_append_task(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout)->task_thread,
				  AGS_TASK(cancel_audio));
    }else{
      AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play)->flags |= AGS_DEVOUT_PLAY_REMOVE;
      AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play)->flags &= (~AGS_DEVOUT_PLAY_DONE);
    }
  }
}

void
ags_matrix_sequencer_count_callback(AgsDelayAudioRun *delay_audio_run, guint nth_run,
				    guint attack,
				    AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsRecallContainer *recall_container;
  AgsCountBeatsAudioRun *count_beats_audio_run;
  AgsToggleLed *toggle_led;
  GList *list;
  guint counter, active_led;
  
  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) matrix, AGS_TYPE_WINDOW));

  list = AGS_RECALL_CONTAINER(AGS_RECALL(matrix->recall_count_beats_audio_run)->container)->recall_audio_run;
  list = ags_recall_find_group_id(list,
				  AGS_RECALL(delay_audio_run)->recall_id->group_id);

  count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  counter = count_beats_audio_run->sequencer_counter;

  matrix->active_led = counter;

  if(matrix->active_led == 0){
    active_led = AGS_COUNT_BEATS_AUDIO(AGS_RECALL_AUDIO_RUN(count_beats_audio_run)->recall_audio)->sequencer_loop_end - 1;
  }else{
    active_led = matrix->active_led - 1;
  }

  toggle_led = ags_toggle_led_new(gtk_container_get_children(GTK_CONTAINER(matrix->led)),
				  matrix->active_led,
				  active_led);

  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(toggle_led));
}

void
ags_matrix_index_callback(GtkWidget *widget, AgsMatrix *matrix)
{
  GtkToggleButton *toggle;

  if(matrix->selected != NULL){
    if(GTK_TOGGLE_BUTTON(widget) != matrix->selected){
      toggle = matrix->selected;
      matrix->selected = NULL;
      gtk_toggle_button_set_active(toggle, FALSE);
      matrix->selected = (GtkToggleButton*) widget;
      ags_matrix_draw_matrix(matrix);

      matrix->play_copy_pattern_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_MATRIX_INDEX));
      matrix->recall_copy_pattern_audio->j = GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_MATRIX_INDEX));
    }else{
      toggle = matrix->selected;
      matrix->selected = NULL;
      gtk_toggle_button_set_active(toggle, TRUE);
      matrix->selected = toggle;
    }
  }
}

gboolean
ags_matrix_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsMatrix *matrix)
{
  ags_matrix_draw_gutter(matrix);

  return(FALSE);
}

gboolean
ags_matrix_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsMatrix *matrix)
{
  if (event->button == 1){
    AgsChannel *channel;
    guint i, j;

    i = (guint) floor((double) event->y / 10.0);
    j = (guint) floor((double) event->x / 12.0);

    channel = ags_channel_nth(AGS_MACHINE(matrix)->audio->input, i + (guint) matrix->adjustment->value);

    ags_pattern_toggle_bit((AgsPattern *) channel->pattern->data, 0, strtol(matrix->selected->button.label_text, NULL, 10) -1, j);
    ags_matrix_redraw_gutter_point(matrix, channel, j, i);
  }else if (event->button == 3){
  }

  return(FALSE);
}

void
ags_matrix_adjustment_value_changed_callback(GtkWidget *widget, AgsMatrix *matrix)
{
  ags_matrix_draw_matrix(matrix);
}

void
ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsApplySequencerLength *apply_sequencer_length;
  gdouble length;

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(matrix));

  length = GTK_SPIN_BUTTON(spin_button)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
							  length);

  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_sequencer_length));
}

void
ags_matrix_tact_callback(GtkWidget *option_menu, AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsApplyTact *apply_tact;
  gdouble tact;

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(matrix));

  tact = exp2(4.0 - (double) gtk_option_menu_get_history((GtkOptionMenu *) matrix->tact));

  apply_tact = ags_apply_tact_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
				  tact);

  ags_task_thread_append_task(AGS_DEVOUT(window->devout)->task_thread,
			      AGS_TASK(apply_tact));
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  gboolean loop;

  loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;

  /* AgsCopyPatternAudio */
  list = ags_recall_find_type(AGS_MACHINE(matrix)->audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop = loop;
  }

  list = ags_recall_find_type(AGS_MACHINE(matrix)->audio->recall,
			      AGS_TYPE_COUNT_BEATS_AUDIO);

  if(list != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop = loop;
  }
}

void
ags_matrix_run_delay_done(AgsRecall *recall, AgsMatrix *matrix)
{
  fprintf(stdout, "ags_matrix_run_delay_done\n\0");

  //  delay = AGS_DELAY(recall);
  //  matrix = AGS_MATRIX(AGS_AUDIO(delay->recall.parent)->machine);
  //  matrix->block_run = TRUE;
  AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play)->flags |= AGS_DEVOUT_PLAY_DONE;
  gtk_toggle_button_set_active(matrix->run, FALSE);

}

void
ags_matrix_copy_pattern_done(AgsRecall *recall, AgsMatrix *matrix)
{
  //  AgsCopyPattern *copy_pattern;

  //  copy_pattern = AGS_COPY_PATTERN(recall);
  recall->flags |= AGS_RECALL_HIDE;
  /*
  g_list_free(copy_pattern->destination);
  copy_pattern->destination = NULL;
  */
}

void
ags_matrix_copy_pattern_cancel(AgsRecall *recall, AgsMatrix *matrix)
{
}

void
ags_matrix_copy_pattern_loop(AgsRecall *recall, AgsMatrix *matrix)
{

}

void
ags_matrix_play_done(AgsRecall *recall, AgsMatrix *matrix)
{
  /*
  matrix->play_ref++;

  if(matrix->play_ref == AGS_MACHINE(matrix)->audio->output_lines){
    matrix->play_ref = 0;
    AGS_DEVOUT_PLAY(AGS_MACHINE(matrix)->audio->devout_play)->flags |= AGS_DEVOUT_PLAY_DONE;
    gtk_toggle_button_set_active(matrix->run, FALSE);
  }
  */
}

void
ags_matrix_play_cancel(AgsRecall *recall, AgsMatrix *matrix)
{
}

