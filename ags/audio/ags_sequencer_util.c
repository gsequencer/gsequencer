/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/ags_sequencer_util.h>

#include <ags/audio/ags_midiin.h>

#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/core-audio/ags_core_audio_midiin.h>

/**
 * ags_sequencer_util_get_obj_mutex:
 * @sequencer: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Get object mutex of @sequencer.
 * 
 * Returns: pthread_mutex_t pointer
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_sequencer_util_get_obj_mutex(GObject *sequencer)
{
  pthread_mutex_t *obj_mutex;

  obj_mutex = NULL;
  
  if(AGS_IS_MIDIIN(sequencer)){
    pthread_mutex_lock(ags_midiin_get_class_mutex());

    obj_mutex = AGS_MIDIIN(sequencer)->obj_mutex;
    
    pthread_mutex_unlock(ags_midiin_get_class_mutex());
  }else if(AGS_IS_JACK_MIDIIN(sequencer)){
    pthread_mutex_lock(ags_jack_midiin_get_class_mutex());

    obj_mutex = AGS_JACK_MIDIIN(sequencer)->obj_mutex;
    
    pthread_mutex_unlock(ags_jack_midiin_get_class_mutex());
  }else if(AGS_IS_CORE_AUDIO_MIDIIN(sequencer)){
    pthread_mutex_lock(ags_core_audio_midiin_get_class_mutex());

    obj_mutex = AGS_CORE_AUDIO_MIDIIN(sequencer)->obj_mutex;
    
    pthread_mutex_unlock(ags_core_audio_midiin_get_class_mutex());
  }else{
    g_warning("unknown sequencer implementation");
  }

  return(obj_mutex);
}
