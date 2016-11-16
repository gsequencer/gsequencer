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

#include <ags/X/ags_navigation_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/recall/ags_count_beats_audio.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/task/recall/ags_apply_bpm.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_editor.h>

#include <ags/X/thread/ags_gui_thread.h>

#include <ags/X/task/ags_change_tact.h>
#include <ags/X/task/ags_display_tact.h>

#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>

void
ags_navigation_parent_set_callback(GtkWidget *widget, GtkObject *old_parent,
				   gpointer data)
{
  AgsWindow *window;
  AgsNavigation *navigation;

  if(old_parent != NULL)
    return;

  window = AGS_WINDOW(gtk_widget_get_ancestor(widget,
					      AGS_TYPE_WINDOW));
  navigation = AGS_NAVIGATION(widget);

  navigation->soundcard = window->soundcard;
}

void
ags_navigation_expander_callback(GtkWidget *widget,
				 AgsNavigation *navigation)
{
  GtkArrow *arrow;
  GList *list;

  list = gtk_container_get_children((GtkContainer *) widget);
  arrow = (GtkArrow *) list->data;
  g_list_free(list);

  list = gtk_container_get_children((GtkContainer *) navigation);
  widget = (GtkWidget *) list->next->data;
  g_list_free(list);

  if(arrow->arrow_type == GTK_ARROW_DOWN){
    gtk_widget_hide_all(widget);
    arrow->arrow_type = GTK_ARROW_RIGHT;
  }else{
    gtk_widget_show_all(widget);
    arrow->arrow_type = GTK_ARROW_DOWN;
  }
}

void
ags_navigation_bpm_callback(GtkWidget *widget,
			    AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsApplyBpm *apply_bpm;

  AgsMutexManager *mutex_manager;
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(gtk_widget_get_ancestor(widget,
					      AGS_TYPE_WINDOW));
  
  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  main_loop = (AgsThread *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(main_loop,
						       AGS_TYPE_TASK_THREAD);

  apply_bpm = ags_apply_bpm_new(G_OBJECT(window->soundcard),
				navigation->bpm->adjustment->value);

  ags_task_thread_append_task(task_thread,
			      AGS_TASK(apply_bpm));
}

void
ags_navigation_rewind_callback(GtkWidget *widget,
			       AgsNavigation *navigation)
{
  AgsWindow *window;
  gdouble tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(window->soundcard)) / 16.0;
  
  gtk_spin_button_set_value(navigation->position_tact,
			    tact +
			    (-1.0 * AGS_NAVIGATION_DEFAULT_TACT_STEP));
}

void
ags_navigation_prev_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  gdouble tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(window->soundcard)) / 16.0;
  
  gtk_spin_button_set_value(navigation->position_tact,
			    tact +
			    (-1.0 * AGS_NAVIGATION_REWIND_STEPS));
}

void
ags_navigation_play_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  GList *machines, *machines_start;
  gboolean initialized_time;
  
  if((AGS_NAVIGATION_BLOCK_PLAY & (navigation->flags)) != 0){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  
  machines_start =
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));
  
  initialized_time = FALSE;
  
  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if(((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0) ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      printf("found machine to play!\n\0");
#endif
      
      if(!initialized_time){
	initialized_time = TRUE;
	navigation->start_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(window->soundcard));
      }
      
      ags_machine_set_run_extended(machine,
				   TRUE,
				   !gtk_toggle_button_get_active((GtkToggleButton *) navigation->exclude_sequencer), TRUE);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);
}

void
ags_navigation_stop_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  GList *machines,*machines_start;
  gchar *timestr;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      printf("found machine to stop!\n\0");

      ags_machine_set_run_extended(machine,
				   FALSE,
				   !gtk_toggle_button_get_active((GtkToggleButton *) navigation->exclude_sequencer), TRUE);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);

  /* toggle play button */
  navigation->flags |= AGS_NAVIGATION_BLOCK_PLAY;
  gtk_toggle_button_set_active(navigation->play,
			       FALSE);

  navigation->flags &= (~AGS_NAVIGATION_BLOCK_PLAY);

  navigation->start_tact = 0.0;

  timestr = ags_time_get_uptime_from_offset(0.0,
					    navigation->bpm->adjustment->value,
					    ags_soundcard_get_delay(AGS_SOUNDCARD(window->soundcard)),
					    ags_soundcard_get_delay_factor(AGS_SOUNDCARD(window->soundcard)));
  gtk_label_set_text(navigation->duration_time, timestr);
  
  g_free(timestr);
  ags_soundcard_set_note_offset(AGS_SOUNDCARD(window->soundcard),
				0);

  /* reset editor */
  window->editor->current_tact = 0.0;
}

void
ags_navigation_next_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  gdouble tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(window->soundcard)) / 16.0;

  gtk_spin_button_set_value(navigation->position_tact,
			    tact +
			    AGS_NAVIGATION_REWIND_STEPS);
}

void
ags_navigation_forward_callback(GtkWidget *widget,
				AgsNavigation *navigation)
{
  AgsWindow *window;
  gdouble tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(window->soundcard)) / 16.0;

  gtk_spin_button_set_value(navigation->position_tact,
			    tact +
			    AGS_NAVIGATION_DEFAULT_TACT_STEP);
}

void
ags_navigation_loop_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *machines, *machines_start;
  GList *list, *list_start;

  guint loop_left, loop_right;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;
  
  GValue do_loop_value = {0,};
  
  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  loop_left = 16 * navigation->loop_left_tact->adjustment->value;
  loop_right = 16 * navigation->loop_right_tact->adjustment->value;
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(window->soundcard),
			 loop_left, loop_right,
			 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
			 
  g_value_init(&do_loop_value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&do_loop_value,
		      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      g_message("found machine to loop!\n\0");

      audio = machine->audio;

      /* get mutex manager and application mutex */
      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* do it so */
      pthread_mutex_lock(audio_mutex);
      
      list = audio->play;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	recall = AGS_RECALL(list->data);
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->notation_loop,
			    &do_loop_value);

	list = list->next;
      }

      pthread_mutex_unlock(audio_mutex);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);

  /* enable fader */
  list = window->editor->editor_child;

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))){
    while(list != NULL){
      GtkWidget *edit_widget;

      edit_widget = AGS_EDITOR_CHILD(list->data)->edit_widget;
      
      if(AGS_IS_PATTERN_EDIT(edit_widget)){
	AGS_PATTERN_EDIT(edit_widget)->flags |= AGS_PATTERN_EDIT_DRAW_FADER;
      }else if(AGS_IS_NOTE_EDIT(edit_widget)){
	AGS_NOTE_EDIT(edit_widget)->flags |= AGS_NOTE_EDIT_DRAW_FADER;
      }

      list = list->next;
    }
  }else{
    while(list != NULL){
      GtkWidget *edit_widget;

      edit_widget = AGS_EDITOR_CHILD(list->data)->edit_widget;
      
      if(AGS_IS_PATTERN_EDIT(edit_widget)){
	AGS_PATTERN_EDIT(edit_widget)->flags &= (~AGS_PATTERN_EDIT_DRAW_FADER);
      }else if(AGS_IS_NOTE_EDIT(edit_widget)){
	AGS_NOTE_EDIT(edit_widget)->flags &= (~AGS_NOTE_EDIT_DRAW_FADER);
      }

      list = list->next;
    }
  }
}

void
ags_navigation_position_tact_callback(GtkWidget *widget,
				      AgsNavigation *navigation)
{
  ags_navigation_change_position(navigation,
				 gtk_spin_button_get_value((GtkSpinButton *) widget));
}

void
ags_navigation_duration_tact_callback(GtkWidget *widget,
				      AgsNavigation *navigation)
{
  /* empty */
}

void
ags_navigation_loop_left_tact_callback(GtkWidget *widget,
				       AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *machines, *machines_start;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio

  guint loop_left, loop_right;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  loop_left = 16 * navigation->loop_left_tact->adjustment->value;
  loop_right = 16 * navigation->loop_right_tact->adjustment->value;
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(window->soundcard),
			 loop_left, loop_right,
			 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_double(&value,
		     loop_left);

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      g_message("found machine to loop!\n\0");

      audio = machine->audio;
      
      /* get mutex manager and application mutex */
      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* do it so */
      pthread_mutex_lock(audio_mutex);      

      list = audio->play;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	recall = AGS_RECALL(list->data);
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->notation_loop_start,
			    &value);

	list = list->next;
      }

      pthread_mutex_unlock(audio_mutex);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);
}

void
ags_navigation_loop_right_tact_callback(GtkWidget *widget,
					AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;

  AgsAudio *audio;
  AgsRecall *recall;

  AgsMutexManager *mutex_manager;

  GList *machines, *machines_start;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio

  guint loop_left, loop_right;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  loop_left = 16 * navigation->loop_left_tact->adjustment->value;
  loop_right = 16 * navigation->loop_right_tact->adjustment->value;
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(window->soundcard),
			 loop_left, loop_right,
			 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_double(&value,
		     loop_right);

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      g_message("found machine to loop!\n\0");

      audio = machine->audio;
      
      /* get mutex manager and application mutex */
      mutex_manager = ags_mutex_manager_get_instance();
      application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

      /* get audio mutex */
      pthread_mutex_lock(application_mutex);

      audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					       (GObject *) audio);
  
      pthread_mutex_unlock(application_mutex);

      /* do it so */
      pthread_mutex_lock(audio_mutex);
      
      list = audio->play;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	recall = AGS_RECALL(list->data);
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->notation_loop_end,
			    &value);

	list = list->next;
      }

      pthread_mutex_unlock(audio_mutex);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);
}

void
ags_navigation_soundcard_stop_callback(GObject *soundcard,
				       AgsNavigation *navigation)
{  
  navigation->flags |= AGS_NAVIGATION_BLOCK_TIC;
}
