/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/ags_peak_util.h>

#include <ags/audio/ags_audio_buffer_util.h>

/**
 * SECTION:ags_peak_util
 * @short_description: Boxed type of peak util
 * @title: AgsPeakUtil
 * @section_id:
 * @include: ags/audio/ags_peak_util.h
 *
 * Boxed type of peak util data type.
 */

GType
ags_peak_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_peak_util = 0;

    ags_type_peak_util =
      g_boxed_type_register_static("AgsPeakUtil",
				   (GBoxedCopyFunc) ags_peak_util_copy,
				   (GBoxedFreeFunc) ags_peak_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_peak_util);
  }

  return g_define_type_id__volatile;
}

/**
 * ags_peak_util_alloc:
 *
 * Allocate #AgsPeakUtil-struct
 *
 * Returns: a new #AgsPeakUtil-struct
 *
 * Since: 3.9.2
 */
AgsPeakUtil*
ags_peak_util_alloc()
{
  AgsPeakUtil *ptr;

  ptr = (AgsPeakUtil *) g_new(AgsPeakUtil,
				1);

  ptr->source = NULL;
  ptr->source_stride = 1;

  ptr->buffer_length = 0;
  ptr->format = AGS_PEAK_UTIL_DEFAULT_FORMAT;
  
  ptr->audio_buffer_util_format = AGS_PEAK_UTIL_DEFAULT_AUDIO_BUFFER_UTIL_FORMAT;

  ptr->samplerate = AGS_PEAK_UTIL_DEFAULT_SAMPLERATE;

  ptr->harmonic_rate = AGS_PEAK_UTIL_DEFAULT_HARMONIC_RATE;
  ptr->pressure_factor = AGS_PEAK_UTIL_DEFAULT_PRESSURE_FACTOR;

  ptr->peak = 0.0;

  return(ptr);
}

/**
 * ags_peak_util_copy:
 * @ptr: the original #AgsPeakUtil-struct
 *
 * Create a copy of @ptr.
 *
 * Returns: a pointer of the new #AgsPeakUtil-struct
 *
 * Since: 3.9.2
 */
gpointer
ags_peak_util_copy(AgsPeakUtil *ptr)
{
  AgsPeakUtil *new_ptr;
  
  new_ptr = (AgsPeakUtil *) g_new(AgsPeakUtil,
				  1);
  
  new_ptr->source = ptr->source;
  new_ptr->source_stride = ptr->source_stride;

  new_ptr->buffer_length = ptr->buffer_length;
  new_ptr->format = ptr->format;

  new_ptr->audio_buffer_util_format = ptr->audio_buffer_util_format;

  new_ptr->samplerate = ptr->samplerate;

  new_ptr->harmonic_rate = ptr->harmonic_rate;
  new_ptr->pressure_factor = ptr->pressure_factor;

  new_ptr->peak = ptr->peak;

  return(new_ptr);
}

/**
 * ags_peak_util_free:
 * @ptr: the #AgsPeakUtil-struct
 *
 * Free the memory of @ptr.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_free(AgsPeakUtil *ptr)
{
  g_free(ptr->source);

  g_free(ptr);
}

/**
 * ags_peak_util_get_source:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get source buffer of @peak_util.
 * 
 * Returns: the source buffer
 * 
 * Since: 3.9.2
 */
gpointer
ags_peak_util_get_source(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(NULL);
  }

  return(peak_util->source);
}

/**
 * ags_peak_util_set_source:
 * @peak_util: the #AgsPeakUtil-struct
 * @source: the source buffer
 *
 * Set @source buffer of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_source(AgsPeakUtil *peak_util,
			   gpointer source)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->source = source;
}

/**
 * ags_peak_util_get_source_stride:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get source stride of @peak_util.
 * 
 * Returns: the source buffer stride
 * 
 * Since: 3.9.2
 */
guint
ags_peak_util_get_source_stride(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(0);
  }

  return(peak_util->source_stride);
}

/**
 * ags_peak_util_set_source_stride:
 * @peak_util: the #AgsPeakUtil-struct
 * @source_stride: the source buffer stride
 *
 * Set @source stride of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_source_stride(AgsPeakUtil *peak_util,
				  guint source_stride)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->source_stride = source_stride;
}

/**
 * ags_peak_util_get_buffer_length:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get buffer length of @peak_util.
 * 
 * Returns: the buffer length
 * 
 * Since: 3.9.2
 */
guint
ags_peak_util_get_buffer_length(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(0);
  }

  return(peak_util->buffer_length);
}

/**
 * ags_peak_util_set_buffer_length:
 * @peak_util: the #AgsPeakUtil-struct
 * @buffer_length: the buffer length
 *
 * Set @buffer_length of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_buffer_length(AgsPeakUtil *peak_util,
				  guint buffer_length)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->buffer_length = buffer_length;
}

/**
 * ags_peak_util_get_format:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get format of @peak_util.
 * 
 * Returns: the format
 * 
 * Since: 3.9.6
 */
guint
ags_peak_util_get_format(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(0);
  }

  return(peak_util->format);
}

/**
 * ags_peak_util_set_format:
 * @peak_util: the #AgsPeakUtil-struct
 * @format: the format
 *
 * Set @format of @peak_util.
 *
 * Since: 3.9.6
 */
void
ags_peak_util_set_format(AgsPeakUtil *peak_util,
			 guint format)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->format = format;

  peak_util->audio_buffer_util_format = ags_audio_buffer_util_format_from_soundcard(format);
}

/**
 * ags_peak_util_get_audio_buffer_util_format:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get audio buffer util format of @peak_util.
 * 
 * Returns: the audio buffer util format
 * 
 * Since: 3.9.2
 */
guint
ags_peak_util_get_audio_buffer_util_format(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(0);
  }

  return(peak_util->audio_buffer_util_format);
}

/**
 * ags_peak_util_set_audio_buffer_util_format:
 * @peak_util: the #AgsPeakUtil-struct
 * @audio_buffer_util_format: the audio buffer util format
 *
 * Set @audio_buffer_util_format of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_audio_buffer_util_format(AgsPeakUtil *peak_util,
					     guint audio_buffer_util_format)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->audio_buffer_util_format = audio_buffer_util_format;
}

/**
 * ags_peak_util_get_samplerate:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get samplerate of @peak_util.
 * 
 * Returns: the samplerate
 * 
 * Since: 3.9.2
 */
guint
ags_peak_util_get_samplerate(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(AGS_PEAK_UTIL_DEFAULT_SAMPLERATE);
  }

  return(peak_util->samplerate);
}

/**
 * ags_peak_util_set_samplerate:
 * @peak_util: the #AgsPeakUtil-struct
 * @samplerate: the samplerate
 *
 * Set @samplerate of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_samplerate(AgsPeakUtil *peak_util,
			     guint samplerate)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->samplerate = samplerate;
}

/**
 * ags_peak_util_get_harmonic_rate:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get harmonic rate of @peak_util.
 * 
 * Returns: the harmonic rate
 * 
 * Since: 3.9.2
 */
gdouble
ags_peak_util_get_harmonic_rate(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(AGS_PEAK_UTIL_DEFAULT_HARMONIC_RATE);
  }

  return(peak_util->harmonic_rate);
}

/**
 * ags_peak_util_set_harmonic_rate:
 * @peak_util: the #AgsPeakUtil-struct
 * @harmonic_rate: the harmonic rate
 *
 * Set @harmonic_rate of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_harmonic_rate(AgsPeakUtil *peak_util,
				gdouble harmonic_rate)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->harmonic_rate = harmonic_rate;
}

/**
 * ags_peak_util_get_pressure_factor:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get pressure factor of @peak_util.
 * 
 * Returns: the pressure factor
 * 
 * Since: 3.9.2
 */
gdouble
ags_peak_util_get_pressure_factor(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(AGS_PEAK_UTIL_DEFAULT_PRESSURE_FACTOR);
  }

  return(peak_util->pressure_factor);
}

/**
 * ags_peak_util_set_pressure_factor:
 * @peak_util: the #AgsPeakUtil-struct
 * @pressure_factor: the pressure factor
 *
 * Set @pressure_factor of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_pressure_factor(AgsPeakUtil *peak_util,
				gdouble pressure_factor)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->pressure_factor = pressure_factor;
}

/**
 * ags_peak_util_get_peak:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Get peak of @peak_util.
 * 
 * Returns: the peak
 * 
 * Since: 3.9.2
 */
gdouble
ags_peak_util_get_peak(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL){
    return(0.0);
  }

  return(peak_util->peak);
}

/**
 * ags_peak_util_set_peak:
 * @peak_util: the #AgsPeakUtil-struct
 * @peak: the peak
 *
 * Set @peak of @peak_util.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_set_peak(AgsPeakUtil *peak_util,
			   gdouble peak)
{
  if(peak_util == NULL){
    return;
  }

  peak_util->peak = peak;
}

/**
 * ags_peak_util_compute_s8:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of signed 8 bit data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_s8(AgsPeakUtil *peak_util)
{
  gint8 *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gint8 *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) G_MAXUINT8) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT8 * peak_util->pressure_factor) * source[0]));
    }

    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_s16:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of signed 16 bit data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_s16(AgsPeakUtil *peak_util)
{
  gint16 *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gint16 *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) G_MAXUINT16) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT16 * peak_util->pressure_factor) * source[0]));
    }

    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_s24:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of signed 24 bit data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_s24(AgsPeakUtil *peak_util)
{
  gint32 *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gint32 *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) 0xffffffff) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0xffffffff * peak_util->pressure_factor) * source[0]));
    }

    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_s32:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of signed 32 bit data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_s32(AgsPeakUtil *peak_util)
{
  gint32 *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gint32 *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) G_MAXUINT32) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT32 * peak_util->pressure_factor) * source[0]));
    }

    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_s64:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of signed 64 bit data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_s64(AgsPeakUtil *peak_util)
{
  gint64 *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gint64 *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) G_MAXUINT64) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) G_MAXUINT64 * peak_util->pressure_factor) * source[0]));
    }

    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_float:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of floating point data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_float(AgsPeakUtil *peak_util)
{
  gfloat *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gfloat *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) 0.5) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * source[0]));
    }

    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_double:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of double floating point data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_double(AgsPeakUtil *peak_util)
{
  gdouble *source;

  gdouble current_peak;
  guint i, i_stop;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  source = (gdouble *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
#if defined(AGS_VECTORIZED_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    ags_v8double v_buffer;
    ags_v8double v_zero;

    v_buffer = (ags_v8double) {
      (gdouble) *(source),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride),
      (gdouble) *(source += peak_util->source_stride)
    };

    source += peak_util->source_stride;

    v_zero = (ags_v8double) {
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0,
      0.0
    };
    
    v_buffer = (1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * v_buffer;

    v_zero = v_buffer == v_zero;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 1.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 1.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 1.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 1.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 1.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 1.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 1.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 1.0;
    }
      
    v_buffer = 1.0 / v_buffer;

    if(v_zero[0] != 0.0){
      v_buffer[0] = 0.0;
    }

    if(v_zero[1] != 0.0){
      v_buffer[1] = 0.0;
    }

    if(v_zero[2] != 0.0){
      v_buffer[2] = 0.0;
    }

    if(v_zero[3] != 0.0){
      v_buffer[3] = 0.0;
    }

    if(v_zero[4] != 0.0){
      v_buffer[4] = 0.0;
    }

    if(v_zero[5] != 0.0){
      v_buffer[5] = 0.0;
    }

    if(v_zero[6] != 0.0){
      v_buffer[6] = 0.0;
    }

    if(v_zero[7] != 0.0){
      v_buffer[7] = 0.0;
    }
      
    current_peak += v_buffer[0] + v_buffer[1] + v_buffer[2] + v_buffer[3] + v_buffer[4] + v_buffer[5] + v_buffer[6] + v_buffer[7];

    i += 8;
  }
#elif defined(AGS_OSX_ACCELERATE_BUILTIN_FUNCTIONS)
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){
    double ret_v_buffer[8];

    double v_buffer[] = {
      (double) *(source),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride),
      (double) *(source += peak_util->source_stride)};

    source += peak_util->source_stride;

    double v_result[8];
    double v_factor[1];
    
    gboolean v_zero[] = {
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE,
      FALSE
    };
    
    static const double v_one[] = { 1.0 };
    
    v_factor[0] = 1.0 / ((double) 0.5) * peak_util->pressure_factor;

    vDSP_vmulD(v_buffer, 1, v_factor, 0, ret_v_buffer, 1, 8);

    if(ret_v_buffer[0] == 0.0){
      ret_v_buffer[0] = 1.0;

      v_zero[0] = TRUE;
    }

    if(ret_v_buffer[1] == 0.0){
      ret_v_buffer[1] = 1.0;
	
      v_zero[1] = TRUE;
    }

    if(ret_v_buffer[2] == 0.0){
      ret_v_buffer[2] = 1.0;
	
      v_zero[2] = TRUE;
    }

    if(ret_v_buffer[3] == 0.0){
      ret_v_buffer[3] = 1.0;
	
      v_zero[3] = TRUE;
    }

    if(ret_v_buffer[4] == 0.0){
      ret_v_buffer[4] = 1.0;
	
      v_zero[4] = TRUE;
    }

    if(ret_v_buffer[5] == 0.0){
      ret_v_buffer[5] = 1.0;
	
      v_zero[5] = TRUE;
    }

    if(ret_v_buffer[6] == 0.0){
      ret_v_buffer[6] = 1.0;
	
      v_zero[6] = TRUE;
    }

    if(ret_v_buffer[7] == 0.0){
      ret_v_buffer[7] = 1.0;
	
      v_zero[7] = TRUE;
    }

    vDSP_vdiv(v_one, 0, ret_v_buffer, 1, v_result, 1, 8);

    if(v_zero[0]){
      v_result[0] = 0.0;
    }

    if(v_zero[1]){
      v_result[1] = 0.0;
    }

    if(v_zero[2]){
      v_result[2] = 0.0;
    }

    if(v_zero[3]){
      v_result[3] = 0.0;
    }

    if(v_zero[4]){
      v_result[4] = 0.0;
    }

    if(v_zero[5]){
      v_result[5] = 0.0;
    }

    if(v_zero[6]){
      v_result[6] = 0.0;
    }

    if(v_zero[7]){
      v_result[7] = 0.0;
    }
      
    current_peak += v_result[0] + v_result[1] + v_result[2] + v_result[3] + v_result[4] + v_result[5] + v_result[6] + v_result[7];
    
    i += 8;
  }
#else
  i_stop = peak_util->buffer_length - (peak_util->buffer_length % 8);

  for(; i < i_stop;){      
    if(buffer[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[0]));
    }

    if(buffer[(current_channel = channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }
    
    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    if(buffer[(current_channel += channels)] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * buffer[current_channel]));
    }

    i += 8;
  }
#endif

  /* loop tail */
  for(; i < peak_util->buffer_length;){
    if(source[0] != 0){
      current_peak += (1.0 / ((1.0 / (gdouble) 0.5 * peak_util->pressure_factor) * source[0]));
    }
    
    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute_complex:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak of complex floating point data.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute_complex(AgsPeakUtil *peak_util)
{
  AgsComplex *source;
  
  gdouble current_peak;
  guint i;

  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }
  
  source = (AgsComplex *) peak_util->source;
  
  current_peak = 0.0;
  
  i = 0;
  
  for(; i < peak_util->buffer_length;){
    double _Complex z;

    z = ags_complex_get(source);

    if(source[0].real != 0.0 ||
       source[0].imag != 0.0){
      current_peak += (1.0 / ((0.5 * peak_util->pressure_factor) * (z / M_PI)));
    }
    
    source += peak_util->source_stride;
    i++;
  }

  if(current_peak != 0.0){
    current_peak = (atan(1.0 / peak_util->harmonic_rate) / sin(current_peak / ((gdouble) peak_util->samplerate / 2.0)));
  }

  peak_util->peak = current_peak;
}

/**
 * ags_peak_util_compute:
 * @peak_util: the #AgsPeakUtil-struct
 * 
 * Compute peak.
 *
 * Since: 3.9.2
 */
void
ags_peak_util_compute(AgsPeakUtil *peak_util)
{
  if(peak_util == NULL ||
     peak_util->source == NULL){
    return;
  }

  switch(peak_util->audio_buffer_util_format){
  case AGS_AUDIO_BUFFER_UTIL_S8:
  {
    ags_peak_util_compute_s8(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S16:
  {
    ags_peak_util_compute_s16(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S24:
  {
    ags_peak_util_compute_s24(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S32:
  {
    ags_peak_util_compute_s32(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_S64:
  {
    ags_peak_util_compute_s64(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_FLOAT:
  {
    ags_peak_util_compute_float(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_DOUBLE:
  {
    ags_peak_util_compute_double(peak_util);
  }
  break;
  case AGS_AUDIO_BUFFER_UTIL_COMPLEX:
  {
    ags_peak_util_compute_complex(peak_util);
  }
  break;
  }
}
