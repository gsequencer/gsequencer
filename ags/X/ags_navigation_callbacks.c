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

#include <ags/X/ags_navigation_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

void
ags_navigation_expander_callback(GtkWidget *widget,
				 AgsNavigation *navigation)
{
  GtkArrow *arrow;

  GList *list;

  guint arrow_type;
  
  list = gtk_container_get_children((GtkContainer *) widget);
  arrow = (GtkArrow *) list->data;
  g_list_free(list);

  list = gtk_container_get_children((GtkContainer *) navigation);
  widget = (GtkWidget *) list->next->data;
  g_list_free(list);

  g_object_get(arrow,
	       "arrow-type", &arrow_type,
	       NULL);
  
  if(arrow_type == GTK_ARROW_DOWN){
    gtk_widget_hide_all(widget);

    g_object_set(arrow,
		 "arrow-type", GTK_ARROW_RIGHT,
		 NULL);
  }else{
    gtk_widget_show_all(widget);

    g_object_set(arrow,
		 "arrow-type", GTK_ARROW_DOWN,
		 NULL);
  }
}

void
ags_navigation_bpm_callback(GtkWidget *widget,
			    AgsNavigation *navigation)
{
  AgsApplyBpm *apply_bpm;

  AgsApplicationContext *application_context;
  
  application_context = ags_application_context_get_instance();

  /* get task thread */
  apply_bpm = ags_apply_bpm_new(application_context,
				gtk_spin_button_get_value(navigation->bpm));
  
  ags_ui_provider_schedule_task(AGS_UI_PROVIDER(application_context),
				(AgsTask *) apply_bpm);
}

void
ags_navigation_rewind_callback(GtkWidget *widget,
			       AgsNavigation *navigation)
{
  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  gdouble tact;

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard)) / 16.0;
  
  gtk_spin_button_set_value(navigation->position_tact,
			    tact +
			    (-1.0 * AGS_NAVIGATION_DEFAULT_TACT_STEP));
}

void
ags_navigation_prev_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  gdouble tact;

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard)) / 16.0;
  
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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;

  gboolean initialized_time;
  
  if((AGS_NAVIGATION_BLOCK_PLAY & (navigation->flags)) != 0){
    return;
  }

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  application_context = ags_application_context_get_instance();
  
  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines_start =
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));
  
  initialized_time = FALSE;
  
  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to play!\n");
#endif
      
      if(!initialized_time){
	initialized_time = TRUE;
	navigation->start_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard));
      }
      
      ags_machine_set_run_extended(machine,
				   TRUE,
				   !gtk_toggle_button_get_active((GtkToggleButton *) navigation->exclude_sequencer), TRUE, FALSE, FALSE);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to play!\n");
#endif
      
      if(!initialized_time){
	initialized_time = TRUE;
	navigation->start_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard));
      }
      
      ags_machine_set_run_extended(machine,
				   TRUE,
				   FALSE, FALSE, TRUE, FALSE);
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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines,*machines_start;

  gchar *timestr;

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to stop!");
#endif
      
      ags_machine_set_run_extended(machine,
				   FALSE,
				   !gtk_toggle_button_get_active((GtkToggleButton *) navigation->exclude_sequencer), TRUE, FALSE, FALSE);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to stop!");
#endif
      
      ags_machine_set_run_extended(machine,
				   FALSE,
				   FALSE, FALSE, TRUE, FALSE);
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
					    gtk_spin_button_get_value(navigation->bpm),
					    ags_soundcard_get_delay(AGS_SOUNDCARD(default_soundcard)),
					    ags_soundcard_get_delay_factor(AGS_SOUNDCARD(default_soundcard)));
  gtk_label_set_text(navigation->duration_time, timestr);
  
  g_free(timestr);
  ags_soundcard_set_note_offset(AGS_SOUNDCARD(default_soundcard),
				0);
}

void
ags_navigation_next_callback(GtkWidget *widget,
			     AgsNavigation *navigation)
{
  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  gdouble tact;

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard)) / 16.0;

  gtk_spin_button_set_value(navigation->position_tact,
			    tact +
			    AGS_NAVIGATION_REWIND_STEPS);
}

void
ags_navigation_forward_callback(GtkWidget *widget,
				AgsNavigation *navigation)
{
  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  gdouble tact;

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard)) / 16.0;

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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;
  GList *list, *list_start;

  guint loop_left, loop_right;
  
  GValue do_loop_value = {0,};
  
  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  loop_left = 16 * gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_right = 16 * gtk_spin_button_get_value(navigation->loop_right_tact);
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(default_soundcard),
			 loop_left, loop_right,
			 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
			 
  g_value_init(&do_loop_value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&do_loop_value,
		      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "notation-loop", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &do_loop_value);

	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_PLAY_WAVE_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "wave-loop", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &do_loop_value);

	list = list->next;
      }

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_CAPTURE_WAVE_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "wave-loop", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &do_loop_value);

	list = list->next;
      }
      
      g_list_free_full(list_start,
		       g_object_unref);
    }

    machines = machines->next;
  }

  g_list_free(machines_start);

  /* enable fader */
  //TODO:JK: implement me
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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;
  GList *list, *list_start; // find AgsPlayNotationAudio and AgsCopyPatternAudio

  guint loop_left, loop_right;

  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  loop_left = 16 * gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_right = 16 * gtk_spin_button_get_value(navigation->loop_right_tact);
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(default_soundcard),
			 loop_left, loop_right,
			 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

  g_value_init(&value, G_TYPE_UINT64);
  g_value_set_uint64(&value,
		     (guint64) loop_left);

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "notation-loop-start", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
	
	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);
      
      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_PLAY_WAVE_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "wave-loop-start", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }

      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_CAPTURE_WAVE_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "wave-loop-start", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }
      
      g_list_free_full(list_start,
		       g_object_unref);
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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;
  GList *list, *list_start; // find AgsPlayNotationAudio and AgsCopyPatternAudio

  guint loop_left, loop_right;

  GValue value = {0,};

  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(navigation)));

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines_start = 
    machines = gtk_container_get_children(GTK_CONTAINER(window->machines));

  loop_left = 16 * gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_right = 16 * gtk_spin_button_get_value(navigation->loop_right_tact);
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(default_soundcard),
			 loop_left, loop_right,
			 gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(navigation->loop)));

  g_value_init(&value, G_TYPE_UINT64);
  g_value_set_uint64(&value,
		     (guint64) loop_right);

  while(machines != NULL){
    machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = machine->audio;
      

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_COUNT_BEATS_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "notation-loop-end", &port,
		     NULL);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = machine->audio;
      
      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_PLAY_WAVE_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "wave-loop-end", &port,
		     NULL);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }

      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_CAPTURE_WAVE_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "wave-loop-end", &port,
		     NULL);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
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
