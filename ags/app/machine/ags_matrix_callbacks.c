/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/app/machine/ags_matrix_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_navigation.h>

#include <math.h>

void
ags_matrix_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsMatrix *matrix)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_ancestor(widget,
						 AGS_TYPE_WINDOW);

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
      GList *start_list, *list;

      gchar *str;

      guint64 bank_index_1;

      toggle = matrix->selected;
      matrix->selected = NULL;

      gtk_toggle_button_set_active(toggle,
				   FALSE);

      matrix->selected = (GtkToggleButton*) widget;

      gtk_widget_queue_draw((GtkWidget *) matrix->cell_pattern->drawing_area);

      /* calculate index 1 */
      str = gtk_button_get_label((GtkButton *) matrix->selected);
      bank_index_1 =
	AGS_MACHINE(matrix)->bank_1 = ((guint) g_ascii_strtoull(str, NULL, 10)) - 1;

      /* play - ags-fx-pattern */
      g_object_get(AGS_MACHINE(matrix)->audio,
		   "play", &start_list,
		   NULL);
  
      list = start_list;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
	AgsFxPatternAudio *fx_pattern_audio;
	AgsPort *port;
    
	fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

	port = NULL;
    
	g_object_get(fx_pattern_audio,
		     "bank-index-1", &port,
		     NULL);

	if(port != NULL){
	  GValue value = {0,};
      
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			    (gfloat) bank_index_1);
      
	  ags_port_safe_write(port,
			      &value);
      
	  g_value_unset(&value);
      
	  g_object_unref(port);
	}
    
	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);

      /* recall - ags-fx-pattern */
      g_object_get(AGS_MACHINE(matrix)->audio,
		   "recall", &start_list,
		   NULL);
  
      list = start_list;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
	AgsFxPatternAudio *fx_pattern_audio;
	AgsPort *port;
    
	fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

	port = NULL;
    
	g_object_get(fx_pattern_audio,
		     "bank-index-1", &port,
		     NULL);

	if(port != NULL){
	  GValue value = {0,};
      
	  g_value_init(&value,
		       G_TYPE_FLOAT);

	  g_value_set_float(&value,
			    (gfloat) bank_index_1);
      
	  ags_port_safe_write(port,
			      &value);
      
	  g_value_unset(&value);
      
	  g_object_unref(port);
	}
    
	/* iterate */
	list = list->next;
      }

      g_list_free_full(start_list,
		       g_object_unref);

      /* queue draw */
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
  AgsApplySequencerLength *apply_sequencer_length;
  
  AgsApplicationContext *application_context;
  
  gdouble length;

  application_context = ags_application_context_get_instance();

  /* task - apply length */
  length = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_button));

  apply_sequencer_length = ags_apply_sequencer_length_new((GObject *) AGS_MACHINE(matrix)->audio,
							  length);

  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) apply_sequencer_length);
}

void
ags_matrix_loop_button_callback(GtkWidget *button, AgsMatrix *matrix)
{
  GList *start_list, *list;

  gboolean loop;

  loop = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

  /* play - ags-fx-pattern */
  g_object_get(AGS_MACHINE(matrix)->audio,
	       "play", &start_list,
	       NULL);
  
  list = start_list;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
    AgsFxPatternAudio *fx_pattern_audio;
    AgsPort *port;
    
    fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

    port = NULL;
    
    g_object_get(fx_pattern_audio,
		 "loop", &port,
		 NULL);

    if(port != NULL){
      GValue value = {0,};
      
      g_value_init(&value,
		   G_TYPE_BOOLEAN);

      g_value_set_boolean(&value,
			  loop);
      
      ags_port_safe_write(port,
			  &value);
      
      g_value_unset(&value);
      
      g_object_unref(port);
    }
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);

  /* recall - ags-fx-pattern */
  g_object_get(AGS_MACHINE(matrix)->audio,
	       "recall", &start_list,
	       NULL);
  
  list = start_list;

  while((list = ags_recall_find_type(list,
				     AGS_TYPE_FX_PATTERN_AUDIO)) != NULL){
    AgsFxPatternAudio *fx_pattern_audio;
    AgsPort *port;
    
    fx_pattern_audio = AGS_FX_PATTERN_AUDIO(list->data);

    port = NULL;
    
    g_object_get(fx_pattern_audio,
		 "loop", &port,
		 NULL);

    if(port != NULL){
      GValue value = {0,};
      
      g_value_init(&value,
		   G_TYPE_BOOLEAN);

      g_value_set_boolean(&value,
			  loop);
      
      ags_port_safe_write(port,
			  &value);
      
      g_value_unset(&value);
      
      g_object_unref(port);
    }
    
    /* iterate */
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
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

void
ags_matrix_volume_callback(GtkRange *range, AgsMatrix *matrix)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(matrix)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VOLUME_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "volume", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  volume);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
