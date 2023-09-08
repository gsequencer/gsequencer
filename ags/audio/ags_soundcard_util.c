/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/alsa/ags_alsa_devout.h>
#include <ags/audio/alsa/ags_alsa_devin.h>
#include <ags/audio/alsa/ags_alsa_midiin.h>

#include <ags/audio/oss/ags_oss_devout.h>
#include <ags/audio/oss/ags_oss_devin.h>
#include <ags/audio/oss/ags_oss_midiin.h>

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

gpointer ags_soundcard_util_copy(gpointer ptr);
void ags_soundcard_util_free(gpointer ptr);

/**
 * SECTION:ags_soundcard_util
 * @short_description: soundcard util
 * @title: AgsSoundcardUtil
 * @section_id:
 * @include: ags/audio/ags_soundcard_util.h
 *
 * Soundcard utility functions.
 */

GType
ags_soundcard_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_soundcard_util = 0;

    ags_type_soundcard_util =
      g_boxed_type_register_static("AgsSoundcardUtil",
				   (GBoxedCopyFunc) ags_soundcard_util_copy,
				   (GBoxedFreeFunc) ags_soundcard_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_soundcard_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_soundcard_util_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsSoundcardUtil));
 
  return(retval);
}

void
ags_soundcard_util_free(gpointer ptr)
{
  g_free(ptr);
}

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
  
  if(AGS_IS_ALSA_DEVOUT(soundcard)){
    obj_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_ALSA_DEVIN(soundcard)){
    obj_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_OSS_DEVOUT(soundcard)){
    obj_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(soundcard);
  }else if(AGS_IS_OSS_DEVIN(soundcard)){
    obj_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(soundcard);
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
  guint *tic_counter;
  guint *note_256th_offset;
  guint *note_256th_offset_last;
  gdouble *note_256th_tic_size;

  gdouble absolute_delay;
  gdouble corrected_delay;
  guint buffer_size;
  guint default_tact_frames;
  guint total_correct_frame_count;
  gdouble correct_frame_count;
  guint i;

  GRecMutex *obj_mutex;

  if(soundcard == NULL ||
     !AGS_IS_SOUNDCARD(soundcard)){
    return;
  }
  
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

  tic_counter = NULL;

  note_256th_offset = NULL;
  note_256th_offset_last = NULL;
  note_256th_tic_size = NULL;
  
  g_rec_mutex_lock(obj_mutex);
  
  if(AGS_IS_ALSA_DEVOUT(soundcard)){
    attack = AGS_ALSA_DEVOUT(soundcard)->attack;
    delay = AGS_ALSA_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_ALSA_DEVOUT(soundcard)->tic_counter);

    note_256th_offset = &(AGS_ALSA_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_ALSA_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_ALSA_DEVOUT(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_ALSA_DEVIN(soundcard)){
    attack = AGS_ALSA_DEVIN(soundcard)->attack;
    delay = AGS_ALSA_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_ALSA_DEVIN(soundcard)->tic_counter);

    note_256th_offset = &(AGS_ALSA_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_ALSA_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_ALSA_DEVIN(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_OSS_DEVOUT(soundcard)){
    attack = AGS_OSS_DEVOUT(soundcard)->attack;
    delay = AGS_OSS_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_OSS_DEVOUT(soundcard)->tic_counter);

    note_256th_offset = &(AGS_OSS_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_OSS_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_OSS_DEVOUT(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_OSS_DEVIN(soundcard)){
    attack = AGS_OSS_DEVIN(soundcard)->attack;
    delay = AGS_OSS_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_OSS_DEVIN(soundcard)->tic_counter);

    note_256th_offset = &(AGS_OSS_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_OSS_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_OSS_DEVIN(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    attack = AGS_PULSE_DEVOUT(soundcard)->attack;
    delay = AGS_PULSE_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_PULSE_DEVOUT(soundcard)->tic_counter);

    note_256th_offset = &(AGS_PULSE_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_PULSE_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_PULSE_DEVOUT(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    attack = AGS_PULSE_DEVIN(soundcard)->attack;
    delay = AGS_PULSE_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_PULSE_DEVIN(soundcard)->tic_counter);

    note_256th_offset = &(AGS_PULSE_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_PULSE_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_PULSE_DEVIN(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    attack = AGS_JACK_DEVOUT(soundcard)->attack;
    delay = AGS_JACK_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_JACK_DEVOUT(soundcard)->tic_counter);

    note_256th_offset = &(AGS_JACK_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_JACK_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_JACK_DEVOUT(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    attack = AGS_JACK_DEVIN(soundcard)->attack;
    delay = AGS_JACK_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_JACK_DEVIN(soundcard)->tic_counter);
    
    note_256th_offset = &(AGS_JACK_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_JACK_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_JACK_DEVIN(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    attack = AGS_CORE_AUDIO_DEVOUT(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->tic_counter);
    
    note_256th_offset = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    attack = AGS_CORE_AUDIO_DEVIN(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_CORE_AUDIO_DEVIN(soundcard)->tic_counter);
    
    note_256th_offset = &(AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    attack = AGS_WASAPI_DEVOUT(soundcard)->attack;
    delay = AGS_WASAPI_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_WASAPI_DEVOUT(soundcard)->tic_counter);

    note_256th_offset = &(AGS_WASAPI_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_WASAPI_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_WASAPI_DEVOUT(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    attack = AGS_WASAPI_DEVIN(soundcard)->attack;
    delay = AGS_WASAPI_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_WASAPI_DEVIN(soundcard)->tic_counter);

    note_256th_offset = &(AGS_WASAPI_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_WASAPI_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_WASAPI_DEVIN(soundcard)->note_256th_tic_size);
#if defined(AGS_WITH_GSTREAMER)
  }else if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    attack = AGS_GSTREAMER_DEVOUT(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_GSTREAMER_DEVOUT(soundcard)->tic_counter);

    note_256th_offset = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_tic_size);
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    attack = AGS_GSTREAMER_DEVIN(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_GSTREAMER_DEVIN(soundcard)->tic_counter);

    note_256th_offset = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_tic_size = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_tic_size);
#endif
  }else{
    g_warning("unknown soundcard implementation");

    g_rec_mutex_unlock(obj_mutex);

    return;
  }

  default_tact_frames = absolute_delay * buffer_size;

  total_correct_frame_count = ((guint) AGS_SOUNDCARD_DEFAULT_PERIOD * default_tact_frames) % buffer_size;

  correct_frame_count = (gdouble) total_correct_frame_count / AGS_SOUNDCARD_DEFAULT_PERIOD;

  corrected_delay = absolute_delay + (correct_frame_count / buffer_size);
  
  attack[0] = 0;
  delay[0] = corrected_delay;
  
  for(i = 1; i < AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    gint64 current_attack;

    current_attack = (default_tact_frames - buffer_size + attack[i - 1]) - (floor(corrected_delay) * buffer_size);

    if(current_attack >= buffer_size){
      attack[i] = buffer_size - 1;
    }else if(current_attack < 0){
      attack[i] = 0;
    }else{
      attack[i] = current_attack;
    }
    
    delay[i] = corrected_delay;
  }
  
  for(; i < 2 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    gint64 current_attack;

    current_attack = attack[2 * (guint) AGS_SOUNDCARD_DEFAULT_PERIOD - i - 1];

    if(current_attack >= buffer_size){
      attack[i] = buffer_size - 1;
    }else if(current_attack < 0){
      attack[i] = 0;
    }else{
      attack[i] = current_attack;
    }

    delay[i] = corrected_delay;
  }

  note_256th_tic_size[0] = 1.0 / (delay[0] / 16.0);

  if(note_256th_tic_size[0] <= 1.0){
    note_256th_offset_last[0] = note_256th_offset[0] + 1;
  }else{
    note_256th_offset_last[0] = note_256th_offset[0] + (guint) fmod(tic_counter[0] * note_256th_tic_size[0], note_256th_tic_size[0]);
  }

  g_rec_mutex_unlock(obj_mutex);  
}

/**
 * ags_soundcard_util_calc_system_time:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Offset of @soundcard to system time.
 * 
 * Returns: the system time
 * 
 * Since: 5.0.0
 */
gint64
ags_soundcard_util_calc_system_time(GObject *soundcard)
{
  gint64 system_time;
  guint samplerate;
  guint buffer_size;
  gdouble delay, delay_counter;
  guint note_offset;
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));

  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(soundcard));

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    &samplerate,
			    &buffer_size,
			    NULL);

  note_offset = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(soundcard));

  system_time = (note_offset * delay + delay_counter) * buffer_size * (AGS_NSEC_PER_SEC / samplerate);

  return(system_time);
}

/**
 * ags_soundcard_util_calc_time_samples:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Offset of @soundcard to system time.
 * 
 * Returns: the system time
 * 
 * Since: 5.0.0
 */
gint64
ags_soundcard_util_calc_time_samples(GObject *soundcard)
{
  gint64 time_samples;
  guint buffer_size;
  gdouble delay, delay_counter;
  guint note_offset;
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));

  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(soundcard));

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(soundcard));

  time_samples = (note_offset * delay + delay_counter) * buffer_size;

  return(time_samples);
}

/**
 * ags_soundcard_util_calc_time_samples_absolute:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * 
 * Offset of @soundcard to system time.
 * 
 * Returns: the system time
 * 
 * Since: 5.0.0
 */
gint64
ags_soundcard_util_calc_time_samples_absolute(GObject *soundcard)
{
  gint64 time_samples;
  guint buffer_size;
  gdouble delay, delay_counter;
  guint note_offset;
  
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(soundcard));

  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(soundcard));

  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  note_offset = ags_soundcard_get_note_offset_absolute(AGS_SOUNDCARD(soundcard));

  time_samples = (note_offset * delay + delay_counter) * buffer_size;

  return(time_samples);
}
