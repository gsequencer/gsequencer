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
  if(matrix->selected != NULL){
    GtkToggleButton *toggle;

    if(GTK_TOGGLE_BUTTON(widget) != matrix->selected){
      AgsPort *port;
      
      AgsCopyPatternAudio *recall_copy_pattern_audio, *play_copy_pattern_audio;

      GList *start_list, *list;

      guint64 index1;

      toggle = matrix->selected;
      matrix->selected = NULL;

      gtk_toggle_button_set_active(toggle,
				   FALSE);

      matrix->selected = (GtkToggleButton*) widget;

      ags_cell_pattern_paint(matrix->cell_pattern);

      /* calculate index 1 */
      AGS_MACHINE(matrix)->bank_1 = 
	index1 = ((guint) g_ascii_strtoull(matrix->selected->button.label_text, NULL, 10)) - 1;

      /* play - set port */
      g_object_get(AGS_MACHINE(matrix)->audio,
		   "play", &start_list,
		   NULL);
      
      list = ags_recall_find_type(start_list,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value,
		     G_TYPE_UINT64);
	
	g_value_set_uint64(&value,
			   index1);

	play_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	g_object_get(play_copy_pattern_audio,
		     "bank-index-1", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_value_unset(&value);
      }

      g_list_free(start_list);
      
      /* recall - set port */
      g_object_get(AGS_MACHINE(matrix)->audio,
		   "recall", &start_list,
		   NULL);
      
      list = ags_recall_find_type(start_list,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value,
		     G_TYPE_UINT64);
	
	g_value_set_uint64(&value,
			   index1);

	recall_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	g_object_get(recall_copy_pattern_audio,
		     "bank-index-1", &port,
		     NULL);

	ags_port_safe_write(port,
			    &value);

	g_value_unset(&value);
      }
      
      g_list_free(start_list);

      gtk_widget_queue_draw(matrix->cell_pattern->drawing_area);
    }else{
      matrix->selected = NULL;
      
      gtk_toggle_button_set_active(toggle,
				   TRUE);
      
      matrix->selected = widget;
    }
  }
}

void
ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix)
{
  AgsWindow *window;

  AgsGuiThread *gui_thread;

  AgsApplySequencerLength *apply_sequencer_length;
  
  AgsApplicationContext *application_context;
  
  gdouble length;

  /* get window and application_context  */
  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(matrix));

  application_context = (AgsApplicationContext *) window->application_context;

  gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

  /* task - apply length */
  length = GTK_SPIN_BUTTON(spin_button)->adjustment->value;

  apply_sequencer_length = ags_apply_sequencer_length_new(AGS_MACHINE(matrix)->audio,
							  length);

  ags_gui_thread_schedule_task(gui_thread,
			       apply_sequencer_length);
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  AgsPort *port;
  AgsCountBeatsAudio *count_beats_audio;

  GList *start_list, *list;

  gboolean loop;  

  loop = (GTK_TOGGLE_BUTTON(button)->active) ? TRUE: FALSE;

  /* play - count beats audio */
  g_object_get(AGS_MACHINE(matrix)->audio,
	       "play", &start_list,
	       NULL);
  
  list = start_list;
  
  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    GValue value = {0,};
    
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
    g_object_get(count_beats_audio,
		 "sequencer-loop", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_BOOLEAN);
    g_value_set_boolean(&value,
			loop);

    ags_port_safe_write(port,
			&value);

    g_value_unset(&value);
    
    /* iterate */
    list = list->next;
  }


  g_list_free(start_list);

  /* recall - count beats audio */
  g_object_get(AGS_MACHINE(matrix)->audio,
	       "recall", &start_list,
	       NULL);

  list = start_list;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
    GValue value = {0,};
    
    count_beats_audio = AGS_COUNT_BEATS_AUDIO(list->data);
    g_object_get(count_beats_audio,
		 "sequencer-loop", &port,
		 NULL);

    g_value_init(&value,
		 G_TYPE_BOOLEAN);
    g_value_set_boolean(&value,
			loop);

    ags_port_safe_write(port,
			&value);

    g_value_unset(&value);
    
    /* iterate */
    list = list->next;
  }

  g_list_free(start_list);  
}

void
ags_matrix_done_callback(AgsMatrix *matrix,
			 AgsRecallID *recall_id,
			 gpointer data)
{
  AgsAudio *audio;
  AgsPlaybackDomain *playback_domain;
  
  GList *start_playback, *playback;

  gboolean all_done;

  audio = AGS_MACHINE(matrix)->audio;
  g_object_get(audio,
	       "playback-domain", &playback_domain,
	       NULL);
  
  /* check unset */
  g_object_get(playback_domain,
	       "playback", &start_playback,
	       NULL);
  
  playback = start_playback;
  all_done = TRUE;

  while(playback != NULL){
    if(ags_playback_get_recall_id(AGS_PLAYBACK(playback->data), AGS_SOUND_SCOPE_SEQUENCER) != NULL){
      all_done = FALSE;

      break;
    }

    playback = playback->next;
  }

  g_list_free(start_playback);
  
  /* all done */
  if(all_done){
    ags_led_array_unset_all(matrix->cell_pattern->hled_array);
  }
}

