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

#include <ags/X/ags_editor_callbacks.h>

#include <ags/object/ags_application_context.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_notation.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/X/ags_window.h>

#include <ags/X/task/ags_scroll_on_play.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_notebook.h>
#include <ags/X/editor/ags_piano.h>
#include <ags/X/editor/ags_note_edit.h>

#include <math.h>
#include <string.h>
#include <cairo.h>

void
ags_editor_parent_set_callback(GtkWidget  *widget, GtkObject *old_parent, AgsEditor *editor)
{
  if(old_parent != NULL)
    return;
}

void
ags_editor_set_audio_channels_callback(AgsAudio *audio,
				       guint audio_channels, guint audio_channels_old,
				       AgsEditor *editor)
{
  GList *tabs;
  GList *notation;
  guint i;

  if(audio_channels_old < audio_channels){
    tabs = g_list_nth(editor->notebook->tabs,
		      audio_channels_old);
    notation = g_list_nth(audio->notation,
			  audio_channels_old);

    for(i = audio_channels_old; i < audio_channels; i++){
      ags_notebook_insert_tab(editor->notebook,
			      i);
      AGS_NOTEBOOK_TAB(tabs->data)->data = notation->data;

      tabs = tabs->next;
      notation = notation->next;
    }
  }else{
    for(i = audio_channels; i < audio_channels_old; i++){
      ags_notebook_remove_tab(editor->notebook,
			      i);
    }
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

  application_context = window->application_context;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /* get audio loop */
  pthread_mutex_lock(application_mutex);

  audio_loop = application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  /* get task and soundcard thread */
  task_thread = (AgsTaskThread *) ags_thread_find_type(audio_loop,
						       AGS_TYPE_TASK_THREAD);

  if(gtk_toggle_button_get_active(window->navigation->scroll)){
    AgsScrollOnPlay *scroll_on_play;
    double tact_factor, zoom_factor;
    double tact;
    gdouble step;
    
    zoom_factor = 0.25;

    tact_factor = exp2(6.0 - (double) gtk_combo_box_get_active((GtkComboBox *) window->editor->toolbar->zoom));
    tact = exp2((double) gtk_combo_box_get_active((GtkComboBox *) window->editor->toolbar->zoom) - 2.0);

    if(ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard)) > editor->current_tact){
      editor->current_tact = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));
      
      scroll_on_play = ags_scroll_on_play_new(editor, 64.0);
      ags_task_thread_append_task(task_thread,
				  AGS_TASK(scroll_on_play));
    }
  }

  ags_note_edit_set_map_height(editor->edit.note_edit,
			       pads * editor->edit.note_edit->control_height);
}

void
ags_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
				    AgsEditor *editor)
{
  ags_editor_machine_changed(editor,
			     machine);
}

void
ags_editor_change_position_callback(AgsNavigation *navigation, gdouble tact,
				    AgsEditor *editor)
{
  cairo_t *cr;
  gdouble loop_start, loop_end;
  gdouble position;

  if(!gtk_toggle_button_get_active(navigation->scroll)){
    return;
  }

  loop_start = gtk_spin_button_get_value(navigation->loop_left_tact);
  loop_end = gtk_spin_button_get_value(navigation->loop_right_tact);

  if(!gtk_toggle_button_get_active(navigation->loop) || tact <= loop_end){
    position = tact * editor->edit.note_edit->control_current.control_width;
  }else{
    position = loop_start * editor->edit.note_edit->control_current.control_width;
  }

  /* scroll */
  if(position - (0.125 * editor->edit.note_edit->control_current.control_width) > 0.0){
    gtk_range_set_value(GTK_RANGE(editor->edit.note_edit->hscrollbar),
			position - (0.125 * editor->edit.note_edit->control_current.control_width));
  }
}

void
ags_editor_edit_vscrollbar_value_changed_callback(GtkWidget *note_edit,
						  AgsEditor *editor)
{
  if((AGS_MACHINE_IS_SYNTHESIZER & (editor->selected_machine->flags)) != 0){
    gtk_widget_queue_draw(editor->meter.piano);
  }else if((AGS_MACHINE_IS_SEQUENCER & (editor->selected_machine->flags)) != 0){
    gtk_widget_queue_draw(editor->meter.soundset);
  }else{
  }
}
