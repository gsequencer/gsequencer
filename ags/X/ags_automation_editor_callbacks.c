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

#include <ags/X/ags_automation_editor_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_notation.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/X/ags_window.h>

#include <ags/X/task/ags_scroll_on_play.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_meter.h>
#include <ags/X/editor/ags_note_edit.h>
#include <ags/X/editor/ags_pattern_edit.h>

#include <math.h>
#include <string.h>
#include <cairo.h>

void
ags_automation_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsAutomationEditor *automation_editor)
{
  if(old_parent != NULL){
    return;
  }
}

void
ags_automation_editor_tic_callback(GObject *soundcard,
				   AgsAutomationEditor *automation_editor)

{
  AgsWindow *window;
  AgsAutomationWindow *automation_window;
  
  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;

  automation_window = AGS_AUTOMATION_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(automation_editor)));
  window = (AgsWindow *) automation_window->parent_window;

  application_context = (AgsApplicationContext *) window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsAudioLoop *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type((AgsThread *) audio_loop,
						       AGS_TYPE_TASK_THREAD);

  if(window->navigation->scroll != NULL &&
     gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(window->navigation->scroll))){
    AgsScrollOnPlay *scroll_on_play;
    gdouble step;
    
    if(ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard)) > automation_editor->current_tact){
      automation_editor->current_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));
      
      scroll_on_play = ags_scroll_on_play_new((GtkWidget *) automation_editor,
					      64.0);
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(scroll_on_play));
    }
  }
}

void
ags_automation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					       AgsAutomationEditor *automation_editor)
{
  ags_automation_editor_machine_changed(automation_editor, machine);
}
