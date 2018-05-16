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

#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>

/**
 * ags_soundcard_util_get_obj_mutex:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Get object mutex of @soundcard.
 * 
 * Returns: pthread_mutex_t pointer
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_soundcard_util_get_obj_mutex(GObject *soundcard)
{
  pthread_mutex_t *obj_mutex;

  obj_mutex = NULL;
  
  if(AGS_IS_MIDIIIN(soundcard)){
    pthread_mutex_lock(ags_devout_get_class_mutex());

    obj_mutex = AGS_DEVOUT(soundcard)->obj_mutex;
    
    pthread_mutex_unlock(ags_devout_get_class_mutex());
  }else if(AGS_IS_JACK_MIDIIIN(soundcard)){
    pthread_mutex_lock(ags_jack_devout_get_class_mutex());

    obj_mutex = AGS_JACK_DEVOUT(soundcard)->obj_mutex;
    
    pthread_mutex_unlock(ags_jack_devout_get_class_mutex());
  }else if(AGS_IS_CORE_AUDIO_MIDIIIN(soundcard)){
    pthread_mutex_lock(ags_core_audio_devout_get_class_mutex());

    obj_mutex = AGS_CORE_AUDIO_DEVOUT(soundcard)->obj_mutex;
    
    pthread_mutex_unlock(ags_core_audio_devout_get_class_mutex());
  }else{
    g_warning("unknown soundcard implementation");
  }

  return(obj_mutex);
}
