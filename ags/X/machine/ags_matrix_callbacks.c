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

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <math.h>

void
ags_matrix_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsMatrix *matrix)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);

  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_MATRIX)->counter);

  g_object_set(AGS_MACHINE(matrix),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_MATRIX);

  g_free(str);
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
  AgsGuiThread *gui_thread;
  
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
  
  /* find gui_thread */
  gui_thread = (AgsGuiThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_GUI_THREAD);

  length = GTK_SPIN_BUTTON(spin_button)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(G_OBJECT(AGS_MACHINE(matrix)->audio),
							  length);

  ags_gui_thread_schedule_task(gui_thread,
			       apply_sequencer_length);
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  AgsCountBeatsAudio *count_beats_audio;

  AgsMutexManager *mutex_manager;

  GList *list;

  gboolean loop;

  GValue value = {0,};
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) AGS_MACHINE(matrix)->audio);
  
  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(audio_mutex);

  loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;

  /* AgsCopyPatternAudio */
  list = AGS_MACHINE(matrix)->audio->play;

  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  
  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    g_value_set_boolean(&value,
			loop);
    ags_port_safe_write(count_beats_audio->sequencer_loop,
			&value);

    list = list->next;
  }

  list = AGS_MACHINE(matrix)->audio->recall;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);

    g_value_set_boolean(&value,
			loop);
    ags_port_safe_write(count_beats_audio->sequencer_loop,
			&value);

    list = list->next;
  }

  pthread_mutex_unlock(audio_mutex);
}

void
ags_matrix_done_callback(AgsMatrix *matrix,
			 AgsRecallID *recall_id,
			 gpointer data)
{
  AgsAudio *audio;
  
  AgsMutexManager *mutex_manager;

  GList *playback;

  gboolean all_done;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  audio = AGS_MACHINE(matrix)->audio;

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);
  
  /* check unset */
  pthread_mutex_lock(audio_mutex);

  playback = AGS_PLAYBACK_DOMAIN(audio->playback_domain)->playback;

  all_done = TRUE;

  while(playback != NULL){
    if(AGS_PLAYBACK(playback->data)->recall_id[1] != NULL){
      all_done = FALSE;
      break;
    }

    playback = playback->next;
  }

  pthread_mutex_unlock(audio_mutex);

  /* all done */
  if(all_done){
    ags_led_array_unset_all(matrix->cell_pattern->hled_array);
  }
}
