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

#include <ags/X/machine/ags_matrix_callbacks.h>

#include <ags/object/ags_application_context.h>

#include <ags/widget/ags_led.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/thread/ags_audio_loop.h>

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

      guint64 index1;

      GValue recall_value = {0,};

      audio = AGS_MACHINE(matrix)->audio;

      /* refresh GUI */
      toggle = matrix->selected;
      matrix->selected = NULL;

      gtk_toggle_button_set_active(toggle, FALSE);

      matrix->selected = (GtkToggleButton*) widget;

      ags_cell_pattern_paint(matrix->cell_pattern);

      /*  */
      AGS_MACHINE(matrix)->bank_1 = 
	index1 = ((guint) g_ascii_strtoull(matrix->selected->button.label_text, NULL, 10)) - 1;

      /* recall */
      list = ags_recall_find_type(audio->recall, AGS_TYPE_COPY_PATTERN_AUDIO);
  
      if(list != NULL){
	recall_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
      }

      g_value_init(&recall_value, G_TYPE_UINT64);
      g_value_set_uint64(&recall_value, index1);

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

void
ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix)
{
  AgsWindow *window;

  AgsApplySequencerLength *apply_sequencer_length;
  
  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;
  
  gdouble length;

  pthread_mutex_t *application_mutex;

  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(matrix));

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);
  
  /* find task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  length = GTK_SPIN_BUTTON(spin_button)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
							  length);

  ags_task_thread_append_task(task_thread,
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

    count_beats_audio->sequencer_loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }

  list = AGS_MACHINE(matrix)->audio->recall;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    count_beats_audio->sequencer_loop->port_value.ags_port_boolean = loop;

    list = list->next;
  }
}

void
ags_matrix_tact_callback(AgsAudio *audio,
			 AgsRecallID *recall_id,
			 AgsMatrix *matrix)
{
  AgsWindow *window;

  AgsCountBeatsAudio *play_count_beats_audio;
  AgsCountBeatsAudioRun *play_count_beats_audio_run;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  GList *list;

  guint counter, active_led;
  gdouble active_led_old, active_led_new;

  GValue value = {0,};

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  if((AGS_RECALL_ID_SEQUENCER & (recall_id->flags)) == 0){
    return;
  }

  gdk_threads_enter();
  
  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) matrix,
						 AGS_TYPE_WINDOW);

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);
  
  /* find task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) AGS_MACHINE(matrix)->audio);
  
  pthread_mutex_unlock(application_mutex);

  /* get some recalls */
  pthread_mutex_lock(audio_mutex);

  list = ags_recall_find_type(audio->play,
			      AGS_TYPE_COUNT_BEATS_AUDIO);
  
  if(list != NULL){
    play_count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
  }

  list = ags_recall_find_type_with_recycling_context(audio->play,
						     AGS_TYPE_COUNT_BEATS_AUDIO_RUN,
						     (GObject *) recall_id->recycling_context);
  
  if(list != NULL){
    play_count_beats_audio_run = AGS_COUNT_BEATS_AUDIO_RUN(list->data);
  }

  if(play_count_beats_audio == NULL ||
     play_count_beats_audio_run == NULL){
    pthread_mutex_unlock(audio_mutex);

    gdk_threads_leave();
    
    return;
  }

  /* set optical feedback */
  active_led_new = play_count_beats_audio_run->sequencer_counter;
  matrix->cell_pattern->active_led = (guint) active_led_new;

  if(active_led_new == 0){
    g_value_init(&value, G_TYPE_DOUBLE);
    ags_port_safe_read(play_count_beats_audio->sequencer_loop_end,
		       &value);

    active_led_old = g_value_get_double(&value) - 1.0;
    g_value_unset(&value);
  }else{
    active_led_old = (gdouble) matrix->cell_pattern->active_led - 1.0;
  }

  pthread_mutex_unlock(audio_mutex);

  gdk_threads_leave();
}

void
ags_matrix_done_callback(AgsAudio *audio,
			 AgsRecallID *recall_id,
			 AgsMatrix *matrix)
{
  GList *playback;
  gboolean all_done;

  playback = AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback;

  /* check unset */
  all_done = TRUE;

  while(playback != NULL){
    if(AGS_PLAYBACK(playback->data)->recall_id[1] != NULL){
      all_done = FALSE;
      break;
    }

    playback = playback->next;
  }

  if(all_done){
    GList *list, *list_start;
    
    /* unset led */
    list_start = 
      list = gtk_container_get_children(GTK_CONTAINER(matrix->cell_pattern->led));

    while(list != NULL){
      ags_led_unset_active(AGS_LED(list->data));
	
      list = list->next;
    }

    g_list_free(list_start);
  }
}
