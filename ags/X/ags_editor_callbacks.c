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

#include <ags/X/ags_editor_callbacks.h>

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
ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor)
{
  if(old_parent != NULL)
    return;

  if(AGS_IS_NOTE_EDIT(editor->current_edit_widget)){
    AGS_NOTE_EDIT(editor->current_edit_widget)->flags |= AGS_NOTE_EDIT_RESETING_HORIZONTALLY;
    ags_note_edit_reset_horizontally(AGS_NOTE_EDIT(editor->current_edit_widget), AGS_NOTE_EDIT_RESET_HSCROLLBAR);
    AGS_NOTE_EDIT(editor->current_edit_widget)->flags &= (~AGS_NOTE_EDIT_RESETING_HORIZONTALLY);
  }else if(AGS_IS_PATTERN_EDIT(editor->current_edit_widget)){
    AGS_PATTERN_EDIT(editor->current_edit_widget)->flags |= AGS_PATTERN_EDIT_RESETING_HORIZONTALLY;
    ags_pattern_edit_reset_horizontally(AGS_PATTERN_EDIT(editor->current_edit_widget), AGS_PATTERN_EDIT_RESET_HSCROLLBAR);
    AGS_PATTERN_EDIT(editor->current_edit_widget)->flags &= (~AGS_PATTERN_EDIT_RESETING_HORIZONTALLY);
  }
}

void
ags_editor_tic_callback(GObject *soundcard,
			AgsEditor *editor)

{
  AgsWindow *window;

  AgsMutexManager *mutex_manager;
  AgsAudioLoop *audio_loop;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  pthread_mutex_t *application_mutex;
  
  window = AGS_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(editor)));

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

  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(window->navigation->scroll))){
    AgsScrollOnPlay *scroll_on_play;

    gdouble step;
    
    if(ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard)) > editor->current_tact){
      editor->current_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));
      
      scroll_on_play = ags_scroll_on_play_new((GtkWidget *) editor,
					      64.0);
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(scroll_on_play));
    }
  }
}

void
ags_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
				    AgsEditor *editor)
{
  ags_editor_machine_changed(editor, machine);
}
