/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_soundcard_util = 0;

    ags_type_soundcard_util =
      g_boxed_type_register_static("AgsSoundcardUtil",
				   (GBoxedCopyFunc) ags_soundcard_util_copy,
				   (GBoxedFreeFunc) ags_soundcard_util_free);

    g_once_init_leave(&g_define_type_id__static, ags_type_soundcard_util);
  }

  return(g_define_type_id__static);
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
  AgsFrameClock *frame_clock;
  
  gint64 system_time;
  guint samplerate;
  guint buffer_size;
  gdouble delay, delay_counter;
  guint64 note_offset;
  
  frame_clock = ags_soundcard_get_frame_clock(AGS_SOUNDCARD(soundcard));

  samplerate = ags_frame_clock_get_samplerate(frame_clock);
  
  buffer_size = ags_frame_clock_get_buffer_size(frame_clock);

  delay = (gdouble) frame_clock->absolute_delay;
  delay_counter = (gdouble) frame_clock->delay_counter;

  note_offset = ags_frame_clock_get_absolute_note_offset(AGS_SOUNDCARD(soundcard));

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
  AgsFrameClock *frame_clock;
  
  gint64 time_samples;
  guint buffer_size;
  gdouble delay, delay_counter;
  guint64 note_offset;
  
  frame_clock = ags_soundcard_get_frame_clock(AGS_SOUNDCARD(soundcard));

  buffer_size = ags_frame_clock_get_buffer_size(frame_clock);

  delay = (gdouble) frame_clock->absolute_delay;
  delay_counter = (gdouble) frame_clock->delay_counter;

  note_offset = ags_frame_clock_get_note_offset(AGS_SOUNDCARD(soundcard));

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
  AgsFrameClock *frame_clock;
  
  gint64 time_samples;
  guint buffer_size;
  gdouble delay, delay_counter;
  guint64 note_offset;

  frame_clock = ags_soundcard_get_frame_clock(AGS_SOUNDCARD(soundcard));

  buffer_size = ags_frame_clock_get_buffer_size(frame_clock);

  delay = (gdouble) frame_clock->absolute_delay;
  delay_counter = (gdouble) frame_clock->delay_counter;

  note_offset = ags_frame_clock_get_absolute_note_offset(AGS_SOUNDCARD(soundcard));
  
  time_samples = (note_offset * delay + delay_counter) * buffer_size;

  return(time_samples);
}
