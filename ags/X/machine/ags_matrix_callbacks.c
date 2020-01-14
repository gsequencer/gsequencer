/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_navigation.h>

#include <math.h>

void
ags_matrix_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsMatrix *matrix)
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

      gchar *str;

      guint64 index1;

      toggle = matrix->selected;
      matrix->selected = NULL;

      gtk_toggle_button_set_active(toggle,
				   FALSE);

      matrix->selected = (GtkToggleButton*) widget;

      gtk_widget_queue_draw(matrix->cell_pattern->drawing_area);

      /* calculate index 1 */
      str = gtk_button_get_label(matrix->selected);
      index1 =
	AGS_MACHINE(matrix)->bank_1 = ((guint) g_ascii_strtoull(str, NULL, 10)) - 1;

      /* play - set port */
      g_object_get(AGS_MACHINE(matrix)->audio,
		   "play", &start_list,
		   NULL);
      
      list = ags_recall_find_type(start_list,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value,
		     G_TYPE_FLOAT);
	
	g_value_set_float(&value,
			  (gfloat) index1);

	play_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	g_object_get(play_copy_pattern_audio,
		     "bank-index-1", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
	
	g_value_unset(&value);
      }

      g_list_free_full(start_list,
		       g_object_unref);
      
      /* recall - set port */
      g_object_get(AGS_MACHINE(matrix)->audio,
		   "recall", &start_list,
		   NULL);
      
      list = ags_recall_find_type(start_list,
				  AGS_TYPE_COPY_PATTERN_AUDIO);

      if(list != NULL){
	GValue value = {0,};

	g_value_init(&value,
		     G_TYPE_FLOAT);
	
	g_value_set_float(&value,
			  (float) index1);

	recall_copy_pattern_audio = AGS_COPY_PATTERN_AUDIO(list->data);
	g_object_get(recall_copy_pattern_audio,
		     "bank-index-1", &port,
		     NULL);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	g_value_unset(&value);
      }
      
      g_list_free_full(start_list,
		       g_object_unref);

      gtk_widget_queue_draw((GtkWidget *) matrix->cell_pattern->drawing_area);
    }else{
      matrix->selected = NULL;
      
      gtk_toggle_button_set_active((GtkToggleButton *) widget,
				   TRUE);
      
      matrix->selected = (GtkToggleButton *) widget;
    }
  }
}

void
ags_matrix_length_spin_callback(GtkWidget *spin_button, AgsMatrix *matrix)
{
  AgsWindow *window;

  AgsApplySequencerLength *apply_sequencer_length;
  
  AgsApplicationContext *application_context;
  
  gdouble length;

  application_context = ags_application_context_get_instance();

  /* get window and application_context  */
  window = (AgsWindow *) gtk_widget_get_toplevel(GTK_WIDGET(matrix));

  /* task - apply length */
  length = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button));

  apply_sequencer_length = ags_apply_sequencer_length_new((GObject *) AGS_MACHINE(matrix)->audio,
							  length);

  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(GObject *) apply_sequencer_length);
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  AgsPort *port;
  AgsCountBeatsAudio *count_beats_audio;

  GList *start_list, *list;

  gboolean loop;  

  loop = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

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

    g_object_unref(port);
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

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

    g_object_unref(port);
    
    /* iterate */
    list = list->next;
  }

  g_list_free(start_list);  
}

void
ags_matrix_stop_callback(AgsMatrix *matrix,
			 GList *recall_id, gint sound_scope,
			 gpointer data)
{
  if(sound_scope != AGS_SOUND_SCOPE_SEQUENCER){
    return;
  }
  
  ags_led_array_unset_all((AgsLedArray *) matrix->cell_pattern->hled_array);
}

