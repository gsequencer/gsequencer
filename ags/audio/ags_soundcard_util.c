/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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
#include <ags/audio/ags_devin.h>

#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>

#include <ags/audio/wasapi/ags_wasapi_devout.h>
#include <ags/audio/wasapi/ags_wasapi_devin.h>

#if defined(AGS_WITH_GSTREAMER)
#include <ags/audio/gstreamer/ags_gstreamer_devout.h>
#include <ags/audio/gstreamer/ags_gstreamer_devin.h>
#endif

#include <math.h>

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
 * Returns: (transfer none): GRecMutex pointer
 * 
 * Since: 3.0.0
 */
GRecMutex*
ags_soundcard_util_get_obj_mutex(GObject *soundcard)
{
  GRecMutex *obj_mutex;

  obj_mutex = NULL;
  
  if(AGS_IS_DEVOUT(soundcard)){
    obj_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_DEVIN(soundcard)){
    obj_mutex = AGS_DEVIN_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    obj_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    obj_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    obj_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    obj_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    obj_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    obj_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    obj_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    obj_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(soundcard);
#if defined(AGS_WITH_GSTREAMER)
  }else if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    obj_mutex = AGS_GSTREAMER_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    obj_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(soundcard);
#endif
  }else{
    g_warning("unknown soundcard implementation");
  }

  return(obj_mutex);
}

/**
 * ags_soundcard_util_adjust_delay_and_attack:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Adjust delay and attack of @soundcard.
 * 
 * Since: 3.3.0
 */
void
ags_soundcard_util_adjust_delay_and_attack(GObject *soundcard)
{
  gdouble *delay;
  guint *attack;
  gdouble absolute_delay;
  guint buffer_size;
  guint default_tact_frames;
  guint i;

  GRecMutex *obj_mutex;

  obj_mutex = ags_soundcard_util_get_obj_mutex(soundcard);
  
  /* get some initial values */
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(soundcard));

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  attack = NULL;
  delay = NULL;
  
  g_rec_mutex_lock(obj_mutex);
  
  if(AGS_IS_DEVOUT(soundcard)){
    attack = AGS_DEVOUT(soundcard)->attack;
    delay = AGS_DEVOUT(soundcard)->delay;
  }else if(AGS_IS_DEVIN(soundcard)){
    attack = AGS_DEVIN(soundcard)->attack;
    delay = AGS_DEVIN(soundcard)->delay;
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    attack = AGS_PULSE_DEVOUT(soundcard)->attack;
    delay = AGS_PULSE_DEVOUT(soundcard)->delay;
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    attack = AGS_PULSE_DEVIN(soundcard)->attack;
    delay = AGS_PULSE_DEVIN(soundcard)->delay;
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    attack = AGS_JACK_DEVOUT(soundcard)->attack;
    delay = AGS_JACK_DEVOUT(soundcard)->delay;
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    attack = AGS_JACK_DEVIN(soundcard)->attack;
    delay = AGS_JACK_DEVIN(soundcard)->delay;
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    attack = AGS_CORE_AUDIO_DEVOUT(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay;
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    attack = AGS_CORE_AUDIO_DEVIN(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVIN(soundcard)->delay;
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    attack = AGS_WASAPI_DEVOUT(soundcard)->attack;
    delay = AGS_WASAPI_DEVOUT(soundcard)->delay;
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    attack = AGS_WASAPI_DEVIN(soundcard)->attack;
    delay = AGS_WASAPI_DEVIN(soundcard)->delay;
#if defined(AGS_WITH_GSTREAMER)
  }else if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    attack = AGS_GSTREAMER_DEVOUT(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVOUT(soundcard)->delay;
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    attack = AGS_GSTREAMER_DEVIN(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVIN(soundcard)->delay;
#endif
  }else{
    g_warning("unknown soundcard implementation");

    g_rec_mutex_unlock(obj_mutex);

    return;
  }

  default_tact_frames = absolute_delay * buffer_size;
  
  attack[0] = 0; // (guint) floor((2.0 * M_PI / (4.0 * buffer_size)) * buffer_size);
  delay[0] = (default_tact_frames + attack[0]) / buffer_size;
  
  for(i = 1; i < AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    gint64 current_attack;

    current_attack = (((attack[i - 1] + default_tact_frames) / buffer_size) - absolute_delay) * buffer_size;

    if(current_attack >= buffer_size){
      attack[i] = buffer_size - 1;
    }else if(current_attack < 0){
      attack[i] = 0;
    }else{
      attack[i] = current_attack;
    }
    
    delay[i] = (default_tact_frames + attack[i] - attack[i - 1]) / buffer_size;
  }
  
  for(; i < 2 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    gint64 current_attack;

    current_attack = ((attack[2 * (guint) AGS_SOUNDCARD_DEFAULT_PERIOD - i - 1] + default_tact_frames) - absolute_delay) * buffer_size;

    if(current_attack >= buffer_size){
      attack[i] = buffer_size - 1;
    }else if(current_attack < 0){
      attack[i] = 0;
    }else{
      attack[i] = current_attack;
    }

    delay[i] = (default_tact_frames + attack[i] - attack[i - 1]) / buffer_size;
  }

  g_rec_mutex_unlock(obj_mutex);  
}
