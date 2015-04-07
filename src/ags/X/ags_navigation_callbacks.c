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

#include <ags/object/ags_application_context.h>

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

  navigation->soundcard = window->soundcard;
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
  
  AgsThread *main_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;
  
  window = AGS_WINDOW(gtk_widget_get_ancestor(widget,
					      AGS_TYPE_WINDOW));

  application_context = window->application_context;
  
  main_loop = application_context->main_loop;

  task_thread = ags_thread_find_type(main_loop,
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
  gtk_spin_button_set_value(navigation->position_tact,
			    -1.0 * AGS_NAVIGATION_REWIND_STEPS +
			    AGS_NAVIGATION_DEFAULT_TACT_STEP);
}

void
ags_navigation_prev_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  gtk_spin_button_set_value(navigation->position_tact,
			    -1.0 * AGS_NAVIGATION_SEEK_STEPS +
			    AGS_NAVIGATION_DEFAULT_TACT_STEP);
}

void
ags_navigation_play_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsWindow *window;
  AgsMachine *machine;
  GList *machines, *machines_start;

  if((AGS_NAVIGATION_BLOCK_PLAY & (navigation->flags)) != 0){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start =
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      printf("found machine to play!\n\0");

      ags_machine_set_run(machine,
			  TRUE);
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

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));
  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) !=0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
      printf("found machine to stop!\n\0");

      ags_machine_set_run(machine,
			  FALSE);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);

  /* toggle play button */
  navigation->flags |= AGS_NAVIGATION_BLOCK_PLAY;
  gtk_toggle_button_set_active(navigation->play,
			       FALSE);

  navigation->flags &= (~AGS_NAVIGATION_BLOCK_PLAY);
}

void
ags_navigation_next_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  gtk_spin_button_set_value(navigation->position_tact,
			    AGS_NAVIGATION_REWIND_STEPS+
			    AGS_NAVIGATION_DEFAULT_TACT_STEP);
}

void
ags_navigation_forward_callback(GtkWidget *widget,
				AgsNavigation *navigation)
{

  gtk_spin_button_set_value(navigation->position_tact,
			    AGS_NAVIGATION_SEEK_STEPS +
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
	ags_port_safe_write(AGS_COUNT_BEATS_AUDIO(recall)->loop,
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
  gdouble bpm;

  bpm = navigation->bpm->adjustment->value;
  ags_navigation_change_position(navigation,
				 gtk_spin_button_get_value(widget),
				 bpm);
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
ags_navigation_tic_callback(AgsSoundcard *soundcard,
			    AgsNavigation *navigation)
{ 
  AgsThread *main_loop;
  AgsTaskThread *task_thread;
  
  AgsChangeTact *change_tact;
  AgsDisplayTact *display_tact;

  AgsApplicationContext *application_context;
  
  GList *list;

  application_context = ags_soundcard_get_application_context(soundcard);
  
  main_loop = application_context->main_loop;

  task_thread = ags_thread_find_type(main_loop,
				     AGS_TYPE_TASK_THREAD);
  list = NULL;

  change_tact = ags_change_tact_new(navigation);
  list = g_list_prepend(list,
			change_tact);

  display_tact = ags_display_tact_new(navigation);
  list = g_list_prepend(list,
			display_tact);

  list = g_list_reverse(list);

  ags_task_thread_append_tasks(task_thread,
			       list);
}
