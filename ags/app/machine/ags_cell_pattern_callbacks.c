/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/machine/ags_cell_pattern_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void
ags_cell_pattern_update_ui_callback(GObject *ui_provider,
				    AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;

  AgsAudio *audio;
  AgsRecallID *recall_id;
    
  AgsFxPatternAudio *play_fx_pattern_audio;
  AgsFxPatternAudioProcessor *play_fx_pattern_audio_processor;

  GList *start_list, *list;
  GList *start_recall, *recall;
    
  guint64 active_led_new;
  gboolean success;

  GRecMutex *play_fx_pattern_audio_processor_mutex;
    
  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  if(machine == NULL){      
    return;
  }

  audio = machine->audio;
  
  /* get some recalls */
  list =
    start_list = ags_audio_get_recall_id(audio);

  recall_id = NULL;

  success = FALSE;
    
  while(list != NULL && !success){
    AgsRecyclingContext *current;

    g_object_get(list->data,
		 "recycling-context", &current,
		 NULL);

    g_object_unref(current);

    if(current != NULL){
      g_object_get(current,
		   "parent", &current,
		   NULL);

      if(current != NULL){
	g_object_unref(current);
      }
	
      if(current == NULL &&
	 ags_recall_id_check_sound_scope(list->data, AGS_SOUND_SCOPE_SEQUENCER)){
	recall_id = list->data;

	g_object_get(audio,
		     "play", &start_recall,
		     NULL);

	play_fx_pattern_audio = NULL;
	play_fx_pattern_audio_processor = NULL;
    
	recall = ags_recall_find_type(start_recall,
				      AGS_TYPE_FX_PATTERN_AUDIO);
    
	if(recall != NULL){
	  play_fx_pattern_audio = AGS_FX_PATTERN_AUDIO(recall->data);
	}
    
	recall = ags_recall_find_type_with_recycling_context(start_recall,
							     AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR,
							     (GObject *) recall_id->recycling_context);
    
	if(recall != NULL){
	  play_fx_pattern_audio_processor = AGS_FX_PATTERN_AUDIO_PROCESSOR(recall->data);
	}

	g_list_free_full(start_recall,
			 g_object_unref);

	if(play_fx_pattern_audio == NULL ||
	   play_fx_pattern_audio_processor == NULL){
	  recall_id = NULL;
	}else{
	  success = TRUE;
	}
      }
    }
      
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
    
  if(recall_id == NULL){
    return;
  }

  /* active led */
  play_fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_fx_pattern_audio_processor);

  g_rec_mutex_lock(play_fx_pattern_audio_processor_mutex);

  active_led_new = play_fx_pattern_audio_processor->offset_counter;
    
  g_rec_mutex_unlock(play_fx_pattern_audio_processor_mutex);

  cell_pattern->active_led = (guint) (active_led_new % cell_pattern->n_cols);
  ags_led_array_unset_all((AgsLedArray *) cell_pattern->hled_array);
  ags_led_array_set_nth((AgsLedArray *) cell_pattern->hled_array,
			cell_pattern->active_led);
}

gboolean
ags_cell_pattern_gesture_click_pressed_callback(GtkGestureClick *event_controller,
						gint n_press,
						gdouble x,
						gdouble y,
						AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;

  AgsAudio *audio;
  AgsChannel *start_input, *nth_channel;

  GList *start_pattern;
    
  guint input_lines;
  guint i, j;
  guint index1;

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  audio = machine->audio;
    
  /* get some audio fields */
  g_object_get(audio,
	       "input-lines", &input_lines,
	       "input", &start_input,
	       NULL);
    
  /* get pattern position */        
  i = (guint) floor((double) y / (double) cell_pattern->cell_height);
  j = (guint) floor((double) x / (double) cell_pattern->cell_width);

  index1 = machine->bank_1;

  nth_channel = ags_channel_nth(start_input,
				input_lines - ((guint) gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)) + i) - 1);

  if(nth_channel != NULL){
    /* toggle pattern */
    g_object_get(nth_channel,
		 "pattern", &start_pattern,
		 NULL);

    ags_pattern_toggle_bit(start_pattern->data,
			   0, index1,
			   j);
      
    g_object_unref(nth_channel);

    g_list_free_full(start_pattern,
		     g_object_unref);
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }
    
  /* queue draw */
  gtk_widget_queue_draw((GtkWidget *) cell_pattern->drawing_area);
  
  return(TRUE);
}

gboolean
ags_cell_pattern_gesture_click_released_callback(GtkGestureClick *event_controller,
						 gint n_press,
						 gdouble x,
						 gdouble y,
						 AgsCellPattern *cell_pattern)
{
  //empty
  
  return(FALSE);
}

gboolean
ags_cell_pattern_key_pressed_callback(GtkEventControllerKey *event_controller,
				      guint keyval,
				      guint keycode,
				      GdkModifierType state,
				      AgsCellPattern *cell_pattern)
{
  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab){
    return(FALSE);
  }

  switch(keyval){
  case GDK_KEY_Control_L:
    {
      cell_pattern->key_mask |= AGS_CELL_PATTERN_KEY_L_CONTROL;
    }
    break;
  case GDK_KEY_Control_R:
    {
      cell_pattern->key_mask |= AGS_CELL_PATTERN_KEY_R_CONTROL;
    }
    break;
  case GDK_KEY_c:
    {
      /* copy notes */
      if((AGS_CELL_PATTERN_KEY_L_CONTROL & (cell_pattern->key_mask)) != 0 || (AGS_CELL_PATTERN_KEY_R_CONTROL & (cell_pattern->key_mask)) != 0){
	AgsMachine *machine;

	machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
							 AGS_TYPE_MACHINE);
	
	ags_machine_copy_pattern(machine);
      }
    }
    break;
  }

  return(TRUE);
}

gboolean
ags_cell_pattern_key_released_callback(GtkEventControllerKey *event_controller,
				       guint keyval,
				       guint keycode,
				       GdkModifierType state,
				       AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;
  
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *nth_channel;

  guint input_lines;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab){
    return(FALSE);
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &start_input,
	       "input-lines", &input_lines,
	       NULL);
  
  switch(keyval){
  case GDK_KEY_Control_L:
    {
      cell_pattern->key_mask &= (~AGS_CELL_PATTERN_KEY_L_CONTROL);
    }
    break;
  case GDK_KEY_Control_R:
    {
      cell_pattern->key_mask &= (~AGS_CELL_PATTERN_KEY_R_CONTROL);
    }
    break;
  case GDK_KEY_Left:
  case GDK_KEY_leftarrow:
    {
      if(cell_pattern->cursor_x > 0){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_x -= 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = (ags_pattern_get_bit(start_pattern->data,
					   0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
	
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);
	  
	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(cell_pattern->cursor_x < cell_pattern->n_cols){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_x += 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = ags_pattern_get_bit(start_pattern->data,
					  0, machine->bank_1, cell_pattern->cursor_x);
	
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);

	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(cell_pattern->cursor_y > 0){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_y -= 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = (ags_pattern_get_bit(start_pattern->data,
					   0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
	
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);

	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
      
      if(cell_pattern->cursor_y < gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar))){
	gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar),
				 gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)) - 1.0);
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(cell_pattern->cursor_y < cell_pattern->n_rows){
	GList *start_pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_y += 1;

	/* audible feedback */
	nth_channel = ags_channel_nth(start_input,
				      input_lines - cell_pattern->cursor_y - 1);

	channel = nth_channel;

	if(channel != NULL){
	  /* check bit */
	  g_object_get(channel,
		       "pattern", &start_pattern,
		       NULL);

	  bit_is_on = ags_pattern_get_bit(start_pattern->data,
					  0, machine->bank_1, cell_pattern->cursor_x);
		
	  if(bit_is_on){
	    AgsPlayback *playback;
	    
	    g_object_get(channel,
			 "playback", &playback,
			 NULL);
	    
	    ags_machine_playback_set_active(machine,
					    playback,
					    TRUE);
	  
	    g_object_unref(playback);
	  }

	  /* unref */
	  g_object_unref(channel);

	  g_list_free_full(start_pattern,
			   g_object_unref);
	}
      }
      
      if(cell_pattern->cursor_y >= gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)) + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
	gtk_adjustment_set_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar),
				 gtk_adjustment_get_value(gtk_scrollbar_get_adjustment(cell_pattern->vscrollbar)) + 1.0);
      }
    }
    break;
  case GDK_KEY_space:
    {
      GList *start_pattern;

      guint i, j;
      guint index1;
      
      i = cell_pattern->cursor_y;
      j = cell_pattern->cursor_x;
      
      index1 = machine->bank_1;

      nth_channel = ags_channel_nth(start_input,
				    input_lines - i - 1);
      
      channel = nth_channel;

      if(channel != NULL){
	/* toggle pattern */
	g_object_get(channel,
		     "pattern", &start_pattern,
		     NULL);

	ags_pattern_toggle_bit(start_pattern->data,
			       0, index1,
			       j);

	/* play pattern */
	if(ags_pattern_get_bit(start_pattern->data,
			       0, index1, j)){
	  AgsPlayback *playback;
	    
	  g_object_get(channel,
		       "playback", &playback,
		       NULL);
	    
	  ags_machine_playback_set_active(machine,
					  playback,
					  TRUE);
	  
	  g_object_unref(playback);
	}

	/* unref */
	g_object_unref(channel);

	g_list_free_full(start_pattern,
			 g_object_unref);
      }
      
      /* queue draw */
      gtk_widget_queue_draw((GtkWidget *) cell_pattern->drawing_area);
    }
    break;
  }

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  return(TRUE);
}

gboolean
ags_cell_pattern_modifiers_callback(GtkEventControllerKey *event_controller,
				    GdkModifierType keyval,
				    AgsCellPattern *cell_pattern)
{
  //empty
  
  return(FALSE);
}

void
ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern)
{
  gtk_widget_queue_draw((GtkWidget *) cell_pattern->drawing_area);
}
