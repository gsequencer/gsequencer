/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/machine/ags_cell_pattern_callbacks.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>
#include <ags/X/ags_machine.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_cell_pattern_start_channel_launch_callback(AgsTask *task, AgsNote *note);

gboolean
ags_cell_pattern_focus_in_callback(GtkWidget *widget, GdkEvent *event, AgsCellPattern *cell_pattern)
{
  pthread_t thread;
  
  pthread_create(&thread, NULL,
		 ags_cell_pattern_blink_worker, cell_pattern);
  
  return(TRUE);
}

gboolean
ags_cell_pattern_drawing_area_configure_callback(GtkWidget *widget, GdkEventConfigure *event, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_paint(cell_pattern);
  
  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_expose_callback(GtkWidget *widget, GdkEventExpose *event, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_paint(cell_pattern);

  return(FALSE);
}

gboolean
ags_cell_pattern_drawing_area_button_press_callback(GtkWidget *widget, GdkEventButton *event, AgsCellPattern *cell_pattern)
{
  if(event->button == 1){
    AgsMachine *machine;

    AgsAudio *audio;
    AgsChannel *input;
    AgsChannel *channel;
    AgsPattern *pattern;
    
    guint input_lines;
    guint i, j;
    guint index1;
    
    pthread_mutex_t *audio_mutex;
    pthread_mutex_t *channel_mutex;

    machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						     AGS_TYPE_MACHINE);

    audio = machine->audio;
    
    /* get audio mutex */
    pthread_mutex_lock(ags_audio_get_class_mutex());

    audio_mutex = audio->obj_mutex;
  
    pthread_mutex_unlock(ags_audio_get_class_mutex());

    /* get some fields */
    pthread_mutex_lock(audio_mutex);

    input = audio->input;

    input_lines = audio->input_lines;
    
    pthread_mutex_unlock(audio_mutex);
    
    /* get pattern position */        
    i = (guint) floor((double) event->y / (double) cell_pattern->cell_height);
    j = (guint) floor((double) event->x / (double) cell_pattern->cell_width);

    index1 = machine->bank_1;

    channel = ags_channel_nth(input,
			      input_lines - ((guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + i) - 1);

    /* get channel mutex */
    pthread_mutex_lock(ags_channel_get_class_mutex());

    channel_mutex = channel->obj_mutex;
  
    pthread_mutex_unlock(ags_channel_get_class_mutex());

    /* toggle pattern */
    pthread_mutex_lock(channel_mutex);

    pattern = channel->pattern->data;

    pthread_mutex_unlock(channel_mutex);

    ags_pattern_toggle_bit(pattern,
			   0, index1,
			   j);

    /* queue draw */
    gtk_widget_queue_draw(cell_pattern->drawing_area);
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
  AgsWindow *window;
  AgsMachine *machine;
      
  AgsGuiThread *gui_thread;

  AgsAudio *audio;
  AgsChannel *input;
  AgsChannel *channel;

  guint input_lines;

  pthread_mutex_t *channel_mutex;
  
  auto void ags_cell_pattern_drawing_area_key_release_event_play_channel(AgsChannel *channel);

  void ags_cell_pattern_drawing_area_key_release_event_play_channel(AgsChannel *channel){
    AgsGuiThread *gui_thread;
    
    AgsAudio *audio;
    
    AgsStartSoundcard *start_soundcard;
    AgsStartChannel *start_channel;

    AgsApplicationContext *application_context;

    GObject *soundcard;
    
    GList *task;

    application_context = (AgsApplicationContext *) window->application_context;
    
    gui_thread = ags_ui_provider_get_gui_thread(AGS_UI_PROVIDER(application_context));

    /* get soundcard */
    g_object_get(channel,
		 "output-soundcard", &soundcard,
		 "audio", &audio,
		 NULL);

    if(soundcard == NULL){
      return;
    }

    task = NULL;

    /* start playback */
    start_channel = ags_start_channel_new(channel,
					  AGS_SOUND_SCOPE_PLAYBACK);
    task = g_list_prepend(task,
			  start_channel);

    /* create start task */
    start_soundcard = ags_start_soundcard_new(application_context);
    task = g_list_prepend(task,
			  start_soundcard);

    /* perform playback */
    task = g_list_reverse(task);
    ags_gui_thread_schedule_task_list(gui_thread,
				      task);
  }
  
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  machine = (AgsMachine *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						   AGS_TYPE_MACHINE);

  window = (AgsWindow *) gtk_widget_get_ancestor((GtkWidget *) cell_pattern,
						 AGS_TYPE_WINDOW);

  audio = machine->audio;

  /* get some fields */
  g_object_get(audio,
	       "input", &input,
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
	AgsPattern *pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_x -= 1;

	/* audible feedback */
	channel = ags_channel_nth(input,
				  input_lines - cell_pattern->cursor_y - 1);
	  
	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check bit */
	pthread_mutex_lock(channel_mutex);

	pattern = channel->pattern->data;
	
	pthread_mutex_unlock(channel_mutex);

	bit_is_on = (ags_pattern_get_bit(pattern,
					 0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
	
	if(bit_is_on){
	  ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
	}
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(cell_pattern->cursor_x < cell_pattern->n_cols){
	AgsPattern *pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_x += 1;

	/* audible feedback */
	channel = ags_channel_nth(input,
				  input_lines - cell_pattern->cursor_y - 1);

	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check bit */
	pthread_mutex_lock(channel_mutex);

	pattern = channel->pattern->data;
	
	pthread_mutex_unlock(channel_mutex);

	bit_is_on = ags_pattern_get_bit(pattern,
					0, machine->bank_1, cell_pattern->cursor_x);
	
	if(bit_is_on){
	  ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
	}
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(cell_pattern->cursor_y > 0){
	AgsPattern *pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_y -= 1;

	/* audible feedback */
	channel = ags_channel_nth(input,
				  input_lines - cell_pattern->cursor_y - 1);

	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check bit */
	pthread_mutex_lock(channel_mutex);

	pattern = channel->pattern->data;
	
	pthread_mutex_unlock(channel_mutex);

	bit_is_on = (ags_pattern_get_bit(pattern,
					 0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
	
	if(bit_is_on){
	  ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
	}
      }

      if(cell_pattern->cursor_y < GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value - 1.0);
      }
    }
    break;
  case GDK_KEY_Down:
  case GDK_KEY_downarrow:
    {
      if(cell_pattern->cursor_y < cell_pattern->n_rows){
	AgsPattern *pattern;
	
	gboolean bit_is_on;
	
	cell_pattern->cursor_y += 1;

	/* audible feedback */
	channel = ags_channel_nth(input,
				  input_lines - cell_pattern->cursor_y - 1);

	/* get channel mutex */
	pthread_mutex_lock(ags_channel_get_class_mutex());

	channel_mutex = channel->obj_mutex;
  
	pthread_mutex_unlock(ags_channel_get_class_mutex());

	/* check bit */
	pthread_mutex_lock(channel_mutex);

	pattern = channel->pattern->data;
	
	pthread_mutex_unlock(channel_mutex);

	bit_is_on = (ags_pattern_get_bit(pattern,
					 0, machine->bank_1, cell_pattern->cursor_x)) ? TRUE: FALSE;
		
	if(bit_is_on){
	  ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
	}
      }

      if(cell_pattern->cursor_y >= GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + AGS_CELL_PATTERN_MAX_CONTROLS_SHOWN_VERTICALLY){
	gtk_range_set_value(GTK_RANGE(cell_pattern->vscrollbar),
			    GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + 1.0);
      }
    }
    break;
  case GDK_KEY_space:
    {
      AgsPattern *pattern;

      guint i, j;
      guint index1;
      
      i = cell_pattern->cursor_y;
      j = cell_pattern->cursor_x;
      
      index1 = machine->bank_1;

      channel = ags_channel_nth(input,
				input_lines - i - 1);
      
      /* get channel mutex */
      pthread_mutex_lock(ags_channel_get_class_mutex());

      channel_mutex = channel->obj_mutex;
  
      pthread_mutex_unlock(ags_channel_get_class_mutex());
      
      /* toggle pattern */
      pthread_mutex_lock(channel_mutex);

      pattern = channel->pattern->data;
	
      pthread_mutex_unlock(channel_mutex);

      ags_pattern_toggle_bit(channel->pattern->data,
			     0, index1,
			     j);

      /* play pattern */
      if(!ags_pattern_get_bit(pattern,
			      0, index1, j)){
	ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
      }

      /* queue draw */
      gtk_widget_queue_draw(cell_pattern->drawing_area);
    }
    break;
  }

  return(TRUE);
}

void
ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_paint(cell_pattern);
}

void
ags_cell_pattern_start_channel_launch_callback(AgsTask *task, AgsNote *note)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsRecycling *recycling, *last_recycling;
  AgsRecycling *end_recycling;
  AgsAudioSignal *audio_signal;
  AgsPlayback *playback;
  AgsRecallID *recall_id;
  
  GObject *output_soundcard;
  
  gdouble delay;
  guint samplerate;
  
  GValue value = {0,};

  channel = AGS_START_CHANNEL(task)->channel;

  /* get some fields */
  g_object_get(channel,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "playback", &playback,
	       NULL);

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
	       "first-recycling", &recycling,
	       "last-recycling", &last_recycling,
	       NULL);

  g_object_get(last_recycling,
	       "next", &end_recycling,
	       NULL);

  /* add audio signal */  
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
      list = ags_audio_signal_get_by_recall_id(start_list,
					       recall_id);
	    
      if(list != NULL){
	audio_signal = list->data;

	g_object_set(audio_signal,
		     "note", note,
		     NULL);
      }

      g_list_free(start_list);

      g_object_set(note,
		   "rt-offset", 0,
		   NULL);
    }

    g_object_get(recycling,
		 "next", &recycling,
		 NULL);
  }
}
