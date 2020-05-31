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

#include <ags/audio/ags_filter_util.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>
#include <ags/audio/ags_fourier_transform_util.h>

#include <math.h>

/**
 * SECTION:ags_filter_util
 * @short_description: filter util
 * @title: AgsFilterUtil
 * @section_id:
 * @include: ags/audio/ags_filter_util.h
 *
 * Utility functions to compute filters.
 */

/**
 * ags_filter_util_pitch_s8:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_s8(gint8 *buffer,
			 guint buffer_length,
			 guint samplerate,
			 gdouble base_key,
			 gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gint8 **ptr_ptr_buffer;
  gint8 *ptr_buffer;
  gint8 *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;
    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint8 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
}

/**
 * ags_filter_util_pitch_s16:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_s16(gint16 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gint16 **ptr_ptr_buffer;
  gint16 *ptr_buffer;
  gint16 *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);



  /* im mix buffer */
  for(i = 0; i < buffer_length; i++){
    complex z, im_z;
    gdouble phase, im_phase;
    guint start_x;

    start_x = (i % (guint) floor(freq_period)) + freq_period * floor((double) i / freq_period);

    im_phase = i % (guint) floor(im_freq / 4.0);

    phase = ((base_freq / im_freq) + 1.0) * im_phase;

#if 1
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
#else
    ptr_mix_buffer = mix_buffer + i;
#endif
    
    ptr_im_mix_buffer = im_mix_buffer + i;

    /* write mix buffer */
    ptr_ptr_mix_buffer = &ptr_mix_buffer;
    
    AGS_AUDIO_BUFFER_UTIL_S16_TO_COMPLEX(buffer[i], ptr_ptr_mix_buffer);

    /* write im mix buffer */
    z = ags_complex_get(ptr_mix_buffer);

    im_z = z;

    ags_complex_set(ptr_im_mix_buffer, im_z);
  }

  /* new mix buffer */
  for(i = 0; i < buffer_length; i++){
    complex new_z;
    gdouble phase, im_phase, new_phase;    
    guint start_x, im_start_x;

    start_x = (i % (guint) floor(freq_period)) + freq_period * floor((double) i / freq_period);
    im_start_x = (i % (guint) floor(im_freq_period)) + im_freq_period * floor((double) i / im_freq_period);

    phase = i % (guint) floor(base_freq / 4.0);
    im_phase = i % (guint) floor(im_freq / 4.0);
    new_phase = i % (guint) floor(new_freq / 4.0);

#if 1
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

    if(im_start_x + (guint) floor(im_phase) < buffer_length){
      ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(im_phase));
    }else{
      if((im_start_x + (guint) floor(im_phase)) - (guint) floor(im_freq_period) < buffer_length &&
	 (im_start_x + (guint) floor(im_phase)) - (guint) floor(im_freq_period) > 0){
	ptr_im_mix_buffer = im_mix_buffer + (im_start_x + (guint) floor(im_phase)) - (guint) floor(im_freq_period);
      }else{
	if(floor(im_phase) < buffer_length){
	  ptr_im_mix_buffer = im_mix_buffer + (guint) floor(im_phase);
	}else{
	  ptr_im_mix_buffer = im_mix_buffer + buffer_length - 1;
	}
      }
    }
#else
    ptr_mix_buffer = mix_buffer + i;
    ptr_im_mix_buffer = im_mix_buffer + i;
#endif
    
    ptr_new_mix_buffer = new_mix_buffer + i;

    /* write new mix buffer */
    t = 0.113; // ((gdouble) ((guint) floor(base_key) % 12) / 4.0);
    
    new_z = (1.0 - t) * ags_complex_get(ptr_im_mix_buffer) + (t * ags_complex_get(ptr_mix_buffer));

    ags_complex_set(ptr_new_mix_buffer,
		    new_z);
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S16(ptr_new_mix_buffer, ptr_buffer);
  }

#if 0
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;

#if 0    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
#else
    AGS_AUDIO_BUFFER_UTIL_S16_TO_COMPLEX(offset[0], ptr_ptr_mix_buffer);
#endif
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;
  tail = 0;
  
  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	if(i + k >= buffer_length){
	  tail = (i + k + 2) - buffer_length;
	}
	
	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1 - tail));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k - tail));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  tail = 0;
  
  while(i < buffer_length && (guint) new_freq_period > 0){
    if(new_freq_period > freq_period){
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  if((guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)) >= buffer_length){
	    tail = (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) + 2) - buffer_length;
	  }
	  
	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  if(i + k >= buffer_length){
	    tail = (i + k + 2) - buffer_length;
	  }
	  
	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1 - tail));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k - tail));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

#if 0
    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
#else
    AGS_AUDIO_BUFFER_UTIL_COMPLEX_TO_S16(ptr_new_mix_buffer, ptr_buffer);
#endif
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint16 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
#endif
}

/**
 * ags_filter_util_pitch_s24:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_s24(gint32 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gint32 **ptr_ptr_buffer;
  gint32 *ptr_buffer;
  gint32 *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;
    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
}

/**
 * ags_filter_util_pitch_s32:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_s32(gint32 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gint32 **ptr_ptr_buffer;
  gint32 *ptr_buffer;
  gint32 *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;
    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint32 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
}

/**
 * ags_filter_util_pitch_s64:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_s64(gint64 *buffer,
			  guint buffer_length,
			  guint samplerate,
			  gdouble base_key,
			  gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gint64 **ptr_ptr_buffer;
  gint64 *ptr_buffer;
  gint64 *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;
    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gint64 sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
}

/**
 * ags_filter_util_pitch_float:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_float(gfloat *buffer,
			    guint buffer_length,
			    guint samplerate,
			    gdouble base_key,
			    gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gfloat **ptr_ptr_buffer;
  gfloat *ptr_buffer;
  gfloat *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;
    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gfloat sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
}

/**
 * ags_filter_util_pitch_double:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_double(gdouble *buffer,
			     guint buffer_length,
			     guint samplerate,
			     gdouble base_key,
			     gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gdouble **ptr_ptr_buffer;
  gdouble *ptr_buffer;
  gdouble *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;
    
    AGS_FOURIER_TRANSFORM_UTIL_COMPUTE_STFT_S16_FRAME(offset, 1,
						      n,
						      buffer_length,
						      ptr_ptr_mix_buffer);
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    AGS_FOURIER_TRANSFORM_UTIL_INVERSE_STFT_S16_FRAME(ptr_new_mix_buffer, 1,
						      n,
						      buffer_length,
						      ptr_ptr_buffer);
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;

      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = buffer[i - 1];
      sum1 = buffer[i];

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      gdouble sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = buffer[i - 1];
      sum1 = buffer[i];
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      buffer[i] = sum;
    }    
  }
}

/**
 * ags_filter_util_pitch_complex:
 * @buffer: the audio buffer
 * @buffer_length: the buffer's length
 * @samplerate: the samplerate
 * @base_key: the base key
 * @tuning: the tuning
 * 
 * Apply pitch filter.
 * 
 * Since: 3.0.0
 */
void
ags_filter_util_pitch_complex(AgsComplex *buffer,
			      guint buffer_length,
			      guint samplerate,
			      gdouble base_key,
			      gdouble tuning)
{
  AgsComplex **ptr_ptr_mix_buffer, **ptr_ptr_im_mix_buffer, **ptr_ptr_new_mix_buffer;
  AgsComplex *ptr_mix_buffer, *ptr_im_mix_buffer, *ptr_new_mix_buffer;
  AgsComplex *mix_buffer;
  AgsComplex *im_mix_buffer, *new_mix_buffer;
  gdouble **ptr_ptr_buffer;
  gdouble *ptr_buffer;
  gdouble *offset;
  
  gdouble base_freq, im_freq, new_freq;
  gdouble offset_factor, im_offset_factor, new_offset_factor;
  gdouble freq_period, im_freq_period, new_freq_period;
  gdouble t;
  complex sum, sum0, sum1;
  gdouble n;
  guint i, j, k;
  guint tail;
  
  if(tuning == 0.0){
    return;
  }
    
  /* frequency */
  base_freq = 27.5 * exp2((base_key + 48.0) / 12.0);

  im_freq = base_freq + 1.0;
  new_freq = 27.5 * exp2(((base_key + 48.0) + (tuning / 100.0)) / 12.0);

  /* get frequency period */
  freq_period = samplerate / base_freq;
  
  im_freq_period = samplerate / im_freq;
  new_freq_period = samplerate / new_freq;

  /* get offset factor */
  offset_factor = 1.0;
  
  im_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / im_freq);
  new_offset_factor = 1.0 / (samplerate / base_freq) * (samplerate / new_freq);  

  /* allocate buffer */
  mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
					       AGS_SOUNDCARD_COMPLEX);
    
  im_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						  AGS_SOUNDCARD_COMPLEX);
    
  new_mix_buffer = (AgsComplex *) ags_stream_alloc(buffer_length,
						   AGS_SOUNDCARD_COMPLEX);
  
  ptr_ptr_buffer = &ptr_buffer;

  ptr_ptr_mix_buffer = &ptr_mix_buffer;

  ptr_ptr_im_mix_buffer = &ptr_im_mix_buffer;
  ptr_ptr_new_mix_buffer = &ptr_new_mix_buffer;

  /* compute mix buffer */
  for(i = 0; i < buffer_length; i++){
    ptr_mix_buffer = mix_buffer + i;
    
    offset = buffer + i;
    n = (gdouble) i;

    ags_complex_set(ptr_mix_buffer,
		    ags_complex_get(offset));
  }

  /* compute intermediate mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(im_mix_buffer,
		  sum);

  i = 1;

  while(i < buffer_length){
    for(j = 0; j < freq_period && i + j < buffer_length;){
      for(k = 1; k < im_freq_period && i + j < buffer_length; j++, k++){
	t = 1.0 / exp((gdouble) k * 2.0 * M_PI * im_freq_period / freq_period);

	sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	sum1 = ags_complex_get(mix_buffer + (guint) (i + k));
	  
	sum = (1.0 - t) * sum0 + (t * sum1);
      
	ags_complex_set(im_mix_buffer + (guint) (i + j),
			sum);
      }
    }

    i += j;
  }
  
  /* compute new mix buffer */
  sum = ags_complex_get(mix_buffer);

  ags_complex_set(new_mix_buffer,
		  sum);

  i = 1;
  
  while(i < buffer_length){
    if(new_freq_period > freq_period){	
      for(j = 0; j < new_freq_period && i + j < buffer_length;){
	sum0 = ags_complex_get(mix_buffer);

	ags_complex_set(new_mix_buffer + (guint) i + j,
			sum);
	
	for(k = 1; k < freq_period && i + j < buffer_length; j++, k++){
	  t = (freq_period / new_freq_period) * 1.0 / exp2(j * 2.0 * M_PI * new_freq / base_freq);

	  sum0 = ags_complex_get(mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor) - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (floor(i / new_freq_period) * new_freq_period + (k / new_offset_factor)));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }else{
      for(j = 0; j < freq_period && i + j < buffer_length;){
	for(k = 1; k < new_freq_period && i + j < buffer_length; j++, k++){
	  t = 1.0 / exp((gdouble) k * 2.0 * M_PI * new_freq_period / freq_period);

	  sum0 = ags_complex_get(mix_buffer + (guint) (i + k - 1));
	  sum1 = ags_complex_get(im_mix_buffer + (guint) (i + k));
	  
	  sum = (1.0 - t) * sum0 + (t * sum1);
      
	  ags_complex_set(new_mix_buffer + (guint) (i + j),
			  sum);
	}
      }
    }

    i += j;
  }
  
  /* rewrite buffer */
  for(i = 0; i < buffer_length; i++){
    n = (gdouble) (i % (guint) new_freq_period);

    ptr_new_mix_buffer = new_mix_buffer + i;
    ptr_buffer = buffer + i;

    ags_complex_set(ptr_buffer,
		    ags_complex_get(ptr_new_mix_buffer));
  }

  ags_stream_free(mix_buffer);

  ags_stream_free(im_mix_buffer);

  ags_stream_free(new_mix_buffer);

  /* interpolation */
  if(freq_period < new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = 1.0 / exp(freq_period / new_freq_period);

      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);
    
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(buffer + i,
		      sum);
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / exp((i * 2.0 * M_PI * new_freq / samplerate) / (new_freq_period - freq_period)))) + 1.0;
	   
      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);
	   
      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(mix_buffer + i,
		      sum);
    }    

    for(i = 1; i < buffer_length; i++){
      ags_complex_set(buffer + i,
		      ags_complex_get(mix_buffer + i));
    }
  }
  
  if(freq_period > new_freq_period){
    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = 1.0 / (freq_period / new_freq_period);

      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(buffer + i,
		      sum);
    }

    for(i = 1; i < buffer_length; i++){
      gdouble t;
      complex sum, sum0, sum1;
    
      t = (-1.0 * (1.0 / (freq_period / new_freq_period * i))) + 1.0;
	   
      sum0 = ags_complex_get(buffer + i);
      sum1 = ags_complex_get(buffer + i - 1);

      sum = (1.0 - t) * sum0 + (t * sum1);
      
      ags_complex_set(mix_buffer + i,
		      sum);
    }    

    for(i = 1; i < buffer_length; i++){
      ags_complex_set(buffer + i,
		      ags_complex_get(mix_buffer + i));
    }
  }
}
