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

#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/ags_devout.h>

#include <ags/audio/jack/ags_jack_devout.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>

#include <ags/audio/wasapi/ags_wasapi_devout.h>

/**
 * SECTION:ags_soundcard_util
 * @short_description: soundcard util
 * @title: AgsSoundcardUtil
 * @section_id:
 * @include: ags/audio/ags_soundcard_util.h
 *
 * Soundcard utility functions.
 */

/**
 * ags_soundcard_util_get_obj_mutex:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Get object mutex of @soundcard.
 * 
 * Returns: GRecMutex pointer
 * 
 * Since: 3.0.0
 */
GRecMutex*
ags_soundcard_util_get_obj_mutex(GObject *soundcard)
{
  GRecMutex *obj_mutex;

  obj_mutex = NULL;
  
  if(AGS_IS_DEVOUT(soundcard)){
    obj_mutex = AGS_DEVOUT(soundcard)->obj_mutex;
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    obj_mutex = AGS_JACK_DEVOUT(soundcard)->obj_mutex;
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    obj_mutex = AGS_CORE_AUDIO_DEVOUT(soundcard)->obj_mutex;
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    obj_mutex = AGS_WASAPI_DEVOUT(soundcard)->obj_mutex;
  }else{
    g_warning("unknown soundcard implementation");
  }

  return(obj_mutex);
}
