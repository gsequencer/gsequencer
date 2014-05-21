/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_navigation_callbacks.h>

#include <ags/main.h>

#include <ags/thread/ags_audio_loop.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/recall/ags_count_beats_audio.h>

#include <ags/audio/task/ags_init_audio.h>
#include <ags/audio/task/ags_append_audio.h>
#include <ags/audio/task/ags_change_tact.h>
#include <ags/audio/task/ags_cancel_audio.h>
#include <ags/audio/task/ags_start_devout.h>

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
  GtkArrow *arrow = (GtkArrow *) gtk_container_get_children((GtkContainer *) widget)->data;

  widget = (GtkWidget *) gtk_container_get_children((GtkContainer *) navigation)->next->data;

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
  AgsWindow *window;
  AgsDevout *devout;
  AgsTaskThread *task_thread;
  AgsChangeTact *change_tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  devout = window->devout;

  task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread);

  change_tact = ags_change_tact_new(navigation,
				    -1.0 * AGS_NAVIGATION_REWIND_STEPS,
				    TRUE);
  ags_task_thread_append_task(task_thread, AGS_TASK(change_tact));
}

void
ags_navigation_prev_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsTaskThread *task_thread;
  AgsChangeTact *change_tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  devout = window->devout;

  task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread);

  change_tact = ags_change_tact_new(navigation,
				    -1.0 * AGS_NAVIGATION_SEEK_STEPS,
				    TRUE);
  ags_task_thread_append_task(task_thread, AGS_TASK(change_tact));
}

void
ags_navigation_play_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsDevout *devout;
  AgsInitAudio *init_audio;
  AgsAppendAudio *append_audio;
  AgsStartDevout *start_devout;
  GList *machines;
  GList *list;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  list = NULL;

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      printf("found machine to play!\n\0");

      devout = AGS_DEVOUT(machine->audio->devout);

      /* create init task */
      init_audio = ags_init_audio_new(machine->audio,
				      FALSE, TRUE, TRUE);
      list = g_list_prepend(list, init_audio);
    
      /* create append task */
      append_audio = ags_append_audio_new(G_OBJECT(AGS_MAIN(window->ags_main)->main_loop),
					  (GObject *) machine->audio);
      
      list = g_list_prepend(list, append_audio);
    }

    machines = machines->next;
  }

  /* create start task */
  if(list != NULL){
    start_devout = ags_start_devout_new(window->devout);
    list = g_list_prepend(list, start_devout);
    list = g_list_reverse(list);

    /* append AgsStartDevout */
    ags_task_thread_append_tasks(AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(window->ags_main)->main_loop)->task_thread),
				 list);
  }  
}

void
ags_navigation_stop_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  //  AGS_DEVOUT_GET_CLASS(navigation->devout)->stop(navigation->devout);

  /* empty */
}

void
ags_navigation_next_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsTaskThread *task_thread;
  AgsChangeTact *change_tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  devout = window->devout;

  task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread);

  change_tact = ags_change_tact_new(navigation,
				    AGS_NAVIGATION_REWIND_STEPS,
				    TRUE);
  ags_task_thread_append_task(task_thread, AGS_TASK(change_tact));
}

void
ags_navigation_forward_callback(GtkWidget *widget,
				AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsTaskThread *task_thread;
  AgsChangeTact *change_tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  devout = window->devout;

  task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread);

  change_tact = ags_change_tact_new(navigation,
				    AGS_NAVIGATION_SEEK_STEPS,
				    TRUE);
  ags_task_thread_append_task(task_thread, AGS_TASK(change_tact));
}

void
ags_navigation_loop_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsAudio *audio;
  AgsRecall *recall;
  GList *machines;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio
  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
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
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->loop,
			    &value);

	list = list->next;
      }
    }

    machines = machines->next;
  }
}

void
ags_navigation_position_tact_callback(GtkWidget *widget,
				      AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsDevout *devout;
  AgsTaskThread *task_thread;
  AgsChangeTact *change_tact;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  devout = window->devout;

  task_thread = AGS_TASK_THREAD(AGS_AUDIO_LOOP(AGS_MAIN(devout->ags_main)->main_loop)->task_thread);

  change_tact = ags_change_tact_new(navigation,
				    gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)),
				    TRUE);
  ags_task_thread_append_task(task_thread, AGS_TASK(change_tact));
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
  GList *machines;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio
  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
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
}

void
ags_navigation_loop_right_tact_callback(GtkWidget *widget,
					AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  AgsAudio *audio;
  AgsRecall *recall;
  GList *machines;
  GList *list; // find AgsPlayNotationAudio and AgsCopyPatternAudio
  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
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
}

void
ags_navigation_tic_callback(AgsDevout *devout,
			    AgsNavigation *navigation)
{
  gtk_spin_button_set_value(navigation->position_tact,
			    gtk_spin_button_get_value(navigation->position_tact) +
			    AGS_NAVIGATION_DEFAULT_TACT_STEP);
}
