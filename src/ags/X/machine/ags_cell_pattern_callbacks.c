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

#include <ags/X/machine/ags_cell_pattern_callbacks.h>

#include <<ags/object/ags_application_context.h>>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/object/ags_soundcard.h>

#include <ags/audio/recall/ags_play_channel_run.h>

#include <ags/audio/task/ags_start_devout.h>
#include <ags/audio/task/ags_init_channel.h>
#include <ags/audio/task/ags_append_channel.h>
#include <ags/audio/task/ags_append_recall.h>
#include <ags/audio/task/ags_add_audio_signal.h>
#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/X/ags_machine.h>

#include <gdk/gdkkeysyms.h>

#include <math.h>

void ags_cell_pattern_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
					   AgsCellPattern *cell_pattern);
void ags_cell_pattern_init_channel_launch_callback(AgsTask *task, gpointer data);

extern pthread_mutex_t ags_application_mutex;

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
    AgsWindow *window;
    AgsMachine *machine;
    
    AgsTogglePatternBit *toggle_pattern_bit;
    AgsChannel *channel;
    
    AgsThread *audio_loop;
    AgsTaskThread *task_thread;

    AgsApplicationContext *application_context;
  
    guint i, j;
    guint index1;

    machine = gtk_widget_get_ancestor(cell_pattern,
				      AGS_TYPE_MACHINE);

    window = gtk_widget_get_ancestor(machine,
				     AGS_TYPE_WINDOW);

    application_context = window->application_context;
  
    /* get audio loop */
    pthread_mutex_lock(&(ags_application_mutex));

    audio_loop = application_context->main_loop;

    pthread_mutex_unlock(&(ags_application_mutex));
  
    task_thread = ags_thread_find_type(audio_loop,
				       AGS_TYPE_TASK_THREAD);
        
    i = (guint) floor((double) event->y / (double) cell_pattern->cell_height);
    j = (guint) floor((double) event->x / (double) cell_pattern->cell_width);

    index1 = machine->bank_1;

    channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - ((guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value + i) - 1);

    toggle_pattern_bit = ags_toggle_pattern_bit_new(channel->pattern->data,
						    channel->line,
						    0, index1,
						    j);
    g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
		     G_CALLBACK(ags_cell_pattern_refresh_gui_callback), cell_pattern);

    ags_task_thread_append_task(task_thread,
				AGS_TASK(toggle_pattern_bit));
  }else if (event->button == 3){
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

	machine = gtk_widget_get_ancestor(cell_pattern,
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
      
  AgsChannel *channel;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  auto void ags_cell_pattern_drawing_area_key_release_event_play_channel(AgsChannel *channel);

  void ags_cell_pattern_drawing_area_key_release_event_play_channel(AgsChannel *channel){
    AgsDevout *devout;

    AgsStartDevout *start_devout;
    AgsInitChannel *init_channel;
    AgsAppendChannel *append_channel;

    AgsAudioLoop *audio_loop;
    AgsTaskThread *task_thread;
    AgsDevoutThread *devout_thread;

    AgsMutexManager *mutex_manager;

    GList *tasks;

    pthread_mutex_t *audio_mutex;

    pthread_mutex_lock(&(ags_application_mutex));
  
    mutex_manager = ags_mutex_manager_get_instance();

    audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					   (GObject *) channel->audio);
  
    pthread_mutex_unlock(&(ags_application_mutex));

    pthread_mutex_lock(audio_mutex);
  
    devout = AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout);

    audio_loop = AGS_AUDIO_LOOP(AGS_APPLLICATION_CONTEXT(devout->application_context)->main_loop);
    
    task_thread = ags_thread_find_type(audio_loop,
				       AGS_TYPE_TASK_THREAD);
    devout_thread = ags_thread_find_type(audio_loop,
					 AGS_TYPE_DEVOUT_THREAD);

    tasks = NULL;

    /* init channel for playback */
    init_channel = ags_init_channel_new(channel, FALSE,
					TRUE, FALSE, FALSE);
    g_signal_connect_after(G_OBJECT(init_channel), "launch\0",
			   G_CALLBACK(ags_cell_pattern_init_channel_launch_callback), NULL);
    tasks = g_list_prepend(tasks, init_channel);
    
    /* append channel for playback */
    append_channel = ags_append_channel_new(G_OBJECT(audio_loop),
					    G_OBJECT(channel));
    tasks = g_list_prepend(tasks, append_channel);

    /* create start task */
    start_devout = ags_start_devout_new(devout);
    tasks = g_list_prepend(tasks,
			   start_devout);

    /* perform playback */
    tasks = g_list_reverse(tasks);
    ags_task_thread_append_tasks(task_thread, tasks);

    pthread_mutex_unlock(audio_mutex);
  }
  
  if(event->keyval == GDK_KEY_Tab){
    return(FALSE);
  }

  machine = gtk_widget_get_ancestor(cell_pattern,
				    AGS_TYPE_MACHINE);
  window = gtk_widget_get_ancestor(cell_pattern,
				   AGS_TYPE_WINDOW);

  audio_loop = AGS_AUDIO_LOOP(AGS_APPLLICATION_CONTEXT(window->application_context)->main_loop);
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

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
	cell_pattern->cursor_x -= 1;

	/* audible feedback */
	channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - cell_pattern->cursor_y - 1);
	  
	if(ags_pattern_get_bit(channel->pattern->data,
				0, machine->bank_1, cell_pattern->cursor_x)){
	  ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
	}
      }
    }
    break;
  case GDK_KEY_Right:
  case GDK_KEY_rightarrow:
    {
      if(cell_pattern->cursor_x < cell_pattern->n_cols){
	cell_pattern->cursor_x += 1;

	/* audible feedback */
	channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - cell_pattern->cursor_y - 1);

	if(ags_pattern_get_bit(channel->pattern->data,
				0, machine->bank_1, cell_pattern->cursor_x)){
	  ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
	}
      }
    }
    break;
  case GDK_KEY_Up:
  case GDK_KEY_uparrow:
    {
      if(cell_pattern->cursor_y > 0){
	cell_pattern->cursor_y -= 1;

	/* audible feedback */
	channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - cell_pattern->cursor_y - 1);

	if(ags_pattern_get_bit(channel->pattern->data,
				0, machine->bank_1, cell_pattern->cursor_x)){
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
	cell_pattern->cursor_y += 1;

	/* audible feedback */
	channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - cell_pattern->cursor_y - 1);

	if(ags_pattern_get_bit(channel->pattern->data,
				0, machine->bank_1, cell_pattern->cursor_x)){
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
      AgsTogglePatternBit *toggle_pattern_bit;
      
      guint i, j;
      guint index1;
      
      i = cell_pattern->cursor_y;
      j = cell_pattern->cursor_x;
      
      index1 = machine->bank_1;

      channel = ags_channel_nth(machine->audio->input, machine->audio->input_lines - i - 1);
      
      toggle_pattern_bit = ags_toggle_pattern_bit_new(channel->pattern->data,
						      channel->line,
						      0, index1,
						      j);
      g_signal_connect(G_OBJECT(toggle_pattern_bit), "refresh-gui\0",
		       G_CALLBACK(ags_cell_pattern_refresh_gui_callback), cell_pattern);


      if(!ags_pattern_get_bit(channel->pattern->data,
			      0, index1, j)){
	ags_cell_pattern_drawing_area_key_release_event_play_channel(channel);
      }
      
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(toggle_pattern_bit));
    }
    break;
  }

  return(TRUE);
}

void
ags_cell_pattern_adjustment_value_changed_callback(GtkWidget *widget, AgsCellPattern *cell_pattern)
{
  ags_cell_pattern_draw_matrix(cell_pattern);
}

void
ags_cell_pattern_refresh_gui_callback(AgsTogglePatternBit *toggle_pattern_bit,
				      AgsCellPattern *cell_pattern)
{
  AgsMachine *machine;
  
  AgsChannel *channel;

  guint line;
  
  machine = gtk_widget_get_ancestor(cell_pattern,
				    AGS_TYPE_MACHINE);
    
  channel = ags_channel_nth(machine->audio->input,
			    toggle_pattern_bit->line);
  line = machine->audio->input_pads - toggle_pattern_bit->line - (guint) GTK_RANGE(cell_pattern->vscrollbar)->adjustment->value - 1;

  ags_cell_pattern_redraw_gutter_point(cell_pattern,
				       channel,
				       toggle_pattern_bit->bit,
				       line);
}

void
ags_cell_pattern_init_channel_launch_callback(AgsTask *task, gpointer data)
{
  AgsDevout *devout;
  AgsChannel *channel;
  AgsRecycling *recycling;

  AgsAddAudioSignal *add_audio_signal;

  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsMutexManager *mutex_manager;

  GList *recall, *tmp;

  pthread_mutex_t *audio_mutex;

  channel = AGS_INIT_CHANNEL(task)->channel;

  pthread_mutex_lock(&(ags_application_mutex));
  
  mutex_manager = ags_mutex_manager_get_instance();

  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) channel->audio);
  
  pthread_mutex_unlock(&(ags_application_mutex));

  pthread_mutex_lock(audio_mutex);
  
  devout = AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout);

  audio_loop = AGS_AUDIO_LOOP(AGS_APPLLICATION_CONTEXT(devout->application_context)->main_loop);
  task_thread = ags_thread_find_type(audio_loop,
				     AGS_TYPE_TASK_THREAD);

  g_message("launch\0");
  
  if(AGS_DEVOUT_PLAY(channel->devout_play) == NULL ||
     AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] == NULL){    
    return;
  }

  /* connect done */
  recall = ags_recall_find_provider_with_recycling_container(channel->play,
							     G_OBJECT(channel),
							     G_OBJECT(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0]->recycling_container));
  
  tmp = recall;
  recall = ags_recall_find_type(recall,
				AGS_TYPE_PLAY_CHANNEL_RUN);
  //TODO:JK: fix me
  //    g_list_free(tmp);

  if(recall != NULL){
    AgsAudioSignal *audio_signal;
      
    /* add audio signal */
    recycling = channel->first_recycling;

    while(recycling != channel->last_recycling->next){
      audio_signal = ags_audio_signal_new((GObject *) devout,
					  (GObject *) recycling,
					  (GObject *) AGS_RECALL(recall->data)->recall_id);
      /* add audio signal */
      ags_recycling_create_audio_signal_with_defaults(recycling,
						      audio_signal,
						      0.0, 0);
      audio_signal->stream_current = audio_signal->stream_beginning;
      ags_audio_signal_connect(audio_signal);
  
      /*
       * emit add_audio_signal on AgsRecycling
       */
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);

      recycling = recycling->next;
    }    
  }

  pthread_mutex_unlock(audio_mutex);
}
