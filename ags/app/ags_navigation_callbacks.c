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

#include <ags/app/ags_navigation_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/editor/ags_notation_edit.h>
#include <ags/app/editor/ags_scrolled_automation_edit_box.h>
#include <ags/app/editor/ags_automation_edit_box.h>
#include <ags/app/editor/ags_automation_edit.h>
#include <ags/app/editor/ags_scrolled_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit_box.h>
#include <ags/app/editor/ags_wave_edit.h>
#include <ags/app/editor/ags_tempo_edit.h>

void
ags_navigation_update_ui_callback(GObject *ui_provider,
				  AgsNavigation *navigation)
{
  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  gchar *str;

  application_context = ags_application_context_get_instance();

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  if(default_soundcard != NULL){
    str = ags_soundcard_get_uptime(AGS_SOUNDCARD(default_soundcard));
    
    g_object_set(navigation->duration_time,
		 "label", str,
		 NULL);
    g_free(str);
  
    gtk_widget_queue_draw((GtkWidget *) navigation->duration_time);
  }
}

void
ags_navigation_expander_callback(GtkWidget *widget,
				 AgsNavigation *navigation)
{
  gchar *icon_name;
  
  g_object_get(navigation->expander,
	       "icon-name", &icon_name,
	       NULL);
  
  if(!g_strcmp0("pan-down",
		icon_name)){
    gtk_widget_hide((GtkWidget *) navigation->expansion_box);

    g_object_set(navigation->expander,
		 "icon-name", "pan-end",
		 NULL);
  }else{
    gtk_widget_show((GtkWidget *) navigation->expansion_box);

    g_object_set(navigation->expander,
		 "icon-name", "pan-down",
		 NULL);
  }
}

void
ags_navigation_bpm_callback(GtkWidget *widget,
			    AgsNavigation *navigation)
{
  AgsApplyBpm *apply_bpm;

  AgsApplicationContext *application_context;

  if((AGS_NAVIGATION_BLOCK_BPM & (navigation->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  /* get task thread */
  apply_bpm = ags_apply_bpm_new((GObject *) application_context,
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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;

  gboolean initialized_time;
  
  if((AGS_NAVIGATION_BLOCK_PLAY & (navigation->flags)) != 0){
    return;
  }

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  machines =
    machines_start = ags_window_get_machine(window);
  
  initialized_time = FALSE;
  
  while(machines != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to play!\n");
#endif
      
      if(!initialized_time){
	initialized_time = TRUE;
	navigation->start_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard));
      }
      
      ags_machine_set_run_extended(current_machine,
				   TRUE,
				   !gtk_check_button_get_active(navigation->exclude_sequencer), TRUE, FALSE, FALSE);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to play!\n");
#endif
      
      if(!initialized_time){
	initialized_time = TRUE;
	navigation->start_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(default_soundcard));
      }
      
      ags_machine_set_run_extended(current_machine,
				   TRUE,
				   FALSE, TRUE, FALSE, FALSE);
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

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines,*machines_start;

  gchar *timestr;
  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines =
    machines_start = ags_window_get_machine(window);

  while(machines != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to stop!");
#endif
      
      ags_machine_set_run_extended(current_machine,
				   FALSE,
				   !gtk_check_button_get_active(navigation->exclude_sequencer), TRUE, FALSE, FALSE);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to stop!");
#endif
      
      ags_machine_set_run_extended(current_machine,
				   FALSE,
				   FALSE, TRUE, FALSE, FALSE);
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
					    ags_soundcard_get_bpm(AGS_SOUNDCARD(default_soundcard)),
					    ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(default_soundcard)),
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

  AgsAudio *audio;
  AgsRecall *recall;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;
  GList *list, *list_start;

  guint loop_left, loop_right;
  
  GValue do_loop_value = {0,};

  application_context = ags_application_context_get_instance();
  
  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines =
    machines_start = ags_window_get_machine(window);

  loop_left = 16 * gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_right = 16 * gtk_spin_button_get_value(navigation->loop_right_tact);
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(default_soundcard),
			 loop_left, loop_right,
			 gtk_check_button_get_active(GTK_CHECK_BUTTON(widget)));
			 
  g_value_init(&do_loop_value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&do_loop_value,
		      gtk_check_button_get_active(GTK_CHECK_BUTTON(widget)));

  while(machines != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!");
#endif
      
      audio = current_machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &do_loop_value);

	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);

      /* recall context */
      g_object_get(audio,
		   "recall", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &do_loop_value);

	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = current_machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &do_loop_value);

	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);

      /* recall context */
      g_object_get(audio,
		   "recall", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
	AgsPort *port;
	
	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop", &port,
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
ags_navigation_scroll_callback(GtkWidget *widget,
			       AgsNavigation *navigation)
{
  AgsCompositeEditor *composite_editor;
  
  AgsApplicationContext *application_context;

  GList *start_list, *list;
  
  gboolean do_scroll;

  application_context = ags_application_context_get_instance();

  composite_editor = (AgsCompositeEditor *) ags_ui_provider_get_composite_editor(AGS_UI_PROVIDER(application_context));

  do_scroll = gtk_check_button_get_active(GTK_CHECK_BUTTON(widget));

  if(do_scroll){
    AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->flags |= AGS_NOTATION_EDIT_AUTO_SCROLL;

    list = 
      start_list = ags_automation_edit_box_get_automation_edit(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box);

    while(list != NULL){
      AGS_AUTOMATION_EDIT(list->data)->flags |= AGS_AUTOMATION_EDIT_AUTO_SCROLL;

      list = list->next;
    }
    
    g_list_free(start_list);
    
    list = 
      start_list = ags_wave_edit_box_get_wave_edit(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box);

    while(list != NULL){
      AGS_WAVE_EDIT(list->data)->flags |= AGS_WAVE_EDIT_AUTO_SCROLL;

      list = list->next;
    }

    g_list_free(start_list);    

    AGS_TEMPO_EDIT(composite_editor->tempo_edit)->flags |= AGS_TEMPO_EDIT_AUTO_SCROLL;
  }else{
    AGS_NOTATION_EDIT(composite_editor->notation_edit->edit)->flags &= (~AGS_NOTATION_EDIT_AUTO_SCROLL);

    list = 
      start_list = ags_automation_edit_box_get_automation_edit(AGS_SCROLLED_AUTOMATION_EDIT_BOX(composite_editor->automation_edit->edit)->automation_edit_box);

    while(list != NULL){
      AGS_AUTOMATION_EDIT(list->data)->flags &= (~AGS_AUTOMATION_EDIT_AUTO_SCROLL);

      list = list->next;
    }

    g_list_free(start_list);    

    list = 
      start_list = ags_wave_edit_box_get_wave_edit(AGS_SCROLLED_WAVE_EDIT_BOX(composite_editor->wave_edit->edit)->wave_edit_box);

    while(list != NULL){
      AGS_WAVE_EDIT(list->data)->flags &= (~AGS_WAVE_EDIT_AUTO_SCROLL);

      list = list->next;
    }

    g_list_free(start_list);
    
    AGS_TEMPO_EDIT(composite_editor->tempo_edit)->flags &= (~AGS_TEMPO_EDIT_AUTO_SCROLL);
  }
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

  AgsAudio *audio;
  AgsRecall *recall;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;
  GList *list, *list_start; // find AgsPlayNotationAudio and AgsCopyPatternAudio

  guint loop_left, loop_right;

  GValue value = {0,};

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines =
    machines_start = ags_window_get_machine(window);

  loop_left = 16 * gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_right = 16 * gtk_spin_button_get_value(navigation->loop_right_tact);
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(default_soundcard),
			 loop_left, loop_right,
			 gtk_check_button_get_active(GTK_CHECK_BUTTON(navigation->loop)));

  g_value_init(&value, G_TYPE_UINT64);
  g_value_set_uint64(&value,
		     (guint64) loop_left);

  while(machines != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = current_machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-start", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
	
	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);

      /* recall context */
      g_object_get(audio,
		   "recall", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-start", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
	
	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = current_machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);
      
      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-start", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }
      
      g_list_free_full(list_start,
		       g_object_unref);

      /* recall context */
      g_object_get(audio,
		   "recall", &list_start,
		   NULL);

      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-start", &port,
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

  AgsAudio *audio;
  AgsRecall *recall;

  AgsApplicationContext *application_context;

  GObject *default_soundcard;

  GList *machines, *machines_start;
  GList *list, *list_start; // find AgsPlayNotationAudio and AgsCopyPatternAudio

  guint loop_left, loop_right;

  GValue value = {0,};

  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));

  default_soundcard = ags_sound_provider_get_default_soundcard(AGS_SOUND_PROVIDER(application_context));

  machines =
    machines_start = ags_window_get_machine(window);

  loop_left = 16 * gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_right = 16 * gtk_spin_button_get_value(navigation->loop_right_tact);
  
  ags_soundcard_set_loop(AGS_SOUNDCARD(default_soundcard),
			 loop_left, loop_right,
			 gtk_check_button_get_active(GTK_CHECK_BUTTON(navigation->loop)));

  g_value_init(&value, G_TYPE_UINT64);
  g_value_set_uint64(&value,
		     (guint64) loop_right);

  while(machines != NULL){
    AgsMachine *current_machine;
    
    current_machine = AGS_MACHINE(machines->data);

    if((AGS_MACHINE_IS_SEQUENCER & (current_machine->flags)) != 0 ||
       (AGS_MACHINE_IS_SYNTHESIZER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = current_machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-end", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
	
	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);

      /* recall context */
      g_object_get(audio,
		   "recall", &list_start,
		   NULL);

      list = list_start;
      
      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_NOTATION_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-end", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
	
	list = list->next;
      }

      g_list_free_full(list_start,
		       g_object_unref);
    }else if((AGS_MACHINE_IS_WAVE_PLAYER & (current_machine->flags)) != 0){
#ifdef AGS_DEBUG
      g_message("found machine to loop!\n");
#endif
      
      audio = current_machine->audio;

      /* do it so */
      g_object_get(audio,
		   "play", &list_start,
		   NULL);
      
      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-end", &port,
		     NULL);
	
	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);

	list = list->next;
      }
      
      g_list_free_full(list_start,
		       g_object_unref);

      /* recall context */
      g_object_get(audio,
		   "recall", &list_start,
		   NULL);

      list = list_start;

      while((list = ags_recall_find_type(list,
					 AGS_TYPE_FX_PLAYBACK_AUDIO)) != NULL){
	AgsPort *port;

	recall = AGS_RECALL(list->data);

	g_object_get(recall,
		     "loop-end", &port,
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
