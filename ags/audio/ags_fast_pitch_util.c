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

#include <ags/audio/ags_fast_pitch_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#include <math.h>
#include <complex.h>

/**
 * SECTION:ags_fast_pitch_util
 * @short_description: fast pitch util
 * @title: AgsFastPitchUtil
 * @section_id:
 * @include: ags/audio/ags_fast_pitch_util.h
 *
 * Utility functions to pitch.
 */

/**
 * ags_fast_pitch_util_compute_s8:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_s8(gint8 *buffer,
			       guint buffer_length,
			       guint samplerate,
			       gdouble base_key,
			       gdouble tuning)
{
  gint8 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint8 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint8 *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gint8 *) ags_stream_alloc(buffer_length,
					  AGS_SOUNDCARD_SIGNED_8_BIT);
    
  im_mix_buffer = (gint8 *) ags_stream_alloc(buffer_length,
					     AGS_SOUNDCARD_SIGNED_8_BIT);
    
  low_mix_buffer = (gint8 *) ags_stream_alloc(buffer_length,
					      AGS_SOUNDCARD_SIGNED_8_BIT);
  
  new_mix_buffer = (gint8 *) ags_stream_alloc(buffer_length,
					      AGS_SOUNDCARD_SIGNED_8_BIT);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint8 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_s16:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_s16(gint16 *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble base_key,
				gdouble tuning)
{
  gint16 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint16 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint16 *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_16_BIT);
    
  im_mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_SIGNED_16_BIT);
    
  low_mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_16_BIT);
  
  new_mix_buffer = (gint16 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_16_BIT);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }
    
    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint16 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_s24:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_s24(gint32 *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble base_key,
				gdouble tuning)
{
  gint32 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint32 *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_24_BIT);
    
  im_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_SIGNED_24_BIT);
    
  low_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_24_BIT);
  
  new_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_24_BIT);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_s32:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_s32(gint32 *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble base_key,
				gdouble tuning)
{
  gint32 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint32 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint32 *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_32_BIT);
    
  im_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_SIGNED_32_BIT);
    
  low_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_32_BIT);
  
  new_mix_buffer = (gint32 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_32_BIT);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint32 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_s64:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_s64(gint64 *buffer,
				guint buffer_length,
				guint samplerate,
				gdouble base_key,
				gdouble tuning)
{
  gint64 *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gint64 *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gint64 *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_SIGNED_64_BIT);
    
  im_mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_SIGNED_64_BIT);
    
  low_mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_64_BIT);
  
  new_mix_buffer = (gint64 *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_SIGNED_64_BIT);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;

    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gint64 new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] = 0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_float:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_float(gfloat *buffer,
				  guint buffer_length,
				  guint samplerate,
				  gdouble base_key,
				  gdouble tuning)
{
  gfloat *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gfloat *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gfloat *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_FLOAT);
    
  im_mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_FLOAT);
    
  low_mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_FLOAT);
  
  new_mix_buffer = (gfloat *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_FLOAT);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0.0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0.0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_double:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_double(gdouble *buffer,
				   guint buffer_length,
				   guint samplerate,
				   gdouble base_key,
				   gdouble tuning)
{
  gdouble *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  gdouble *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  gdouble *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_DOUBLE);
    
  im_mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_DOUBLE);
    
  low_mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_DOUBLE);
  
  new_mix_buffer = (gdouble *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_DOUBLE);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ptr_mix_buffer[0] = buffer[i];
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gfloat z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_im_mix_buffer[0] = im_z;
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = mix_buffer[i];
    mix_z = ptr_mix_buffer[0];

    t = low_freq_period / freq_period;
        
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ptr_low_mix_buffer[0] = low_z;
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer[0] != 0.0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_im_mix_buffer[0] / im_freq_period) / (ptr_mix_buffer[0] / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ptr_mix_buffer[0] / freq_period) * (ptr_low_mix_buffer[0] / low_freq_period) / (ptr_mix_buffer[0] / freq_period)));
    }else{
      new_z = 0.0;
    }
    
    ptr_new_mix_buffer[0] = new_z;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ptr_buffer[0] = ptr_new_mix_buffer[0];
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}

/**
 * ags_fast_pitch_util_compute_complex:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.6.2
 */
void
ags_fast_pitch_util_compute_complex(AgsComplex *buffer,
				    guint buffer_length,
				    guint samplerate,
				    gdouble base_key,
				    gdouble tuning)
{
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_low_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer, *im_mix_buffer, *low_mix_buffer, *new_mix_buffer;
  AgsComplex *ptr_buffer;

  gdouble volume;
  gdouble im_key, low_key;
  gdouble base_freq, im_freq, low_freq, new_freq;
  gdouble offset_factor, im_offset_factor, low_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, low_freq_period, new_freq_period;
  gdouble t;
  guint i, j;
  
  /* frequency */
  base_freq = exp2((base_key) / 12.0) * 440.0;

  im_key = (gdouble) ((gint) floor(tuning / 100.0) % 12);
  
  if(im_key < 0.0){
    im_key += 12.0;
  }

  if(im_key == 0.0){
    im_key = 1.0;
  }
  
  im_freq = exp2((base_key + im_key) / 12.0) * 440.0;

  low_key = base_key - 12.0;
  
  low_freq = exp2((low_key) / 12.0) * 440.0;

  new_freq = exp2((base_key + (tuning / 100.0))  / 12.0) * 440.0;

  if(base_freq <= 0.0){
    g_warning("rejecting pitch base freq %f <= 0.0", base_freq);
    
    return;
  }

  if(im_freq <= 0.0){
    g_warning("rejecting pitch intermediate freq %f <= 0.0", im_freq);
    
    return;
  }

  if(new_freq <= 0.0){
    g_warning("rejecting pitch new freq %f <= 0.0", new_freq);
    
    return;
  }

  volume = 1.0 / base_freq * new_freq;
  
  /* get frequency period */
  freq_period = 2.0 * M_PI * samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  low_freq_period = samplerate / low_freq;
  new_freq_period = samplerate / new_freq;
  
  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  low_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / low_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  low_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);

  /* mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;

    /* write mix buffer */
    ags_complex_set(ptr_mix_buffer, ags_complex_get(buffer + i));
  }

  /* im mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    gdouble z, mix_z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    if(j >= im_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    im_phase = fmod(i, im_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write im mix buffer */
    z = ags_complex_get(mix_buffer + i);
    mix_z = ags_complex_get(ptr_mix_buffer);

    t = ((j / im_offset_factor) - j) / (freq_period - im_freq_period);
    
    im_z = (1.0 - t) * z + (t * mix_z);
    
    ags_complex_set(ptr_im_mix_buffer, im_z);
  }

  /* low mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double complex z, mix_z, low_z;
    gdouble phase, low_phase;
    guint start_x;

    if(j >= low_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }
    
    low_phase = fmod(i, low_freq_period);

    phase = fmod(i, freq_period);

    if(start_x + (guint) floor(phase) < buffer_length){
      ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase));
    }else{
      if((start_x + (guint) floor(phase)) - (guint) floor(freq_period) < buffer_length &&
	 (start_x + (guint) floor(phase)) - (guint) floor(freq_period) > 0){
	ptr_mix_buffer = mix_buffer + (start_x + (guint) floor(phase)) - (guint) floor(freq_period);
      }else{
	if(floor(phase) < buffer_length){
	  ptr_mix_buffer = mix_buffer + (guint) floor(phase);
	}else{
	  ptr_mix_buffer = mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_low_mix_buffer = low_mix_buffer + i;

    /* write low mix buffer */
    z = ags_complex_get(mix_buffer + i);
    mix_z = ags_complex_get(ptr_mix_buffer);

    t = low_freq_period / freq_period;
    
    low_z = (1.0 - t) * z + (t * mix_z);
    
    ags_complex_set(ptr_low_mix_buffer, low_z);
  }
  
  /* new mix buffer */
  for(i = 0, j = 0; i < buffer_length; i++, j++){
    double complex new_z;
    gdouble phase, im_phase, low_phase, new_phase;    
    guint start_x, im_start_x, low_start_x;

    if(j >= new_freq_period){
      j = 0;
    }

    if(floor(freq_period) != 0.0){
      start_x = freq_period * floor((double) i / freq_period);
    }else{
      start_x = 0;
    }

    if(floor(im_freq_period) != 0.0){
      im_start_x = im_freq_period * floor((double) i / im_freq_period);
    }else{
      im_start_x = 0;
    }

    if(floor(low_freq_period) != 0.0){
      low_start_x = low_freq_period * floor((double) i / low_freq_period);
    }else{
      low_start_x = 0;
    }

    phase = fmod(i, freq_period);

    im_phase = fmod(i, im_freq_period);

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

    if(im_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase));
    }else{
      if((im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(new_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }

    if(low_start_x + (guint) floor(new_phase) < buffer_length){
      ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase));
    }else{
      if((low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) < buffer_length &&
	 (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period) > 0){
	ptr_low_mix_buffer = low_mix_buffer + (low_start_x + (guint) floor(new_phase)) - (guint) floor(low_freq_period);
      }else{
	if(floor(new_phase) < buffer_length){
	  ptr_low_mix_buffer = low_mix_buffer + (guint) floor(new_phase);
	}else{
	  ptr_low_mix_buffer = low_mix_buffer + buffer_length - 1;
	}
      }
    }
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    if(ptr_mix_buffer->real != 0.0){
      new_z = volume * ((1.0 / 2.0) * (new_freq_period * (ags_complex_get(ptr_mix_buffer) / freq_period) * (ags_complex_get(ptr_im_mix_buffer) / im_freq_period) / (ags_complex_get(ptr_mix_buffer) / freq_period)) + (1.0 / 2.0) * (new_freq_period * (ags_complex_get(ptr_mix_buffer) / freq_period) * (ags_complex_get(ptr_low_mix_buffer) / low_freq_period) / (ags_complex_get(ptr_mix_buffer) / freq_period)));
    }else{
      new_z = 0.0 + I * 0.0;
    }
    
    ags_complex_set(ptr_new_mix_buffer,
		    new_z);
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ags_complex_set(ptr_buffer, ags_complex_get(ptr_new_mix_buffer));
  }
  
  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(low_mix_buffer);

  ags_stream_free(new_mix_buffer);
}
