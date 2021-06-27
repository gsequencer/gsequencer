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

#include <ags/audio/ags_hq_pitch_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_linear_interpolate_util.h>

gpointer ags_hq_pitch_util_strct_copy(gpointer ptr);
void ags_hq_pitch_util_strct_free(gpointer ptr);

/**
 * SECTION:ags_hq_pitch_util
 * @short_description: hq pitch util
 * @title: AgsHQPitchUtil
 * @section_id:
 * @include: ags/audio/ags_hq_pitch_util.h
 *
 * Utility functions to pitch.
 */

GType
ags_hq_pitch_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_hq_pitch_util = 0;

    ags_type_hq_pitch_util =
      g_boxed_type_register_static("AgsHQPitchUtil",
				   (GBoxedCopyFunc) ags_hq_pitch_util_strct_copy,
				   (GBoxedFreeFunc) ags_hq_pitch_util_strct_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_hq_pitch_util);
  }

  return g_define_type_id__volatile;
}

gpointer
ags_hq_pitch_util_strct_copy(gpointer ptr)
{
  gpointer retval;

  retval = g_memdup(ptr, sizeof(AgsHQPitchUtil));
 
  return(retval);
}

void
ags_hq_pitch_util_strct_free(gpointer ptr)
{
  g_free(ptr);
}

/**
 * ags_hq_pitch_util_compute_s8:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_s8(gint8 *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble base_key,
			     gdouble tuning)
{
  gint8 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint8 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }
  
  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;

  low_mix_buffer = (gint8 *) ags_stream_alloc(low_mix_buffer_length,
					      AGS_SOUNDCARD_SIGNED_8_BIT);
  
  new_mix_buffer = (gint8 *) ags_stream_alloc(buffer_length,
					      AGS_SOUNDCARD_SIGNED_8_BIT);

  ags_linear_interpolate_util_fill_s8(low_mix_buffer,
				      buffer,
				      low_mix_buffer_length,
				      freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_s16:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_s16(gint16 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning)
{
  gint16 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint16 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;

  low_freq_period = samplerate / low_freq;
  
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (gint16 *) ags_stream_alloc(low_mix_buffer_length,
					       AGS_SOUNDCARD_SIGNED_16_BIT);

  new_mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_16_BIT);

  ags_linear_interpolate_util_fill_s16(low_mix_buffer,
				       buffer,
				       low_mix_buffer_length,
				       freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_s24:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_s24(gint32 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning)
{
  gint32 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (gint32 *) ags_stream_alloc(low_mix_buffer_length,
					       AGS_SOUNDCARD_SIGNED_24_BIT);

  new_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_24_BIT);

  ags_linear_interpolate_util_fill_s24(low_mix_buffer,
				       buffer,
				       low_mix_buffer_length,
				       freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_s32:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_s32(gint32 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning)
{
  gint32 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (gint32 *) ags_stream_alloc(low_mix_buffer_length,
					       AGS_SOUNDCARD_SIGNED_32_BIT);

  new_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_32_BIT);
  
  ags_linear_interpolate_util_fill_s32(low_mix_buffer,
				       buffer,
				       low_mix_buffer_length,
				       freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_s64:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_s64(gint64 *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning)
{
  gint64 *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint64 *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (gint64 *) ags_stream_alloc(low_mix_buffer_length,
					       AGS_SOUNDCARD_SIGNED_64_BIT);

  new_mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_64_BIT);

  ags_linear_interpolate_util_fill_s64(low_mix_buffer,
				       buffer,
				       low_mix_buffer_length,
				       freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_float:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_float(gfloat *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble base_key,
				gdouble tuning)
{
  gfloat *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gfloat *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (gfloat *) ags_stream_alloc(low_mix_buffer_length,
					       AGS_SOUNDCARD_FLOAT);

  new_mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_FLOAT);

  ags_linear_interpolate_util_fill_float(low_mix_buffer,
					 buffer,
					 low_mix_buffer_length,
					 freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_double:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_double(gdouble *buffer,
				 guint buffer_length,
				 guint samplerate,
				 gdouble base_key,
				 gdouble tuning)
{
  gdouble *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gdouble *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (gdouble *) ags_stream_alloc(low_mix_buffer_length,
						AGS_SOUNDCARD_DOUBLE);

  new_mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
						AGS_SOUNDCARD_DOUBLE);

  ags_linear_interpolate_util_fill_double(low_mix_buffer,
					  buffer,
					  low_mix_buffer_length,
					  freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    buffer[i] = new_mix_buffer[i];
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_hq_pitch_util_compute_complex:
 * @buffer: the audio buffer
 * @buffer_length: the audio buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.8.0
 */
void
ags_hq_pitch_util_compute_complex(AgsComplex *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble base_key,
				  gdouble tuning)
{
  AgsComplex *ptr_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer, *new_mix_buffer, *low_mix_buffer;

  gdouble volume;
  gdouble base_freq, low_freq, new_freq;
  gdouble offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, low_freq_period, new_freq_period;
  guint low_mix_buffer_length;  
  guint i, j;

  if(buffer == NULL ||
     buffer_length == 0){
    return;
  }

  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;
  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  low_freq = exp2((base_key - 0.25) / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(low_freq <= 0.0){
    g_warning("rejecting pitch low freq %f <= 0.0", low_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  if(tuning == 0.0){
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = buffer;

  low_mix_buffer_length = (freq_period / low_freq_period) * buffer_length;
  
  low_mix_buffer = (AgsComplex *) ags_stream_alloc(low_mix_buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);

  ags_linear_interpolate_util_fill_complex(low_mix_buffer,
					   buffer,
					   low_mix_buffer_length,
					   freq_period / low_freq_period);
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double _Complex new_z;
    gdouble phase, low_phase, new_phase;    
    guint start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    low_phase = fmod(i, low_freq_period);

    new_phase = fmod(i, new_freq_period);

    if(start_x + (guint) floor(new_phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase));
    }else{
      if((start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(new_phase)) - (guint) floor(freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < low_mix_buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < low_mix_buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < low_mix_buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + low_mix_buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ags_complex_get(ptr_mix_buffer) != 0.0 &&
       ags_complex_get(ptr_mix_buffer) != I * 0.0){
      new_z = volume * ((new_freq_period * (ags_complex_get(ptr_mix_buffer) / freq_period) * (ags_complex_get(ptr_low_mix_buffer) / low_freq_period) / (ags_complex_get(ptr_mix_buffer) / freq_period)));
    }else{
      new_z = 0;
    }
    
    ags_complex_set(ptr_new_mix_buffer,
		    new_z);
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ags_complex_set(buffer + i,
		    ags_complex_get(new_mix_buffer + i));
  }

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}
