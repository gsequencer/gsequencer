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

#include <ags/main.h>

#include <ags/widget/ags_led.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/task/ags_toggle_led.h>
#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/audio/task/recall/ags_apply_bpm.h>
#include <ags/audio/task/recall/ags_apply_sequencer_length.h>

#include <ags/audio/recall/ags_delay_audio.h>
#include <ags/audio/recall/ags_count_beats_audio.h>
#include <ags/audio/recall/ags_count_beats_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

#include <math.h>

void ags_matrix_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				     AgsMatrix *matrix);

extern const char *AGS_MATRIX_INDEX;

void
ags_matrix_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMatrix *matrix)
{
  AgsWindow *window;

  if(old_parent != NULL)
    return;

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  AGS_MACHINE(matrix)->name = g_strdup_printf("Default %d\0",
					      ags_window_find_machine_counter(window, AGS_TYPE_MATRIX)->counter);
  ags_window_increment_machine_counter(window,
				       AGS_TYPE_MATRIX);
}

void
ags_matrix_index_callback(GtkWidget *widget, AgsMatrix *matrix)
{
  GtkToggleButton *toggle;

  if(matrix->selected != NULL){
    if(GTK_TOGGLE_BUTTON(widget) != matrix->selected){
      AgsAudio *audio;
      AgsCopyPatternAudio *recall_copy_pattern_audio, *play_copy_pattern_audio;
      GList *list;
      GValue play_value = {0,};
      GValue recall_value = {0,};

      audio = AGS_MACHINE(matrix)->audio;

      /* refresh GUI */
      toggle = matrix->selected;
      matrix->selected = NULL;

      gtk_toggle_button_set_active(toggle, FALSE);

      matrix->selected = (GtkToggleButton*) widget;
      ags_matrix_draw_matrix(matrix);

      /* modify port */
      /* play */
      list = ags_recall_find_type(audio->play, AGS_TYPE_COPY_PATTERN_AUDIO);
  
      if(list != NULL){
	play_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
      }

      g_value_init(&play_value, G_TYPE_UINT);
      g_value_set_uint(&play_value, GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_MATRIX_INDEX)));

      ags_port_safe_write(play_copy_pattern_audio->bank_index_1, &play_value);

      g_value_unset(&play_value);

      /* recall */
      list = ags_recall_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO);
  
      if(list != NULL){
	recall_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
      }

      g_value_init(&recall_value, G_TYPE_UINT);
      g_value_set_uint(&recall_value, GPOINTER_TO_UINT(g_object_get_data((GObject *) widget, AGS_MATRIX_INDEX)));

      ags_port_safe_write(recall_copy_pattern_audio->bank_index_1, &recall_value);

      g_value_unset(&recall_value);
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
    AgsTogglePatternBit *toggle_pattern_bit;
    AgsChannel *channel;
    guint i, j;

    i = (guint) floor((double) event->y / (double) AGS_MATRIX_CELL_HEIGHT);
    j = (guint) floor((double) event->x / (double) AGS_MATRIX_CELL_WIDTH);

    channel = ags_channel_nth(AGS_MACHINE(matrix)->audio->input, i + (guint) matrix->adjustment->value);

    toggle_pattern_bit = ags_toggle_pattern_bit_new(channel->pattern->data,
						    i + (guint) matrix->adjustment->value,
						    0, strtol(matrix->selected->button.label_text, NULL, 10) - 1,
						    j);
    g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
		     G_CALLBACK(ags_matrix_refresh_gui_callback), matrix);

    ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(AGS_DEVOUT(AGS_MACHINE(matrix)->audio->devout)->ags_main)->main_loop)->task_thread),
				AGS_TASK(toggle_pattern_bit));
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

  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(apply_sequencer_length));
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  AgsCountBeatsAudio *count_beats_audio;
  GList *list;
  gboolean loop;

  loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;

  /* AgsCopyPatternAudio */
  list = AGS_MACHINE(matrix)->audio->play;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }

  list = AGS_MACHINE(matrix)->audio->recall;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }
}

void
ags_matrix_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				AgsMatrix *matrix)
{
  AgsChannel *channel;
  guint line;

  channel = ags_channel_nth(AGS_MACHINE(matrix)->audio->input, toggle_pattern_bit->line);

  ags_matrix_redraw_gutter_point(matrix, channel, toggle_pattern_bit->bit, toggle_pattern_bit->line - (guint) matrix->adjustment->value);
}

void
ags_matrix_tact_callback(AgsAudio *audio,
			 AgsRecallID *recall_id,
			 AgsMatrix *matrix)
{
  AgsWindow *window;
  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;
  AgsToggleLed *toggle_led;
  GList *list;
  guint counter, active_led;
  gdouble active_led_old, active_led_new;
  GValue value = {0,};
  
  window = AGS_WINDOW(gtk_widget_get_ancestor((GtkWidget *) matrix, AGS_TYPE_WINDOW));

  /* get some recalls */
  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);
  
  if(list != NULL){
    play_count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
  }

  list = ags_recall_find_type_with_recycling_container(audio->play,
						       AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
						       (GObject *) recall_id->recycling_container);
  
  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  }

  /* set optical feedback */
  active_led_new = play_count_beats_audio_run->sequencer_counter;
  matrix->active_led = (guint) active_led_new;

  if(active_led_new == 0){
    g_value_init(&value, G_TYPE_DOUBLE);
    ags_port_safe_read(play_count_beats_audio->sequencer_loop_end,
		       &value);

    active_led_old = g_value_get_double(&value) - 1.0;
  }else{
    active_led_old = (gdouble) matrix->active_led - 1.0;
  }

  //FIXME:JK: memory leak of GList
  toggle_led = ags_toggle_led_new(gtk_container_get_children(GTK_CONTAINER(matrix->led)),
				  (guint) active_led_new,
				  (guint) active_led_old);

  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(toggle_led));
}

void
ags_matrix_done_callback(AgsAudio *audio,
			 AgsRecallID *recall_id,
			 AgsMatrix *matrix)
{
  //TODO:JK: implement me  
}
