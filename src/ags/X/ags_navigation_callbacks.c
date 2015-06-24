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

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_gui_thread.h>

#include <ags/audio/recall/ags_count_beats_audio.h>

#include <ags/audio/task/ags_change_tact.h>
#include <ags/audio/task/ags_display_tact.h>

#include <ags/audio/task/recall/ags_apply_bpm.h>

#include <ags/X/ags_window.h>

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

  navigation->devout = window->devout;
}

gboolean
ags_navigation_destroy_callback(GtkObject *object,
				gpointer data)
{
  ags_navigation_destroy(object);

  return(FALSE);
}

void
ags_navigation_show_callback(GtkWidget *widget,
			     gpointer data)
{
  ags_navigation_show(widget);
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
  
  window = AGS_WINDOW(gtk_widget_get_ancestor(widget,
					      AGS_TYPE_WINDOW));

  apply_bpm = ags_apply_bpm_new(G_OBJECT(window->devout),
				navigation->bpm->adjustment->value);

  ags_task_thread_append_task(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
			      AGS_TASK(apply_bpm));
}

void
ags_navigation_rewind_callback(GtkWidget *widget,
			       AgsNavigation *navigation)
{
  gtk_spin_button_set_value(navigation->position_tact,
			    navigation->position_tact->adjustment->value +
			    (-1.0 * AGS_NAVIGATION_REWIND_STEPS));
}

void
ags_navigation_prev_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  gtk_spin_button_set_value(navigation->position_tact,
			    navigation->position_tact->adjustment->value +
			    (-1.0 * AGS_NAVIGATION_SEEK_STEPS));
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
	navigation->start_tact = window->devout->tact_counter;
      }
      
      ags_machine_set_run_extended(machine,
				   TRUE,
				   !gtk_toggle_button_get_active(navigation->exclude_sequencer), TRUE);
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

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      printf("found machine to stop!\n\0");

      ags_machine_set_run_extended(machine,
				   FALSE,
				   !gtk_toggle_button_get_active(navigation->exclude_sequencer), TRUE);
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

  timestr = ags_navigation_tact_to_time_string(0.0,
					       navigation->bpm->adjustment->value);
  gtk_label_set_text(navigation->position_time, timestr);
  
  g_free(timestr);
}

void
ags_navigation_next_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  gtk_spin_button_set_value(navigation->position_tact,
			    navigation->position_tact->adjustment->value +
			    AGS_NAVIGATION_REWIND_STEPS);
}

void
ags_navigation_forward_callback(GtkWidget *widget,
				AgsNavigation *navigation)
{

  gtk_spin_button_set_value(navigation->position_tact,
			    navigation->position_tact->adjustment->value +
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
  GList *machines, *machines_start;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio
  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value,
		      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      g_message("found machine to loop!\n\0");

      audio = machine->audio;

      list = audio->play;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	recall = AGS_RECALL(list->data);
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->notation_loop,
			    &value);

	list = list->next;
      }
    }

    machines = machines->next;
  }

  g_list_free(machines_start);
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
  GList *machines, *machines_start;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio
  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_boolean(&value,
		      gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      g_message("found machine to loop!\n\0");

      audio = machine->audio;

      list = audio->play;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	recall = AGS_RECALL(list->data);
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->notation_loop_start,
			    &value);

	list = list->next;
      }
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
  GList *machines, *machines_start;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio
  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_boolean(&value,
		      gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      g_message("found machine to loop!\n\0");

      audio = machine->audio;

      list = audio->play;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	recall = AGS_RECALL(list->data);
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->notation_loop_end,
			    &value);

	list = list->next;
      }
    }

    machines = machines->next;
  }

  g_list_free(machines_start);
}

void
ags_navigation_tic_callback(AgsDevout *devout,
			    AgsNavigation *navigation)
{
  AgsTaskThread *task_thread;
  AgsChangeTact *change_tact;
  AgsDisplayTact *display_tact;
  GList *list;

  if((AGS_NAVIGATION_BLOCK_TIC & (navigation->flags)) != 0){
    navigation->flags &= (~AGS_NAVIGATION_BLOCK_TIC);
    return;
  }

  task_thread = (AgsTaskThread *) AGS_AUDIO_LOOP(AGS_MAIN(navigation->devout->ags_main)->main_loop)->task_thread;

  list = NULL;

  //  change_tact = ags_change_tact_new(navigation);
  //  list = g_list_prepend(list,
  //			change_tact);

  display_tact = ags_display_tact_new((GtkWidget *) navigation);
  list = g_list_prepend(list,
			display_tact);

  list = g_list_reverse(list);

  ags_task_thread_append_tasks(task_thread,
			       list);
}

void
ags_navigation_devout_stop_callback(AgsDevout *devout,
				    AgsNavigation *navigation)
{  
  navigation->flags |= AGS_NAVIGATION_BLOCK_TIC;
}
