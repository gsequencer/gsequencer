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

  guint *note_256th_attack;

  guint *tic_counter;

  guint *note_256th_offset;
  guint *note_256th_offset_last;
  gdouble *note_256th_delay;

  gdouble bpm;
  gdouble absolute_delay;
  gdouble corrected_delay;
  gdouble delay_overflow_counter;
  guint buffer_size;
  guint default_tact_frames;
  guint total_correct_frame_count;
  gdouble correct_note_4th_frame_count;
  gdouble correct_frame_count;
  gdouble correct_note_256th_frame_count;
  guint frame_counter;
  gint frame_counter_delay_remainder;
  guint nth_list;
  guint current_attack;
  guint current_note_256th_attack;
  guint current_note_256th_counter;
  gboolean decrement_delay;
  guint i, j;
  
  GList *start_note_256th_attack;

  GRecMutex *obj_mutex;

  if(soundcard == NULL ||
     !AGS_IS_SOUNDCARD(soundcard)){
    return;
  }
  
  obj_mutex = ags_soundcard_util_get_obj_mutex(soundcard);
  
  /* get some initial values */
  bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(soundcard));
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(soundcard));

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);

  attack = NULL;
  delay = NULL;

  tic_counter = NULL;

  start_note_256th_attack = NULL;

  note_256th_offset = NULL;
  note_256th_offset_last = NULL;
  note_256th_delay = NULL;
  
  g_rec_mutex_lock(obj_mutex);
  
  if(AGS_IS_ALSA_DEVOUT(soundcard)){
    attack = AGS_ALSA_DEVOUT(soundcard)->attack;
    delay = AGS_ALSA_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_ALSA_DEVOUT(soundcard)->tic_counter);

    start_note_256th_attack = AGS_ALSA_DEVOUT(soundcard)->note_256th_attack;
    
    note_256th_offset = &(AGS_ALSA_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_ALSA_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_ALSA_DEVOUT(soundcard)->note_256th_delay);
  }else if(AGS_IS_ALSA_DEVIN(soundcard)){
    attack = AGS_ALSA_DEVIN(soundcard)->attack;
    delay = AGS_ALSA_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_ALSA_DEVIN(soundcard)->tic_counter);

    start_note_256th_attack = AGS_ALSA_DEVIN(soundcard)->note_256th_attack;
    
    note_256th_offset = &(AGS_ALSA_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_ALSA_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_ALSA_DEVIN(soundcard)->note_256th_delay);
  }else if(AGS_IS_OSS_DEVOUT(soundcard)){
    attack = AGS_OSS_DEVOUT(soundcard)->attack;
    delay = AGS_OSS_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_OSS_DEVOUT(soundcard)->tic_counter);

    start_note_256th_attack = AGS_OSS_DEVOUT(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_OSS_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_OSS_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_OSS_DEVOUT(soundcard)->note_256th_delay);
  }else if(AGS_IS_OSS_DEVIN(soundcard)){
    attack = AGS_OSS_DEVIN(soundcard)->attack;
    delay = AGS_OSS_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_OSS_DEVIN(soundcard)->tic_counter);

    start_note_256th_attack = AGS_OSS_DEVIN(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_OSS_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_OSS_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_OSS_DEVIN(soundcard)->note_256th_delay);
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    attack = AGS_PULSE_DEVOUT(soundcard)->attack;
    delay = AGS_PULSE_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_PULSE_DEVOUT(soundcard)->tic_counter);

    start_note_256th_attack = AGS_PULSE_DEVOUT(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_PULSE_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_PULSE_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_PULSE_DEVOUT(soundcard)->note_256th_delay);
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    attack = AGS_PULSE_DEVIN(soundcard)->attack;
    delay = AGS_PULSE_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_PULSE_DEVIN(soundcard)->tic_counter);

    start_note_256th_attack = AGS_PULSE_DEVIN(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_PULSE_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_PULSE_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_PULSE_DEVIN(soundcard)->note_256th_delay);
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    attack = AGS_JACK_DEVOUT(soundcard)->attack;
    delay = AGS_JACK_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_JACK_DEVOUT(soundcard)->tic_counter);

    start_note_256th_attack = AGS_JACK_DEVOUT(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_JACK_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_JACK_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_JACK_DEVOUT(soundcard)->note_256th_delay);
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    attack = AGS_JACK_DEVIN(soundcard)->attack;
    delay = AGS_JACK_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_JACK_DEVIN(soundcard)->tic_counter);
    
    start_note_256th_attack = AGS_JACK_DEVIN(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_JACK_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_JACK_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_JACK_DEVIN(soundcard)->note_256th_delay);
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    attack = AGS_CORE_AUDIO_DEVOUT(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->tic_counter);
    
    start_note_256th_attack = AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_delay);
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    attack = AGS_CORE_AUDIO_DEVIN(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_CORE_AUDIO_DEVIN(soundcard)->tic_counter);
    
    start_note_256th_attack = AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_attack;
    
    note_256th_offset = &(AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_delay);
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    attack = AGS_WASAPI_DEVOUT(soundcard)->attack;
    delay = AGS_WASAPI_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_WASAPI_DEVOUT(soundcard)->tic_counter);

    start_note_256th_attack = AGS_WASAPI_DEVOUT(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_WASAPI_DEVOUT(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_WASAPI_DEVOUT(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_WASAPI_DEVOUT(soundcard)->note_256th_delay);
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    attack = AGS_WASAPI_DEVIN(soundcard)->attack;
    delay = AGS_WASAPI_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_WASAPI_DEVIN(soundcard)->tic_counter);

    start_note_256th_attack = AGS_WASAPI_DEVIN(soundcard)->note_256th_attack;
    
    note_256th_offset = &(AGS_WASAPI_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_WASAPI_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_WASAPI_DEVIN(soundcard)->note_256th_delay);
#if defined(AGS_WITH_GSTREAMER)
  }else if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    attack = AGS_GSTREAMER_DEVOUT(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVOUT(soundcard)->delay;

    tic_counter = &(AGS_GSTREAMER_DEVOUT(soundcard)->tic_counter);

    start_note_256th_attack = AGS_GSTREAMER_DEVOUT(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_delay);
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    attack = AGS_GSTREAMER_DEVIN(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVIN(soundcard)->delay;

    tic_counter = &(AGS_GSTREAMER_DEVIN(soundcard)->tic_counter);

    start_note_256th_attack = AGS_GSTREAMER_DEVIN(soundcard)->note_256th_attack;

    note_256th_offset = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset);
    note_256th_offset_last = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_offset_last);
    note_256th_delay = &(AGS_GSTREAMER_DEVIN(soundcard)->note_256th_delay);
#endif
  }else{
    g_warning("unknown soundcard implementation");

    g_rec_mutex_unlock(obj_mutex);

    return;
  }

  default_tact_frames = (guint) floor(absolute_delay * (gdouble) buffer_size);

  total_correct_frame_count = buffer_size * ((guint) floor((gdouble) ((guint) AGS_SOUNDCARD_DEFAULT_PERIOD * default_tact_frames) / (gdouble) buffer_size));

  correct_note_4th_frame_count = floor((gdouble) total_correct_frame_count / (AGS_SOUNDCARD_DEFAULT_PERIOD / 4.0));
  
  correct_frame_count = (gdouble) correct_note_4th_frame_count / 4.0;

  correct_note_256th_frame_count = correct_frame_count / 16.0;
  
  corrected_delay = correct_frame_count / (gdouble) buffer_size;
  
  attack[0] = 0;
  delay[0] = floor(corrected_delay);

  //  fprintf(stdout, "-> delay{#0} = %f, attack{#0} = %d\n", delay[0], attack[0]);

  frame_counter = (guint) 0;
  frame_counter_delay_remainder = 0;

  nth_list = 0;
  note_256th_attack = g_list_nth_data(start_note_256th_attack,
				      nth_list);

  current_note_256th_attack = 0;
  current_note_256th_counter = 0;

  delay_overflow_counter = ((floor(corrected_delay) + 1.0) * (double) buffer_size) - (floor(corrected_delay) * (double) buffer_size);
  
  for(i = 0; i < AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    guint current_16th_attack;
    
    nth_list = floor(16.0 * (double) i / AGS_SOUNDCARD_DEFAULT_PERIOD);
    note_256th_attack = g_list_nth_data(start_note_256th_attack,
					nth_list);

    for(j = 0; j < 16; j++){
      //      fprintf(stdout, " `-> nth list = %d, i = {#%d}\n", nth_list, ((16 * i) + j) % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD);

      note_256th_attack[((16 * i) + j) % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD] = current_note_256th_attack;

      if(j == 0){
	current_16th_attack = current_note_256th_attack;
      }
      
      if(correct_note_256th_frame_count < buffer_size){
	current_note_256th_counter = (guint) floor((double) (frame_counter + buffer_size) / (double) correct_note_256th_frame_count);
	
	frame_counter += buffer_size;
      }else{
	current_note_256th_counter = (guint) floor((double) (frame_counter + (buffer_size * (floor((double) correct_note_256th_frame_count / (double) buffer_size) + 1.0))) / (double) correct_note_256th_frame_count);	

	frame_counter += buffer_size * (floor((double) correct_note_256th_frame_count / (double) buffer_size) + 1.0);
      }

      current_note_256th_attack = frame_counter % (guint) correct_note_256th_frame_count;
      
      //      fprintf(stdout, " `-> note_256th_attack{#%d} = %d\n", ((16 * i) + j) % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD, current_note_256th_attack);
    }

    delay_overflow_counter += (((floor(corrected_delay) + 1.0) * (double) buffer_size) - (corrected_delay * (double) buffer_size));

    decrement_delay = FALSE;

    if((gint) floor(delay_overflow_counter) >= buffer_size){
      decrement_delay = TRUE;

      delay_overflow_counter -= (double) buffer_size;
    }
    
    if(!decrement_delay){
      delay[i] = floor(corrected_delay);
    }else{
      delay[i] = floor(corrected_delay) - 1.0;
    }

    attack[i] = (guint) current_16th_attack;
  }
  
  for(; i < 2 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    guint current_attack;

    current_attack = (guint) attack[2 * (guint) AGS_SOUNDCARD_DEFAULT_PERIOD - i - 1];

    if(current_attack < 0){
      current_attack = -1 * current_attack;
    }
    
    if(current_attack >= buffer_size){
      attack[i] = buffer_size - 1;
    }else{
      attack[i] = (guint) current_attack;
    }

    delay[i] = corrected_delay;
  }
  
  note_256th_delay[0] = absolute_delay / 16.0;

  if(note_256th_delay[0] >= 1.0){
    note_256th_offset_last[0] = note_256th_offset[0];
  }else{
    note_256th_offset_last[0] = note_256th_offset[0] + (guint) floor(1.0 / note_256th_delay[0]);
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

/**
 * ags_soundcard_util_calc_next_note_256th_offset:
 * @soundcard: the #GObject sub-type implementing #AgsSoundcard
 * @note_256th_offset_lower: (out): the return location of note 256th offset lower
 * @note_256th_offset_upper: (out): the return location of note 256th offset upper
 * 
 * Calc next note 256th offset of @soundcard.
 * 
 * Since: 6.9.0
 */
void
ags_soundcard_util_calc_next_note_256th_offset(GObject *soundcard,
					       guint *note_256th_offset_lower,
					       guint *note_256th_offset_upper)
{
  gdouble *delay;
  guint *attack;

  gdouble note_256th_delay;
  gdouble delay_counter;
  guint tic_counter;

  guint note_offset;
  guint note_256th_offset;
  guint note_256th_offset_last;
  guint note_256th_attack;
  guint note_256th_attack_last;
  guint next_note_256th_offset_lower;
  guint next_note_256th_offset_upper;
  guint next_note_256th_attack;
  
  gdouble absolute_delay;
  guint buffer_size;
  guint loop_left, loop_right;
  gboolean do_loop;

  GRecMutex *obj_mutex;

  if(soundcard == NULL ||
     !AGS_IS_SOUNDCARD(soundcard)){
    return;
  }
  
  obj_mutex = ags_soundcard_util_get_obj_mutex(soundcard);

  /* get some initial values */
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(soundcard));

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);
  
  note_offset = ags_soundcard_get_note_offset(soundcard);

  note_256th_offset = 0;
  note_256th_offset_last = 0;
  
  ags_soundcard_get_note_256th_offset(soundcard,
				      &note_256th_offset,
				      &note_256th_offset_last);

  loop_left = 0;
  loop_right = 64;
  
  do_loop = FALSE;
  
  ags_soundcard_get_loop(soundcard,
			 &loop_left, &loop_right,
			 &do_loop);

  note_256th_attack = 0;
  note_256th_attack_last = 0;

  ags_soundcard_get_note_256th_attack(soundcard,
				      &note_256th_attack,
				      &note_256th_attack_last);
  
  attack = NULL;
  delay = NULL;

  tic_counter = 0;

  note_256th_delay = AGS_SOUNDCARD_DEFAULT_DELAY / 16.0;
  
  g_rec_mutex_lock(obj_mutex);
  
  if(AGS_IS_ALSA_DEVOUT(soundcard)){
    attack = AGS_ALSA_DEVOUT(soundcard)->attack;
    delay = AGS_ALSA_DEVOUT(soundcard)->delay;

    delay_counter = AGS_ALSA_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_ALSA_DEVOUT(soundcard)->tic_counter;
    
    note_256th_delay = AGS_ALSA_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_ALSA_DEVIN(soundcard)){
    attack = AGS_ALSA_DEVIN(soundcard)->attack;
    delay = AGS_ALSA_DEVIN(soundcard)->delay;

    delay_counter = AGS_ALSA_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_ALSA_DEVIN(soundcard)->tic_counter;
    
    note_256th_delay = AGS_ALSA_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_OSS_DEVOUT(soundcard)){
    attack = AGS_OSS_DEVOUT(soundcard)->attack;
    delay = AGS_OSS_DEVOUT(soundcard)->delay;

    delay_counter = AGS_OSS_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_OSS_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_OSS_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_OSS_DEVIN(soundcard)){
    attack = AGS_OSS_DEVIN(soundcard)->attack;
    delay = AGS_OSS_DEVIN(soundcard)->delay;

    delay_counter = AGS_OSS_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_OSS_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_OSS_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    attack = AGS_PULSE_DEVOUT(soundcard)->attack;
    delay = AGS_PULSE_DEVOUT(soundcard)->delay;

    delay_counter = AGS_PULSE_DEVOUT(soundcard)->delay_counter;
    
    tic_counter = AGS_PULSE_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_PULSE_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    attack = AGS_PULSE_DEVIN(soundcard)->attack;
    delay = AGS_PULSE_DEVIN(soundcard)->delay;

    delay_counter = AGS_PULSE_DEVIN(soundcard)->delay_counter;
    
    tic_counter = AGS_PULSE_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_PULSE_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    attack = AGS_JACK_DEVOUT(soundcard)->attack;
    delay = AGS_JACK_DEVOUT(soundcard)->delay;

    delay_counter = AGS_JACK_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_JACK_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_JACK_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    attack = AGS_JACK_DEVIN(soundcard)->attack;
    delay = AGS_JACK_DEVIN(soundcard)->delay;

    delay_counter = AGS_JACK_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_JACK_DEVIN(soundcard)->tic_counter;
    
    note_256th_delay = AGS_JACK_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    attack = AGS_CORE_AUDIO_DEVOUT(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay;

    delay_counter = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_CORE_AUDIO_DEVOUT(soundcard)->tic_counter;
    
    note_256th_delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    attack = AGS_CORE_AUDIO_DEVIN(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVIN(soundcard)->delay;

    delay_counter = AGS_CORE_AUDIO_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_CORE_AUDIO_DEVIN(soundcard)->tic_counter;
    
    note_256th_delay = AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    attack = AGS_WASAPI_DEVOUT(soundcard)->attack;
    delay = AGS_WASAPI_DEVOUT(soundcard)->delay;

    delay_counter = AGS_WASAPI_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_WASAPI_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_WASAPI_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    attack = AGS_WASAPI_DEVIN(soundcard)->attack;
    delay = AGS_WASAPI_DEVIN(soundcard)->delay;

    delay_counter = AGS_WASAPI_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_WASAPI_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_WASAPI_DEVIN(soundcard)->note_256th_delay;
#if defined(AGS_WITH_GSTREAMER)
  }else if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    attack = AGS_GSTREAMER_DEVOUT(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVOUT(soundcard)->delay;

    delay_counter = AGS_GSTREAMER_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_GSTREAMER_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_GSTREAMER_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    attack = AGS_GSTREAMER_DEVIN(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVIN(soundcard)->delay;

    delay_counter = AGS_GSTREAMER_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_GSTREAMER_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_GSTREAMER_DEVIN(soundcard)->note_256th_delay;
#endif
  }else{
    g_warning("unknown soundcard implementation");

    g_rec_mutex_unlock(obj_mutex);

    return;
  }
  
  //  fprintf(stdout, "-> delay{#0} = %f, attack{#0} = %d\n", delay[0], attack[0]);
  next_note_256th_attack = 0;
  
  if(note_256th_delay < 1.0){
    next_note_256th_attack = (note_256th_attack_last + (guint) floor(note_256th_delay * (double) buffer_size)) % buffer_size;
  }
  
  if(delay_counter >= delay[tic_counter]){
    if(do_loop &&
       note_offset + 1 == loop_right){      
      next_note_256th_offset_lower = 16 * loop_left;

      if(note_256th_attack < attack[tic_counter]){
	if(next_note_256th_offset_lower - (guint) floor((double) (attack[tic_counter] - note_256th_attack) / (note_256th_delay * (double) buffer_size)) > note_256th_offset_last){
	  next_note_256th_offset_lower -= (guint) floor((double) (attack[tic_counter] - note_256th_attack) / (note_256th_delay * (double) buffer_size));
	}
      }
      
      next_note_256th_offset_upper = next_note_256th_offset_lower;
      
      if(note_256th_delay < 1.0){
	next_note_256th_offset_upper += (guint) floor((double) (buffer_size - next_note_256th_attack) / (note_256th_delay * (double) buffer_size));
      }
    }else{
      next_note_256th_offset_lower = note_256th_offset_last + 1;
    
      next_note_256th_offset_upper = next_note_256th_offset_lower;
      
      if(note_256th_delay < 1.0){
	next_note_256th_offset_upper += (guint) floor((double) (buffer_size - next_note_256th_attack) / (note_256th_delay * (double) buffer_size));
      }
    }
  }else{
    next_note_256th_offset_lower = note_256th_offset_last + 1;
    
    next_note_256th_offset_upper = next_note_256th_offset_lower;
    
    if(note_256th_delay < 1.0){
      next_note_256th_offset_upper += (guint) floor((double) (buffer_size - next_note_256th_attack) / (note_256th_delay * (double) buffer_size));
    }
  }
  
  g_rec_mutex_unlock(obj_mutex);
  
  if(note_256th_offset_lower != NULL){
    note_256th_offset_lower[0] = next_note_256th_offset_lower;
  }
  
  if(note_256th_offset_upper != NULL){
    note_256th_offset_upper[0] = next_note_256th_offset_upper;
  }
}

void
ags_soundcard_util_calc_next_note_256th_attack(GObject *soundcard,
					       guint *note_256th_attack_lower,
					       guint *note_256th_attack_upper)
{
  gdouble *delay;
  guint *attack;

  gdouble note_256th_delay;
  gdouble delay_counter;
  guint tic_counter;

  guint note_offset;
  guint note_256th_offset;
  guint note_256th_offset_last;
  guint note_256th_attack;
  guint note_256th_attack_last;
  guint next_note_256th_attack;
  guint next_note_256th_offset_lower;
  guint next_note_256th_offset_upper;
  guint next_note_256th_attack_lower;
  guint next_note_256th_attack_upper;
  
  gdouble absolute_delay;
  guint buffer_size;
  guint loop_left, loop_right;
  gboolean do_loop;
  guint note_256th_attack_position_lower;
  guint note_256th_attack_position_upper;
  
  GRecMutex *obj_mutex;

  if(soundcard == NULL ||
     !AGS_IS_SOUNDCARD(soundcard)){
    return;
  }
  
  obj_mutex = ags_soundcard_util_get_obj_mutex(soundcard);

  /* get some initial values */
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(soundcard));

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(soundcard),
			    NULL,
			    NULL,
			    &buffer_size,
			    NULL);
  
  note_offset = ags_soundcard_get_note_offset(soundcard);

  note_256th_offset = 0;
  note_256th_offset_last = 0;
  
  ags_soundcard_get_note_256th_offset(soundcard,
				      &note_256th_offset,
				      &note_256th_offset_last);

  loop_left = 0;
  loop_right = 64;
  
  do_loop = FALSE;
  
  ags_soundcard_get_loop(soundcard,
			 &loop_left, &loop_right,
			 &do_loop);

  note_256th_attack = 0;
  note_256th_attack_last = 0;

  ags_soundcard_get_note_256th_attack(soundcard,
				      &note_256th_attack,
				      &note_256th_attack_last);
  
  ags_soundcard_get_note_256th_attack_position(soundcard,
					       &note_256th_attack_position_lower,
					       &note_256th_attack_position_upper);
  
  attack = NULL;
  delay = NULL;

  tic_counter = 0;

  note_256th_delay = AGS_SOUNDCARD_DEFAULT_DELAY / 16.0;
  
  g_rec_mutex_lock(obj_mutex);
  
  if(AGS_IS_ALSA_DEVOUT(soundcard)){
    attack = AGS_ALSA_DEVOUT(soundcard)->attack;
    delay = AGS_ALSA_DEVOUT(soundcard)->delay;

    delay_counter = AGS_ALSA_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_ALSA_DEVOUT(soundcard)->tic_counter;
    
    note_256th_delay = AGS_ALSA_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_ALSA_DEVIN(soundcard)){
    attack = AGS_ALSA_DEVIN(soundcard)->attack;
    delay = AGS_ALSA_DEVIN(soundcard)->delay;

    delay_counter = AGS_ALSA_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_ALSA_DEVIN(soundcard)->tic_counter;
    
    note_256th_delay = AGS_ALSA_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_OSS_DEVOUT(soundcard)){
    attack = AGS_OSS_DEVOUT(soundcard)->attack;
    delay = AGS_OSS_DEVOUT(soundcard)->delay;

    delay_counter = AGS_OSS_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_OSS_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_OSS_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_OSS_DEVIN(soundcard)){
    attack = AGS_OSS_DEVIN(soundcard)->attack;
    delay = AGS_OSS_DEVIN(soundcard)->delay;

    delay_counter = AGS_OSS_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_OSS_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_OSS_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_PULSE_DEVOUT(soundcard)){
    attack = AGS_PULSE_DEVOUT(soundcard)->attack;
    delay = AGS_PULSE_DEVOUT(soundcard)->delay;

    delay_counter = AGS_PULSE_DEVOUT(soundcard)->delay_counter;
    
    tic_counter = AGS_PULSE_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_PULSE_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    attack = AGS_PULSE_DEVIN(soundcard)->attack;
    delay = AGS_PULSE_DEVIN(soundcard)->delay;

    delay_counter = AGS_PULSE_DEVIN(soundcard)->delay_counter;
    
    tic_counter = AGS_PULSE_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_PULSE_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_JACK_DEVOUT(soundcard)){
    attack = AGS_JACK_DEVOUT(soundcard)->attack;
    delay = AGS_JACK_DEVOUT(soundcard)->delay;

    delay_counter = AGS_JACK_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_JACK_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_JACK_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    attack = AGS_JACK_DEVIN(soundcard)->attack;
    delay = AGS_JACK_DEVIN(soundcard)->delay;

    delay_counter = AGS_JACK_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_JACK_DEVIN(soundcard)->tic_counter;
    
    note_256th_delay = AGS_JACK_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    attack = AGS_CORE_AUDIO_DEVOUT(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay;

    delay_counter = AGS_CORE_AUDIO_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_CORE_AUDIO_DEVOUT(soundcard)->tic_counter;
    
    note_256th_delay = AGS_CORE_AUDIO_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    attack = AGS_CORE_AUDIO_DEVIN(soundcard)->attack;
    delay = AGS_CORE_AUDIO_DEVIN(soundcard)->delay;

    delay_counter = AGS_CORE_AUDIO_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_CORE_AUDIO_DEVIN(soundcard)->tic_counter;
    
    note_256th_delay = AGS_CORE_AUDIO_DEVIN(soundcard)->note_256th_delay;
  }else if(AGS_IS_WASAPI_DEVOUT(soundcard)){
    attack = AGS_WASAPI_DEVOUT(soundcard)->attack;
    delay = AGS_WASAPI_DEVOUT(soundcard)->delay;

    delay_counter = AGS_WASAPI_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_WASAPI_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_WASAPI_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_WASAPI_DEVIN(soundcard)){
    attack = AGS_WASAPI_DEVIN(soundcard)->attack;
    delay = AGS_WASAPI_DEVIN(soundcard)->delay;

    delay_counter = AGS_WASAPI_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_WASAPI_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_WASAPI_DEVIN(soundcard)->note_256th_delay;
#if defined(AGS_WITH_GSTREAMER)
  }else if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    attack = AGS_GSTREAMER_DEVOUT(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVOUT(soundcard)->delay;

    delay_counter = AGS_GSTREAMER_DEVOUT(soundcard)->delay_counter;

    tic_counter = AGS_GSTREAMER_DEVOUT(soundcard)->tic_counter;

    note_256th_delay = AGS_GSTREAMER_DEVOUT(soundcard)->note_256th_delay;
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    attack = AGS_GSTREAMER_DEVIN(soundcard)->attack;
    delay = AGS_GSTREAMER_DEVIN(soundcard)->delay;

    delay_counter = AGS_GSTREAMER_DEVIN(soundcard)->delay_counter;

    tic_counter = AGS_GSTREAMER_DEVIN(soundcard)->tic_counter;

    note_256th_delay = AGS_GSTREAMER_DEVIN(soundcard)->note_256th_delay;
#endif
  }else{
    g_warning("unknown soundcard implementation");

    g_rec_mutex_unlock(obj_mutex);

    return;
  }
  
  //  fprintf(stdout, "-> delay{#0} = %f, attack{#0} = %d\n", delay[0], attack[0]);

  next_note_256th_attack = 0;
  
  if(note_256th_delay < 1.0){
    next_note_256th_attack = (note_256th_attack_last + (guint) floor(note_256th_delay * (double) buffer_size)) % buffer_size;
  }
  
  if(delay_counter >= delay[tic_counter]){
    if(do_loop &&
       note_offset + 1 == loop_right){      
      next_note_256th_offset_lower = 16 * loop_left;

      if(note_256th_attack < attack[tic_counter]){
	if(next_note_256th_offset_lower - (guint) floor((double) (attack[tic_counter] - note_256th_attack) / (note_256th_delay * (double) buffer_size)) > note_256th_offset_last){
	  next_note_256th_offset_lower -= (guint) floor((double) (attack[tic_counter] - note_256th_attack) / (note_256th_delay * (double) buffer_size));
	}
      }
      
      next_note_256th_offset_upper = next_note_256th_offset_lower;
      
      if(note_256th_delay < 1.0){
	next_note_256th_offset_upper += (guint) floor((double) (buffer_size - next_note_256th_attack) / (note_256th_delay * (double) buffer_size));
      }
    }else{
      next_note_256th_offset_lower = note_256th_offset_last + 1;
    
      next_note_256th_offset_upper = next_note_256th_offset_lower;
      
      if(note_256th_delay < 1.0){
	next_note_256th_offset_upper += (guint) floor((double) (buffer_size - next_note_256th_attack) / (note_256th_delay * (double) buffer_size));
      }
    }
  }else{
    next_note_256th_offset_lower = note_256th_offset_last + 1;
    
    next_note_256th_offset_upper = next_note_256th_offset_lower;
    
    if(note_256th_delay < 1.0){
      next_note_256th_offset_upper += (guint) floor((double) (buffer_size - next_note_256th_attack) / (note_256th_delay * (double) buffer_size));
    }
  }

  next_note_256th_attack_lower = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										 note_256th_attack_position_lower);
  next_note_256th_attack_upper = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										 note_256th_attack_position_upper);

  if(next_note_256th_offset_lower != note_256th_offset){
    if(do_loop){
      next_note_256th_attack_lower = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										     0);
      next_note_256th_attack_upper = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										     (next_note_256th_offset_upper - next_note_256th_offset_lower));
    }else{
      next_note_256th_attack_lower = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										     note_256th_attack_position_upper + 1);
      next_note_256th_attack_upper = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										     note_256th_attack_position_upper + 1 + (next_note_256th_offset_upper - next_note_256th_offset_lower));
    }
  }
  
  g_rec_mutex_unlock(obj_mutex);
    
  if(note_256th_attack != NULL){
    note_256th_attack_lower[0] = next_note_256th_attack_lower;
  }
  
  if(note_256th_attack_upper != NULL){
    note_256th_attack_upper[0] = next_note_256th_attack_upper;
  }
}
