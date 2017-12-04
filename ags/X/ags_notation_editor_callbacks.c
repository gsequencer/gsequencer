/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_notation_editor_callbacks.h>

void
ags_notation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					     AgsNotationEditor *notation_editor)
{
  ags_notation_editor_machine_changed(notation_editor, machine);
}

void
ags_notation_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
				      guint pads, guint pads_old,
				      AgsNotationEditor *notation_editor)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *audio_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* get audio mutex */
  pthread_mutex_lock(application_mutex);
  
  audio_mutex = ags_mutex_manager_lookup(mutex_manager,
					 (GObject *) audio);
  
  pthread_mutex_unlock(application_mutex);

  /* verify pads */
  pthread_mutex_lock(audio_mutex);
  
  if((AGS_AUDIO_NOTATION_DEFAULT & (audio->flags)) != 0){
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_INPUT)){
      pthread_mutex_unlock(audio_mutex);
      
      return;
    }    
  }else{
    if(!g_type_is_a(channel_type,
		    AGS_TYPE_OUTPUT)){
      pthread_mutex_unlock(audio_mutex);
      
      return;
    }
  }
  
  pthread_mutex_unlock(audio_mutex);

  /*  */
  g_object_set(notation_editor->scrolled_piano->piano,
	       "key-count", pads,
	       NULL);
  gtk_widget_queue_draw(notation_editor->scrolled_piano->piano);

  /*  */
  gtk_widget_queue_draw(notation_editor->notation_edit);
}
