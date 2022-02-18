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

#include <ags/app/machine/ags_cell_pattern_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_machine.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_cell_pattern_start_channel_launch_callback(AgsTask *task, AgsNote *note);

gboolean
ags_cell_pattern_draw_callback(GtkWidget *drawing_area, cairo_t *cr, AgsCellPattern *cell_pattern)
{
//  cairo_surface_flush(cairo_get_target(cr));

  cairo_push_group(cr);

  /* the grid */
  ags_cell_pattern_draw_grid(cell_pattern, cr);

  /* the pattern */  
  ags_cell_pattern_draw_matrix(cell_pattern, cr);

  ags_cell_pattern_draw_cursor(cell_pattern, cr);

  /* paint */
  cairo_pop_group_to_source(cr);

  cairo_paint(cr);
  
//  cairo_surface_mark_dirty(cairo_get_target(cr));

  return(FALSE);
}

gboolean
ags_cell_pattern_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsCellPattern *cell_pattern)
{
  //TODO:JK: implement me, blink cursor
  
  return(TRUE);
}

gboolean
ags_cell_pattern_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsCellPattern *cell_pattern)
{
  if(event->button == 1){
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
    i = (guint) floor((double) event->y / (double) cell_pattern->cell_height);
    j = (guint) floor((double) event->x / (double) cell_pattern->cell_width);

    index1 = machine->bank_1;

    nth_channel = ags_channel_nth(start_input,
				  input_lines - ((guint) gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) + i) - 1);

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
  }

  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_key_press_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *cell_pattern)
{
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  switch(event->keyval){
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
ags_cell_pattern_drawing_area_key_release_event(GtkWidget *widget, GdkEventKey *event, AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;
  
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *channel, *nth_channel;

  guint input_lines;
  
  if(event->keyval == GDK_KEY_Tab){
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
  
  switch(event->keyval){
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
      
      if(cell_pattern->cursor_y < gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar))){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) - 1.0);
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
      
      if(cell_pattern->cursor_y >= gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    gtk_range_get_value(GTK_RANGE(cell_pattern->vscrollbar)) + 1.0);
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

void
ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern)
{
  gtk_widget_queue_draw((GtkWidget *) cell_pattern->drawing_area);
}

void
ags_cell_pattern_start_channel_launch_callback(AgsTask *task, AgsNote *note)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *first_recycling, *last_recycling;
  AgsRecycling *recycling, *next_recycling, *end_recycling;
  AgsAudioSignal *audio_signal;
  AgsPlayback *playback;
  AgsRecallID *recall_id;
  
  GObject *output_soundcard;
  
  gdouble delay;
  guint samplerate;
  
  channel = AGS_START_CHANNEL(task)->channel;

  /* get some fields */
  g_object_get(channel,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "playback", &playback,
	       NULL);

  g_object_unref(audio);

  g_object_unref(output_soundcard);
  
  if(playback != NULL){
    g_object_unref(playback);
  }
  
  recall_id = ags_playback_get_recall_id(playback, AGS_SOUND_SCOPE_PLAYBACK);

#ifdef AGS_DEBUG
  g_message("launch");
#endif

  if(playback == NULL ||
     recall_id == NULL){
    return;
  }

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    NULL,
			    &samplerate,
			    NULL,
			    NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* get some fields */
  g_object_get(channel,
	       "first-recycling", &first_recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  end_recycling = ags_recycling_next(last_recycling);
  
  /* add audio signal */
  recycling = first_recycling;
  g_object_ref(recycling);
  
  next_recycling = NULL;
  
  while(recycling != end_recycling){
    if(!ags_recall_global_get_rt_safe()){
      guint note_x0, note_x1;
      
      audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					  (GObject *) recycling,
					  (GObject *) recall_id);
      g_object_set(audio_signal,
		   "note", note,
		   NULL);

      /* add audio signal */
      g_object_get(note,
		   "x0", &note_x0,
		   "x1", &note_x1,
		   NULL);

      ags_recycling_create_audio_signal_with_frame_count(recycling,
							 audio_signal,
							 (note_x1 - note_x0) * ((gdouble) samplerate / delay),
							 0.0, 0);
      audio_signal->stream_current = audio_signal->stream;
      ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
  
      /*
       * emit add_audio_signal on AgsRecycling
       */
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);
    }else{
      GList *start_list, *list;

      g_object_get(recycling,
		   "audio-signal", &start_list,
		   NULL);
      
      audio_signal = NULL;
      list = ags_audio_signal_find_by_recall_id(start_list,
						(GObject *) recall_id);
	    
      if(list != NULL){
	audio_signal = list->data;

	g_object_set(audio_signal,
		     "note", note,
		     NULL);
      }

      g_list_free_full(start_list,
		       g_object_unref);

      g_object_set(note,
		   "rt-offset", 0,
		   NULL);
    }

    /* iterate */
    next_recycling = ags_recycling_next(recycling);

    g_object_unref(recycling);

    recycling = next_recycling;
  }

  if(first_recycling != NULL){
    g_object_unref(first_recycling);
    g_object_unref(last_recycling);
  }

  if(end_recycling != NULL){
    g_object_unref(end_recycling);
  }

  if(next_recycling != NULL){
    g_object_unref(next_recycling);
  }
}
